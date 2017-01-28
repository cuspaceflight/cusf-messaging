#ifndef COMPONENT_STATE_CONFIG_H
#define COMPONENT_STATE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    avionics_component_messaging = 0,
    avionics_component_telemetry_allocator,
    avionics_component_adis16405,
    avionics_component_mpu9250,
    avionics_component_ms5611,
	avionics_component_can_telemetry,
    avionics_component_world_mag_model,
	avionics_component_ublox,
	avionics_component_sd_card,
	avionics_component_file_telemetry_output,
    avionics_component_state_board_config,
    avionics_component_max // This must be last
} avionics_component_t;

typedef enum {
	state_ok = 0,
	state_initializing = 1<<0,
	state_error = 1<<1,
} avionics_component_state_t;

#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_STATE_CONFIG_H */
