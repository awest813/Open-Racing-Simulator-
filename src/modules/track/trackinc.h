/***************************************************************************

    file                 : trackinc.h
    created              : Sun Jan 30 22:57:40 CET 2000
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
 
 
#ifndef _TRACKINC_H__
#define _TRACKINC_H__

#include <cstddef>
#include <cstdlib>
#include <cstring>

template <typename T>
T* trackCalloc(const char* context)
{
	T* pointer = static_cast<T*>(std::calloc(1, sizeof(T)));
	if (pointer == nullptr) {
		GfFatal("%s: Memory allocation failed\n", context);
	}

	return pointer;
}

template <typename T>
T* trackCallocArray(std::size_t count, const char* context)
{
	if (count == 0) {
		return nullptr;
	}

	T* pointer = static_cast<T*>(std::calloc(count, sizeof(T)));
	if (pointer == nullptr) {
		GfFatal("%s: Memory allocation failed\n", context);
	}

	return pointer;
}

inline char* trackDuplicateString(const char* source, const char* context)
{
	if (source == nullptr) {
		return nullptr;
	}

	const std::size_t length = std::strlen(source) + 1;
	char* copy = static_cast<char*>(std::malloc(length));
	if (copy == nullptr) {
		GfFatal("%s: Memory allocation failed\n", context);
	}

	std::memcpy(copy, source, length);
	return copy;
}

extern void TrackShutdown(void);
extern void ReadTrack3(tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);
extern void ReadTrack4(tTrack *theTrack, void *TrackHandle, tRoadCam **camList, int ext);

extern tTrack *TrackBuildv1(char *trackfile);
extern tTrack *TrackBuildEx(char *trackfile);
extern tdble TrackHeightG(tTrackSeg *seg, tdble x, tdble y);
extern tdble TrackHeightL(tTrkLocPos *p);
extern void TrackGlobal2Local(tTrackSeg *segment, tdble X, tdble Y, tTrkLocPos *p, int sides);
extern void TrackLocal2Global(tTrkLocPos *p, tdble *X, tdble *Y);
extern void TrackSideNormal(tTrackSeg*, tdble, tdble, int, t3Dd*);
extern void TrackSurfaceNormal(tTrkLocPos *p, t3Dd *norm);
extern tRoadCam *TrackGetCamList(void);
extern tdble TrackSpline(tdble p0, tdble p1, tdble t0, tdble t1, tdble t);


#endif /* _TRACKINC_H__ */ 



