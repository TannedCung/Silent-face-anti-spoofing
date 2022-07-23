#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "utils.h"
#include "opencv2/opencv.hpp"
#include "nlohmann/json.hpp" 

// #include <cryptopp/secblock.h>

using json = nlohmann::json;
using namespace cv;

// template <typename T>
// std::vector<T> vector_add(std::vector<T>v1, std::vector<T>v2){
//   std::vector<T> sum;
//   std::transform(v1.begin(), v1.end(), v2.begin(), std::back_inserter(sum), std::plus<T>()); 
//   return sum;
// }

NvDsInferObjectDetectionInfo rec2square(NvDsInferObjectDetectionInfo & rec){
  NvDsInferObjectDetectionInfo square;
  auto S = rec.width*rec.height;
  float center_x = rec.left + rec.width/2;
  float center_y = rec.top + rec.height/2;
  square.width = square.height = sqrt(S);
  square.top = center_y-square.height/2;
  square.left = center_x-square.width/2;
  return square;
}

NvDsInferObjectDetectionInfo crop_box_by_scale(int img_w, int img_h, float scale, NvDsInferObjectDetectionInfo box) {
  box = rec2square(box);
  scale = std::min({float((img_h-1)/box.height), float((img_w-1)/box.width), scale});
  std::cout << "[INFO]: scale: " << scale << " - float((img_h-1)/box.height): " << float((img_h-1)/box.height) << " - float((img_w-1)/box.width): " << float((img_w-1)/box.width) <<  std::endl;
  int new_width = int(box.width*scale);
  int new_height = int(box.height*scale);
  cv::Point center (int(box.left + box.width/2), int(box.top + box.height/2));
  int new_x = int(center.x - new_width/2);
  int new_y = int(center.y - new_height/2);
  // make sure cropped image is face centered
  if (new_x<0) {
    new_width += 2*new_x;
    new_x = 0;
  }
  if (new_y<0) {
    new_height += 2*new_y;
    new_y = 0;
  }
  if (new_x+new_width > img_w-1) {
    int redundant = new_x+new_width-img_w+1;
    new_x += redundant;
    new_width -= 2*redundant;
  }
  if (new_y+new_height > img_h-1) {
    int redundant = new_y+new_height-img_h+1;
    new_y += redundant;
    new_height -= 2*redundant;
  }
  NvDsInferObjectDetectionInfo scaled_box;
  scaled_box.left = new_x;
  scaled_box.top = new_y;
  scaled_box.width = new_width;
  scaled_box.height = new_height;
  return scaled_box;
}

static gchar *
get_absolute_file_path (gchar *cfg_file_path, gchar *file_path)
{
  gchar abs_cfg_path[PATH_MAX + 1];
  gchar *abs_file_path;
  gchar *delim;

  if (file_path && file_path[0] == '/') {
    return file_path;
  }

  if (!realpath (cfg_file_path, abs_cfg_path)) {
    g_free (file_path);
    return NULL;
  }

  // Return absolute path of config file if file_path is NULL.
  if (!file_path) {
    abs_file_path = g_strdup (abs_cfg_path);
    return abs_file_path;
  }

  delim = g_strrstr (abs_cfg_path, "/");
  *(delim + 1) = '\0';

  abs_file_path = g_strconcat (abs_cfg_path, file_path, NULL);
  g_free (file_path);

  return abs_file_path;
}

