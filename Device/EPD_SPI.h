#ifndef __EPD_SPI_H
#define __EPD_SPI_H
#include "debug.h"


#define  EPD_MOSI_PIN    GPIO_Pin_10
#define  EPD_CLK_PIN     GPIO_Pin_11
#define  EPD_CS_PIN      GPIO_Pin_8    //低电平选择
#define  EPD_DC_PIN      GPIO_Pin_9    //高电平传数据，低电平传命令
#define  EPD_RST_PIN     GPIO_Pin_3    //低电平复位

#define  EPD_OUT_PORT    GPIOD
#define  EPD_OUT_CLK     RCC_APB2Periph_GPIOD            

#define  EPD_BUSY_PIN    GPIO_Pin_15    //低电平表示繁忙
#define  EPD_BUSY_PORT   GPIOE
#define  EPD_BUSY_CLK    RCC_APB2Periph_GPIOE

#define  EPD_MOSI_SET    GPIO_SetBits(EPD_OUT_PORT, EPD_MOSI_PIN)
#define  EPD_MOSI_CLR    GPIO_ResetBits(EPD_OUT_PORT, EPD_MOSI_PIN)
#define  EPD_CLK_SET     GPIO_SetBits(EPD_OUT_PORT, EPD_CLK_PIN)
#define  EPD_CLK_CLR     GPIO_ResetBits(EPD_OUT_PORT, EPD_CLK_PIN)
#define  EPD_CS_SET      GPIO_SetBits(EPD_OUT_PORT, EPD_CS_PIN)
#define  EPD_CS_CLR      GPIO_ResetBits(EPD_OUT_PORT, EPD_CS_PIN)
#define  EPD_DC_SET      GPIO_SetBits(EPD_OUT_PORT, EPD_DC_PIN)
#define  EPD_DC_CLR      GPIO_ResetBits(EPD_OUT_PORT, EPD_DC_PIN)
#define  EPD_RST_SET     GPIO_SetBits(EPD_OUT_PORT, EPD_RST_PIN)
#define  EPD_RST_CLR     GPIO_ResetBits(EPD_OUT_PORT, EPD_RST_PIN)
#define  EPD_BUSY_READ   GPIO_ReadInputDataBit(EPD_BUSY_PORT, EPD_BUSY_PIN)

void EPD_SPI_Init(void);

void EPD_WriteCMD(uint8_t cmd);
void EPD_WriteDATA(uint8_t data);
void EPD_CheckBusy(void);
void EPD_Delay(uint32_t ms);



#endif