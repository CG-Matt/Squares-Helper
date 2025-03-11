cmake_minimum_required(VERSION 3.18)

# Set the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Name the project
project(SQUARESHELPER)

# List all source files
set(SOURCES
    "src/CGMString.c"
    "src/fileManager.c"
    "src/hashSet.c"
    "src/multithreading.c"
    "src/webAgent.c"
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

# Include the libcurl library
find_package(CURL REQUIRED)

# Add the required files to the project
add_executable(
    SQUARESHELPER
    ${SOURCES}
    ${HEADERS}
)

# Link with libcurl
target_link_libraries(SQUARESHELPER PRIVATE CURL::libcurl)

# Compile with all warkings and treat as errors
target_compile_options(SQUARESHELPER PRIVATE -Wall -Wextra -Wpedantic -Werror)

# Change the executable name to squares-helper
set_target_properties(SQUARESHELPER PROPERTIES OUTPUT_NAME "squares-helper")