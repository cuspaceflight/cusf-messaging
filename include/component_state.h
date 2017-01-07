#ifndef COMPONENT_STATE_H
#define COMPONENT_STATE_H
#include "component_state_config.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COMPONENT_STATE_UPDATE(component, state) component_state_update(component, state, __LINE__ / 2)

typedef void(*component_state_update_handler_t)(avionics_component_t component, avionics_component_state_t state, int line);

// Can be called multiple times, but will do nothing on subsequent calls
void component_state_start(component_state_update_handler_t handler, bool enable_messages);

// The macro above is the recommended way to call this - it will add the line number for you
void component_state_update(avionics_component_t component, avionics_component_state_t state, uint8_t line_number);

volatile const uint8_t* component_state_get_states(void);

avionics_component_state_t component_state_get_overall_state(void);

const char* component_state_get_name(avionics_component_t component);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENT_STATE_H */
