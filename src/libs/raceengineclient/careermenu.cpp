/***************************************************************************

    file        : careermenu.cpp
    created     : 2024
    copyright   : (C) 2024 Open Racing Simulator Contributors
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
    Text/menu based career mode screens.
    Provides a career mode with persistent player profiles, season standings,
    and links to the existing race engine.
*/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <tgfclient.h>
#include <raceman.h>
#include <portability.h>

#include "raceengine.h"
#include "raceinit.h"
#include "racestate.h"

#include "careermenu.h"

/* ==================== Constants ==================== */

/* Career save file path components (relative to GetLocalDir()) */
static const char *CAREER_SUBDIR      = "career";
static const char *CAREER_SAVE_FNAME  = "career.xml";

/* Career race manager config path (relative to GetLocalDir()) */
static const char *CAREER_RACEMAN_PATH = "config/raceman/career.xml";

/* Career XML sections / attributes */
#define CS_SECT_PLAYER   "Player"
#define CS_ATTR_NAME     "name"
#define CS_ATTR_SEASON   "season"
#define CS_ATTR_WINS     "total wins"
#define CS_ATTR_CHAMPS   "championships"
#define CS_ATTR_LAST_TRK "last track"    /* last CUR_TRACK seen from results  */
#define CS_ATTR_SEA_DONE "season done"   /* "yes" when all 8 rounds complete  */

/* Race name in career.xml - must match the <Races><1><name> attribute */
static const char *CAREER_RACE_NAME = "Career Race";
static const int NUM_SEASON_TRACKS = 8;

/* Human-readable track names matching the career.xml order */
static const char *seasonTrackNames[NUM_SEASON_TRACKS] = {
    "Forza", "Corkscrew", "Brondehach", "E-Track 3",
    "Wheel 1", "G-Track 1", "Street 1", "Aalborg"
};

/* Internal track directory names matching the career.xml order (used to read
   per-round race results from the results XML). */
static const char *seasonTrackKeys[NUM_SEASON_TRACKS] = {
    "forza", "corkscrew", "brondehach", "e-track-3",
    "wheel-1", "g-track-1", "street-1", "aalborg"
};

/* ==================== Screen Handles ==================== */

static void *careerSelHandle  = nullptr; /* career entry / selection screen */
static void *careerNewHandle  = nullptr; /* new career (name entry) screen   */
static void *careerHubHandle  = nullptr; /* career hub (main overview) screen */

/* ==================== Widget IDs ==================== */

/* New-career screen */
static int newNameEditId = -1;

/* Career hub screen - dynamic labels refreshed on every activation */
static int hubTitleId      = -1;
static int hubSeasonId     = -1;
static int hubRaceId       = -1;
static int hubWinsId       = -1;
static int hubChampsId     = -1;
static int hubSeaDoneId    = -1; /* "Season complete!" banner */
static int hubRaceNextBtnId  = -1; /* "Race Next Event" button */
static int hubNewSeasonBtnId = -1; /* "Start New Season" button */

/* ==================== Career Save Helpers ==================== */

static void buildSavePath(char *buf, int size)
{
    snprintf(buf, size, "%s%s/%s", GetLocalDir(), CAREER_SUBDIR, CAREER_SAVE_FNAME);
}

/** Open the career save file; optionally create it if it does not exist. */
static void *openCareerSave(bool create)
{
    char path[1024];
    buildSavePath(path, sizeof(path));
    int mode = GFPARM_RMODE_STD | GFPARM_RMODE_PRIVATE;
    if (create) {
        mode |= GFPARM_RMODE_CREAT;
    }
    return GfParmReadFile(path, mode);
}

/** Returns true when a career save file already exists. */
static bool careerSaveExists()
{
    void *h = openCareerSave(false);
    if (h) {
        GfParmReleaseHandle(h);
        return true;
    }
    return false;
}

