/***************************************************************************

    file                 : aero.cpp
    created              : Sun Mar 19 00:04:50 CET 2000
    copyright            : (C) 2000-2017 by Eric Espie, Bernhard Wymann
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



#include "sim.h"

void SimAeroConfig(tCar *car)
{
	void *hdle = car->params;
	tdble Cx, FrntArea;

	Cx       = GfParmGetNum(hdle, SECT_AERODYNAMICS, PRM_CX, nullptr, 0.4f);
	FrntArea = GfParmGetNum(hdle, SECT_AERODYNAMICS, PRM_FRNTAREA, nullptr, 2.5f);
	car->aero.Clift[0] = GfParmGetNum(hdle, SECT_AERODYNAMICS, PRM_FCL, nullptr, 0.0f);
	car->aero.Clift[1] = GfParmGetNum(hdle, SECT_AERODYNAMICS, PRM_RCL, nullptr, 0.0f);
	// Base coefficient will be computed with actual air density in Update
	car->aero.baseSCx2 = Cx * FrntArea;
	car->aero.Cd += car->aero.baseSCx2 * car->airDensity;
}


void  SimAeroUpdate(tCar *car, tSituation *s)
{
	tdble hm;
	int i;    
	tCar *otherCar;
	tdble x, y;
	tdble yaw, otherYaw, tmpas, spdang, tmpsdpang, dyaw;
	tdble dragK = 1.0;

	x = car->DynGCg.pos.x;
	y = car->DynGCg.pos.y;
	yaw = car->DynGCg.pos.az;
	
	// Compute effective airspeed by subtracting wind from car velocity
	tdble carVelX = car->DynGC.vel.x;
	tdble carVelY = car->DynGC.vel.y;
	tdble windX = car->localWindX;
	tdble windY = car->localWindY;
	tdble relVelX = carVelX - windX;
	tdble relVelY = carVelY - windY;
	tdble relVelSqr = relVelX * relVelX + relVelY * relVelY;
	car->airSpeed2 = relVelSqr;

    if (relVelSqr > 100.0f) {
		bool spdang_calculated = false;

		for (i = 0; i < s->_ncars; i++) {
			if (i == car->carElt->index) {
				// skip myself
				continue;
			}
			
			otherCar = &(SimCarTable[i]);
			otherYaw = otherCar->DynGCg.pos.az;
			dyaw = yaw - otherYaw;
			NORM_PI_PI(dyaw);

			if ((otherCar->DynGC.vel.x > 10.0f) && (fabs(dyaw) < 0.1396f)) {
					if (!spdang_calculated) {
						spdang = atan2(relVelY, relVelX);
						spdang_calculated = true;
					}
				// BOLT: Defer expensive atan2 calculation
				tmpsdpang = spdang - atan2(y - otherCar->DynGCg.pos.y, x - otherCar->DynGCg.pos.x);
				NORM_PI_PI(tmpsdpang);
				if (fabs(tmpsdpang) > 2.9671f) {	    /* 10 degrees */
					// behind another car
					tmpas = 1.0f - exp(- 2.0f * DIST(x, y, otherCar->DynGCg.pos.x, otherCar->DynGCg.pos.y) /
									  (otherCar->aero.Cd * otherCar->DynGC.vel.x));
					if (tmpas < dragK) {
						dragK = tmpas;
					}
				} else if (fabs(tmpsdpang) < 0.1396f) {	    /* 8 degrees */
					// before another car, lower drag by maximum 15% (this is just another guess)
					// Use effective airspeed for drafting calculations
					tdble effAirSpeed = sqrt(car->DynGC.vel.x * car->DynGC.vel.x + car->DynGC.vel.y * car->DynGC.vel.y);
					tmpas = 1.0f - 0.15f * exp(- 8.0f * DIST(x, y, otherCar->DynGCg.pos.x, otherCar->DynGCg.pos.y) / (car->aero.Cd * effAirSpeed));
					if (tmpas < dragK) {
						dragK = tmpas;
					}
				}
			}
		}
    }

	car->airSpeed2 = relVelSqr;
	tdble v2 = car->airSpeed2;
	
	// simulate ground effect drop off caused by non-frontal airflow (diffusor stops working etc.)
	// Use car's forward velocity for ground effect, not wind-adjusted
	tdble cosa = 1.0f;	
	if (car->speed > 1.0f) {
		cosa = car->DynGC.vel.x/car->speed;
	}
	
	if (cosa < 0.0f) {
		cosa = 0.0f;
	}
			
	// Use variable air density in drag calculation
	car->aero.drag = -SIGN(car->DynGC.vel.x) * car->aero.baseSCx2 * car->airDensity * v2 * (1.0f + (tdble)car->dammage / 10000.0f) * dragK * dragK;

	hm = 1.5f * (car->wheel[0].rideHeight + car->wheel[1].rideHeight + car->wheel[2].rideHeight + car->wheel[3].rideHeight);
	hm = hm*hm;
	hm = hm*hm;
	hm = 2.0f * exp(-3.0f*hm);
	car->aero.lift[0] = - car->aero.Clift[0] * v2 * hm * cosa;
	car->aero.lift[1] = - car->aero.Clift[1] * v2 * hm * cosa;
}

