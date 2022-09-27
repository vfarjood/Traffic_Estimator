#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <iomanip>

#include "Timer.hpp"
#include "Lane.hpp"
#include "../entity/Vehicle.hpp"
#include "../entity/Result.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>


class FileUtils {
public:
   static void save(const std::stringstream& os, const std::string& path);
   static void save(const std::string& sample_path, std::vector<Centroid>& tracked_vehicles, const Result& result);
   static void printMatrix(std::vector<std::pair<int, int>>& mismatch,
                            std::map<std::string, int>& labels,
                               std::map<std::string, int>& predictions,
                                 std::stringstream& output_stream);
   static void printMismatchTable(const int total_detection_predict, const int total_tracking_predict, const int estimation_mismatch_size,
                                    const int total_detection_size, const int total_tracking_size, const int total_estimation_size,
                                    std::stringstream& output_stream);
   static void drawResultOnImage(std::vector<std::string>& input_files, Lane& lines,
                                    std::vector<std::vector<Centroid>>& vector_of_centeroids, 
                                       const std::string images[],
                                          const float INPUT_WIDTH, const float INPUT_HEIGHT);
private:
   static void drawGrid(cv::Mat &image, Lane& lines_vector);
};