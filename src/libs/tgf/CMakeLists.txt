set(TGF_SOURCES
    directory.cpp
    hash.cpp
    module.cpp
    os.cpp
    params.cpp
    profiler.cpp
    tgf.cpp
    trace.cpp
)

add_library(tgf SHARED ${TGF_SOURCES})

target_include_directories(tgf PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)

if(WIN32)
    target_link_libraries(tgf PUBLIC ws2_32)
endif()
