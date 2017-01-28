#ifndef M3_TEL_OUTPUT_H
#define M3_TEL_OUTPUT_H

#include "file_telemetry.h"


#if FILE_TELEMETRY_OUTPUT_ENABLED
void m3tel_output_thread(void* arg);
#endif


#endif