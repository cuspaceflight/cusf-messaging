#include <fstream>
#include <ctime>
#include "file_telemetry.h"
#include "cpp_utils.h"
#include "impl/OutputFileDriver.h"
#include "impl/InputFileDriver.h"
#include "impl/M3InputFileDriver.h"
#include "impl/M3OutputFileDriver.h"
#include "impl/CSVOutputFileDriver.h"


#if FILE_TELEMETRY_INPUT_ENABLED || FILE_TELEMETRY_OUTPUT_ENABLED
inline bool fileExists(const std::string& fileName) {
    std::ifstream infile(fileName);
    return infile.good();
}
#endif


#if FILE_TELEMETRY_INPUT_ENABLED
std::unique_ptr<IInputFileDriver> in_driver;

void file_telemetry_input_start(const char* filename) {
    if (!filename || in_driver)
        return;

    if (!fileExists(filename)) {
        printf("Input file not found: %s\n", filename);
        return;
    }
    std::string fn(filename);
    auto extension = fn.substr(fn.find_last_of(".") + 1);

    if (extension == "tel")
        in_driver = std::make_unique<InputFileDriver>(filename);
    else if (extension == "m3tel")
        in_driver = std::make_unique<M3InputFileDriver>(filename);
    else
        printf("Unrecognised input format %s", extension.c_str());
}

bool file_telemetry_input_connected(void) {
    return !!in_driver && in_driver->getConnected();
}

#endif

#if FILE_TELEMETRY_OUTPUT_ENABLED
std::unique_ptr<IOutputFileDriver> out_driver;

bool file_telemetry_output_start(const char* filename, bool overwrite) {
    if (!filename || out_driver)
        return false;

    std::string output_file_name = std::string(filename);
    std::string output_file_extension = output_file_name.substr(1 + output_file_name.find_last_of('.'));
    output_file_name.erase(output_file_name.find_last_of('.'));


    std::string name = output_file_name + "." + output_file_extension;
    if (fileExists(name)) {
        if (!overwrite) {
            fprintf(stderr, "Output file already exists\n");
            return false;
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

    return true;
}



bool file_telemetry_output_connected(void) {
    return !!out_driver && out_driver->getConnected();
}
#endif
