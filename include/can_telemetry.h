#ifndef CAN_TELEMETRY_H
#define CAN_TELEMETRY_H

#include <stdint.h>
#include "compilermacros.h"

#ifndef CAN_TELEMETRY_ENABLED
#if defined MESSAGING_OS_STD
#define CAN_TELEMETRY_ENABLED 1
#elif defined MESSAGING_OS_CHIBIOS

#include "hal.h"

#define CAN_TELEMETRY_ENABLED HAL_USE_CAN
#else
#error Unrecognised Messaging OS
#endif
#endif

#if CAN_TELEMETRY_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

void can_telemetry_start(void);

bool can_telemetry_connected(void);

#ifdef __cplusplus
}
#endif

#endif

#endif /* CAN_TELEMETRY_H */
