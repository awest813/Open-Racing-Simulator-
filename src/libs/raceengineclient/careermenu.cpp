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

    Career Mode Progression:
      Rookie Series (4 tracks, 5 drivers) ->
      Pro Series    (6 tracks, 7 drivers) ->
      Elite Series  (8 tracks, 8 drivers)

    The player must win the championship (finish season as rank-1 driver)
    to advance from one tier to the next.  Elite is the final tier.

    Persistent career data is saved to ~/.torcs/career/career.xml and
    tracks per-season statistics as well as a season history log.
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

/* ==================== Career Tier Definitions ==================== */

/** Describes one tier of the career ladder. */
struct CareerTierDef {
    int         id;                 /**< 1 = Rookie, 2 = Pro, 3 = Elite */
    const char *name;               /**< Human-readable tier name        */
    const char *racemanFile;        /**< Filename under config/raceman/  */
    int         numTracks;          /**< Number of rounds in the season  */
    const char *trackNames[8];      /**< Display names (same order as XML) */
    const char *trackKeys[8];       /**< XML/results directory keys        */
    float       color[4];           /**< UI accent colour (RGBA)           */
};

static const CareerTierDef kTiers[3] = {
    /* ---- Tier 1: Rookie ---- */
    { 1, "Rookie", "career-rookie.xml", 4,
      { "Forza", "Corkscrew", "Wheel 1", "Street 1", nullptr, nullptr, nullptr, nullptr },
      { "forza", "corkscrew", "wheel-1", "street-1", nullptr, nullptr, nullptr, nullptr },
      { 0.25f, 0.85f, 0.25f, 1.0f } },
    /* ---- Tier 2: Pro ---- */
    { 2, "Pro", "career-pro.xml", 6,
      { "Brondehach", "E-Track 3", "Ole Road 1", "Ruudskogen",
        "G-Track 2", "Alpine 1", nullptr, nullptr },
      { "brondehach", "e-track-3", "ole-road-1", "ruudskogen",
        "g-track-2", "alpine-1", nullptr, nullptr },
      { 0.3f, 0.6f, 1.0f, 1.0f } },
    /* ---- Tier 3: Elite ---- */
    { 3, "Elite", "career-elite.xml", 8,
      { "Forza", "Corkscrew", "Brondehach", "E-Track 3",
        "Wheel 1", "G-Track 1", "Street 1", "Aalborg" },
      { "forza", "corkscrew", "brondehach", "e-track-3",
        "wheel-1", "g-track-1", "street-1", "aalborg" },
      { 1.0f, 0.85f, 0.0f, 1.0f } },
};
static const int NUM_TIERS = 3;

/* ==================== Constants ==================== */

/* Career save file path components (relative to GetLocalDir()) */
static const char *CAREER_SUBDIR     = "career";
static const char *CAREER_SAVE_FNAME = "career.xml";

/* Race name in career XMLs - must match the <Races><1><name> attribute */
static const char *CAREER_RACE_NAME  = "Career Race";

/* Maximum seasons stored in the history log */
static const int MAX_HISTORY = 20;

/* Career XML sections / attributes */
#define CS_SECT_PLAYER    "Player"
#define CS_ATTR_NAME      "name"
#define CS_ATTR_SEASON    "season"
#define CS_ATTR_WINS      "total wins"
#define CS_ATTR_CHAMPS    "championships"
#define CS_ATTR_PODIUMS   "total podiums"
#define CS_ATTR_RACES     "total races"
#define CS_ATTR_LAST_TRK  "last track"   /* last CUR_TRACK seen from results */
#define CS_ATTR_SEA_DONE  "season done"  /* "yes" when all rounds complete   */
#define CS_ATTR_TIER      "tier"         /* 1 = Rookie, 2 = Pro, 3 = Elite   */
#define CS_ATTR_PROMOTED  "promoted"     /* "yes" if promoted at last season end */
#define CS_ATTR_SEA_CHAMP "season champion" /* "yes" if human won the last season */

/* Season history sub-section */
#define CS_SECT_HISTORY   "History"
#define CS_ATTR_H_TIER    "tier"
#define CS_ATTR_H_WINS    "wins"
#define CS_ATTR_H_PODIUMS "podiums"
#define CS_ATTR_H_CHAMP   "champion"
#define CS_ATTR_H_POINTS  "points"

/* ==================== Screen Handles ==================== */

static void *careerSelHandle     = nullptr; /* entry/selection screen     */
static void *careerNewHandle     = nullptr; /* new career (name entry)    */
static void *careerHubHandle     = nullptr; /* career hub (dashboard)     */
static void *careerStatsHandle   = nullptr; /* career statistics screen   */
static void *careerConfirmHandle = nullptr; /* overwrite confirmation      */

/* ==================== Widget IDs ==================== */

/* New career screen */
static int newNameEditId = -1;

/* Career hub screen - updated on every activation */
static int hubTitleId       = -1;  /* "CAREER - <name>"              */
static int hubTierId        = -1;  /* "[ROOKIE SERIES]" (coloured)   */
static int hubSeasonId      = -1;  /* "Season 2"                     */
static int hubRaceId        = -1;  /* "Next event: Forza (1/4)"      */
static int hubStatsId       = -1;  /* "Wins: 5  Podiums: 12  Champs: 1" */
static int hubCarId         = -1;  /* "Active Car: ..."               */
static int hubSeaDoneId     = -1;  /* "Season complete!" banner       */
static int hubPromotionId   = -1;  /* "Promoted to Pro!" / "Champion!" */
static int hubRaceNextBtnId   = -1;
static int hubNewSeasonBtnId  = -1;

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

