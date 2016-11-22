set (SOURCES ${SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/messaging.c
		${CMAKE_CURRENT_SOURCE_DIR}/platform.c
		${CMAKE_CURRENT_SOURCE_DIR}/telemetry_allocator.c
		${CMAKE_CURRENT_SOURCE_DIR}/can_telemetry.c
        ${CMAKE_CURRENT_SOURCE_DIR}/usb_telemetry.c
        ${CMAKE_CURRENT_SOURCE_DIR}/usbcfg.c
    PARENT_SCOPE)

set (HEADERS ${HEADERS}
    PARENT_SCOPE)