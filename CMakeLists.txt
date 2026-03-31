cmake_minimum_required(VERSION 3.10)
project(OpenRacingSimulator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TORCS_RUNTIME_DIRECTORY "${CMAKE_BINARY_DIR}/runtime")

if(MSVC)
    list(APPEND CMAKE_VS_GLOBALS VcpkgXUseBuiltInApplocalDeps=true)
endif()

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

function(torcs_set_runtime_output target relative_dir)
    set(output_dir "${TORCS_RUNTIME_DIRECTORY}/${relative_dir}")
    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
        LIBRARY_OUTPUT_DIRECTORY "${output_dir}"
    )
    foreach(cfg Debug Release MinSizeRel RelWithDebInfo)
        string(TOUPPER "${cfg}" cfg_upper)
        set_target_properties(${target} PROPERTIES
            "RUNTIME_OUTPUT_DIRECTORY_${cfg_upper}" "${output_dir}"
            "LIBRARY_OUTPUT_DIRECTORY_${cfg_upper}" "${output_dir}"
        )
    endforeach()
endfunction()

file(MAKE_DIRECTORY "${TORCS_RUNTIME_DIRECTORY}")
file(MAKE_DIRECTORY "${TORCS_RUNTIME_DIRECTORY}/modules")
file(COPY "${CMAKE_SOURCE_DIR}/data" DESTINATION "${TORCS_RUNTIME_DIRECTORY}")
file(COPY "${CMAKE_SOURCE_DIR}/data/data/" DESTINATION "${TORCS_RUNTIME_DIRECTORY}/data")
file(COPY
    "${CMAKE_SOURCE_DIR}/runtime/config"
    "${CMAKE_SOURCE_DIR}/runtime/drivers"
    "${CMAKE_SOURCE_DIR}/runtime/results"
    "${CMAKE_SOURCE_DIR}/runtime/telemetry"
    DESTINATION "${TORCS_RUNTIME_DIRECTORY}"
)

if(WIN32)
    file(COPY "${CMAKE_SOURCE_DIR}/src/windows/dll/" DESTINATION "${TORCS_RUNTIME_DIRECTORY}")
endif()

# Include directories
configure_file(config.h.in ${CMAKE_BINARY_DIR}/config.h)
file(COPY src/libs/math/ DESTINATION ${CMAKE_BINARY_DIR}/tmath)
include_directories(${CMAKE_BINARY_DIR})
include_directories(src/interfaces)
include_directories(src/libs)
include_directories(src/libs/client)
include_directories(src/libs/confscreens)
include_directories(src/libs/learning)
include_directories(src/libs/math)
include_directories(src/libs/musicplayer)
include_directories(src/libs/portability)
include_directories(src/libs/raceengineclient)
include_directories(src/libs/racescreens)
include_directories(src/libs/robottools)
include_directories(src/libs/tgf)
include_directories(src/libs/tgfclient)
include_directories(src/libs/txml)
include_directories(src/modules)

if(WIN32)
    add_definitions(-DWIN32 -D_WINDOWS)
    include_directories(src/windows)
    include_directories(src/windows/include)
    link_directories(${CMAKE_SOURCE_DIR}/src/windows/lib)
    link_libraries(legacy_stdio_definitions.lib)
else()
    include_directories(src/linux)
endif()

# Subdirectories
add_subdirectory(src/libs/client)
add_subdirectory(src/libs/confscreens)
add_subdirectory(src/libs/learning)
add_subdirectory(src/libs/musicplayer)
add_subdirectory(src/libs/raceengineclient)
add_subdirectory(src/libs/racescreens)
add_subdirectory(src/libs/robottools)
add_subdirectory(src/libs/tgf)
add_subdirectory(src/libs/tgfclient)
add_subdirectory(src/libs/txml)
add_subdirectory(src/modules/simu/simuv2)
add_subdirectory(src/modules/simu/simuv3)
add_subdirectory(src/modules/track)
add_subdirectory(src/modules/graphic)
add_subdirectory(src/drivers)

if(WIN32)
    add_subdirectory(src/windows)
else()
    add_subdirectory(src/linux)
endif()
# Add more as needed
