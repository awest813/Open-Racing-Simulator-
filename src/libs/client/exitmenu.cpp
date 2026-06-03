/***************************************************************************

    file                 : exitmenu.cpp
    created              : Sat Mar 18 23:42:12 CET 2000
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
#include <tgfclient.h>
#include "exitmenu.h"
#include "mainmenu.h"

static void 
endofprog(void * /* dummy */)
{
    STOP_ACTIVE_PROFILES();
    PRINT_PROFILE();
/*     glutSetKeyRepeat(GLUT_KEY_REPEAT_ON); */
    GfScrShutdown();
    exit(0);
}

static void *exitmenuHandle = nullptr;
static void *exitMainMenuHandle = nullptr;


namespace {

float kAccentColor[4] = {1.0f, 0.58f, 0.08f, 1.0f};
float kBodyColor[4] = {0.83f, 0.86f, 0.92f, 1.0f};
float kMutedColor[4] = {0.56f, 0.62f, 0.74f, 1.0f};

} // namespace

void * exitMenuInit(void *menu, void *menuHandle)
{
    if (menuHandle) {
		GfuiScreenRelease(menuHandle);
    }
    
    menuHandle = GfuiScreenCreateEx(nullptr, 
				    nullptr, nullptr, 
				    nullptr, nullptr, 
				    1);
    GfuiScreenAddBgImg(menuHandle, "data/img/splash-quit.png");

    GfuiLabelCreateEx(menuHandle,
		    "TERMINATE SESSION",
		    kAccentColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    660,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiTitleCreate(menuHandle, "EXIT SIMULATION", 0);

    GfuiLabelCreateEx(menuHandle,
		    "Are you sure you want to end your current session and return to the desktop?",
		    kBodyColor,
		    GFUI_FONT_MEDIUM_C,
		    110,
		    620,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(menuHandle,
		    "Any unsaved progress on the current race weekend or configurations will be lost.",
		    kMutedColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    582,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(menuHandle,
		    "CONFIRMATION",
		    kAccentColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    220,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(menuHandle,
		    "Select Exit to OS to shut down the simulation, or Continue to return to the active screen.",
		    kBodyColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    192,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiMenuButtonCreate(menuHandle,
		      "No, Continue",
		      "Return to the game",
		      menu,
		      GfuiScreenActivate);
    
    GfuiMenuButtonCreate(menuHandle,
		      "Exit to OS",
		      "Exit the simulation and return to the OS",
		      nullptr,
		      endofprog);

    GfuiAddKey(menuHandle, (unsigned char)27, "Return to the game", menu, GfuiScreenActivate, nullptr);
    return menuHandle;
}

/*
 * Function
 *	TorcsExitMenuInit
 *
 * Description
 *	init the exit menus
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
void * TorcsExitMenuInit(void *menu)
{
	exitmenuHandle = exitMenuInit(menu, exitmenuHandle);
	return exitmenuHandle;
}


void * TorcsMainExitMenuInit(void *mainMenu)
{
	exitMainMenuHandle = exitMenuInit(mainMenu, exitMainMenuHandle);
	return exitMainMenuHandle;
}
