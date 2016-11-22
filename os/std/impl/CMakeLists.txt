
set (SOURCES ${SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/CanSerialDriver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SerialDriver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/InputFileSerialDriver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OutputFileSerialDriver.cpp
    PARENT_SCOPE)

set (HEADERS ${HEADERS}
	${CMAKE_CURRENT_SOURCE_DIR}/TQueue.h
    PARENT_SCOPE)
