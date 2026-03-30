set(MUSICPLAYER_SOURCES
    musicplayer.cpp
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
