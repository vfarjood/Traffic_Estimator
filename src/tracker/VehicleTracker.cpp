#include "VehicleTracker.hpp"

void VehicleTracker::track(std::vector<std::vector<Centroid>>& vector_of_centeroids, 
                        std::vector<Centroid>& tracking_vehicles, Lane& lines, const cv::Size& image_size )
{
    // find vehicles lane number:
    for(auto& vehicles : vector_of_centeroids){
        lines.findLineNumber(vehicles, image_size);
    }

    // remove outside vehicles:
    std::vector<std::vector<int>> index_for_deleting;
    auto coverage_mask = lines.findCoverage(image_size);
    for(int j = 0; j < vector_of_centeroids.size(); j++){
        for(int i = 0; i < vector_of_centeroids[j].size(); i++){
            if((cv::pointPolygonTest(coverage_mask, vector_of_centeroids[j][i].center, false)) == -1){
                vector_of_centeroids[j].erase(vector_of_centeroids[j].begin()+i);
                i--;
            }
        }
    }

    for (auto &vehicle : vector_of_centeroids[0]){
        vehicle.position_history.push_back(vehicle.center);
        tracking_vehicles.push_back(vehicle);
    }

    findVehiclesTrajectory(tracking_vehicles, vector_of_centeroids[1], lines, image_size);

    // sort the vecotr of vehicle based on their lane number:
    std::sort(tracking_vehicles.begin(), tracking_vehicles.end(), 
            [](const Centroid& v1, const Centroid& v2) 
                {return v1.lane_num < v2.lane_num;});

}

void VehicleTracker::findVehiclesTrajectory(std::vector<Centroid> &existingVehicles,
                                std::vector<Centroid> &currentFrameVehicles,
                                    Lane& lines, const cv::Size& image_size) 
{
    for (auto &currentFrameVehicle : currentFrameVehicles) 
    {
        int least_distance_index = 0;
        double least_distance = 100000.0;
        for (int i = 0; i < existingVehicles.size(); i++) 
        {
            if((existingVehicles[i].lane_num == currentFrameVehicle.lane_num) &&
               (existingVehicles[i].position_history.size() == 1))
            {
                if ((existingVehicles[i].area >= currentFrameVehicle.area) ||
                    (existingVehicles[i].box.y >= currentFrameVehicle.box.y))
                {
                    double distance = distanceBetweenPoints(currentFrameVehicle.center, existingVehicles[i].center);
                    if (distance < least_distance) 
                    {
                        least_distance = distance;
                        least_distance_index = i;
                    }
                }
            }
        }

        if (least_distance < (image_size.height * 0.6)){

            const int fps = 1;
            const float speed_factor = (existingVehicles[least_distance_index].center.y / image_size.height) + 2;
            existingVehicles[least_distance_index].distance = least_distance;
            existingVehicles[least_distance_index].speed = (least_distance/speed_factor)/fps;
            existingVehicles[least_distance_index].box_history.push_back(currentFrameVehicle.box);
            existingVehicles[least_distance_index].position_history.push_back(currentFrameVehicle.center);
        } else { 
            currentFrameVehicle.position_history.push_back(currentFrameVehicle.center);
            existingVehicles.push_back(currentFrameVehicle);
        }
    }
}

double VehicleTracker::distanceBetweenPoints(const cv::Point& point1, const cv::Point& point2) 
{
    double distance = sqrt(pow((point2.x - point1.x), 2) + pow((point2.y - point1.y), 2));
    return distance;
}