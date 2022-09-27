#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include "../utils/Lane.hpp"
#include "../entity/Vehicle.hpp"
#include "../entity/Result.hpp"
#include "../utils/Log.hpp"
#include "../utils/Progressor.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/ml.hpp>


class TrafficEstimator{
public:
	void estimate(const std::vector<Centroid>& vehicles,
						 const Lane& lines, const cv::Size& image_size, Result& prediction);
	static void trainSVM(const std::string& path);

private:
	static float densityEstimator(const std::vector<Centroid>& vehicles,
								  	const Lane& lines, const cv::Size& image_size);	
	
};
