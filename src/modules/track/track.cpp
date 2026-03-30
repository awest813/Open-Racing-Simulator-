/***************************************************************************

    file                 : track.cpp
    created              : Sun Jan 30 22:54:56 CET 2000
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


#include <cstdlib>
#include <cmath>
#include <cstdio>

#include <tgf.h>
#include <track.h>
#include <portability.h>
#include "trackinc.h"

const tdble DEGPRAD = 180.0 / PI;   /* degrees per radian */

static tTrack	*theTrack = nullptr;
static tRoadCam *theCamList;
static void	*TrackHandle;

static void GetTrackHeader(void *TrackHandle);

namespace {

void freeTrackCameraRing(tRoadCam* cameraList)
{
	if (cameraList == nullptr) {
		return;
	}

	tRoadCam* currentCamera = cameraList;
	do {
		tRoadCam* nextCamera = currentCamera->next;
		free(currentCamera);
		currentCamera = nextCamera;
	} while (currentCamera != cameraList);
}

void freeTrackSurfaceList(tTrackSurface* surfaceList)
{
	while (surfaceList != nullptr) {
		tTrackSurface* nextSurface = surfaceList->next;
		free(surfaceList);
		surfaceList = nextSurface;
	}
}

} // namespace


/*
 * External function used to (re)build a track
 * from the track file
 */
tTrack *
TrackBuildv1(char *trackfile)
{
    TrackShutdown();

    theTrack = trackCalloc<tTrack>("TrackBuildv1");
    theCamList = nullptr;

    theTrack->params = TrackHandle = GfParmReadFile (trackfile, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT | GFPARM_RMODE_PRIVATE);
    
    theTrack->filename = trackDuplicateString(trackfile, "TrackBuildv1 filename");

    GetTrackHeader(TrackHandle);

    
    switch(theTrack->version) {
    case 0:
    case 1:
    case 2:
    case 3:
	ReadTrack3(theTrack, TrackHandle, &theCamList, 0);
	break;
    case 4:
	ReadTrack4(theTrack, TrackHandle, &theCamList, 0);
	break;
    }

    return theTrack;
}

tTrack *
TrackBuildEx(char *trackfile)
{
    void	*TrackHandle;

    theTrack = trackCalloc<tTrack>("TrackBuildEx");
    theCamList = nullptr;

    theTrack->params = TrackHandle = GfParmReadFile (trackfile, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT | GFPARM_RMODE_PRIVATE);
    
    theTrack->filename = trackDuplicateString(trackfile, "TrackBuildEx filename");

    GetTrackHeader(TrackHandle);

    switch(theTrack->version) {
    case 0:
    case 1:
    case 2:
    case 3:
	ReadTrack3(theTrack, TrackHandle, &theCamList, 1);
	break;
    case 4:
	ReadTrack4(theTrack, TrackHandle, &theCamList, 1);
	break;
    }
    
    return theTrack;
}


/*
 * Function
 *	GetTrackHeader
 *
 * Description
 *	Get the header of the track file
 *	in order to know the number of segments
 * Parameters
 *	
 *
 * Return
 *	
 *
 * Remarks
 *	
 */
