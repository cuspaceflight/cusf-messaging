#ifndef TELEMETRY_CONFIG_H
#define TELEMETRY_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// Defines a telemetry source
#define TELEMETRY_SOURCE(name, parent_name, id, delta_prefix_length) \
    name##_prefix_length = parent_name##_prefix_length + delta_prefix_length, \
    name##_mask = 0xFFFF ^ ((1 << (16 - name##_prefix_length))-1), \
    name = parent_name | (id << (16 - name##_prefix_length))

typedef enum {
    ts_all = 0b00000000000,
    ts_all_mask = 0b00000000000,
    ts_all_prefix_length = 0,

    // Root Namespace - 16 bits remaining
    TELEMETRY_SOURCE(ts_system, ts_all, 0, 5),
    TELEMETRY_SOURCE(ts_spalax, ts_all, 1, 5),
    TELEMETRY_SOURCE(ts_m3_can, ts_all, 2, 5), // Telemetry source for unrecognised CAN packets


    // System Namespace - 11 bits remaining
    TELEMETRY_SOURCE(ts_component_state, ts_system, 0, 8),


    // Spalax Namespace - 11 bits remaining
    TELEMETRY_SOURCE(ts_raw_data, ts_spalax, 0, 2),
    TELEMETRY_SOURCE(ts_state_estimate, ts_spalax, 1, 2),


    // Spalax Raw Data Namespace - 6 bits remaining
    TELEMETRY_SOURCE(ts_imu_data, ts_raw_data, 0, 2),
    TELEMETRY_SOURCE(ts_gps_data, ts_raw_data, 1, 2),


    // Spalax IMU Data Namespace - 4 bits remaining
    TELEMETRY_SOURCE(ts_ms5611_data, ts_imu_data, 0, 4),
    TELEMETRY_SOURCE(ts_mpu9250_data, ts_imu_data, 1, 4),
    TELEMETRY_SOURCE(ts_adis16405_data, ts_imu_data, 2, 4),

    // Spalax GPS Data Namespace - 4 bits remaining
    TELEMETRY_SOURCE(ts_ublox_nav, ts_gps_data, 0, 4),

    // Spalax State Estimate Namespace - 4 bits remaining
    TELEMETRY_SOURCE(ts_state_estimate_data, ts_state_estimate, 0, 4),
    TELEMETRY_SOURCE(ts_state_estimate_debug, ts_state_estimate, 1, 4),

} telemetry_id_t;

#ifdef __cplusplus
}
#endif

#endif /* TELEMETRY_CONFIG_H */
