#ifndef MESSAGING_H
#define MESSAGING_H
#include "telemetry.h"
#include <stdbool.h>
#include "messaging_defs.h"
#include "platform.h"
#include "compilermacros.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MESSAGING_PRODUCER(name, packet_id, payload_size, max_num_packets) \
    TELEMETRY_ALLOCATOR(name##_allocator, (payload_size + sizeof(telemetry_t))*max_num_packets) \
    static message_producer_t name = {packet_id, payload_size, &name##_allocator, NULL};

#if defined MESSAGING_OS_STD
// On windows we don't bother allocating the buffer as it won't be used
#define MESSAGING_CONSUMER(name, packet_source, packet_source_mask, message_metadata, message_metadata_mask, consumer_func, mailbox_size) \
    static message_consumer_t name = {packet_source, packet_source_mask, message_metadata, message_metadata_mask, consumer_func, mailbox_size, NULL, NULL};
#elif defined MESSAGING_OS_CHIBIOS
#define MESSAGING_CONSUMER(name, packet_source, packet_source_mask, message_metadata, message_metadata_mask, consumer_func, mailbox_size) \
    static volatile msg_t name##_mailbox_buffer[mailbox_size] MEMORY_BUFFER_ATTRIBUTES; \
    static message_consumer_t name = {packet_source, packet_source_mask, message_metadata, message_metadata_mask, consumer_func, mailbox_size, name##_mailbox_buffer, NULL};
#else
#error Unrecognised Messaging OS
#endif

void messaging_start(void);

bool messaging_started(void);

// Initialise a producer - returns false on error
bool messaging_producer_init(message_producer_t* producer);

// Initialise a consumer - returns false on error
bool messaging_consumer_init(message_consumer_t* consumer);

// Send a mesage from the specified producer
// A copy of the data will be made, so you can freely modify/release the data after this call
messaging_send_return_codes messaging_producer_send(message_producer_t* producer, message_metadata_t flags, const uint8_t* data);

// Consume the next packet in the consumer's buffer
// If silent is specified will not invoke the callback function
// This function can be called recursively (e.g flushing buffer during callback)
// NB: A blocking call on a paused consumer will deadlock if no packets in its buffer.
messaging_receive_return_codes messaging_consumer_receive(message_consumer_t* consumer_id, bool blocking, bool silent);

// Pause a consumer - no further packets will be enqueued into its buffer until it is resumed
// If flush_buffer is specified all packets currently in the buffer will be
// silently dropped (without calling the callback function)
void messaging_pause_consumer(message_consumer_t* consumer, bool flush_buffer);

// Resume a consumer allowing it to receive packets again
void messaging_resume_consumer(message_consumer_t* consumer);

///
// Internal functions: USE WITH EXTREME CARE!
///

// messaging_producer_send is the reccomended method to send packets.
// This method is intended for predominantely internal use.
// NB: A copy will NOT be made of the provided packet - on calling this function
// ownership is transferred to the messaging system - you should not modify it
// or delete it after this call. It will be deleted for you when appropriate
// In the event of an error the packet will still be deleted
// The packet MUST have been allocated by the telemetry allocator component
messaging_send_return_codes messaging_send(telemetry_t* packet, message_metadata_t flags);

#ifdef __cplusplus
}
#endif

#endif /* MESSAGING_H */
