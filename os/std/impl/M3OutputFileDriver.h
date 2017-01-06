#pragma once
#include "OutputFileDriver.h"

class M3OutputFileDriver : public IOutputFileDriver {
    public:
    M3OutputFileDriver(const char* filename);
    ~M3OutputFileDriver();

    bool getConnected() override;

    private:
    std::thread thread_;
    std::unique_ptr<std::ofstream> output_stream;
};
