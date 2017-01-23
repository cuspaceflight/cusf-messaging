#pragma once
#include "OutputFileDriver.h"

class CSVOutputFileDriver : public IOutputFileDriver {
public:
    CSVOutputFileDriver(const char* filename);
    ~CSVOutputFileDriver();

    bool getConnected() override;

private:
    std::thread thread_;
    std::unique_ptr<std::ofstream> output_stream;
};