/** Flush the in-memory save handle to disk. */
static void writeCareerSave(void *handle)
{
    char path[1024];
    buildSavePath(path, sizeof(path));
    GfParmWriteFile(path, handle, "Career Save");
}

/* ==================== Results Reading ==================== */

/**
 * Search the career results directory and read standings from the most
 * recent results XML file (filenames are timestamps so alphabetical order
 * equals chronological order).
 *
 * @param namesOut  Array of strings to receive "<name>  <pts> pts" entries.
 * @param numOut    Set to the number of valid entries placed in namesOut.
 * @param maxLines  Maximum entries to fill.
 * @return The current track index (1-based) from the results file,
 *         or 0 if no results are available.
 */
static int readLatestCareerResults(char namesOut[][72], int *numOut, int maxLines)
{
    *numOut = 0;

    char resultsDir[1024];
    snprintf(resultsDir, sizeof(resultsDir), "%sresults/career", GetLocalDir());

    tFList *files = GfDirGetListFiltered(resultsDir, "xml");
    if (!files) {
        GfTrace("Career: no results files in %s\n", resultsDir);
        return 0;
    }

    /* Find the alphabetically latest entry (= newest timestamp filename) */
    tFList *latest = files;
    tFList *cur = files->next;
    while (cur != files) {
        if (strcmp(cur->name, latest->name) > 0) {
            latest = cur;
        }
        cur = cur->next;
    }

    char resultPath[1024];
    snprintf(resultPath, sizeof(resultPath), "%sresults/career/%s",
             GetLocalDir(), latest->name);
    GfDirFreeList(files, nullptr, true, false);

    void *results = GfParmReadFile(resultPath, GFPARM_RMODE_STD | GFPARM_RMODE_PRIVATE);
    if (!results) {
        return 0;
    }

    /* Current track index tells us how far through the season we are */
    int curTrack = static_cast<int>(GfParmGetNum(results, RE_SECT_CURRENT, RE_ATTR_CUR_TRACK, nullptr, 1));

    /* Driver standings */
    int n = GfParmGetEltNb(results, RE_SECT_STANDINGS);
    if (n > maxLines) {
        n = maxLines;
    }
    *numOut = n;

    for (int i = 0; i < n; i++) {
        char secPath[256];
        snprintf(secPath, sizeof(secPath), "%s/%d", RE_SECT_STANDINGS, i + 1);
        const char *name = GfParmGetStr(results, secPath, RE_ATTR_NAME, "Unknown");
        int pts = static_cast<int>(GfParmGetNum(results, secPath, RE_ATTR_POINTS, nullptr, 0));
        snprintf(namesOut[i], 72, "%-24s %3d pts", name, pts);
    }

    GfParmReleaseHandle(results);
    return curTrack;
}

/**
 * Open the latest career results file and return its handle.
 * The caller must call GfParmReleaseHandle() on the returned handle.
 * Returns nullptr if no results file exists.
 */
static void *openLatestCareerResults()
{
    char resultsDir[1024];
    snprintf(resultsDir, sizeof(resultsDir), "%sresults/career", GetLocalDir());

    tFList *files = GfDirGetListFiltered(resultsDir, "xml");
    if (!files) {
        return nullptr;
    }

    tFList *latest = files;
    tFList *cur = files->next;
    while (cur != files) {
        if (strcmp(cur->name, latest->name) > 0) {
            latest = cur;
        }
        cur = cur->next;
    }

    char resultPath[1024];
    snprintf(resultPath, sizeof(resultPath), "%sresults/career/%s",
             GetLocalDir(), latest->name);
    GfDirFreeList(files, nullptr, true, false);

    return GfParmReadFile(resultPath, GFPARM_RMODE_STD | GFPARM_RMODE_PRIVATE);
}

/**
 * Count the number of race wins (P1 finishes) for the human driver in a
 * completed season.  Iterates over each of the NUM_SEASON_TRACKS tracks and
 * checks whether the rank-1 driver name matches the human driver's name as
 * stored in the standings.
 *
 * @param results  Handle to the latest career results XML file.
 * @return Number of rounds won by the human driver.
 */
