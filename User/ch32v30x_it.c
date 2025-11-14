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
/*
 * @brief CRC-16校验计算函数
 * @param data 数据指针
 * @param len 数据长度
 * @return CRC校验值
 */
uint16_t CRC16_Calculate(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;
    
    for(i = 0; i < len; i++)
    {
        crc ^= data[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 1)
            {
                crc = (crc >> 1) ^ 0xA001;  // CRC-16-MODBUS多项式
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    
    return crc;
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
    static uint16_t data_index = 0;//接收有效数据索引
    
    static RecvState_t recState = RECV_STATE_HEADER1;          // 当前接收状态
    uint16_t crc_calculated = 0;//计算得到的CRC值

    switch(recState)
    {
        case RECV_STATE_HEADER1:
            if(data == FRAME_HEADER1)
            {
                recState = RECV_STATE_HEADER2;
            }
            else
            {
                printf("[ERR] Header1 mismatch: 0x%02X (expected 0xA5)\r\n", data);
            }
            break;
            
        case RECV_STATE_HEADER2:
            if(data == FRAME_HEADER2)
            {
                recState = RECV_STATE_CMD_TYPE;
            }
            else
            {
                printf("[ERR] Header2 mismatch: 0x%02X (expected 0x5A)\r\n", data);
                recState = RECV_STATE_HEADER1;
            }
            break;
            
        case RECV_STATE_CMD_TYPE:
            g_frame_data.cmd_type = data;
            recState = RECV_STATE_CMD;
            break;
            
        case RECV_STATE_CMD:
            g_frame_data.cmd = data;
            recState = RECV_STATE_LEN_HIGH;
            break;
            
        case RECV_STATE_LEN_HIGH:
            g_frame_data.data_len = data << 8;
            recState = RECV_STATE_LEN_LOW;
            break;
            
        case RECV_STATE_LEN_LOW:
            g_frame_data.data_len |= data;
            
<<<<<<< HEAD
            // 判断有效数据是否超过允许范围
            // data_len 包含: 总包数(1) + 包序号(1) + 图像数据(可变) = 最大1024字节
=======

            //判断有效数据是否超过允许范围
>>>>>>> 46d6483c1bfe9628b6953e5b2c01064a49835570
            if(g_frame_data.data_len > 0 && g_frame_data.data_len <= MAX_DATA_LEN)
            {
                data_index = 0;
                recState = RECV_STATE_DATA;
            }
            else
            {
<<<<<<< HEAD
                printf("[ERR] Invalid data length: %d (max: %d)\r\n", 
                       g_frame_data.data_len, MAX_DATA_LEN);
                recState = RECV_STATE_HEADER1;
=======
                printf("Invalid data length: %d\r\n", g_frame_data.data_len);
                g_frame_data.state = RECV_STATE_HEADER1;
>>>>>>> 46d6483c1bfe9628b6953e5b2c01064a49835570
            }
            break;
            
        case RECV_STATE_DATA:
            if(data_index < g_frame_data.data_len)  // 接收所有有效数据
            {
                g_frame_data.data_buffer[data_index++] = data;
                
                // 所有有效数据接收完，准备接收CRC
                if(data_index >= g_frame_data.data_len)
                {
                    recState = RECV_STATE_CRC_HIGH;
                }
            }
            break;
            
        case RECV_STATE_CRC_HIGH:
            g_frame_data.crc_received = (data << 8);  // 第一个字节是高字节
            recState = RECV_STATE_CRC_LOW;
            break;
            
        case RECV_STATE_CRC_LOW:
            g_frame_data.crc_received |= data;  // 第二个字节是低字节
            
            // 计算接收到的数据的CRC（不包含帧头，只包含命令类型、命令、长度和有效数据）
            uint8_t crc_data[6 + MAX_DATA_LEN];  // cmd_type(1) + cmd(1) + len_high(1) + len_low(1) + data(MAX_DATA_LEN) + 最多
            uint16_t crc_len = 0;
            
            crc_data[crc_len++] = g_frame_data.cmd_type;
            crc_data[crc_len++] = g_frame_data.cmd;
            crc_data[crc_len++] = (g_frame_data.data_len >> 8) & 0xFF;
            crc_data[crc_len++] = g_frame_data.data_len & 0xFF;
            
            // 添加有效数据（所有有效数据，包含总包数、包序号、图像数据）
            for(uint16_t i = 0; i < g_frame_data.data_len; i++)
            {
                crc_data[crc_len++] = g_frame_data.data_buffer[i];
            }
            
            crc_calculated = CRC16_Calculate(crc_data, crc_len);
            
            // 验证CRC
            if(g_frame_data.crc_received == crc_calculated)
            {
                printf("[OK] Frame received: type=0x%02X, cmd=0x%02X, len=%d, CRC=0x%04X\r\n",
                       g_frame_data.cmd_type, g_frame_data.cmd, g_frame_data.data_len, 
                       g_frame_data.crc_received);
                // 发送接收完成事件
                LOS_EventWrite(&gWifiEventGroup, WIFI_EVENT_RECV);
            }
            else
            {
                printf("[ERR] CRC mismatch: received=0x%04X, calculated=0x%04X\r\n",
                       g_frame_data.crc_received, crc_calculated);
            }

            data_index = 0;
            recState = RECV_STATE_HEADER1;
            break;
            
        default:
            printf("[ERR] Invalid state: %d\r\n", recState);
            data_index = 0;
            recState = RECV_STATE_HEADER1;
            break;
    }
}


