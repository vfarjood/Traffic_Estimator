#include "CmdLineParser.hpp"


CmdLineParser::CmdLineParser(int argc, char** argv) 
{
    this->keys_ =
        "{ input  i          | |      path to data location where you have input images                 }"
        "{ img1           |img1.jpg|  Image number 1 name (just name with extention)                    }"
        "{ img2           |img2.jpg|  Image number 2 name (just name with extention)                    }"
        "{ detector d       |yolo|    Model name [1:yolo, 2:mobilenet]                                  }"
        "{ evaluate e      |false|    Evaluate models with test set.                                    }"
        "{ train t         |false|    Train svm classifier model.                                       }"
        "{ model  m          | |      Model path contains trained weights.                              }"
        "{ config f          | |      Path to model's config parameters.                                }"
        "{ class  c          | |      Path to class file.                                               }"
        "{ path   l          | |      Path to save the result.                                          }"
      
        // "{ thr t           | 0.5 |    Confidence threshold.                                             }"
        // "{ nms n           | 0.5 |    Non-maximum suppression threshold.                                }"

        "{ show s          |false|    show computation result on the terminal.                          }"

        "{ logMode         |DISABLE|   Define log mode  <ENABLE, DISABLE>.                               }"
        "{ logType         |ALL   |   Define log type  <FILE, CONSOLE, ALL>.                            }"
        "{ logLevel        |INFO |   Define log level <INFO, WARNING, ERROR, BUFFER, TRACE, DEBUG >.   }"

        "{ help  h           | |      Print help message.                                               }";

    this->parser_ = std::make_unique<cv::CommandLineParser>(argc, argv, keys_);
    this->argc_ = argc;
    this->argv_ = argv;
    
    this->parser_->about("Use this script to run Traffic Estimator application using deep learning. ");
    if (this->parser_->has("help"))
    {
        this->parser_->printMessage();
        this->printExample();
        exit(0);
    }
}
Parameters CmdLineParser::getParameters(){
    Parameters param;
    param.evaluate = parser_->get<bool>("evaluate");
    param.train = parser_->get<bool>("train");
    if(parser_->get<std::string>("detector") != "")
        param.model_name  = parser_->get<std::string>("detector");
    if(parser_->get<std::string>("model") != "")
        param.yolo_model  = parser_->get<std::string>("model");
    if(parser_->get<std::string>("class") != "")
        param.yolo_class  = parser_->get<std::string>("class");
    if(parser_->get<std::string>("model") != "")
        param.mobilenet_model  = parser_->get<std::string>("model");
    if(parser_->get<std::string>("config") != "")
        param.mobilenet_config  = parser_->get<std::string>("config");
    if(parser_->get<std::string>("class") != "")
        param.mobilenet_class  = parser_->get<std::string>("class");

    if(parser_->get<std::string>("input") != ""){
        param.data_path  = parser_->get<std::string>("input");
        param.svm_dataset  = parser_->get<std::string>("input");
    }
    if(parser_->get<std::string>("img1") != "")
        param.images[0]  = parser_->get<std::string>("img1");
    if(parser_->get<std::string>("img2") != "")
        param.images[1]  = parser_->get<std::string>("img2");
    return param;
}

void CmdLineParser::printExample() 
{
    std::cout << "Example: \n\t"
                   "./app "
                   " --input=../media/traffic/road_1/ "
                   " --name=yolo"
                   " --model=../models/yolo/yolov5n.onnx\n" << std::endl;
}