static int careerCountSeasonWins(void *results)
{
    if (!results) {
        return 0;
    }

    /* Find the human driver's name from the standings */
    const char *humanName = nullptr;
    int n = GfParmGetEltNb(results, RE_SECT_STANDINGS);
    for (int i = 1; i <= n; i++) {
        char secPath[256];
        snprintf(secPath, sizeof(secPath), "%s/%d", RE_SECT_STANDINGS, i);
        const char *mod = GfParmGetStr(results, secPath, RE_ATTR_MODULE, "");
        if (strcmp(mod, "human") == 0) {
            humanName = GfParmGetStr(results, secPath, RE_ATTR_NAME, nullptr);
            break;
        }
    }
    if (!humanName) {
        return 0;
    }

    /* Count rounds where human was rank 1 */
    int wins = 0;
    for (int t = 0; t < NUM_SEASON_TRACKS; t++) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s/%s/%s/1",
                 seasonTrackKeys[t], RE_SECT_RESULTS, CAREER_RACE_NAME, RE_SECT_RANK);
        const char *winner = GfParmGetStr(results, path, RE_ATTR_NAME, nullptr);
        if (winner && strcmp(winner, humanName) == 0) {
            wins++;
        }
    }
    return wins;
}

/**
 * Return true when the human driver is the season champion (rank 1 in the
 * final standings with module == "human").
 *
 * @param results  Handle to the latest career results XML file.
 */
static bool careerIsChampion(void *results)
{
    if (!results) {
        return false;
    }
    if (GfParmGetEltNb(results, RE_SECT_STANDINGS) < 1) {
        return false;
    }
    char secPath[256];
    snprintf(secPath, sizeof(secPath), "%s/1", RE_SECT_STANDINGS);
    const char *mod = GfParmGetStr(results, secPath, RE_ATTR_MODULE, "");
    return (strcmp(mod, "human") == 0);
}

/* ==================== Season Standings Screen ==================== */

/** Build and activate the season standings screen. */
static void showStandingsScreen(void *prevMenu)
{
    /* Always re-create standings to load fresh data */
    static void *standsHandle = nullptr;
    if (standsHandle) {
        GfuiScreenRelease(standsHandle);
        standsHandle = nullptr;
    }

    standsHandle = GfuiScreenCreateEx(nullptr, nullptr, nullptr, nullptr, nullptr, 1);
    GfuiScreenAddBgImg(standsHandle, "data/img/splash-main.png");

    /* Season number */
    void *saveH = openCareerSave(false);
    int season = 1;
    if (saveH) {
        season = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON, nullptr, 1));
        GfParmReleaseHandle(saveH);
    }

    char title[64];
    snprintf(title, sizeof(title), "SEASON %d STANDINGS", season);
    GfuiTitleCreate(standsHandle, title, 0);

    const int MAX_DRIVERS = 20;
    char driverLines[MAX_DRIVERS][72];
    int numDrivers = 0;
    int curTrack = readLatestCareerResults(driverLines, &numDrivers, MAX_DRIVERS);

    if (curTrack > 0) {
        char subTitle[128];
        if (curTrack == 1 && numDrivers > 0) {
            /* curTrack wrapped back to 1 — all rounds are complete */
            snprintf(subTitle, sizeof(subTitle), "Final Standings - Season %d", season);
        } else {
            /* CUR_TRACK points to the next race to run, so completed = curTrack - 1 */
            int racesCompleted = curTrack - 1;
            snprintf(subTitle, sizeof(subTitle),
                     "After round %d of %d", racesCompleted, NUM_SEASON_TRACKS);
        }
        GfuiLabelCreate(standsHandle, subTitle, GFUI_FONT_MEDIUM_C,
                        320, 418, GFUI_ALIGN_HC_VB, 0);

        /* Column header */
        float headerColor[4] = {1.0f, 0.85f, 0.0f, 1.0f};
        GfuiLabelCreateEx(standsHandle,
                          "Pos  Driver                   Points",
                          headerColor, GFUI_FONT_SMALL_C,
                          320, 395, GFUI_ALIGN_HC_VB, 0);

        /* Driver rows */
        float rowColors[2][4] = {
            {1.0f, 1.0f, 1.0f, 1.0f},
            {0.7f, 0.85f, 1.0f, 1.0f}
        };
        int y = 375;
        for (int i = 0; i < numDrivers && y > 60; i++, y -= 18) {
            char row[80];
            snprintf(row, sizeof(row), " %2d.  %s", i + 1, driverLines[i]);
            GfuiLabelCreateEx(standsHandle, row, rowColors[i % 2],
                              GFUI_FONT_SMALL_C, 320, y, GFUI_ALIGN_HC_VB, 0);
        }
    } else {
        GfuiLabelCreate(standsHandle,
                        "No race results found.",
                        GFUI_FONT_MEDIUM_C, 320, 360, GFUI_ALIGN_HC_VB, 0);
        GfuiLabelCreate(standsHandle,
                        "Complete a career event to see standings here.",
                        GFUI_FONT_SMALL_C, 320, 330, GFUI_ALIGN_HC_VB, 0);
    }

    GfuiMenuDefaultKeysAdd(standsHandle);
    GfuiMenuBackQuitButtonCreate(standsHandle,
                                  "Back", "Return to Career Hub",
                                  prevMenu, GfuiScreenActivate);
    GfuiScreenActivate(standsHandle);
}

