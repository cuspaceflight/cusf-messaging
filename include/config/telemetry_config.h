#ifndef TELEMETRY_CONFIG_H
#define TELEMETRY_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// Defines a telemetry source
#define TELEMETRY_SOURCE(name, parent_name, id, suffix_length) \
    name##_mask = ((1 << suffix_length)-1), \
    name = ((id << parent_name##_suffix_length) & name##_mask) | parent_name, \
    name##_suffix_length = suffix_length

typedef enum {
    ts_all = 0b00000000000,
    ts_all_mask = 0b00000000000,
    ts_all_suffix_length = 0,

    TELEMETRY_SOURCE(ts_m3fc, ts_all,                                   1,        5),
    TELEMETRY_SOURCE(ts_m3psu, ts_all,                                  2,        5),
    TELEMETRY_SOURCE(ts_m3pyro, ts_all,                                 3,        5),
    TELEMETRY_SOURCE(ts_m3radio, ts_all,                                4,        5),
    TELEMETRY_SOURCE(ts_m3imu, ts_all,                                  5,        5),
    TELEMETRY_SOURCE(ts_m3dl, ts_all,                                   6,        5),


    TELEMETRY_SOURCE(ts_component_state, ts_m3imu,                      0,       11),

    // State Estimation
    TELEMETRY_SOURCE(ts_state_estimation, ts_m3imu,                     0b001,    8),
    TELEMETRY_SOURCE(ts_state_estimate_config, ts_state_estimation,     0b000,   11),
    TELEMETRY_SOURCE(ts_state_estimate_status, ts_state_estimation,     0b010,   11),
    TELEMETRY_SOURCE(ts_state_estimate_data, ts_state_estimation,       0b001,    9),


    // MS5611
    TELEMETRY_SOURCE(ts_ms5611, ts_m3imu,                               0b001010,11),
    TELEMETRY_SOURCE(ts_ms5611_data, ts_ms5611,                         0,       11),

    // MPU9250
    TELEMETRY_SOURCE(ts_mpu9250, ts_m3imu,                              0b011,    8),
    TELEMETRY_SOURCE(ts_mpu9250_config, ts_mpu9250,                     0b000,    9),
    TELEMETRY_SOURCE(ts_mpu9250_data, ts_mpu9250,                       0b001,    9),

    // ADIS16405
    TELEMETRY_SOURCE(ts_adis16405, ts_m3imu,                            0b110,    8),
    TELEMETRY_SOURCE(ts_adis16405_config, ts_adis16405,                 0b000,    9),
    TELEMETRY_SOURCE(ts_adis16405_data, ts_adis16405,                   0b000,    9),

    // UBLOX
    TELEMETRY_SOURCE(ts_ublox, ts_m3imu,                                0b101,    8),
    TELEMETRY_SOURCE(ts_ublox_nav, ts_ublox,                            0b000,    8),
} telemetry_id_t;

#ifdef __cplusplus
}
#endif

#endif /* TELEMETRY_CONFIG_H */
