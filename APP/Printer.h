#ifndef __PRINTER_H
#define __PRINTER_H

#include "usartx.h"
#include "Pin.h"

typedef struct
{
    Pin_T  Tx_pin;
    Pin_T  Rx_pin;
    UsartT usart; 
}PrinterT;


void printer_init(void);
void SendCharToPrint(u8 ch);
void SendStringToPrint(char *buf);

#endif


