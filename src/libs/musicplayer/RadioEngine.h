/***************************************************************************

    file                 : RadioEngine.h
    created              : 2024
    copyright            : (C) 2024 Open Racing Simulator Contributors
    license              : GNU General Public License v2

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __RadioEngine_h__
#define __RadioEngine_h__

/*
 * RadioEngine
 *
 * Implements a Forza-style in-game radio subsystem.
 *
 * Station layout on disk:
 *   data/music/radio/
 *   ├── Synthwave_FM/
 *   │   ├── track1.ogg
 *   │   └── track2.ogg
 *   └── Rock_Radio/
 *       └── track1.ogg
 *
 * API:
 *   RadioEngine::instance()   - singleton access
 *   .scanStations(basePath)   - discover stations from directory tree
 *   .startRadio()             - begin playback of current station/track
 *   .stopRadio()              - halt playback
 *   .update()                 - call every ~100ms to pump audio buffers
 *   .nextStation()            - cycle forward through stations
 *   .prevStation()            - cycle backward through stations
 *   .nextTrack()              - advance to next track on current station
 *   .setVolume(0.0-1.0)       - master radio volume
 *   .getStationName()         - current station name string
 *   .getTrackName()           - current track filename (no path)
 */

#include <string>
#include <vector>

// Forward declarations to avoid pulling in heavy headers here.
class OggSoundStream;
class OpenALMusicPlayer;

struct RadioStation
{
    std::string            name;          // folder name, e.g. "Synthwave_FM"
    std::vector<std::string> tracks;      // full absolute paths to .ogg files
};

class RadioEngine
{
public:
    // Singleton accessor – creates on first call.
    static RadioEngine& instance();

    // Deleted copy/move.
    RadioEngine(const RadioEngine&)            = delete;
    RadioEngine& operator=(const RadioEngine&) = delete;

    // -----------------------------------------------------------------------
    // Lifecycle
    // -----------------------------------------------------------------------

    /** Scan basePath (e.g. "data/music/radio") for station sub-folders. */
    void scanStations(const std::string& basePath);

    /** Start playback from the current station / track. */
    void startRadio();

    /** Stop all playback and release OpenAL resources. */
    void stopRadio();

    /**
     * Pump the OpenAL buffer.  Must be called regularly (~100 ms).
     * Returns false if no stations are available or an error occurred.
     */
    bool update();

    // -----------------------------------------------------------------------
    // Navigation
    // -----------------------------------------------------------------------

    void nextStation();
    void prevStation();
    void nextTrack();

    // -----------------------------------------------------------------------
    // Volume  (0.0 – 1.0)
    // -----------------------------------------------------------------------

    void  setVolume(float vol);
    float getVolume() const { return volume_; }

    // -----------------------------------------------------------------------
    // Info
    // -----------------------------------------------------------------------

    const std::string& getStationName() const;
    const std::string& getTrackName()   const;
    int  getStationCount()              const { return static_cast<int>(stations_.size()); }
    bool isPlaying()                    const { return playing_; }

private:
    RadioEngine();
    ~RadioEngine();

    // Tear down the current OpenAL player and stream.
    void teardown();

    // Build a new player for the current station/track.
    void setupCurrent();

    // Utility: extract filename from a full path.
    static std::string basename(const std::string& path);

    // -----------------------------------------------------------------------
    // State
    // -----------------------------------------------------------------------

    std::vector<RadioStation> stations_;
    int   stationIdx_  = 0;
    int   trackIdx_    = 0;
    float volume_      = 0.8f;
    bool  playing_     = false;
    bool  initialized_ = false;

    // Currently active stream and player (heap-allocated so we can swap them).
    OggSoundStream*    currentStream_ = nullptr;
    OpenALMusicPlayer* currentPlayer_ = nullptr;

    // Cache the base path for re-scan support.
    std::string basePath_;

    // Fallback strings for empty state.
    static const std::string kNoStation;
    static const std::string kNoTrack;
};

#endif // __RadioEngine_h__
