macro(add_torcs_robot NAME DIR)
    file(GLOB ROBOT_SOURCES "${DIR}/*.cpp")
    message(STATUS "Robot ${NAME} sources: ${ROBOT_SOURCES}")
    add_library(${NAME} SHARED ${ROBOT_SOURCES})

    if(WIN32 AND EXISTS "${DIR}/${NAME}.def")
        set_target_properties(${NAME} PROPERTIES
            LINK_FLAGS "/DEF:\"${DIR}/${NAME}.def\""
            LINK_DEPENDS "${DIR}/${NAME}.def"
        )
    endif()
    
    target_include_directories(${NAME} PRIVATE
        ${DIR}
        ${PROJECT_SOURCE_DIR}/src/interfaces
        ${PROJECT_SOURCE_DIR}/src/libs
        ${PROJECT_SOURCE_DIR}/src/libs/tgfclient
        ${PROJECT_SOURCE_DIR}/src/libs/client
        ${PROJECT_SOURCE_DIR}/src/windows/include
    )
    
    if(WIN32)
        target_link_libraries(${NAME} PRIVATE
            tgf
            robottools
            sg_vs2008_32
            ul_vs2008_32
        )
    else()
        target_link_libraries(${NAME} PRIVATE
            tgf
            robottools
            plibsg
            plibul
        )
    endif()
    
    # Robots are usually named after their directory
    set_target_properties(${NAME} PROPERTIES PREFIX "")
    torcs_set_runtime_output(${NAME} "drivers/${NAME}")
    
    # Extra dependencies for human
    if("${NAME}" STREQUAL "human")
        if(WIN32)
            target_link_libraries(${NAME} PRIVATE
                client
                tgfclient
                musicplayer
                js_vs2008_32
                libpng15_vs2008_32
                zlib1_vs2008_32
                glut32
                opengl32
                glu32
            )
        else()
            target_link_libraries(${NAME} PRIVATE
                client
                tgfclient
                musicplayer
                plibjs
                glut
                GL
                GLU
            )
        endif()
    endif()

    if("${NAME}" STREQUAL "olethros")
        target_link_libraries(${NAME} PRIVATE learning)
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

set(TORCS_ROBOT_TARGETS ${ROBOT_LIST} PARENT_SCOPE)
