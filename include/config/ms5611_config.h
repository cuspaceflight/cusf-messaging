#ifndef MS5611_CONFIG_H
#define MS5611_CONFIG_H
#include "compilermacros.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ms5611data_t {
    int32_t temperature; // Temperature in Celcius
    int32_t pressure; // Pressure in Pascals
} ms5611data_t;

typedef struct {
    uint16_t c1, c2, c3, c4, c5, c6;
} MS5611CalData;

STATIC_ASSERT(sizeof(ms5611data_t) == 8, ms5611data_padded);

static const uint32_t ms5611_send_over_usb_count = 5; // Will send 1 in every 100 samples

#ifdef __cplusplus
}
#endif

#endif /* MS5611_CONFIG_H */