/* ==================== Career Hub Screen ==================== */

/** Called every time the career hub screen is activated (including on return
 *  from the race engine).  Re-initialises the race engine and refreshes all
 *  dynamic labels from the career save and the latest results file.
 *  Also detects end-of-season and updates wins/championships in the save. */
static void careerHubActivate(void * /* dummy */)
{
    /* Re-init race engine (same pattern as singleplayer.cpp).
       This cleans up any previous race state and sets _reMenuScreen so that
       "Back to Main" on the race manager menu returns here. */
    ReInit();
    ReInfo->_reMenuScreen = careerHubHandle;

    /* Load career save for player statistics */
    void *saveH = openCareerSave(false);
    if (!saveH) {
        return;
    }

    const char *nameStr = GfParmGetStr(saveH, CS_SECT_PLAYER, CS_ATTR_NAME,  "Player");
    char        playerName[64];
    snprintf(playerName, sizeof(playerName), "%s", nameStr ? nameStr : "Player");
    int season        = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON,   nullptr, 1));
    int wins          = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_WINS,     nullptr, 0));
    int champs        = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_CHAMPS,   nullptr, 0));
    int lastTrack     = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK, nullptr, 0));
    const char *seaDone = GfParmGetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_DONE, "no");
    bool seasonDone   = (strcmp(seaDone, "yes") == 0);

    /* Determine current race position from the latest results file. */
    char dummy[20][72];
    int  nDummy   = 0;
    int  curTrack = readLatestCareerResults(dummy, &nDummy, 20);

    /* ---- Season completion detection ----
       The race engine resets CUR_TRACK to 1 after the final round.
       Condition: we last saw track NUM_SEASON_TRACKS and now it wrapped to 1
       (and there are standings, meaning races were actually played). */
    if (!seasonDone && lastTrack == NUM_SEASON_TRACKS && curTrack == 1 && nDummy > 0) {
        seasonDone = true;

        /* Count race wins and award championship from completed season results */
        void *results = openLatestCareerResults();
        int seasonWins = careerCountSeasonWins(results);
        bool champion  = careerIsChampion(results);
        if (results) {
            GfParmReleaseHandle(results);
        }

        wins  += seasonWins;
        if (champion) {
            champs++;
        }

        /* Persist updated statistics in the career save */
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_WINS,     nullptr, static_cast<tdble>(wins));
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_CHAMPS,   nullptr, static_cast<tdble>(champs));
        GfParmSetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_DONE, "yes");
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK, nullptr, 0);
        writeCareerSave(saveH);
    } else if (!seasonDone && curTrack > lastTrack) {
        /* Advance lastTrack so we can detect the season end next time */
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK, nullptr, static_cast<tdble>(curTrack));
        writeCareerSave(saveH);
        lastTrack = curTrack;
    }

    GfParmReleaseHandle(saveH);

    char buf[128];

    snprintf(buf, sizeof(buf), "CAREER  -  %s", playerName);
    GfuiLabelSetText(careerHubHandle, hubTitleId, buf);

    snprintf(buf, sizeof(buf), "Season %d", season);
    GfuiLabelSetText(careerHubHandle, hubSeasonId, buf);

    if (seasonDone) {
        /* Season is over: show completion banner and swap buttons */
        GfuiLabelSetText(careerHubHandle, hubSeaDoneId,
                         "Season complete!  Start a new season to continue.");
        GfuiLabelSetText(careerHubHandle, hubRaceId, "All rounds finished.");
        GfuiVisibilitySet(careerHubHandle, hubRaceNextBtnId,  GFUI_INVISIBLE);
        GfuiVisibilitySet(careerHubHandle, hubNewSeasonBtnId, GFUI_VISIBLE);
    } else {
        GfuiLabelSetText(careerHubHandle, hubSeaDoneId, "");
        GfuiVisibilitySet(careerHubHandle, hubRaceNextBtnId,  GFUI_VISIBLE);
        GfuiVisibilitySet(careerHubHandle, hubNewSeasonBtnId, GFUI_INVISIBLE);

        if (curTrack >= 1 && curTrack <= NUM_SEASON_TRACKS) {
            snprintf(buf, sizeof(buf), "Next event: %s  (Round %d / %d)",
                     seasonTrackNames[curTrack - 1], curTrack, NUM_SEASON_TRACKS);
        } else {
            snprintf(buf, sizeof(buf), "Round 1 / %d: %s",
                     NUM_SEASON_TRACKS, seasonTrackNames[0]);
        }
        GfuiLabelSetText(careerHubHandle, hubRaceId, buf);
    }

    snprintf(buf, sizeof(buf), "Career wins: %d", wins);
    GfuiLabelSetText(careerHubHandle, hubWinsId, buf);

    snprintf(buf, sizeof(buf), "Championships: %d", champs);
    GfuiLabelSetText(careerHubHandle, hubChampsId, buf);
}

