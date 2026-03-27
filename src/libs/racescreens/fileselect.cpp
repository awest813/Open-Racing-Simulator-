/***************************************************************************

    file        : fileselect.cpp
    created     : Sun Feb 16 13:09:23 CET 2003
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
    Files selection screen.
    @author	Bernhard Wymann, Eric Espie
    @version	$Id$
*/


#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <tgfclient.h>
#include <racescreens.h>

static void *scrHandle = nullptr;
static int fileScrollList;
static tRmFileSelect *rmFs;
static tFList *FileList = nullptr;
static tFList *FileSelected;


static void rmActivate(void * /* dummy */ )
{
}


static void rmClickOnFile(void * /*dummy*/)
{
    GfuiScrollListGetSelectedElement(scrHandle, fileScrollList, (void**)&FileSelected);
}


static void rmSelect(void * /* dummy */ )
{
	if (FileList) {
		rmFs->select(FileSelected->name);
		GfDirFreeList(FileList, nullptr, true, false);
		FileList = nullptr;
	} else {
		rmFs->select(nullptr);
	}
}

static void rmDeactivate(void * /* dummy */ )
{
	if (FileList) {
		GfDirFreeList(FileList, nullptr, true, false);
		FileList = nullptr;
	}
	GfuiScreenActivate(rmFs->prevScreen);
}


/** @brief File selection
 * 
 *  The files listed are the ones contained in the directory given by the path in tRmFileSelect.path
 *  @ingroup racemantools
 *  @param[in,out] vs Pointer on tRmFileSelect structure (cast to void)
 */
void RmFileSelect(void *vs)
{
	tFList *FileCur;

	rmFs = (tRmFileSelect*)vs;

	if (scrHandle) {
		GfuiScreenRelease(scrHandle);
	}
	scrHandle = GfuiScreenCreateEx(nullptr, nullptr, rmActivate, nullptr, nullptr, 1);
	GfuiScreenAddBgImg(scrHandle, "data/img/splash-filesel.png");
	GfuiTitleCreate(scrHandle, rmFs->title, 0);

	/* Scroll List containing the File list */
	fileScrollList = GfuiScrollListCreate(scrHandle, GFUI_FONT_MEDIUM_C, 120, 80, GFUI_ALIGN_HC_VB,
						400, 310, GFUI_SB_RIGHT, nullptr, rmClickOnFile);

	FileList = GfDirGetList(rmFs->path);
	if (FileList == nullptr) {
		GfuiScreenActivate(rmFs->prevScreen);
		return;
	}
	
	FileSelected = FileList;
	FileCur = FileList;
	do {
		FileCur = FileCur->next;
		GfuiScrollListInsertElement(scrHandle, fileScrollList, FileCur->name, 1000, (void*)FileCur);
	} while (FileCur != FileList);

	/* Bottom buttons */
	GfuiButtonCreate(scrHandle, "Select", GFUI_FONT_LARGE, 210, 40, 150, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
				nullptr, rmSelect, nullptr, nullptr, nullptr);

	GfuiButtonCreate(scrHandle, "Cancel", GFUI_FONT_LARGE, 430, 40, 150, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
				nullptr, rmDeactivate, nullptr, nullptr, nullptr);

	/* Default menu keyboard actions */
	GfuiMenuDefaultKeysAdd(scrHandle);
	GfuiScreenActivate(scrHandle);
}
