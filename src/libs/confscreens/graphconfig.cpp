/***************************************************************************

    file                 : graphconfig.cpp
    created              : Sun Jun  9 16:30:25 CEST 2002
    copyright            : (C) 2001 by Eric Espie
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
    		
    @author	<a href=mailto:eric.espie@torcs.org>Eric Espie</a>
    @version	$Id$
*/

#include <cstdio>
#include <cstdlib>
#include <tgfclient.h>
#include <graphic.h>
#include <raceinit.h>
#include <portability.h>

#include "graphconfig.h"

static void	*scrHandle = nullptr;
static int	FovEditId;
static int	FovFactorValue = 100;
static int	SmokeEditId;
static int	SmokeValue = 300;
static int	SkidEditId;
static int	SkidValue = 20;
static int	LodFactorEditId;
static tdble	LodFactorValue = 1.0;

// Wheel detail.
static const char *wheelDetailOptionList[] = {
	GR_ATT_WHEELRENDERING_DETAILED,
	GR_ATT_WHEELRENDERING_SIMPLE
};

static float LabelColor[] = {1.0f, 0.58f, 0.08f, 1.0f};
static const int nbOptionsWheelDetail = sizeof(wheelDetailOptionList) / sizeof(wheelDetailOptionList[0]);
static int curOptionWheelDetail = 0;
static int WheelDetailOptionId;

// Graphics Engine / Renderer option.
static const char *rendererOptionList[] = {
	"ssggraph",
	"oglgraph"
};
static const char *rendererDisplayList[] = {
	"Legacy (PLIB SSG)",
	"Modern (OpenGL 3.3)"
};
static const int nbOptionsRenderer = sizeof(rendererOptionList) / sizeof(rendererOptionList[0]);
static int curOptionRenderer = 0;
static int RendererOptionId;





static void ExitGraphicOptions(void *prevMenu)
{
	scrHandle = nullptr;
	GfuiScreenActivate(prevMenu);
}




static void SaveGraphicOptions(void *prevMenu)
{
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	
	snprintf(buf, BUFSIZE, "%s%s", GetLocalDir(), GR_PARAM_FILE);
	void * grHandle = GfParmReadFile(buf, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);

	GfParmSetNum(grHandle, GR_SCT_GRAPHIC, GR_ATT_FOVFACT, "%", FovFactorValue);
	GfParmSetNum(grHandle, GR_SCT_GRAPHIC, GR_ATT_SMOKENB, nullptr, SmokeValue);
	GfParmSetNum(grHandle, GR_SCT_GRAPHIC, GR_ATT_MAXSTRIPBYWHEEL, nullptr, SkidValue);
	GfParmSetNum(grHandle, GR_SCT_GRAPHIC, GR_ATT_LODFACTOR, nullptr, LodFactorValue);	
	GfParmSetStr(grHandle, GR_SCT_GRAPHIC, GR_ATT_WHEELRENDERING, wheelDetailOptionList[curOptionWheelDetail]);	
	
	GfParmWriteFile(nullptr, grHandle, "graph");
	GfParmReleaseHandle(grHandle);

	// Save graphics engine to raceengine.xml
	char re_buf[BUFSIZE];
	snprintf(re_buf, BUFSIZE, "%s%s", GetLocalDir(), RACE_ENG_CFG);
	void *reHandle = GfParmReadFile(re_buf, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);
	GfParmSetStr(reHandle, "Modules", "graphic", rendererOptionList[curOptionRenderer]);
	GfParmWriteFile(nullptr, reHandle, "raceengine");
	GfParmReleaseHandle(reHandle);

	ExitGraphicOptions(prevMenu);
}





static void ChangeFov(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	val = GfuiEditboxGetString(scrHandle, FovEditId);
	FovFactorValue = strtol(val, nullptr, 0);
	snprintf(buf, BUFSIZE, "%d", FovFactorValue);
	GfuiEditboxSetString(scrHandle, FovEditId, buf);
}




static void ChangeLodFactor(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	val = GfuiEditboxGetString(scrHandle, LodFactorEditId);
	sscanf(val, "%g", &LodFactorValue);
	snprintf(buf, BUFSIZE, "%g", LodFactorValue);
	GfuiEditboxSetString(scrHandle, LodFactorEditId, buf);
}




