#include <memory.h>
#include "file_telemetry.h"

#if FILE_TELEMETRY_OUTPUT_ENABLED

#include "impl/m3teloutput.h"

static THD_WORKING_AREA(waFileTelemetryOutput, 1024);

bool output_running = false;

bool file_telemetry_output_start(const char* filename, bool overwrite) {
    (void)overwrite;

    if (!filename || output_running)
        return false;

    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) {
        return false;
    }
    const char* ext = dot + 1;

    if (strcmp(ext, "m3tel") == 0) {
        output_running = true;
        chThdCreateStatic(waFileTelemetryOutput, sizeof(waFileTelemetryOutput), NORMALPRIO, m3tel_output_thread,
                          (void *) filename);
        return true;
    }
    return false;
}

bool file_telemetry_output_connected(void) {
    return output_running;
}

#endif