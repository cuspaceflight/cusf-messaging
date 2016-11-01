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

typedef struct mpu9250_config_t {
    // Converts to m/s^2
    float accel_sf;

    // Converts to rad/s
    float gyro_sf;

    int16_t magno_sf[3];
    int16_t magno_bias[3];
} mpu9250_config_t;

STATIC_ASSERT(sizeof(mpu9250_config_t) == 20, mpu9250_config_padded);

typedef struct ms5611data_t {
    int32_t temperature; // Temperature in Celcius
    int32_t pressure; // Pressure in Pascals
} ms5611data_t;

STATIC_ASSERT(sizeof(ms5611data_t) == 8, ms5611data_padded);

typedef struct state_estimate_t {
    float orientation_q[4];
    float angular_velocity[3];
} state_estimate_t;

STATIC_ASSERT(sizeof(state_estimate_t) == 28, ms5611data_padded);

typedef struct state_estimate_calibration_t {
    // The length of time over which samples were taken
    float sample_time;
    float accel_bias[3];
    float mag_bias[3];
} state_estimate_calibration_t;

STATIC_ASSERT(sizeof(state_estimate_calibration_t) == 28, state_estimate_calibration_padded);

typedef struct state_estimate_status_t {
    // The length of time over which samples were taken
    float sample_time;
    uint16_t number_prediction_steps;
    uint16_t number_update_steps;
} state_estimate_status_t;

STATIC_ASSERT(sizeof(state_estimate_status_t) == 8, state_estimate_status_padded);

typedef struct adis16405_config_t {
    // Converts to m/s^2
    float accel_sf;

    // Converts to rad/s
    float gyro_sf;

    uint16_t magno_sf[3];
    int16_t magno_bias[3];
} adis16405_config_t;

STATIC_ASSERT(sizeof(adis16405_config_t) == 20, adis16405_config_padded);

typedef struct adis16405_data_t {
    int16_t supply;
    int16_t gyro[3];
    int16_t accel[3];
    int16_t magno[3];
} adis16405_data_t;

STATIC_ASSERT(sizeof(adis16405_data_t) == 20, adis16405_data_padded);

typedef struct {
    int16_t magno_sf[3];
    uint8_t procedure;
    uint8_t padding;
    int16_t magno_bias[3];
} magno_calibration_data_t;

STATIC_ASSERT(sizeof(magno_calibration_data_t) == 14, calibration_data_padded);



#ifdef __cplusplus
}
#endif

#endif //SPALAX_TELEMETRY_PACKETS_H
