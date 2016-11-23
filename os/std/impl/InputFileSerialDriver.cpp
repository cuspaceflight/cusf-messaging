#include "InputFileSerialDriver.h"
#include "serial_interface.h"
#include "messaging.h"
#include "Utils.h"

#define READ_BUFFER_SIZE 255
#define WRITE_BUFFER_SIZE 255

static unsigned int read_buffer_index = 0;

static unsigned int read_buffer_limit = 0;
static uint8_t read_buffer[READ_BUFFER_SIZE];

static bool is_initialised = false;
static std::ifstream* s_stream = nullptr;

static uint8_t stream_get() {
    while (read_buffer_index >= read_buffer_limit) {
        if (s_stream == nullptr) {
            // Trigger termination of the read
            return 0x7E;
        }
        read_buffer_limit = (unsigned int) s_stream->readsome((char *) read_buffer, READ_BUFFER_SIZE);
        read_buffer_index = 0;
        if (read_buffer_index >= read_buffer_limit)
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    UtilAssert(read_buffer_index < read_buffer_limit, "Failed to fill read buffer!");
    return read_buffer[read_buffer_index++];
}

SERIAL_INTERFACE(serial_interface, stream_get, nullptr, nullptr, 1024);

static void writer_thread() {
    while (s_stream != nullptr) {
        telemetry_t* packet = serial_interface_next_packet(&serial_interface);
        if (packet != nullptr)
            messaging_send(packet, message_flags_dont_send_to_file);
    }
}

InputFileSerialDriver::InputFileSerialDriver(const char* filename) {
    UtilAssert(!is_initialised, "Only one serial driver can be active at once");

    input_stream_ = std::make_unique<std::ifstream>(filename, std::ifstream::binary | std::ifstream::in);
    read_buffer_index = 0;

    if (input_stream_ && *input_stream_) {

        is_initialised = true;
        s_stream = input_stream_.get();

        serial_interface_init(&serial_interface);

        thread_ = std::thread(writer_thread);
    }
}

InputFileSerialDriver::~InputFileSerialDriver() {
    if (!is_initialised)
        return; // If initialisation failed we don't have anything to clean up

    is_initialised = false;
    s_stream = nullptr;

    thread_.join();
}

bool InputFileSerialDriver::getConnected() {
    return is_initialised && input_stream_;
}
