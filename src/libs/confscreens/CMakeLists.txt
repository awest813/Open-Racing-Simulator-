set(CONFSCREENS_SOURCES
    controlconfig.cpp
    driverconfig.cpp
    graphconfig.cpp
    joystickconfig.cpp
    mouseconfig.cpp
    openglconfig.cpp
    simuconfig.cpp
    soundconfig.cpp
)

add_library(confscreens STATIC ${CONFSCREENS_SOURCES})

target_include_directories(confscreens PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)

target_link_libraries(confscreens PUBLIC tgfclient)
