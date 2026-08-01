set(TGFCLIENT_SOURCES
    control.cpp
    fg_gm.cpp
    glfeatures.cpp
    gui.cpp
    guibutton.cpp
    guiedit.cpp
    guifont.cpp
    guihelp.cpp
    guiimage.cpp
    guilabel.cpp
    guimenu.cpp
    guiobject.cpp
    guiscrollbar.cpp
    guiscrollist.cpp
    img.cpp
    screen.cpp
    sdl2glut.cpp
    tgfclient.cpp
    RmlUiMain.cpp
    ${PROJECT_SOURCE_DIR}/src/libs/thirdparty/RmlUi/Backends/RmlUi_Platform_SDL.cpp
    ${PROJECT_SOURCE_DIR}/src/libs/thirdparty/RmlUi/Backends/RmlUi_Renderer_GL2.cpp
)

add_library(tgfclient STATIC ${TGFCLIENT_SOURCES})

target_include_directories(tgfclient PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)
target_include_directories(tgfclient PRIVATE
    ${PROJECT_SOURCE_DIR}/src/libs/thirdparty/stb
    ${PROJECT_SOURCE_DIR}/src/libs/thirdparty/RmlUi/Backends
)

target_link_libraries(tgfclient PUBLIC
    tgf
    txml
    musicplayer
    rmlui_core
    rmlui_debugger
)

if(TORCS_USE_SDL2)
    target_compile_definitions(tgfclient PUBLIC TORCS_USE_SDL2 FREEGLUT)
    target_link_libraries(tgfclient PUBLIC PkgConfig::SDL2)
endif()

if(WIN32)
    target_link_libraries(tgfclient PUBLIC
        js_vs2008_32
        glut32
        opengl32
        glu32
    )
endif()
