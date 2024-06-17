//
// Created by Taco on 6/16/2024.
//

#ifndef AUTOPILOTONROIDS_H
#define AUTOPILOTONROIDS_H
#include <vector>
#include "Console/Console.h"

namespace AutopilotOnRoids {
    struct SimVars {
        double planeHeadingDegreesTrue;
        double planeBankDegrees;
        double planePitchDegrees;
        double planeLatitude;
        double planeLongitude;
        double planeAltAboveGround;
    };

    struct SimvarsReadonly {
        double atcRunwayRelativePositionX;
        double atcRunwayRelativePositionY;
        double atcRunwayRelativePositionZ;
        double atcRunwayHeadingDegreesTrue;
    };

    class AutopilotOnRoids {
    public:
        AutopilotOnRoids(Console &console);

        void setReadonlyVars(const SimvarsReadonly &vars) { m_readonlyVars = vars; }
        void setWriteableVars(const SimVars &vars) { m_writeableVars = vars; }

        SimVars getWritableVars();

    private:
        SimvarsReadonly m_readonlyVars;
        SimVars m_writeableVars;
        Console &m_console;
    };
}

#endif // AUTOPILOTONROIDS_H

// ATC RUNWAY TDPOINT RELATIVE POSITION X 	This float represents the player's main gear relative X (transverse) position according to the aiming point of the runway selected by the ATC. 	Meters
// ATC RUNWAY TDPOINT RELATIVE POSITION Y 	This float represents the player's main gear relative Y (height) position according to the aiming point of the runway selected by the ATC. 	Meters
// ATC RUNWAY TDPOINT RELATIVE POSITION Z This float represents the player's main relative Z (longitudinal) position according to the aiming point of the runway selected by the ATC. Meters
