#include "Timer.hpp"

void Timer::start() {
    begin = std::chrono::steady_clock::now();
}
float Timer::stop(){
    end = std::chrono::steady_clock::now();
    duration = end - begin; 
    float second = duration.count();
    return second;
}
std::string Timer::getCurrentTime(){
    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}