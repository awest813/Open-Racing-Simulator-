/***************************************************************************
                  trackselect.cpp -- interactive track selection
                             -------------------
    created              : Mon Aug 16 21:43:00 CEST 1999
    copyright            : (C) 1999-2014 by Eric Espie, Bernhard Wymann
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

/** @file
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
#include <osspec.h>
#include <raceman.h>
#include <racescreens.h>
#include <portability.h>

namespace {

float kAccentColor[4] = {1.0f, 0.58f, 0.08f, 1.0f};
float kBodyColor[4] = {0.84f, 0.87f, 0.93f, 1.0f};
float kMutedColor[4] = {0.58f, 0.63f, 0.75f, 1.0f};

} // namespace


/* Tracks Categories */
static tFList *CategoryList;
static void *scrHandle;
static int TrackLabelId;
static int CatLabelId;
static int MapId;
static int AuthorId;
static int LengthId;
static int WidthId;
static int DescId;
static int PitsId;
static tRmTrackSelect *ts;


static void rmtsActivate(void * /* dummy */)
{
	/* call display function of graphic */
	//gfuiReleaseImage(MapId);
}


static void rmtsFreeLists(void *vl)
{
	GfDirFreeList((tFList*)vl, nullptr, true, true);
}


static char * rmGetMapName(char* buf, const int BUFSIZE)
{
	snprintf(buf, BUFSIZE, "tracks/%s/%s/%s.png", CategoryList->name,
		((tFList*)CategoryList->userData)->name, ((tFList*)CategoryList->userData)->name);
	return buf;
}


static void rmtsDeactivate(void *screen)
{
	GfuiScreenRelease(scrHandle);

	GfDirFreeList(CategoryList, rmtsFreeLists, true, true);
	if (screen) {
		GfuiScreenActivate(screen);
	}
}


static void rmUpdateTrackInfo(void)
{
	void *trackHandle;
	float tmp;
	tTrack *trk;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	
	snprintf(buf, BUFSIZE, "tracks/%s/%s/%s.%s", CategoryList->name, ((tFList*)CategoryList->userData)->name,
		((tFList*)CategoryList->userData)->name, TRKEXT);
	trackHandle = GfParmReadFile(buf, GFPARM_RMODE_STD); /* COMMENT VALID? don't release, the name is used later */

	if (!trackHandle) {
		GfTrace("File %s has pb\n", buf);
		return;
	}
	trk = ts->trackItf.trkBuild(buf);

	GfuiLabelSetText(scrHandle, DescId, GfParmGetStr(trackHandle, TRK_SECT_HDR, TRK_ATT_DESCR, ""));
	GfuiLabelSetText(scrHandle, AuthorId, GfParmGetStr(trackHandle, TRK_SECT_HDR, TRK_ATT_AUTHOR, ""));

	tmp = GfParmGetNum(trackHandle, TRK_SECT_MAIN, TRK_ATT_WIDTH, nullptr, 0);
	snprintf(buf, BUFSIZE, "%.2f m", tmp);
	GfuiLabelSetText(scrHandle, WidthId, buf);
	tmp = trk->length;
	snprintf(buf, BUFSIZE, "%.2f m", tmp);
	GfuiLabelSetText(scrHandle, LengthId, buf);

	if (trk->pits.nMaxPits != 0) {
		snprintf(buf, BUFSIZE, "%d", trk->pits.nMaxPits);
		GfuiLabelSetText(scrHandle, PitsId, buf);
	} else {
		GfuiLabelSetText(scrHandle, PitsId, "none");
	}

	ts->trackItf.trkShutdown();
	GfParmReleaseHandle(trackHandle);
}


static void rmtsPrevNext(void *vsel)
{
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	
	if (vsel == 0) {
		CategoryList->userData = (void*)(((tFList*)CategoryList->userData)->prev);
	} else {
		CategoryList->userData = (void*)(((tFList*)CategoryList->userData)->next);
	}

	GfuiLabelSetText(scrHandle, TrackLabelId, ((tFList*)CategoryList->userData)->dispName);
	GfuiStaticImageSet(scrHandle, MapId, rmGetMapName(buf, BUFSIZE));
	rmUpdateTrackInfo();
}


static void rmCatPrevNext(void *vsel)
{
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	if (vsel == 0) {
		CategoryList = CategoryList->prev;
	} else {
		CategoryList = CategoryList->next;
	}

	GfuiLabelSetText(scrHandle, CatLabelId, CategoryList->dispName);
	GfuiLabelSetText(scrHandle, TrackLabelId, ((tFList*)CategoryList->userData)->dispName);
	GfuiStaticImageSet(scrHandle, MapId, rmGetMapName(buf, BUFSIZE));
	rmUpdateTrackInfo();
}


