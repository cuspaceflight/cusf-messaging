cmake_minimum_required (VERSION 3.2)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(messaging)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(include)
add_subdirectory(src)

if (NOT DEFINED MESSAGING_OS)
	message(FATAL_ERROR "MESSAGING_OS not defined")
elseif (${MESSAGING_OS} STREQUAL std)
    add_subdirectory(os/std)
elseif (${MESSAGING_OS} STREQUAL chibios)
    add_subdirectory(os/chibios)
else (NOT DEFINED MESSAGING_OS)
    message(FATAL_ERROR "Unrecognised MESSAGING_OS")
endif (NOT DEFINED MESSAGING_OS)

add_library(messaging ${HEADERS} ${SOURCES})


message(STATUS "${ChibiOS_INCLUDE_DIRS}")
SET (MESSAGING_INCLUDES
        ${ChibiOS_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config
        ${OS_INCLUDES}
)

target_include_directories ( messaging PUBLIC
    ${MESSAGING_INCLUDES}
)