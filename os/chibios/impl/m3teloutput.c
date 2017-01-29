// Heavily based on https://github.com/cuspaceflight/m3-avionics/blob/master/m3dl/firmware/logging.c

#include <can_interface.h>
#include <component_state.h>
#include "m3teloutput.h"

#if FILE_TELEMETRY_OUTPUT_ENABLED
#include "microsd.h"

typedef struct {
    uint16_t ID;
    uint8_t RTR;
    uint8_t len;
    uint8_t data[8];
    uint32_t timestamp;
} __attribute__((packed)) DLPacket;

#define BUFFER_SIZE 16384 / sizeof(DLPacket)

static DLPacket packet_buffer[BUFFER_SIZE];
static int buffer_index = 0;
static uint32_t last_timestamp = 0;

static SDFILE file;

static SDRESULT open_file(void) {
    static SDFS file_system;
    return microsd_open_file_inc(&file, "log", "m3tel", &file_system);
}

static void flush_buffer(void) {
    SDRESULT write_res = microsd_write(&file, (char*)packet_buffer, sizeof(packet_buffer));
    while (write_res != FR_OK) {
        microsd_close_file(&file);

        if(open_file() == FR_OK) {
            COMPONENT_STATE_UPDATE(avionics_component_file_telemetry_output, state_ok);
            write_res = microsd_write(&file, (char*)packet_buffer, sizeof(packet_buffer));
        }
        COMPONENT_STATE_UPDATE(avionics_component_file_telemetry_output, state_error);
        chThdSleepSeconds(1);
    }
    buffer_index = 0;
}

static void can_send(uint16_t msg_id, bool can_rtr, uint8_t* data, uint8_t datalen) {
    DLPacket packet;
    packet.ID = msg_id;
    packet.RTR = (uint8_t)can_rtr;
    packet.len = datalen;
    packet.timestamp = last_timestamp;

    int i;
    for (i = 0; i < datalen; i++) {
        packet.data[i] = data[i];
    }
    for (; i < 8; i++) {
        packet.data[i] = 0;
    }

    packet_buffer[buffer_index++] = packet;
    if (buffer_index >= BUFFER_SIZE)
        flush_buffer();
}

// As we don't use this for receiving, we don't need a large buffer, we use a small value as some
// compilers have a problem with 0 sized arrays
CAN_INTERFACE(can_interface, can_send, 4)

static bool receive_packet(const telemetry_t* packet, message_metadata_t flags) {
    last_timestamp = packet->header.timestamp;
    return can_interface_send(&can_interface, packet, flags);
}

MESSAGING_CONSUMER(messaging_consumer, 0, 0, 0, 0, receive_packet, 100);

void m3tel_output_thread(void* arg) {
    // TODO: Use filename
    // const char* filename = arg;

    COMPONENT_STATE_UPDATE(avionics_component_file_telemetry_output, state_initializing);
    while (open_file() != FR_OK) {
        COMPONENT_STATE_UPDATE(avionics_component_file_telemetry_output, state_error);
        chThdSleepSeconds(1);
    }
    COMPONENT_STATE_UPDATE(avionics_component_file_telemetry_output, state_initializing);

    can_interface_init(&can_interface);
    messaging_consumer_init(&messaging_consumer);

    COMPONENT_STATE_UPDATE(avionics_component_file_telemetry_output, state_ok);

    while (true) {
        messaging_consumer_receive(&messaging_consumer, true, false);
        chThdYield();
    }
}

#endif