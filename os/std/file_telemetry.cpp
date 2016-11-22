#include <fstream>
#include <ctime>
#include <config/avionics_config.h>
#include "file_telemetry.h"
#include "impl/Utils.h"
#include "impl/OutputFileSerialDriver.h"
#include "impl/InputFileSerialDriver.h"

std::unique_ptr<OutputFileSerialDriver> out_driver;
std::unique_ptr<InputFileSerialDriver> in_driver;

inline bool fileExists(const char *fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

static void file_telemetry_output_start(void) {
    if (!local_config.output_file_name || out_driver)
        return;

    std::time_t now = std::time(NULL);
    std::tm * ptm = std::localtime(&now);
    char buffer[64];
    std::strftime(buffer, 64, "- %a, %d.%m.%Y %H:%M:%S", ptm);

    std::string name = local_config.output_file_name + std::string(buffer);

    if (fileExists(name.c_str())) {
        printf("Output file already exists!\n");
        exit(1);
    }

    out_driver = std::make_unique<OutputFileSerialDriver>(name.c_str());
}

static void file_telemetry_input_start(void) {
    if (!local_config.input_file_name || in_driver)
        return;

    in_driver = std::make_unique<InputFileSerialDriver>(local_config.input_file_name);
}

void file_telemetry_start(void) {
    file_telemetry_output_start();
    file_telemetry_input_start();
}

bool file_telemetry_input_connected(void) {
    return !!in_driver;
}

bool file_telemetry_output_connected(void) {
    return !!out_driver;
}
