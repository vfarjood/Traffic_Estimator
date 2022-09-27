#pragma once
#include <opencv2/core/utility.hpp>
#include <iostream>
#include <string>
#include "../entity/Parameters.hpp"

class CmdLineParser {
public:
    CmdLineParser(int argc, char* argv[]);
    ~CmdLineParser() = default;
    CmdLineParser(const CmdLineParser& source) = delete;
    CmdLineParser(CmdLineParser&& source) noexcept = delete;

    CmdLineParser& operator=(const CmdLineParser& source) = delete;
    CmdLineParser& operator=(CmdLineParser&& source) noexcept = delete;

    template <typename T>
    T get(const std::string& cmd) 
    {
        return parser_->get<T>(cmd);
    }
    Parameters getParameters();

private:
    int argc_;
    char** argv_;
    std::unique_ptr<cv::CommandLineParser> parser_;
    std::string keys_;

    void printExample(); 

};

