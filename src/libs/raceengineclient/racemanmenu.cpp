/***************************************************************************

    file        : racemanmenu.cpp
    created     : Fri Jan  3 22:24:41 CET 2003
    copyright   : (C) 2003-2014 by Eric Espie, Bernhard Wymann
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
#include <racescreens.h>
#include <driverconfig.h>
#include <portability.h>

#include "raceengine.h"
#include "racemain.h"
#include "raceinit.h"
#include "racestate.h"

#include "racemanmenu.h"
static float red[4]  = {1.0, 0.0, 0.0, 1.0};

namespace {

float kAccentColor[4] = {1.0f, 0.58f, 0.08f, 1.0f};
float kBodyColor[4] = {0.84f, 0.87f, 0.93f, 1.0f};
float kMutedColor[4] = {0.58f, 0.63f, 0.75f, 1.0f};

} // namespace

static void *racemanMenuHdle = nullptr;
static void *newTrackMenuHdle = nullptr;
static tRmTrackSelect ts;
static tRmDrvSelect ds;
static tRmRaceParam rp;
static tRmFileSelect fs;

static void reConfigRunState(void);

static void
reConfigBack(void)
{
	void *params = ReInfo->params;

	/* Go back one step in the conf */
	GfParmSetNum(params, RM_SECT_CONF, RM_ATTR_CUR_CONF, nullptr, 
			GfParmGetNum(params, RM_SECT_CONF, RM_ATTR_CUR_CONF, nullptr, 1) - 2);

	reConfigRunState();
}


/***************************************************************/
/* Callback hooks used only to run the automaton on activation */
static void	*configHookHandle = 0;

static void
configHookActivate(void * /* dummy */)
{
	reConfigRunState();
}

static void *
reConfigHookInit(void)
{
	if (configHookHandle) {
		return configHookHandle;
	}

	configHookHandle = GfuiHookCreate(0, configHookActivate);
	return configHookHandle;
}

/***************************************************************/
/* Config Back Hook */

static void	*ConfigBackHookHandle = 0;

static void
ConfigBackHookActivate(void * /* dummy */)
{
	reConfigBack();
}

static void *
reConfigBackHookInit(void)
{
	if (ConfigBackHookHandle) {
		return ConfigBackHookHandle;
	}

	ConfigBackHookHandle = GfuiHookCreate(0, ConfigBackHookActivate);
	return ConfigBackHookHandle;
}

