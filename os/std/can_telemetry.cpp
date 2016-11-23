#include "can_telemetry.h"
#include "impl/CanSerialDriver.h"
#include "impl/Utils.h"

#if CAN_TELEMETRY_ENABLED

static std::unique_ptr<CanSerialDriver> driver;

void can_telemetry_start(void) {
    if (driver)
        return;

    // TODO: Correct Port name
    driver = std::make_unique<CanSerialDriver>("COM3", 38400);
}

bool can_telemetry_connected(void) {
    return driver != nullptr && driver->getConnected();
}

#endif