static void ChangeSmoke(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	val = GfuiEditboxGetString(scrHandle, SmokeEditId);
	SmokeValue = strtol(val, nullptr, 0);
	snprintf(buf, BUFSIZE, "%d", SmokeValue);
	GfuiEditboxSetString(scrHandle, SmokeEditId, buf);
}




static void ChangeSkid(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	val = GfuiEditboxGetString(scrHandle, SkidEditId);
	SkidValue = strtol(val, nullptr, 0);
	snprintf(buf, BUFSIZE, "%d", SkidValue);
	GfuiEditboxSetString(scrHandle, SkidEditId, buf);
}




static void changeWheelDetailState(void *vp)
{
	if (vp == 0) {
		curOptionWheelDetail--;
		if (curOptionWheelDetail < 0) {
	    	curOptionWheelDetail = nbOptionsWheelDetail - 1;
		}
	} else {
		curOptionWheelDetail++;
		if (curOptionWheelDetail == nbOptionsWheelDetail) {
	    	curOptionWheelDetail = 0;
		}
	}
	GfuiLabelSetText(scrHandle, WheelDetailOptionId, wheelDetailOptionList[curOptionWheelDetail]);
}


static void changeRendererState(void *vp)
{
	if (vp == 0) {
		curOptionRenderer--;
		if (curOptionRenderer < 0) {
			curOptionRenderer = nbOptionsRenderer - 1;
		}
	} else {
		curOptionRenderer++;
		if (curOptionRenderer == nbOptionsRenderer) {
			curOptionRenderer = 0;
		}
	}
	GfuiLabelSetText(scrHandle, RendererOptionId, rendererDisplayList[curOptionRenderer]);
}





