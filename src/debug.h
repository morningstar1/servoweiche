#pragma once

#include <stdint.h>
uint16_t getDebug(uint16_t register_);
void setDebug(uint16_t register_, uint16_t value);
void incDebug(uint16_t register_);

void initDebug();