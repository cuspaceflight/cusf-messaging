## Chibios Compilation

In order to compile you will need CMake v3.2 or higher installed and added to your path. To add the messaging library make the following modifications to your project's makefile.

First define the following near the top of your makefile

```make
MESSAGING = REL_PATH_TO_MESSAGING_ROOT_DIR
include $(MESSAGING)/messaging.mk
```

Then add $(MESSAGING_INCLUDES) includes to your include directories. For example

```make
INCDIR = $(PORTINC) $(KERNINC) $(TESTINC) \
         $(HALINC) $(PLATFORMINC) $(BOARDINC) \
         $(CHIBIOS)/os/various $(FATFSINC) $(MESSAGING_INCLUDES)
```

Then instruct the linker to link the library. For example

```make
# List the default directory to look for the libraries here
DLIBDIR = $(MESSAGING_LIB_DIR)

# List all default libraries here
DLIBS = -lmessaging -lm
```

Then after the line include $(RULESPATH)/rules.mk add the following to ensure it recompiles the library when needed.

```make
$(OBJS): build_messaging
clean: clean_messaging
```

Somewhere in your executable you will need to define the following struct.

```c
const avionics_config_t local_config = { ORIGIN, HANDLER };
```

where ORIGIN is the telemetry_origin_t of the local board and HANDLER is the error handling function. HANDLER should either be NULL or a pointer to a function with the below signature, to be invoked in case of an error.

```c
void error_handler(avionics_component_t component, int line)
```

Before using any of the library's functionality you must call the following in this order

```c
component_state_start();
checksum_init();
telemetry_allocator_start();
messaging_start();
```

