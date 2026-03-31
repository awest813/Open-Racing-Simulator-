macro(add_torcs_robot NAME DIR)
    file(GLOB ROBOT_SOURCES "${DIR}/*.cpp")
    message(STATUS "Robot ${NAME} sources: ${ROBOT_SOURCES}")
    add_library(${NAME} SHARED ${ROBOT_SOURCES})
    
    target_include_directories(${NAME} PRIVATE
        ${DIR}
        ${PROJECT_SOURCE_DIR}/src/interfaces
        ${PROJECT_SOURCE_DIR}/src/libs
        ${PROJECT_SOURCE_DIR}/src/libs/tgfclient
        ${PROJECT_SOURCE_DIR}/src/libs/client
        ${PROJECT_SOURCE_DIR}/src/windows/include
    )
    
    target_link_libraries(${NAME} PRIVATE
        tgf
        robottools
        sg_vs2008_32
        ul_vs2008_32
    )
    
    # Robots are usually named after their directory
    set_target_properties(${NAME} PROPERTIES 
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/drivers/${NAME}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/drivers/${NAME}"
    )
    
    # Extra dependencies for human
    if(NAME STREQUAL "human")
        target_link_libraries(${NAME} PRIVATE client tgfclient)
    endif()
endmacro()

set(ROBOT_LIST 
    berniw berniw2 berniw3 bt damned human inferno inferno2 lliaw olethros sparkle tita
)

foreach(R ${ROBOT_LIST})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${R}")
        add_torcs_robot(${R} "${CMAKE_CURRENT_SOURCE_DIR}/${R}")
    endif()
endforeach()
