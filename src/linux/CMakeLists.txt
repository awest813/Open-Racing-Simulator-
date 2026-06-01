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

set(TORCS_LINUX_LIBS
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

if(TORCS_USE_SDL2)
    list(APPEND TORCS_LINUX_LIBS PkgConfig::SDL2)
    target_compile_definitions(torcs PRIVATE TORCS_USE_SDL2 FREEGLUT)
else()
    list(APPEND TORCS_LINUX_LIBS glut)
endif()

target_link_libraries(torcs PRIVATE ${TORCS_LINUX_LIBS})

# Linux binary is usually named 'torcs-bin' because a shell script launches it.
set_target_properties(torcs PROPERTIES OUTPUT_NAME "torcs-bin")

add_dependencies(torcs torcs_runtime_assets)
