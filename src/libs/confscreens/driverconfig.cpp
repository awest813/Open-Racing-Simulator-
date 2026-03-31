/***************************************************************************

    file                 : driverconfig.cpp
    created              : Wed Apr 26 20:05:12 CEST 2000
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

#include <cstdio>
#include <cstdlib>
#include <tgfclient.h>
#include <track.h>
#include <robot.h>
#include <playerpref.h>
#include <controlconfig.h>
#include <portability.h>

#include "driverconfig.h"

#define NO_DRV	"--- empty ---"
#define dllname   "human"

static const char *level_str[] = { ROB_VAL_ROOKIE, ROB_VAL_AMATEUR, ROB_VAL_SEMI_PRO, ROB_VAL_PRO };
static const int nbLevels = sizeof(level_str) / sizeof(level_str[0]);

static float LabelColor[] = {1.0, 0.0, 1.0, 1.0};

static int	scrollList;
static void	*scrHandle = nullptr;
static void	*prevHandle = nullptr;

static int NameEditId;
static int CarEditId;
static int CatEditId;
static int RaceNumEditId;
static int TransEditId;
static int PitsEditId;
static int SkillId;
static int AutoReverseId;

#define NB_DRV	10

typedef struct tInfo
{
    char	*name;
    char	*dispname;
} tInfo;

struct tCarInfo;
struct tCatInfo;

GF_TAILQ_HEAD(CarsInfoHead, struct tCarInfo);
GF_TAILQ_HEAD(CatsInfoHead, struct tCatInfo);

typedef struct tCatInfo
{
    struct tCatInfo	*next;
    struct tCatInfo	*prev;
    tInfo		info;
    tCarsInfoHead	CarsInfoList;
    GF_TAILQ_ENTRY(struct tCatInfo) link;
} tCatInfo;

typedef struct tCarInfo
{
    struct tCarInfo	*next;
    struct tCarInfo	*prev;
    tInfo		info;
    tCatInfo		*cat;
    GF_TAILQ_ENTRY(struct tCarInfo) link;
} tCarInfo;

typedef struct PlayerInfo
{
    tInfo	info;
    tCarInfo	*carinfo;
    int		racenumber;
    const char *transmission;
    int		nbpitstops;
    float	color[4];
    int		skilllevel;
    int		autoreverse;
} tPlayerInfo;

#define _Name		info.name
#define _DispName	info.dispname

static tPlayerInfo PlayersInfo[NB_DRV];

static tCatsInfoHead CatsInfoList;

static tPlayerInfo	*curPlayer;

static const char *Yn[] = {HM_VAL_YES, HM_VAL_NO};

namespace {

void buildDataPath(char* buffer, int bufferSize, const char* relativePath)
{
	if ((buffer == nullptr) || (bufferSize <= 0)) {
		return;
	}

	const char* dataDir = GetDataDir();
	if (dataDir == nullptr) {
		dataDir = "";
	}

	snprintf(buffer, bufferSize, "%s%s", dataDir, relativePath);
	buffer[bufferSize - 1] = '\0';
}

} // namespace

static void
refreshEditVal(void)
{
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	
	if ((curPlayer == nullptr) || (curPlayer->carinfo == nullptr) || (curPlayer->carinfo->cat == nullptr)) {
		GfuiEditboxSetString(scrHandle, NameEditId, "");
		GfuiEnable(scrHandle, NameEditId, GFUI_DISABLE);
		
		GfuiEditboxSetString(scrHandle, RaceNumEditId, "");
		GfuiEnable(scrHandle, RaceNumEditId, GFUI_DISABLE);
		
		GfuiLabelSetText(scrHandle, CarEditId, "");
		GfuiEnable(scrHandle, CarEditId, GFUI_DISABLE);
		
		GfuiLabelSetText(scrHandle, CatEditId, "");
		
		GfuiEditboxSetString(scrHandle, RaceNumEditId, "");
		GfuiEnable(scrHandle, RaceNumEditId, GFUI_DISABLE);
		
		GfuiLabelSetText(scrHandle, TransEditId, "");
		
		GfuiEditboxSetString(scrHandle, PitsEditId, "");
		GfuiEnable(scrHandle, PitsEditId, GFUI_DISABLE);
		
		GfuiLabelSetText(scrHandle, SkillId, "");
		
		GfuiLabelSetText(scrHandle, AutoReverseId, "");
	} else {
		GfuiEditboxSetString(scrHandle, NameEditId, curPlayer->_DispName);
		GfuiEnable(scrHandle, NameEditId, GFUI_ENABLE);
		
		snprintf(buf, BUFSIZE, "%d", curPlayer->racenumber);
		GfuiEditboxSetString(scrHandle, RaceNumEditId, buf);
		GfuiEnable(scrHandle, RaceNumEditId, GFUI_ENABLE);
		
		GfuiLabelSetText(scrHandle, CarEditId, curPlayer->carinfo->_DispName);
		GfuiEnable(scrHandle, CarEditId, GFUI_ENABLE);
		
		GfuiLabelSetText(scrHandle, CatEditId, curPlayer->carinfo->cat->_DispName);
		
		snprintf(buf, BUFSIZE, "%d", curPlayer->racenumber);
		GfuiEditboxSetString(scrHandle, RaceNumEditId, buf);
		GfuiEnable(scrHandle, RaceNumEditId, GFUI_ENABLE);
		
		GfuiLabelSetText(scrHandle, TransEditId, curPlayer->transmission);
		
		snprintf(buf, BUFSIZE, "%d", curPlayer->nbpitstops);
		GfuiEditboxSetString(scrHandle, PitsEditId, buf);
		GfuiEnable(scrHandle, PitsEditId, GFUI_ENABLE);
		
		GfuiLabelSetText(scrHandle, SkillId, level_str[curPlayer->skilllevel]);
		
		GfuiLabelSetText(scrHandle, AutoReverseId, Yn[curPlayer->autoreverse]);
	}
}

static void
onSelect(void * /* Dummy */)
{
    GfuiScrollListGetSelectedElement(scrHandle, scrollList, (void**)&curPlayer);
    refreshEditVal();
}

