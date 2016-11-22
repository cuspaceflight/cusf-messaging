#include "usb_telemetry.h"
#include "impl/SerialDriver.h"
#include "impl/Utils.h"

static std::unique_ptr<SerialDriver> driver;

void usb_telemetry_start(void) {
    if (driver)
        return;

    driver = std::make_unique<SerialDriver>("/dev/serial/by-id/usb-CUSF_Spalax_100-if00", 38400);
}

bool usb_telemetry_connected(void) {
    return driver != nullptr && driver->getConnected();
}