#ifndef MPU9250_CONFIG_H
#define MPU9250_CONFIG_H
#include "compilermacros.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mpu9250_data_t {
    int16_t accel[3];
    int16_t temp;
    int16_t gyro[3];
    int16_t magno[3];
} mpu9250_data_t;

STATIC_ASSERT(sizeof(mpu9250_data_t) == 20, ms5611data_padded);

typedef struct mpu9250_config_t {
    // Converts to m/s^2
    float accel_sf;

    // Converts to rad/s
    float gyro_sf;

    // Converts to micro Tesla
    float magno_sf;

    float accel_bias[3];
    float magno_bias[3];

    uint8_t mag_sensitivity_adjustment[3];
} mpu9250_config_t;

typedef struct mpu9250_calibrated_data_t {
    // In m/s^3
    float accel[3];
    // In rad/s
    float gyro[3];
    // In micro Tesla
    float magno[3];

} mpu9250_calibrated_data_t;

// Also alters the magnetometer's axes to match those of the other sensors
void mpu9250_calibrate_data(const mpu9250_config_t* config, const mpu9250_data_t* uncalibrated_data, mpu9250_calibrated_data_t* calibrated_data);

static const uint32_t mpu9250_send_over_usb_count = 100; // Will send 1 in every 100 samples
static const uint32_t mpu9250_send_config_count = 5000; // Will resend config every 1000 samples

#ifdef __cplusplus
}
#endif

#endif /* MPU9250_CONFIG_H */