static void
GenCarsInfo(void)
{
	tCarInfo *curCar;
	tCatInfo *curCat;
	tCatInfo *tmpCat;
	tFList *files;
	tFList *curFile;
	void *carparam;
	void *hdle;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	/* Empty the lists */
	while ((curCat = GF_TAILQ_FIRST(&CatsInfoList)) != nullptr) {
		GF_TAILQ_REMOVE(&CatsInfoList, curCat, link);
		while ((curCar = GF_TAILQ_FIRST(&(curCat->CarsInfoList))) != nullptr) {
			GF_TAILQ_REMOVE(&(curCat->CarsInfoList), curCar, link);
			free(curCar->_Name);
			free(curCar->_DispName);
			free(curCar);
		}
		free(curCat->_Name);
		free(curCat->_DispName);
		free(curCat);
	}
	
	buildDataPath(buf, BUFSIZE, "data/cars/categories");
	files = GfDirGetList(buf);
	curFile = files;
	if (curFile != nullptr) {
		do {
			curFile = curFile->next;
			curCat = (tCatInfo*)calloc(1, sizeof(tCatInfo));
			GF_TAILQ_INIT(&(curCat->CarsInfoList));
			curCat->_Name = strdup(curFile->name);
			snprintf(buf, BUFSIZE, "%sdata/cars/categories/%s/%s.xml", GetDataDir(), curFile->name, curFile->name);
			hdle = GfParmReadFile(buf, GFPARM_RMODE_STD);
			if (!hdle) {
				free(curCat->_Name);
				free(curCat);
				continue;
			}
			curCat->_DispName = strdup(GfParmGetName(hdle));
			GfParmReleaseHandle(hdle);
			GF_TAILQ_INSERT_TAIL(&CatsInfoList, curCat, link);
		} while (curFile != files);
	}
	GfDirFreeList(files, nullptr, true, false);
	
	buildDataPath(buf, BUFSIZE, "data/cars/models");
	files = GfDirGetList(buf);
	curFile = files;
	if (curFile != nullptr) {
		do {
			curFile = curFile->next;
			curCar = (tCarInfo*)calloc(1, sizeof(tCarInfo));
			curCar->_Name = strdup(curFile->name);
			snprintf(buf, BUFSIZE, "%sdata/cars/models/%s/%s.xml", GetDataDir(), curFile->name, curFile->name);
			carparam = GfParmReadFile(buf, GFPARM_RMODE_STD);
			if (!carparam) {
				free(curCar->_Name);
				free(curCar);
				continue;
			}
		
			curCar->_DispName = strdup(GfParmGetName(carparam));
			/* search for the category */
			const char* str = GfParmGetStr(carparam, SECT_CAR, PRM_CATEGORY, "");
			curCat = GF_TAILQ_FIRST(&CatsInfoList);
			if (curCat != nullptr) {
				do {
					if (strcmp(curCat->_Name, str) == 0) {
					break;
					}
				} while ((curCat = GF_TAILQ_NEXT(curCat, link)) != nullptr);
			}
			curCar->cat = curCat;
			if (curCar->cat == nullptr) {
				GfParmReleaseHandle(carparam);
				free(curCar->_Name);
				free(curCar->_DispName);
				free(curCar);
				continue;
			}
			GF_TAILQ_INSERT_TAIL(&(curCat->CarsInfoList), curCar, link);
			GfParmReleaseHandle(carparam);
		} while (curFile != files);
	}
	GfDirFreeList(files, nullptr, true, false);
	
	/* Remove the empty categories */
	curCat = GF_TAILQ_FIRST(&CatsInfoList);
	while (curCat != nullptr) {
		curCar = GF_TAILQ_FIRST(&(curCat->CarsInfoList));
		tmpCat = curCat;
		curCat = GF_TAILQ_NEXT(curCat, link);
		if (curCar == nullptr) {
			GfOut("Removing empty category %s\n", tmpCat->_DispName);
			GF_TAILQ_REMOVE(&CatsInfoList, tmpCat, link);
			free(tmpCat->_Name);
			free(tmpCat->_DispName);
			free(tmpCat);
		}
	}
	
}

