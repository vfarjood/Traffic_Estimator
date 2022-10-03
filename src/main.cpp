//***************************************************************************//
//                                                                           //
// @File Name:     main.cpp                                                  //
// @Author:        Vahid Farjood Chafi                                       //
// @Version:       0.0.1                                                     //
// @Date:          13th September 2022                                       //
// @Email:         vahid.farjood@gmail.com                                   //
// @Description:   Traffic Estimator main file.                              //
//                                                                           //
//***************************************************************************//

#include <config.h>
#include <iostream>
#include <vector>
#include <map>
#include "utils/Log.hpp"
#include "utils/Timer.hpp"
#include "utils/Lane.hpp"
#include "utils/Capture.hpp"
#include "utils/FileUtils.hpp"
#include "entity/Result.hpp"
#include "entity/Parameters.hpp"
#include "detector/YoloDetector.hpp"
#include "detector/MobilenetDetector.hpp"
#include "tracker/VehicleTracker.hpp"
#include "estimator/TrafficEstimator.hpp"
#include "utils/CmdLineParser.hpp"
#include "evaluate/Evaluate.hpp"

constexpr int INPUT_WIDTH = 640;
constexpr int INPUT_HEIGHT = 640;

int main(int argc, char** argv )
{
    CmdLineParser parser(argc, argv);
    LOG_UPDATE_CONFIGURE(parser.get<std::string>("logMode"),
                            parser.get<std::string>("logType"),
                                parser.get<std::string>("logLevel")); 
    LOG_INFO("Main: Program started..");

    std::cout << "VERSION: " << ESTIMATOR_VERSION_MAJOR << "."
		 << ESTIMATOR_VERSION_MINOR << "." 
		    << ESTIMATOR_VERSION_PATCH <<std::endl;

    Timer total_time;
    total_time.start();

    Parameters param;
    param = parser.getParameters();

    if(param.evaluate == 1){
	if(SUPPORT_EVALUATION){
            evaluate(param, INPUT_WIDTH, INPUT_HEIGHT);
            return 0;
	}else
	    std::cout << " Evaluation module is not supported for ARM platform!" << std::endl;
	    return 0;
    }
    if(param.train == 1){
        TrafficEstimator::trainSVM(param.svm_dataset);
        return 0;
    }
    LOG_INFO("Main: Inferencing mode..");
    
    // load input image list:
    if(param.capture){
        capture();
    }

    std::vector<std::string> input_files= {(param.data_path + param.images[0]),
                                           (param.data_path + param.images[1])};
    // keep all detected vehicles in a vector for both images:
    std::vector<std::vector<Centroid>> vector_of_vehicles; 
    vector_of_vehicles.reserve(50);

    //Detect all the vehicles:
    Result traffic;
    Timer time;
    time.start();
    LOG_INFO("Main: Detecting vehicles:");
    if(param.model_name == "yolo"){
        LOG_INFO("Main: YOLOv5n model is selected as object detector.");
        YoloDetector detector;
        detector.predict(vector_of_vehicles, param.yolo_model, param.yolo_class, 
                            input_files, INPUT_WIDTH, INPUT_HEIGHT);
    } 
    else if(param.model_name == "mobilenet"){
        LOG_INFO("Main: MobilNet_SSDv2 model is selected as object detector.");
        MobilenetDetector detector;
        detector.predict(vector_of_vehicles, param.mobilenet_model, param.mobilenet_config, param.mobilenet_class, 
                            input_files, INPUT_WIDTH, INPUT_HEIGHT);
    } 
    else {
        LOG_ERROR("Main: Incorrect Model name!");
        return -1;
    }
    traffic.detection_time = time.stop();

    // Find Vehicles Trajectory:
    LOG_INFO("Main: Tracking vehicles:");
    Lane lines(param.data_path + "../lines.txt");
    time.start();
    std::vector<Centroid> tracked_vehicles;
    cv::Size image_size = {INPUT_WIDTH, INPUT_HEIGHT};
    VehicleTracker tracker;
    tracker.track(vector_of_vehicles, tracked_vehicles, lines, image_size);
    traffic.total_detected_vehicles = vector_of_vehicles[0].size() + vector_of_vehicles[1].size();
    traffic.total_tracked_vehicles = tracked_vehicles.size();
    traffic.tracking_time = time.stop();
    LOG_TRACE("Main: Tracking time: ", traffic.tracking_time );

    // Traffic estimation:
    LOG_INFO("Main: Estimating traffic:");
    time.start();
    TrafficEstimator estimator;
    estimator.estimate(tracked_vehicles, lines, image_size, traffic);
    traffic.estimation_time = time.stop();
    LOG_TRACE("Main: Estimation time: ", traffic.estimation_time );

    // Write to result file:
    time.start();
    FileUtils::save(param.data_path, tracked_vehicles, traffic);
    FileUtils::drawResultOnImage(input_files, lines, vector_of_vehicles, param.images, INPUT_WIDTH, INPUT_HEIGHT);
    traffic.saving_time = time.stop();
    LOG_TRACE("Main: Saved the result to: ", "'result/result.txt'" );
    LOG_TRACE("Main: Saving time: ", traffic.saving_time );


    std::cout << "\nComputation for " << input_files.size() << " frames:\n";
    std::cout << "    -Traffic Status: " << traffic.prediction << "\n";
    std::cout << "    -Number of detected vehicles: "  << traffic.total_detected_vehicles << "\n";
    std::cout << "    -Number of tracked vehicles: "  << traffic.total_tracked_vehicles << "\n";
    std::cout << "    -Flow density: "  << (int)traffic.density << "% \n";
    std::cout << "    -Flow average speed: "  << (int)traffic.average_flow_speed << "\n";
    std::cout << "    -Highest flow is in lane number: "  << traffic.highest_lane_flow << "\n\n";

    std::cout << "Computation Time" << "\n";
    std::cout << "    -Detection     = " << std::fixed << std::setprecision(4)<< traffic.detection_time << "s\n";
    std::cout << "    -Tracking      = " << std::fixed << std::setprecision(4)<< traffic.tracking_time << "s\n";
    std::cout << "    -Estimation    = " << std::fixed << std::setprecision(4)<< traffic.estimation_time << "s\n";
    std::cout << "    -Total         = " << std::fixed << std::setprecision(4)<< total_time.stop() << "s\n";

    return 0;
}
