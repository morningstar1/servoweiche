//#include <msp430g2553.h>
#include <msp430.h>
#include "pwm.h"

#define SERVO1_PWM_BIT BIT7
#define SERVO2_PWM_BIT BIT6

#define SERVO_COUNT 2

#define pwm_min   ( 7*200)
#define pwm_mitte (15*200)
#define pwm_max   (28*200)

int speed = 3;

struct ServoData{
  int target;
  int v_;
  int step;
  int servoPowerOnCounter;
};
struct ServoData servo[SERVO_COUNT] = {{pwm_mitte,0,pwm_min,0},{pwm_mitte,0,pwm_min,0}};

void pwm_powerServer(int nr, int powerOn)
{
  if(nr == 0 && powerOn){
    P1DIR  |= (SERVO1_PWM_BIT);
    P1SEL0 |= (SERVO1_PWM_BIT);
  }
  if(nr == 1 && powerOn){
    P1DIR  |= (SERVO2_PWM_BIT);
    P1SEL0 |= (SERVO2_PWM_BIT);
  }
  if(nr == 0 && !powerOn){
    P1DIR  &= ~(SERVO1_PWM_BIT);
    P1SEL0 &= ~(SERVO1_PWM_BIT);
  }
  if(nr == 1 && !powerOn){
    P1DIR  &= ~(SERVO2_PWM_BIT);
    P1SEL0 &= ~(SERVO2_PWM_BIT);
  }
}

int pwm_init() {
  //turn off servos
  for(int i=0;i<SERVO_COUNT;i++)
    pwm_powerServer(i, 0);

  TA0CTL = TASSEL_2 | TACLR | ID_3 | MC_1 /*| TAIE*/;
  TA0R = 0;
  TA0CCR0 = 40000;
  TA0CCR1 = pwm_min;
  TA0CCR2 = pwm_min;
  //20000 = 10ms
  //2000 = 1ms
  // 200 = 0,1ms
  TA0CCTL0 = CCIE;
  TA0CCTL1 = OUTMOD_7;
  TA0CCTL2 = OUTMOD_7;

  //1,5 ms = 90°
}

void pwm_set(int nr, int v) {
  servo[nr].v_ = v;
  servo[nr].target = pwm_min + v * 3*200;
}

int pwm_get(int nr)
{
  return servo[nr].v_;
}

void pwm_setSpeed(int s){
  speed = s;
}

int pwm_getSpeed()
{
  return speed;
}

/*
 * Power On Delay Loop
*/
void pwm_checkServoPowerOn(int nr, int on){
  if(on)
    servo[nr].servoPowerOnCounter = 50;

  pwm_powerServer(nr, servo[nr].servoPowerOnCounter != 0);

  if(servo[nr].servoPowerOnCounter > 0)
    servo[nr].servoPowerOnCounter--;
}

void pwm_step(){
  for(int nr = 0; nr < SERVO_COUNT; nr++){
    struct ServoData * s = &servo[nr];
    if(s->target < pwm_min)
      s->target = pwm_min;
    if(s->target > pwm_max)
      s->target = pwm_max;


    if(s->target < s->step)
      s->step -= 1*200; //0,1ms
    if(s->target > s->step)
      s->step += 1*200;

    if(nr == 0)
      TA0CCR1 = s->step;
    else if(nr == 1)  
      TA0CCR2 = s->step;

    pwm_checkServoPowerOn(nr, s->target != s->step);
  }
}

static void __attribute__((__interrupt__(TIMER0_A0_VECTOR)))
isr_cc0_TA0(void)
{
  static int counter = 0;
  if(speed == 0 || counter++ % speed == 0){
    pwm_step();
  }
  TA0CCTL0 &= ~CCIFG;
}