/** Return the CareerTierDef for the tier stored in the save, defaulting to
 *  Tier 1 (Rookie) if the attribute is absent or out of range. */
static const CareerTierDef *getTierDef(void *saveH)
{
    int t = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_TIER, nullptr, 1));
    if (t < 1) t = 1;
    if (t > NUM_TIERS) t = NUM_TIERS;
    return &kTiers[t - 1];
}

/* ==================== Results Helpers ==================== */

/** Raceman XML basename without extension (e.g. "career-rookie"). */
static void careerRacemanStem(char *buf, int size, const CareerTierDef *tier)
{
    snprintf(buf, size, "%s", tier->racemanFile);
    char *dot = strstr(buf, ".xml");
    if (dot) {
        *dot = '\0';
    }
}

/** Results directory for a career tier (matches ReInfo->_reFilename from raceman XML). */
static void careerResultsDir(char *buf, int size, const CareerTierDef *tier)
{
    char stem[128];
    careerRacemanStem(stem, sizeof(stem), tier);
    snprintf(buf, size, "%sresults/%s", GetLocalDir(), stem);
}

/**
 * Search the tier's career results directory and load standings from the most
 * recent results XML file.  Returns the file handle; the caller must release it.
 * Returns nullptr if no results exist.
 */
static void *openLatestCareerResultsForTier(const CareerTierDef *tier)
{
    char resultsDir[1024];
    careerResultsDir(resultsDir, sizeof(resultsDir), tier);

    tFList *files = GfDirGetListFiltered(resultsDir, "xml");
    /* Legacy path used by early career builds before per-tier directories. */
    if (!files && tier->id == 1) {
        snprintf(resultsDir, sizeof(resultsDir), "%sresults/career", GetLocalDir());
        files = GfDirGetListFiltered(resultsDir, "xml");
    }
    if (!files) {
        return nullptr;
    }

    /* Alphabetically last filename = most recent timestamp */
    tFList *latest = files;
    tFList *cur    = files->next;
    while (cur != files) {
        if (strcmp(cur->name, latest->name) > 0) {
            latest = cur;
        }
        cur = cur->next;
    }

    char resultPath[1024];
    snprintf(resultPath, sizeof(resultPath), "%s/%s", resultsDir, latest->name);
    GfDirFreeList(files, nullptr, true, false);

    return GfParmReadFile(resultPath, GFPARM_RMODE_STD | GFPARM_RMODE_PRIVATE);
}

/**
 * Read the season standings from the latest career results file and fill
 * namesOut with formatted "<name>  <pts> pts" strings.
 *
 * @param tier      Career tier (selects results/<raceman-stem>/).
 * @param namesOut  Output array; each entry up to 72 chars.
 * @param numOut    Set to the number of valid entries placed.
 * @param maxLines  Maximum entries to fill.
 * @return          Current CUR_TRACK index (1-based), or 0 if no results.
 */
static int readLatestCareerResults(const CareerTierDef *tier,
                                   char namesOut[][72], int *numOut, int maxLines)
{
    *numOut = 0;

    void *results = openLatestCareerResultsForTier(tier);
    if (!results) {
        char resultsDir[1024];
        careerResultsDir(resultsDir, sizeof(resultsDir), tier);
        GfTrace("Career: no results files in %s\n", resultsDir);
        return 0;
    }

    int curTrack = static_cast<int>(
        GfParmGetNum(results, RE_SECT_CURRENT, RE_ATTR_CUR_TRACK, nullptr, 1));

    int n = GfParmGetEltNb(results, RE_SECT_STANDINGS);

    /* Standings-carryover fix: Check if we are at the start of a brand new season
       (lastTrack == 0) and the found results file actually belongs to the previous
       completed season (curTrack == 1 with existing standings). If so, ignore it. */
    int lastTrack = 0;
    bool seasonDone = false;
    void *saveH = openCareerSave(false);
    if (saveH) {
        lastTrack = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK, nullptr, 0));
        const char *seaDone = GfParmGetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_DONE, "no");
        seasonDone = (strcmp(seaDone, "yes") == 0);
        GfParmReleaseHandle(saveH);
    }
    if (!seasonDone && lastTrack == 0 && curTrack == 1 && n > 0) {
        GfParmReleaseHandle(results);
        return 1;
    }
    if (n > maxLines) n = maxLines;
    *numOut = n;

    for (int i = 0; i < n; i++) {
        char secPath[256];
        snprintf(secPath, sizeof(secPath), "%s/%d", RE_SECT_STANDINGS, i + 1);
        const char *name = GfParmGetStr(results, secPath, RE_ATTR_NAME, "Unknown");
        int pts = static_cast<int>(
            GfParmGetNum(results, secPath, RE_ATTR_POINTS, nullptr, 0));
        snprintf(namesOut[i], 72, "%-24s %4d pts", name, pts);
    }

    GfParmReleaseHandle(results);
    return curTrack;
}

/** Find the human driver's name in the standings of the given results handle.
 *  Returns nullptr if not found.  The returned pointer is owned by the handle. */
static const char *careerGetHumanName(void *results)
{
    int n = GfParmGetEltNb(results, RE_SECT_STANDINGS);
    for (int i = 1; i <= n; i++) {
        char secPath[256];
        snprintf(secPath, sizeof(secPath), "%s/%d", RE_SECT_STANDINGS, i);
        const char *mod = GfParmGetStr(results, secPath, RE_ATTR_MODULE, "");
        if (strcmp(mod, "human") == 0) {
            return GfParmGetStr(results, secPath, RE_ATTR_NAME, nullptr);
        }
    }
    return nullptr;
}