static void
reConfigRunState(void)
{
	int i;
	const char* opt;
	const char* conf;
	int curConf;
	int numOpt;
	void *params = ReInfo->params;
	const int BUFSIZE = 1024;
	char path[BUFSIZE];
	
	curConf = static_cast<int>(GfParmGetNum(params, RM_SECT_CONF, RM_ATTR_CUR_CONF, nullptr, 1));
	if (curConf > GfParmGetEltNb(params, RM_SECT_CONF)) {
		GfOut("End of configuration\n");
		GfParmWriteFile(nullptr, ReInfo->params, ReInfo->_reName);
		goto menuback;
	}
	
	snprintf(path, BUFSIZE, "%s/%d", RM_SECT_CONF, curConf);
	conf = GfParmGetStr(params, path, RM_ATTR_TYPE, 0);
	if (!conf) {
		GfOut("no %s here %s\n", RM_ATTR_TYPE, path);
		goto menuback;
	}
	
	GfOut("Configuration step %s\n", conf);
	if (!strcmp(conf, RM_VAL_TRACKSEL)) {
		/* Track Select Menu */
		ts.nextScreen = reConfigHookInit();
		if (curConf == 1) {
			ts.prevScreen = racemanMenuHdle;
		} else {
			ts.prevScreen = reConfigBackHookInit();
		}
		ts.param = ReInfo->params;
		ts.trackItf = ReInfo->_reTrackItf;
		RmTrackSelect(&ts);	
	} else if (!strcmp(conf, RM_VAL_DRVSEL)) {
		/* Drivers select menu */
		ds.nextScreen = reConfigHookInit();
		if (curConf == 1) {
			ds.prevScreen = racemanMenuHdle;
		} else {
			ds.prevScreen = reConfigBackHookInit();
		}
		ds.param = ReInfo->params;
		RmDriversSelect(&ds);
	
	} else if (!strcmp(conf, RM_VAL_RACECONF)) {
		/* Race Options menu */
		rp.nextScreen = reConfigHookInit();
		if (curConf == 1) {
			rp.prevScreen = racemanMenuHdle;
		} else {
			rp.prevScreen = reConfigBackHookInit();
		}
		rp.param = ReInfo->params;
		rp.title = GfParmGetStr(params, path, RM_ATTR_RACE, "Race");
		/* Select options to configure */
		rp.confMask = 0;
		snprintf(path, BUFSIZE, "%s/%d/%s", RM_SECT_CONF, curConf, RM_SECT_OPTIONS);
		numOpt = GfParmGetEltNb(params, path);
		for (i = 1; i < numOpt + 1; i++) {
			snprintf(path, BUFSIZE, "%s/%d/%s/%d", RM_SECT_CONF, curConf, RM_SECT_OPTIONS, i);
			opt = GfParmGetStr(params, path, RM_ATTR_TYPE, "");
			if (!strcmp(opt, RM_VAL_CONFRACELEN)) {
			/* Configure race length */
			rp.confMask |= RM_CONF_RACE_LEN;
			} else if (!strcmp(opt, RM_VAL_CONFRACETIME)) {
			/* Configure race time */
			rp.confMask |= RM_CONF_RACE_TIME;
			} else if (!strcmp(opt, RM_VAL_CONFDISPMODE)) {
				/* Configure display mode */
				rp.confMask |= RM_CONF_DISP_MODE;
			}
		}
		RmRaceParamMenu(&rp);
	}
	
	curConf++;
	GfParmSetNum(params, RM_SECT_CONF, RM_ATTR_CUR_CONF, nullptr, curConf);
	
	return;

    /* Back to the race menu */
 menuback:
	GfuiScreenActivate(racemanMenuHdle);
	return;
}

static void
reConfigureMenu(void * /* dummy */)
{
	void *params = ReInfo->params;

	/* Reset configuration automaton */
	GfParmSetNum(params, RM_SECT_CONF, RM_ATTR_CUR_CONF, nullptr, 1);
	reConfigRunState();
}

static void
reSelectLoadFile(char *filename)
{
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	
	snprintf(buf, BUFSIZE, "%sresults/%s/%s", GetLocalDir(), ReInfo->_reFilename, filename);
	GfOut("Loading Saved File %s...\n", buf);
	if (ReInfo->results) {
		GfParmReleaseHandle(ReInfo->results);
	}
	ReInfo->results = GfParmReadFile(buf, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);
	ReInfo->_reRaceName = ReInfo->_reName;
	RmShowStandings(ReInfo->_reGameScreen, ReInfo);
}

// FIXME: remove this static shared buffer!
const int VARBUFSIZE = 1024;
char varbuf[VARBUFSIZE];

static void
reLoadMenu(void *prevHandle)
{
	void *params = ReInfo->params;
	
	fs.prevScreen = prevHandle;
	fs.select = reSelectLoadFile;
	
	const char* str = GfParmGetStr(params, RM_SECT_HEADER, RM_ATTR_NAME, 0);
	if (str) {
		fs.title = str;
	}

	snprintf(varbuf, VARBUFSIZE, "%sresults/%s", GetLocalDir(), ReInfo->_reFilename);
	fs.path = varbuf;
	
	RmFileSelect(static_cast<void*>(&fs));
}

