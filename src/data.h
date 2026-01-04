#pragma once
#include <stdint.h>

enum Function {
    FN_Unset = 0,
    FN_Weiche4x = 1,
    FN_Kreuzweiche2x = 2,
    FN_Weiche2x_Kreuzweiche = 3,
    FN_Doppelkreuzweiche = 4,
    FN_Kreuzweiche_Weiche2x = 5,
    FN_LAST
};

enum RegisterMap {
    RM_Function = 0,
    RM_ServoFunction1,
    RM_ServoFunction2,
    RM_ServoFunction3,
    RM_ServoFunction4,
    RM_ServoLimitMin1,
    RM_ServoLimitMin2,
    RM_ServoLimitMin3,
    RM_ServoLimitMin4,
    RM_ServoLimitMax1,
    RM_ServoLimitMax2,
    RM_ServoLimitMax3,
    RM_ServoLimitMax4,
    RM_ServoSwitch1,
    RM_ServoSwitch2,
    RM_ServoSwitch3,
    RM_ServoSwitch4,
    RM_ServoSpeed,
    RM_ServoStep,
    RM_LAST
};

extern uint16_t getFRAMValue(enum RegisterMap reg);
extern uint16_t setFRAMValue(enum RegisterMap reg, uint16_t value);
