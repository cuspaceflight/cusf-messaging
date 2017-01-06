#include <fstream>
#include <ctime>
#include <config/avionics_config.h>
#include "file_telemetry.h"
#include "cpp_utils.h"
#include "impl/OutputFileDriver.h"
#include "impl/InputFileDriver.h"
#include "impl/M3InputFileDriver.h"
#include "impl/M3OutputFileDriver.h"
#include "impl/CSVOutputFileDriver.h"


#if FILE_TELEMETRY_ENABLED

std::unique_ptr<IOutputFileDriver> out_driver;
std::unique_ptr<IInputFileDriver> in_driver;

inline bool fileExists(const std::string& fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}

static void file_telemetry_output_start(void) {
    if (!local_config.output_file_name || out_driver)
        return;

    std::string output_file_name = std::string(local_config.output_file_name);
    std::string output_file_extension = output_file_name.substr(1 + output_file_name.find_last_of('.'));
    output_file_name.erase(output_file_name.find_last_of('.'));


    std::string name = output_file_name + "." + output_file_extension;
    if (fileExists(name)) {
        if (!local_config.output_file_overwrite_enabled) {
            fprintf(stderr, "Output file already exists\n");
            return;
        } else {
            printf("Overwriting existing file: %s\n", name.c_str());
        }
    } else {
        printf("Writing data to file: %s\n", name.c_str());
    }

    if (output_file_extension == "tel") {
        out_driver = std::make_unique<OutputFileDriver>(name.c_str());
    } else if (output_file_extension == "m3tel") {
        out_driver = std::make_unique<M3OutputFileDriver>(name.c_str());
    } else if (output_file_extension == "csv") {
        out_driver = std::make_unique<CSVOutputFileDriver>(name.c_str());
    } else {
        printf("Unrecognised output format %s", output_file_extension.c_str());
    }

}

static void file_telemetry_input_start(void) {
    if (!local_config.input_file_name || in_driver)
        return;

    if (!fileExists(local_config.input_file_name)) {
        printf("Input file not found: %s\n", local_config.input_file_name);
        return;
    }
    std::string fn(local_config.input_file_name);
    auto extension = fn.substr(fn.find_last_of(".") + 1);

    if (extension == "tel")
        in_driver = std::make_unique<InputFileDriver>(local_config.input_file_name);
    else if (extension == "m3tel")
        in_driver = std::make_unique<M3InputFileDriver>(local_config.input_file_name);
    else
        printf("Unrecognised input format %s", extension.c_str());
}

void file_telemetry_start(void) {
    file_telemetry_output_start();
    file_telemetry_input_start();
}

bool file_telemetry_input_connected(void) {
    return !!in_driver && in_driver->getConnected();
}

bool file_telemetry_output_connected(void) {
    return !!out_driver && out_driver->getConnected();
}

#endif