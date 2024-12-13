#pragma once
#include <stddef.h>

#define SMCLK_115200    0
#define SMCLK_9600      1

#define UART_MODE       SMCLK_115200//SMCLK_9600//

void  uart_init(void);
char uart_read(int pos);
void uart_seek(int seek);
int  uart_getValidBufferSize();

void uart_send(char * data, size_t size);
