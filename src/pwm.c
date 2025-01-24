#include <msp430.h>
#include "pwm.h"
#include "data.h"
#include "gpio.h"

#define pwm_min   ( 7*200)
#define pwm_mitte (15*200)
#define pwm_max   (28*200)

struct ServoData{
    int target;
    int step;
};

struct ServoData servo[SERVO_COUNT];

void pwm_setTargetValue(int nr, int v){
    if(v == getFRAMValue(RM_ServoSwitch1 + nr)){
        servo[nr].target = getFRAMValue(RM_ServoLimitMin1 + nr);
    }else{
        servo[nr].target = getFRAMValue(RM_ServoLimitMax1 + nr);
    }
}

void pwm_toggle(int nr, int v){
    if(nr >= SERVO_COUNT || v > 1)
        return;

    pwm_setTargetValue(nr, v);
}

void pwm_step(){
    for(int nr = 0; nr < SERVO_COUNT; nr++){
        struct ServoData * s = &servo[nr];
        if(s->target < pwm_min)
          s->target = pwm_min;
        if(s->target > pwm_max)
          s->target = pwm_max;

        // 1*2000 = 1 ms
        if(s->target < s->step)
          s->step -= getFRAMValue(RM_ServoStep);
        if(s->target > s->step)
          s->step += getFRAMValue(RM_ServoStep);

        switch (nr)
        {
        case 0: TB0CCR1 = s->step; break;
        case 1: TB0CCR2 = s->step; break;
        case 2: TB1CCR1 = s->step; break;
        case 3: TB1CCR2 = s->step; break;
        }
    }
}

int pwm_init()
{
    for(int nr = 0; nr < SERVO_COUNT; nr++)
        servo[nr].step = servo[nr].target;
    TB0CTL = TBSSEL__SMCLK | TBCLR | ID_3 | MC__UP;
    TB0R = 0;
    TB0CCR0 = 40000;
    TB0CCR1 = servo[0].step;
    TB0CCR2 = servo[1].step;
    TB0CCTL0 = CCIE;
    TB0CCTL1 = OUTMOD_7;
    TB0CCTL2 = OUTMOD_7;
    
    TB1CTL = TBSSEL__SMCLK | TBCLR | ID_3 | MC__UP;
    TB1R = 0;
    TB1CCR0 = 40000;
    TB1CCR1 = servo[2].step;
    TB1CCR2 = servo[3].step;
    TB1CCTL0 = 0;
    TB1CCTL1 = OUTMOD_7;
    TB1CCTL2 = OUTMOD_7;

    //20000 = 10ms
    //2000 = 1ms
    // 200 = 0,1ms
    //1,5 ms = 90°
}

int relaisRequests[2][2];

void requestRelais(int nr, int dir){
    if(nr > 1 || dir > 1)
        return;
    relaisRequests[nr][dir] = 1;
    relaisRequests[nr][dir ? 0 : 1] = 0; // anti direction
}

void checkRelaisRequest(){
    for(int relais=0; relais < 2; relais++){
        for(int direction = 0; direction < 2; direction++){
            if(relaisRequests[relais][direction]){
                setRelais(relais, direction, 1);
            }else{
                setRelais(relais, direction, 0);
            }
            relaisRequests[relais][direction] = 0;
        }
    }
}

static void __attribute__((__interrupt__(TIMER0_B0_VECTOR)))
isr_cc0_T0B0(void)
{
    static int counter = 0;
    uint16_t speed = getFRAMValue(RM_ServoSpeed);
    if(speed == 0 || counter++ % speed == 0){
        pwm_step();
    }
    checkRelaisRequest();
    TB0CCTL0 &= ~CCIFG;
}
