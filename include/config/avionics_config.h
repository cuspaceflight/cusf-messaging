#ifndef AVIONICS_CONFIG_H
#define AVIONICS_CONFIG_H
#include "component_state_config.h"
#include "telemetry_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*avionics_component_state_update_handler_t)(avionics_component_t component, avionics_component_state_t state, int line);

typedef struct avionics_config_t {
    // NB: This will be called from a variety of threads
    // and so should be a thread safe function
    // May be set to NULL
    const avionics_component_state_update_handler_t state_update_handler;

    const char* input_file_name;

    const char* output_file_name;

    bool output_file_overwrite_enabled;

    bool mute_component_state;
} avionics_config_t;

// This should be defined somewhere with the local configuration e.g in main.c
extern avionics_config_t local_config;

#ifdef __cplusplus
}
#endif

#endif /* AVIONICS_CONFIG_H */
