#pragma once

#include <thread>
#include <fstream>

class InputFileSerialDriver {
public:
    InputFileSerialDriver(const char* filename);
    ~InputFileSerialDriver();

    bool getConnected();

private:
    std::thread thread_;
    std::ifstream input_stream_;
};
