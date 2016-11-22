add_subdirectory(include)
add_subdirectory(src)

if (NOT DEFINED MESSAGING_OS)
	message(FATAL_ERROR "MESSAGING_OS not defined")
elseif (${MESSAGING_OS} STREQUAL std)
    add_subdirectory(os/std)
    add_subdirectory(external/serial)
    set(OS_LIBS serial)
    set(OS_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/external/serial/include)
elseif (${MESSAGING_OS} STREQUAL chibios)
    add_subdirectory(os/chibios)
    set(OS_INCLUDES ${ChibiOS_INCLUDE_DIRS})
else (NOT DEFINED MESSAGING_OS)
    message(FATAL_ERROR "Unrecognised MESSAGING_OS")
endif (NOT DEFINED MESSAGING_OS)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Enable warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -Werror")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -Werror")
endif()

add_library(messaging ${HEADERS} ${SOURCES})

target_include_directories ( messaging PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/config
        ${OS_INCLUDES}
)

target_link_libraries(messaging PRIVATE ${OS_LIBS})