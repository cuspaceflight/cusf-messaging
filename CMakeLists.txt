add_subdirectory(include)
add_subdirectory(src)

if (UNIX)
    add_subdirectory(os/std)
    add_subdirectory(external/serial)
    set(OS_LIBS serial)
    set(OS_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/external/serial/include)
elseif (DEFINED ChibiOS_INCLUDE_DIRS)
    add_subdirectory(os/chibios)
    set(OS_INCLUDES ${ChibiOS_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Platform not supported")
endif ()

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