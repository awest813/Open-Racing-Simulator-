set(RACEENGINECLIENT_SOURCES
    careermenu.cpp
    raceengine.cpp
    racegl.cpp
    raceinit.cpp
    racemain.cpp
    racemanmenu.cpp
    raceresults.cpp
    racestate.cpp
    singleplayer.cpp
    telemetryrecorder.cpp
)

add_library(raceengineclient STATIC ${RACEENGINECLIENT_SOURCES})

target_include_directories(raceengineclient PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)

target_link_libraries(raceengineclient PUBLIC
    racescreens
)
