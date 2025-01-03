#include <msp430.h>

#include "uart.h"
#include "pwm.h"
#include "protocol.h"
#include "data.h"
#include "gpio.h"

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************

#define RELAY_DELAY_CYCLES 2000

void initClockTo16MHz()
{
    // Configure one FRAM waitstate as required by the device datasheet for MCLK
    // operation beyond 8MHz _before_ configuring the clock system.
    FRCTL0 = FRCTLPW | NWAITS_1;

    __bis_SR_register(SCG0);    // disable FLL
    CSCTL3 |= SELREF__REFOCLK;  // Set REFO as FLL reference source
    CSCTL0 = 0;                 // clear DCO and MOD registers
    CSCTL1 &= ~(DCORSEL_7);     // Clear DCO frequency select bits first
    CSCTL1 |= DCORSEL_5;        // Set DCO = 16MHz
    CSCTL2 = FLLD_0 + 487;      // set to fDCOCLKDIV = (FLLN + 1)*(fFLLREFCLK/n)
                                //                   = (487 + 1)*(32.768 kHz/1)
                                //                   = 16 MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                        // enable FLL
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));      // FLL locked

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
}

void initGPIO(){
    P1DIR  = BIT6 | BIT7; // enable output
    P1SEL0 = BIT6 | BIT7; // enable PWM out
    P2DIR  = BIT0 | BIT1; // enable output
    P2SEL0 = BIT0 | BIT1; // enable PWM out
    P3DIR  = 0xFF; // output for LED
    P4DIR  = BIT0| BIT1 | BIT6 | BIT7; // output Relais
    P4SEL0 = BIT2 | BIT3; // enable UART1

    P1IES = BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5; // Hi/Low edge
    P1IE  = BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5; // interrupt enabled
    P1IFG = 0; // IFG cleared
    P2IES = BIT4 | BIT5; // Hi/Low edge
    P2IE  = BIT4 | BIT5; // interrupt enabled
    P2IFG = 0; // IFG cleared
}

void updateLEDs()
{   
    uint8_t leds = 0;
    switch (getFRAMValue(RM_Function))
    {
    case FN_Weiche4x:
        for(int nr = 0; nr < 4; nr++){
            leds |= (0x1 << getFRAMValue(RM_ServoFunction1 + nr) ? 1 : 0) << (nr * 2);
        }
        break;
    case FN_Kreuzweiche2x:
        for(int nr = 0; nr < 2; nr++){
            leds |= (0x1 << getFRAMValue(RM_ServoFunction1 + nr) ? 1 : 0) << (nr * 2);
        }
        break;
    case FN_Weiche2x_Kreuzweiche:
        for(int nr = 0; nr < 2; nr++){
            leds |= (0x1 << getFRAMValue(RM_ServoFunction1 + nr) ? 1 : 0) << (nr * 2);
        }
        leds |= (0x1 << getFRAMValue(RM_ServoFunction3) ? 1 : 0) << (6);
        break;
    case FN_Doppelkreuzweiche:
        leds |= 0x1 << getFRAMValue(RM_ServoFunction1);
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
        pwm_toggle(nr, getFRAMValue(RM_ServoFunction1 + nr));
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
        setRelais(nr, v, 1);
    }
    __delay_cycles(RELAY_DELAY_CYCLES);
    for(int nr = 0; nr < 2; nr++){
        setRelais(nr, getFRAMValue(RM_ServoFunction1 + nr), 0);
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
    setRelais(1, v, 1);
    __delay_cycles(RELAY_DELAY_CYCLES);
    setRelais(1, v, 0);
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
        setRelais(0, 0, 1);
        setRelais(1, 0, 1);
        __delay_cycles(RELAY_DELAY_CYCLES);
        setRelais(0, 0, 0);
        setRelais(1, 0, 0);
        break;
    case 1:
        pwm_toggle(0, 1);
        pwm_toggle(1, 0);
        pwm_toggle(2, 0);
        pwm_toggle(3, 1);
        setRelais(0, 1, 1);
        setRelais(1, 0, 1);
        __delay_cycles(RELAY_DELAY_CYCLES);
        setRelais(0, 1, 0);
        setRelais(1, 0, 0);
        break;
    case 2:
        pwm_toggle(0, 0);
        pwm_toggle(1, 1);
        pwm_toggle(2, 1);
        pwm_toggle(3, 0);
        setRelais(0, 0, 1);
        setRelais(1, 1, 1);
        __delay_cycles(RELAY_DELAY_CYCLES);
        setRelais(0, 0, 0);
        setRelais(1, 1, 0);
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
    case FN_Doppelkreuzweiche:
        Doppelkreuzweiche_in(getSwitchInput());
        Doppelkreuzweiche_out();
        break;
    case FN_Unset:
        setFRAMValue(RM_ServoFunction1, getSwitchInput());
        break;
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop Watchdog
    initClockTo16MHz();
    initGPIO();
    PM5CTL0 &= ~LOCKLPM5;                    // Disable the GPIO power-on default high-impedance mode
                                            // to activate 1previously configured port settings
    uart_init();

    //lade alten zustand für PWM und Relais
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
    case FN_Doppelkreuzweiche:
        Doppelkreuzweiche_out();
        break;
    }
    pwm_init();
    while(1){
        updateLEDs();
#if UART_MODE == SMCLK_9600
        __bis_SR_register(LPM3_bits + GIE);       // Since ACLK is source, enter LPM3, interrupts enabled
#else
        __bis_SR_register(LPM0_bits + GIE);       // Since SMCLK is source, enter LPM0, interrupts enabled
#endif
        while (checkUart()) { }
        //__delay_cycles(1000); // Entprellen???
        if(isSwitchUpdated())
            checkInputs();
    }
    __no_operation();                         // For debugger
}

