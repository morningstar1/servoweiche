#pragma once
#include <stdio.h>
extern uint8_t getSwitchInput();
extern uint8_t isSwitchUpdated();
extern void setLEDs(uint8_t output);
extern void setRelais(int nr, int direction, int value);
