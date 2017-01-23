#include "OutputFileDriver.h"
#include "serial_interface.h"
#include "messaging.h"
#include "cpp_utils.h"

#define READ_BUFFER_SIZE 255
#define WRITE_BUFFER_SIZE 255

static unsigned int write_buffer_index = 0;

static uint8_t write_buffer[WRITE_BUFFER_SIZE];

static bool is_running = false;
static std::ofstream* s_stream = nullptr;


static bool stream_put(uint8_t byte) {
    if (write_buffer_index >= WRITE_BUFFER_SIZE) {
        s_stream->write((const char *)write_buffer, WRITE_BUFFER_SIZE);
        write_buffer_index = 0;
    }
    write_buffer[write_buffer_index++] = byte;
    return true;
}

static bool stream_flush() {
    s_stream->write((const char*)write_buffer, write_buffer_index);
    return true;
}


SERIAL_INTERFACE(serial_interface, nullptr, stream_put, stream_flush, 1024);

static bool receive_packet(const telemetry_t* packet, message_metadata_t flags) {
    return serial_interface_send_packet(&serial_interface, packet);
}


MESSAGING_CONSUMER(messaging_consumer, 0, 0, 0, 0, receive_packet, 100);

static void reader_thread() {
    while (s_stream != nullptr && is_running) {
        messaging_consumer_receive(&messaging_consumer, true, false);
    }
    while(s_stream != nullptr && messaging_consumer_receive(&messaging_consumer, false, false) == messaging_receive_ok);
}

OutputFileDriver::OutputFileDriver(const char* filename) {
    UtilAssert(!is_running && !s_stream, "Only one serial driver can be active at once");

    output_stream = std::make_unique<std::ofstream>(filename, std::ofstream::binary | std::ofstream::out);
    write_buffer_index = 0;

    if (output_stream && *output_stream) {
        is_running = true;
        s_stream = output_stream.get();

        messaging_consumer_init(&messaging_consumer);
        serial_interface_init(&serial_interface);

        thread_ = std::thread(reader_thread);
    }
}

OutputFileDriver::~OutputFileDriver() {
    if (!is_running)
        return; // If initialisation failed we don't have anything to clean up

    is_running = false;

    messaging_consumer_terminate(&messaging_consumer);

    thread_.join();

    s_stream = nullptr;
}

bool OutputFileDriver::getConnected() {
    return is_running && output_stream && *output_stream;
}
