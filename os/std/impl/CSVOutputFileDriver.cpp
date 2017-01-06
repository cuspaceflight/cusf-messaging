#include "CSVOutputFileDriver.h"
#include <can_interface.h>
#include <config/telemetry_packets.h>
#include <iostream>
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
static std::ostream* s_stream = nullptr;

static bool receive_packet(const telemetry_t* packet, message_metadata_t flags) {
    if (packet->header.id == ts_mpu9250_data) {
        mpu9250_data_t* data = (mpu9250_data_t*)packet->payload;
        *s_stream << "MPU9250Data,";

        *s_stream << data->accel[0] << ',';
        *s_stream << data->accel[1] << ',';
        *s_stream << data->accel[2] << ',';

        *s_stream << data->gyro[0] << ',';
        *s_stream << data->gyro[1] << ',';
        *s_stream << data->gyro[2] << ',';

        *s_stream << data->magno[0] << ',';
        *s_stream << data->magno[1] << ',';
        *s_stream << data->magno[2] << std::endl;
    }
    return true;
}


MESSAGING_CONSUMER(messaging_consumer, 0, 0, 0, message_flags_dont_send_to_file, receive_packet, 100);

static void reader_thread() {
    while (s_stream != nullptr) {
        messaging_consumer_receive(&messaging_consumer, true, false);
    }
}

CSVOutputFileDriver::CSVOutputFileDriver(const char* filename) {
    UtilAssert(!is_initialised, "Only one serial driver can be active at once");

    if (std::string("stdout.csv") != filename) {
        output_stream = std::make_unique<std::ofstream>(filename, std::ofstream::out);
        s_stream = output_stream.get();
    } else {
        s_stream = &std::cout;
    }

    if (s_stream && *s_stream) {
        is_initialised = true;

        messaging_consumer_init(&messaging_consumer);

        thread_ = std::thread(reader_thread);
    } else {
        s_stream = nullptr;
    }
}

CSVOutputFileDriver::~CSVOutputFileDriver() {
    if (!is_initialised)
        return; // If initialisation failed we don't have anything to clean up

    is_initialised = false;
    s_stream = nullptr;

    messaging_consumer_terminate(&messaging_consumer);

    thread_.join();
}

bool CSVOutputFileDriver::getConnected() {
    return is_initialised && s_stream && *s_stream;
}