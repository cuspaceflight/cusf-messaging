#include "component_state.h"
#include "messaging.h"
#include "telemetry_packets.h"

STATIC_ASSERT(avionics_component_max < UINT8_MAX, too_many_avionics_components);
static volatile uint8_t component_states[avionics_component_max];

MESSAGING_PRODUCER(messaging_producer, ts_component_state, sizeof(component_state_update_t), 20);

static volatile bool is_started = false;
static volatile bool registered_with_messaging = false;

static bool messaging_enabled = true;
static component_state_update_handler_t update_handler = NULL;

void component_state_start(component_state_update_handler_t handler, bool enable_messages) {
    if (is_started)
        return;

    for (int i = 0; i < avionics_component_max; i++)
        component_states[i] = state_ok;

    memory_barrier_release();

    update_handler = handler;

    is_started = true;
}

void component_state_register_with_messaging(void) {
    if (messaging_enabled) {
        messaging_producer_init(&messaging_producer);
        memory_barrier_release();
        registered_with_messaging = true;
    }
}

void component_state_update(avionics_component_t component, avionics_component_state_t state, uint8_t line_number) {
    component_states[component] = state;

    bool messaging_enabled_local = registered_with_messaging;
    memory_barrier_acquire();


    if (messaging_enabled_local && ((component != avionics_component_messaging && component != avionics_component_telemetry_allocator) || component_states[component] != state)) {
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

    if (update_handler != NULL)
        update_handler(component, state, line_number);
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

const char *component_state_get_name(avionics_component_t component) {
    switch (component) {
        case avionics_component_messaging:
            return "Messaging";
        case avionics_component_telemetry_allocator:
            return "Telemetry Allocator";
        case avionics_component_adis16405:
            return "ADIS16405";
        case avionics_component_mpu9250:
            return "MPU9250";
        case avionics_component_ms5611:
            return "MS5611";
        case avionics_component_can_telemetry:
            return "Can Telemetry";
        case avionics_component_world_mag_model:
            return "World Magnetic Model";
        case avionics_component_ublox:
            return "Ublox";
        default:
            return "Unknown Component";
    }
}
