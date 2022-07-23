// g++ box_by_scale.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -o test
#include <iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/imgcodecs.hpp>
#include "nvdsinfer.h"
#include <math.h>

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

  std::cout << "[INFO]: squared box: [" << box.left << ", " << box.top << ", " << box.width << ", " << box.height << "]" << std::endl;

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

// s
NvDsInferObjectDetectionInfo crop_box_by_scale2(int img_w, int img_h, float scale, NvDsInferObjectDetectionInfo box) {
  box = rec2square(box);
  scale = std::min({float((img_h-1)/box.height), float((img_w-1)/box.width), scale});

  std::cout << "[INFO]: squared box: [" << box.left << ", " << box.top << ", " << box.width << ", " << box.height << "]" << std::endl;

  std::cout << "[INFO]: scale: " << scale << " - float((img_h-1)/box.height): " << float((img_h-1)/box.height) << " - float((img_w-1)/box.width): " << float((img_w-1)/box.width) <<  std::endl;
  float new_width = box.width*scale;
  float new_height = box.height*scale;
  cv::Point center (int(box.left + box.width/2), int(box.top + box.height/2));

  if (center.x+new_width/2-1 > img_w)  new_width -= 2*(img_w-center.x-new_width/2+1);
  if (center.y+new_height/2-1 > img_h)  new_height -= 2*(img_h-center.y-new_height/2+1);
  if (center.x-new_width/2-1 < 0)  new_width += 2*(center.x-new_width/2-1);
  if (center.y-new_height/2-1 < 0)  new_height += 2*(center.y-new_height/2-1);

  NvDsInferObjectDetectionInfo scaled_box;
  scaled_box.left = center.x-new_width/2;
  scaled_box.top = center.y-new_height/2;
  scaled_box.width = center.x+new_width/2;
  scaled_box.height = center.y+new_height/2;
  return scaled_box;
}

int main(){
  // cv::Mat img = cv::Mat::zeros(cv::Size(640,480),CV_8UC4);
  cv::Mat img = cv::imread("/workspace/cxview-face-edge/plugins/deepstream/test/input.png");
  NvDsInferObjectDetectionInfo _box;
  _box.left = 480;
  _box.top = 165;
  _box.width = 95;
  _box.height = 145;

  NvDsInferObjectDetectionInfo _new_box = crop_box_by_scale(img.cols,img.rows, 10, _box);
  cv::Rect box(_box.left, _box.top, _box.width, _box.height);
  cv::Rect new_box(_new_box.left, _new_box.top, _new_box.width, _new_box.height);

  cv::rectangle(img, box, cv::Scalar(254, 255, 0));
  cv::rectangle(img, new_box, cv::Scalar(0, 255, 0));
  std::cout << "[INFO]: RECT: " << new_box << std::endl;
  cv::imwrite("scaled.jpg", img);
  cv::imshow("scaled", img);
  cv::waitKey(0);
}