#include "component_state.h"
#include "checksum.h"
#include "telemetry_allocator.h"
#include "messaging.h"
#include "usb_telemetry.h"
#include "can_telemetry.h"
#include "messaging_all.h"
#include "file_telemetry.h"

void messaging_all_start(void) {
    messaging_all_start_options(true, true);
}

void messaging_all_start_options(bool can_telemetry_enabled, bool usb_telemetry_enabled) {
    component_state_start(NULL, true);
    checksum_init();
    telemetry_allocator_start();
    messaging_start();

#if USB_TELEMETRY_ENABLED
    if (usb_telemetry_enabled)
        usb_telemetry_start();
#endif

#if CAN_TELEMETRY_ENABLED
    if (can_telemetry_enabled)
        can_telemetry_start();
#endif
}
