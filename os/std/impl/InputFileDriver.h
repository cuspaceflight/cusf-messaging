#pragma once

#include <thread>
#include <fstream>

class IInputFileDriver {
public:
    virtual bool getConnected() = 0;
    virtual ~IInputFileDriver() {

    }
};

class InputFileDriver : public IInputFileDriver {
public:
    InputFileDriver(const char* filename);
    ~InputFileDriver();

    bool getConnected() override;

private:
    std::thread thread_;
    std::unique_ptr<std::ifstream> input_stream_;
};
