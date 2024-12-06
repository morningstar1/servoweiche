#include <msp430.h>
#include <stddef.h>
#include <string.h>

#include "uart_init.h"

int uart_writepos = 0;
char uart_writebuffer[uart_buffer_size];
int uart_writebuffersize;

void uart_send(char * data, size_t size){
  if(size > uart_buffer_size)
    return;
  memcpy(uart_writebuffer, data, size);
  uart_writepos = 0;
  uart_writebuffersize = size;
  IE2 |= UCA0TXIE;                        // Enable USCI_A0 TX interrupt
  UCA0TXBUF = uart_writebuffer[uart_writepos++];
}

// USCI A0/B0 Transmit ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  UCA0TXBUF = uart_writebuffer[uart_writepos++];

  if (uart_writepos == uart_writebuffersize)                  // TX over?
    IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
}