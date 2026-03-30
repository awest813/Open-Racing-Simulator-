set(ROBOTTOOLS_SOURCES
    rttelem.cpp
    rttrack.cpp
)

add_library(robottools STATIC ${ROBOTTOOLS_SOURCES})

target_include_directories(robottools PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)
