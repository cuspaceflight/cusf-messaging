#include "config/mpu9250_config.h"

void mpu9250_calibrate_data(const mpu9250_config_t* config, const mpu9250_data_t* uncalibrated_data, mpu9250_calibrated_data_t* calibrated_data) {
    for (int i = 0; i < 3; i++) {
        calibrated_data->accel[i] = uncalibrated_data->accel[i] * config->accel_sf;
        calibrated_data->gyro[i] = uncalibrated_data->gyro[i] * config->gyro_sf;
        calibrated_data->magno[i] = uncalibrated_data->magno[i] * config->magno_sf;
    }
}