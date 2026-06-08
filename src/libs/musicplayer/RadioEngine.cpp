/***************************************************************************

    file                 : RadioEngine.cpp
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

#include "RadioEngine.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

// Platform-specific directory scanning
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <dirent.h>
#  include <sys/stat.h>
#endif

#include <tgf.h>       // GfOut / GfError

#include "OggSoundStream.h"
#include "OpenALMusicPlayer.h"

// ---------------------------------------------------------------------------
// Static member initialisation
// ---------------------------------------------------------------------------

const std::string RadioEngine::kNoStation = "(no station)";
const std::string RadioEngine::kNoTrack   = "(no track)";

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

RadioEngine& RadioEngine::instance()
{
    static RadioEngine s;
    return s;
}

RadioEngine::RadioEngine()  = default;
RadioEngine::~RadioEngine() { stopRadio(); }

// ---------------------------------------------------------------------------
// Directory helpers
// ---------------------------------------------------------------------------

/** Returns true when `path` is a directory. */
static bool isDirectory(const std::string& path)
{
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path.c_str());
    return (attr != INVALID_FILE_ATTRIBUTES) &&
           (attr & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat st;
    return (stat(path.c_str(), &st) == 0) && S_ISDIR(st.st_mode);
#endif
}

/** List immediate children of `dir`.  Returns empty on failure. */
static std::vector<std::string> listDirectory(const std::string& dir)
{
    std::vector<std::string> result;

#ifdef _WIN32
    WIN32_FIND_DATAA ffd;
    std::string pattern = dir + "\\*";
    HANDLE hFind = FindFirstFileA(pattern.c_str(), &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        return result;

    do {
        const char* name = ffd.cFileName;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
            continue;
        result.push_back(std::string(name));
    } while (FindNextFileA(hFind, &ffd));

    FindClose(hFind);
#else
    DIR* d = opendir(dir.c_str());
    if (!d) return result;
    struct dirent* ent;
    while ((ent = readdir(d)) != nullptr) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;
        result.push_back(std::string(ent->d_name));
    }
    closedir(d);
#endif

    std::sort(result.begin(), result.end());
    return result;
}

// ---------------------------------------------------------------------------
// Scan
// ---------------------------------------------------------------------------

void RadioEngine::scanStations(const std::string& basePath)
{
    basePath_ = basePath;
    stations_.clear();
    stationIdx_ = 0;
    trackIdx_   = 0;

    if (!isDirectory(basePath)) {
        GfError("RadioEngine: base path not found: %s\n", basePath.c_str());
        return;
    }

    auto dirs = listDirectory(basePath);
    for (const auto& dirName : dirs) {
        std::string stationPath = basePath + "/" + dirName;
        if (!isDirectory(stationPath))
            continue;

        RadioStation station;
        station.name = dirName;

        auto files = listDirectory(stationPath);
        for (const auto& file : files) {
            // Accept only .ogg files (case-insensitive suffix check)
            if (file.size() < 4)
                continue;
            std::string ext = file.substr(file.size() - 4);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext != ".ogg")
                continue;

            station.tracks.push_back(stationPath + "/" + file);
        }

        if (!station.tracks.empty()) {
            stations_.push_back(std::move(station));
            GfOut("RadioEngine: Found station '%s' with %zu track(s)\n",
                  stations_.back().name.c_str(),
                  stations_.back().tracks.size());
        }
    }

    if (stations_.empty()) {
        GfOut("RadioEngine: No stations found in '%s'\n", basePath.c_str());
    } else {
        GfOut("RadioEngine: Scan complete – %zu station(s) available\n",
              stations_.size());
    }
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

void RadioEngine::teardown()
{
    if (currentPlayer_) {
        currentPlayer_->stop();
        delete currentPlayer_;
        currentPlayer_ = nullptr;
    }
    if (currentStream_) {
        delete currentStream_;
        currentStream_ = nullptr;
    }
    playing_ = false;
}

