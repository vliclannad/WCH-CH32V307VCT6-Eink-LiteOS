#ifndef _USER_UART_H
#define _USER_UART_H
#include "debug.h"

void USER_UART_Transmit(USART_TypeDef *USARTx,uint8_t *Data,uint32_t count);
void USER_UART_SendString(USART_TypeDef *USARTx,uint8_t *Data);







#endif
