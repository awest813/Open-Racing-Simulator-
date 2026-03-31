set(CLIENT_SOURCES
    entry.cpp
    errno.cpp
    exitmenu.cpp
    mainmenu.cpp
    optionmenu.cpp
    splash.cpp
)

add_library(client STATIC ${CLIENT_SOURCES})

target_include_directories(client PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)

target_link_libraries(client PUBLIC
    confscreens
    raceengineclient
)
