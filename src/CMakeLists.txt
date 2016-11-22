set (SOURCES ${SOURCES}
		${CMAKE_CURRENT_SOURCE_DIR}/time_utils.c
		${CMAKE_CURRENT_SOURCE_DIR}/component_state.c
		${CMAKE_CURRENT_SOURCE_DIR}/checksum.c
		${CMAKE_CURRENT_SOURCE_DIR}/serial_interface.c
		${CMAKE_CURRENT_SOURCE_DIR}/can_interface.c
		${CMAKE_CURRENT_SOURCE_DIR}/can_telemetry.c
    PARENT_SCOPE)

set (HEADERS ${HEADERS}
    PARENT_SCOPE)