void rmtsSelect(void * /* dummy */)
{
	int curTrkIdx;
	const int BUFSIZE = 1024;
	char path[BUFSIZE];

	curTrkIdx = (int)GfParmGetNum(ts->param, RM_SECT_TRACKS, RE_ATTR_CUR_TRACK, nullptr, 1);
	snprintf(path, BUFSIZE, "%s/%d", RM_SECT_TRACKS, curTrkIdx);
	GfParmSetStr(ts->param, path, RM_ATTR_CATEGORY, CategoryList->name);
	GfParmSetStr(ts->param, path, RM_ATTR_NAME, ((tFList*)CategoryList->userData)->name);

	rmtsDeactivate(ts->nextScreen);
}


static void rmtsAddKeys(void)
{
	GfuiAddKey(scrHandle, 13, "Select track", nullptr, rmtsSelect, nullptr);
	GfuiAddKey(scrHandle, 27, "Cancel selection", ts->prevScreen, rmtsDeactivate, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_LEFT, "Previous track", (void*)0, rmtsPrevNext, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_RIGHT, "Next track", (void*)1, rmtsPrevNext, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_F12, "Take a screenshot", nullptr, GfuiScreenShot, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_UP, "Previous track category", (void*)0, rmCatPrevNext, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_DOWN, "Next track category", (void*)1, rmCatPrevNext, nullptr);
}


/** @brief Get the track name defined in the parameters
 *  @ingroup racemantools
 *  @param[in] category Track category directory
 *  @param[in] trackName Track file name
 *  @return Long track name on success
 *  <br>Empty string on failure
 *  @note The returned string is allocated on the heap and must be released by the caller at some point
 */
char* RmGetTrackName(char *category, char *trackName)
{
	void *trackHandle;
	char *name;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	snprintf(buf, BUFSIZE, "tracks/%s/%s/%s.%s", category, trackName, trackName, TRKEXT);
	trackHandle = GfParmReadFile(buf, GFPARM_RMODE_STD); /* don't release, the name is used later */

	if (trackHandle) {
		name = strdup(GfParmGetStr(trackHandle, TRK_SECT_HDR, TRK_ATT_NAME, trackName));
	} else {
		GfTrace("File %s has pb\n", buf);
		return strdup("");
	}

	GfParmReleaseHandle(trackHandle);
	return name;
}


/** @brief Get the track category name from the track category file
 *  @ingroup racemantools
 *  @param[in] category Track category file
 *  @return Category display name on success
 *  <br>Empty string on failure
 *  @note The returned string is allocated on the heap and must be released by the caller at some point   
 */
char* RmGetCategoryName(char *category)
{
	void *categoryHandle;
	char *name;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	snprintf(buf, BUFSIZE, "data/tracks/%s.%s", category, TRKEXT);
	categoryHandle = GfParmReadFile(buf, GFPARM_RMODE_STD); /* don't release, the name is used later */

	if (categoryHandle) {
		name = strdup(GfParmGetStr(categoryHandle, TRK_SECT_HDR, TRK_ATT_NAME, category));
	} else {
		GfTrace("File %s has pb\n", buf);
		return strdup("");
	}

	GfParmReleaseHandle(categoryHandle);
	return name;
}


/** @brief Track selection, the race manager parameter set is handed over in vs, tRmTrackSelect.param
 *  @ingroup racemantools
 *  @param[in,out] vs Pointer on a tRmTrackSelect structure (cast to void *)
 *  @note The race manager parameter set is modified in memory but not persisted.
 */