static void 
GetTrackHeader(void *TrackHandle)
{
	tTrackGraphicInfo *graphic;
	const char **env;
	int i;
	const int BUFSIZE = 256;
	char buf[BUFSIZE];
	char *s;
	
	theTrack->name = GfParmGetStr(TrackHandle, TRK_SECT_HDR, TRK_ATT_NAME, "no name");
	theTrack->version = static_cast<int>(GfParmGetNum(TrackHandle, TRK_SECT_HDR, TRK_ATT_VERSION, nullptr, 0));
	theTrack->width = GfParmGetNum(TrackHandle, TRK_SECT_MAIN, TRK_ATT_WIDTH, nullptr, 15.0);
	theTrack->author = GfParmGetStr(TrackHandle, TRK_SECT_HDR, TRK_ATT_AUTHOR, "none");
	theTrack->category = GfParmGetStr(TrackHandle, TRK_SECT_HDR, TRK_ATT_CAT, "road");
	
	/* Graphic part */
	graphic = &theTrack->graphic;
	
	graphic->background = GfParmGetStr(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BKGRND,
						"background.png");
	graphic->bgtype = static_cast<int>(GfParmGetNum(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGTYPE, nullptr, 0.0));
/*     if (graphic->bgtype > 2) { */
/* 	graphic->background2 = GfParmGetStr(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BKGRND2, */
/* 					    "background.png"); */
/*     } */
	graphic->bgColor[0] = static_cast<float>(GfParmGetNum(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_R, nullptr, 0.0f));
	graphic->bgColor[1] = static_cast<float>(GfParmGetNum(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_G, nullptr, 0.0f));
	graphic->bgColor[2] = static_cast<float>(GfParmGetNum(TrackHandle, TRK_SECT_GRAPH, TRK_ATT_BGCLR_B, nullptr, 0.1f));
	
	/* env map images */
	snprintf(buf, BUFSIZE, "%s/%s", TRK_SECT_GRAPH, TRK_LST_ENV);
	graphic->envnb = GfParmGetEltNb(TrackHandle, buf);
	if (graphic->envnb < 1) {
		graphic->envnb = 1;
	}

	graphic->env = trackCallocArray<const char*>(graphic->envnb, "GetTrackHeader env");
	env = graphic->env;
	for (i = 1; i <= graphic->envnb; i++) {
		snprintf(buf, BUFSIZE, "%s/%s/%d", TRK_SECT_GRAPH, TRK_LST_ENV, i);
		*env = GfParmGetStr(TrackHandle, buf, TRK_ATT_ENVNAME, "env.png");
		env ++;
	}
	
	theTrack->nseg = 0;
	
	s = strrchr(theTrack->filename, '/');
	if (s == nullptr) {
		s = theTrack->filename;
	} else {
		s++;
	}
	
	theTrack->internalname = trackDuplicateString(s, "GetTrackHeader internalname");
	s = strrchr(theTrack->internalname, '.');
	if (s != nullptr) {
		*s = 0;
	}
	
	graphic->turnMarksInfo.height = GfParmGetNum(TrackHandle, TRK_SECT_TURNMARKS, TRK_ATT_HEIGHT, nullptr, 1);
	graphic->turnMarksInfo.width  = GfParmGetNum(TrackHandle, TRK_SECT_TURNMARKS, TRK_ATT_WIDTH,  nullptr, 1);
	graphic->turnMarksInfo.vSpace = GfParmGetNum(TrackHandle, TRK_SECT_TURNMARKS, TRK_ATT_VSPACE, nullptr, 0);
	graphic->turnMarksInfo.hSpace = GfParmGetNum(TrackHandle, TRK_SECT_TURNMARKS, TRK_ATT_HSPACE, nullptr, 0);	
}

static void
freeSeg(tTrackSeg *seg)
{
	if (seg->barrier[0]) {
		free(seg->barrier[0]);
	}
	if (seg->barrier[1]) {
		free(seg->barrier[1]);
	}
	if (seg->ext) {
		free(seg->ext->marks);
		free(seg->ext);
	}
	if (seg->lside) {
		freeSeg(seg->lside);
	}
	if (seg->rside) {
		freeSeg(seg->rside);
	}
	free(seg);
}

void
TrackShutdown(void)
{
	tTrackSeg *curSeg;
	tTrackSeg *nextSeg;

	if (!theTrack) {
		return;
	}

	nextSeg = theTrack->seg->next;
	do {
		curSeg = nextSeg;
		nextSeg = nextSeg->next;
		freeSeg(curSeg);
	} while (curSeg != theTrack->seg);

	freeTrackSurfaceList(theTrack->surfaces);
	freeTrackCameraRing(theCamList);
	theCamList = nullptr;

	if (theTrack->pits.driversPits) free(theTrack->pits.driversPits);
	free(theTrack->graphic.env);
	free(theTrack->internalname);
	free(theTrack->filename);
	free(theTrack);

	GfParmReleaseHandle(TrackHandle);
	theTrack = nullptr;
}
