#include "platform.h"
#include "hal.h"

uint32_t platform_get_counter_value() {
    return port_rt_get_counter_value();
}

uint32_t platform_get_counter_frequency() {
    return STM32_HCLK;
}

// Data Memory Barrier acts as a memory barrier. It ensures that all explicit
// memory accesses that appear in program order before the DMB instruction
// are observed before any explicit memory accesses that appear in program
// order after the DMB instruction.

inline void memory_barrier_acquire(void) {
    asm volatile("DMB" ::: "memory");
}


inline void memory_barrier_release(void) {
    asm volatile("DMB" ::: "memory");
}

void platform_set_thread_name(const char* name) {
    chRegSetThreadName(name);
}

void platform_thread_yield() {
    chThdYield();
}