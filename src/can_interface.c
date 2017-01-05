#include "can_interface.h"
#include "messaging.h"
#include <string.h>
#include <avionics_config.h>
#include <component_state.h>
#include <can_interface.h>
#include "telemetry_packets.h"

typedef struct {
    uint16_t base_id;
    uint8_t size_in_bytes;
    uint8_t num_bits;

    uint8_t size_in_packets;
    uint16_t seqno_mask;
} multipacket_message_def_t;


#define MULTIPACKET_DEFINITION(_base_id_, _size_in_bytes, _num_bits) \
{\
    .base_id = _base_id_, \
    .size_in_bytes = _size_in_bytes, \
    .num_bits = _num_bits, \
    .size_in_packets = (uint8_t) ((_size_in_bytes + 7) / 8), \
    .seqno_mask = (uint16_t) ((1 << _num_bits) - 1)\
}

static const multipacket_message_def_t multipacket_message_definitions[NUM_MULTIPACKET_MESSAGES] = {
        MULTIPACKET_DEFINITION(telemetry_id_mpu9250_data, sizeof(mpu9250_data_t), 2),
        MULTIPACKET_DEFINITION(telemetry_id_mpu9250_config, sizeof(mpu9250_config_t), 2),
        MULTIPACKET_DEFINITION(telemetry_id_adis16405_config, sizeof(adis16405_config_t), 2),
        MULTIPACKET_DEFINITION(telemetry_id_adis16405_data, sizeof(adis16405_data_t), 2),
        MULTIPACKET_DEFINITION(telemetry_id_state_estimate_data, sizeof(state_estimate_t), 2),
};

static bool checkDefinitions(void) {
    for (int i = 0; i < NUM_MULTIPACKET_MESSAGES; i++) {
        const multipacket_message_def_t* def = &multipacket_message_definitions[i];
        if (def->size_in_packets > 4) {
            COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
            return false;
        }
        if (def->seqno_mask < def->size_in_packets - 1) {
            COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
            return false;
        }
    }
    return true;
}

static void resetMultipacketMessage(multipacket_message_buffer_t* msg) {
	for (int i = 0; i < 4; i++) {
		msg->is_valid[i] = false;
	}
	msg->current_origin = telemetry_origin_invalid;
}

static bool isMultipacketValid(const multipacket_message_def_t* def, multipacket_message_buffer_t* msg) {
	for (int i = 0; i < def->size_in_packets; i++)
		if (!msg->is_valid[i])
			return false;
	return true;
}

void can_interface_init(can_interface_t* id) {
    if (!checkDefinitions())
        return;

    telemetry_allocator_init(id->telemetry_allocator);

    COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_ok);
	for (int i = 0; i < NUM_MULTIPACKET_MESSAGES; i++)
		resetMultipacketMessage(&id->multipacket_message_buffers[i]);

    id->initialized = true;
}

static void handleFullPacket(can_interface_t* interface, uint16_t telemetry_id, telemetry_origin_t origin, uint8_t* data, uint8_t length, uint32_t timestamp) {
	telemetry_t* packet = telemetry_allocator_alloc(interface->telemetry_allocator, length);
	if (packet == NULL)
		return;
	memcpy(packet->payload, data, length);
	packet->header.id = telemetry_id;
	packet->header.length = length;
	packet->header.origin = origin;
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

	uint16_t id = (uint16_t) ((can_msg_id >> 5) & 0x3F);
	uint8_t origin = (uint8_t) (can_msg_id & 0x1F);

	// We don't understand packets from other sources
	if (origin != telemetry_origin_avionics_gui && origin != telemetry_origin_m3imu)
		return;

    int multipacket_index = getMultipacketIndex(id);
    if (multipacket_index < 0) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return;
    }

    multipacket_message_buffer_t* multipacket = &interface->multipacket_message_buffers[multipacket_index];
    const multipacket_message_def_t* def = &multipacket_message_definitions[multipacket_index];
    if (datalen <= 8) {
        handleFullPacket(interface, id, (telemetry_origin_t)origin, data, datalen, timestamp);
        return;
    }

	if (multipacket->current_origin == telemetry_origin_invalid) {
		multipacket->current_origin = (telemetry_origin_t) origin;
	} else if (multipacket->current_origin != origin) {
		COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
		return;
	}

	uint8_t seqno = (uint8_t) (id & def->seqno_mask);

	uint8_t* ptr = (uint8_t*)&multipacket->data_buffer;
	ptr += seqno * 8;

	if (seqno*8 + datalen > def->size_in_bytes) {
		COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
		return;
	}

	memcpy(ptr, data, datalen);

	multipacket->is_valid[seqno] = true;

	if (isMultipacketValid(def, multipacket)) {
		handleFullPacket(interface, def->base_id, multipacket->current_origin, multipacket->data_buffer, def->size_in_bytes, timestamp);
		resetMultipacketMessage(multipacket);
	}
}

bool can_interface_send(can_interface_t* interface, const telemetry_t* packet, message_metadata_t metadata) {
    (void)metadata;
    if (packet->header.origin == local_config.origin) {
        if (packet->header.length <= 8) {
			interface->can_send((packet->header.id << 5) | local_config.origin, false, packet->payload, packet->header.length);
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
        int remaining = packet->header.length;
        int i = 0;
        do {
			interface->can_send(((packet->header.id+i) << 5) | local_config.origin, false, ptr, remaining > 8 ? 8 : remaining);
            ptr += 8;
            i++;
            remaining -= 8;
        } while (remaining > 0);
    }
    return true;
}
