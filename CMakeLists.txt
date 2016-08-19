cmake_minimum_required (VERSION 3.2)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(messaging)

# Visual Studio misbehaves unless you set all of these
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

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

# We can use GLOB here as in order for a CMakeList to be used a pre-existing one
# must have been modified and therefore the build system will know to rebuild
FILE(GLOB_RECURSE CMAKELISTS */*CMakeLists.txt)

# Rebuild directory structure within Visual Studio
foreach(FILE ${SOURCES} ${HEADERS} ${CMAKELISTS})
  get_filename_component(PARENT_DIR "${FILE}" PATH)

  string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} "" GROUP "${PARENT_DIR}")
  string(REPLACE "/" "\\" GROUP "${GROUP}")

  source_group("${GROUP}" FILES "${FILE}")
endforeach()

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} ${ADDITIONAL_C_FLAGS})
add_library(messaging ${HEADERS} ${SOURCES} ${CMAKELISTS})

SET (MESSAGING_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config
	${OS_INCLUDES}
)

get_directory_property(hasParent PARENT_DIRECTORY)
if (hasParent)
	SET(MESSAGING_INCLUDES ${MESSAGING_INCLUDES} PARENT_SCOPE)
endif()

target_include_directories ( messaging PUBLIC
    ${MESSAGING_INCLUDES}
)
