#include "CSVOutputFileDriver.h"
#include <can_interface.h>
#include <config/telemetry_packets.h>
#include <iostream>
#include <component_state.h>
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


static bool is_running = false;
static std::ostream* s_stream = nullptr;

static void print_formats() {
    *s_stream << "StartFormatDescriptors" << std::endl;
    *s_stream << "MPU9250Data, Accel X, Accel Y, Accel Z, Gyro X, Gyro Y, Gyro Z, Magno X, Magno Y, Magno Z" << std::endl;
    *s_stream << "StateUpdate, Component String, Component #, State, Overall State, Line Number" << std::endl;
    *s_stream << "Ublox, Fix Type, Latitude, Longitude, Height, Height MSL" << std::endl;
    *s_stream << "MS5611, Temperature, Pressure" << std::endl;
    *s_stream << "EndFormatDescriptors" << std::endl;
}

static bool receive_packet(const telemetry_t* packet, message_metadata_t flags) {
    if (packet->header.id == ts_mpu9250_data) {
        auto data = telemetry_get_payload<mpu9250_data_t>(packet);
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
    } else if (packet->header.id == ts_component_state) {
        auto data = telemetry_get_payload<component_state_update_t>(packet);
        const char* component = component_state_get_name((avionics_component_t) data->component);
        *s_stream << "StateUpdate,";
        *s_stream << component << ',';
        *s_stream << (int)data->component << ',';
        *s_stream << (int)data->state << ',';
        *s_stream << (int)data->overall_state << ',';
        *s_stream << (int)data->line_number*2 << std::endl;
    } else if (packet->header.id == ts_ublox_nav) {
        auto data = telemetry_get_payload<ublox_nav_t>(packet);
        *s_stream << "Ublox,";
        *s_stream << (int)data->fix_type << ',';
        *s_stream << data->lat << ',';
        *s_stream << data->lon << ',';
        *s_stream << data->height << ',';
        *s_stream << data->h_msl << std::endl;
    } else if (packet->header.id == ts_ms5611_data) {
        auto data = telemetry_get_payload<ms5611data_t>(packet);
        *s_stream << "MS5611,";
        *s_stream << data->temperature << ',';
        *s_stream << data->pressure << std::endl;
    }
    return true;
}


MESSAGING_CONSUMER(messaging_consumer, 0, 0, 0, 0, receive_packet, 100);

static void reader_thread() {
    while (s_stream != nullptr && is_running) {
        messaging_consumer_receive(&messaging_consumer, true, false);
    }
    while(s_stream != nullptr && messaging_consumer_receive(&messaging_consumer, false, false) == messaging_receive_ok);
}

CSVOutputFileDriver::CSVOutputFileDriver(const char* filename) {
    UtilAssert(!is_running && !s_stream, "Only one serial driver can be active at once");

    if (std::string("stdout.csv") != filename) {
        output_stream = std::make_unique<std::ofstream>(filename, std::ofstream::out);
        s_stream = output_stream.get();
    } else {
        s_stream = &std::cout;
    }

    if (s_stream && *s_stream) {
        print_formats();
        is_running = true;

        messaging_consumer_init(&messaging_consumer);

        thread_ = std::thread(reader_thread);
    } else {
        s_stream = nullptr;
    }
}

CSVOutputFileDriver::~CSVOutputFileDriver() {
    if (!is_running)
        return; // If initialisation failed we don't have anything to clean up

    is_running = false;

    messaging_consumer_terminate(&messaging_consumer);

    thread_.join();

    s_stream = nullptr;
}

bool CSVOutputFileDriver::getConnected() {
    return is_running && s_stream && *s_stream;
}