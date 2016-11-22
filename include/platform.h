#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdint.h>
#include "compilermacros.h"

#if defined MESSAGING_OS_STD
#define MEMORY_BUFFER_ATTRIBUTES
#elif defined MESSAGING_OS_CHIBIOS
#include "ch.h"
#define MEMORY_BUFFER_ATTRIBUTES __attribute__((aligned(sizeof(stkalign_t)))) __attribute__((section(".ram4")))
#else
#error Unrecognised Messaging OS
#endif

#ifdef __cplusplus
extern "C" {
#endif

void memory_barrier_acquire(void);

void memory_barrier_release(void);

uint32_t platform_get_counter_value(void);

void platform_set_counter_value(uint32_t value);

void platform_halt();

uint32_t platform_get_counter_frequency(void);

void platform_set_thread_name(const char* name);

void platform_thread_yield();

#ifdef __cplusplus
}
#endif

#endif /*PLATFORM_H*/
