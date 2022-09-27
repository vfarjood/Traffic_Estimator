#pragma once
#include <vector>
#include <string>
#include <opencv2/core/types.hpp>

struct Centroid {
    int id;
    float conf;
    float area;
    cv::Point center;
    cv::Rect box;
    std::vector<cv::Rect> box_history;
    std::vector<cv::Point> position_history;
    cv::Point next_position;

    std::string name;
    int lane_num = 0;
    float distance{0.0};
    float speed{0.0};
};