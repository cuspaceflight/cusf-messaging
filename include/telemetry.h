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
    uint16_t length;
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

template <typename T>
const T* telemetry_get_payload(const telemetry_t* packet) {
    if (packet->header.length != sizeof(T))
        return nullptr;
    return (T*)packet->payload;
}

#endif

#endif /* TELEMETRY_H */