gboolean set_tracker_properties (GstElement *nvtracker, gchar *config)
{
  gboolean ret = FALSE;
  GError *error = NULL;
  gchar **keys = NULL;
  gchar **key = NULL;
  GKeyFile *key_file = g_key_file_new ();

  if (!g_key_file_load_from_file (key_file, config, G_KEY_FILE_NONE,
          &error)) {
    g_printerr ("Failed to load config file: %s\n", error->message);
    return FALSE;
  }

  keys = g_key_file_get_keys (key_file, CONFIG_GROUP_TRACKER, NULL, &error);
  CHECK_ERROR (error);

  for (key = keys; *key; key++) {
    if (!g_strcmp0 (*key, CONFIG_GROUP_TRACKER_WIDTH)) {
      gint width =
          g_key_file_get_integer (key_file, CONFIG_GROUP_TRACKER,
          CONFIG_GROUP_TRACKER_WIDTH, &error);
      CHECK_ERROR (error);
      g_object_set (G_OBJECT (nvtracker), "tracker-width", width, NULL);
    } else if (!g_strcmp0 (*key, CONFIG_GROUP_TRACKER_HEIGHT)) {
      gint height =
          g_key_file_get_integer (key_file, CONFIG_GROUP_TRACKER,
          CONFIG_GROUP_TRACKER_HEIGHT, &error);
      CHECK_ERROR (error);
      g_object_set (G_OBJECT (nvtracker), "tracker-height", height, NULL);
    } else if (!g_strcmp0 (*key, CONFIG_GPU_ID)) {
      guint gpu_id =
          g_key_file_get_integer (key_file, CONFIG_GROUP_TRACKER,
          CONFIG_GPU_ID, &error);
      CHECK_ERROR (error);
      g_object_set (G_OBJECT (nvtracker), "gpu_id", gpu_id, NULL);
    } else if (!g_strcmp0 (*key, CONFIG_GROUP_TRACKER_LL_CONFIG_FILE)) {
      char* ll_config_file = get_absolute_file_path (config,
                g_key_file_get_string (key_file,
                    CONFIG_GROUP_TRACKER,
                    CONFIG_GROUP_TRACKER_LL_CONFIG_FILE, &error));
      CHECK_ERROR (error);
      g_object_set (G_OBJECT (nvtracker), "ll-config-file", ll_config_file, NULL);
    } else if (!g_strcmp0 (*key, CONFIG_GROUP_TRACKER_LL_LIB_FILE)) {
      char* ll_lib_file = get_absolute_file_path (config,
                g_key_file_get_string (key_file,
                    CONFIG_GROUP_TRACKER,
                    CONFIG_GROUP_TRACKER_LL_LIB_FILE, &error));
      CHECK_ERROR (error);
      g_object_set (G_OBJECT (nvtracker), "ll-lib-file", ll_lib_file, NULL);
    } else if (!g_strcmp0 (*key, CONFIG_GROUP_TRACKER_ENABLE_BATCH_PROCESS)) {
      gboolean enable_batch_process =
          g_key_file_get_integer (key_file, CONFIG_GROUP_TRACKER,
          CONFIG_GROUP_TRACKER_ENABLE_BATCH_PROCESS, &error);
      CHECK_ERROR (error);
      g_object_set (G_OBJECT (nvtracker), "enable_batch_process",
                    enable_batch_process, NULL);
    } else {
      g_printerr ("Unknown key '%s' for group [%s]", *key,
          CONFIG_GROUP_TRACKER);
    }
  }

  ret = TRUE;
done:
  if (error) {
    g_error_free (error);
  }
  if (keys) {
    g_strfreev (keys);
  }
  if (!ret) {
    g_printerr ("%s failed", __func__);
  }
  return ret;
}

void generate_ts_rfc3339 (char *buf, int buf_size)
{
  time_t tloc;
  struct tm tm_log;
  struct timespec ts;
  char strmsec[6]; //.nnnZ\0

  clock_gettime(CLOCK_REALTIME,  &ts);
  memcpy(&tloc, (void *)(&ts.tv_sec), sizeof(time_t));
  gmtime_r(&tloc, &tm_log);
  strftime(buf, buf_size,"%Y-%m-%dT%H:%M:%S", &tm_log);
  int ms = ts.tv_nsec/1000000;
  g_snprintf(strmsec, sizeof(strmsec),".%.3dZ", ms);
  strncat(buf, strmsec, buf_size);
}

// std::string mat_encode(cv::Mat &img) {
//     std::vector<uchar> buf;
//     cv::imencode(".jpg", img, buf);
//     auto *enc_msg = reinterpret_cast<unsigned char*>(buf.data());
//     std::string encoded = base64_encode(enc_msg, buf.size());
//     return encoded;
// }

double string2double(std::string s)
{
	std::stringstream ss(s);
	double d;
	ss >> d;
	return d;
}



static std::string double2string(double d)
{
	std::stringstream ss;
	ss << d;
	return ss.str();
}

void expand_rect(cv::Rect &rect, int width, int height) {
  float d = std::min(rect.width, rect.height) * 1.9;
  float cX = rect.x + rect.width / 2;
  float cY = rect.y + rect.height / 2;
  rect.x = std::max(int(cX - d/2), 0);
  rect.y = std::max(int(cY - d/2), 0);
  rect.width = std::min(int(width - rect.x), int(d));
  rect.height = std::min(int(height - rect.y), int(d));
}

void ExpandSquareBox(cv::Rect & roi, int img_width, int img_height, float ratio=1.2) {
	int diff = (roi.width - roi.height)*ratio;
	int delta = int(abs(diff) / (2*ratio));

  cv::Rect _roi = roi;

  if (diff < 0){ 
		roi.width = roi.width*ratio + abs(diff);
    roi.height *= ratio;
	}
	else {
		roi.height = roi.height*ratio + abs(diff);
    roi.width *= ratio;
	}
  roi.x -= (roi.width- _roi.width)/2;
  roi.y -= (roi.height- _roi.height)/2;

  if (roi.x<0 or roi.y<0 or (roi.x+roi.width)>img_width or (roi.y+roi.height)>img_height){
    roi = _roi;
  }
  // else {
  //   roi.x -= delta;
  //   roi.y -= delta;
	//   roi.width += abs(diff);
	//   roi.height += abs(diff);
  // }
}