void *GraphMenuInit(void *prevMenu)
{
	const int	x = 50;
	int y = 370;
	const int x2 = 220;
	const int dy = 30;
	const int width = 130;
	const int center = x2 + width/2;
	
	/* screen already created */
	if (scrHandle) {
		return scrHandle;
	}
	
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	scrHandle = GfuiMenuScreenCreate("Graphics Configuration");
	
	GfuiScreenAddBgImg(scrHandle, "data/img/splash-graphconf.png");
	
	snprintf(buf, BUFSIZE, "%s%s", GetLocalDir(), GR_PARAM_FILE);
	void * grHandle = GfParmReadFile(buf, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);	
	
	GfuiLabelCreate(scrHandle, "Visibility factor (%):", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	FovFactorValue = (int)GfParmGetNum(grHandle, GR_SCT_GRAPHIC, GR_ATT_FOVFACT, "%", 100.0);
	snprintf(buf, BUFSIZE, "%d", FovFactorValue);
	FovEditId = GfuiEditboxCreate(scrHandle, buf, GFUI_FONT_MEDIUM_C,
					x2+10, y, width-20, 16, nullptr, nullptr, ChangeFov);
	
	y -= dy;
	GfuiLabelCreate(scrHandle, "Smoke particles:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	SmokeValue = (int)GfParmGetNum(grHandle, GR_SCT_GRAPHIC, GR_ATT_SMOKENB, nullptr, 300.0);
	snprintf(buf, BUFSIZE, "%d", SmokeValue);
	SmokeEditId = GfuiEditboxCreate(scrHandle, buf, GFUI_FONT_MEDIUM_C,
					x2+10, y, width-20, 16, nullptr, nullptr, ChangeSmoke);
	
	y -= dy;
	GfuiLabelCreate(scrHandle, "Skid mark strips:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	SkidValue = (int)GfParmGetNum(grHandle, GR_SCT_GRAPHIC, GR_ATT_MAXSTRIPBYWHEEL, nullptr, 20.0);
	snprintf(buf, BUFSIZE, "%d", SkidValue);
	SkidEditId = GfuiEditboxCreate(scrHandle, buf, GFUI_FONT_MEDIUM_C,
					x2+10, y, width-20, 16, nullptr, nullptr, ChangeSkid);
	
	y -= dy;
	GfuiLabelCreate(scrHandle, "Level of detail factor:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	LodFactorValue = GfParmGetNum(grHandle, GR_SCT_GRAPHIC, GR_ATT_LODFACTOR, nullptr, 1.0);
	snprintf(buf, BUFSIZE, "%g", LodFactorValue);
	LodFactorEditId = GfuiEditboxCreate(scrHandle, buf, GFUI_FONT_MEDIUM_C,
					x2+10, y, width-20, 16, nullptr, nullptr, ChangeLodFactor);
	
					
	// Wheel detail option
	y -= dy;
	GfuiLabelCreate(scrHandle, "Wheel rendering:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-left.png", "data/img/arrow-left.png",
		"data/img/arrow-left.png", "data/img/arrow-left-pushed.png",
		x2, y-5, GFUI_ALIGN_HL_VB, 1,
		(void*)-1, changeWheelDetailState,
		nullptr, nullptr, nullptr);

	GfuiGrButtonCreate(scrHandle, "data/img/arrow-right.png", "data/img/arrow-right.png",
		"data/img/arrow-right.png", "data/img/arrow-right-pushed.png",
		x2+width, y-5, GFUI_ALIGN_HR_VB, 1,
		(void*)1, changeWheelDetailState,
		nullptr, nullptr, nullptr);

	// Read wheel detail parameter.
	int i;
	const char *optionName = GfParmGetStr(grHandle, GR_SCT_GRAPHIC, GR_ATT_WHEELRENDERING, wheelDetailOptionList[0]);
	for (i = 0; i < nbOptionsWheelDetail; i++) {
		if (strcmp(optionName, wheelDetailOptionList[i]) == 0) {
			curOptionWheelDetail = i;
			break;
		}
	}
				
	WheelDetailOptionId = GfuiLabelCreate(scrHandle, wheelDetailOptionList[curOptionWheelDetail], GFUI_FONT_MEDIUM_C, center, y, GFUI_ALIGN_HC_VB, 32);
	GfuiLabelSetColor(scrHandle, WheelDetailOptionId, LabelColor);

	// Graphics Engine / Renderer Option
	y -= dy;
	GfuiLabelCreate(scrHandle, "Graphics engine:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);

	GfuiGrButtonCreate(scrHandle, "data/img/arrow-left.png", "data/img/arrow-left.png",
		"data/img/arrow-left.png", "data/img/arrow-left-pushed.png",
		x2, y-5, GFUI_ALIGN_HL_VB, 1,
		(void*)-1, changeRendererState,
		nullptr, nullptr, nullptr);

	GfuiGrButtonCreate(scrHandle, "data/img/arrow-right.png", "data/img/arrow-right.png",
		"data/img/arrow-right.png", "data/img/arrow-right-pushed.png",
		x2+width, y-5, GFUI_ALIGN_HR_VB, 1,
		(void*)1, changeRendererState,
		nullptr, nullptr, nullptr);

	// Read graphics engine parameter from raceengine.xml
	char re_buf[BUFSIZE];
	snprintf(re_buf, BUFSIZE, "%s%s", GetLocalDir(), RACE_ENG_CFG);
	void *reHandle = GfParmReadFile(re_buf, GFPARM_RMODE_STD);
	const char *graphicModule = GfParmGetStr(reHandle, "Modules", "graphic", "ssggraph");
	curOptionRenderer = 0;
	for (i = 0; i < nbOptionsRenderer; i++) {
		if (strcmp(graphicModule, rendererOptionList[i]) == 0) {
			curOptionRenderer = i;
			break;
		}
	}
	GfParmReleaseHandle(reHandle);

	RendererOptionId = GfuiLabelCreate(scrHandle, rendererDisplayList[curOptionRenderer], GFUI_FONT_MEDIUM_C, center, y, GFUI_ALIGN_HC_VB, 32);
	GfuiLabelSetColor(scrHandle, RendererOptionId, LabelColor);
										
	// Navigation
	GfuiButtonCreate(scrHandle, "Accept", GFUI_FONT_LARGE, 210, 40, 150, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
				prevMenu, SaveGraphicOptions, nullptr, nullptr, nullptr);
	
	GfuiButtonCreate(scrHandle, "Cancel", GFUI_FONT_LARGE, 430, 40, 150, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
				prevMenu, ExitGraphicOptions, nullptr, nullptr, nullptr);
	
	GfuiAddKey(scrHandle, 27, "Cancel", prevMenu, ExitGraphicOptions, nullptr);
	
	GfParmReleaseHandle(grHandle);
	
	return scrHandle;
}
