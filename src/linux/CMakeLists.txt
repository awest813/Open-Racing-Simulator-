set(TORCS_SOURCES
    main.cpp
    linuxspec.cpp
)

add_executable(torcs ${TORCS_SOURCES})

target_include_directories(torcs PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
    ${PROJECT_SOURCE_DIR}/src/libs/client
)

target_link_libraries(torcs
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
    plibjs
    plibssgaux
    plibssg
    plibsm
    plibsl
    plibsg
    plibul
    GL
    GLU
    glut
    X11
    Xext
    Xrender
    Xrandr
    Xxf86vm
    Xmu
    Xi
    Xt
    SM
    ICE
    openal
    vorbisfile
    vorbis
    ogg
    z
    png
    dl
    m
    txml
)

# Linux binary is usually named 'torcs-bin' because a shell script launches it.
set_target_properties(torcs PROPERTIES OUTPUT_NAME "torcs-bin")

add_dependencies(torcs torcs_runtime_assets)
