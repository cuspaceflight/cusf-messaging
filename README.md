# cusf-messaging
A messaging system allowing seamless inter and intra board communication between software components. 

## Installation

See [INSTALL.md](INSTALL.md) for compilation instructions.

## Configuration

The config directory contains a number of files that are used to configure the various packet assignments, etc...

- [avionics_config.h](include/config/avionics_config.h): contains a struct declaration that must be defined somewhere in your executable - see [INSTALL.md](INSTALL.md) for more info

- [component_state_config.h](include/config/component_state_config.h): contains the avionics components to be tracked by the Component State component

- [messaging_config.h](include/config/messaging_config.h): defines the meaning of the various message metadata bits

- [telemetry_config.h](include/config/telemetry_config.h): defines the various packet origins along with various packet ids (TELEMETRY_ID). It also defines various telemetry sources (TELEMETRY_SOURCE) which group together various packet ids.

## Components

The library contains a number of components which users can mix and match as required. It is reccomended you read [INSTALL.md](INSTALL.md) before trying to get the library to work in your project.

### Telemetry Component

A variable sized packet system, complete with an allocator system that facilitates allocating said packets from statically allocated buffers.

To create a telemetry allocator, use the following macro within a source file.

```c
TELEMETRY_ALLOCATOR(allocator_name, heap_size);
```

where allocator_name is the name to give the static variable and heap_size is the number of bytes storage to allocate.

Before you use the allocator you must call

```c
telemetry_allocator_init(&allocator_name);
```

You can then allocate a packet using

```c
telemetry_t* packet = telemetry_allocator_alloc(&allocator_name, payload_size);
```

where payload_size is the size of the desired packets payload.

Allocated packets are freed using the following

```c
telemetry_allocator_free(packet);
```


### Messaging Component

The messaging component builds on top of the Telemetry Component to provide seamless inter and intra board communication between software components.

Messages are produced by messaging producers and consumed by messaging consumers without any coupling between the two.

#### Producer

A messaging producer is defined using

```c
MESSAGING_PRODUCER(producer_name, packet_id, payload_size, max_num_packets);
```

where:
- producer_name is the name to give the static variable
- packet_id is the id of the packet to allocate
- payload_size is the size of the packet's payload
- max_num_packets is the maximum number of packets that can be in existance in the messaging system at once

A small number (~15) should be sufficient for max_num_packets in all but the most high data rate (~1kHz) situations.

Before using the producer one must call

```c
messaging_producer_init(&producer_name);
```

One can then send data with

```c
messaging_producer_send(&producer_name, message_metadata, data);
```

Where message_metadata is for filtering (discussed later) and data is the data to send. The contents of data are copied by the messaging system and so the memory can be freed/reused after this call.

#### Consumer

A messaging consumer is declared using

```c
MESSAGING_CONSUMER(consumer_name, packet_source, packet_source_mask, message_metadata, message_metadata_mask, consumer_func, mailbox_size)
```

where:

- consumer_name is the name of the static variable
- packet_source is the packet source to receive packets from
- packet_source_mask is the packet source mask to receive packets from (specifies which bits of a packet's id to compare for equality with packet_source)
- message_metadata specifies the desired packet metadata to receive packets matching
- message_metadata_mask specifies the packet metadata bits to compare for equality with message_metadata
- consumer_func is the consumer function (see later)
- mailbox_size is the maximum number of packets that can be enqueued to this consumer

For most applicaations message_metadata and message_metatadata_mask can both be 0 - i.e. don't perform any metadata based filtering. They are currently only used to limit high data rate producers from swamping USB and CAN connections.

A small number (~15) should be sufficient for mailbox_size in all but the most high data rate (~1kHz) situations.

As before this must be initialized before it can be used using 

```c
messaging_consumer_init(&consumer_name);

consumer_func should be a function with the following signature

```c
bool consumer_func(const telemetry_t* packet, message_metadata_t metadata)
```

To process packets one should frequently call

```c
messaging_consumer_receive(&consumer_name, blocking, silent)
```

where blocking specifies whether to block waiting for packets and silent specifies whether to just drop and not process packets.

Assuming silent is not true and there is a packet waiting a call to messaging_consumer_receive will result in a single call to consumer_func with the packet at the head of the queue.

The return code of messaging_consumer_receive will indicate if 
- a packet was processed
- no packet was waiting (will only occur if blocking=false)
- consumer_func returned false


### Component State Component

The Component State component tracks the 'health' of various software components in the system. Each component can be in one of three states - state_ok, state_initializing and state_error. 

The state of the various components is stored in an array which can be obtained using

```c
component_state_get_states();
```

One can set the state of a component using

```c
COMPONENT_STATE_UPDATE(component,state)
```

This will automatically obtain the line number of the call to aid debugging.

State updates are sent out using the messaging system, alternatively one can register an error handler function - see [INSTALL.md](INSTALL.md) for more info.


