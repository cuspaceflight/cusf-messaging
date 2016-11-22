#include "can_telemetry.h"
#include "impl/CanSerialDriver.h"
#include "impl/Utils.h"

static std::unique_ptr<CanSerialDriver> driver;

void can_telemetry_start(void) {
    // TODO: Correct Port name
    driver = std::make_unique<CanSerialDriver>("COM3", 38400);
}

void can_telemetry_transmit_thread(void* arg) {
    // Handled by CanSerialDriver
}

void can_telemetry_receive_thread(void* arg) {
    // Handled by CanSerialDriver
}