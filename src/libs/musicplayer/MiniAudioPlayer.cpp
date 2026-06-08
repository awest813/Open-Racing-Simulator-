/***************************************************************************
 * MiniAudioPlayer.cpp  --  miniaudio Vorbis streaming player
 *
 * Uses miniaudio's high-level sound API with the built-in stb_vorbis
 * decoder.  No OpenAL, no external Vorbis DLL.
 ***************************************************************************/

// miniaudio single-header implementation (compiled in this TU only)
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "MiniAudioPlayer.h"

#include <tgf.h>
#include <cstring>
#include <cstdio>

// =========================================================================
// Pimpl
// =========================================================================

struct MiniAudioPlayer::Impl {
    ma_engine   engine  = {};
    ma_sound    sound   = {};
    bool        engineOk = false;
    bool        soundOk  = false;
    std::string filePath;
    float       volume   = 1.0f;
};

// =========================================================================
// Public API
// =========================================================================

MiniAudioPlayer::MiniAudioPlayer(const std::string& filePath)
{
    m_impl = new Impl();
    m_impl->filePath = filePath;

    ma_engine_config cfg = ma_engine_config_init();
    if (ma_engine_init(&cfg, &m_impl->engine) != MA_SUCCESS) {
        GfOut("MiniAudioPlayer: ma_engine_init failed\n");
        return;
    }
    m_impl->engineOk = true;

    // Load the sound with streaming (MA_SOUND_FLAG_STREAM) so large OGG files
    // are not held entirely in RAM.
    ma_uint32 flags = MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_NO_SPATIALIZATION;
    if (ma_sound_init_from_file(&m_impl->engine,
                                filePath.c_str(),
                                flags, nullptr, nullptr,
                                &m_impl->sound) != MA_SUCCESS) {
        GfOut("MiniAudioPlayer: could not open '%s'\n", filePath.c_str());
        return;
    }
    ma_sound_set_volume(&m_impl->sound, m_impl->volume);
    m_impl->soundOk = true;
}

MiniAudioPlayer::~MiniAudioPlayer()
{
    if (m_impl) {
        if (m_impl->soundOk) {
            ma_sound_stop(&m_impl->sound);
            ma_sound_uninit(&m_impl->sound);
        }
        if (m_impl->engineOk) {
            ma_engine_uninit(&m_impl->engine);
        }
        delete m_impl;
        m_impl = nullptr;
    }
}

void MiniAudioPlayer::start()
{
    if (!m_impl || !m_impl->soundOk) return;
    ma_sound_set_volume(&m_impl->sound, m_impl->volume);
    ma_sound_start(&m_impl->sound);
}

void MiniAudioPlayer::stop()
{
    if (!m_impl || !m_impl->soundOk) return;
    ma_sound_stop(&m_impl->sound);
}

void MiniAudioPlayer::rewind()
{
    if (!m_impl || !m_impl->soundOk) return;
    ma_sound_seek_to_pcm_frame(&m_impl->sound, 0);
}

bool MiniAudioPlayer::playAndManageBuffer()
{
    if (!m_impl || !m_impl->soundOk) return false;
    // miniaudio handles buffering internally; we just check if it is still
    // playing and auto-loop at the end.
    if (ma_sound_at_end(&m_impl->sound)) {
        rewind();
        start();
    }
    return ma_sound_is_playing(&m_impl->sound) == MA_TRUE;
}

void MiniAudioPlayer::setVolume(float vol)
{
    if (!m_impl) return;
    m_impl->volume = vol;
    if (m_impl->soundOk) {
        ma_sound_set_volume(&m_impl->sound, vol);
    }
}

bool MiniAudioPlayer::isPlaying() const
{
    if (!m_impl || !m_impl->soundOk) return false;
    return ma_sound_is_playing(&m_impl->sound) == MA_TRUE;
}
