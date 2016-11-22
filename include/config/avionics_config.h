#ifndef AVIONICS_CONFIG_H
#define AVIONICS_CONFIG_H
#include "component_state_config.h"
#include "telemetry_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*avionics_component_state_update_handler_t)(avionics_component_t component, avionics_component_state_t state, int line);
typedef void(*avionics_can_send_t)(uint16_t msg_id, bool can_rtr, uint8_t* data, uint8_t datalen);

typedef struct avionics_config_t {
    // The origin of the local system
    const telemetry_origin_t origin;

    // NB: This will be called from a variety of threads
    // and so should be a thread safe function
    // May be set to NULL
    const avionics_component_state_update_handler_t state_update_handler;

    // Used to send data over a CAN bus
    // May be set to NULL
    const avionics_can_send_t can_send;
} avionics_config_t;

// This should be defined somewhere with the local configuration e.g in main.c
extern const avionics_config_t local_config;

#ifdef __cplusplus
}
#endif

#endif /* AVIONICS_CONFIG_H */