/** Deactivate the career hub: return to the previous screen and shut down
 *  the race engine cleanly. */
static void careerHubShutdown(void *prevMenu)
{
    GfuiScreenActivate(prevMenu);
    ReShutdown();
}

/** Launch the career race series using career.xml. */
static void careerRaceNextEvent(void * /* dummy */)
{
    char path[1024];
    snprintf(path, sizeof(path), "%s%s", GetLocalDir(), CAREER_RACEMAN_PATH);
    void *params = GfParmReadFile(path, GFPARM_RMODE_STD);
    if (!params) {
        GfTrace("Career: cannot open %s\n", path);
        return;
    }
    ReLaunchRaceman(params);
}

/** Advance to the next career season.
 *  Increments the season counter, clears the season-done flag, resets
 *  the last-track cursor, then re-launches career.xml to start fresh. */
static void careerStartNewSeason(void * /* dummy */)
{
    void *saveH = openCareerSave(false);
    if (!saveH) {
        return;
    }
    int season = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON, nullptr, 1));
    GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON,   nullptr, static_cast<tdble>((season + 1)));
    GfParmSetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_DONE, "no");
    GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK, nullptr, static_cast<tdble>(0));
    writeCareerSave(saveH);
    GfParmReleaseHandle(saveH);

    /* Launch a fresh career race; ReInitResults() inside the race engine
       creates a new timestamped results file for the new season. */
    careerRaceNextEvent(nullptr);
}

