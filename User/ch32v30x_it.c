/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_it.h"
#include "los_interrupt.h"
#include "user.h"
#include <string.h>
#include "ESP8266.h"

// 中断相关全局变量
FrameData_t g_frame_data;     // 数据帧缓冲区（中断接收使用）
uint8_t WIFI_CMD[128];        // WiFi命令缓冲区（中断接收使用）

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void WIFI_USART_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void recDataProcessing(uint8_t data);   // 数据接收状态机处理

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
    GET_INT_SP();
    HalIntEnter();
    while(1)
    {

    }

    HalIntExit();
    FREE_INT_SP();
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{

  GET_INT_SP();
  HalIntEnter();

  printf("mcause:%08x\r\n",__get_MCAUSE());
  printf("mtval:%08x\r\n",__get_MTVAL());
  printf("mepc:%08x\r\n",__get_MEPC());

  while (1)
  {
  }
  HalIntExit();
  FREE_INT_SP();
}


void RecvFrame_Init(void)
{
    g_frame_data.state = RECV_STATE_HEADER1;
    g_frame_data.data_index = 0;
}

void WIFI_USART_IRQHandler()
{
    if(USART_GetITStatus(WIFI_USARTx, USART_IT_RXNE) != RESET)
    {
        static uint16_t cmd_index = 0;
        uint8_t data = USART_ReceiveData(WIFI_USARTx);
        
        switch (WIFI_RECVSTOP)
        {

        //进入数据接收模式
        case WIFI_RECV_DATA:
            recDataProcessing(data);
            break;
            
        //进入命令接收模式
        case WIFI_RECV_CMD:
            if(cmd_index < sizeof(WIFI_CMD) - 1)
            {
                WIFI_CMD[cmd_index++] = data;
                

                if(data == '\n')
                {
                    WIFI_CMD[cmd_index] = '\0';
                    WIFI_CMD_ReturnCheck();
                    cmd_index = 0;
                }
            }
            else
            {

                WIFI_CMD[sizeof(WIFI_CMD) - 1] = '\0';
                cmd_index = 0;

                WIFI_CMD_FLAG = 2;
            }
            break;   
        default:
            cmd_index = 0;
            break;
        }
        USART_ClearITPendingBit(WIFI_USARTx, USART_IT_RXNE);
    }
}

void recDataProcessing(uint8_t data)
{
    switch(g_frame_data.state)
    {
        case RECV_STATE_HEADER1:
            if(data == FRAME_HEADER1)
            {
                g_frame_data.state = RECV_STATE_HEADER2;
            }
            break;
            
        case RECV_STATE_HEADER2:
            if(data == FRAME_HEADER2)
            {
                g_frame_data.state = RECV_STATE_CMD_TYPE;
            }
            else
            {

                g_frame_data.state = RECV_STATE_HEADER1;
            }
            break;
            
        case RECV_STATE_CMD_TYPE:
            g_frame_data.cmd_type = data;
            g_frame_data.state = RECV_STATE_CMD;
            break;
            
        case RECV_STATE_CMD:
            g_frame_data.cmd = data;
            g_frame_data.state = RECV_STATE_LEN_HIGH;
            break;
            
        case RECV_STATE_LEN_HIGH:
            g_frame_data.data_len = data << 8;
            g_frame_data.state = RECV_STATE_LEN_LOW;
            break;
            
        case RECV_STATE_LEN_LOW:
            g_frame_data.data_len |= data;
            

            //判断有效数据是否超过允许范围
            if(g_frame_data.data_len > 0 && g_frame_data.data_len <= MAX_DATA_LEN)
            {
                g_frame_data.data_index = 0;
                g_frame_data.state = RECV_STATE_DATA;
            }
            else
            {
                printf("Invalid data length: %d\r\n", g_frame_data.data_len);
                g_frame_data.state = RECV_STATE_HEADER1;
            }
            break;
            
        case RECV_STATE_DATA:

            if(g_frame_data.data_index < g_frame_data.data_len)
            {
                g_frame_data.data_buffer[g_frame_data.data_index++] = data;
                

                if(g_frame_data.data_index >= g_frame_data.data_len)
                {
                    //发送接收完成事件
                    LOS_EventWrite(&gWifiEventGroup,WIFI_EVENT_RECV);
                    

                    g_frame_data.state = RECV_STATE_HEADER1;
                }
            }
            break;
            
        default:
            g_frame_data.state = RECV_STATE_HEADER1;
            break;
    }
}


