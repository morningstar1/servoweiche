#pragma once

#define uart_buffer_bits 5
#define uart_buffer_size (1 << uart_buffer_bits)

extern char uart_readbuffer[uart_buffer_size];

extern int uart_readpos_in;
extern int uart_readpos_out;

extern int uart_writepos;
extern char uart_writebuffer[uart_buffer_size];
extern int uart_writebuffersize;