add_library(simuv3 MODULE
    SimulationOptions.cpp
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
    transmission.cpp
    wheel.cpp
)

target_include_directories(simuv3 PRIVATE ${CMAKE_SOURCE_DIR}/src/interfaces)
target_include_directories(simuv3 PRIVATE ${CMAKE_SOURCE_DIR}/src/modules)
target_include_directories(simuv3 PRIVATE ${CMAKE_SOURCE_DIR}/src/libs)

if(WIN32)
    target_compile_definitions(simuv3 PRIVATE WIN32 _WINDOWS)
    set_target_properties(simuv3 PROPERTIES
        LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/simuv3.def\""
        LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/simuv3.def"
    )
endif()
torcs_add_plugin_module(simuv3 "modules/simu")

if(MSVC)
    target_compile_options(simuv3 PRIVATE /wd4996)
endif()

# solid target is built by simuv2's subdirectory; its PUBLIC includes propagate here
if(WIN32)
    target_link_libraries(simuv3 PRIVATE tgf txml robottools solid sg ul)
else()
    target_link_libraries(simuv3 PRIVATE tgf txml robottools solid plibsg plibul)
endif()
