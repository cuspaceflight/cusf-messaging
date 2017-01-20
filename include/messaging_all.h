#ifndef MESSAGING_ALL_H
#define MESSAGING_ALL_H
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void messaging_all_start(void);

void messaging_all_start_options(bool can_telemetry_enabled, bool usb_telemetry_enabled);

#ifdef __cplusplus
}
#endif

#endif /* MESSAGING_ALL_H */
