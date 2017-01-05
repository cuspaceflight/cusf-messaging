#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H
#include <stdint.h>
#include "telemetry_allocator.h"
#include "messaging.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*can_interface_send_t)(uint16_t msg_id, bool can_rtr, uint8_t* data, uint8_t datalen);

#define NUM_MULTIPACKET_MESSAGES 6
typedef struct {
    uint8_t data_buffer[32];
    bool is_valid[4];
    telemetry_origin_t current_origin;
} multipacket_message_buffer_t;

typedef struct can_interface_t {
    const can_interface_send_t can_send;
    telemetry_allocator_t* const telemetry_allocator;
    bool initialized;
    multipacket_message_buffer_t multipacket_message_buffers[NUM_MULTIPACKET_MESSAGES];
} can_interface_t;

#define CAN_INTERFACE(name, can_send, heap_size) \
    TELEMETRY_ALLOCATOR(name##_allocator, heap_size) \
    static can_interface_t name = {can_send, &name##_allocator, 0};

void can_interface_init(can_interface_t* interface);

bool can_interface_send(can_interface_t* interface, const telemetry_t* packet, message_metadata_t metadata);

void can_interface_receive(can_interface_t* id, uint16_t msg_id, bool can_rtr, uint8_t* data, uint8_t datalen, uint32_t timestamp);

#ifdef __cplusplus
}
#endif

#endif /* CAN_INTERFACE_H */
