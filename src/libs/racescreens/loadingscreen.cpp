/***************************************************************************

    file                 : loadingscreen.cpp
    created              : Sun Feb 25 00:34:46 /etc/localtime 2001
    copyright            : (C) 2000-2014 by Eric Espie, Bernhard Wymann
    email                : eric.espie@torcs.org
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

/** @file
    Loading screen.
    @author	Eric Espie, Bernhard Wymann
    @version	$Id$
*/

#include <cstdlib>
#ifdef WIN32
#include <windows.h>
#endif
#include <tgfclient.h>
#include <car.h>

static void *menuHandle = nullptr;
#define TEXTLINES 23
static int rmTextId[TEXTLINES];
static char *rmTextLines[TEXTLINES] = {0};
static int rmCurText;

float black[4] = { 0.0, 0.0, 0.0, 0.0 };
float white[TEXTLINES][4];
float accent[4] = { 1.0, 0.58, 0.08, 1.0 };
float body[4] = { 0.84, 0.87, 0.93, 1.0 };
float muted[4] = { 0.58, 0.63, 0.75, 1.0 };



static void rmDeativate(void * /* dummy */)
{
}


/** @brief Set up loading screen
 *  @ingroup racemantools
 *  @param title Screen title.
 *  @param bgimg Optionnal backgrounf image (nullptr for no img).
*/
void RmLoadingScreenStart(const char *title, const char *bgimg)
{
	int i;
	int y;
	
	if (GfuiScreenIsActive(menuHandle)) {
		/* Already active */
		return;
	}
	
	if (menuHandle) {
		GfuiScreenRelease(menuHandle);
	}
	menuHandle = GfuiScreenCreateEx(black, nullptr, nullptr, nullptr, rmDeativate, 0);
	
	GfuiTitleCreate(menuHandle, title, strlen(title));
	GfuiLabelCreateEx(menuHandle, "SESSION LOAD", accent, GFUI_FONT_SMALL_C, 110, 660, GFUI_ALIGN_HL_VB, 0);
	GfuiLabelCreateEx(menuHandle,
			"Building the race weekend, loading the field, and preparing the start sequence.",
			body,
			GFUI_FONT_SMALL_C,
			110,
			620,
			GFUI_ALIGN_HL_VB,
			0);
	GfuiLabelCreateEx(menuHandle,
			"Live status scrolls below as the circuit and competitors come online.",
			muted,
			GFUI_FONT_SMALL_C,
			110,
			594,
			GFUI_ALIGN_HL_VB,
			0);
	
	/* create TEXTLINES lines of text */
	for (i = 0, y = 530; i < TEXTLINES; i++, y -= 18) {
		white[i][0] = white[i][1] = white[i][2] = 1.0;
		white[i][3] = (float)i * 0.0421 + 0.2;
		rmTextId[i] = GfuiLabelCreateEx(menuHandle, "", white[i], GFUI_FONT_MEDIUM_C, 110, y,
						GFUI_ALIGN_HL_VB, 100);
		if (rmTextLines[i]) {
			/* free old text */
			free(rmTextLines[i]);
			rmTextLines[i] = nullptr;
		}
	}
	
	rmCurText = 0;
	
	if (bgimg) {
		GfuiScreenAddBgImg(menuHandle, bgimg);
	}
	
	GfuiScreenActivate(menuHandle);
	GfuiDisplay();
}


/** @brief Shut down loading screen
 *  @ingroup racemantools
 */
void RmShutdownLoadingScreen(void)
{
	if (menuHandle) {
		GfuiScreenRelease(menuHandle);
		menuHandle = 0;
		// TODO: release rmTextLines here instead of in RmLoadingScreenStart, or both?
	}
}


/** @brief Set a new line of text on the loading screen
 *  @ingroup racemantools
 *  @param[in] text Text to display
 */
void RmLoadingScreenSetText(const char *text)
{
	int i, j;
	
	GfOut("%s\n", text);
	
	if (menuHandle) {
		if (text) {
			if (rmTextLines[rmCurText]) {
				free(rmTextLines[rmCurText]);
			}
			rmTextLines[rmCurText] = strdup(text);
			rmCurText = (rmCurText + 1) % TEXTLINES;
		}
		
		i = rmCurText;
		j = 0;
		do {
			if (rmTextLines[i]) {
				GfuiLabelSetText(menuHandle, rmTextId[j], rmTextLines[i]);
			}
			j++;
			i = (i + 1) % TEXTLINES;
		} while (i != rmCurText);
		
		GfuiDisplay();
	}
}