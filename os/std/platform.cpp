#include "platform.h"
#include <atomic>
#include <stdlib.h>
#include <thread>

uint32_t counter_value = 0;

extern "C" void platform_set_counter_value(uint32_t t) {
    counter_value = t;
}

extern "C" uint32_t platform_get_counter_value() {
    return counter_value;
}

extern "C" uint32_t platform_get_counter_frequency() {
    return 168000000;
}

extern "C" void memory_barrier_acquire(void) {
    std::atomic_thread_fence(std::memory_order_acquire);
}

extern "C" void memory_barrier_release(void) {
    std::atomic_thread_fence(std::memory_order_release);
}

void platform_set_thread_name(const char* name) {
    
}

void platform_thread_yield() {
    std::this_thread::yield();
}