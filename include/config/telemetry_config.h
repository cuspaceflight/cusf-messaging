#ifndef TELEMETRY_CONFIG_H
#define TELEMETRY_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// A CAN ID has 11 bits
// The CAN driver uses 5 bits of that to identify the board
// This leaves 6 bits for assigning ids

// Defines a telemetry source
// TODO: Add system which generates error if prefix length not at least length of parent
#define TELEMETRY_SOURCE(name, parent_name, id, prefix_length) \
    name = ((id & (~(parent_name##_mask))) | parent_name), \
    name##_mask = 0b111111 - ((1 << (6-prefix_length))-1)

// Defines a packet from the given source
#define TELEMETRY_ID(name, source, tag) \
    name = source | (tag & ~source##_mask)

typedef enum {
    telemetry_source_all = 0b000000,
    telemetry_source_all_mask = 0b000000,
    telemetry_source_packet_specific_mask = 0b111111,

    TELEMETRY_ID(telemetry_id_can_msg_id, telemetry_source_all,                             0b111111),
    TELEMETRY_ID(telemetry_id_can_status, telemetry_source_all,                             0b000000),

    // State Estimation
    TELEMETRY_SOURCE(telemetry_source_state_estimation, telemetry_source_all,               0b001000, 3),
    TELEMETRY_ID(telemetry_id_state_estimate_config, telemetry_source_state_estimation,     0b001000),
    TELEMETRY_ID(telemetry_id_state_estimate_data, telemetry_source_state_estimation,       0b001001),
    TELEMETRY_ID(telemetry_id_state_estimate_status, telemetry_source_state_estimation,     0b001010),

    // Calibration
    TELEMETRY_SOURCE(telemetry_source_calibration, telemetry_source_all,                    0b010000, 3),
    TELEMETRY_ID(telemetry_id_calibration_control, telemetry_source_calibration,            0b010000),
    TELEMETRY_ID(telemetry_id_calibration_data, telemetry_source_calibration,               0b010100),

    // Component State
    TELEMETRY_SOURCE(telemetry_source_component_state, telemetry_source_all,                0b011000, 3),
    TELEMETRY_ID(telemetry_id_component_state_update, telemetry_source_component_state,     0b011000),

    TELEMETRY_SOURCE(telemetry_source_imu_data, telemetry_source_all,                       0b100000, 1),

    // MS5611
    TELEMETRY_SOURCE(telemetry_source_ms5611, telemetry_source_imu_data,                    0b100000, 3),
    TELEMETRY_ID(telemetry_id_ms5611_config, telemetry_source_ms5611,                       0b100000),
    TELEMETRY_ID(telemetry_id_ms5611_data, telemetry_source_ms5611,                         0b100100),

    // MPU9250
    TELEMETRY_SOURCE(telemetry_source_mpu9250, telemetry_source_imu_data,                   0b101000, 3),
    TELEMETRY_ID(telemetry_id_mpu9250_config, telemetry_source_mpu9250,                     0b101000),
    TELEMETRY_ID(telemetry_id_mpu9250_data, telemetry_source_mpu9250,                       0b101100),

    // ADIS16405
    TELEMETRY_SOURCE(telemetry_source_adis16405, telemetry_source_imu_data,                 0b111000, 3),
    TELEMETRY_ID(telemetry_id_adis16405_config, telemetry_source_adis16405,                 0b111000),
    TELEMETRY_ID(telemetry_id_adis16405_data, telemetry_source_adis16405,                   0b111100),

} telemetry_id_t;

typedef enum {
    telemetry_origin_m3fc = 1,
    telemetry_origin_m3psu = 2,
    telemetry_origin_m3pyro = 3,
    telemetry_origin_m3radio = 4,
    telemetry_origin_m3imu = 5,
    telemetry_origin_m3dl = 6,
    telemetry_origin_avionics_gui = 7
} telemetry_origin_t;

#ifdef __cplusplus
}
#endif

#endif /* TELEMETRY_CONFIG_H */
