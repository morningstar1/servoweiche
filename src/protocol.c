#include <stdint.h>
#include <msp430.h>
#include "uart.h"
#include "data.h"
#include "reset.h"
#include "debug.h"
#include "gpio.h"

#if 0
char getHex(int v){
    return  v > 9 ? 'A' + v - 10 : '0' + v;
}
void sendHexData(uint16_t v1, uint16_t v2)
{
    char hexbuf[8];
    hexbuf[7] = getHex((v2 & 0x000F));
    hexbuf[6] = getHex((v2 & 0x00F0) >> 4);
    hexbuf[5] = getHex((v2 & 0x0F00) >> 8);
    hexbuf[4] = getHex((v2 & 0xF000) >> 12);
    hexbuf[3] = getHex((v1 & 0x000F));
    hexbuf[2] = getHex((v1 & 0x00F0) >> 4);
    hexbuf[1] = getHex((v1 & 0x0F00) >> 8);
    hexbuf[0] = getHex((v1 & 0xF000) >> 12);
    uart_send(hexbuf, 8);
}
#endif

typedef union  
{
    struct 
    {
        uint16_t function;
        uint16_t register_;
        uint16_t data;
        uint16_t crc;
    }; 
    uint8_t data_u8[8];
    uint16_t data_u16[4];
    char data_c[8];
} Message;

#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(COND)?1:-1]
STATIC_ASSERT(sizeof(Message) == 8, wrong_message_size);

const unsigned int CRC_Init = 0xffff;
uint16_t calcCRC(const Message * d)
{
    CRCINIRES = CRC_Init;
    for(int i=0; i < 3; i++) {
        CRCDIRB = d->data_u16[i];
        __no_operation();
    }
    return CRCINIRES;
}

void checkData(Message * d){
    switch (d->function)
    {
    case 1: //read
      d->data = getFRAMValue(d->register_);
      break;
    case 2: //write
      d->data = setFRAMValue(d->register_, d->data);
      break;
    case 3:
      d->data = 1;
      requestReset();
    case 4:
      d->data = getDebug(d->register_);
      break;
    case 5:
      setRelais(
        d->data & 0x01 ? 1 : 0, 
        d->data & 0x02 ? 1 : 0, 
        d->data & 0x04 ? 1 : 0
      );
      d->data = 1;
      break;
    }

    d->crc = calcCRC(d);
    uart_send(d->data_c, sizeof(Message));
}

int checkUart()
{
    if (uart_getValidBufferSize() < sizeof(Message))
        return 0; //need more data

    Message d;
    for(int i=0; i < sizeof(Message); i++) {
        d.data_c[i] = uart_read(i);
    }
#if 1
    if(calcCRC(&d) != d.crc){
        //nothing found, try again with one byte less
        uart_seek(1);
    }else{
        uart_seek(sizeof(Message));
        checkData(&d);
    }
#else
    sendHexData(CRCINIRES, d.crc);
    uart_seek(sizeof(Message));
#endif
    return 1; //try again with more data
}