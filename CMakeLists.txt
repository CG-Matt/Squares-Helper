cmake_minimum_required(VERSION 3.18)

# Set the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Name the project
project(SQUARESHELPER)

# Use different source files depending on platform
if(NOT WIN32)
    set(PLATFORM_SOURCES "src/webAgent-unix.c")
else()
    set(PLATFORM_SOURCES "src/webAgent-win.c")
endif()

# List all source files
set(COMMON_SOURCES
    "src/CGMString.c"
    "src/fileManager.c"
    "src/hashSet.c"
    "src/multithreading.c"
    "src/main.c"
)

# List all header files
set(HEADERS
    "src/CGMString.h"
    "src/fileManager.h"
    "src/hashSet.h"
    "src/multithreading.h"
    "src/webAgent.h"
    "src/config.h"
    "src/types.h"
)

if(NOT WIN32)
    # Include the libcurl library
    find_package(CURL REQUIRED)
endif()

# Set the output directory to the project directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Add the required files to the project
add_executable(
    SQUARESHELPER
    ${COMMON_SOURCES}
    ${PLATFORM_SOURCES}
    ${HEADERS}
)

if(NOT WIN32)
    # Link with libcurl
    target_link_libraries(SQUARESHELPER PRIVATE CURL::libcurl)
else()
    # Link with winhttp
    target_link_libraries(SQUARESHELPER PRIVATE winhttp)
endif()

# Compile with all warkings and treat as errors
if(NOT WIN32)
    target_compile_options(SQUARESHELPER PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Change the executable name to squares-helper
set_target_properties(SQUARESHELPER PROPERTIES OUTPUT_NAME "squares-helper")