set(TXML_SOURCES
    xml.cpp
    xmlparse.c
    xmlrole.c
    xmltok.c
    hashtable.c
)

add_library(txml STATIC ${TXML_SOURCES})

target_include_directories(txml PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)
