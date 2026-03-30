cmake_minimum_required(VERSION 3.10)
project(OpenRacingSimulator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(src/interfaces)
include_directories(src/libs)
include_directories(src/modules)

if(WIN32)
    add_definitions(-DWIN32 -D_WINDOWS)
    include_directories(src/windows)
else()
    include_directories(src/linux)
endif()

# Subdirectories
add_subdirectory(src/libs/tgf)
add_subdirectory(src/libs/txml)
# Add more as needed
