/***************************************************************************

    file        : replayrecorder.cpp
    created     : 2026
    copyright   : (C) 2026 Open Racing Simulator Contributors

 ***************************************************************************/

/** @file
    Minimal ORSR replay recorder (v0.1 spike). See doc/planning/REPLAY_FORMAT.md.
    Enable with environment variable TORCS_REPLAY_RECORD=1.
*/

#include <car.h>
#include <tgf.h>
#include <raceman.h>

#include "raceengine.h"
#include "raceinit.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sys/stat.h>

#if defined(WIN32) || defined(_WIN32)
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#define ORSR_MAGIC 0x4F525352u /* "ORSR" */
#define ORSR_VERSION_MAJOR 0u
#define ORSR_VERSION_MINOR 1u
#define ORSR_HEADER_SIZE 64u
#define ORSR_MAX_CARS 40
#define ORSR_DT_UNITS_PER_SEC 10000.0 /* dt field is 0.1 ms units */

static FILE *replayFile = nullptr;
static int replayCarCount = 0;
static uint32_t replayFrameCount = 0;
static bool replayActive = false;

static void EnsureReplayDir(void)
{
    char dir[512];
    snprintf(dir, sizeof(dir), "%sreplay", GetLocalDir());
    struct stat st;
    if (stat(dir, &st) != 0 || !S_ISDIR(st.st_mode)) {
        MKDIR(dir);
    }
}

static int8_t PackSteer(tdble steer)
{
    if (steer > 1.0) {
        steer = 1.0;
    } else if (steer < -1.0) {
        steer = -1.0;
    }
    return static_cast<int8_t>(steer * 127.0);
}

static void PackCarInput(const tCarElt *car, unsigned char out[3])
{
    unsigned char b0 = 0;
    if (car->_accelCmd > 0.05) {
        b0 |= 0x01;
    }
    if (car->_brakeCmd > 0.05) {
        b0 |= 0x02;
    }
    if (car->_clutchCmd > 0.05) {
        b0 |= 0x04;
    }
    out[0] = b0;
    out[1] = static_cast<unsigned char>(PackSteer(car->_steerCmd));
    out[2] = 0;
}

static void WriteHeaderPlaceholder(void)
{
    unsigned char header[ORSR_HEADER_SIZE];
    memset(header, 0, sizeof(header));

    header[0] = static_cast<unsigned char>((ORSR_MAGIC >> 24) & 0xff);
    header[1] = static_cast<unsigned char>((ORSR_MAGIC >> 16) & 0xff);
    header[2] = static_cast<unsigned char>((ORSR_MAGIC >> 8) & 0xff);
    header[3] = static_cast<unsigned char>(ORSR_MAGIC & 0xff);
    header[4] = static_cast<unsigned char>(ORSR_VERSION_MAJOR);
    header[5] = static_cast<unsigned char>(ORSR_VERSION_MINOR);
    /* build fingerprint at 8: leave zero for spike */
    const uint32_t tickHz = static_cast<uint32_t>(1.0 / RCM_MAX_DT_SIMU + 0.5);
    header[12] = static_cast<unsigned char>(tickHz & 0xff);
    header[13] = static_cast<unsigned char>((tickHz >> 8) & 0xff);
    header[14] = static_cast<unsigned char>((tickHz >> 16) & 0xff);
    header[15] = static_cast<unsigned char>((tickHz >> 24) & 0xff);
    header[16] = static_cast<unsigned char>(replayCarCount & 0xff);
    header[17] = static_cast<unsigned char>((replayCarCount >> 8) & 0xff);
    header[18] = static_cast<unsigned char>((replayCarCount >> 16) & 0xff);
    header[19] = static_cast<unsigned char>((replayCarCount >> 24) & 0xff);
    /* frame count at 20, filled on close */

    fwrite(header, 1, sizeof(header), replayFile);
}

static bool ReplayRecordingRequested(void)
{
    const char *env = getenv("TORCS_REPLAY_RECORD");
    return env != nullptr && env[0] != '\0' && strcmp(env, "0") != 0;
}

void ReReplayStartRecording(tSituation *s)
{
    replayActive = false;
    replayFrameCount = 0;
    replayCarCount = 0;

    if (!ReplayRecordingRequested() || s == nullptr) {
        return;
    }

    EnsureReplayDir();

    time_t t = time(nullptr);
    struct tm *stm = localtime(&t);
    char path[512];
    snprintf(path, sizeof(path),
        "%sreplay/replay-%s-%04d%02d%02d-%02d%02d%02d.orsr",
        GetLocalDir(),
        ReInfo->track != nullptr ? ReInfo->track->name : "track",
        stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday,
        stm->tm_hour, stm->tm_min, stm->tm_sec);

    replayFile = fopen(path, "wb");
    if (replayFile == nullptr) {
        GfError("ReplayRecorder: could not open %s\n", path);
        return;
    }

    replayCarCount = s->_ncars;
    if (replayCarCount > ORSR_MAX_CARS) {
        replayCarCount = ORSR_MAX_CARS;
    }

    WriteHeaderPlaceholder();
    replayActive = true;
    GfOut("ReplayRecorder: writing %s (%d cars)\n", path, replayCarCount);
}

void ReReplayRecordInputs(tSituation *s, double deltaTime)
{
    if (!replayActive || replayFile == nullptr || s == nullptr) {
        return;
    }
    if (s->currentTime < 0.0) {
        return;
    }

    const uint16_t dtUnits = static_cast<uint16_t>(deltaTime * ORSR_DT_UNITS_PER_SEC + 0.5);
    fwrite(&dtUnits, sizeof(dtUnits), 1, replayFile);

    unsigned char input[3];
    for (int i = 0; i < replayCarCount; i++) {
        PackCarInput(s->cars[i], input);
        fwrite(input, 1, 3, replayFile);
    }

    replayFrameCount++;
}

void ReReplayStopRecording(void)
{
    if (!replayActive || replayFile == nullptr) {
        return;
    }

    if (fseek(replayFile, 20, SEEK_SET) == 0) {
        fwrite(&replayFrameCount, sizeof(replayFrameCount), 1, replayFile);
    }

    fclose(replayFile);
    replayFile = nullptr;
    replayActive = false;
    GfOut("ReplayRecorder: closed (%u frames)\n", replayFrameCount);
}
