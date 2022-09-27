#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <map>
#include <fstream>
#include <filesystem>

#include "../utils/Log.hpp"
#include "../utils/Timer.hpp"
#include "../utils/Lane.hpp"
#include "../utils/FileUtils.hpp"
#include "../utils/Progressor.hpp"
#include "../entity/Result.hpp"
#include "../entity/Parameters.hpp"
#include "../detector/YoloDetector.hpp"
#include "../detector/MobilenetDetector.hpp"
#include "../tracker/VehicleTracker.hpp"
#include "../estimator/TrafficEstimator.hpp"

void evaluate(Parameters& param, const int image_width, const int image_height);
