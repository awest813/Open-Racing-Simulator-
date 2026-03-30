/***************************************************************************

    file                 : carstruct.h
    created              : Sun Mar 19 00:06:07 CET 2000
    copyright            : (C) 2000-2013 by Eric Espie, Bernhard Wymann
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

#ifndef _CAR__H_
#define _CAR__H_

#include <plib/sg.h>
#include <SOLID/solid.h>

#include "aero.h"
#include "susp.h"
#include "brake.h"
#include "axle.h"
#include "steer.h"
#include "wheel.h"
#include "transmission.h"
#include "engine.h"

typedef struct
{
    /* driver's interface */
    tCarCtrl	*ctrl;
    void	*params;
    tCarElt	*carElt;

    tCarCtrl	preCtrl;

    /* components */
    tAxle		axle[2];
    tWheel		wheel[4];
    tSteer		steer;
    tBrakeSyst		brkSyst;
    tAero		aero;
    tWing		wing[2];
    tTransmission	transmission;	/* includes clutch, gearbox and driveshaft */
    tEngine		engine;

    /* static */
    t3Dd	dimension;	/* car's mesures */
    tdble	mass;		/* mass with pilot (without fuel) */
    tdble	Minv;		/* 1 / mass with pilot (without fuel) */
    tdble	tank;		/* fuel tank capa */
    t3Dd	statGC;		/* static pos of GC */
    t3Dd	Iinv;		/* inverse of inertial moment along the car's 3 axis */

    /* dynamic */
    tdble	fuel;		/* current fuel load */
    tDynPt	DynGC;		/* GC local data except position */
    tDynPt	DynGCg;		/* GC global data */
    tPosd	VelColl;	/* resulting velocity after collision */
    tDynPt	preDynGC;	/* previous one */
    tTrkLocPos	trkPos;		/* current track position */
    tdble	airSpeed2;	/* current air speed (squared) for aerodynamic forces */

    /* internals */
    tdble	Cosz;
    tdble	Sinz;
    tDynPt	corner[4];	/* x,y,z for static relative pos, ax,ay,az for dyn. world coord */
    int		collision;
    t3Dd	normal;
    t3Dd	collpos;
    tdble	wheelbase;
    tdble	wheeltrack;
    sgMat4	posMat;
    DtShapeRef	shape;		/* for collision */
    int		blocked;		// Flag to show if the car has had already a collision in the same timestep.
    int		dammage;
    
    tDynPt	restPos;	/* target rest position after the car is broken */

    int		collisionAware;
	tdble	speed;		// total speed = sqrt(vx*vx + vy*vy + vz*vz)
	tRmInfo	*ReInfo;	// race manager context for environment and rules

	tdble	localTemperature;	// Environment temperature for the car
	tdble	localPressure;		// Environment pressure for the car
	tdble	localWindX;		// Wind velocity component in car-local X (forward) axis
	tdble	localWindY;		// Wind velocity component in car-local Y (left) axis
	tdble	airDensity;		// Air density computed from temperature and pressure
} tCar;

#if 0

#define CHECK_VAR(_var_, _msg_) do {						\
    if (isnan(_var_) || isinf(_var_)) {						\
	GfOut("%s = %f  in %s line %d\n", _msg_, _var_, __FILE__, __LINE__);	\
        assert (0);								\
	exit(0);								\
    }										\
} while (0)

#define DUMP_CAR(_car_) do {					\
	GfOut("DynGC.acc.x  = %f\n", (_car_)->DynGC.acc.x);	\
	GfOut("DynGC.acc.y  = %f\n", (_car_)->DynGC.acc.y);	\
	GfOut("DynGC.acc.y  = %f\n", (_car_)->DynGC.acc.y);	\
	GfOut("DynGC.vel.x  = %f\n", (_car_)->DynGC.vel.x);	\
	GfOut("DynGC.vel.y  = %f\n", (_car_)->DynGC.vel.y);	\
	GfOut("DynGCg.pos.x = %f\n", (_car_)->DynGCg.pos.x);	\
	GfOut("DynGCg.pos.y = %f\n", (_car_)->DynGCg.pos.y);	\
	GfOut("DynGCg.acc.x = %f\n", (_car_)->DynGCg.acc.x);	\
	GfOut("DynGCg.acc.y = %f\n", (_car_)->DynGCg.acc.y);	\
	GfOut("DynGCg.vel.x = %f\n", (_car_)->DynGCg.vel.x);	\
	GfOut("DynGCg.vel.y = %f\n", (_car_)->DynGCg.vel.y);	\
	GfOut("DynGCg.pos.x = %f\n", (_car_)->DynGCg.pos.x);	\
	GfOut("DynGCg.pos.y = %f\n", (_car_)->DynGCg.pos.y);	\
	GfOut("aero.drag    = %f\n", (_car_)->aero.drag);	\
} while (0)


#define CHECK(_car_) do {									\
    if (isnan((_car_)->DynGC.acc.x) || isinf((_car_)->DynGC.acc.x) ||				\
	isnan((_car_)->DynGC.acc.y) || isinf((_car_)->DynGC.acc.y) ||				\
	isnan((_car_)->DynGC.vel.x) || isinf((_car_)->DynGC.vel.x) ||				\
	isnan((_car_)->DynGC.vel.y) || isinf((_car_)->DynGC.vel.y) ||				\
	isnan((_car_)->DynGC.acc.x) || isinf((_car_)->DynGC.acc.x) ||				\
	isnan((_car_)->DynGCg.acc.y) || isinf((_car_)->DynGCg.acc.y) ||				\
	isnan((_car_)->DynGCg.vel.x) || isinf((_car_)->DynGCg.vel.x) ||				\
	isnan((_car_)->DynGCg.vel.y) || isinf((_car_)->DynGCg.vel.y) ||				\
	isnan((_car_)->DynGCg.pos.x) || isinf((_car_)->DynGCg.pos.x) ||				\
	isnan((_car_)->DynGCg.pos.y) || isinf((_car_)->DynGCg.pos.y) ||				\
	isnan((_car_)->aero.drag) || isinf((_car_)->aero.drag)) {				\
	GfOut("Problem for %s in %s line %d\n", (_car_)->carElt->_name, __FILE__, __LINE__);	\
	DUMP_CAR(_car_);									\
	assert (0);										\
	/* GfScrShutdown(); */									\
	exit(0);										\
    }												\
} while (0)

#else

#define CHECK_VAR(_var_, _msg_)
#define CHECK(_car_)

#endif

#endif /* _CAR__H_ */ 








