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

#include <cctype>
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

inline int trackParseMarkIds(const char* marks, int* values, int maxCount)
{
	if ((marks == nullptr) || (values == nullptr) || (maxCount <= 0)) {
		return 0;
	}

	int count = 0;
	const char* cursor = marks;

	while ((*cursor != '\0') && (count < maxCount)) {
		while ((*cursor != '\0') && ((*cursor == ';') || std::isspace(static_cast<unsigned char>(*cursor)))) {
			++cursor;
		}
		if (*cursor == '\0') {
			break;
		}

		char* end = nullptr;
		values[count] = static_cast<int>(std::strtol(cursor, &end, 0));
		if (end != cursor) {
			++count;
			cursor = end;
		}

		while ((*cursor != '\0') && (*cursor != ';')) {
			++cursor;
		}
		if (*cursor == ';') {
			++cursor;
		}
	}

	return count;
}

inline tSegExt* trackCreateMarkExtension(const int* values, int count, const char* context)
{
	if ((values == nullptr) || (count <= 0)) {
		return nullptr;
	}

	tSegExt* extension = trackCalloc<tSegExt>(context);
	int* marks = trackCallocArray<int>(count, context);
	std::memcpy(marks, values, static_cast<std::size_t>(count) * sizeof(*marks));
	extension->nbMarks = count;
	extension->marks = marks;
	return extension;
}

inline tRoadCam* trackAppendCamera(tRoadCam** cameraList, const char* context)
{
	tRoadCam* camera = trackCalloc<tRoadCam>(context);
	if (*cameraList == nullptr) {
		*cameraList = camera;
		camera->next = camera;
	} else {
		camera->next = (*cameraList)->next;
		(*cameraList)->next = camera;
		*cameraList = camera;
	}

	return camera;
}

inline int trackParseBankingType(const char* value)
{
	return (std::strcmp(value, TRK_VAL_LEVEL) == 0) ? 0 : 1;
}

inline int trackParseBorderStyle(const char* style)
{
	if (std::strcmp(style, TRK_VAL_PLAN) == 0) {
		return TR_PLAN;
	}
	if (std::strcmp(style, TRK_VAL_CURB) == 0) {
		return TR_CURB;
	}

	return TR_WALL;
}

inline const char* trackBorderStyleValue(int style)
{
	switch (style) {
	case TR_PLAN:
		return TRK_VAL_PLAN;
	case TR_CURB:
		return TRK_VAL_CURB;
	default:
		return TRK_VAL_WALL;
	}
}

inline int trackParseBarrierStyle(const char* style)
{
	return (std::strcmp(style, TRK_VAL_FENCE) == 0) ? TR_FENCE : TR_WALL;
}

inline const char* trackBarrierStyleValue(int style)
{
	return (style == TR_FENCE) ? TRK_VAL_FENCE : TRK_VAL_WALL;
}

inline tTrackSurface* trackFindSurface(tTrack* track, const char* material)
{
	for (tTrackSurface* surface = track->surfaces; surface != nullptr; surface = surface->next) {
		if (std::strcmp(surface->material, material) == 0) {
			return surface;
		}
	}

	return nullptr;
}

inline tTrackSurface* trackAppendSurface(tTrack* track, const char* material, const char* context)
{
	tTrackSurface* surface = trackCalloc<tTrackSurface>(context);
	surface->material = material;
	surface->next = track->surfaces;
	track->surfaces = surface;
	return surface;
}

inline void trackLoadSurfaceProperties(
	tTrackSurface* surface,
	void* trackHandle,
	const char* path,
	tdble reboundDefault)
{
	surface->kFriction = GfParmGetNum(trackHandle, path, TRK_ATT_FRICTION, nullptr, 0.8f);
	surface->kRollRes = GfParmGetNum(trackHandle, path, TRK_ATT_ROLLRES, nullptr, 0.001f);
	surface->kRoughness = GfParmGetNum(trackHandle, path, TRK_ATT_ROUGHT, nullptr, 0.0f) / 2.0f;
	surface->kRoughWaveLen = 2.0 * PI / GfParmGetNum(trackHandle, path, TRK_ATT_ROUGHTWL, nullptr, 1.0f);
	surface->kDammage = GfParmGetNum(trackHandle, path, TRK_ATT_DAMMAGE, nullptr, 10.0f);
	surface->kRebound = GfParmGetNum(trackHandle, path, TRK_ATT_REBOUND, nullptr, reboundDefault);
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



