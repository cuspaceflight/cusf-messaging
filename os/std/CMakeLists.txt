
add_subdirectory(impl)

set (SOURCES ${SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/messaging.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/platform.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/telemetry_allocator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/can_telemetry.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/usb_telemetry.cpp
    PARENT_SCOPE)

set (HEADERS ${HEADERS}
    PARENT_SCOPE)
