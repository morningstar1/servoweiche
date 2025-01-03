#include <msp430.h>
#include "data.h"
#include "pwm.h"

void FRAMWrite (enum RegisterMap reg, uint16_t data)
{
    SYSCFG0 &= ~DFWP;
    uint16_t * FRAM_write_ptr = ((uint16_t*)INFO_START) + reg;
    *FRAM_write_ptr = data;
    SYSCFG0 |= DFWP;
}

uint16_t getFRAMValue(enum RegisterMap reg)
{
    if(reg < RM_LAST){
        return *(((uint16_t*)INFO_START) + reg);
    }
    return 0;
}
uint16_t setFRAMValue(enum RegisterMap reg, uint16_t data)
{
    switch (reg)
    {
    case RM_Function:
        if(data < FN_LAST){
            FRAMWrite(reg, data);
            return 1;
        }else{
            return 0;
        }
    case RM_ServoLimitMin1:
    case RM_ServoLimitMin2:
    case RM_ServoLimitMin3:
    case RM_ServoLimitMin4:
    case RM_ServoLimitMax1:
    case RM_ServoLimitMax2:
    case RM_ServoLimitMax3:
    case RM_ServoLimitMax4:
            FRAMWrite(reg, data);
        return 1;
    case RM_ServoSwitch1:
    case RM_ServoSwitch2:
    case RM_ServoSwitch3:
    case RM_ServoSwitch4:
        if(data == 0 || data == 1){
            FRAMWrite(reg, data);
            return 1;
        }else{
            return 0;
        }
    case RM_ServoSpeed:
            FRAMWrite(reg, data);
        return 1;
    }
    return 0;
}