static void
UpdtScrollList(void)
{
	char	*str;
	int		i;
	void	*tmp;
	
	/* free the previous scrollist elements */
	while((str = GfuiScrollListExtractElement(scrHandle, scrollList, 0, (void**)&tmp)) != nullptr) {
	}

	for (i = 0; i < NB_DRV; i++) {
		GfuiScrollListInsertElement(scrHandle, scrollList, PlayersInfo[i]._DispName, i, (void*)&(PlayersInfo[i]));
	}
}

static void
DeletePlayer(void * /* dummy */)
{
	if (curPlayer) {
		curPlayer->_DispName = strdup(NO_DRV);
		refreshEditVal();
		UpdtScrollList();
	}
}

static void
ConfControls(void * /* dummy */ )
{
	int index;
	
	if (curPlayer) {
		index = curPlayer - PlayersInfo + 1;
		GfuiScreenActivate(TorcsControlMenuInit(scrHandle, index));
	}
}

static int
GenDrvList(void)
{
	void *drvinfo;
	const int SSTRINGSIZE = 256;
	char sstring[SSTRINGSIZE];
	int i;
	int j;
	const char *driver;
	tCarInfo *car;
	tCatInfo *cat;
	const char *str;
	int found;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	snprintf(buf, BUFSIZE, "%s%s", GetLocalDir(), HM_DRV_FILE);
	drvinfo = GfParmReadFile(buf, GFPARM_RMODE_REREAD);
	if (drvinfo == nullptr) {
		return -1;
	}

	tCatInfo* firstCategory = GF_TAILQ_FIRST(&CatsInfoList);
	tCarInfo* firstCar = (firstCategory != nullptr) ? GF_TAILQ_FIRST(&(firstCategory->CarsInfoList)) : nullptr;
	if (firstCar == nullptr) {
		GfParmReleaseHandle(drvinfo);
		GfError("Driver configuration could not find any available cars in the packaged data.\n");
		return -1;
	}

	for (i = 0; i < NB_DRV; i++) {
		snprintf(sstring, SSTRINGSIZE, "%s/%s/%d", ROB_SECT_ROBOTS, ROB_LIST_INDEX, i+1);
		driver = GfParmGetStr(drvinfo, sstring, ROB_ATTR_NAME, "");
		if (strlen(driver) == 0) {
			PlayersInfo[i]._DispName = strdup(NO_DRV);
			PlayersInfo[i]._Name = strdup(dllname);
			PlayersInfo[i].carinfo = firstCar;
			PlayersInfo[i].racenumber = 0;
			PlayersInfo[i].color[0] = 1.0;
			PlayersInfo[i].color[1] = 1.0;
			PlayersInfo[i].color[2] = 0.5;
			PlayersInfo[i].color[3] = 1.0;
		} else {
			PlayersInfo[i]._DispName = strdup(driver);
			PlayersInfo[i]._Name = strdup(dllname);
			PlayersInfo[i].skilllevel = 0;
			str = GfParmGetStr(drvinfo, sstring, ROB_ATTR_LEVEL, level_str[0]);
			for(j = 0; j < nbLevels; j++) {
				if (strcmp(level_str[j], str) == 0) {
					PlayersInfo[i].skilllevel = j;
					break;
				}
			}
			str = GfParmGetStr(drvinfo, sstring, ROB_ATTR_CAR, "");
			found = 0;
			cat = GF_TAILQ_FIRST(&CatsInfoList);
			PlayersInfo[i].carinfo = firstCar;
			while (cat != nullptr && !found) {
				car = GF_TAILQ_FIRST(&(cat->CarsInfoList));
				while (car != nullptr) {
					if (strcmp(car->_Name, str) == 0) {
						found = 1;
						PlayersInfo[i].carinfo = car;
						break;
					}
					car = GF_TAILQ_NEXT(car, link);
				}
				cat = GF_TAILQ_NEXT(cat, link);
			}
			PlayersInfo[i].racenumber  = (int)GfParmGetNum(drvinfo, sstring, ROB_ATTR_RACENUM, nullptr, 0);
			PlayersInfo[i].color[0]    = (float)GfParmGetNum(drvinfo, sstring, ROB_ATTR_RED, nullptr, 1.0);
			PlayersInfo[i].color[1]    = (float)GfParmGetNum(drvinfo, sstring, ROB_ATTR_GREEN, nullptr, 1.0);;
			PlayersInfo[i].color[2]    = (float)GfParmGetNum(drvinfo, sstring, ROB_ATTR_BLUE, nullptr, 0.5);;
			PlayersInfo[i].color[3]    = 1.0;
		}
    }
    UpdtScrollList();

    snprintf(buf, BUFSIZE, "%s%s", GetLocalDir(), HM_PREF_FILE);
    void* PrefHdle = GfParmReadFile(buf, GFPARM_RMODE_REREAD);
    if (PrefHdle == nullptr) {
		GfParmReleaseHandle(drvinfo);
		return -1;
    }

    for (i = 0; i < NB_DRV; i++) {
		snprintf(sstring, SSTRINGSIZE, "%s/%s/%d", HM_SECT_PREF, HM_LIST_DRV, i+1);
		str = GfParmGetStr(PrefHdle, sstring, HM_ATT_TRANS, HM_VAL_AUTO);
		if (strcmp(str, HM_VAL_AUTO) == 0) {
			PlayersInfo[i].transmission = HM_VAL_AUTO;
		} else {
			PlayersInfo[i].transmission = HM_VAL_MANUAL;
		}
		PlayersInfo[i].nbpitstops = (int)GfParmGetNum(PrefHdle, sstring, HM_ATT_NBPITS, nullptr, 0);
		if (!strcmp(GfParmGetStr(PrefHdle, sstring, HM_ATT_AUTOREVERSE, Yn[0]), Yn[0])) {
			PlayersInfo[i].autoreverse = 0;
		} else {
			PlayersInfo[i].autoreverse = 1;
		}
    }

	GfParmReleaseHandle(PrefHdle);
	GfParmReleaseHandle(drvinfo);

    return 0;
}

