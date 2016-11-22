#pragma once

#include <thread>
#include <fstream>

class OutputFileSerialDriver {
public:
    OutputFileSerialDriver(const char* filename);
    ~OutputFileSerialDriver();

    bool getConnected();

private:
    std::thread thread_;
    std::unique_ptr<std::ofstream> output_stream;
};
