#include <gst/gst.h>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <string>
#include <algorithm> // for transform
#include <functional> // for plus

#include "opencv2/opencv.hpp"
#include <opencv2/imgcodecs.hpp>
#include "nlohmann/json.hpp" 
#include "nvdsinfer.h"

/* Tracker config parsing */

#define CHECK_ERROR(error) \
    if (error) { \
        g_printerr ("Error while parsing config file: %s\n", error->message); \
        goto done; \
    }

static gchar * get_absolute_file_path (gchar *cfg_file_path, gchar *file_path);

#define CONFIG_GROUP_TRACKER "tracker"
#define CONFIG_GROUP_TRACKER_WIDTH "tracker-width"
#define CONFIG_GROUP_TRACKER_HEIGHT "tracker-height"
#define CONFIG_GROUP_TRACKER_LL_CONFIG_FILE "ll-config-file"
#define CONFIG_GROUP_TRACKER_LL_LIB_FILE "ll-lib-file"
#define CONFIG_GROUP_TRACKER_ENABLE_BATCH_PROCESS "enable-batch-process"
#define CONFIG_GPU_ID "gpu-id"

gboolean set_tracker_properties (GstElement *nvtracker, gchar *config);

void generate_ts_rfc3339 (char *buf, int buf_size);

// std::string mat_encode(cv::Mat &img);
// template <typename T>
// std::vector<T> vector_add(std::vector<T>v1, std::vector<T>v2);

template <typename T>
std::vector<T> vector_add(std::vector<T>v1, std::vector<T>v2){
  std::vector<T> sum;
  std::transform(v1.begin(), v1.end(), v2.begin(), std::back_inserter(sum), std::plus<T>()); 
  return sum;
}

NvDsInferObjectDetectionInfo rec2square(NvDsInferObjectDetectionInfo & rec);

NvDsInferObjectDetectionInfo crop_box_by_scale(int img_w, int img_h, float scale, NvDsInferObjectDetectionInfo box);

void expand_rect(cv::Rect &rect, int width, int height);

void ExpandSquareBox(cv::Rect & roi, int img_width, int img_height, float ratio);