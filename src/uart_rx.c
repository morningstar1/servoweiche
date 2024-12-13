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
