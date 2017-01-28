#include "can_interface.h"
#include <string.h>
#include <component_state.h>
#include <can_interface.h>
#include "telemetry_packets.h"

typedef struct {
    uint16_t base_id;
    uint8_t size_in_bytes;
    uint8_t suffix_length;

    uint8_t size_in_packets;
    uint16_t seqno_mask;
} multipacket_message_def_t;


#define MULTIPACKET_DEFINITION(_base_id_, _size_in_bytes) \
{\
    .base_id = _base_id_, \
    .size_in_bytes = _size_in_bytes, \
    .size_in_packets = (uint8_t) ((_size_in_bytes + 7) / 8), \
    .seqno_mask = ~_base_id_##_mask, \
    .suffix_length = _base_id_##_suffix_length \
}

static const multipacket_message_def_t multipacket_message_definitions[NUM_MULTIPACKET_MESSAGES] = {
        MULTIPACKET_DEFINITION(ts_mpu9250_data, sizeof(mpu9250_data_t)),
        MULTIPACKET_DEFINITION(ts_adis16405_data, sizeof(adis16405_data_t)),
        MULTIPACKET_DEFINITION(ts_state_estimate_data, sizeof(state_estimate_t)),
        MULTIPACKET_DEFINITION(ts_ublox_nav, sizeof(ublox_nav_t)),
        MULTIPACKET_DEFINITION(ts_state_estimate_data, sizeof(state_estimate_t)),
};

bool can_interface_check_multipacket_definitions(void) {
    for (int i = 0; i < NUM_MULTIPACKET_MESSAGES; i++) {
        const multipacket_message_def_t* def = &multipacket_message_definitions[i];
        if (def->size_in_packets > MAX_SEQNO) {
            COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
            return false;
        }
        if (((def->size_in_packets - 1) << def->suffix_length) > 0b11111111111) {
            COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
            return false;
        }
    }
    return true;
}

static void resetMultipacketMessage(multipacket_message_buffer_t* msg) {
	for (int i = 0; i < MAX_SEQNO; i++) {
		msg->is_valid[i] = false;
	}
}

static bool isMultipacketValid(const multipacket_message_def_t* def, multipacket_message_buffer_t* msg) {
	for (int i = 0; i < def->size_in_packets; i++)
		if (!msg->is_valid[i])
			return false;
	return true;
}

void can_interface_init(can_interface_t* id) {
    if (!can_interface_check_multipacket_definitions())
        return;

    telemetry_allocator_init(id->telemetry_allocator);

    COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_ok);
	for (int i = 0; i < NUM_MULTIPACKET_MESSAGES; i++)
		resetMultipacketMessage(&id->multipacket_message_buffers[i]);

    id->initialized = true;
}

static void handleFullPacket(can_interface_t* interface, uint16_t telemetry_id, uint8_t* data, uint8_t length, uint32_t timestamp) {
	telemetry_t* packet = telemetry_allocator_alloc(interface->telemetry_allocator, length);
	if (packet == NULL)
		return;
	memcpy(packet->payload, data, length);
	packet->header.id = telemetry_id;
	packet->header.length = length;
    packet->header.timestamp = timestamp;
	messaging_send(packet, 0);
}

static int getMultipacketIndex(uint16_t telemetry_id) {
	for (int i = 0; i < NUM_MULTIPACKET_MESSAGES; i++) {
		uint16_t mask = ~(multipacket_message_definitions[i].seqno_mask);
		if ((telemetry_id & mask) == multipacket_message_definitions[i].base_id)
			return i;
	}
	return -1;
}

void can_interface_receive(can_interface_t* interface, uint16_t can_msg_id, bool can_rtr, uint8_t *data, uint8_t datalen, uint32_t timestamp) {
	(void)can_rtr;
	if (!interface->initialized)
		return;

    // There was a bug in the output for a while that would insert invalid empty packets - we ignore them
    if (datalen == 0)
        return;

    int multipacket_index = getMultipacketIndex(can_msg_id);
    if (multipacket_index < 0) {
        handleFullPacket(interface, can_msg_id, data, datalen, timestamp);
        return;
    }

    multipacket_message_buffer_t* multipacket = &interface->multipacket_message_buffers[multipacket_index];
    const multipacket_message_def_t* def = &multipacket_message_definitions[multipacket_index];

	uint8_t seqno = (uint8_t) ((can_msg_id & def->seqno_mask) >> def->suffix_length);

	uint8_t* ptr = (uint8_t*)&multipacket->data_buffer;
	ptr += seqno * 8;

    if (seqno >= def->size_in_packets) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return;
    }

	if (seqno*8 + datalen > def->size_in_bytes) {
		COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
		return;
	}

    if (multipacket->is_valid[seqno]) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return;
    }

	memcpy(ptr, data, datalen);

	multipacket->is_valid[seqno] = true;

	if (isMultipacketValid(def, multipacket)) {
		handleFullPacket(interface, def->base_id, multipacket->data_buffer, def->size_in_bytes, timestamp);
		resetMultipacketMessage(multipacket);
	}
}

bool can_interface_send(can_interface_t* interface, const telemetry_t* packet, message_metadata_t metadata) {
    (void)metadata;
    if (packet->header.length <= 8) {
        interface->can_send(packet->header.id, false, packet->payload, packet->header.length);
        return true;
    }

    int multipacket_index = getMultipacketIndex(packet->header.id);
    if (multipacket_index < 0) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return true;
    }

    const multipacket_message_def_t* def = &multipacket_message_definitions[multipacket_index];

    if (def->size_in_bytes != packet->header.length) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return true;
    }

    uint8_t* ptr = packet->payload;
    uint8_t remaining = packet->header.length;
    int i = 0;
    while (true) {
        if ((i << def->suffix_length) & ~def->seqno_mask || (i << def->suffix_length) > 0b11111111111) {
            COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
            return true;
        }

        interface->can_send((uint16_t) (packet->header.id | i << def->suffix_length), false, ptr, remaining > 8 ? (uint8_t)8 : remaining);
        ptr += 8;
        i++;

        if (remaining <= 8)
            break;
        remaining -= 8;
    };
    return true;
}
