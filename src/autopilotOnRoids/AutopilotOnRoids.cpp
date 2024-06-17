//
// Created by Taco on 6/16/2024.
//

#include "AutopilotOnRoids.h"
#include <sstream>
#include <cmath>

# define M_PI           3.14159265358979323846

namespace AutopilotOnRoids {
    AutopilotOnRoids::AutopilotOnRoids(Console &console) : m_console(console) {
    }

    SimVars AutopilotOnRoids::getWritableVars() {
        SimVars vars = this->m_writeableVars;

        const double EARTH_RADIUS = 6371000;

        double xRad = this->m_readonlyVars.atcRunwayRelativePositionX / EARTH_RADIUS;
        double yRad = this->m_readonlyVars.atcRunwayRelativePositionY / EARTH_RADIUS;


        double runwayAimPointLat = this->m_writeableVars.planeLatitude += atan2(yRad, cos(xRad)) * 180 / M_PI;
        double runwayAimPointLon = this->m_writeableVars.planeLongitude += atan2(xRad, 1) * 180 / M_PI;
        static double crazyAltitude = 10000.0; // this will allow the plane to avoid any obstacles


        double planeHeading = this->m_readonlyVars.atcRunwayHeadingDegreesTrue;

        const SimVars writingVars = {planeHeading, 0.0, 0.0, runwayAimPointLat, runwayAimPointLon, crazyAltitude};

        return writingVars;
    }
}
