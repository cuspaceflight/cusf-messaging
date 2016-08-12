#include "config/mpu9250_config.h"

void mpu9250_calibrate_data(const mpu9250_config_t* config, const mpu9250_data_t* uncalibrated_data, mpu9250_calibrated_data_t* calibrated_data) {
    float magno_temp[3];
    for (int i = 0; i < 3; i++) {
        calibrated_data->accel[i] = uncalibrated_data->accel[i] * config->accel_sf - config->accel_bias[i];
        calibrated_data->gyro[i] = uncalibrated_data->gyro[i] * config->gyro_sf;

        float sensitivity_adjustment = (float)(config->mag_sensitivity_adjustment[i] - 128) * 0.5f / 128.0f + 1;
        magno_temp[i] = uncalibrated_data->magno[i] * config->magno_sf * sensitivity_adjustment - config->magno_bias[i];
    }

    // We correct for the magnetometer's strange orientation
    calibrated_data->magno[0] = magno_temp[1];
    calibrated_data->magno[1] = magno_temp[0];
    calibrated_data->magno[2] = -magno_temp[2];
}