/** Return the human driver's total championship points from the standings. */
static int careerGetHumanPoints(void *results)
{
    int n = GfParmGetEltNb(results, RE_SECT_STANDINGS);
    for (int i = 1; i <= n; i++) {
        char secPath[256];
        snprintf(secPath, sizeof(secPath), "%s/%d", RE_SECT_STANDINGS, i);
        const char *mod = GfParmGetStr(results, secPath, RE_ATTR_MODULE, "");
        if (strcmp(mod, "human") == 0) {
            return static_cast<int>(
                GfParmGetNum(results, secPath, RE_ATTR_POINTS, nullptr, 0));
        }
    }
    return 0;
}

/**
 * Count the number of race wins (P1 finishes) by the human driver across
 * all rounds of the tier's track list.
 */
static int careerCountSeasonWins(void *results, const CareerTierDef *tier,
                                  const char *humanName)
{
    if (!results || !humanName) return 0;
    int wins = 0;
    for (int t = 0; t < tier->numTracks; t++) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s/%s/%s/1",
                 tier->trackKeys[t],
                 RE_SECT_RESULTS, CAREER_RACE_NAME, RE_SECT_RANK);
        const char *winner = GfParmGetStr(results, path, RE_ATTR_NAME, nullptr);
        if (winner && strcmp(winner, humanName) == 0) {
            wins++;
        }
    }
    return wins;
}

/**
 * Count the number of podium finishes (P1-P3) by the human driver across
 * all rounds of the tier's track list.
 */
static int careerCountSeasonPodiums(void *results, const CareerTierDef *tier,
                                     const char *humanName)
{
    if (!results || !humanName) return 0;
    int podiums = 0;
    for (int t = 0; t < tier->numTracks; t++) {
        for (int pos = 1; pos <= 3; pos++) {
            char path[512];
            snprintf(path, sizeof(path), "%s/%s/%s/%s/%d",
                     tier->trackKeys[t],
                     RE_SECT_RESULTS, CAREER_RACE_NAME, RE_SECT_RANK, pos);
            const char *driver = GfParmGetStr(results, path, RE_ATTR_NAME, nullptr);
            if (driver && strcmp(driver, humanName) == 0) {
                podiums++;
                break; /* human can only appear once per race */
            }
        }
    }
    return podiums;
}

/**
 * Count how many race rounds have been run (i.e. have at least one ranked
 * driver in the results) within the tier's track list.
 */
static int careerCountSeasonRaces(void *results, const CareerTierDef *tier)
{
    if (!results) return 0;
    int races = 0;
    for (int t = 0; t < tier->numTracks; t++) {
        char path[512];
        snprintf(path, sizeof(path), "%s/%s/%s/%s",
                 tier->trackKeys[t],
                 RE_SECT_RESULTS, CAREER_RACE_NAME, RE_SECT_RANK);
        if (GfParmGetEltNb(results, path) > 0) {
            races++;
        }
    }
    return races;
}

/** Return true when the human driver is the season champion (rank-1 with
 *  module == "human"). */
static bool careerIsChampion(void *results)
{
    if (!results) return false;
    if (GfParmGetEltNb(results, RE_SECT_STANDINGS) < 1) return false;
    char secPath[256];
    snprintf(secPath, sizeof(secPath), "%s/1", RE_SECT_STANDINGS);
    const char *mod = GfParmGetStr(results, secPath, RE_ATTR_MODULE, "");
    return (strcmp(mod, "human") == 0);
}

/* ==================== Season History Helpers ==================== */

/**
 * Append a completed season's statistics to the history section of the save.
 * Only the last MAX_HISTORY entries are retained (oldest entries are dropped).
 */
static void appendSeasonHistory(void *saveH, const CareerTierDef *tier,
                                  int wins, int podiums, bool champion, int points)
{
    int existing = GfParmGetEltNb(saveH, CS_SECT_HISTORY);

    /* Shift entries down if we would exceed the limit */
    if (existing >= MAX_HISTORY) {
        for (int i = 1; i < existing; i++) {
            char oldSec[64], newSec[64];
            snprintf(oldSec, sizeof(oldSec), "%s/%d", CS_SECT_HISTORY, i + 1);
            snprintf(newSec, sizeof(newSec), "%s/%d", CS_SECT_HISTORY, i);

            const char *t  = GfParmGetStr(saveH, oldSec, CS_ATTR_H_TIER,    "");
            const char *ch = GfParmGetStr(saveH, oldSec, CS_ATTR_H_CHAMP,   "no");
            int w          = static_cast<int>(GfParmGetNum(saveH, oldSec, CS_ATTR_H_WINS,    nullptr, 0));
            int p          = static_cast<int>(GfParmGetNum(saveH, oldSec, CS_ATTR_H_PODIUMS, nullptr, 0));
            int pts        = static_cast<int>(GfParmGetNum(saveH, oldSec, CS_ATTR_H_POINTS,  nullptr, 0));

            GfParmSetStr(saveH, newSec, CS_ATTR_H_TIER,    t);
            GfParmSetStr(saveH, newSec, CS_ATTR_H_CHAMP,   ch);
            GfParmSetNum(saveH, newSec, CS_ATTR_H_WINS,    nullptr, static_cast<tdble>(w));
            GfParmSetNum(saveH, newSec, CS_ATTR_H_PODIUMS, nullptr, static_cast<tdble>(p));
            GfParmSetNum(saveH, newSec, CS_ATTR_H_POINTS,  nullptr, static_cast<tdble>(pts));
        }
        existing = MAX_HISTORY - 1;
    }

    /* Write new entry */
    char newSec[64];
    snprintf(newSec, sizeof(newSec), "%s/%d", CS_SECT_HISTORY, existing + 1);
    GfParmSetStr(saveH, newSec, CS_ATTR_H_TIER,    tier->name);
    GfParmSetStr(saveH, newSec, CS_ATTR_H_CHAMP,   champion ? "yes" : "no");
    GfParmSetNum(saveH, newSec, CS_ATTR_H_WINS,    nullptr, static_cast<tdble>(wins));
    GfParmSetNum(saveH, newSec, CS_ATTR_H_PODIUMS, nullptr, static_cast<tdble>(podiums));
    GfParmSetNum(saveH, newSec, CS_ATTR_H_POINTS,  nullptr, static_cast<tdble>(points));
}

