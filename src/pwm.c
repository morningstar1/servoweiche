#include <msp430g2553.h>

#define PWM BIT6
const int min = 14;
const int max = 82;
int target = 14;
int speed = 3;

int pwm_init() {
  P1DIR |= (PWM);
  P1SEL |= PWM;
  P1SEL2 &= ~PWM;

  TA0CTL = TASSEL_1 | TACLR | ID_0 | MC_1 /*| TAIE*/;
  TA0R = 0;
  TA0CCR0 = 164 * 4;
  TA0CCR1 = 8 * 4;
  TA0CCTL1 = OUTMOD_7 /* | CCIE*/;
  TA0CCTL0 = CCIE;
}

void pwm_set(int v) {
  //target = v ? max : min;

  //int delta = ;
  target = ((max-min) * v) / (8-1) + min;
  //target = ((8 / ((max - min) * v))) + min; 
}
void pwm_setSpeed(int s){
  speed = s;
}
void step(){
  if(target < min)
    target = min;
  if(target > max)
    target = max;


  static int step = min;
  if(target < step)
    step--;
  if(target > step)
    step++;

  TA0CCR1 = target;
}

static void __attribute__((__interrupt__(TIMER0_A0_VECTOR)))
isr_cc0_TA0(void)
{
  static int counter = 0;
  if(speed == 0 || counter++ % speed == 0){
    step();
  }
  TA0CCTL0 &= ~CCIFG;
}

static void __attribute__((__interrupt__(TIMER0_A1_VECTOR)))
isr_cc1_TA0(void)
{
  TA0CCTL1 &= ~CCIFG;
}
