set(WTORCS_SOURCES
    main.cpp
    windowsspec.cpp
    ${PROJECT_SOURCE_DIR}/torcs.rc
)

add_executable(wtorcs ${WTORCS_SOURCES})
torcs_set_runtime_output(wtorcs ".")

target_include_directories(wtorcs PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
    ${PROJECT_SOURCE_DIR}/src/libs/client
)

# Fix for MSVC: wtorcs needs these link flags for the resource compiler
if(MSVC)
    set_target_properties(wtorcs PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

target_link_libraries(wtorcs
    client
    tgfclient
    tgf
    txml
    raceengineclient
    robottools
    learning
    confscreens
    racescreens
    musicplayer
    ssggraph
    # Third party libs from src/windows/lib (already in link_directories)
    ssg_vs2008_32
    ssgAux_vs2008_32
    sg_vs2008_32
    sl_vs2008_32
    ul_vs2008_32
    js_vs2008_32
    libpng15_vs2008_32
    zlib1_vs2008_32
    glut32
    ul_vs2008_32
    sg_vs2008_32
    sl_vs2008_32
    ssg_vs2008_32
    OpenAL32
    libvorbis
    libvorbisfile
    libogg
    opengl32
    glu32
)

add_dependencies(wtorcs
    torcs_runtime_assets
    simuv2
    simuv3
    track
    ${TORCS_ROBOT_TARGETS}
)
