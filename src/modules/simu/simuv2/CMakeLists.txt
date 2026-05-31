# Build SOLID-2.0 collision library first
add_subdirectory(SOLID-2.0)

add_library(simuv2 MODULE
    aero.cpp
    atmosphere.cpp
    axle.cpp
    brake.cpp
    car.cpp
    categories.cpp
    collide.cpp
    differential.cpp
    engine.cpp
    simu.cpp
    simuitf.cpp
    steer.cpp
    susp.cpp
    tracksurface.cpp
    transmission.cpp
    wheel.cpp
)

target_include_directories(simuv2 PRIVATE ${CMAKE_SOURCE_DIR}/src/interfaces)
target_include_directories(simuv2 PRIVATE ${CMAKE_SOURCE_DIR}/src/modules)
target_include_directories(simuv2 PRIVATE ${CMAKE_SOURCE_DIR}/src/libs)

if(WIN32)
    target_compile_definitions(simuv2 PRIVATE WIN32 _WINDOWS)
    set_target_properties(simuv2 PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/simuv2.def\""
        LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/simuv2.def"
    )
endif()
torcs_add_plugin_module(simuv2 "modules/simu")

if(MSVC)
    target_compile_options(simuv2 PRIVATE /wd4996)
endif()

# solid's PUBLIC include_directories propagate <SOLID/solid.h> and <3D/*.h>
if(WIN32)
    target_link_libraries(simuv2 PRIVATE tgf txml robottools solid sg ul)
else()
    target_link_libraries(simuv2 PRIVATE tgf txml robottools solid plibsg plibul)
endif()