/* ==================== Career Stats Screen ==================== */

/** Build and activate the career statistics/history screen. */
static void showCareerStatsScreen(void *prevMenu)
{
    if (careerStatsHandle) {
        GfuiScreenRelease(careerStatsHandle);
        careerStatsHandle = nullptr;
    }

    careerStatsHandle = GfuiScreenCreateEx(nullptr, nullptr, nullptr,
                                            nullptr, nullptr, 1);
    GfuiScreenAddBgImg(careerStatsHandle, "data/img/splash-main.png");
    GfuiMenuDefaultKeysAdd(careerStatsHandle);

    GfuiTitleCreate(careerStatsHandle, "CAREER STATISTICS", 0);

    void *saveH = openCareerSave(false);

    if (!saveH) {
        GfuiLabelCreate(careerStatsHandle, "No career data found.",
                        GFUI_FONT_MEDIUM_C, 320, 360, GFUI_ALIGN_HC_VB, 0);
        GfuiMenuBackQuitButtonCreate(careerStatsHandle, "Back", nullptr,
                                      prevMenu, GfuiScreenActivate);
        GfuiScreenActivate(careerStatsHandle);
        return;
    }

    const char *nameStr = GfParmGetStr(saveH, CS_SECT_PLAYER, CS_ATTR_NAME, "Player");
    char playerName[64];
    snprintf(playerName, sizeof(playerName), "%s", nameStr ? nameStr : "Player");

    int season  = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON,  nullptr, 1));
    int wins    = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_WINS,    nullptr, 0));
    int champs  = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_CHAMPS,  nullptr, 0));
    int pods    = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_PODIUMS, nullptr, 0));
    int races   = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_RACES,   nullptr, 0));
    const CareerTierDef *tier = getTierDef(saveH);

    /* Driver summary */
    char buf[160];
    snprintf(buf, sizeof(buf), "Driver: %s", playerName);
    GfuiLabelCreate(careerStatsHandle, buf, GFUI_FONT_MEDIUM_C,
                    320, 418, GFUI_ALIGN_HC_VB, 0);

    snprintf(buf, sizeof(buf), "Current Tier: %s Series", tier->name);
    GfuiLabelCreateEx(careerStatsHandle, buf, const_cast<float *>(tier->color),
                      GFUI_FONT_MEDIUM_C, 320, 398, GFUI_ALIGN_HC_VB, 0);

    int seasonsCompleted = season - 1;
    snprintf(buf, sizeof(buf),
             "Seasons: %d   Races: %d   Wins: %d   Podiums: %d   Championships: %d",
             seasonsCompleted, races, wins, pods, champs);
    GfuiLabelCreate(careerStatsHandle, buf, GFUI_FONT_SMALL_C,
                    320, 378, GFUI_ALIGN_HC_VB, 0);

    /* History table */
    int histCount = GfParmGetEltNb(saveH, CS_SECT_HISTORY);
    if (histCount > 0) {
        float headerColor[4] = { 1.0f, 0.85f, 0.0f, 1.0f };
        GfuiLabelCreateEx(careerStatsHandle,
                          "#    Tier       W   Pod   Pts   Champion",
                          headerColor, GFUI_FONT_SMALL_C,
                          320, 355, GFUI_ALIGN_HC_VB, 0);

        float rowColors[2][4] = {
            { 1.0f, 1.0f, 1.0f, 1.0f },
            { 0.7f, 0.85f, 1.0f, 1.0f }
        };

        /* Display newest first */
        int displayIdx = 0;
        int y = 337;
        for (int i = histCount; i >= 1 && y > 60; i--, displayIdx++, y -= 18) {
            char secPath[64];
            snprintf(secPath, sizeof(secPath), "%s/%d", CS_SECT_HISTORY, i);
            const char *t  = GfParmGetStr(saveH, secPath, CS_ATTR_H_TIER,    "?");
            const char *ch = GfParmGetStr(saveH, secPath, CS_ATTR_H_CHAMP,   "no");
            int w          = static_cast<int>(GfParmGetNum(saveH, secPath, CS_ATTR_H_WINS,    nullptr, 0));
            int p          = static_cast<int>(GfParmGetNum(saveH, secPath, CS_ATTR_H_PODIUMS, nullptr, 0));
            int pts        = static_cast<int>(GfParmGetNum(saveH, secPath, CS_ATTR_H_POINTS,  nullptr, 0));
            /* History entry i is season number i (entries stored chronologically). */

            char row[96];
            snprintf(row, sizeof(row), "%-4d %-9s %2d   %3d   %3d   %s",
                     i, t, w, p, pts,
                     (strcmp(ch, "yes") == 0) ? "YES" : "-");
            GfuiLabelCreateEx(careerStatsHandle, row, rowColors[displayIdx % 2],
                              GFUI_FONT_SMALL_C, 320, y, GFUI_ALIGN_HC_VB, 0);
        }
    } else {
        GfuiLabelCreate(careerStatsHandle,
                        "No completed seasons yet.",
                        GFUI_FONT_MEDIUM_C, 320, 345, GFUI_ALIGN_HC_VB, 0);
        GfuiLabelCreate(careerStatsHandle,
                        "Complete a career season to see history here.",
                        GFUI_FONT_SMALL_C, 320, 322, GFUI_ALIGN_HC_VB, 0);
    }

    GfParmReleaseHandle(saveH);

    GfuiMenuBackQuitButtonCreate(careerStatsHandle, "Back",
                                  "Return to Career Hub",
                                  prevMenu, GfuiScreenActivate);
    GfuiScreenActivate(careerStatsHandle);
}

