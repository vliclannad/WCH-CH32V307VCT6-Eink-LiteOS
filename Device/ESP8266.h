#ifndef __ESP8266_H
#define __ESP8266_H
#include "debug.h"

// WIFI 串口配置信息
#define WIFI_USARTx                     UART6
#define WIFI_USART_CLK                  RCC_APB1Periph_UART6
#define WIFI_USART_BAUDRATE             115200

#define WIFI_USART_GPIO_CLK             RCC_APB2Periph_GPIOC
#define WIFI_USART_TX_GPIO_PORT         GPIOC
#define WIFI_USART_TX_GPIO_PIN          GPIO_Pin_0
#define WIFI_USART_RX_GPIO_PORT         GPIOC
#define WIFI_USART_RX_GPIO_PIN          GPIO_Pin_1

#define WIFI_USART_IRQ                  UART6_IRQn
#define WIFI_USART_IRQHandler           UART6_IRQHandler

//定义函数返回状态
#define WIFI_OK 0
#define WIFI_ERROR 1
#define WIFI_NORESPONSE 2


//工作模式
#define WIFI_STATION 1
#define WIFI_AP      2
#define WIFI_ST_AP   3

//接收模式
#define WIFI_RECV_DATA 0
#define WIFI_RECV_CMD  1
#define WIFI_RECV_RST  2

//透传模式
#define WIFI_NONE_PASS_THROUGH 0
#define WIFI_PASS_THROUGH 1

//==============全局状态变量（可在其他文件中使用）==============
extern uint8_t WIFI_RECVSTOP;        // 接收模式标志
extern uint8_t WIFI_CONNECT_STATE;   // TCP连接状态
extern uint8_t WIFI_AP_STATE;        // Wi-Fi连接状态
extern uint8_t WIFI_SEND_STATE;      // 透传模式下发送模式状态标志
extern uint8_t WIFI_BOOTFLAG;        // 启动完成标志
extern uint8_t WIFI_CMD_FLAG;        // 命令状态标志

extern uint8_t WIFI_CMD[128];   //命令返回数据接收缓冲区

void WIFI_Init(uint8_t mode_type);
uint8_t WIFI_LinkSSID(uint8_t* ssid,uint8_t* password);
uint8_t WIFI_TCPConnect(uint8_t* server_ip,uint8_t* server_port);
uint8_t WIFI_Disconnnect();
uint8_t WIFI_SetTxMode(uint8_t modetype);
uint8_t WIFI_CMD_ReturnCheck();
#endif
