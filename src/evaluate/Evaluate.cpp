#include "Evaluate.hpp"

void evaluate(Parameters& param, const int image_width, const int image_height){

    LOG_INFO("Evaluate: evaluating mode:");

    std::vector<std::vector<std::filesystem::path>> dataset;
    std::vector<std::filesystem::path> dataset_lines;
    std::vector<std::vector<int>> labels;
    std::vector<std::vector<int>> predictions;
    
    Timer eval_time;
    eval_time.start();
    for (auto const& road_directory : std::filesystem::directory_iterator{param.data_path}) 
    {
        if(road_directory.path() == param.data_path + ".DS_Store" ){
            continue;
        }
        std::vector<std::filesystem::path> samples_dir;
        for (auto const& samples_directory : std::filesystem::directory_iterator{road_directory.path()}){
            if(samples_directory.path() == road_directory.path()/".DS_Store" ){
                continue;
            }else if(samples_directory.path() == road_directory.path()/"lines.txt" ){
                dataset_lines.push_back(samples_directory.path());
                continue;
            }else{
                samples_dir.push_back(samples_directory);

                std::ifstream label_file(samples_directory.path()/"label.txt");
                if (!label_file.is_open()){
                    std::cout << "Unable to open file";
                }
                std::vector<int> label;
                int detection, tracking, estimation;
                label_file >> detection >> tracking >> estimation;
                label.push_back(detection);
                label.push_back(tracking);
                label.push_back(estimation);
                label_file.close();
                labels.push_back(label);
            }
        }
        dataset.push_back(samples_dir);
    }

    Progressor bar(labels.size());
    std::cout << "\nEvaluating..\n";
    std::stringstream result_file;
    std::stringstream labels_file;
    int index = 0;
    for(auto road : dataset){ //vector of roads        
        Lane lines(dataset_lines[index]);
        for(auto samples_path : road){ //vector of samples
            bar.update();

            // load input image list:
            std::vector<std::string> input_files= {(samples_path/"img1.jpg"),
                                                   (samples_path/"img2.jpg")};
            // keep all detected vehicles in a vector for both images:
            std::vector<std::vector<Centroid>> vector_of_vehicles; 
            vector_of_vehicles.reserve(50);
        
	       //Detect all the vehicles:
            Result traffic;
            Timer time;
            time.start();
            LOG_INFO("Evaluate: Detecting vehicles:");
            if(param.model_name == "yolo"){
                LOG_INFO("Evaluate: YOLOv5n model is selected as object detector.");
                YoloDetector detector;
                detector.predict(vector_of_vehicles, param.yolo_model, param.yolo_class, 
                                    input_files, image_width, image_height);
            } 
            else if(param.model_name == "mobilenet"){
                LOG_INFO("Evaluate: MobilNet_SSDv2 model is selected as object detector.");
                MobilenetDetector detector;
                detector.predict(vector_of_vehicles, param.mobilenet_model, param.mobilenet_config, param.mobilenet_class, 
                                    input_files, image_width, image_height);
            } 
            else {
                LOG_ERROR("Evaluate: Incorrect Model name!");
                exit(-1);
            }
            traffic.detection_time = time.stop();
        
            // Find Vehicles Trajectory:
            LOG_INFO("Evaluate: Tracking vehicles:");
            time.start();
            std::vector<Centroid> tracked_vehicles;
            cv::Size image_size = {image_width, image_height};
            VehicleTracker tracker;
            tracker.track(vector_of_vehicles, tracked_vehicles, lines, image_size);
            traffic.total_detected_vehicles = vector_of_vehicles[0].size() + vector_of_vehicles[1].size();
            traffic.total_tracked_vehicles = tracked_vehicles.size();
            traffic.tracking_time = time.stop();
            LOG_TRACE("Evaluate: Tracking time: ", traffic.tracking_time );
        
            // Traffic estimation:
            LOG_INFO("Evaluate: Estimating traffic:");
            time.start();
            TrafficEstimator estimator;
            estimator.estimate(tracked_vehicles, lines, image_size, traffic);
            traffic.estimation_time = time.stop();
            LOG_TRACE("Evaluate: Estimation time: ", traffic.estimation_time );
            
            std::vector<int> predict;
            predict.push_back(traffic.total_detected_vehicles);
            predict.push_back(traffic.total_tracked_vehicles);
            predict.push_back(traffic.prediction);
            predictions.push_back(predict);

            result_file << traffic.total_detected_vehicles << " ";
            result_file << traffic.total_tracked_vehicles << " ";
            result_file << traffic.prediction << " \n";
        }
        index++;
    }

    //post-processing for evaluation result:
    int total_detection_label = 0;
    int total_tracking_label = 0;
    for(auto label : labels){
        labels_file << label[0] << " " << label[1] << " " << label[2] << std::endl;
        total_detection_label += label[0];
        total_tracking_label += label[1];
    }
    int total_detection_predict = 0;
    int total_tracking_predict = 0;
    for(auto predict : predictions){
        total_detection_predict += predict[0];
        total_tracking_predict += predict[1];
    }
    std::vector<std::pair<int, int>> estimation_mismatch_type;
    std::vector<int> mismatch_index;
    std::map<std::string, int> label_types{ {"low",0}, {"medium",0}, {"high",0}};
    std::map<std::string, int> predicted_types{ {"low",0}, {"medium",0}, {"high",0}};
    for(int i = 0; i < predictions.size(); i++){
        switch(labels[i][2])
        {
            case 1:
                label_types["low"]++; break;
            case 2:
                label_types["medium"]++; break;
            case 3:
                label_types["high"]++; break;
        }
        switch(predictions[i][2])
        {
            case 1:
                predicted_types["low"]++; break;
            case 2:
                predicted_types["medium"]++; break;
            case 3:
                predicted_types["high"]++; break;
        }
        if(predictions[i][2] != labels[i][2]){
            mismatch_index.push_back(i);
            estimation_mismatch_type.push_back(std::make_pair(labels[i][2], predictions[i][2]));
        }
    }
    std::cout << "  Completed!" << std::endl;
    std::cout << "Computed time for \e[1m" << labels.size() << "\e[0m samples: "<< eval_time.stop() << "s\n";


    std::stringstream output_stream;
    FileUtils::printMatrix(estimation_mismatch_type, label_types, predicted_types, output_stream);

    FileUtils::printMismatchTable(total_detection_predict, total_tracking_predict, estimation_mismatch_type.size(),
                                  total_detection_label, total_tracking_label, labels.size(), output_stream);

    std::cout << output_stream.str() << std::endl;

    FileUtils::save(labels_file, "../result/labels.txt");
    FileUtils::save(result_file, "../result/evaluated.txt");
    FileUtils::save(output_stream, "../result/result_table.txt");

}
