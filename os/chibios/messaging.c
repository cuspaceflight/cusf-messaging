#include "messaging.h"
#include "hal.h"
#include "chtypes.h"
#include <string.h>
#include "core_cmInstr.h"
#include "platform.h"
#include "component_state.h"
#include "avionics_config.h"

#define MAX_NUM_CONSUMERS 20
#define MAX_NUM_PRODUCERS 20

#define MAX_NUM_TELEMETRY_REFS 1000

typedef struct telemetry_ref_t {
	telemetry_t* packet;
    message_metadata_t flags;
	volatile uint32_t reference_count;
} telemetry_ref_t;

// This may be used for load tracking later on so is included
struct message_producer_impl_t {
	message_producer_t* parent;
};

struct message_consumer_impl_t {
	Mailbox mailbox;
	message_consumer_t* parent;
    volatile bool is_paused;
};

static volatile uint32_t cur_consumer_pool_index = 0;

static message_consumer_impl_t consumer_pool[MAX_NUM_CONSUMERS];

static Mutex consumer_register_mutex;

static volatile uint32_t cur_producer_pool_index = 0;

static message_producer_impl_t producer_pool[MAX_NUM_PRODUCERS];

static Mutex producer_register_mutex;

static volatile char telemetry_ref_memory_pool_buffer[MAX_NUM_TELEMETRY_REFS * sizeof(telemetry_ref_t)];

static MemoryPool telemetry_ref_memory_pool;

static volatile bool is_started = false;

// Defined in component_state.c
void component_state_register_with_messaging(void);

void messaging_start(void) {
	chPoolInit(&telemetry_ref_memory_pool, sizeof(telemetry_ref_t), NULL);
	chPoolLoadArray(&telemetry_ref_memory_pool, (void*)telemetry_ref_memory_pool_buffer, MAX_NUM_TELEMETRY_REFS);
	chMtxInit(&consumer_register_mutex);
	chMtxInit(&producer_register_mutex);

    memory_barrier_release();

    is_started = true;

    component_state_register_with_messaging();
    COMPONENT_STATE_UPDATE(avionics_component_messaging, state_ok);
}

bool messaging_started(void) {
    bool is_started_local = is_started;
    memory_barrier_acquire();
    return is_started_local;
}

//
// Telemetry Reference Counting System
//

static uint32_t safe_increment(volatile uint32_t *addr, uint32_t increment) {
	uint32_t new_value;
	do {
		new_value = __LDREXW(addr) + increment;
	} while (__STREXW(new_value, addr));
	return new_value;
}

static uint32_t safe_decrement(volatile uint32_t *addr, uint32_t decrement) {
	uint32_t new_value;
	do {
		new_value = __LDREXW(addr) - decrement;
	} while (__STREXW(new_value, addr));
	return new_value;
}

static telemetry_ref_t* telemetry_reference_create(void) {
	telemetry_ref_t* ref = chPoolAlloc(&telemetry_ref_memory_pool);
	if (ref == NULL)
		return NULL;
	ref->packet = 0;
    ref->flags = 0;
	ref->reference_count = 1;
	return ref;
}

static void telemetry_reference_retain(telemetry_ref_t* ref) {
	safe_increment(&ref->reference_count, 1);
}

static void telemetry_reference_release(telemetry_ref_t* ref) {
	if (safe_decrement(&ref->reference_count, 1) == 0) {
		telemetry_allocator_free(ref->packet);
        chPoolFree(&telemetry_ref_memory_pool, ref);
	}
}


//
// Messaging System Implementation
//

// Initialise a producer - returns false on error
bool messaging_producer_init(message_producer_t* producer) {
    chMtxLock(&producer_register_mutex);
	if (producer->impl != NULL) {
        chMtxUnlock(); // producer_register_mutex
		return true; // We assume it has already been initialised
    }
	if (cur_producer_pool_index >= MAX_NUM_PRODUCERS) {
		chMtxUnlock(); // producer_register_mutex
        COMPONENT_STATE_UPDATE(avionics_component_messaging, state_error);
		return false;
	}
	producer->impl = &producer_pool[cur_producer_pool_index];

	// Perform any initialisation
	producer->impl->parent = producer;
    telemetry_allocator_init(producer->telemetry_allocator);

    memory_barrier_release();

	// We don't need the thread safe version
	// As stores are atomic and we only modify within this lock zone
	cur_producer_pool_index++; // NB: this must be done after all initialization
	chMtxUnlock(); // producer_register_mutex
	return true;
}

