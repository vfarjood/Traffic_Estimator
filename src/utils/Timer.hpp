#pragma once
#include <chrono>
#include <iomanip>
#include <sstream>

class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> begin, end;
    std::chrono::duration<float> duration;
public:
    void start();
    float stop();
    std::string getCurrentTime();
};