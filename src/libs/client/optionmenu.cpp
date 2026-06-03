/***************************************************************************

    file                 : optionmenu.cpp
    created              : Mon Apr 24 14:22:53 CEST 2000
    copyright            : (C) 2000, 2001 by Eric Espie
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
#include "optionmenu.h"
#include <graphconfig.h>
#include <simuconfig.h>
#include <soundconfig.h>
#include <openglconfig.h>

namespace {

float kAccentColor[4] = {1.0f, 0.58f, 0.08f, 1.0f};
float kBodyColor[4] = {0.83f, 0.86f, 0.92f, 1.0f};
float kMutedColor[4] = {0.56f, 0.62f, 0.74f, 1.0f};

} // namespace

static void *optionHandle = nullptr;

void *
TorcsOptionOptionInit(void *precMenu)
{
    if (optionHandle) return optionHandle;

    optionHandle = GfuiScreenCreateEx(nullptr, 
				      nullptr, nullptr, 
				      nullptr, nullptr, 
				      1);

    GfuiScreenAddBgImg(optionHandle, "data/img/splash-options.png");

    GfuiLabelCreateEx(optionHandle,
		    "SETTINGS & PREFERENCES",
		    kAccentColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    660,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiTitleCreate(optionHandle, "TORCS // SETTINGS", 0);

    GfuiLabelCreateEx(optionHandle,
		    "Configure visual rendering, audio outputs, controller layouts, and physical simulation behavior.",
		    kBodyColor,
		    GFUI_FONT_MEDIUM_C,
		    110,
		    620,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(optionHandle,
		    "Dial in advanced OpenGL details and sound volumes to match your hardware's capability",
		    kMutedColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    582,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(optionHandle,
		    "so the on-track representation runs with maximum responsiveness.",
		    kMutedColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    558,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(optionHandle,
		    "QUICK TIPS",
		    kAccentColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    220,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiLabelCreateEx(optionHandle,
		    "Focus a settings category to preview configuration options.",
		    kBodyColor,
		    GFUI_FONT_SMALL_C,
		    110,
		    192,
		    GFUI_ALIGN_HL_VB,
		    0);

    GfuiMenuButtonCreate(optionHandle,
			 "Display", "Configure screen resolution and display settings",
			 GfScrMenuInit(optionHandle), GfuiScreenActivate);

    GfuiMenuButtonCreate(optionHandle,
			 "Graphics", "Configure rendering quality and visual effects",
			 GraphMenuInit(optionHandle), GfuiScreenActivate);

    GfuiMenuButtonCreate(optionHandle,
			 "Sound", "Configure sound volume and audio settings",
			 SoundMenuInit(optionHandle), GfuiScreenActivate);

    GfuiMenuButtonCreate(optionHandle,
			 "OpenGL", "Configure advanced OpenGL rendering options",
			 OpenGLMenuInit(optionHandle), GfuiScreenActivate);

    GfuiMenuButtonCreate(optionHandle,
			 "Simulation", "Configure physics simulation engine",
			 SimuMenuInit(optionHandle), GfuiScreenActivate);

    GfuiMenuDefaultKeysAdd(optionHandle);

    GfuiMenuBackQuitButtonCreate(optionHandle,
				 "Back",
				 "Return to Main Menu",
				 precMenu,
				 GfuiScreenActivate);

    return optionHandle;
}
