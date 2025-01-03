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
  //__delay_cycles(1250);
	UCA0IFG |= UCTXIFG;                           // Set an interruput manually
	UCA0IE |= UCTXIE | UCTXCPTIE;                 // Enable transfer interrupt
}
