#include <can_interface.h>
#include "M3InputFileDriver.h"
#include "messaging.h"
#include "cpp_utils.h"

static volatile bool is_initialised = false;
static std::ifstream* s_stream = nullptr;

typedef struct {
    uint16_t ID;
    uint8_t RTR;
    uint8_t len;
    uint8_t data[8];
    uint32_t timestamp;
} __attribute__((packed)) DLPacket;

CAN_INTERFACE(can_interface, NULL, 1024)

static void writer_thread() {
    while (s_stream && *s_stream) {
        DLPacket packet;
        s_stream->read((char *)&packet, sizeof(packet));
        can_interface_receive(&can_interface, packet.ID, packet.RTR, packet.data, packet.len, packet.timestamp);
    }
}

M3InputFileDriver::M3InputFileDriver(const char* filename) {
    UtilAssert(!is_initialised, "Only one serial driver can be active at once");

    can_interface_init(&can_interface);

    input_stream_ = std::make_unique<std::ifstream>(filename, std::ifstream::binary | std::ifstream::in);

    if (input_stream_ && *input_stream_) {

        is_initialised = true;
        s_stream = input_stream_.get();

        thread_ = std::thread(writer_thread);
    }
}

M3InputFileDriver::~M3InputFileDriver() {
    if (!is_initialised)
        return; // If initialisation failed we don't have anything to clean up

    is_initialised = false;
    s_stream = nullptr;

    thread_.join();
}

bool M3InputFileDriver::getConnected() {
    return is_initialised && input_stream_ && *input_stream_;
}
