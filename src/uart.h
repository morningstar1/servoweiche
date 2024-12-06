#include <stddef.h>

void  uart_init(void);
char uart_read(int pos);
void uart_seek(int seek);
int  uart_getValidBufferSize();

void uart_send(char * data, size_t size);
