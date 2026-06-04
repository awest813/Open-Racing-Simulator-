/***************************************************************************
 * MiniAudioPlayer.h  --  miniaudio-backed music player
 *
 * Drop-in replacement for OpenALMusicPlayer.  Plays .ogg files using
 * miniaudio's built-in Vorbis decoder (stb_vorbis) without any external
 * OpenAL runtime DLL.
 *
 * Feature parity with OpenALMusicPlayer:
 *   start(), stop(), rewind(), playAndManageBuffer(), setVolume()
 ***************************************************************************/
#pragma once

#include <string>

class MiniAudioPlayer {
public:
    explicit MiniAudioPlayer(const std::string& filePath);
    ~MiniAudioPlayer();

    /** Begin playback from the current position. */
    void start();

    /** Stop playback (does not rewind). */
    void stop();

    /** Seek back to the beginning. */
    void rewind();

    /** Pump the audio buffers.  Call periodically (~every 100 ms).
     *  Returns false when playback has finished (call rewind+start to loop). */
    bool playAndManageBuffer();

    /** Master volume: 0.0 (silent) – 1.0 (full). */
    void setVolume(float vol);

    /** True when audio is currently playing. */
    bool isPlaying() const;

private:
    struct Impl;
    Impl* m_impl = nullptr;
};
