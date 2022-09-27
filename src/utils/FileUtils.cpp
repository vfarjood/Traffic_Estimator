#include "FileUtils.hpp"


void FileUtils::save(const std::stringstream& os, const std::string& path){
    std::ofstream result_file(path);
    if (!result_file.is_open()){
        std::cout << "Unable to open file";
    }
    result_file << os.str();
    result_file.close();
}

void FileUtils::save(const std::string& sample_path, std::vector<Centroid>& tracked_vehicles, const Result& result)
{
    std::ofstream result_file("../result/result.txt");
    if (!result_file.is_open()){
        std::cout << "Unable to open file";
    }

    Timer current;
    result_file << "*** "<< current.getCurrentTime() << " ***\n\n";
    result_file << "Traffic Status: ";
    if (result.prediction == 1)
        result_file << "Low\n";
    else if (result.prediction == 2)
        result_file << "Medium\n";
    else if (result.prediction == 3)
        result_file << "High\n";
    result_file << "    -Number of detected vehicles: "  << result.total_detected_vehicles << "\n";
    result_file << "    -Number of tracked vehicles: "  << result.total_tracked_vehicles << "\n";
    result_file << "    -Flow density: "  << (int)result.density << "% \n";
    result_file << "    -Flow average speed: "  << (int)result.average_flow_speed << "\n";
    result_file << "    -Highest flow is in lane number: "  << result.highest_lane_flow << "\n";
    result_file << "---------------------" << "\n\n";

    // write quantitative information into results file:
    for(int num=0; num < tracked_vehicles.size(); num++)
    {
        result_file << "Vehicle Number: "<< (num+1) << "\n";
        result_file << "Vehicle Type: "  << tracked_vehicles[num].name << "\n";
        result_file << "Lane Number: "   << tracked_vehicles[num].lane_num << "\n";
        result_file << "Speed: "         << tracked_vehicles[num].speed << "\n";
        result_file << "Positions: "     << "\n";
        for(int i = 0; i < tracked_vehicles[num].position_history.size(); i++){
            result_file << "    frame["<< (int)(i+1) << "]: [" <<
            tracked_vehicles[num].position_history[i].x << ", " <<
            tracked_vehicles[num].position_history[i].y << "]\n";
        }
        result_file << "---------------------" << "\n";
    }
}

void FileUtils::drawResultOnImage(std::vector<std::string>& input_files, Lane& lines,
                         std::vector<std::vector<Centroid>>& vector_of_centeroids, const std::string images[],
                            const float INPUT_WIDTH, const float INPUT_HEIGHT)
{
    // std::default_random_engine generator;
    std::mt19937 generator(2019);
    // set uniform distribution for each R,G,B color:
    std::uniform_int_distribution<int> distribution(0, 255);

    int j=0;
    for(auto& centroids: vector_of_centeroids)
    {    
        cv::Mat image = cv::imread(input_files[j], 1);
        std::string path =  "../result/" + images[j];

        cv::resize(image, image, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));
        for(int i = 0; i < centroids.size(); i++)
        {
            cv::Scalar color = cv::Scalar(0.0, 255.0, 255.0);
            color = cv::Scalar(distribution(generator), distribution(generator), distribution(generator));
            std::string label = "[" + centroids[i].name + " : " + cv::format("%.2f",centroids[i].conf) + "]"; // + " L:" + std::to_string(centroids[i].lane_num);
    
            cv::rectangle(image, cv::Point(centroids[i].box.x, centroids[i].box.y), cv::Point(centroids[i].box.x+centroids[i].box.width, centroids[i].box.y+centroids[i].box.height), color, 2);
            cv::putText(image, label, cv::Point(centroids[i].box.x, centroids[i].box.y-5), cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 1);
        }
        lines.draw(image);
        // drawGrid(image, lines);
        cv::imwrite(path, image);
        ++j;
    }
}

void FileUtils::drawGrid(cv::Mat &image, Lane& lines_vector)
{
    cv::LineIterator left_it(image.size(), lines_vector.line.front().first, lines_vector.line.front().second, 8);
    cv::LineIterator right_it(image.size(), lines_vector.line.back().first, lines_vector.line.back().second, 8);

    for(int i = 0; i < left_it.count; i++, left_it++, right_it++)
    {
        if((i%8) == 0)
        {
            cv::Point left= left_it.pos();
            cv::Point right= right_it.pos();
            cv::line(image, cv::Point(left.x, left.y), cv::Point(right.x, right.y), cv::Scalar(173.0, 255.0, 47.0), 2);
        }
    }

}

