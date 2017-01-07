#ifndef FILE_TELEMETRY_H
#define FILE_TELEMETRY_H
#include <stdint.h>
#include "telemetry_allocator.h"
#include "messaging.h"
#include "compilermacros.h"

#ifdef __cplusplus
extern "C" {
#endif

// The maximum extension the incremental naming system will use
#define MAX_INC_NAME_EXTENSION 5

#ifndef FILE_TELEMETRY_ENABLED
#if defined MESSAGING_OS_STD
#define FILE_TELEMETRY_ENABLED 1
#elif defined MESSAGING_OS_CHIBIOS
#define FILE_TELEMETRY_ENABLED 0
#else
#error Unrecognised Messaging OS
#endif
#endif

#if FILE_TELEMETRY_ENABLED

void file_telemetry_output_start(const char* filename, bool overwrite);

void file_telemetry_input_start(const char* filename);

bool file_telemetry_input_connected(void);

bool file_telemetry_output_connected(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* FILE_TELEMETRY_H */
