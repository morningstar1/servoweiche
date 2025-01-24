#include <msp430.h>
#include "gpio.h"
#include "debug.h"

uint8_t switchInput = 0;

uint8_t getSwitchInput()
{
    return switchInput;
}
uint8_t switchUpdate = 0;

uint8_t isSwitchUpdated()
{
    uint8_t v = switchUpdate;
    switchUpdate = 0;
    return v;
}
void setLEDs(uint8_t output)
{
    P3OUT = output;
}

void setRelais(int nr, int direction, int value)
{
    if(nr == 0){
        if(direction == 0){
            if(value == 0){
                P4OUT &= ~BIT0;
            }else if(value = 1){
                P4OUT |= BIT0;
            }
        }else if(direction == 1){
            if(value == 0){
                P4OUT &= ~BIT1;
            }else if(value == 1){
                P4OUT |= BIT1;
            }
        }
    }else if(nr == 1){
        if(direction == 0){
            if(value == 0){
                P4OUT &= ~BIT6;
            }else if(value = 1){
                P4OUT |= BIT6;
            }
        }else if(direction == 1){
            if(value == 0){
                P4OUT &= ~BIT7;
            }else if(value == 1){
                P4OUT |= BIT7;
            }
        }
    }
}

uint8_t getPort1(){
    return (P1IFG | ~P1IN) & (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
}

uint8_t getPort2(){
    return ((P2IFG | ~P2IN) & (BIT4 | BIT5)) << 2;
}

// Port 1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_1 (void)
#else
#error Compiler not supported!
#endif
{
    switchInput = getPort1() | getPort2();
    P1IFG = 0;
    switchUpdate = 1;
    __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
}

// Port 2 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT2_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
    switchInput = getPort1() | getPort2();
    P2IFG = 0;
    switchUpdate = 1;
    __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
}