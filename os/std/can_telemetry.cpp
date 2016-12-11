#include "can_telemetry.h"
#include "impl/CanSerialDriver.h"
#include "cpp_utils.h"

#if CAN_TELEMETRY_ENABLED

static std::unique_ptr<CanSerialDriver> driver;

void can_telemetry_start(void) {
    if (driver)
        return;

    // TODO: Correct Port name
    driver = std::make_unique<CanSerialDriver>("/dev/serial/by-id/usb-Black_Sphere_Technologies_Black_Magic_Probe__m3debug____Firmware_v1.6-rc0-177-ge5584c4-di_66968A95-if02", 38400);
}

bool can_telemetry_connected(void) {
    return driver != nullptr && driver->getConnected();
}

#endif