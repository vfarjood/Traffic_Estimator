#include "YoloDetector.hpp"

void YoloDetector::predict(std::vector<std::vector<Centroid>>& vector_of_centeroids,
                             const std::string& model_path,
                                const std::string& classes_path,
                                    std::vector<std::string>& input_files,
							             const float INPUT_WIDTH, const float INPUT_HEIGHT){

    // load yolo neural network model:
    Timer time;
    time.start();
    cv::dnn::Net model = cv::dnn::readNetFromONNX(model_path);
    LOG_TRACE("YoloDetector: Yolo model is loaded from: ", model_path );
    LOG_TRACE("YoloDetector: Model loading time: ", time.stop() );

    // Load class names:
    classes.reserve(5);
    std::ifstream ifs(std::string(classes_path).c_str());
    std::string line;
    while (getline(ifs, line)) {classes.push_back(line);} 
    LOG_TRACE("YoloDetector: Yolo classes is loaded from: ", classes_path );

    float imageTime = 0.0f;
    float detectioTime = 0.0f;
	for (std::string const& file : input_files)
    {
        // Load the image:
        Timer time;
        time.start();
        cv::Mat image = cv::imread(file, 1);
        cv::resize (image, image, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));
        imageTime += time.stop();

        time.start();
        // create blob from image: 
        cv::Mat input;   
        cv::dnn::blobFromImage(image, input, 1./255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
    
        // set the blob to the model:
        model.setInput(input);
    
        // forward pass through the model to carry out the detection:
        cv::Mat output;
        model.forward(output);
        LOG_TRACE("YoloDetector: Forwarding pass is done for image: ", file);

		std::vector<int> class_ids;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;
        std::vector<Centroid> centroids;
        centroids.reserve(50);
        cv::Mat detectedMat(output.size[1], output.size[2], CV_32F, output.ptr<float>());

        for (int i = 0; i < detectedMat.rows; ++i) 
        {
            float confidence = detectedMat.at<float>(i, 4);
            if (confidence >= CONFIDENCE_THRESHOLD) {
                float* classes_scores = &detectedMat.at<float>(i, 5);
                cv::Point class_id;
                double max_class_score;
                cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
                cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
                
                if (max_class_score > SCORE_THRESHOLD) {
                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);
                    int x = static_cast<int>(detectedMat.at<float>(i, 0));
                    int y = static_cast<int>(detectedMat.at<float>(i, 1));
                    int w = static_cast<int>(detectedMat.at<float>(i, 2));
                    int h = static_cast<int>(detectedMat.at<float>(i, 3));
                    boxes.push_back(cv::Rect((x - w / 2),(y - h / 2),w,h));
                }
            }
        }
        std::vector<int> nms_result;
        cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);
        for (int i = 0; i < nms_result.size(); i++) 
        {
            int idx = nms_result[i];
            Centroid object;
            object.id = class_ids[idx];
            object.conf = confidences[idx];
            object.name = classes[class_ids[idx]];
            object.box  = boxes[idx];
            object.area = (object.box.width * object.box.height);
            object.center     = cv::Point((object.box.x + (object.box.width/2)), (object.box.y + (object.box.height/2)));
            centroids.push_back(object);
        }
        vector_of_centeroids.push_back(centroids);
        LOG_TRACE("YoloDetector: Post processing is finished for image: ", file);
        detectioTime += time.stop();

    }
    LOG_TRACE("YoloDetector: Image Loading time: ", imageTime);
    LOG_TRACE("YoloDetector: Model detection time: ", detectioTime);

}