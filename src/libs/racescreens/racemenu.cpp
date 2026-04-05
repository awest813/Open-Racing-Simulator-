/***************************************************************************
 *
 *    file                 : racemenu.cpp
 *    created              : Thu May  2 22:02:51 CEST 2002
 *    copyright            : (C) 2001-2014 by Eric Espie, Bernhard Wymann
 *    email                : eric.espie@torcs.org
 *    version              : $Id$
 *
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
    Race options menu.
    @author Bernhard Wymann, Eric Espie
    @version $Id$
*/

#include <cstdlib>
#include <cstdio>
#ifdef WIN32
#include <windows.h>
#endif
#include <tgfclient.h>
#include <track.h>
#include <car.h>
#include <raceman.h>
#include <robot.h>
#include <racescreens.h>
#include <portability.h>

static void *scrHandle;
static tRmRaceParam *rp;
static int rmrpDistance;
static int rmrpLaps;
static int rmrpDuration;
static int rmrpDistId;
static int rmrpLapsId;
static int rmrpDurationId;
static int rmDispModeEditId;
static int rmCurDispMode;
static const char *rmCurDispModeList[] = { RM_VAL_VISIBLE, RM_VAL_INVISIBLE };

namespace {

float kAccentColor[4] = {1.0f, 0.58f, 0.08f, 1.0f};
float kBodyColor[4] = {0.84f, 0.87f, 0.93f, 1.0f};
float kMutedColor[4] = {0.58f, 0.63f, 0.75f, 1.0f};

} // namespace


static void rmrpDeactivate(void *screen)
{
	GfuiScreenRelease(scrHandle);

	if (screen) {
		GfuiScreenActivate(screen);
	}
}


static void rmrpUpdDist(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	val = GfuiEditboxGetString(scrHandle, rmrpDistId);
	rmrpDistance = strtol(val, nullptr, 0);
	if (rmrpDistance == 0) {
		snprintf(buf, BUFSIZE, "---");
	} else {
		snprintf(buf, BUFSIZE, "%d", rmrpDistance);
		rmrpLaps = 0;
		GfuiEditboxSetString(scrHandle, rmrpLapsId, "---");
	}
	GfuiEditboxSetString(scrHandle, rmrpDistId, buf);
}


static void rmrpUpdLaps(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	val = GfuiEditboxGetString(scrHandle, rmrpLapsId);
	rmrpLaps = strtol(val, nullptr, 0);
	if (rmrpLaps == 0) {
		snprintf(buf, BUFSIZE, "---");
	} else {
		snprintf(buf, BUFSIZE, "%d", rmrpLaps);
		rmrpDistance = 0;
		GfuiEditboxSetString(scrHandle, rmrpDistId, "---");
	}
	GfuiEditboxSetString(scrHandle, rmrpLapsId, buf);
}

static void rmrpUpdDuration(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	val = GfuiEditboxGetString(scrHandle, rmrpDurationId);
	rmrpDuration = strtol(val, nullptr, 0);
	snprintf(buf, BUFSIZE, "%d", rmrpDuration);
	GfuiEditboxSetString(scrHandle, rmrpDurationId, buf);
}


static void rmrpValidate(void * /* dummy */)
{
	if (rp->confMask & RM_CONF_RACE_LEN) {
		rmrpUpdDist(0);
		rmrpUpdLaps(0);
		GfParmSetNum(rp->param, rp->title, RM_ATTR_DISTANCE, "km", rmrpDistance);
		GfParmSetNum(rp->param, rp->title, RM_ATTR_LAPS, nullptr, rmrpLaps);
	}

	if (rp->confMask & RM_CONF_RACE_TIME) {
		rmrpUpdDuration(0);
		GfParmSetNum(rp->param, rp->title, RM_ATTR_RACE_TIME, "s", rmrpDuration);
	}

	if (rp->confMask & RM_CONF_DISP_MODE) {
		GfParmSetStr(rp->param, rp->title, RM_ATTR_DISPMODE, rmCurDispModeList[rmCurDispMode]);
	}

	rmrpDeactivate(rp->nextScreen);
}


static void rmrpAddKeys(void)
{
	GfuiAddKey(scrHandle, 27, "Cancel changes", rp->prevScreen, rmrpDeactivate, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_F1, "Help", scrHandle, GfuiHelpScreen, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_F12, "Take a screenshot", nullptr, GfuiScreenShot, nullptr);
	GfuiAddKey(scrHandle, 13, "Accept changes", nullptr, rmrpValidate, nullptr);
}


void rmChangeDisplayMode(void * /* dummy */)
{
	rmCurDispMode = 1 - rmCurDispMode;
	GfuiLabelSetText(scrHandle, rmDispModeEditId, rmCurDispModeList[rmCurDispMode]);
}


/** @brief Race options menu
 *  @ingroup racemantools
 *  @param[in,out] vrp Pointer on tRmRaceParam structure (cast to void)
 *  @note The race manager parameter set is modified in memory but not persisted.
 */
