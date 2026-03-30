/***************************************************************************
    file                 : atmosphere.cpp
    created              : Sun Mar 29 2026
    copyright            : (C) 2026 by Open Racing Simulator Team
    email                : info@opencarrsim.org
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
#include <cmath>

void SimAtmosphereUpdate(tCar *car, tSituation *s)
{
	tdble ambientTempC = 20.0f;
	tdble windSpeed = 0.0f;
	tdble windDir = 0.0f;

	if (car->ReInfo) {
		ambientTempC = car->ReInfo->environment.ambientTemperature;
		windSpeed = car->ReInfo->environment.windSpeed;
		windDir = car->ReInfo->environment.windDirection;
	}

	car->localTemperature = 273.15f + ambientTempC;
	car->localPressure = 101300.0f;

	// Air density using ideal gas law: rho = P / (R_specific * T)
	// R_specific for dry air = 287.058 J/(kg*K)
	car->airDensity = car->localPressure / (287.058f * car->localTemperature);

	// Compute wind vector in global frame, then rotate to car-local frame
	tdble cosYaw = cos(car->DynGCg.pos.az);
	tdble sinYaw = sin(car->DynGCg.pos.az);
	tdble cosWd = cos(windDir);
	tdble sinWd = sin(windDir);

	tdble windGlobalX = windSpeed * cosWd;
	tdble windGlobalY = windSpeed * sinWd;

	// Rotate global wind to car-local frame (inverse rotation by yaw)
	car->localWindX = cosYaw * windGlobalX + sinYaw * windGlobalY;
	car->localWindY = -sinYaw * windGlobalX + cosYaw * windGlobalY;
}
