#pragma once
#include <stdint.h>

extern uint16_t getRead(uint16_t register_);
extern uint16_t setWrite(uint16_t register_, uint16_t data);
extern uint8_t getMySlaveAddr();
extern void initLed();
extern void setLed(uint16_t data);
