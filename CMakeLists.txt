cmake_minimum_required(VERSION 3.10)

# Avoid link failures during compiler sanity checks in minimal CI images.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(OpenRacingSimulator LANGUAGES C CXX)

include(CheckSymbolExists)
check_symbol_exists(strndup "string.h" HAVE_STRNDUP)
if(HAVE_STRNDUP)
    add_compile_definitions(HAVE_STRNDUP=1)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Static libs (e.g. tgf) are linked into MODULE plugins (simuv2.so).
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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

# Runtime-loaded .so modules (no lib prefix; e.g. simuv2.so not libsimuv2.so)
function(torcs_add_plugin_module target runtime_subdir)
    set_target_properties(${target} PROPERTIES PREFIX "")
    if(WIN32)
        set_target_properties(${target} PROPERTIES OUTPUT_NAME "${target}")
    endif()
    torcs_set_runtime_output(${target} "${runtime_subdir}")
    add_dependencies(${target} torcs_runtime_assets)
endfunction()

file(MAKE_DIRECTORY "${TORCS_RUNTIME_DIRECTORY}")
file(MAKE_DIRECTORY "${TORCS_RUNTIME_DIRECTORY}/modules")

file(GLOB_RECURSE TORCS_RUNTIME_ASSET_INPUTS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/data/*"
    "${CMAKE_SOURCE_DIR}/runtime/config/*"
    "${CMAKE_SOURCE_DIR}/runtime/drivers/*"
    "${CMAKE_SOURCE_DIR}/runtime/results/*"
    "${CMAKE_SOURCE_DIR}/runtime/telemetry/*"
)

if(WIN32)
    file(GLOB TORCS_WINDOWS_RUNTIME_DLLS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/windows/dll/*.dll")
    list(APPEND TORCS_RUNTIME_ASSET_INPUTS ${TORCS_WINDOWS_RUNTIME_DLLS})
endif()

set(TORCS_RUNTIME_ASSET_STAMP "${CMAKE_BINARY_DIR}/CMakeFiles/torcs_runtime_assets.stamp")

add_custom_command(
    OUTPUT "${TORCS_RUNTIME_ASSET_STAMP}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${TORCS_RUNTIME_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${TORCS_RUNTIME_DIRECTORY}/modules"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/data" "${TORCS_RUNTIME_DIRECTORY}/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/data/data" "${TORCS_RUNTIME_DIRECTORY}/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/runtime/config" "${TORCS_RUNTIME_DIRECTORY}/config"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/runtime/drivers" "${TORCS_RUNTIME_DIRECTORY}/drivers"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/runtime/results" "${TORCS_RUNTIME_DIRECTORY}/results"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/runtime/telemetry" "${TORCS_RUNTIME_DIRECTORY}/telemetry"
    COMMAND ${CMAKE_COMMAND} -E touch "${TORCS_RUNTIME_ASSET_STAMP}"
    DEPENDS ${TORCS_RUNTIME_ASSET_INPUTS}
    COMMENT "Syncing TORCS runtime assets"
    VERBATIM
)

if(WIN32)
    add_custom_command(
        OUTPUT "${TORCS_RUNTIME_ASSET_STAMP}"
        APPEND
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TORCS_WINDOWS_RUNTIME_DLLS} "${TORCS_RUNTIME_DIRECTORY}"
    )
endif()

add_custom_target(torcs_runtime_assets DEPENDS "${TORCS_RUNTIME_ASSET_STAMP}")

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

# Portable install layout (matches runtime/ tree used by torcs -l)
install(TARGETS torcs
    RUNTIME DESTINATION bin
)
if(TORCS_ROBOT_TARGETS)
    install(TARGETS ${TORCS_ROBOT_TARGETS}
        LIBRARY DESTINATION drivers
        RUNTIME DESTINATION drivers
    )
endif()
if(TARGET simuv2)
    install(TARGETS simuv2 LIBRARY DESTINATION modules/simu)
endif()
if(TARGET track)
    install(TARGETS track LIBRARY DESTINATION modules/track)
endif()
if(TARGET ssggraph)
    install(TARGETS ssggraph
        LIBRARY DESTINATION modules/graphic
    )
endif()
install(DIRECTORY "${CMAKE_SOURCE_DIR}/data"
    DESTINATION .
    USE_SOURCE_PERMISSIONS
)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/runtime/config"
    DESTINATION config
    USE_SOURCE_PERMISSIONS
)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/runtime/drivers"
    DESTINATION drivers
    USE_SOURCE_PERMISSIONS
    OPTIONAL
)
