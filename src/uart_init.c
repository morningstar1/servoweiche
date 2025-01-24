#include <msp430.h>
#include "uart_init.h"
#include "uart.h"

void uart_init()
{
    // Configure USCI_A0 for UART mode
    UCA1CTLW0 |= UCSWRST;                      // Put eUSCI in reset
#if UART_MODE == SMCLK_115200

    UCA1CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
    // Baud Rate Setting
    // Use Table 21-5
    UCA1BRW = 8;
    UCA1MCTLW |= UCOS16 | UCBRF_10 | 0xF700;   //0xF700 is UCBRSx = 0xF7

#elif UART_MODE == SMCLK_9600

    UCA1CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
    // Baud Rate Setting
    // Use Table 21-5
    UCA1BRW = 104;
    UCA1MCTLW |= UCOS16 | UCBRF_2 | 0xD600;   //0xD600 is UCBRSx = 0xD6
#else
    # error "Please specify baud rate to 115200 or 9600"
#endif

    UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA1IE |= UCRXIE;             // Enable USCI_A0 RX interrupt
}


//******************************************************************************
// UART Interrupt ***********************************************************
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  //switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
  switch(UCA1IV)
  {
    case USCI_NONE:
      break;
    case USCI_UART_UCRXIFG:
      while(uart_getValidBufferSize() >= uart_buffer_size){
        // make some space
        uart_seek(1);
      }
      uart_readbuffer[uart_readpos_in++ & (uart_buffer_size - 1)] = UCA1RXBUF;
      UCA1IFG &= ~(UCRXIFG); //clear interrupt
      __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
      break;
    case USCI_UART_UCTXIFG:
      UCA1TXBUF = uart_writebuffer[uart_writepos++];
      if (uart_writepos == uart_writebuffersize)                  // TX over?
        UCA1IE &= ~UCTXIE;                       // Disable USCI_A0 TX interrupt
      break;
    case USCI_UART_UCSTTIFG: 
      break;
    case USCI_UART_UCTXCPTIFG: 
      if (uart_writepos == uart_writebuffersize)                  // TX over?
      {
        //__delay_cycles(1250);
        //UART_OUT &= ~(UART_TX_ENABLE); // auf 0 setzen
        UCA1IE &= ~UCTXCPTIE;               
      }
      break;
  }
}
