#include "usb_telemetry.h"
#include "impl/SerialDriver.h"
#include "impl/Utils.h"

static std::unique_ptr<SerialDriver> driver;

void usb_telemetry_start(void) {
    // TODO: Correct Port name
    driver = std::make_unique<SerialDriver>("/dev/serial/by-id/usb-CUSF_Spalax_100-if00", 38400);
}

void usb_telemetry_transmit_thread(void* arg) {
    // Handled by CanSerialDriver
}

void usb_telemetry_receive_thread(void* arg) {
    // Handled by CanSerialDriver
}

bool usb_telemetry_connected(void) {
    return driver->getConnected();
}