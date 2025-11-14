#include "user_usart.h"



//发送指定数量的字符
void USER_UART_Transmit(USART_TypeDef *USARTx,uint8_t *Data,uint32_t count)
{
    while(count--)
    {
        USART_SendData(USARTx, *Data++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);

    }

}

//发送以'\0'为结尾的字符串
void USER_UART_SendString(USART_TypeDef *USARTx,uint8_t *Data)
{
    while(*Data!='\0')
    {
        USART_SendData(USARTx, *Data++);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE)==RESET);
    }
}
