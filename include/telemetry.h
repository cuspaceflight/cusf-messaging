#ifndef TELEMETRY_H
#define TELEMETRY_H
#include <stdint.h>
#include "telemetry_config.h"
#include "compilermacros.h"

#ifdef __cplusplus
extern "C" {
#endif

// This is somewhat arbitrary atm
#define MAX_TELEMETRY_PAYLOAD_SIZE 4000

typedef struct telemetry_header_t {
    uint16_t id;
    uint8_t length;
    uint8_t reserved;
    uint32_t timestamp;
} telemetry_header_t;

typedef struct telemetry_t {
    struct telemetry_header_t header;
    uint8_t* payload;
} telemetry_t;

// Make sure compiler isn't inserting padding
STATIC_ASSERT(sizeof(telemetry_header_t) == 8, telemetry_header_padded_by_compiler);

#ifdef __cplusplus
}
#endif

#endif /* TELEMETRY_H */
