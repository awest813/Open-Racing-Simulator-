#ifndef __musicplayer_h__
#define __musicplayer_h__

/***************************************************************************

    file                 : musicplayer.h
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

#define MM_SOUND_PARM_CFG			"config/sound.xml"
#define MM_SCT_SOUND				"Menu Music"
#define MM_ATT_SOUND_ENABLE			"enable"
#define MM_VAL_SOUND_ENABLED		"enabled"
#define MM_VAL_SOUND_DISABLED		"disabled"

/* Radio station base directory (relative to game working directory) */
#define MM_RADIO_BASE_DIR           "data/music/radio"

/* ---------------------------------------------------------------------- */
/* Menu background music                                                    */
/* ---------------------------------------------------------------------- */
extern void startMenuMusic();
extern void stopMenuMusic();

/* ---------------------------------------------------------------------- */
/* In-Game Radio (cRadioEngine facade)                                     */
/* ---------------------------------------------------------------------- */

/** Scan MM_RADIO_BASE_DIR and begin playback on the first available station. */
extern void startRadio();

/** Halt radio playback. */
extern void stopRadio();

/**
 * Pump the radio audio buffers.  Call every ~100 ms (e.g. from a glutTimerFunc).
 * Returns false when nothing is playing.
 */
extern bool updateRadio();

/** Cycle to the next radio station. */
extern void radioNextStation();

/** Cycle to the previous radio station. */
extern void radioPrevStation();

/** Advance to the next track on the current station. */
extern void radioNextTrack();

/** Set master radio volume (0.0 – 1.0). */
extern void radioSetVolume(float vol);

/** Get master radio volume (0.0 – 1.0). */
extern float radioGetVolume();


/** Returns current station name, e.g. "Synthwave_FM". */
extern const char* radioGetStationName();

/** Returns current track filename, e.g. "track1.ogg". */
extern const char* radioGetTrackName();

/** Returns true if the radio is currently playing. */
extern bool isRadioPlaying();

#endif //__musicplayer_h__