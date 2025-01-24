#pragma once
extern int  pwm_init();
extern void pwm_toggle(int nr, int v);
extern void requestRelais(int nr, int dir);

#define SERVO_COUNT 4
