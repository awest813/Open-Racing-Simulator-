/***************************************************************************

    file                 : mainmenu.cpp
    created              : Sat Mar 18 23:42:38 CET 2000
    copyright            : (C) 2000 by Eric Espie
    email                : torcs@free.fr
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


#include <cstdio>
#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <tgfclient.h>
#include <singleplayer.h>
#include <driverconfig.h>
#include <careermenu.h>

#include "mainmenu.h"
#include "exitmenu.h"
#include "optionmenu.h"

namespace {

float kAccentColor[4] = {1.0f, 0.58f, 0.08f, 1.0f};
float kBodyColor[4] = {0.83f, 0.86f, 0.92f, 1.0f};
float kMutedColor[4] = {0.56f, 0.62f, 0.74f, 1.0f};

// Return the first existing splash image path from the preferred source-tree
// layout, falling back to the legacy packaged asset layout when needed.
// The resolved path is cached for the life of the process and resolved relative
// to the current working directory when the simulator starts.
std::string
GetMainMenuBackgroundPath()
{
    static std::once_flag pathInitOnce;
    static std::string cachedBackgroundPath;

    std::call_once(pathInitOnce, [] {
        // The source-tree layout is preferred, but the legacy packaged layout is
        // still used by some runtime directories.
        const char* kCandidatePaths[] = {
            "data/img/splash-main.png",
            "data/data/img/splash-main.png",
        };

        std::string checkedPaths;
        for (const char* path : kCandidatePaths) {
            if (!checkedPaths.empty()) {
                checkedPaths += ", ";
            }
            checkedPaths += path;

            if (std::filesystem::exists(path)) {
                cachedBackgroundPath = path;
                return;
            }
        }

        std::fprintf(stderr,
                     "Warning: Main menu splash image not found. Launch from the repository root or verify asset installation. Searched: %s\n",
                     checkedPaths.c_str());
        cachedBackgroundPath.reset();
    });

    return cachedBackgroundPath;
}

} // namespace


void *menuHandle = nullptr;
tModList *RacemanModLoaded = nullptr;

static void
TorcsMainMenuActivate(void * /* dummy */)
{
	if (RacemanModLoaded != nullptr) {
		GfModUnloadList(&RacemanModLoaded);
	}
}

/*
 * Function
 *	TorcsMainMenuInit
 *
 * Description
 *	init the main menus
 *
 * Parameters
 *	none
 *
 * Return
 *	0 ok -1 nok
 *
 * Remarks
 *	
 */
int
TorcsMainMenuInit(void)
{
    const int infoLeft = 110;

    menuHandle = GfuiScreenCreateEx(nullptr, 
				    nullptr, TorcsMainMenuActivate, 
				    nullptr, nullptr, 
				    1);

    const std::string backgroundPath = GetMainMenuBackgroundPath();
    if (!backgroundPath.empty()) {
        GfuiScreenAddBgImg(menuHandle, backgroundPath.c_str());
    }

    GfuiLabelCreateEx(menuHandle,
		    "OPEN RACING SIMULATOR",
		    kAccentColor,
		    GFUI_FONT_SMALL_C,
		    infoLeft,
		    660,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiTitleCreate(menuHandle, "ORS // MAIN MENU", 0);

    GfuiLabelCreateEx(menuHandle,
		    "Choose a race mode, configure your setup, and launch straight onto the grid.",
		    kBodyColor,
		    GFUI_FONT_MEDIUM_C,
		    infoLeft,
		    620,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(menuHandle,
		    "Career seasons, quick races, player profiles, and options are arranged",
		    kMutedColor,
		    GFUI_FONT_SMALL_C,
		    infoLeft,
		    582,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(menuHandle,
		    "as a streamlined hub so the path from menu to green flag stays fast and clear.",
		    kMutedColor,
		    GFUI_FONT_SMALL_C,
		    infoLeft,
		    558,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(menuHandle,
		    "QUICK NAVIGATION",
		    kAccentColor,
		    GFUI_FONT_SMALL_C,
		    infoLeft,
		    220,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(menuHandle,
		    "Use arrow keys to move, Enter to select, and Esc to go back or quit.",
		    kBodyColor,
		    GFUI_FONT_SMALL_C,
		    infoLeft,
		    192,
		    GFUI_ALIGN_HL_VB,
		    0);

    const char* versionString =
#ifdef VERSION
		    VERSION;
#else
		    "dev";
#endif
    char versionLabel[256];
    std::snprintf(versionLabel, sizeof(versionLabel), "Version %s", versionString);
    GfuiLabelCreateEx(menuHandle,
		    versionLabel,
		    kMutedColor,
		    GFUI_FONT_SMALL_C,
		    infoLeft,
		    54,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiMenuButtonCreate(menuHandle,
			 "Career", "Season-long progression with standings pressure, long-term rivalries, and a championship arc",
			 CareerMenuInit(menuHandle), GfuiScreenActivate);

    GfuiMenuButtonCreate(menuHandle,
			 "Race", "Jump into the modern launch flow to pick an event type, tune the field, and roll to the grid",
			 ReSinglePlayerInit(menuHandle), GfuiScreenActivate);

    GfuiMenuButtonCreate(menuHandle,
			 "Player Profiles", "Dial in drivers, control presets, and profile-specific garage preferences",
			 TorcsDriverMenuInit(menuHandle), GfuiScreenActivate);

    GfuiMenuButtonCreate(menuHandle,
			 "Options", "Adjust visuals, sound, and simulation settings for a sharper race-day presentation",
			 TorcsOptionOptionInit(menuHandle), GfuiScreenActivate);
    
    GfuiMenuDefaultKeysAdd(menuHandle);

    GfuiMenuBackQuitButtonCreate(menuHandle,
				 "Quit", "Exit the game",
				 TorcsMainExitMenuInit(menuHandle), GfuiScreenActivate);

    return 0;
}

/*
 * Function
 *	
 *
 * Description
 *	
 *
 * Parameters
 *	
 *
 * Return
 *	
 *
 * Remarks
 *	
 */
int
TorcsMainMenuRun(void)
{
    GfuiScreenActivate(menuHandle);
    return 0;
}
