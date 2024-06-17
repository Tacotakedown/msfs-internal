//
// Created by Taco on 6/17/2024.
//

#include "simvars.h"


const char *SimVarsC::getSimVarUnit(SimVarUnits unit) {
    switch (unit) {
        case SimVarUnits::FEET:
            return "feet";
        case SimVarUnits::METERS:
            return "meters";
        case SimVarUnits::DEGREES:
            return "degrees";
        case SimVarUnits::RADIANS:
            return "radians";
        default:
            return "";
    }
}

const char *SimVarsC::getSimVarName(SimVarsEnum simVar) {
    switch (simVar) {
        case SimVarsEnum::PLANE_LATITUDE:
            return "PLANE LATTITUDE";
        case SimVarsEnum::PLANE_LONGITUDE:
            return "PLANE LONGITUDE";
        case SimVarsEnum::PLANE_BANK_DEGREES:
            return "PLANE BANK DEGREES";
        case SimVarsEnum::PLANE_PITCH_DEGREES:
            return "PLANE PITCH DEGREES";
        case SimVarsEnum::PLANE_ALT_ABOVE_GROUND:
            return "PLANE ALT ABOVE GROUND";
        case SimVarsEnum::PLANE_HEADING_DEGREES_TRUE:
            return "PLANE HEADING DEGREES TRUE";
        case SimVarsEnum::ATC_RUNWAY_RELATIVE_POSITION_X:
            return "ATC RUNWAY RELATIVE POSITION X";
        case SimVarsEnum::ATC_RUNWAY_RELATIVE_POSITION_Y:
            return "ATC RUNWAY RELATIVE POSITION Y";
        case SimVarsEnum::ATC_RUNWAY_RELATIVE_POSITION_Z:
            return "ATC RUNWAY RELATIVE POSITION Z";
        case SimVarsEnum::ATC_RUNWAY_HEADING_DEGREES_TRUE:
            return "ATC RUNWAY HEADING DEGREES TRUE";
        default:
            return "";
    }
}

