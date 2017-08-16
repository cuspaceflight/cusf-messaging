#include "can_interface.h"
#include <string.h>
#include <component_state.h>
#include <can_interface.h>
#include "telemetry_packets.h"

typedef struct {
    uint16_t telemetry_id;
    uint8_t size_in_bytes;
    uint8_t size_in_packets;

    uint8_t remapped_id;
    uint8_t remapped_board;
} can_mapping_t;


#define CAN_MAPPING(_telemetry_id_, _size_in_bytes, _remapped_id_, _remapped_board_) \
{\
    .telemetry_id = _telemetry_id_, \
    .size_in_bytes = _size_in_bytes, \
    .size_in_packets = (uint8_t) ((_size_in_bytes + 7) / 8), \
    .remapped_id = _remapped_id_, \
    .remapped_board = _remapped_board_ \
}

static const can_mapping_t can_mappings[NUM_CAN_MAPPINGS] = {
        CAN_MAPPING(ts_mpu9250_data, sizeof(mpu9250_data_t), 7, 5),
        CAN_MAPPING(ts_adis16405_data, sizeof(adis16405_data_t), 6, 5),
        CAN_MAPPING(ts_state_estimate_data, sizeof(state_estimate_t), 5, 5),
        CAN_MAPPING(ts_ms5611_data, sizeof(ms5611data_t), 4, 5),
        CAN_MAPPING(ts_component_state, sizeof(component_state_update_t), 0, 5),
};

bool can_interface_check_multipacket_definitions(void) {
    for (int i = 0; i < NUM_CAN_MAPPINGS; i++) {
        const can_mapping_t* def = &can_mappings[i];
        if (def->size_in_packets > MAX_SEQNO) {
            COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
            return false;
        }
    }
    return true;
}

static void resetMultipacketMessage(can_mapping_buffer_t* msg) {
	msg->valid_idx = 0;
}

static bool isMultipacketValid(const can_mapping_t* map, can_mapping_buffer_t* msg) {
	return map->size_in_packets == msg->valid_idx;
}

void can_interface_init(can_interface_t* id) {
    if (!can_interface_check_multipacket_definitions())
        return;

    telemetry_allocator_init(id->telemetry_allocator);

    COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_ok);
	for (int i = 0; i < NUM_CAN_MAPPINGS; i++)
		resetMultipacketMessage(&id->mapping_buffers[i]);

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

static int getMappingForTelemetryID(uint16_t telemetry_id) {
	for (int i = 0; i < NUM_CAN_MAPPINGS; i++) {
		if (telemetry_id == can_mappings[i].telemetry_id)
			return i;
	}
	return -1;
}

static int getMappingForCanID(uint16_t can_id) {
    uint8_t id = (uint8_t)((can_id >> 8) & 0b111);
    uint8_t board = (uint8_t)(can_id & 0b11111);
    for (int i = 0; i < NUM_CAN_MAPPINGS; i++) {
        if (id == can_mappings[i].remapped_id && board == can_mappings[i].remapped_board)
            return i;
    }
    return -1;
}

static uint16_t getCanID(uint16_t remapped_id, uint16_t seqno, uint16_t board) {
    return (remapped_id << 8) | (seqno << 5) | board;
}

static void handleUnrecognisedPacket(can_interface_t* interface, uint16_t can_msg_id, bool can_rtr, uint8_t *data, uint8_t datalen, uint32_t timestamp) {
    if (can_msg_id & ts_m3_can_mask) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return;
    }

    handleFullPacket(interface, can_msg_id | ts_m3_can, data, datalen, timestamp);
}

void can_interface_receive(can_interface_t* interface, uint16_t can_msg_id, bool can_rtr, uint8_t *data, uint8_t datalen, uint32_t timestamp) {
	(void)can_rtr;
	if (!interface->initialized)
		return;

    // There was a bug in the output for a while that would insert invalid empty packets - we ignore them
    if (datalen == 0)
        return;

    int mapping_idx = getMappingForCanID(can_msg_id);
    if (mapping_idx < 0) {
        handleUnrecognisedPacket(interface, can_msg_id, can_rtr, data, datalen, timestamp);
        return; // Ignore packets we don't know how to handle
    }

    can_mapping_buffer_t* mapping_buffer = &interface->mapping_buffers[mapping_idx];
    const can_mapping_t* map = &can_mappings[mapping_idx];

	uint8_t seqno = (uint8_t)((can_msg_id >> 5) & 0b111);

    if (seqno != mapping_buffer->valid_idx) {
        mapping_buffer->valid_idx = 0;
        return;
    }

	uint8_t* ptr = (uint8_t*)&mapping_buffer->data_buffer;
	ptr += seqno * 8;

    if (seqno >= map->size_in_packets) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return;
    }

	if (seqno*8 + datalen > map->size_in_bytes) {
		COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
		return;
	}

	memcpy(ptr, data, datalen);

	mapping_buffer->valid_idx++;

	if (isMultipacketValid(map, mapping_buffer)) {
		handleFullPacket(interface, map->telemetry_id, mapping_buffer->data_buffer, map->size_in_bytes, timestamp);
		resetMultipacketMessage(mapping_buffer);
	}
}

static bool sendWrappedPacket(can_interface_t* interface, const telemetry_t* packet, message_metadata_t metadata) {
    if (packet->header.length > 8) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return true;
    }

    interface->can_send(packet->header.id & (~ts_m3_can_mask), false, packet->payload, (uint8_t) packet->header.length);

    return true;
}

bool can_interface_send(can_interface_t* interface, const telemetry_t* packet, message_metadata_t metadata) {
    (void)metadata;

    if (packet->header.id & ts_m3_can_mask) {
        return sendWrappedPacket(interface, packet, metadata);
    }

    int mapping_idx = getMappingForTelemetryID(packet->header.id);
    if (mapping_idx < 0) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return true;
    }

    const can_mapping_t* map = &can_mappings[mapping_idx];

    if (map->size_in_bytes != packet->header.length) {
        COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
        return true;
    }

    uint8_t* ptr = packet->payload;
    uint8_t remaining = (uint8_t)packet->header.length;
    uint8_t i = 0;
    while (true) {
        if (i >= MAX_SEQNO) {
            COMPONENT_STATE_UPDATE(avionics_component_can_telemetry, state_error);
            return true;
        }

        interface->can_send(getCanID(map->remapped_id, i, map->remapped_board), false, ptr, remaining > 8 ? (uint8_t)8 : remaining);
        ptr += 8;
        i++;

        if (remaining <= 8)
            break;
        remaining -= 8;
    };
    return true;
}
bool can_interface_can_send(const telemetry_t *packet, message_metadata_t metadata) {
    return (metadata & message_flags_send_over_can) || (packet->header.id & ts_m3_can_mask);
}