/* ==================== Season Standings Screen ==================== */

/** Build and activate the season standings screen. */
static void showStandingsScreen(void *prevMenu)
{
    static void *standsHandle = nullptr;
    if (standsHandle) {
        GfuiScreenRelease(standsHandle);
        standsHandle = nullptr;
    }

    standsHandle = GfuiScreenCreateEx(nullptr, nullptr, nullptr, nullptr, nullptr, 1);
    GfuiScreenAddBgImg(standsHandle, "data/img/splash-main.png");

    void *saveH = openCareerSave(false);
    int season  = 1;
    const CareerTierDef *tier = &kTiers[0];
    if (saveH) {
        season = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON, nullptr, 1));
        tier   = getTierDef(saveH);
        GfParmReleaseHandle(saveH);
    }

    char title[80];
    snprintf(title, sizeof(title), "SEASON %d STANDINGS  [%s]", season, tier->name);
    GfuiTitleCreate(standsHandle, title, 0);

    const int MAX_DRIVERS = 20;
    char driverLines[MAX_DRIVERS][72];
    int  numDrivers = 0;
    int  curTrack   = readLatestCareerResults(tier, driverLines, &numDrivers, MAX_DRIVERS);

    if (curTrack > 0) {
        char subTitle[128];
        if (curTrack == 1 && numDrivers > 0) {
            snprintf(subTitle, sizeof(subTitle), "Final Standings - Season %d", season);
        } else {
            int racesCompleted = curTrack - 1;
            snprintf(subTitle, sizeof(subTitle),
                     "After round %d of %d", racesCompleted, tier->numTracks);
        }
        GfuiLabelCreate(standsHandle, subTitle, GFUI_FONT_MEDIUM_C,
                        320, 418, GFUI_ALIGN_HC_VB, 0);

        float headerColor[4] = { 1.0f, 0.85f, 0.0f, 1.0f };
        GfuiLabelCreateEx(standsHandle,
                          "Pos  Driver                    Points",
                          headerColor, GFUI_FONT_SMALL_C,
                          320, 395, GFUI_ALIGN_HC_VB, 0);

        float rowColors[2][4] = {
            { 1.0f, 1.0f, 1.0f, 1.0f },
            { 0.7f, 0.85f, 1.0f, 1.0f }
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
 *  Also handles end-of-season: updates career stats, records season history,
 *  and detects tier promotion. */
static void careerHubActivate(void * /* dummy */)
{
    /* Re-init the race engine (same pattern as singleplayer.cpp). */
    ReInit();
    ReInfo->_reMenuScreen = careerHubHandle;

    void *saveH = openCareerSave(false);
    if (!saveH) {
        return;
    }

    const char *nameStr = GfParmGetStr(saveH, CS_SECT_PLAYER, CS_ATTR_NAME, "Player");
    char playerName[64];
    snprintf(playerName, sizeof(playerName), "%s", nameStr ? nameStr : "Player");

    int season     = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON,  nullptr, 1));
    int wins       = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_WINS,    nullptr, 0));
    int champs     = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_CHAMPS,  nullptr, 0));
    int podiums    = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_PODIUMS, nullptr, 0));
    int totalRaces = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_RACES,   nullptr, 0));
    int lastTrack  = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK,nullptr, 0));
    const char *seaDone    = GfParmGetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_DONE,  "no");
    bool seasonDone        = (strcmp(seaDone,  "yes") == 0);
    const char *promStr    = GfParmGetStr(saveH, CS_SECT_PLAYER, CS_ATTR_PROMOTED,  "no");
    bool justPromoted      = (strcmp(promStr,  "yes") == 0);
    const char *champStr   = GfParmGetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_CHAMP, "no");
    bool seasonChampion    = (strcmp(champStr, "yes") == 0);

    const CareerTierDef *tier = getTierDef(saveH);

    /* Determine how far through the season we are */
    char dummy[20][72];
    int  nDummy   = 0;
    int  curTrack = readLatestCareerResults(tier, dummy, &nDummy, 20);

    /* ---- Season completion detection ----
       The race engine resets CUR_TRACK to 1 after the final round.
       Condition: last seen track == numTracks, now wrapped to 1, and
       standings entries exist (races were actually played). */
    if (!seasonDone && lastTrack == tier->numTracks && curTrack == 1 && nDummy > 0) {
        seasonDone = true;

        void *results      = openLatestCareerResultsForTier(tier);
        const char *hName  = results ? careerGetHumanName(results) : nullptr;
        int seasonWins     = careerCountSeasonWins(results, tier, hName);
        int seasonPodiums  = careerCountSeasonPodiums(results, tier, hName);
        int seasonRaces    = careerCountSeasonRaces(results, tier);
        bool champion      = careerIsChampion(results);
        int humanPoints    = careerGetHumanPoints(results);
        if (results) GfParmReleaseHandle(results);

        wins       += seasonWins;
        podiums    += seasonPodiums;
        totalRaces += seasonRaces;
        if (champion) champs++;

        /* Record season in history before potentially changing tier */
        appendSeasonHistory(saveH, tier, seasonWins, seasonPodiums,
                            champion, humanPoints);

        /* Tier promotion: champion and not already in the top tier */
        bool promoted = false;
        int newTier = tier->id;
        if (champion && tier->id < NUM_TIERS) {
            newTier  = tier->id + 1;
            promoted = true;
        }

        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_WINS,    nullptr, static_cast<tdble>(wins));
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_CHAMPS,  nullptr, static_cast<tdble>(champs));
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_PODIUMS, nullptr, static_cast<tdble>(podiums));
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_RACES,   nullptr, static_cast<tdble>(totalRaces));
        GfParmSetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_DONE,  "yes");
        GfParmSetStr(saveH, CS_SECT_PLAYER, CS_ATTR_PROMOTED,  promoted ? "yes" : "no");
        GfParmSetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_CHAMP, champion ? "yes" : "no");
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK, nullptr, 0);
        if (promoted) {
            GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_TIER, nullptr, static_cast<tdble>(newTier));
            tier         = &kTiers[newTier - 1];
            justPromoted = true;
        }
        seasonChampion = champion;
        writeCareerSave(saveH);
    } else if (!seasonDone && curTrack > lastTrack && nDummy > 0) {
        /* Advance lastTrack so we can detect season end next time */
        GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK, nullptr, static_cast<tdble>(curTrack));
        writeCareerSave(saveH);
        lastTrack = curTrack;
    }

    GfParmReleaseHandle(saveH);

    /* ---- Update hub labels ---- */
    char buf[160];

    snprintf(buf, sizeof(buf), "CAREER  -  %s", playerName);
    GfuiLabelSetText(careerHubHandle, hubTitleId, buf);

    snprintf(buf, sizeof(buf), "[ %s SERIES ]", tier->name);
    GfuiLabelSetText(careerHubHandle, hubTierId, buf);
    GfuiLabelSetColor(careerHubHandle, hubTierId, const_cast<float *>(tier->color));

    snprintf(buf, sizeof(buf), "Season %d", season);
    GfuiLabelSetText(careerHubHandle, hubSeasonId, buf);

    snprintf(buf, sizeof(buf), "Wins: %d   Podiums: %d   Championships: %d   Races: %d",
             wins, podiums, champs, totalRaces);
    GfuiLabelSetText(careerHubHandle, hubStatsId, buf);

    if (tier->id == 1) {
        GfuiLabelSetText(careerHubHandle, hubCarId, "Active Car: Stock Car (car1-stock1)");
    } else if (tier->id == 2) {
        GfuiLabelSetText(careerHubHandle, hubCarId, "Active Car: GT Touring (car7-trb1)");
    } else if (tier->id == 3) {
        GfuiLabelSetText(careerHubHandle, hubCarId, "Active Car: Supercar (car1-trb3)");
    }

    if (seasonDone) {
        GfuiLabelSetText(careerHubHandle, hubSeaDoneId,
                         "Season complete!  Start a new season to continue.");
        GfuiLabelSetText(careerHubHandle, hubRaceId, "All rounds finished.");
        GfuiVisibilitySet(careerHubHandle, hubRaceNextBtnId,  GFUI_INVISIBLE);
        GfuiVisibilitySet(careerHubHandle, hubNewSeasonBtnId, GFUI_VISIBLE);

        if (justPromoted) {
            snprintf(buf, sizeof(buf),
                     "CHAMPION!  Promoted to the %s Series!", tier->name);
        } else if (seasonChampion && tier->id == NUM_TIERS) {
            snprintf(buf, sizeof(buf),
                     "Elite Series champion!  Start a new season to defend your title.");
        } else {
            snprintf(buf, sizeof(buf),
                     "Season finished.  Win the championship to advance to the next tier.");
        }
        GfuiLabelSetText(careerHubHandle, hubPromotionId, buf);
        GfuiVisibilitySet(careerHubHandle, hubSeaDoneId,    GFUI_VISIBLE);
        GfuiVisibilitySet(careerHubHandle, hubPromotionId,  GFUI_VISIBLE);
    } else {
        GfuiLabelSetText(careerHubHandle, hubSeaDoneId, "");
        GfuiVisibilitySet(careerHubHandle, hubSeaDoneId,    GFUI_INVISIBLE);
        GfuiVisibilitySet(careerHubHandle, hubRaceNextBtnId,  GFUI_VISIBLE);
        GfuiVisibilitySet(careerHubHandle, hubNewSeasonBtnId, GFUI_INVISIBLE);
        GfuiVisibilitySet(careerHubHandle, hubPromotionId,    GFUI_INVISIBLE);

        if (curTrack >= 1 && curTrack <= tier->numTracks) {
            snprintf(buf, sizeof(buf), "Next event: %s  (Round %d / %d)",
                     tier->trackNames[curTrack - 1], curTrack, tier->numTracks);
        } else {
            snprintf(buf, sizeof(buf), "Round 1 / %d: %s",
                     tier->numTracks, tier->trackNames[0]);
        }
        GfuiLabelSetText(careerHubHandle, hubRaceId, buf);
    }
}

