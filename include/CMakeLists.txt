add_subdirectory(config)

set (SOURCES ${SOURCES}
    PARENT_SCOPE)

set (HEADERS ${HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/compilermacros.h
        ${CMAKE_CURRENT_SOURCE_DIR}/messaging.h
        ${CMAKE_CURRENT_SOURCE_DIR}/messaging_defs.h
        ${CMAKE_CURRENT_SOURCE_DIR}/platform.h
        ${CMAKE_CURRENT_SOURCE_DIR}/telemetry.h
        ${CMAKE_CURRENT_SOURCE_DIR}/telemetry_allocator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/time_utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/checksum.h
        ${CMAKE_CURRENT_SOURCE_DIR}/serial_interface.h
        ${CMAKE_CURRENT_SOURCE_DIR}/component_state.h
        ${CMAKE_CURRENT_SOURCE_DIR}/can_interface.h
        ${CMAKE_CURRENT_SOURCE_DIR}/can_telemetry.h
        ${CMAKE_CURRENT_SOURCE_DIR}/usb_telemetry.h
        PARENT_SCOPE)
