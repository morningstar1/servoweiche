#include <msp430.h>
#include "data.h"
#include "gpio.h"
#include "pwm.h"

void updateLEDs()
{   
    uint8_t leds = 0;
    switch (getFRAMValue(RM_Function))
    {
    case FN_Weiche4x:
        for(int nr = 0; nr < 4; nr++){
            leds |= (getFRAMValue(RM_ServoFunction1 + nr) ? 2 : 1) << (nr * 2);
        }
        break;
    case FN_Kreuzweiche2x:
        for(int nr = 0; nr < 2; nr++){
            leds |= (getFRAMValue(RM_ServoFunction1 + nr) ? 2 : 1) << (nr * 2);
        }
        break;
    case FN_Weiche2x_Kreuzweiche:
        for(int nr = 0; nr < 2; nr++){
            leds |= (getFRAMValue(RM_ServoFunction1 + nr) ? 2 : 1) << (nr * 2);
        }
        leds |= (getFRAMValue(RM_ServoFunction3) ? 2 : 1) << (6);
        break;
    case FN_Kreuzweiche_Weiche2x:
        leds |= (getFRAMValue(RM_ServoFunction1) ? 2 : 1) << (2);
        for(int nr = 0; nr < 2; nr++){
            leds |= (getFRAMValue(RM_ServoFunction2 + nr) ? 2 : 1) << ((nr + 2) * 2);
        }
        break;
    case FN_Doppelkreuzweiche:
        leds |= getFRAMValue(RM_ServoFunction1) ? 2 : 1;
        break;
    case FN_Unset:
        leds |= getFRAMValue(RM_ServoFunction1);
        break;
    }
    setLEDs(leds);
}

void Weiche4x_in(uint8_t input)
{
    for(int nr = 0; nr < 4; nr++){
        uint8_t switchstate = (input & (0x3 << (nr * 2))) >> (nr * 2);
        if(switchstate == BIT0){ // left
            setFRAMValue(RM_ServoFunction1 + nr, 0);
        }else if(switchstate == BIT1){ // right
            setFRAMValue(RM_ServoFunction1 + nr, 1);
        }
    }
}

void Weiche4x_out(){
    for(int nr = 0; nr < 4; nr++){
        uint16_t v = getFRAMValue(RM_ServoFunction1 + nr);
        pwm_toggle(nr, v);
        requestRelais(nr, v);
    }
}

void Kreuzweiche2x_in(uint8_t input)
{
    for(int nr = 0; nr < 2; nr++){
        uint8_t switchstate = (input & (0x3 << (nr * 2))) >> (nr * 2);
        if(switchstate == BIT0){ // beide gerade
            setFRAMValue(RM_ServoFunction1 + nr, 0);
        }else if(switchstate == BIT1){ // beide gekreuzt
            setFRAMValue(RM_ServoFunction1 + nr, 1);
        }
    }
}

void Kreuzweiche2x_out(){
    for(int nr = 0; nr < 2; nr++){
        uint16_t v = getFRAMValue(RM_ServoFunction1 + nr);
        pwm_toggle(nr * 2,     v);
        pwm_toggle(nr * 2 + 1, v);
        requestRelais(nr, v);
    }
}

void Weiche2x_Kreuzweiche_in(uint8_t input){
    for(int nr = 0; nr < 2; nr++){
        uint8_t switchstate = (input & (0x3 << (nr * 2))) >> (nr * 2);
        if(switchstate == BIT0){ // left
            setFRAMValue(RM_ServoFunction1 + nr, 0);
        }else if(switchstate == BIT1){ // right
            setFRAMValue(RM_ServoFunction1 + nr, 1);
        }
    }
    uint8_t switchstate = (input & (0x3 << (6))) >> (6);
    if(switchstate == BIT0){ // beide gerade
        setFRAMValue(RM_ServoFunction3, 0);
    }else if(switchstate == BIT1){ // beide gekreuzt
        setFRAMValue(RM_ServoFunction3, 1);
    }
}

void Weiche2x_Kreuzweiche_out(){
    pwm_toggle(0, getFRAMValue(RM_ServoFunction1));
    pwm_toggle(1, getFRAMValue(RM_ServoFunction2));

    uint16_t v = getFRAMValue(RM_ServoFunction3);
    pwm_toggle(2, v);
    pwm_toggle(3, v);
    requestRelais(1, v);
}

