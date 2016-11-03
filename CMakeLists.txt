if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Enable warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Werror")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -Werror")
endif()

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