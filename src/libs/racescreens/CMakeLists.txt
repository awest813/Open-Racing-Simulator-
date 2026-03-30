set(RACESCREENS_SOURCES
    carsetupscreen.cpp
    driverselect.cpp
    fileselect.cpp
    loadingscreen.cpp
    miscscreens.cpp
    pitmenu.cpp
    racemenu.cpp
    results.cpp
    trackselect.cpp
)

add_library(racescreens STATIC ${RACESCREENS_SOURCES})

target_include_directories(racescreens PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)
