#pragma once
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "../entity/Vehicle.hpp"

class Lane {
public:
    std::vector<std::pair<cv::Point, cv::Point>> line;

public:
    Lane(const std::string& filename);
    void draw(cv::Mat const& image);
    void findLineNumber(std::vector<Centroid> &vehicles, const cv::Size& image_size);
    std::vector<cv::Point> findCoverage(const cv::Size& image_size) const;
    
private:
    static bool checkTheLine(const cv::Point& center,const std::pair<cv::Point, cv::Point>& left_line, 
                                    const std::pair<cv::Point, cv::Point>& right_line, const cv::Size& image_size);
    
    
};