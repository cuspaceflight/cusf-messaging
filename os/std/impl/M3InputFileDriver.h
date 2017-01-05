#pragma once

#include <thread>
#include <fstream>
#include "InputFileDriver.h"

class M3InputFileDriver : public IInputFileDriver {
public:
    M3InputFileDriver(const char* filename);
    ~M3InputFileDriver();

    bool getConnected();

private:
    std::thread thread_;
    std::unique_ptr<std::ifstream> input_stream_;
};
