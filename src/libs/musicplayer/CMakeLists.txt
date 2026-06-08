set(MUSICPLAYER_SOURCES
    musicplayer.cpp
    RadioEngine.cpp
    OggSoundStream.cpp
    SoundStream.cpp
)

# ---- Backend selection ----------------------------------------------------
# Default: use the legacy OpenAL+Vorbis player on platforms where it is
# available.  Define TORCS_USE_MINIAUDIO (e.g. -DTORCS_USE_MINIAUDIO=ON) to
# switch to the self-contained miniaudio backend instead.
option(TORCS_USE_MINIAUDIO "Use miniaudio instead of OpenAL for music playback" ON)

if(TORCS_USE_MINIAUDIO)
    list(APPEND MUSICPLAYER_SOURCES MiniAudioPlayer.cpp)
    message(STATUS "musicplayer: using miniaudio backend")
else()
    list(APPEND MUSICPLAYER_SOURCES OpenALMusicPlayer.cpp)
    message(STATUS "musicplayer: using OpenAL backend")
endif()

add_library(musicplayer STATIC ${MUSICPLAYER_SOURCES})

target_include_directories(musicplayer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/interfaces
    ${PROJECT_SOURCE_DIR}/src/libs
    ${PROJECT_SOURCE_DIR}/src/libs/thirdparty/miniaudio
)

target_link_libraries(musicplayer PUBLIC tgf)

if(TORCS_USE_MINIAUDIO)
    target_compile_definitions(musicplayer PUBLIC TORCS_USE_MINIAUDIO)
    # miniaudio pulls in platform audio APIs automatically
    if(WIN32)
        target_link_libraries(musicplayer PUBLIC ole32 uuid)
    elseif(APPLE)
        target_link_libraries(musicplayer PUBLIC
            "-framework CoreAudio"
            "-framework AudioToolbox"
            "-framework AVFoundation"
        )
    else()
        # Linux: link pthread; miniaudio uses ALSA or PulseAudio at runtime
        target_link_libraries(musicplayer PUBLIC pthread dl m)
    endif()
else()
    if(WIN32)
        target_link_libraries(musicplayer PUBLIC
            OpenAL32
            libvorbis
            libvorbisfile
            libogg
        )
    endif()
endif()