static void
SaveDrvList(void * /* dummy */)
{
	void	*drvinfo;
	int		i;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];

	snprintf(buf, BUFSIZE, "%s%s", GetLocalDir(), HM_DRV_FILE);
	drvinfo = GfParmReadFile(buf, GFPARM_RMODE_STD);
	if (drvinfo == nullptr) {
		return;
	}

	for (i = 0; i < NB_DRV; i++) {
		snprintf(buf, BUFSIZE, "%s/%s/%d", ROB_SECT_ROBOTS, ROB_LIST_INDEX, i+1);
		if (strcmp(PlayersInfo[i]._DispName, NO_DRV) == 0) {
			GfParmSetStr(drvinfo, buf, ROB_ATTR_NAME, "");
		} else {
			GfParmSetStr(drvinfo, buf, ROB_ATTR_NAME, PlayersInfo[i]._DispName);
			GfParmSetStr(drvinfo, buf, ROB_ATTR_CAR, PlayersInfo[i].carinfo->_Name);
			GfParmSetNum(drvinfo, buf, ROB_ATTR_RACENUM, nullptr, PlayersInfo[i].racenumber);
			GfParmSetNum(drvinfo, buf, ROB_ATTR_RED, nullptr, PlayersInfo[i].color[0]);
			GfParmSetNum(drvinfo, buf, ROB_ATTR_GREEN, nullptr, PlayersInfo[i].color[1]);
			GfParmSetNum(drvinfo, buf, ROB_ATTR_BLUE, nullptr, PlayersInfo[i].color[2]);
			GfParmSetStr(drvinfo, buf, ROB_ATTR_TYPE, ROB_VAL_HUMAN);
			GfParmSetStr(drvinfo, buf, ROB_ATTR_LEVEL, level_str[PlayersInfo[i].skilllevel]);
		}
	}
	GfParmWriteFile(nullptr, drvinfo, dllname);
	GfParmReleaseHandle(drvinfo);

	snprintf(buf, BUFSIZE, "%s%s", GetLocalDir(), HM_PREF_FILE);
	void* PrefHdle = GfParmReadFile(buf, GFPARM_RMODE_STD | GFPARM_RMODE_CREAT);
	for (i = 0; i < NB_DRV; i++) {
		snprintf(buf, BUFSIZE, "%s/%s/%d", HM_SECT_PREF, HM_LIST_DRV, i+1);
		GfParmSetStr(PrefHdle, buf, HM_ATT_TRANS, PlayersInfo[i].transmission);
		GfParmSetNum(PrefHdle, buf, HM_ATT_NBPITS, nullptr, (tdble)PlayersInfo[i].nbpitstops);
		GfParmSetStr(PrefHdle, buf, HM_ATT_AUTOREVERSE, Yn[PlayersInfo[i].autoreverse]);
	}

	GfParmWriteFile(nullptr, PrefHdle, "preferences");
	GfParmReleaseHandle(PrefHdle);
	GfuiScreenActivate(prevHandle);
	return;
}


