set(MUSICPLAYER_SOURCES
    musicplayer.cpp
    RadioEngine.cpp
    OggSoundStream.cpp
    OpenALMusicPlayer.cpp
    SoundStream.cpp
)

add_library(musicplayer STATIC ${MUSICPLAYER_SOURCES})

target_include_directories(musicplayer PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
)

target_link_libraries(musicplayer PUBLIC tgf)

if(WIN32)
    target_link_libraries(musicplayer PUBLIC
        OpenAL32
        libvorbis
        libvorbisfile
        libogg
    )
endif()
