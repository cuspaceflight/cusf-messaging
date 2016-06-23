# cusf-messaging
A messaging system allowing seamless inter and intra board communication between software components

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