static void
ChangeName(void * /* dummy */)
{
	char	*val;
	
	val = GfuiEditboxGetString(scrHandle, NameEditId);
	if (curPlayer != nullptr) {
		if (curPlayer->_DispName) {
			free(curPlayer->_DispName);
		}

		if (strlen(val)) {
			curPlayer->_DispName = strdup(val);
		} else {
			curPlayer->_DispName = strdup(NO_DRV);
		}
	}
	UpdtScrollList();
}

static void
ChangeNum(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	
	val = GfuiEditboxGetString(scrHandle, RaceNumEditId);
	if (curPlayer != nullptr) {
		curPlayer->racenumber = (int)strtol(val, nullptr, 0);
		snprintf(buf, BUFSIZE, "%d", curPlayer->racenumber);
		GfuiEditboxSetString(scrHandle, RaceNumEditId, buf);
	}
}

static void
ChangePits(void * /* dummy */)
{
	char *val;
	const int BUFSIZE = 1024;
	char buf[BUFSIZE];
	
	val = GfuiEditboxGetString(scrHandle, PitsEditId);
	if (curPlayer != nullptr) {    
		curPlayer->nbpitstops = (int)strtol(val, nullptr, 0);
		snprintf(buf, BUFSIZE, "%d", curPlayer->nbpitstops);
		GfuiEditboxSetString(scrHandle, PitsEditId, buf);
	}
}