/** Show the season standings screen. */
static void careerStandingsCallback(void * /* dummy */)
{
    showStandingsScreen(careerHubHandle);
}

/** Create (first call) or return the cached career hub screen. */
static void *careerHubInit(void *prevMenu)
{
    if (careerHubHandle) {
        return careerHubHandle;
    }

    careerHubHandle = GfuiScreenCreateEx(nullptr,
                                          nullptr, careerHubActivate,
                                          nullptr, nullptr,
                                          1);
    GfuiScreenAddBgImg(careerHubHandle, "data/img/splash-main.png");
    GfuiMenuDefaultKeysAdd(careerHubHandle);

    /* Register the hub with the race state machine so keyboard shortcuts
       and state transitions know which menu to return to. */
    ReStateInit(careerHubHandle);

    /* --- Dynamic info labels (updated on every activation) --- */
    hubTitleId = GfuiLabelCreate(careerHubHandle,
                                  "CAREER MODE",
                                  GFUI_FONT_LARGE_C,
                                  320, 458,
                                  GFUI_ALIGN_HC_VB, 80);

    hubSeasonId = GfuiLabelCreate(careerHubHandle,
                                   "Season 1",
                                   GFUI_FONT_MEDIUM_C,
                                   320, 428,
                                   GFUI_ALIGN_HC_VB, 64);

    hubRaceId = GfuiLabelCreate(careerHubHandle,
                                 "Round 1 / 8: Forza",
                                 GFUI_FONT_SMALL_C,
                                 320, 407,
                                 GFUI_ALIGN_HC_VB, 80);

    hubWinsId = GfuiLabelCreate(careerHubHandle,
                                 "Career wins: 0",
                                 GFUI_FONT_SMALL_C,
                                 320, 388,
                                 GFUI_ALIGN_HC_VB, 64);

    hubChampsId = GfuiLabelCreate(careerHubHandle,
                                   "Championships: 0",
                                   GFUI_FONT_SMALL_C,
                                   320, 372,
                                   GFUI_ALIGN_HC_VB, 64);

    /* Season-complete banner (hidden until season ends) */
    {
        float goldColor[4] = {1.0f, 0.85f, 0.0f, 1.0f};
        hubSeaDoneId = GfuiLabelCreateEx(careerHubHandle,
                                          "",
                                          goldColor,
                                          GFUI_FONT_MEDIUM_C,
                                          320, 350,
                                          GFUI_ALIGN_HC_VB, 80);
        GfuiVisibilitySet(careerHubHandle, hubSeaDoneId, GFUI_INVISIBLE);
    }

    /* --- Buttons --- */
    hubRaceNextBtnId = GfuiMenuButtonCreate(careerHubHandle,
                         "Race Next Event",
                         "Start the next race in your career season",
                         nullptr, careerRaceNextEvent);

    hubNewSeasonBtnId = GfuiMenuButtonCreate(careerHubHandle,
                          "Start New Season",
                          "Begin the next season of your racing career",
                          nullptr, careerStartNewSeason);
    /* Hidden until the season is complete */
    GfuiVisibilitySet(careerHubHandle, hubNewSeasonBtnId, GFUI_INVISIBLE);

    GfuiMenuButtonCreate(careerHubHandle,
                         "Season Standings",
                         "View current season driver standings",
                         nullptr, careerStandingsCallback);

    GfuiMenuBackQuitButtonCreate(careerHubHandle,
                                  "Back",
                                  "Return to the Career menu",
                                  prevMenu, careerHubShutdown);

    return careerHubHandle;
}

/* ==================== New Career Screen ==================== */

