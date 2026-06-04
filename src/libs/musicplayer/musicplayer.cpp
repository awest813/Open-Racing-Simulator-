/***************************************************************************

    file                 : musicplayer.cpp
    created              : Fri Dec 23 17:35:18 CET 2011
    copyright            : (C) 2011 Bernhard Wymann
    email                : berniw@bluewin.ch
    version              : $Id$

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "musicplayer.h"

#include <glutshim.h>
#include <cstring>
#include <tgf.h>
#include <portability.h>

#ifdef TORCS_USE_MINIAUDIO
#include "MiniAudioPlayer.h"
static MiniAudioPlayer* getMusicPlayer()
{
	// Build the full path the same way the OpenAL path does.
	static char s_path[1024];
	snprintf(s_path, sizeof(s_path), "%sdata/music/torcs1.ogg", GetLocalDir());
	static MiniAudioPlayer player(s_path);
	return &player;
}
#define PLAYER_START()   getMusicPlayer()->start()
#define PLAYER_STOP()    getMusicPlayer()->stop()
#define PLAYER_REWIND()  getMusicPlayer()->rewind()
#define PLAYER_PUMP()    getMusicPlayer()->playAndManageBuffer()
#else
#include "OggSoundStream.h"
#include "OpenALMusicPlayer.h"
static SoundStream* getMenuSoundStream(char* oggFilePath)
{
	static OggSoundStream stream(oggFilePath);
	return &stream;
}
static OpenALMusicPlayer* getMusicPlayer()
{
	static OpenALMusicPlayer player(getMenuSoundStream(
	    const_cast<char*>("data/music/torcs1.ogg")));
	return &player;
}
#define PLAYER_START()   getMusicPlayer()->start()
#define PLAYER_STOP()    getMusicPlayer()->stop()
#define PLAYER_REWIND()  getMusicPlayer()->rewind()
#define PLAYER_PUMP()    getMusicPlayer()->playAndManageBuffer()
#endif

#include "RadioEngine.h"


static bool isEnabled()
{
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	snprintf(buf, BUFSIZE, "%s%s", GetLocalDir(), MM_SOUND_PARM_CFG);
	bool enabled = false;
	
	void *handle = GfParmReadFile(buf, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);
	const char* s = GfParmGetStr(handle, MM_SCT_SOUND, MM_ATT_SOUND_ENABLE, MM_VAL_SOUND_DISABLED);
	
	if (strcmp(s, MM_VAL_SOUND_ENABLED) == 0) {
		enabled = true;
	}
	
	GfParmReleaseHandle(handle);
	return enabled;
}

static void playMenuMusicPump(int /* value */)
{
	const int nextcallinms = 100;
	if (PLAYER_PUMP()) {
		glutTimerFunc(nextcallinms, playMenuMusicPump, 0);
	}
}

void startMenuMusic()
{
	if (isEnabled()) {
		PLAYER_START();
		playMenuMusicPump(0);
	}
}

void stopMenuMusic()
{
	PLAYER_STOP();
	PLAYER_REWIND();
}


/* ======================================================================== */
/* In-Game Radio – thin C facade over RadioEngine singleton                  */
/* ======================================================================== */

static void pumpRadio(int /* value */)
{
	const int nextcallinms = 100;
	if (updateRadio()) {
		glutTimerFunc(nextcallinms, pumpRadio, 0);
	}
}


void startRadio()
{
	RadioEngine& radio = RadioEngine::instance();
	radio.scanStations(MM_RADIO_BASE_DIR);
	radio.startRadio();

	if (radio.isPlaying()) {
		// Kick off the recurring buffer-pump timer (same pattern as menu music).
		glutTimerFunc(100, pumpRadio, 0);
	}
}


void stopRadio()
{
	RadioEngine::instance().stopRadio();
}


bool updateRadio()
{
	return RadioEngine::instance().update();
}


void radioNextStation()
{
	RadioEngine::instance().nextStation();
}


void radioPrevStation()
{
	RadioEngine::instance().prevStation();
}


void radioNextTrack()
{
	RadioEngine::instance().nextTrack();
}


void radioSetVolume(float vol)
{
	RadioEngine::instance().setVolume(vol);
}


float radioGetVolume()
{
	return RadioEngine::instance().getVolume();
}



const char* radioGetStationName()
{
	return RadioEngine::instance().getStationName().c_str();
}


const char* radioGetTrackName()
{
	return RadioEngine::instance().getTrackName().c_str();
}


bool isRadioPlaying()
{
	return RadioEngine::instance().isPlaying();
}