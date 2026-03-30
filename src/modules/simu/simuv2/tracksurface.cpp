#include "sim.h"

static tTrack *theTrack = nullptr;
static tdble lastUpdateTime = 0.0f;

#define TRACK_SURFACE_UPDATE_INTERVAL 0.5f
#define RUBBER_DEPOSIT_RATE 0.0004f
#define RUBBER_MAX 1.0f
#define RUBBER_GRIP_FACTOR 0.12f
#define GRIP_WARMUP_TIME 300.0f
#define AMBIENT_TEMP_K 293.15f

void SimTrackSurfaceInit(tTrack *track)
{
    theTrack = track;
    lastUpdateTime = 0.0f;

    if (!theTrack) return;

    tTrackSeg *seg = theTrack->seg;
    for (int i = 0; i < theTrack->nseg; i++) {
        seg->dynSurface = (tSegDynamicSurface *)calloc(1, sizeof(tSegDynamicSurface));
        seg->dynSurface->rubber = 0.0f;
        seg->dynSurface->gripMod = 0.0f;
        seg->dynSurface->temperature = AMBIENT_TEMP_K;
        seg->dynSurface->lastUpdate = 0.0f;
        seg = seg->next;
    }
}

void SimTrackSurfaceUpdate(tCar *car, tSituation *s)
{
    if (!theTrack || !car->trkPos.seg || !car->trkPos.seg->dynSurface) return;

    tTrackSeg *seg = car->trkPos.seg;
    tSegDynamicSurface *ds = seg->dynSurface;

    const char *material = (seg->surface) ? seg->surface->material : nullptr;
    if (material && strcmp(material, "asphalt") != 0 &&
        strncmp(material, "asphalt-", 8) != 0) {
        return;
    }

    tdble speed = fabs(car->DynGC.vel.x);
    tdble slip = 0.0f;
    for (int i = 0; i < 4; i++) {
        slip += car->wheel[i].sa * car->wheel[i].sa;
        slip += car->wheel[i].sx * car->wheel[i].sx;
    }
    slip = sqrt(slip / 4.0f) * 0.5f;

    tdble deposit = RUBBER_DEPOSIT_RATE * (speed / 50.0f) * SimDeltaTime * (1.0f + slip * 10.0f);
    if (deposit < 0.0f) deposit = 0.0f;
    ds->rubber += deposit;
    if (ds->rubber > RUBBER_MAX) ds->rubber = RUBBER_MAX;

    tdble progress = MIN(s->currentTime / GRIP_WARMUP_TIME, 1.0f);
    ds->gripMod = ds->rubber * RUBBER_GRIP_FACTOR * progress;

    if (s->currentTime - ds->lastUpdate > TRACK_SURFACE_UPDATE_INTERVAL) {
        tdble amb = car->localTemperature;
        tdble heat = speed * 0.002f;
        ds->temperature += (amb + heat - ds->temperature) * 0.1f;
        ds->lastUpdate = s->currentTime;
    }
}

void SimTrackSurfaceShutdown(void)
{
    if (!theTrack) return;

    tTrackSeg *seg = theTrack->seg;
    for (int i = 0; i < theTrack->nseg; i++) {
        if (seg->dynSurface) {
            free(seg->dynSurface);
            seg->dynSurface = nullptr;
        }
        seg = seg->next;
    }

    theTrack = nullptr;
}

tdble SimTrackSurfaceGripMod(tTrackSeg *seg)
{
    if (seg && seg->dynSurface) {
        return seg->dynSurface->gripMod;
    }
    return 0.0f;
}
