/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 * 
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430G2xx3 Demo - USCI_A0, Ultra-Low Pwr UART 9600 RX/TX, 32kHz ACLK
//
//   Description: This program demonstrates a full-duplex 9600-baud UART using
//   USCI_A0 and a 32kHz crystal.  The program will wait in LPM3, and receive
//   a string1 into RAM, and echo back the complete string.
//   ACLK = BRCLK = LFXT1 = 32768Hz, MCLK = SMCLK = DCO ~1.2MHz
//   Baud rate divider with 32768Hz XTAL @9600 = 32768Hz/9600 = 3.41
//* An external watch crystal is required on XIN XOUT for ACLK *//
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 9600 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include <msp430.h>
#include "pwm.h"
#include "uart.h"

#define LED1 BIT0
#define LED2 BIT6

#include <stdint.h>
#include <stdio.h>

int checkModbus()
{
  if (uart_getValidBufferSize() < 4)
    return 0; //need more data

  uint8_t slaveadr = uart_read(0);

  switch (uart_read(1)) //function
  {
    case 0x01: //Read	Discrete Output Coils
      break;
    case 0x05: //Write single	Discrete Output Coil
      break;
    case 0x0F: //Write multiple	Discrete Output Coils
      break;
    case 0x02: //Read	Discrete Input Contacts
      break;
    case 0x04: //Read	Analog Input Registers
      break;
    case 0x03: //Read	Analog Output Holding Registers
      break;
    case 0x06: //Write single	Analog Output Holding Register
      //we need 8 Byte total
      if(uart_getValidBufferSize() >= 8 && slaveadr == 1){
        uint16_t write_register = (((int)uart_read(2)) << 8) & ((int)uart_read(3));
        uint16_t data = (((int)uart_read(4)) << 8) & ((int)uart_read(5));
        uint16_t crc = (((int)uart_read(7)) << 8) & ((int)uart_read(6));
        pwm_set(uart_read(5));
        P1OUT ^= (LED1);
        char ret[8];
        for(int i = 0; i< 8; i++)
          ret[i] = uart_read(i);
        uart_seek(8);
        uart_send(ret, 8);
        return 1; //try again with more data
      }else{
        return 0; //need more data
      }
      break;
    case 0x10: //Write multiple	Analog Output Holding Registers
      break;
  }
  //nothing found, try again with one byte less
  uart_seek(1);
  return 1;
}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P1DIR |=  (LED1);
  P1OUT &= ~(LED1); //LED1 aus

  uart_init();
  pwm_init();

  while(1){
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, interrupts enabled
    //check tasks;
    while (checkModbus())  {  }
  }

}