void Kreuzweiche_Weiche2x_in(uint8_t input){
    uint8_t switchstate = (input & (0x3 << (2))) >> (2);
    if(switchstate == BIT0){ // beide gerade
        setFRAMValue(RM_ServoFunction1, 0);
    }else if(switchstate == BIT1){ // beide gekreuzt
        setFRAMValue(RM_ServoFunction1, 1);
    }

    for(int nr = 0; nr < 2; nr++){
        uint8_t switchstate = (input & (0x3 << ((nr + 2) * 2))) >> ((nr + 2) * 2);
        if(switchstate == BIT0){ // left
            setFRAMValue(RM_ServoFunction2 + nr, 0);
        }else if(switchstate == BIT1){ // right
            setFRAMValue(RM_ServoFunction2 + nr, 1);
        }
    }
}

void Kreuzweiche_Weiche2x_out(){
    uint16_t v1 = getFRAMValue(RM_ServoFunction1);
    uint16_t v2 = getFRAMValue(RM_ServoFunction2);
    uint16_t v3 = getFRAMValue(RM_ServoFunction3);
    pwm_toggle(0, v1);
    pwm_toggle(1, v1);
    requestRelais(0, v1);
    requestRelais(1, v2);

    pwm_toggle(2, v2);
    pwm_toggle(3, v3);
}

void Doppelkreuzweiche_in(uint8_t input)
{
    if(input == BIT0){ // Alles Gerade
        setFRAMValue(RM_ServoFunction1, 0);
    }else if(input == BIT1){ // Kreuz 1
        setFRAMValue(RM_ServoFunction1, 1);
    }else if(input == BIT2){ // Kreuz 2
        setFRAMValue(RM_ServoFunction1, 2);
    }
}

void Doppelkreuzweiche_out()
{
    switch(getFRAMValue(RM_ServoFunction1)){
    case 0:
        pwm_toggle(0, 0);
        pwm_toggle(1, 0);
        pwm_toggle(2, 0);
        pwm_toggle(3, 0);
        requestRelais(0, 0);
        requestRelais(1, 0);
        break;
    case 1:
        pwm_toggle(0, 1);
        pwm_toggle(1, 0);
        pwm_toggle(2, 0);
        pwm_toggle(3, 1);
        requestRelais(0, 1);
        requestRelais(1, 0);
        break;
    case 2:
        pwm_toggle(0, 0);
        pwm_toggle(1, 1);
        pwm_toggle(2, 1);
        pwm_toggle(3, 0);
        requestRelais(0, 0);
        requestRelais(1, 1);
        break;
    }
}

void checkInputs()
{
    switch (getFRAMValue(RM_Function))
    {
    case FN_Weiche4x:
        Weiche4x_in(getSwitchInput());
        Weiche4x_out();
        break;
    case FN_Kreuzweiche2x:
        Kreuzweiche2x_in(getSwitchInput());
        Kreuzweiche2x_out();
        break;
    case FN_Weiche2x_Kreuzweiche:
        Weiche2x_Kreuzweiche_in(getSwitchInput());
        Weiche2x_Kreuzweiche_out();
        break;
    case FN_Kreuzweiche_Weiche2x:
        Kreuzweiche_Weiche2x_in(getSwitchInput());
        Kreuzweiche_Weiche2x_out();
        break;
    case FN_Doppelkreuzweiche:
        Doppelkreuzweiche_in(getSwitchInput());
        Doppelkreuzweiche_out();
        break;
    case FN_Unset:
        setFRAMValue(RM_ServoFunction1, getSwitchInput());
        break;
    }
}

void setupInitalValues()
{
    switch (getFRAMValue(RM_Function))
    {
    case FN_Weiche4x:
        Weiche4x_out();
        break;
    case FN_Kreuzweiche2x:
        Kreuzweiche2x_out();
        break;
    case FN_Weiche2x_Kreuzweiche:
        Weiche2x_Kreuzweiche_out();
        break;
    case FN_Kreuzweiche_Weiche2x:
        Kreuzweiche_Weiche2x_out();
        break;
    case FN_Doppelkreuzweiche:
        Doppelkreuzweiche_out();
        break;
    default:
        pwm_toggle(0, 0);
        pwm_toggle(1, 1);
        pwm_toggle(2, 0);
        pwm_toggle(3, 1);
        break;
    }
}
