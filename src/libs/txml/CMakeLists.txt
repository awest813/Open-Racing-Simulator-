set(TXML_SOURCES
    txml.cpp
)

add_library(txml STATIC ${TXML_SOURCES})

target_include_directories(txml PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)
