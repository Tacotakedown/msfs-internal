//
// Created by Taco on 6/17/2024.
//

#ifndef SIMVARS_H
#define SIMVARS_H

// ATC RUNWAY RELATIVE POSITION X meters //readonly
// ATC RUNWAY RELATIVE POSITION Y meters //readonly
// ATC RUNWAY RELATIVE POSITION Z meters //readonly
// ATC RUNWAY HEADING DEGREES TRUE degrees //readonly
// PLANE HEADING DEGREES TRUE degrees
// PLANE BANK DEGREES radians
// PLANE PITCH DEGREES radians
// PLANE LATITUDE radians
// PLANE LONGITUDE radians
// PLANE ALT ABOVE GROUND feet


enum SimVarsEnum {
    ATC_RUNWAY_RELATIVE_POSITION_X,
    ATC_RUNWAY_RELATIVE_POSITION_Y,
    ATC_RUNWAY_RELATIVE_POSITION_Z,
    ATC_RUNWAY_HEADING_DEGREES_TRUE,
    PLANE_HEADING_DEGREES_TRUE,
    PLANE_BANK_DEGREES,
    PLANE_PITCH_DEGREES,
    PLANE_LATITUDE,
    PLANE_LONGITUDE,
    PLANE_ALT_ABOVE_GROUND,
};

enum SimVarUnits {
    METERS,
    DEGREES,
    RADIANS,
    FEET // 🤤
};


class SimVarsC {
public:
    const char *getSimVarName(SimVarsEnum simVar);

    const char *getSimVarUnit(SimVarUnits unit);
};

#endif // SIMVARS_H