void RadioEngine::setupCurrent()
{
    teardown();

    if (stations_.empty())
        return;

    const RadioStation& st    = stations_[stationIdx_];
    const std::string&  track = st.tracks[trackIdx_];

    // OggSoundStream expects a mutable char* for legacy reasons.
    static char pathBuf[4096];
#ifdef _WIN32
    strncpy_s(pathBuf, sizeof(pathBuf), track.c_str(), _TRUNCATE);
#else
    strncpy(pathBuf, track.c_str(), sizeof(pathBuf) - 1);
    pathBuf[sizeof(pathBuf) - 1] = '\0';
#endif

    currentStream_ = new OggSoundStream(pathBuf);
    if (!currentStream_->isValid()) {
        GfError("RadioEngine: Failed to open '%s'\n", pathBuf);
        teardown();
        return;
    }

    currentPlayer_ = new OpenALMusicPlayer(currentStream_);
    GfOut("RadioEngine: Loaded [%s] %s\n",
          st.name.c_str(), basename(track).c_str());
}

void RadioEngine::startRadio()
{
    if (stations_.empty()) {
        GfOut("RadioEngine: No stations – radio disabled.\n");
        return;
    }

    setupCurrent();

    if (currentPlayer_) {
        currentPlayer_->setVolume(volume_);
        currentPlayer_->start();
        playing_ = true;
        GfOut("RadioEngine: Playback started on station '%s'\n",
              stations_[stationIdx_].name.c_str());
    }

}

void RadioEngine::stopRadio()
{
    teardown();
}

bool RadioEngine::update()
{
    if (!playing_ || !currentPlayer_)
        return false;

    return currentPlayer_->playAndManageBuffer();
}

// ---------------------------------------------------------------------------
// Navigation
// ---------------------------------------------------------------------------

void RadioEngine::nextStation()
{
    if (stations_.empty()) return;

    stationIdx_ = (stationIdx_ + 1) % static_cast<int>(stations_.size());
    trackIdx_   = 0;

    GfOut("RadioEngine: Switching to station '%s'\n",
          stations_[stationIdx_].name.c_str());

    if (playing_) {
        setupCurrent();
        if (currentPlayer_) {
            currentPlayer_->setVolume(volume_);
            currentPlayer_->start();
        }
    }

}

void RadioEngine::prevStation()
{
    if (stations_.empty()) return;

    stationIdx_ = (stationIdx_ - 1 + static_cast<int>(stations_.size()))
                  % static_cast<int>(stations_.size());
    trackIdx_   = 0;

    GfOut("RadioEngine: Switching to station '%s'\n",
          stations_[stationIdx_].name.c_str());

    if (playing_) {
        setupCurrent();
        if (currentPlayer_) {
            currentPlayer_->setVolume(volume_);
            currentPlayer_->start();
        }
    }

}

void RadioEngine::nextTrack()
{
    if (stations_.empty()) return;

    const RadioStation& st = stations_[stationIdx_];
    trackIdx_ = (trackIdx_ + 1) % static_cast<int>(st.tracks.size());

    GfOut("RadioEngine: Next track '%s'\n",
          basename(st.tracks[trackIdx_]).c_str());

    if (playing_) {
        setupCurrent();
        if (currentPlayer_) {
            currentPlayer_->setVolume(volume_);
            currentPlayer_->start();
        }
    }

}

// ---------------------------------------------------------------------------
// Volume
// ---------------------------------------------------------------------------

void RadioEngine::setVolume(float vol)
{
    volume_ = (vol < 0.0f) ? 0.0f : (vol > 1.0f) ? 1.0f : vol;
    if (currentPlayer_) {
        currentPlayer_->setVolume(volume_);
    }
    GfOut("RadioEngine: Volume set to %.2f\n", volume_);
}


// ---------------------------------------------------------------------------
// Info
// ---------------------------------------------------------------------------

const std::string& RadioEngine::getStationName() const
{
    if (stations_.empty()) return kNoStation;
    return stations_[stationIdx_].name;
}

const std::string& RadioEngine::getTrackName() const
{
    if (stations_.empty()) return kNoTrack;
    const auto& tracks = stations_[stationIdx_].tracks;
    if (tracks.empty()) return kNoTrack;
    // Return a reference to the cached string held in the station list.
    // We convert the full path to a basename each time – cache in a local.
    static std::string cachedBasename;
    cachedBasename = basename(tracks[trackIdx_]);
    return cachedBasename;
}

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------

/*static*/ std::string RadioEngine::basename(const std::string& path)
{
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos)
        return path;
    return path.substr(pos + 1);
}
