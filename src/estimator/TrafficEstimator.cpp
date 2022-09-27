#include "TrafficEstimator.hpp"


void TrafficEstimator::estimate(const std::vector<Centroid>& vehicles,
                                     const Lane& lines, const cv::Size& image_size, Result& prediction){
	
	// Calculate average traffic flow speed:
    prediction.average_flow_speed = [&]() {
        float avg = 0;
        int count = 0;
        for(auto it = vehicles.begin(); it != vehicles.end(); it++){
            if(it->position_history.size() > 1){
                avg += it->speed;
                count++;
            }
        }
        if(avg != 0)
            return (avg/count);
        else 
            return (0.0f);
    }();

    // Find the lane with the highest flow:
    prediction.highest_lane_flow = [&]() {
        std::map<int, int> frequency_map;
        int max_frequency = 0;
        int most_frequent_element = 0;
        for(auto it = vehicles.begin(); it != vehicles.end(); it++){
            int value = ++frequency_map[it->lane_num];
            if (value > max_frequency)
            {
                max_frequency = value;
                most_frequent_element = it->lane_num;
            }
        }
        return most_frequent_element;
    }();

    // Density estimation: 
    prediction.density = densityEstimator(vehicles, lines, image_size);

    // Traffci estimation:
    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("../models/svm/svm_model.xml");
    LOG_TRACE("Estimator: Svm model is loaded from: ", "../models/svm/svm_model.xml");

    float input[1][3] = {static_cast<float>(vehicles.size()), prediction.density, prediction.average_flow_speed};
    cv::Mat inputMat(1, 3, CV_32F, input);
    float predicted = svm->predict(inputMat);
    prediction.prediction = predicted;
}

float TrafficEstimator::densityEstimator(const std::vector<Centroid>& vehicles, const Lane& lines, const cv::Size& image_size){

    std::vector<cv::Point> road_coverage = lines.findCoverage(image_size);
    float road_area = cv::contourArea(road_coverage);

    float sum_of_boxes_area = [&]() {
        int sum_of_elems =0;
        for(auto it = vehicles.begin(); it != vehicles.end(); it++){
            if(cv::pointPolygonTest(road_coverage, (it->center), false) == -1)
                continue;
            sum_of_elems += it->area;
        }
        return sum_of_elems;
    }();

    return (sum_of_boxes_area/road_area) * 100;
}

void TrafficEstimator::trainSVM(const std::string& path){
    std::ifstream data_file(path);
    if (!data_file.is_open()){
        std::cout << "Unable to open file";
    }

    std::vector<int> labels;
    std::vector<std::array<int, 3>> inputs;
    std::array<int, 3> input = {0,0,0};
    int label;
    char skip{':'};
    while(data_file >> label >> skip >> input[0] >> input[1] >> input[2]){
        labels.push_back(label);
        inputs.push_back(input);
    }
    data_file.close();

    size_t data_size = inputs.size();
    Progressor bar(data_size);

    int label_data[data_size];
    float input_data[data_size][3];

    for(int i = 0; i < data_size; i++){
        label_data[i] = labels[i];
        for(int j = 0; j < 3; j++){
            input_data[i][j] = inputs[i][j];
        }
        bar.update();
    }
    cv::Mat trainingDataMat(data_size, 3, CV_32F, input_data);
    cv::Mat labelsMat(data_size, 1, CV_32SC1, label_data);

    // Train the SVM
    cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
    svm->setType(cv::ml::SVM::C_SVC);
    svm->setKernel(cv::ml::SVM::LINEAR);
    svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));
    svm->trainAuto(trainingDataMat, cv::ml::ROW_SAMPLE, labelsMat);
    svm->save("../models/svm/svm_model.xml");
    std::cout << "\nCompleted!" << std::endl;
    std::cout << "Number of samples: "<< data_size << std::endl;
    std::cout << "Model is saved at: /models/svm/svm_model.xml" << std::endl;
}