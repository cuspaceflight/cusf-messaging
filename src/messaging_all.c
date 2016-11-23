#include <component_state.h>
#include <checksum.h>
#include <telemetry_allocator.h>
#include <messaging.h>
#include <usb_telemetry.h>
#include <can_telemetry.h>
#include "messaging_all.h"

void messaging_all_start(void) {
    component_state_start();
    checksum_init();
    telemetry_allocator_start();
    messaging_start();

#if USB_TELEMETRY_ENABLED
    usb_telemetry_start();
#endif

#if CAN_TELEMETRY_ENABLED
    can_telemetry_start();
#endif
}