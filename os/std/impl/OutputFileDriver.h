#pragma once

#include <thread>
#include <fstream>

class IOutputFileDriver {
public:
    virtual bool getConnected() = 0;

    virtual ~IOutputFileDriver() {

    }
};

class OutputFileDriver : public IOutputFileDriver {
public:
    OutputFileDriver(const char* filename);
    ~OutputFileDriver();

    bool getConnected() override;

private:
    std::thread thread_;
    std::unique_ptr<std::ofstream> output_stream;
};