static void
ChangeCar(void *vp)
{
	tCarInfo	*car;
	tCatInfo	*cat;
	
	if ((curPlayer == nullptr) || (curPlayer->carinfo == nullptr) || (curPlayer->carinfo->cat == nullptr)) {
		return;
	}
	
	cat = curPlayer->carinfo->cat;
	if (vp == 0) {
		car = GF_TAILQ_PREV(curPlayer->carinfo, CarsInfoHead, link);
		if (car == nullptr) {
			car = GF_TAILQ_LAST(&(cat->CarsInfoList), CarsInfoHead);
		}
	} else {
		car = GF_TAILQ_NEXT(curPlayer->carinfo, link);
		if (car == nullptr) {
			car = GF_TAILQ_FIRST(&(cat->CarsInfoList));
		}
	}
	curPlayer->carinfo = car;
	refreshEditVal();
}

static void
ChangeCat(void *vp)
{
	tCarInfo	*car;
	tCatInfo	*cat;
	
	if ((curPlayer == nullptr) || (curPlayer->carinfo == nullptr) || (curPlayer->carinfo->cat == nullptr)) {
		return;
	}
	
	cat = curPlayer->carinfo->cat;
	if (vp == 0) {
		do {
			cat = GF_TAILQ_PREV(cat, CatsInfoHead, link);
			if (cat == nullptr) {
			cat = GF_TAILQ_LAST(&CatsInfoList, CatsInfoHead);
			}
			car = GF_TAILQ_FIRST(&(cat->CarsInfoList));
		} while (car == nullptr);	/* skip empty categories */
	} else {
		do {
			cat = GF_TAILQ_NEXT(cat, link);
			if (cat == nullptr) {
			cat = GF_TAILQ_FIRST(&CatsInfoList);
			}
			car = GF_TAILQ_FIRST(&(cat->CarsInfoList));
		} while (car == nullptr);	/* skip empty categories */
	}

	curPlayer->carinfo = car;
	refreshEditVal();
}

static void
ChangeLevel(void *vp)
{
	if (curPlayer == nullptr) {
		return;
	}
	
	if (vp == 0) {
		curPlayer->skilllevel--;
		if (curPlayer->skilllevel < 0) {
			curPlayer->skilllevel = nbLevels - 1;
		}
	} else {
		curPlayer->skilllevel++;
		if (curPlayer->skilllevel == nbLevels) {
			curPlayer->skilllevel = 0;
		}
	}
	refreshEditVal();
}

static void
ChangeReverse(void *vdelta)
{
	long delta = (long)vdelta;
	
	if (curPlayer == nullptr) {
		return;
	}
	
	curPlayer->autoreverse += (int)delta;
	if (curPlayer->autoreverse < 0) {
		curPlayer->autoreverse = 1;
	} else if (curPlayer->autoreverse > 1) {
		curPlayer->autoreverse = 0;
	}
	
	refreshEditVal();
}

static void
ChangeTrans(void * /* dummy */)
{
	if (curPlayer == nullptr) {
		return;
	}
	
	if (strcmp(curPlayer->transmission,HM_VAL_AUTO) != 0) {
		curPlayer->transmission = HM_VAL_AUTO;
	} else {
		curPlayer->transmission = HM_VAL_MANUAL;
	}
	refreshEditVal();
}

