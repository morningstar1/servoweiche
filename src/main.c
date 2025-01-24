#include <msp430.h>

#include "uart.h"
#include "pwm.h"
#include "protocol.h"
//#include "data.h"
#include "gpio.h"
#include "debug.h"
#include "logic.h"

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************

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
    P1SEL0 &= !(BIT6 | BIT7); // enable PWM out
    P1SEL1 = BIT6 | BIT7; // enable PWM out
    P2DIR  = BIT0 | BIT1; // enable output
    P2SEL0 = BIT0 | BIT1; // enable PWM out
    P2SEL1 &= !(BIT0 | BIT1); // enable PWM out
    P3DIR  = 0xFF; // output for LED
    P4DIR  = BIT0| BIT1 | BIT6 | BIT7; // output Relais
    P4SEL0 = BIT2 | BIT3; // enable UART1
    P4SEL1 &= ~(BIT2 | BIT3); // enable UART1

    P1IES = BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5; // Hi/Low edge
    P1IE  = BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5; // interrupt enabled
    P1IFG = 0; // IFG cleared
    P2IES = BIT4 | BIT5; // Hi/Low edge
    P2IE  = BIT4 | BIT5; // interrupt enabled
    P2IFG = 0; // IFG cleared
}

uint8_t resetNeeded = 0;
void requestReset(){
    resetNeeded = 1;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop Watchdog
    initClockTo16MHz();

    initDebug();

    initGPIO();
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate 1previously configured port settings
    uart_init();

    //lade alten zustand für PWM und Relais
    setupInitalValues();
    
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
        if(resetNeeded)
            PMMCTL0 = PMMPW | PMMSWPOR;   // This triggers a Software BOR
        
    }
    __no_operation();                         // For debugger
}

