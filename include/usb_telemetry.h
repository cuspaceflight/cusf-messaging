#ifndef USB_TELEMETRY_H
#define USB_TELEMETRY_H

#include <stdint.h>
#include <stdbool.h>
#include "compilermacros.h"

#ifndef USB_TELEMETRY_ENABLED
#if defined MESSAGING_OS_STD
#define USB_TELEMETRY_ENABLED 1
#elif defined MESSAGING_OS_CHIBIOS

#include "hal.h"

#define USB_TELEMETRY_ENABLED HAL_USE_USB
#else
#error Unrecognised Messaging OS
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if USB_TELEMETRY_ENABLED

void usb_telemetry_start(void);

bool usb_telemetry_connected(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* USB_TELEMETRY_H */
