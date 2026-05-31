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

add_library(ssggraph MODULE ${SSGGRAPH_SOURCES})

target_include_directories(ssggraph PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
    ${PROJECT_SOURCE_DIR}/src/libs/tgfclient
    ${PROJECT_SOURCE_DIR}/src/libs/robottools
)

if(WIN32)
    target_include_directories(ssggraph PRIVATE ${PROJECT_SOURCE_DIR}/src/windows/include)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ssggraph.def")
        set_target_properties(ssggraph PROPERTIES
            LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/ssggraph.def\""
            LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ssggraph.def"
        )
    endif()
    target_link_libraries(ssggraph PRIVATE
        tgf txml robottools
        ssg_vs2008_32 ssgAux_vs2008_32 sg_vs2008_32 sl_vs2008_32 ul_vs2008_32 js_vs2008_32
        OpenAL32 ALut
        opengl32 glu32 glut32
        libpng15_vs2008_32 zlib1_vs2008_32
    )
else()
    target_link_libraries(ssggraph PRIVATE
        tgf txml robottools
        plibssg plibssgaux plibsg plibul plibsl plibsm
        GL GLU glut
        openal alut vorbisfile vorbis ogg
        z png
        X11
    )
endif()

torcs_add_plugin_module(ssggraph "modules/graphic")

if(MSVC)
    target_compile_options(ssggraph PRIVATE /wd4996)
endif()
