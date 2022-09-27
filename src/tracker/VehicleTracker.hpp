#pragma once
#include <vector>
#include <opencv2/core/types.hpp>
#include "../entity/Vehicle.hpp"
#include "../utils/Lane.hpp"

class VehicleTracker {
public:
	void track(std::vector<std::vector<Centroid>>& vector_of_centeroids, 
                        std::vector<Centroid>& tracking_vehicles, Lane& lines, const cv::Size& image_size );
private:
	static void findVehiclesTrajectory(std::vector<Centroid> &existingVehicles,
                                std::vector<Centroid> &currentFrameVehicles,
                                    Lane& lines, const cv::Size& image_size);
    static double distanceBetweenPoints(const cv::Point& point1, const cv::Point& point2);
};