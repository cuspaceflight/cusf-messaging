#ifndef TELEMETRY_CONFIG_H
#define TELEMETRY_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


// Defines a telemetry source
// TODO: Add system which generates error if prefix length not at least length of parent
#define TELEMETRY_SOURCE(name, parent_name, id, prefix_length) \
    name = id & (~(parent_name##_mask)), \
    name##_mask = 0b11111111111 - ((1 << (11-prefix_length))-1)

// Defines a packet from the given source
#define TELEMETRY_ID(name, source, tag) \
    name = source | (tag & ~source##_mask)

typedef enum {
    telemetry_source_all = 0b00000000000,
    telemetry_source_all_mask = 0b00000000000,
    telemetry_source_packet_specific_mask = 0b11111111111,

    ///
    // Aggregate Sources
    ///

    TELEMETRY_SOURCE(telemetry_source_system_state, telemetry_source_all,                   0b00000000000, 4),
    TELEMETRY_SOURCE(telemetry_source_sensor_data, telemetry_source_all,                    0b00010000000, 4),
    TELEMETRY_SOURCE(telemetry_source_imu_data, telemetry_source_sensor_data,               0b00010000000, 5),
    ///
    // Unique Sources
    ///

    // Component State
    TELEMETRY_SOURCE(telemetry_source_component_state, telemetry_source_system_state,       0b00000000000, 11), 
    TELEMETRY_ID(telemetry_id_component_state_update, telemetry_source_component_state,     0b00000000000),
    

    // MS5611
    TELEMETRY_SOURCE(telemetry_source_ms5611, telemetry_source_imu_data,                    0b00010000010, 10),
    TELEMETRY_ID(telemetry_id_ms5611_config, telemetry_source_ms5611,                       0b00010000010),
    TELEMETRY_ID(telemetry_id_ms5611_data, telemetry_source_ms5611,                         0b00010000011),

    // MPU9250
    TELEMETRY_SOURCE(telemetry_source_mpu9250, telemetry_source_imu_data,                   0b00010000100, 10),
    TELEMETRY_ID(telemetry_id_mpu9250_config, telemetry_source_mpu9250,                     0b00010000100),
    TELEMETRY_ID(telemetry_id_mpu9250_data, telemetry_source_mpu9250,                       0b00010000101),

    // ADIS16405
    TELEMETRY_SOURCE(telemetry_source_adis16405, telemetry_source_imu_data,                 0b00010000110, 10),
    TELEMETRY_ID(telemetry_id_adis16405_config, telemetry_source_adis16405,                 0b00010000110),
    TELEMETRY_ID(telemetry_id_adis16405_data, telemetry_source_adis16405,                   0b00010000111),

    // State Estimation
    TELEMETRY_SOURCE(telemetry_source_state_estimation, telemetry_source_all,               0b10000000000, 9),
    TELEMETRY_ID(telemetry_id_state_estimate_config, telemetry_source_state_estimation,     0b10000000000),
    TELEMETRY_ID(telemetry_id_state_estimate_data, telemetry_source_state_estimation,       0b10000000001),
    TELEMETRY_ID(telemetry_id_state_estimate_status, telemetry_source_state_estimation,     0b10000000010),

    // Calibration
    TELEMETRY_SOURCE(telemetry_source_calibration, telemetry_source_all,                    0b01000000000, 10),
    TELEMETRY_ID(telemetry_id_calibration_control, telemetry_source_calibration,            0b01000000000),
    TELEMETRY_ID(telemetry_id_calibration_data, telemetry_source_calibration,               0b01000000001),
} telemetry_id_t;

typedef enum {
    telemetry_origin_imu = 0,
    telemetry_origin_avionics_gui = 1
} telemetry_origin_t;

#ifdef __cplusplus
}
#endif

#endif /* TELEMETRY_CONFIG_H */