/** Deactivate the career hub: return to the previous screen and shut down
 *  the race engine cleanly. */
static void careerHubShutdown(void *prevMenu)
{
    GfuiScreenActivate(prevMenu);
    ReShutdown();
}

/** Launch the career race for the player's current tier. */
static void careerRaceNextEvent(void * /* dummy */)
{
    void *saveH = openCareerSave(false);
    const CareerTierDef *tier = &kTiers[0];
    if (saveH) {
        tier = getTierDef(saveH);
        GfParmReleaseHandle(saveH);
    }

    char path[1024];
    snprintf(path, sizeof(path), "%sconfig/raceman/%s", GetLocalDir(), tier->racemanFile);
    void *params = GfParmReadFile(path, GFPARM_RMODE_STD);
    if (!params) {
        GfTrace("Career: cannot open %s\n", path);
        return;
    }
    ReLaunchRaceman(params);
}

/** Advance to the next career season.
 *  Increments the season counter, clears season-done and promoted flags,
 *  resets the last-track cursor, and launches the race for the current tier
 *  (which may already have been promoted at season end). */
static void careerStartNewSeason(void * /* dummy */)
{
    void *saveH = openCareerSave(false);
    if (!saveH) return;

    int season = static_cast<int>(GfParmGetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON, nullptr, 1));
    GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_SEASON,   nullptr, static_cast<tdble>(season + 1));
    GfParmSetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_DONE,  "no");
    GfParmSetStr(saveH, CS_SECT_PLAYER, CS_ATTR_PROMOTED,  "no");
    GfParmSetStr(saveH, CS_SECT_PLAYER, CS_ATTR_SEA_CHAMP, "no");
    GfParmSetNum(saveH, CS_SECT_PLAYER, CS_ATTR_LAST_TRK, nullptr, static_cast<tdble>(0));
    writeCareerSave(saveH);
    GfParmReleaseHandle(saveH);

    careerRaceNextEvent(nullptr);
}

