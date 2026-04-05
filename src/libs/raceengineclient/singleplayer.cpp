/***************************************************************************

    file        : singleplayer.cpp
    created     : Sat Nov 16 09:36:29 CET 2002
    copyright   : (C) 2002 by Eric Espié                        
    email       : eric.espie@torcs.org   
    version     : $Id$                                  

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** @file   
    		
    @author	<a href=mailto:eric.espie@torcs.org>Eric Espie</a>
    @version	$Id$
*/

#include <cstdlib>
#include <cstdio>
#include <tgfclient.h>
#include <raceman.h>

#include "raceengine.h"
#include "racemain.h"
#include "raceinit.h"
#include "racestate.h"

namespace {

float kAccentColor[4] = {1.0f, 0.58f, 0.08f, 1.0f};
float kBodyColor[4] = {0.84f, 0.87f, 0.93f, 1.0f};
float kMutedColor[4] = {0.58f, 0.63f, 0.75f, 1.0f};

} // namespace

static void *singlePlayerHandle = nullptr;

/* Called when the menu is activated */
static void
singlePlayerMenuActivate(void * /* dummy */)
{
    /* Race engine init */
    ReInit();
    ReInfo->_reMenuScreen = singlePlayerHandle;
}

/* Exit from Race engine */
static void
singlePLayerShutdown(void *precMenu)
{
    GfuiScreenActivate(precMenu);
    ReShutdown();
}


/* Initialize the single player menu */
void *
ReSinglePlayerInit(void *precMenu)
{
    if (singlePlayerHandle) return singlePlayerHandle;
    
    singlePlayerHandle = GfuiScreenCreateEx(nullptr, 
					    nullptr, singlePlayerMenuActivate, 
					    nullptr, nullptr, 
					    1);

    GfuiLabelCreateEx(singlePlayerHandle,
		    "EVENT SELECT",
		    kAccentColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    660,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiTitleCreate(singlePlayerHandle, "LAUNCH TO RACE", 0);

    GfuiScreenAddBgImg(singlePlayerHandle, "data/img/splash-single-player.png");

    GfuiLabelCreateEx(singlePlayerHandle,
		    "Choose the format of the weekend before you shape the circuit, roster, and race rules.",
		    kBodyColor,
		    GFUI_FONT_MEDIUM_C,
		    110,
		    620,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(singlePlayerHandle,
		    "Sprint events, practice runs, endurance sessions, and championships all route",
		    kMutedColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    582,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(singlePlayerHandle,
		    "through the same upgraded setup flow so the path to the grid stays consistent.",
		    kMutedColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    558,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(singlePlayerHandle,
		    "Focus a mode to preview its role in the race weekend.",
		    kBodyColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    220,
		    GFUI_ALIGN_HL_VB,
		    0);

    /* Display the raceman button selection */
    ReAddRacemanListButton(singlePlayerHandle);

    GfuiMenuDefaultKeysAdd(singlePlayerHandle);

    ReStateInit(singlePlayerHandle);

    GfuiMenuBackQuitButtonCreate(singlePlayerHandle,
				 "Back to Main", "Return to the main race hub",
				 precMenu, singlePLayerShutdown);
    
    return singlePlayerHandle;
}
