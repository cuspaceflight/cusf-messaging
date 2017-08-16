#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H
#include <stdint.h>
#include "telemetry_allocator.h"
#include "messaging.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*can_interface_send_t)(uint16_t msg_id, bool can_rtr, uint8_t* data, uint8_t datalen);

#define NUM_CAN_MAPPINGS 5
#define MAX_SEQNO 8

typedef struct {
    uint8_t data_buffer[MAX_SEQNO * 8];
    int valid_idx;
} can_mapping_buffer_t;

typedef struct can_interface_t {
    const can_interface_send_t can_send;
    telemetry_allocator_t* const telemetry_allocator;
    bool initialized;
    can_mapping_buffer_t mapping_buffers[NUM_CAN_MAPPINGS];
} can_interface_t;

#define CAN_INTERFACE(name, can_send, heap_size) \
    TELEMETRY_ALLOCATOR(name##_allocator, heap_size) \
    static can_interface_t name = {can_send, &name##_allocator, 0};

bool can_interface_check_multipacket_definitions(void);

void can_interface_init(can_interface_t* interface);

bool can_interface_can_send(const telemetry_t* packet, message_metadata_t metadata);

bool can_interface_send(can_interface_t* interface, const telemetry_t* packet, message_metadata_t metadata);

void can_interface_receive(can_interface_t* id, uint16_t msg_id, bool can_rtr, uint8_t* data, uint8_t datalen, uint32_t timestamp);

#ifdef __cplusplus
}
#endif

#endif /* CAN_INTERFACE_H */
