/***************************************************************************

    file                 : simuitf.h
    created              : 2026
    copyright            : (C) 2000-2017 Eric Espie, Bernhard Wymann
                           (C) 2026 Open Racing Simulator Contributors

 ***************************************************************************/

/** @file
    Simulation module callback interface (extracted from simu.h).
    Included by raceman.h so race-engine headers do not pull the full simu stack.
    @ingroup simumodint
*/

#ifndef _SIMUITF_H_
#define _SIMUITF_H_

#include <track.h>
#include <car.h>

#define SIM_IDENT	0

struct Situation;
struct RmInfo;

typedef void (*tfSimInit)(int nbCars, tTrack* track, tdble fuelFactor, tdble damageFactor, tdble tireFactor);
typedef void (*tfSimConfig)(tCarElt* carElt, struct RmInfo* reInfo);
typedef void (*tfSimReConfig)(tCarElt* carElt);
typedef void (*tfSimUpdate)(struct Situation* s, double deltaTime, int telemetry);
typedef void (*tfSimShutdown)(void);

typedef struct
{
	tfSimInit		init;
	tfSimConfig		config;
	tfSimReConfig	reconfig;
	tfSimUpdate		update;
	tfSimShutdown	shutdown;
} tSimItf;

#endif /* _SIMUITF_H_ */