void RmTrackSelect(void *vs)
{
	const char *defaultTrack;
	const char *defaultCategory;
	tFList *CatCur;
	tFList *TrList, *TrCur;
	int Xpos, Ypos, DX, DY;
	int curTrkIdx;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	char path[BUFSIZE];

	ts = (tRmTrackSelect*)vs;

	/* Get the list of categories directories */
	CategoryList = GfDirGetList("tracks");
	if (CategoryList == nullptr) {
		GfTrace("RmTrackSelect: No track category available\n");
		return;
	}

	CatCur = CategoryList;
	do {
		CatCur->dispName = RmGetCategoryName(CatCur->name);
		if (strlen(CatCur->dispName) == 0) {
			GfTrace("RmTrackSelect: No definition for track category %s\n", CatCur->name);
			return;
		}

		/* get the tracks in the category directory */
		snprintf(buf, BUFSIZE, "tracks/%s", CatCur->name);
		TrList = GfDirGetList(buf);
		if (TrList == nullptr) {
			GfTrace("RmTrackSelect: No track for category %s available\n", CatCur->name);
			return;
		}
		TrList = TrList->next; /* get the first one */
		CatCur->userData = (void*)TrList;
		TrCur = TrList;
		do {
			TrCur->dispName = RmGetTrackName(CatCur->name, TrCur->name);
			if (strlen(TrCur->dispName) == 0) {
				GfTrace("RmTrackSelect: No definition for track %s\n", TrCur->name);
				return;
			}
			TrCur = TrCur->next;
		} while (TrCur != TrList);

		CatCur = CatCur->next;
	} while (CatCur != CategoryList);

	curTrkIdx = (int)GfParmGetNum(ts->param, RM_SECT_TRACKS, RE_ATTR_CUR_TRACK, nullptr, 1);
	snprintf(path, BUFSIZE, "%s/%d", RM_SECT_TRACKS, curTrkIdx);
	defaultCategory = GfParmGetStr(ts->param, path, RM_ATTR_CATEGORY, CategoryList->name);
	/* XXX coherency check */
	defaultTrack = GfParmGetStr(ts->param, path, RM_ATTR_NAME, ((tFList*)CategoryList->userData)->name);

	CatCur = CategoryList;
	do {
	if (strcmp(CatCur->name, defaultCategory) == 0) {
		CategoryList = CatCur;
		TrCur = (tFList*)(CatCur->userData);
		do {
		if (strcmp(TrCur->name, defaultTrack) == 0) {
			CatCur->userData = (void*)TrCur;
			break;
		}
		TrCur = TrCur->next;
		} while (TrCur != TrList);
		break;
	}
	CatCur = CatCur->next;
	} while (CatCur != CategoryList);

	scrHandle = GfuiScreenCreateEx(nullptr, nullptr, rmtsActivate, nullptr, nullptr, 1);
	GfuiScreenAddBgImg(scrHandle, "data/img/splash-qrtrk.png");

	rmtsAddKeys();

	GfuiLabelCreateEx(scrHandle,
			"CIRCUIT SELECT",
			kAccentColor,
			GFUI_FONT_SMALL_C,
			110,
			660,
			GFUI_ALIGN_HL_VB,
			0);
	GfuiTitleCreate(scrHandle, "Select Circuit", 0);
	GfuiLabelCreateEx(scrHandle,
			"Preview the venue, browse categories, and lock the track before moving to the roster.",
			kBodyColor,
			GFUI_FONT_MEDIUM_C,
			110,
			620,
			GFUI_ALIGN_HL_VB,
			0);
	GfuiLabelCreateEx(scrHandle,
			"Category and circuit selectors sit on the left; preview and venue data stay live on the right.",
			kMutedColor,
			GFUI_FONT_SMALL_C,
			110,
			592,
			GFUI_ALIGN_HL_VB,
			0);

	GfuiLabelCreateEx(scrHandle, "CATEGORY", kAccentColor, GFUI_FONT_SMALL_C, 110, 470, GFUI_ALIGN_HL_VB, 0);

	GfuiGrButtonCreate(scrHandle,
			"data/img/arrow-left.png",
			"data/img/arrow-left.png",
			"data/img/arrow-left.png",
			"data/img/arrow-left-pushed.png",
			70, 430, GFUI_ALIGN_HC_VB, 0,
			(void*)0, rmCatPrevNext,
			nullptr, nullptr, nullptr);


	CatLabelId = GfuiLabelCreate(scrHandle,
				CategoryList->dispName,
				GFUI_FONT_LARGE_C,
				270, 430, GFUI_ALIGN_HC_VB,
				30);

	GfuiGrButtonCreate(scrHandle,
			"data/img/arrow-right.png",
			"data/img/arrow-right.png",
			"data/img/arrow-right.png",
			"data/img/arrow-right-pushed.png",
			470, 430, GFUI_ALIGN_HC_VB, 0,
			(void*)1, rmCatPrevNext,
			nullptr, nullptr, nullptr);

	GfuiLabelCreateEx(scrHandle, "CIRCUIT", kAccentColor, GFUI_FONT_SMALL_C, 110, 412, GFUI_ALIGN_HL_VB, 0);

	GfuiGrButtonCreate(scrHandle,
			"data/img/arrow-left.png",
			"data/img/arrow-left.png",
			"data/img/arrow-left.png",
			"data/img/arrow-left-pushed.png",
			70, 370, GFUI_ALIGN_HC_VB, 0,
			(void*)0, rmtsPrevNext,
			nullptr, nullptr, nullptr);


	TrackLabelId = GfuiLabelCreate(scrHandle,
				((tFList*)CategoryList->userData)->dispName,
				GFUI_FONT_LARGE_C,
				270, 370, GFUI_ALIGN_HC_VB,
				30);

	GfuiGrButtonCreate(scrHandle,
			"data/img/arrow-right.png",
			"data/img/arrow-right.png",
			"data/img/arrow-right.png",
			"data/img/arrow-right-pushed.png",
			470, 370, GFUI_ALIGN_HC_VB, 0,
			(void*)1, rmtsPrevNext,
			nullptr, nullptr, nullptr);

	int scrw, scrh, vw, vh;
	GfScrGetSize(&scrw, &scrh, &vw, &vh);
	GfuiLabelCreateEx(scrHandle, "TRACK MAP", kAccentColor, GFUI_FONT_SMALL_C, 800, 470, GFUI_ALIGN_HL_VB, 0);
	MapId = GfuiStaticImageCreate(scrHandle,
				930, 120, (int) (vh * 420.0f / vw), 315,
				rmGetMapName(buf, BUFSIZE));

	GfuiButtonCreate(scrHandle, "Lock Circuit", GFUI_FONT_LARGE, 900, 60, 170, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
			nullptr, rmtsSelect, nullptr, nullptr, nullptr);

	GfuiButtonCreate(scrHandle, "Back", GFUI_FONT_LARGE, 1090, 60, 150, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
			ts->prevScreen, rmtsDeactivate, nullptr, nullptr, nullptr);

	GfuiLabelCreateEx(scrHandle, "CIRCUIT DATA", kAccentColor, GFUI_FONT_SMALL_C, 110, 320, GFUI_ALIGN_HL_VB, 0);

	Xpos = 110;
	Ypos = 286;
	DX = 150;
	DY = 34;

	GfuiLabelCreate(scrHandle,
			"Description:",
			GFUI_FONT_MEDIUM,
			Xpos, Ypos,
			GFUI_ALIGN_HL_VB, 0);

	DescId =  GfuiLabelCreate(scrHandle,
				"",
				GFUI_FONT_MEDIUM_C,
				Xpos + DX, Ypos,
				GFUI_ALIGN_HL_VB, 50);

	Ypos -= DY;

	GfuiLabelCreate(scrHandle,
			"Author:",
			GFUI_FONT_MEDIUM,
			Xpos, Ypos,
			GFUI_ALIGN_HL_VB, 0);

	AuthorId = GfuiLabelCreate(scrHandle,
				"",
				GFUI_FONT_MEDIUM_C,
				Xpos + DX, Ypos,
				GFUI_ALIGN_HL_VB, 20);

	Ypos -= DY;

	GfuiLabelCreate(scrHandle,
			"Length:",
			GFUI_FONT_MEDIUM,
			Xpos, Ypos,
			GFUI_ALIGN_HL_VB, 0);

	LengthId = GfuiLabelCreate(scrHandle,
				"",
				GFUI_FONT_MEDIUM_C,
				Xpos + DX, Ypos,
				GFUI_ALIGN_HL_VB, 20);

	Ypos -= DY;

	GfuiLabelCreate(scrHandle,
			"Width:",
			GFUI_FONT_MEDIUM,
			Xpos, Ypos,
			GFUI_ALIGN_HL_VB, 0);

	WidthId = GfuiLabelCreate(scrHandle,
				"",
				GFUI_FONT_MEDIUM_C,
				Xpos + DX, Ypos,
				GFUI_ALIGN_HL_VB, 20);

	Ypos -= DY;

	GfuiLabelCreate(scrHandle,
			"Pit Stalls:",
			GFUI_FONT_MEDIUM,
			Xpos, Ypos,
			GFUI_ALIGN_HL_VB, 0);

	PitsId = GfuiLabelCreate(scrHandle,
				"",
				GFUI_FONT_MEDIUM_C,
				Xpos + DX, Ypos,
				GFUI_ALIGN_HL_VB, 20);

	rmUpdateTrackInfo();

	GfuiScreenActivate(scrHandle);
}