// Initialise a consumer - returns false on error
bool messaging_consumer_init(message_consumer_t* consumer) {
    chMtxLock(&consumer_register_mutex);
	if (consumer->impl != NULL) {
        chMtxUnlock(); // consumer_register_mutex
		return true; // We assume it has already been initialised
    }

	if (cur_consumer_pool_index >= MAX_NUM_CONSUMERS) {
		chMtxUnlock(); // consumer_register_mutex
        COMPONENT_STATE_UPDATE(avionics_component_messaging, state_error);
		return false;
	}
	consumer->impl = &consumer_pool[cur_consumer_pool_index];
	consumer->impl->parent = consumer;

	// Perform any initialisation
	chMBInit(&consumer->impl->mailbox, (msg_t*)consumer->mailbox_buffer, consumer->mailbox_size);

    memory_barrier_release();

	// We don't need the thread safe version
	// As stores are atomic and we only modify within this lock zone
	cur_consumer_pool_index++; // NB: this must be done after all initialization
	chMtxUnlock(); // consumer_register_mutex
	return true;
}


static bool messaging_consumer_enqueue_packet(message_consumer_t* consumer, telemetry_ref_t* ref) {
	msg_t retval = chMBPost(&consumer->impl->mailbox, (intptr_t)ref, TIME_IMMEDIATE);
	if (retval == RDY_OK) {
		telemetry_reference_retain(ref);
		return true;
	}
	return false;
}

messaging_send_return_codes messaging_send(telemetry_t* packet, message_metadata_t flags) {
    telemetry_ref_t* ref = telemetry_reference_create();
	if (ref == NULL) {
        COMPONENT_STATE_UPDATE(avionics_component_messaging, state_error);
        telemetry_allocator_free(packet);
		return messaging_send_internal_pool_full;
    }
    ref->packet = packet;
    ref->flags = flags;

    bool enqueue_successful = true;
	// We create a local copy as it frees up the compiler
	// If a consumer register during this call it isn't a massive deal that
	// we won't pass it the packet
	uint32_t num_consumers = cur_consumer_pool_index;
    memory_barrier_acquire();

	for (uint32_t i = 0; i < num_consumers; ++i) {
		message_consumer_t* consumer = consumer_pool[i].parent;
		if (!consumer->impl->is_paused
            && (consumer->packet_source_mask & packet->header.id) == consumer->packet_source
            && (consumer->message_metadata_mask & flags) == consumer->message_metadata
		    && !messaging_consumer_enqueue_packet(consumer, ref)) {
			enqueue_successful = false;
        }
	}

	telemetry_reference_release(ref);
	return enqueue_successful ? messaging_send_ok : messaging_send_consumer_buffer_full;
}

// Send a mesage from the specified producer
// A copy of the data will be made, so you can freely modify/release the data after this call
messaging_send_return_codes messaging_producer_send(message_producer_t* producer, message_metadata_t flags, const uint8_t* data) {
	if (producer->impl == NULL) {
        COMPONENT_STATE_UPDATE(avionics_component_messaging, state_error);
		return messaging_send_invalid_producer;
    }

	telemetry_t* packet = telemetry_allocator_alloc(producer->telemetry_allocator, producer->payload_size);
	if (packet == NULL) {
        COMPONENT_STATE_UPDATE(avionics_component_messaging, state_error);
		return messaging_send_producer_heap_full;
	}

	memcpy(packet->payload, data, producer->payload_size);
    // We have already checked the tag and source don't overlap earlier
    packet->header.id = producer->packet_id;
    packet->header.length = producer->payload_size;
    packet->header.timestamp = platform_get_counter_value();
    packet->header.origin = local_config.origin;

    return messaging_send(packet, flags);
}

// Consume the next packet in the consumer's buffer
// If silent is specified will not invoke the callback function
messaging_receive_return_codes messaging_consumer_receive(message_consumer_t* consumer, bool blocking, bool silent) {
    if (consumer->impl == NULL) {
        COMPONENT_STATE_UPDATE(avionics_component_messaging, state_error);
        return messaging_receive_invalid_consumer;
    }

    intptr_t data_msg;
    msg_t mailbox_ret = chMBFetch(&(consumer->impl->mailbox), (msg_t*)&data_msg, blocking ? TIME_INFINITE : TIME_IMMEDIATE);
    if (mailbox_ret != RDY_OK || data_msg == 0)
        return messaging_receive_buffer_empty;

    telemetry_ref_t* ref = (telemetry_ref_t*)data_msg;
    if (!silent) {
        if (!consumer->consumer_func(ref->packet, ref->flags)) {
            telemetry_reference_release(ref);
            return messaging_receive_callback_error;
        }
    }

    telemetry_reference_release(ref);
    return messaging_receive_ok;
}

void messaging_pause_consumer(message_consumer_t* consumer, bool flush_buffer) {
    consumer->impl->is_paused = true;
    if (flush_buffer)
        while (messaging_consumer_receive(consumer, false, true) != messaging_receive_buffer_empty) {
            consumer->impl->is_paused = true;
        }
}

void messaging_resume_consumer(message_consumer_t* consumer) {
    consumer->impl->is_paused = false;
}