void FileUtils::printMatrix(std::vector<std::pair<int, int>>& mismatch,
                             std::map<std::string, int>& labels,
                              std::map<std::string, int>& predictions,
                               std::stringstream& output_stream){
    
    int dataset_size = (labels["low"] + labels["medium"] + labels["high"]);
    
                           // L     M     H
    float array_low[3] =    {0.0f, 0.0f, 0.0f};
    float array_medium[3] = {0.0f, 0.0f, 0.0f};
    float array_high[3] =   {0.0f, 0.0f, 0.0f};

    int low=predictions["low"];
    int medium=predictions["medium"];
    int high=predictions["high"];

    for(auto miss : mismatch){
        if(miss.second == 1)
            --low;
        else if(miss.second == 2)
            --medium;
        else if(miss.second == 3)
            --high;
        
        if(miss.first == 1 && miss.second == 2)
            array_low[1]++;
        else if(miss.first == 1 && miss.second == 3)
            array_low[2]++;     

        if(miss.first == 2 && miss.second == 1)
            array_medium[0]++;
        else if(miss.first == 2 && miss.second == 3)
            array_medium[2]++;   

        if(miss.first == 3 && miss.second == 1)
            array_high[0]++;
        else if(miss.first == 3 && miss.second == 2)
            array_high[1]++;          
    }
    array_low[0] = low;
    array_medium[1] = medium;
    array_high[2] = high;

    float recall_low  =  array_low[0] / (array_low[0] + array_low[1] + array_low[2]);
    float recall_medium  =  array_medium[1] / (array_medium[0] + array_medium[1] + array_medium[2]);
    float recall_high  =  array_high[2] / (array_high[0] + array_high[1] + array_high[2]);

    float precision_low  =  array_low[0] / (array_low[0] + array_medium[0] + array_high[0]);
    float precision_medium  =  array_medium[1] / (array_low[1] + array_medium[1] + array_high[1]);
    float precision_high  =  array_high[2] / (array_low[2] + array_medium[2] + array_high[2]);

    float total_acc = (array_low[0] + array_medium[1] + array_high[2]) / dataset_size;


    output_stream << std::endl;
    output_stream << "|------------------------------------|" << std::endl;
    output_stream << "|\e[1m  Confusion Matrix for Estimation   \e[0m|"<< std::endl;
    output_stream << "|------------------------------------|" << std::endl;

    output_stream
        << "|Input:"
        << std::left
        << std::setw(6)
        << dataset_size
        << std::left
        << std::setw(8)
        << "|Low"
        << std::left
        << std::setw(8)
        << "|Medium"
        << std::left
        << std::setw(8)
        << "|High"
        << "|\n";
        output_stream << "|------------ ------- ------- -------|" << std::endl;

        output_stream
            << "|Low:"
            << std::left
            << std::setw(8)
            << (int)(array_low[0] + array_low[1] + array_low[2])
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_low[0]
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_low[1]
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_low[2]
            << "|\n";
        output_stream

            << "|Medium:"
            << std::left
            << std::setw(5)
            << (int)(array_medium[0] + array_medium[1] + array_medium[2])
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_medium[0]
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_medium[1]
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_medium[2]
            << "|\n";
        output_stream
            << "|High:"
            << std::left
            << std::setw(7)
            << (int)(array_high[0] + array_high[1] + array_high[2])
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_high[0]
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_high[1]
            << "|"
            << std::left
            << std::setw(7)
            << (int)array_high[2]
            << "|\n";
    output_stream << "|------------ ------- ------- -------|" << std::endl;
    output_stream 
            << std::left
            << std::setw(13)
            << "|Recall:"
            << "|"
            << std::left
            << std::setw(7)
            << std::fixed << std::setprecision(2)<< recall_low
            << "|"
            << std::left
            << std::setw(7)
            << std::fixed << std::setprecision(2)<< recall_medium
            << "|"
            << std::left
            << std::setw(7)
            << std::fixed << std::setprecision(2)<< recall_high
            << "|\n";
    output_stream 
            << std::left
            << std::setw(13)
            << "|Precision:"
            << "|"
            << std::left
            << std::setw(7)
            << std::fixed << std::setprecision(2)<< precision_low
            << "|"
            << std::left
            << std::setw(7)
            << std::fixed << std::setprecision(2)<< precision_medium
            << "|"
            << std::left
            << std::setw(7)
            << std::fixed << std::setprecision(2)<< precision_high
            << "|\n";
    output_stream << "|------------------------------------|" << std::endl;
    output_stream 
            << std::left
            << std::setw(14)
            << "|Total Acc:"
            // << "|"
            << std::left
            << std::setw(23)
            << std::fixed << std::setprecision(2)<< total_acc
            << "|\n";
    output_stream << "|------------------------------------|" << std::endl;
}

