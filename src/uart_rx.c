#include <msp430.h>

#include <uart_init.h>

char uart_readbuffer[uart_buffer_size];
int uart_readpos_in = 0;
int uart_readpos_out = 0;

char uart_read(int pos){
  return uart_readbuffer[(uart_readpos_out + pos) & (uart_buffer_size - 1)];
}

void uart_seek(int seek){
  uart_readpos_out += seek;
}

int uart_getValidBufferSize(){
  return uart_readpos_in - uart_readpos_out;
}

// USCI A0/B0 Receive ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  while(uart_getValidBufferSize() >= uart_buffer_size){
    // make some space
    uart_seek(1);
  }
  uart_readbuffer[uart_readpos_in++ & (uart_buffer_size - 1)] = UCA0RXBUF;
  __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
}