int
ReRacemanMenu(void)
{
	void	*params = ReInfo->params;
	const int BUFSIZE = 1024;
	const int infoLeft = 110;
	char buf[BUFSIZE];
	char path[BUFSIZE];

	if (racemanMenuHdle) {
		GfuiScreenRelease(racemanMenuHdle);
	}

	racemanMenuHdle = GfuiScreenCreateEx(nullptr, 
					nullptr, nullptr, 
					nullptr, nullptr, 
					1);

	const char* str = GfParmGetStr(params, RM_SECT_HEADER, RM_ATTR_BGIMG, 0);
	if (str) {
		GfuiScreenAddBgImg(racemanMenuHdle, str);
	}
	
	GfuiMenuDefaultKeysAdd(racemanMenuHdle);

	str = GfParmGetStr(params, RM_SECT_HEADER, RM_ATTR_NAME, 0);
	if (str) {
		GfuiTitleCreate(racemanMenuHdle, str, strlen(str));
	}

	GfuiLabelCreateEx(racemanMenuHdle,
			"WEEKEND OVERVIEW",
			kAccentColor,
			GFUI_FONT_SMALL_C,
			infoLeft,
			660,
			GFUI_ALIGN_HL_VB,
			0);

	const char* modeDescr = GfParmGetStr(params, RM_SECT_HEADER, RM_ATTR_DESCR, "");
	if (modeDescr && modeDescr[0] != '\0') {
		GfuiLabelCreateEx(racemanMenuHdle,
				modeDescr,
				kBodyColor,
				GFUI_FONT_MEDIUM_C,
				infoLeft,
				620,
				GFUI_ALIGN_HL_VB,
				0);
	}

	const int currentTrack = static_cast<int>(GfParmGetNum(params, RM_SECT_TRACKS, RE_ATTR_CUR_TRACK, nullptr, 1));
	snprintf(path, BUFSIZE, "%s/%d", RM_SECT_TRACKS, currentTrack);
	const char* trackCategory = GfParmGetStr(params, path, RM_ATTR_CATEGORY, "");
	const char* trackName = GfParmGetStr(params, path, RM_ATTR_NAME, "");
	char* trackDisplayName = nullptr;
	if (trackCategory[0] != '\0' && trackName[0] != '\0') {
		trackDisplayName = RmGetTrackName(const_cast<char*>(trackCategory), const_cast<char*>(trackName));
	}

	snprintf(buf, BUFSIZE, "Current circuit: %s",
		 (trackDisplayName && trackDisplayName[0] != '\0') ? trackDisplayName : "Circuit not selected");
	GfuiLabelCreateEx(racemanMenuHdle, buf, kBodyColor, GFUI_FONT_SMALL_C, infoLeft, 582, GFUI_ALIGN_HL_VB, 0);

	snprintf(buf, BUFSIZE, "Rounds in event: %d", GfParmGetEltNb(params, RM_SECT_TRACKS));
	GfuiLabelCreateEx(racemanMenuHdle, buf, kMutedColor, GFUI_FONT_SMALL_C, infoLeft, 554, GFUI_ALIGN_HL_VB, 0);

	snprintf(buf, BUFSIZE, "Race sessions: %d", GfParmGetEltNb(params, RM_SECT_RACES));
	GfuiLabelCreateEx(racemanMenuHdle, buf, kMutedColor, GFUI_FONT_SMALL_C, infoLeft, 530, GFUI_ALIGN_HL_VB, 0);

	snprintf(buf, BUFSIZE, "Grid capacity: %d drivers",
		 static_cast<int>(GfParmGetNum(params, RM_SECT_DRIVERS, RM_ATTR_MAXNUM, nullptr, 0)));
	GfuiLabelCreateEx(racemanMenuHdle, buf, kMutedColor, GFUI_FONT_SMALL_C, infoLeft, 506, GFUI_ALIGN_HL_VB, 0);

	GfuiLabelCreateEx(racemanMenuHdle,
			"Use Tune Weekend before launch if you want to change the circuit, roster,",
			kMutedColor,
			GFUI_FONT_SMALL_C,
			infoLeft,
			220,
			GFUI_ALIGN_HL_VB,
			0);
	GfuiLabelCreateEx(racemanMenuHdle,
			"or race rules. Start Event will push straight into the active race flow.",
			kMutedColor,
			GFUI_FONT_SMALL_C,
			infoLeft,
			196,
			GFUI_ALIGN_HL_VB,
			0);

	free(trackDisplayName);

	GfuiMenuButtonCreate(racemanMenuHdle,
			"Start Event", "Launch the prepared event and move directly into the active race weekend",
			nullptr, ReStartNewRace);

	GfuiMenuButtonCreate(racemanMenuHdle, 
			"Tune Weekend", "Adjust the circuit, driver roster, and race parameters before launch",
			nullptr, reConfigureMenu);

/*     GfuiMenuButtonCreate(racemanMenuHdle, */
/* 			 "Configure Players", "Players configuration menu", */
/* 			 TorcsDriverMenuInit(racemanMenuHdle), GfuiScreenActivate); */

	if (GfParmGetEltNb(params, RM_SECT_TRACKS) > 1) {
		GfuiMenuButtonCreate(racemanMenuHdle, 
					"Load Saved Weekend", "Resume a previously saved race weekend from local results data",
					racemanMenuHdle, reLoadMenu);
	}
	
	GfuiMenuBackQuitButtonCreate(racemanMenuHdle,
				"Back to Event Select", "Return to the event-type selection screen",
				ReInfo->_reMenuScreen, GfuiScreenActivate);

	GfuiScreenActivate(racemanMenuHdle);

	return RM_ASYNC | RM_NEXT_STEP;
}