/** Called when the player confirms their name and starts a new career. */
static void careerStartNew(void * /* dummy */)
{
    const char *name = GfuiEditboxGetString(careerNewHandle, newNameEditId);
    if (!name || name[0] == '\0') {
        name = "Player";
    }

    /* Ensure the save directory exists */
    char dirTestPath[1024];
    snprintf(dirTestPath, sizeof(dirTestPath), "%s%s/_placeholder",
             GetLocalDir(), CAREER_SUBDIR);
    GfCreateDirForFile(dirTestPath);

    /* Write a fresh career save */
    char savePath[1024];
    buildSavePath(savePath, sizeof(savePath));
    void *h = GfParmReadFile(savePath, GFPARM_RMODE_CREAT | GFPARM_RMODE_PRIVATE);
    if (h) {
        GfParmSetStr(h, CS_SECT_PLAYER, CS_ATTR_NAME,   name);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_SEASON, nullptr, 1.0f);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_WINS,   nullptr, 0.0f);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_CHAMPS, nullptr, 0.0f);
        writeCareerSave(h);
        GfParmReleaseHandle(h);
    }

    /* Discard any cached hub so it is re-created with fresh state */
    if (careerHubHandle) {
        GfuiScreenRelease(careerHubHandle);
        careerHubHandle = nullptr;
    }

    GfuiScreenActivate(careerHubInit(careerSelHandle));
}

/** Build and return the new-career name-entry screen. */
static void *careerNewMenuInit(void *prevMenu)
{
    if (careerNewHandle) {
        GfuiScreenRelease(careerNewHandle);
    }

    careerNewHandle = GfuiScreenCreateEx(nullptr, nullptr, nullptr, nullptr, nullptr, 1);
    GfuiScreenAddBgImg(careerNewHandle, "data/img/splash-main.png");
    GfuiMenuDefaultKeysAdd(careerNewHandle);

    GfuiTitleCreate(careerNewHandle, "NEW CAREER", 0);

    GfuiLabelCreate(careerNewHandle,
                    "Enter your driver name:",
                    GFUI_FONT_MEDIUM_C,
                    320, 370, GFUI_ALIGN_HC_VB, 0);

    newNameEditId = GfuiEditboxCreate(careerNewHandle,
                                      "Player",
                                      GFUI_FONT_MEDIUM_C,
                                      220, 340,
                                      200, 31,
                                      nullptr, nullptr, nullptr);

    GfuiMenuButtonCreate(careerNewHandle,
                         "Start Career",
                         "Begin your racing career with this driver name",
                         nullptr, careerStartNew);

    GfuiMenuBackQuitButtonCreate(careerNewHandle,
                                  "Back",
                                  "Return to Career menu",
                                  prevMenu, GfuiScreenActivate);

    return careerNewHandle;
}

/* ==================== Career Selection Screen ==================== */

/** Switch to the career hub for an existing career. */
static void careerContinue(void * /* dummy */)
{
    GfuiScreenActivate(careerHubInit(careerSelHandle));
}

/** Initialize the career entry/selection screen and return its handle.
 *  If a career save already exists the screen offers both "Continue" and
 *  "New Career".  Otherwise only "New Career" is available.
 *  @param prevMenu  Handle to activate when the player presses Back.
 */
void *CareerMenuInit(void *prevMenu)
{
    /* Rebuild on every call so "Continue" reflects the current save state */
    if (careerSelHandle) {
        GfuiScreenRelease(careerSelHandle);
    }

    careerSelHandle = GfuiMenuScreenCreate("CAREER MODE");
    GfuiScreenAddBgImg(careerSelHandle, "data/img/splash-main.png");

    if (careerSaveExists()) {
        GfuiMenuButtonCreate(careerSelHandle,
                             "Continue Career",
                             "Resume your existing career",
                             nullptr, careerContinue);
    }

    GfuiMenuButtonCreate(careerSelHandle,
                         "New Career",
                         "Start a brand-new career from scratch",
                         careerNewMenuInit(careerSelHandle), GfuiScreenActivate);

    GfuiMenuBackQuitButtonCreate(careerSelHandle,
                                  "Back",
                                  "Return to the Main Menu",
                                  prevMenu, GfuiScreenActivate);

    return careerSelHandle;
}
