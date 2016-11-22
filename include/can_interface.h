#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H
#include <stdint.h>
#include "telemetry_allocator.h"
#include "messaging.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*can_interface_send_t)(uint16_t msg_id, bool can_rtr, uint8_t* data, uint8_t datalen);

typedef struct can_interface_t {
    const can_interface_send_t can_send;
} can_interface_t;

void can_interface_init(can_interface_t* interface);

bool can_interface_send(can_interface_t* interface, const telemetry_t* packet, message_metadata_t metadata);

void can_interface_receive(can_interface_t* id, uint16_t msg_id, bool can_rtr, uint8_t* data, uint8_t datalen);

#ifdef __cplusplus
}
#endif

#endif /* CAN_INTERFACE_H */