static const char *WingSect[2] = {SECT_FRNTWING, SECT_REARWING};

void SimWingConfig(tCar *car, int index)
{
	void *hdle = car->params;
	tWing *wing = &(car->wing[index]);
	tdble area;

	area              = GfParmGetNum(hdle, WingSect[index], PRM_WINGAREA, nullptr, 0);
	wing->angle       = GfParmGetNum(hdle, WingSect[index], PRM_WINGANGLE, nullptr, 0);
	wing->staticPos.x = GfParmGetNum(hdle, WingSect[index], PRM_XPOS, nullptr, 0);
	wing->staticPos.z = GfParmGetNum(hdle, WingSect[index], PRM_ZPOS, nullptr, 0);
	wing->staticPos.x -= car->statGC.x;
	
	// Base coefficient will be computed with actual air density in Update
	wing->baseKx = -area;
	wing->Kx = wing->baseKx * car->airDensity;
	wing->Kz = 4.0f * wing->Kx;

	if (index == 1) {
		car->aero.Cd -= wing->baseKx * sin(wing->angle) * car->airDensity;
	}
}


void SimWingReConfig(tCar *car, int index)
{
	tCarPitSetupValue* v = &car->carElt->pitcmd.setup.wingangle[index];
	if (SimAdjustPitCarSetupParam(v)) {
		tWing *wing = &(car->wing[index]);
		tdble oldCd = wing->baseKx * sin(wing->angle) * car->airDensity;
		wing->angle = v->value;
		
		// Recompute Kx with current air density
		wing->Kx = wing->baseKx * car->airDensity;
		wing->Kz = 4.0f * wing->Kx;
		
		if (index == 1) {
			car->aero.Cd += oldCd;
			car->aero.Cd -= wing->baseKx * sin(wing->angle) * car->airDensity;
		}
	}
}


void SimWingUpdate(tCar *car, int index, tSituation* s)
{
	tWing  *wing = &(car->wing[index]);
	tdble vt2 = car->airSpeed2;
	// compute angle of attack using wind-adjusted velocity
	tdble vel_xy_magSqr = car->DynGC.vel.x * car->DynGC.vel.x + car->DynGC.vel.y * car->DynGC.vel.y;

	if (vel_xy_magSqr > 0.0f) {
		tdble vel_xy_mag = sqrt(vel_xy_magSqr);
		tdble aoa = atan2(car->DynGC.vel.z, vel_xy_mag);
		aoa += wing->angle;
		// the sinus of the angle of attack
		tdble sinaoa = sin(aoa);
		wing->forces.x = wing->Kx * vt2 * (1.0f + (tdble)car->dammage / 10000.0f) * sinaoa;
		wing->forces.z = wing->Kz * vt2 * sinaoa;
	} else {
		wing->forces.x = wing->forces.z = 0.0f;
	}
}
