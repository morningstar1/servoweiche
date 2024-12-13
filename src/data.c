#include <msp430.h>
#include "data.h"
#include "pwm.h"

#define FRAM_TEST_START 0x1800
void setLed(uint16_t data);

void FRAMWrite (uint16_t * FRAM_write_ptr, uint16_t data)
{
    SYSCFG0 &= ~DFWP;
    *FRAM_write_ptr++ = data;
    SYSCFG0 |= DFWP;
}

enum FramMap {
    FM_SlaveAddr = 0,
    FM_ServoPosition = 1
};

enum RegisterMap {
    SlaveAddr = 1,
    ActivityBitmap = 2,
    MatchSlaveAddr = 3,
    ServoLimits = 4,
    ServoPos = 5,  
    ServoSpeed = 6
};

uint16_t getRead(uint16_t register_)
{
    switch (register_)
    {
    case SlaveAddr:
        return getMySlaveAddr() & 0x00FF;
    case ActivityBitmap:
        return *(((uint16_t*)FRAM_TEST_START) + FM_ServoPosition);
        break;
    case MatchSlaveAddr:
        break;
    case ServoLimits:
        break;
    case ServoPos:
        return pwm_get(0);
    case ServoSpeed:
        return pwm_getSpeed();
    }
    return 0;
}


uint16_t setWrite(uint16_t register_, uint16_t data)
{
    switch (register_)
    {
    case SlaveAddr:
        FRAMWrite(((uint16_t*)FRAM_TEST_START) + FM_SlaveAddr, data);
        return 1;
    case ActivityBitmap:
        setLed(data);
        return 1;
        break;
    case MatchSlaveAddr:
        break;
    case ServoLimits:
        break;
    case ServoPos:
        pwm_set(0, data);
        return 1;
    case ServoSpeed:
        pwm_setSpeed(data);
        return 1;
    }
    return 0;
}
uint8_t getMySlaveAddr()
{
    return (*(((uint16_t*)FRAM_TEST_START) + FM_SlaveAddr) & 0x00FF);
}

void initLed()
{
    P5DIR |= BIT3;
    P1DIR |= BIT3 | BIT4;
    setLed(0);
}
void setLed(uint16_t data)
{
    /*
    1.3
    1.4
    */
    if(data & 0x0001)
        P1OUT |= BIT3;
    else
        P1OUT &= ~BIT3;
    if(data & 0x0002)
        P1OUT |= BIT4;
    else
        P1OUT &= ~BIT4;
}