void RmRaceParamMenu(void *vrp)
{
	int y, x, x2, dy, dx;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	rp = (tRmRaceParam*)vrp;
	
	snprintf(buf, BUFSIZE, "%s Setup", rp->title);
	scrHandle = GfuiMenuScreenCreate(buf);
	GfuiScreenAddBgImg(scrHandle, "data/img/splash-raceopt.png");

	GfuiLabelCreateEx(scrHandle, "EVENT SETTINGS", kAccentColor, GFUI_FONT_SMALL_C, 110, 660, GFUI_ALIGN_HL_VB, 0);
	GfuiLabelCreateEx(scrHandle,
			"Fine-tune the race format before you move from setup to the live weekend.",
			kBodyColor,
			GFUI_FONT_MEDIUM_C,
			110,
			620,
			GFUI_ALIGN_HL_VB,
			0);
	GfuiLabelCreateEx(scrHandle,
			"Each value updates the selected event immediately when you confirm the panel.",
			kMutedColor,
			GFUI_FONT_SMALL_C,
			110,
			592,
			GFUI_ALIGN_HL_VB,
			0);

	x = 110;
	x2 = 365;
	y = 370;
	dx = 255;
	dy = GfuiFontHeight(GFUI_FONT_LARGE) + 5;

	GfuiLabelCreateEx(scrHandle, "RACE RULES", kAccentColor, GFUI_FONT_SMALL_C, x, y + 36, GFUI_ALIGN_HL_VB, 0);

	if (rp->confMask & RM_CONF_RACE_LEN) {
		GfuiLabelCreate(scrHandle, "Distance Target (km):", GFUI_FONT_MEDIUM_C, x, y, GFUI_ALIGN_HL_VB, 0);
		rmrpDistance = (int)GfParmGetNum(rp->param, rp->title, RM_ATTR_DISTANCE, "km", 0);
		if (rmrpDistance == 0) {
			snprintf(buf, BUFSIZE, "---");
			rmrpLaps = (int)GfParmGetNum(rp->param, rp->title, RM_ATTR_LAPS, nullptr, 25);
		} else {
			snprintf(buf, BUFSIZE, "%d", rmrpDistance);
			rmrpLaps = 0;
		}
		
		rmrpDistId = GfuiEditboxCreate(scrHandle, buf, GFUI_FONT_MEDIUM_C,
						x + dx, y,
						0, 8, nullptr, nullptr, rmrpUpdDist);

		y -= dy;
		GfuiLabelCreate(scrHandle, "Lap Count:", GFUI_FONT_MEDIUM_C, x, y, GFUI_ALIGN_HL_VB, 0);
		if (rmrpLaps == 0) {
			snprintf(buf, BUFSIZE, "---");
		} else {
			snprintf(buf, BUFSIZE, "%d", rmrpLaps);
		}
		
		rmrpLapsId = GfuiEditboxCreate(scrHandle, buf, GFUI_FONT_MEDIUM_C,
						x + dx, y,
						0, 8, nullptr, nullptr, rmrpUpdLaps);
		y -= dy;
	}

	if (rp->confMask & RM_CONF_RACE_TIME) {
		GfuiLabelCreate(scrHandle, "Session Time (s):", GFUI_FONT_MEDIUM_C, x, y, GFUI_ALIGN_HL_VB, 0);
		rmrpDuration = (int)GfParmGetNum(rp->param, rp->title, RM_ATTR_RACE_TIME, "s", 60);
		snprintf(buf, BUFSIZE, "%d", rmrpDuration);

		rmrpDurationId = GfuiEditboxCreate(scrHandle, buf, GFUI_FONT_MEDIUM_C,
						x + dx, y,
						0, 8, nullptr, nullptr, rmrpUpdDuration);
		y -= dy;
	}

	if (rp->confMask & RM_CONF_DISP_MODE) {
		GfuiLabelCreate(scrHandle, "Field Visibility:", GFUI_FONT_MEDIUM_C, x, y, GFUI_ALIGN_HL_VB, 0);
		GfuiGrButtonCreate(scrHandle, "data/img/arrow-left.png", "data/img/arrow-left.png",
				"data/img/arrow-left.png", "data/img/arrow-left-pushed.png",
				x2, y, GFUI_ALIGN_HL_VB, 1,
				(void*)0, rmChangeDisplayMode,
				nullptr, nullptr, nullptr);	    
		GfuiGrButtonCreate(scrHandle, "data/img/arrow-right.png", "data/img/arrow-right.png",
				"data/img/arrow-right.png", "data/img/arrow-right-pushed.png",
				x2 + 150, y, GFUI_ALIGN_HL_VB, 1,
				(void*)1, rmChangeDisplayMode,
				nullptr, nullptr, nullptr);
		if (!strcmp(GfParmGetStr(rp->param, rp->title, RM_ATTR_DISPMODE, RM_VAL_VISIBLE), RM_VAL_INVISIBLE)) {
			rmCurDispMode = 1;
		} else {
			rmCurDispMode = 0;
		}
		rmDispModeEditId = GfuiLabelCreate(scrHandle, rmCurDispModeList[rmCurDispMode], GFUI_FONT_MEDIUM_C, x2 + 35, y, GFUI_ALIGN_HL_VB, 20);
		y -= dy;
	}

	GfuiLabelCreateEx(scrHandle,
			"Distance and laps remain linked so you can tune the event around either target.",
			kMutedColor,
			GFUI_FONT_SMALL_C,
			110,
			200,
			GFUI_ALIGN_HL_VB,
			0);

	GfuiButtonCreate(scrHandle, "Apply Setup", GFUI_FONT_LARGE, 210, 40, 170, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
				nullptr, rmrpValidate, nullptr, nullptr, nullptr);

	GfuiButtonCreate(scrHandle, "Back", GFUI_FONT_LARGE, 430, 40, 150, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
				rp->prevScreen, rmrpDeactivate, nullptr, nullptr, nullptr);

	rmrpAddKeys();

	GfuiScreenActivate(scrHandle);
}
