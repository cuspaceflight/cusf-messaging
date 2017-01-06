#include "M3OutputFileDriver.h"
#include <can_interface.h>
#include "cpp_utils.h"

#define READ_BUFFER_SIZE 255
#define WRITE_BUFFER_SIZE 255

typedef struct {
    uint16_t ID;
    uint8_t RTR;
    uint8_t len;
    uint8_t data[8];
    uint32_t timestamp;
} __attribute__((packed)) DLPacket;



static bool is_initialised = false;
static std::ofstream* s_stream = nullptr;
static uint32_t last_timestamp = 0;


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
    s_stream->write((const char *) &packet, sizeof(packet));
}

CAN_INTERFACE(can_interface, can_send, 1024)


static bool receive_packet(const telemetry_t* packet, message_metadata_t flags) {
    last_timestamp = packet->header.timestamp;
    return can_interface_send(&can_interface, packet, flags);
}


MESSAGING_CONSUMER(messaging_consumer, 0, 0, 0, message_flags_dont_send_to_file, receive_packet, 100);

static void reader_thread() {
    while (s_stream != nullptr) {
        messaging_consumer_receive(&messaging_consumer, true, false);
    }
}

M3OutputFileDriver::M3OutputFileDriver(const char* filename) {
    UtilAssert(!is_initialised, "Only one serial driver can be active at once");

    output_stream = std::make_unique<std::ofstream>(filename, std::ofstream::binary | std::ofstream::out);

    if (output_stream && *output_stream) {

        is_initialised = true;
        s_stream = output_stream.get();

        messaging_consumer_init(&messaging_consumer);
        can_interface_init(&can_interface);

        thread_ = std::thread(reader_thread);
    }
}

M3OutputFileDriver::~M3OutputFileDriver() {
    if (!is_initialised)
        return; // If initialisation failed we don't have anything to clean up

    is_initialised = false;
    s_stream = nullptr;

    messaging_consumer_terminate(&messaging_consumer);

    thread_.join();
}

bool M3OutputFileDriver::getConnected() {
    return is_initialised && output_stream && *output_stream;
}