void *
TorcsDriverMenuInit(void *prevMenu)
{
	int		x, y, x2, x3, x4, dy;
	static int	firstTime = 1;
	
	if (firstTime) {
		firstTime = 0;
		GF_TAILQ_INIT(&CatsInfoList);
	}
	
	/* screen already created */
	if (scrHandle) {
		GenCarsInfo();
		GenDrvList();
		return scrHandle;
	}
	prevHandle = prevMenu;
	
	scrHandle = GfuiScreenCreate();
	GfuiTitleCreate(scrHandle, "Player Configuration", 0);
	
	GfuiScreenAddBgImg(scrHandle, "data/img/splash-qrdrv.png");
	
	GfuiLabelCreate(scrHandle, "Players", GFUI_FONT_LARGE, 496, 400, GFUI_ALIGN_HC_VB, 0);
	
	scrollList = GfuiScrollListCreate(scrHandle, GFUI_FONT_MEDIUM_C,
						396, 390 - NB_DRV * GfuiFontHeight(GFUI_FONT_MEDIUM_C),
						GFUI_ALIGN_HL_VB, 200, NB_DRV * GfuiFontHeight(GFUI_FONT_MEDIUM_C), GFUI_SB_NONE,
						nullptr, onSelect);
	
	GfuiButtonCreate(scrHandle, "Delete", GFUI_FONT_LARGE, 496, 340 - NB_DRV * GfuiFontHeight(GFUI_FONT_MEDIUM_C),
				140, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
				nullptr, DeletePlayer, nullptr, nullptr, nullptr);
	
	GfuiButtonCreate(scrHandle, "Controls", GFUI_FONT_LARGE, 496, 340 - NB_DRV * GfuiFontHeight(GFUI_FONT_MEDIUM_C) - 30,
				140, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
				nullptr, ConfControls, nullptr, nullptr, nullptr);
	
	GenCarsInfo();
	if (GenDrvList()) {
		GfuiScreenRelease(scrHandle);
		return nullptr;
	}
	
	x = 20;
	x2 = 170;
	x3 = x2 + 100;
	x4 = x2 + 200;
	y = 370;
	dy = 30;
	
	GfuiLabelCreate(scrHandle, "Name:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	NameEditId = GfuiEditboxCreate(scrHandle, "", GFUI_FONT_MEDIUM_C,
					x2+10, y, 180, 16, nullptr, nullptr, ChangeName);
	
	y -= dy;
	GfuiLabelCreate(scrHandle, "Category:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-left.png", "data/img/arrow-left.png",
				"data/img/arrow-left.png", "data/img/arrow-left-pushed.png",
				x2, y, GFUI_ALIGN_HL_VB, 1,
				(void*)0, ChangeCat,
				nullptr, nullptr, nullptr);	    
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-right.png", "data/img/arrow-right.png",
				"data/img/arrow-right.png", "data/img/arrow-right-pushed.png",
				x4, y, GFUI_ALIGN_HR_VB, 1,
				(void*)1, ChangeCat,
				nullptr, nullptr, nullptr);
	CatEditId = GfuiLabelCreate(scrHandle, "", GFUI_FONT_MEDIUM_C, x3, y, GFUI_ALIGN_HC_VB, 32);
	GfuiLabelSetColor(scrHandle, CatEditId, LabelColor);
	
	y -= dy;
	GfuiLabelCreate(scrHandle, "Car:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-left.png", "data/img/arrow-left.png",
				"data/img/arrow-left.png", "data/img/arrow-left-pushed.png",
				x2, y, GFUI_ALIGN_HL_VB, 1,
				(void*)0, ChangeCar,
				nullptr, nullptr, nullptr);	    
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-right.png", "data/img/arrow-right.png",
				"data/img/arrow-right.png", "data/img/arrow-right-pushed.png",
				x4, y, GFUI_ALIGN_HR_VB, 1,
				(void*)1, ChangeCar,
				nullptr, nullptr, nullptr);
	CarEditId = GfuiLabelCreate(scrHandle, "", GFUI_FONT_MEDIUM_C, x3, y, GFUI_ALIGN_HC_VB, 32);
	GfuiLabelSetColor(scrHandle, CarEditId, LabelColor);
	
	y -= dy;
	GfuiLabelCreate(scrHandle, "Race Number:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	RaceNumEditId = GfuiEditboxCreate(scrHandle, "0", GFUI_FONT_MEDIUM_C,
						x2+10, y, 0, 2, nullptr, nullptr, ChangeNum);
	y -= dy;
	GfuiLabelCreate(scrHandle, "Transmission:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-left.png", "data/img/arrow-left.png",
				"data/img/arrow-left.png", "data/img/arrow-left-pushed.png",
				x2, y, GFUI_ALIGN_HL_VB, 1,
				(void*)0, ChangeTrans,
				nullptr, nullptr, nullptr);	    
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-right.png", "data/img/arrow-right.png",
				"data/img/arrow-right.png", "data/img/arrow-right-pushed.png",
				x4, y, GFUI_ALIGN_HR_VB, 1,
				(void*)1, ChangeTrans,
				nullptr, nullptr, nullptr);
	TransEditId = GfuiLabelCreate(scrHandle, "", GFUI_FONT_MEDIUM_C, x3, y, GFUI_ALIGN_HC_VB, 32);
	GfuiLabelSetColor(scrHandle, TransEditId, LabelColor);
	
	y -= dy;
	GfuiLabelCreate(scrHandle, "Pit Stops:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	PitsEditId = GfuiEditboxCreate(scrHandle, "", GFUI_FONT_MEDIUM_C,
					x2+10, y, 0, 2, nullptr, nullptr, ChangePits);
	y -= dy;
	GfuiLabelCreate(scrHandle, "Level:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-left.png", "data/img/arrow-left.png",
				"data/img/arrow-left.png", "data/img/arrow-left-pushed.png",
				x2, y, GFUI_ALIGN_HL_VB, 1,
				(void*)0, ChangeLevel,
				nullptr, nullptr, nullptr);	    
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-right.png", "data/img/arrow-right.png",
				"data/img/arrow-right.png", "data/img/arrow-right-pushed.png",
				x4, y, GFUI_ALIGN_HR_VB, 1,
				(void*)1, ChangeLevel,
				nullptr, nullptr, nullptr);
	SkillId = GfuiLabelCreate(scrHandle, "", GFUI_FONT_MEDIUM_C, x3, y, GFUI_ALIGN_HC_VB, 32);
	GfuiLabelSetColor(scrHandle, SkillId, LabelColor);
	
	y -= dy;
	GfuiLabelCreate(scrHandle, "Auto Reverse:", GFUI_FONT_MEDIUM, x, y, GFUI_ALIGN_HL_VB, 0);
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-left.png", "data/img/arrow-left.png",
				"data/img/arrow-left.png", "data/img/arrow-left-pushed.png",
				x2, y, GFUI_ALIGN_HL_VB, 1,
				(void*)-1, ChangeReverse,
				nullptr, nullptr, nullptr);	    
	GfuiGrButtonCreate(scrHandle, "data/img/arrow-right.png", "data/img/arrow-right.png",
				"data/img/arrow-right.png", "data/img/arrow-right-pushed.png",
				x4, y, GFUI_ALIGN_HR_VB, 1,
				(void*)1, ChangeReverse,
				nullptr, nullptr, nullptr);
	AutoReverseId = GfuiLabelCreate(scrHandle, "", GFUI_FONT_MEDIUM_C, x3, y, GFUI_ALIGN_HC_VB, 32);
	GfuiLabelSetColor(scrHandle, AutoReverseId, LabelColor);
	
	GfuiButtonCreate(scrHandle, "Accept", GFUI_FONT_LARGE, 210, 40, 150, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
		nullptr, SaveDrvList, nullptr, nullptr, nullptr);
	
	GfuiButtonCreate(scrHandle, "Cancel", GFUI_FONT_LARGE, 430, 40, 150, GFUI_ALIGN_HC_VB, GFUI_MOUSE_UP,
		prevMenu, GfuiScreenActivate, nullptr, nullptr, nullptr);
	
	GfuiAddKey(scrHandle, 13, "Save Drivers", nullptr, SaveDrvList, nullptr);
	GfuiAddKey(scrHandle, 27, "Cancel Selection", prevMenu, GfuiScreenActivate, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_F12, "Screen-Shot", nullptr, GfuiScreenShot, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_LEFT, "Previous Car", (void*)0, ChangeCar, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_RIGHT, "Next Car", (void*)1, ChangeCar, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_UP, "Previous Car Category", (void*)0, ChangeCat, nullptr);
	GfuiAddSKey(scrHandle, GLUT_KEY_DOWN, "Next Car Category", (void*)1, ChangeCat, nullptr);
	
	refreshEditVal();
	return scrHandle;
}

