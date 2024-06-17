//
// Created by Taco on 6/16/2024.
//

#include "AutopilotOnRoids.h"


namespace AutopilotOnRoids {
    SimVars AutopilotOnRoids::getWritableVars() {
        SimVars vars = this->m_writeableVars;
        SimVars writingVars = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

        return writingVars;
    }
}
