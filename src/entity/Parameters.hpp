#pragma once
#include <string>

struct Parameters {
    std::string model_name{"yolo"};
    std::string yolo_model{"../models/yolo/yolov5n.onnx"};
    std::string yolo_class{"../models/yolo/classes.txt"};

    std::string mobilenet_model{"../models/mobilenet/frozen_inference_graph.pb"};
    std::string mobilenet_config{"../models/mobilenet/ssd_mobilenet_v2_coco_2018_03_29.pbtxt"};
    std::string mobilenet_class{"../models/mobilenet/classes.txt"};

    std::string data_path{"../media/dataset/road_1/1/"};
    std::string svm_dataset{"../media/svm_dataset/dataset.txt"};
    std::string images[2] = {"img1.jpg", "img2.jpg"};
    bool evaluate = false;
    bool train = false;
};
