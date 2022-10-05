#pragma once
#include <string>

struct Result {
    int total_detected_vehicles = 0;
    int total_tracked_vehicles = 0;
    int highest_lane_flow = 0;
    float average_flow_speed = 0.0f;
    float density = 0.0f;
    int prediction = 0;

    float capturing_time = 0.0f;
    float detection_time = 0.0f;
    float tracking_time = 0.0f;
    float estimation_time = 0.0f;
    float saving_time = 0.0f;
};