#include <car.h>
#include <tgf.h>

#include "raceengine.h"
#include "racemain.h"
#include "raceinit.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(WIN32) || defined(_WIN32)
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif

#define TLM_MAX_CARS 40
#define TLM_FLUSH_INTERVAL 50

typedef struct {
    FILE *file;
    char path[256];
    int lap;
    int stepsSinceFlush;
    bool active;
} tCarRecorder;

static tCarRecorder recorders[TLM_MAX_CARS];
static bool recording = false;
static double sessionStart = 0.0;

static const char *CSV_HEADER =
    "time,lap,dist,speed_x,speed_y,speed_z,speed,"
    "pos_x,pos_y,pos_z,yaw,yaw_rate,"
    "steer,throttle,brake,clutch,gear,rpm,fuel,damage,"
    "susp0,susp1,susp2,susp3,"
    "fx0,fx1,fx2,fx3,fy0,fy1,fy2,fy3,"
    "temp0,temp1,temp2,temp3,"
    "wear0,wear1,wear2,wear3,"
    "grip_mod";

static void EnsureDir(const char *path)
{
    struct stat st;
    if (stat(path, &st) != 0 || !S_ISDIR(st.st_mode)) {
        MKDIR(path);
    }
}

static void WriteHeader(FILE *f)
{
    fprintf(f, "%s\n", CSV_HEADER);
}

void ReTelemetryStartRecording(tSituation *s)
{
    recording = false;
    sessionStart = s->currentTime;

    const int BUFSIZE = 512;
    char dir[BUFSIZE];

    snprintf(dir, BUFSIZE, "%stelemetry", GetLocalDir());
    EnsureDir(dir);

    for (int i = 0; i < s->_ncars && i < TLM_MAX_CARS; i++) {
        recorders[i].file = nullptr;
        recorders[i].lap = 0;
        recorders[i].stepsSinceFlush = 0;
        recorders[i].active = false;
        recorders[i].path[0] = '\0';

        tCarElt *car = s->cars[i];
        if (car->_driverType != RM_DRV_HUMAN) continue;

        char basename[128];
        snprintf(basename, sizeof(basename), "%s_%s",
            car->_name, ReInfo->track->name);

        char cleanname[128];
        int j = 0;
        for (int k = 0; basename[k] && j < (int)sizeof(cleanname) - 1; k++) {
            if (basename[k] == ' ' || basename[k] == '/' || basename[k] == '\\') {
                cleanname[j++] = '_';
            } else {
                cleanname[j++] = basename[k];
            }
        }
        cleanname[j] = '\0';

        time_t t = time(nullptr);
        struct tm *stm = localtime(&t);
        char timestamp[32];
        snprintf(timestamp, sizeof(timestamp), "%04d%02d%02d_%02d%02d%02d",
            stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday,
            stm->tm_hour, stm->tm_min, stm->tm_sec);

        snprintf(recorders[i].path, sizeof(recorders[i].path),
            "%stelemetry/%s_%s.csv", GetLocalDir(), cleanname, timestamp);

        recorders[i].file = fopen(recorders[i].path, "w");
        if (recorders[i].file) {
            WriteHeader(recorders[i].file);
            recorders[i].active = true;
            recording = true;
        }
    }
}

void ReTelemetryRecordStep(tSituation *s)
{
    if (!recording) return;

    for (int i = 0; i < s->_ncars && i < TLM_MAX_CARS; i++) {
        tCarRecorder *rec = &recorders[i];
        if (!rec->active || !rec->file) continue;

        tCarElt *car = s->cars[i];

        tdble gm = 0.0f;
        if (car->_trkPos.seg && car->_trkPos.seg->dynSurface) {
            gm = car->_trkPos.seg->dynSurface->gripMod;
        }

        fprintf(rec->file,
            "%.6f,%d,%.3f,%.3f,%.3f,%.3f,%.3f,"
            "%.3f,%.3f,%.3f,%.6f,%.6f,"
            "%.4f,%.4f,%.4f,%.4f,%d,%.1f,%.3f,%d,"
            "%.5f,%.5f,%.5f,%.5f,"
            "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,"
            "%.1f,%.1f,%.1f,%.1f,"
            "%.4f,%.4f,%.4f,%.4f,"
            "%.5f\n",
            s->currentTime - sessionStart,
            car->_laps - 1,
            car->_distFromStartLine,
            car->_speed_x, car->_speed_y, car->_speed_z,
            car->pub.speed,
            car->_pos_X, car->_pos_Y, car->_pos_Z, car->_yaw,
            car->_yaw_rate,
            car->_steerCmd, car->_accelCmd, car->_brakeCmd, car->_clutchCmd,
            car->_gear, car->_enginerpm, car->_fuel, car->_dammage,
            car->priv.wheel[0].suspDeflection, car->priv.wheel[1].suspDeflection,
            car->priv.wheel[2].suspDeflection, car->priv.wheel[3].suspDeflection,
            car->priv.wheel[0].Fx, car->priv.wheel[1].Fx,
            car->priv.wheel[2].Fx, car->priv.wheel[3].Fx,
            car->priv.wheel[0].Fy, car->priv.wheel[1].Fy,
            car->priv.wheel[2].Fy, car->priv.wheel[3].Fy,
            car->priv.wheel[0].currentTemperature, car->priv.wheel[1].currentTemperature,
            car->priv.wheel[2].currentTemperature, car->priv.wheel[3].currentTemperature,
            car->priv.wheel[0].currentWear, car->priv.wheel[1].currentWear,
            car->priv.wheel[2].currentWear, car->priv.wheel[3].currentWear,
            gm
        );

        rec->stepsSinceFlush++;
        if (rec->stepsSinceFlush >= TLM_FLUSH_INTERVAL) {
            fflush(rec->file);
            rec->stepsSinceFlush = 0;
        }
    }
}

void ReTelemetryStopRecording(void)
{
    if (!recording) return;

    for (int i = 0; i < TLM_MAX_CARS; i++) {
        if (recorders[i].active && recorders[i].file) {
            fflush(recorders[i].file);
            fclose(recorders[i].file);
            recorders[i].file = nullptr;
            recorders[i].active = false;
        }
    }

    recording = false;
}
