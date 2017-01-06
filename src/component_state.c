#include "component_state.h"
#include "messaging.h"
#include "avionics_config.h"
#include "telemetry_packets.h"

STATIC_ASSERT(avionics_component_max < UINT8_MAX, too_many_avionics_components);
static volatile uint8_t component_states[avionics_component_max];

MESSAGING_PRODUCER(messaging_producer, ts_component_state, sizeof(component_state_update_t), 20);

static volatile bool is_started = false;
static volatile bool messaging_enabled = false;

void component_state_start(void) {
    for (int i = 0; i < avionics_component_max; i++)
        component_states[i] = state_ok;

    memory_barrier_release();

    is_started = true;
}

bool component_state_started(void) {
    bool is_started_local = is_started;
    memory_barrier_acquire();
    return is_started_local;
}

void component_state_register_with_messaging(void) {
    messaging_producer_init(&messaging_producer);
    memory_barrier_release();
    messaging_enabled = true;
}

void component_state_update(avionics_component_t component, avionics_component_state_t state, uint8_t line_number) {
    component_states[component] = state;

    bool messaging_enabled_local = messaging_enabled;
    memory_barrier_acquire();


    if (!local_config.mute_component_state && messaging_enabled_local && ((component != avionics_component_messaging && component != avionics_component_telemetry_allocator) || component_states[component] != state)) {
        // The if statement protects against potential error loops when
        // there is a problem in the messaging system:
        // error generated -> we send a packet -> an error is generated...
        component_state_update_t packet;
        packet.overall_state = (uint8_t)component_state_get_overall_state();
        packet.component = component;
        packet.state = state;
        packet.line_number = line_number;

        messaging_producer_send(&messaging_producer, message_flags_send_over_can, (uint8_t*)&packet);
    }

    if (local_config.state_update_handler != NULL)
        local_config.state_update_handler(component, state, line_number);
}

avionics_component_state_t component_state_get_overall_state(void) {
    avionics_component_state_t overall_state = state_ok;
    for (int i = 0; i < avionics_component_max; i++) {
        avionics_component_state_t state = (avionics_component_state_t) component_states[i];
        if (state == state_error) {
            overall_state = state_error;
            break;
        } else if (state == state_initializing) {
            overall_state = state_initializing;
        }
    }
    return overall_state;
}

volatile const uint8_t* component_state_get_states(void) {
    return component_states;
}