void FileUtils::printMismatchTable(const int total_detection_predict, const int total_tracking_predict, const int estimation_mismatch_size,
                                    const int total_detection_size, const int total_tracking_size, const int total_estimation_size,
                                     std::stringstream& output_stream){

    int detection_mismatch_size = std::abs(total_detection_size - total_detection_predict);
    int tracking_mismatch_size = std::abs(total_tracking_size - total_tracking_predict);

    float detection_error = (float)detection_mismatch_size / (float)total_detection_size;
    float tracking_error = (float)tracking_mismatch_size / (float)total_tracking_size;
    float estimation_error = (float)estimation_mismatch_size / (float)total_estimation_size;
    
    float detection_acc = (float)total_detection_predict / (float)total_detection_size;
    float tracking_acc = (float)total_tracking_predict / (float)total_tracking_size;
    float estimation_acc = (float)(total_estimation_size - estimation_mismatch_size) / (float)total_estimation_size;

    float total_acc = (detection_acc + tracking_acc + estimation_acc) / 3;

    output_stream << std::endl;
    output_stream << "|--------------------------------------------|" << std::endl;
    output_stream << "|\e[1m               Total Accuracy               \e[0m|"<< std::endl;
    output_stream << "|--------------------------------------------|" << std::endl;
    output_stream
        << std::left
        << std::setw(12)
        << "|Model:"
        << std::left
        << std::setw(11)
        << "|Detection"
        << std::left
        << std::setw(11)
        << "|Tracking"
        << std::left
        << std::setw(10)
        << "|Estimation"
        << "|\n";
    output_stream << "|----------- ---------- ---------- ----------|" << std::endl;

        output_stream 
            << std::left
            << std::setw(12)
            << "|Samples:"
            << "|"
            << std::left
            << std::setw(10)
            << total_detection_size
            << "|"
            << std::left
            << std::setw(10)
            << total_tracking_size
            << "|"
            << std::left
            << std::setw(10)
            << total_estimation_size
            << "|\n";
        output_stream 
            << std::left
            << std::setw(12)
            << "|Mismatch:"
            << "|"
            << std::left
            << std::setw(10)
            << detection_mismatch_size
            << "|"
            << std::left
            << std::setw(10)
            << tracking_mismatch_size
            << "|"
            << std::left
            << std::setw(10)
            << estimation_mismatch_size
            << "|\n";
    output_stream << "|----------- ---------- ---------- ----------|" << std::endl;
        output_stream 
            << std::left
            << std::setw(12)
            << "|Errors:"
            << "|"
            << std::left
            << std::setw(10)
            << std::fixed << std::setprecision(2)<< detection_error
            << "|"
            << std::left
            << std::setw(10)
            << std::fixed << std::setprecision(2)<< tracking_error
            << "|"
            << std::left
            << std::setw(10)
            << std::fixed << std::setprecision(2)<< estimation_error
            << "|\n";
        output_stream 
            << std::left
            << std::setw(12)
            << "|Accuracy:"
            << "|"
            << std::left
            << std::setw(10)
            << std::fixed << std::setprecision(2)<< detection_acc
            << "|"
            << std::left
            << std::setw(10)
            << std::fixed << std::setprecision(2)<< tracking_acc
            << "|"
            << std::left
            << std::setw(10)
            << std::fixed << std::setprecision(2)<< estimation_acc
            << "|\n";
        output_stream << "|--------------------------------------------|" << std::endl;
        output_stream 
            << std::left
            << std::setw(13)
            << "|Total Acc:"
            // << "|"
            << std::left
            << std::setw(32)
            << std::fixed << std::setprecision(2)<< total_acc
            << "|\n";
    output_stream << "|--------------------------------------------|" << std::endl;
    output_stream << std::endl;
}