/** Show the season standings screen. */
static void careerStandingsCallback(void * /* dummy */)
{
    showStandingsScreen(careerHubHandle);
}

/** Show the career statistics screen. */
static void careerStatsCallback(void * /* dummy */)
{
    showCareerStatsScreen(careerHubHandle);
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

    ReStateInit(careerHubHandle);

    /* Static layout: all dynamic labels created with placeholder text.
       Values are replaced in careerHubActivate() on every activation. */

    hubTitleId = GfuiLabelCreate(careerHubHandle,
                                  "CAREER MODE",
                                  GFUI_FONT_LARGE_C,
                                  320, 458,
                                  GFUI_ALIGN_HC_VB, 80);

    /* Tier badge - colour set dynamically per tier */
    {
        float tierColor[4] = { 0.25f, 0.85f, 0.25f, 1.0f }; /* Rookie green */
        hubTierId = GfuiLabelCreateEx(careerHubHandle,
                                       "[ ROOKIE SERIES ]",
                                       tierColor,
                                       GFUI_FONT_MEDIUM_C,
                                       320, 436,
                                       GFUI_ALIGN_HC_VB, 64);
    }

    hubSeasonId = GfuiLabelCreate(careerHubHandle,
                                   "Season 1",
                                   GFUI_FONT_MEDIUM_C,
                                   320, 415,
                                   GFUI_ALIGN_HC_VB, 64);

    hubRaceId = GfuiLabelCreate(careerHubHandle,
                                 "Round 1 / 4: Forza",
                                 GFUI_FONT_SMALL_C,
                                 320, 396,
                                 GFUI_ALIGN_HC_VB, 80);

    hubStatsId = GfuiLabelCreate(careerHubHandle,
                                  "Wins: 0   Podiums: 0   Championships: 0   Races: 0",
                                  GFUI_FONT_SMALL_C,
                                  320, 377,
                                  GFUI_ALIGN_HC_VB, 80);

    {
        float greyColor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
        hubCarId = GfuiLabelCreateEx(careerHubHandle,
                                     "Active Car: ...",
                                     greyColor,
                                     GFUI_FONT_SMALL_C,
                                     320, 358,
                                     GFUI_ALIGN_HC_VB, 80);
    }

    /* Season-complete banner (hidden until season ends) - repositioned to 338 */
    {
        float goldColor[4] = { 1.0f, 0.85f, 0.0f, 1.0f };
        hubSeaDoneId = GfuiLabelCreateEx(careerHubHandle,
                                          "",
                                          goldColor,
                                          GFUI_FONT_MEDIUM_C,
                                          320, 338,
                                          GFUI_ALIGN_HC_VB, 80);
        GfuiVisibilitySet(careerHubHandle, hubSeaDoneId, GFUI_INVISIBLE);
    }

    /* Promotion / champion message (hidden until season ends) - repositioned to 318 */
    {
        float cyanColor[4] = { 0.4f, 1.0f, 1.0f, 1.0f };
        hubPromotionId = GfuiLabelCreateEx(careerHubHandle,
                                             "",
                                             cyanColor,
                                             GFUI_FONT_SMALL_C,
                                             320, 318,
                                             GFUI_ALIGN_HC_VB, 80);
        GfuiVisibilitySet(careerHubHandle, hubPromotionId, GFUI_INVISIBLE);
    }

    /* Buttons */
    hubRaceNextBtnId = GfuiMenuButtonCreate(careerHubHandle,
                         "Race Next Event",
                         "Start the next race in your career season",
                         nullptr, careerRaceNextEvent);

    hubNewSeasonBtnId = GfuiMenuButtonCreate(careerHubHandle,
                          "Start New Season",
                          "Begin the next season of your racing career",
                          nullptr, careerStartNewSeason);
    GfuiVisibilitySet(careerHubHandle, hubNewSeasonBtnId, GFUI_INVISIBLE);

    GfuiMenuButtonCreate(careerHubHandle,
                         "Season Standings",
                         "View current season driver standings",
                         nullptr, careerStandingsCallback);

    GfuiMenuButtonCreate(careerHubHandle,
                         "Career Stats",
                         "View full career statistics and season history",
                         nullptr, careerStatsCallback);

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

    /* Write a fresh career save (Tier 1: Rookie) */
    char savePath[1024];
    buildSavePath(savePath, sizeof(savePath));
    void *h = GfParmReadFile(savePath, GFPARM_RMODE_CREAT | GFPARM_RMODE_PRIVATE);
    if (h) {
        GfParmSetStr(h, CS_SECT_PLAYER, CS_ATTR_NAME,     name);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_SEASON,   nullptr, 1.0f);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_WINS,     nullptr, 0.0f);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_CHAMPS,   nullptr, 0.0f);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_PODIUMS,  nullptr, 0.0f);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_RACES,    nullptr, 0.0f);
        GfParmSetNum(h, CS_SECT_PLAYER, CS_ATTR_TIER,     nullptr, 1.0f);
        GfParmSetStr(h, CS_SECT_PLAYER, CS_ATTR_SEA_DONE,  "no");
        GfParmSetStr(h, CS_SECT_PLAYER, CS_ATTR_PROMOTED,  "no");
        GfParmSetStr(h, CS_SECT_PLAYER, CS_ATTR_SEA_CHAMP, "no");
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

    GfuiLabelCreate(careerNewHandle,
                    "You will start in the Rookie Series.",
                    GFUI_FONT_SMALL_C,
                    320, 302, GFUI_ALIGN_HC_VB, 0);
    GfuiLabelCreate(careerNewHandle,
                    "Win championships to advance: Rookie -> Pro -> Elite",
                    GFUI_FONT_SMALL_C,
                    320, 284, GFUI_ALIGN_HC_VB, 0);

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

