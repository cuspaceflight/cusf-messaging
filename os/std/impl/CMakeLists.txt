
set (SOURCES ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/CanSerialDriver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SerialDriver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/InputFileDriver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OutputFileDriver.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/M3InputFileDriver.cpp
    PARENT_SCOPE)

set (HEADERS ${HEADERS}
	${CMAKE_CURRENT_SOURCE_DIR}/TQueue.h
    PARENT_SCOPE)
