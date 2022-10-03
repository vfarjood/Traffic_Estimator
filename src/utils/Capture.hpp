#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>

void capture(){
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT,640);
    if(!cap.isOpened()){
     std::cout << "Failed to connect to the camera." << std::endl;
    }
    std::string path{"../media/capture/"};
  for(int i=1; i <=2; i++){
    sleep(1);
    cv::Mat frame;
    cap >> frame;
    if(frame.empty()){
      std::cout << "Failed to capture an image" << std::endl;
      exit(0);
    }
    cv::imwrite((path + "img" + std::to_string(i) + ".jpg"), frame);
  }
}