#ifndef SPALAX_TELEMETRY_PACKETS_H
#define SPALAX_TELEMETRY_PACKETS_H
#include "stdint.h"
#include "compilermacros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mpu9250_data_t {
    int16_t accel[3];
    int16_t temp;
    int16_t gyro[3];
    int16_t magno[3];
} mpu9250_data_t;

STATIC_ASSERT(sizeof(mpu9250_data_t) == 20, mpu9250data_padded);

typedef struct ms5611data_t {
    int32_t temperature; // Temperature in Celcius
    int32_t pressure; // Pressure in Pascals
} ms5611data_t;

STATIC_ASSERT(sizeof(ms5611data_t) == 8, ms5611data_padded);

typedef struct adis16405_data_t {
    int16_t supply;
    int16_t gyro[3];
    int16_t accel[3];
    int16_t magno[3];
} adis16405_data_t;

STATIC_ASSERT(sizeof(adis16405_data_t) == 20, adis16405_data_padded);

typedef struct component_state_update_t {
    uint8_t overall_state;
    uint8_t component;
    uint8_t state;
    uint8_t line_number;
} component_state_update_t;

STATIC_ASSERT(sizeof(component_state_update_t) == 4, component_state_padded);

// A selection of fields from the UBX-NAV_PVT packet
typedef struct ublox_nav_t {
    uint16_t year; // Year (UTC)
    uint8_t month, day, hour, minute, second;
    uint8_t valid; // Validity Flags
    uint32_t t_acc;
    int32_t nano;
    uint8_t fix_type;
    uint8_t flags;
    uint8_t num_sv;
    int32_t lon, lat;
    int32_t height, h_msl; // In mm
    uint32_t h_acc, v_acc;
    int32_t velN, velE, velD; // In mm/sec
    uint32_t s_acc;
    uint16_t p_dop;
} ublox_nav_t;

STATIC_ASSERT(sizeof(ublox_nav_t) == 64, ublox_nav_padded);


typedef struct state_estimate_t {
    float orientation_q[4];
    float angular_velocity[3];

    // East, North, Up using WGS84 Ellipsoid
    float position[3]; // Latitude, Longitude, Elevation
    float velocity[3];
    float acceleration[3];
} state_estimate_t;

STATIC_ASSERT(sizeof(state_estimate_t) == 64, state_estimate_padded);

typedef struct state_estimate_debug_t {
    float gyro_bias[3];
} state_estimate_debug_t;

#ifdef __cplusplus
}
#endif

#endif //SPALAX_TELEMETRY_PACKETS_H