static void
reStateManage(void * /* dummy */)
{
    ReStateManage();
}

int
ReNewTrackMenu(void)
{
	void *params = ReInfo->params;
	void *results = ReInfo->results;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	
	if (newTrackMenuHdle) {
		GfuiScreenRelease(newTrackMenuHdle);
	}

	newTrackMenuHdle = GfuiScreenCreateEx(nullptr, 
						nullptr, nullptr, 
						nullptr, nullptr, 
						1);
	
	const char* str = GfParmGetStr(params, RM_SECT_HEADER, RM_ATTR_BGIMG, 0);
	if (str) {
		GfuiScreenAddBgImg(newTrackMenuHdle, str);
	}

	str = GfParmGetStr(params, RM_SECT_HEADER, RM_ATTR_NAME, "");
	GfuiTitleCreate(newTrackMenuHdle, str, strlen(str));

	GfuiLabelCreateEx(newTrackMenuHdle,
			"RACE DAY",
			kAccentColor,
			GFUI_FONT_SMALL_C,
			110,
			660,
			GFUI_ALIGN_HL_VB,
			0);
	
	GfuiMenuDefaultKeysAdd(newTrackMenuHdle);
	
	snprintf(buf, BUFSIZE, "Race Day #%d/%d on %s",
		static_cast<int>(GfParmGetNum(results, RE_SECT_CURRENT, RE_ATTR_CUR_TRACK, nullptr, 1)),
		GfParmGetEltNb(params, RM_SECT_TRACKS),
		ReInfo->track->name);
	
	GfuiLabelCreateEx(newTrackMenuHdle,
				buf,
				red,
				GFUI_FONT_MEDIUM_C,
				320, 420,
				GFUI_ALIGN_HC_VB, 50);

	GfuiLabelCreateEx(newTrackMenuHdle,
			"Grid is locked. Launch when the field, circuit, and rules feel right.",
			kBodyColor,
			GFUI_FONT_SMALL_C,
			110,
			620,
			GFUI_ALIGN_HL_VB,
			0);
	
	GfuiMenuButtonCreate(newTrackMenuHdle,
				"Launch Session", "Start the current event session from the prepared grid",
				nullptr, reStateManage);
	
	
	GfuiMenuButtonCreate(newTrackMenuHdle, 
				"Abort Weekend", "Exit the prepared event and return to the previous menu",
				ReInfo->_reMenuScreen, GfuiScreenActivate);
	
	GfuiAddKey(newTrackMenuHdle, 27,  "Abort weekend", ReInfo->_reMenuScreen, GfuiScreenActivate, nullptr);
	
	GfuiScreenActivate(newTrackMenuHdle);
	
	return RM_ASYNC | RM_NEXT_STEP;
}
