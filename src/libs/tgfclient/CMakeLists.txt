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
    tgfclient.cpp
)

add_library(tgfclient STATIC ${TGFCLIENT_SOURCES})

target_include_directories(tgfclient PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)
