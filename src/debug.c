#include "debug.h"

#define DEBUGREGISTERS 16
uint16_t counter[DEBUGREGISTERS];

void initDebug()
{
    for(int i = 0; i< DEBUGREGISTERS;i ++){
        counter[i] = 0;
    }
}

uint16_t getDebug(uint16_t register_)
{
    if(register_ < DEBUGREGISTERS){
        return counter[register_];
    }
    return 0;
}

void setDebug(uint16_t register_, uint16_t value)
{
    if(register_ < DEBUGREGISTERS){
        counter[register_] = value;
    }
}

void incDebug(uint16_t register_)
{
    if(register_ < DEBUGREGISTERS){
        counter[register_]++;
    }
}