/* ==================== Overwrite Confirmation Screen ==================== */

/** Activate the new-career name entry after the player confirms overwrite. */
static void careerConfirmOverwrite(void * /* dummy */)
{
    GfuiScreenActivate(careerNewMenuInit(careerSelHandle));
}

/** Build and activate the overwrite-confirmation screen.
 *  Shown when a save already exists and the player clicks "New Career". */
static void *careerConfirmInit(void *prevMenu)
{
    if (careerConfirmHandle) {
        GfuiScreenRelease(careerConfirmHandle);
    }

    careerConfirmHandle = GfuiScreenCreateEx(nullptr, nullptr, nullptr,
                                              nullptr, nullptr, 1);
    GfuiScreenAddBgImg(careerConfirmHandle, "data/img/splash-main.png");
    GfuiMenuDefaultKeysAdd(careerConfirmHandle);

    GfuiTitleCreate(careerConfirmHandle, "START NEW CAREER?", 0);

    float warnColor[4] = { 1.0f, 0.4f, 0.2f, 1.0f };
    GfuiLabelCreateEx(careerConfirmHandle,
                      "WARNING: This will permanently erase your existing career,",
                      warnColor, GFUI_FONT_MEDIUM_C,
                      320, 380, GFUI_ALIGN_HC_VB, 0);
    GfuiLabelCreateEx(careerConfirmHandle,
                      "including all season history and statistics.",
                      warnColor, GFUI_FONT_MEDIUM_C,
                      320, 358, GFUI_ALIGN_HC_VB, 0);
    GfuiLabelCreate(careerConfirmHandle,
                    "Are you sure you want to start over?",
                    GFUI_FONT_MEDIUM_C,
                    320, 325, GFUI_ALIGN_HC_VB, 0);

    GfuiMenuButtonCreate(careerConfirmHandle,
                         "Yes, Start New Career",
                         "Erase existing career and start fresh",
                         nullptr, careerConfirmOverwrite);

    GfuiMenuBackQuitButtonCreate(careerConfirmHandle,
                                  "Cancel",
                                  "Keep my existing career",
                                  prevMenu, GfuiScreenActivate);

    return careerConfirmHandle;
}

/* ==================== Career Selection Screen ==================== */

/** Switch to the career hub for an existing career. */
static void careerContinue(void * /* dummy */)
{
    GfuiScreenActivate(careerHubInit(careerSelHandle));
}

/** Initialize the career entry/selection screen and return its handle.
 *  If a career save already exists the screen offers both "Continue Career"
 *  (which resumes the existing save) and "New Career" (which first shows a
 *  confirmation warning before erasing the existing save).
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
        /* Warn before overwriting an existing career */
        GfuiMenuButtonCreate(careerSelHandle,
                             "New Career",
                             "Start a brand-new career (existing career will be erased)",
                             careerConfirmInit(careerSelHandle), GfuiScreenActivate);
    } else {
        GfuiMenuButtonCreate(careerSelHandle,
                             "New Career",
                             "Start a brand-new career from scratch",
                             careerNewMenuInit(careerSelHandle), GfuiScreenActivate);
    }

    GfuiMenuBackQuitButtonCreate(careerSelHandle,
                                  "Back",
                                  "Return to the Main Menu",
                                  prevMenu, GfuiScreenActivate);

    return careerSelHandle;
}
