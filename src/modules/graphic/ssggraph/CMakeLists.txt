set(SSGGRAPH_SOURCES
    CarSoundData.cpp
    grboard.cpp
    grcam.cpp
    grcar.cpp
    grcarlight.cpp
    grloadac.cpp
    grmain.cpp
    grmultitexstate.cpp
    grscene.cpp
    grscreen.cpp
    grshadow.cpp
    grskidmarks.cpp
    grsmoke.cpp
    grsound.cpp
    grtexture.cpp
    grtrackmap.cpp
    grutil.cpp
    grvtxtable.cpp
    OpenalSoundInterface.cpp
    PlibSoundInterface.cpp
    SoundInterface.cpp
    ssggraph.cpp
    TorcsSound.cpp
)

add_library(ssggraph STATIC ${SSGGRAPH_SOURCES})

target_include_directories(ssggraph PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
    ${PROJECT_SOURCE_DIR}/src/windows/include
)

# ssggraph needs many third party libraries
target_link_libraries(ssggraph
    tgf
    txml
    # PLIB
    ssg_vs2008_32
    ssgAux_vs2008_32
    sg_vs2008_32
    sl_vs2008_32
    ul_vs2008_32
    js_vs2008_32
    # Audio
    OpenAL32
    ALut
    # Graphics
    opengl32
    glu32
    glut32
    # Data
    libpng15_vs2008_32
    zlib1_vs2008_32
)
