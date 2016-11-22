#ifndef FILE_TELEMETRY_H
#define FILE_TELEMETRY_H
#include <stdint.h>
#include "telemetry_allocator.h"
#include "messaging.h"

#ifdef __cplusplus
extern "C" {
#endif

void file_telemetry_start(void);

bool file_telemetry_input_connected(void);

bool file_telemetry_output_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* FILE_TELEMETRY_H */
