#ifndef USB_TELEMETRY_H
#define USB_TELEMETRY_H
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void usb_telemetry_start(void);

bool usb_telemetry_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_TELEMETRY_H */
