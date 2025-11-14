#include "EPD_SPI.h"
#include "los_task.h"


void EPD_SPI_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(EPD_OUT_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(EPD_BUSY_CLK, ENABLE);

    //MOSI,CLK,CS,DC,RST
    GPIO_InitStructure.GPIO_Pin = EPD_MOSI_PIN | EPD_CLK_PIN | EPD_CS_PIN | EPD_DC_PIN | EPD_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(EPD_OUT_PORT, &GPIO_InitStructure);

    //BUSY
    GPIO_InitStructure.GPIO_Pin = EPD_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(EPD_BUSY_PORT, &GPIO_InitStructure);
}

//写入1字节
void EPD_Write(uint8_t data)
{
    for(uint8_t i=0; i<8; i++)
    {
        EPD_CLK_CLR;
        if(data & 0x80)
        {
            EPD_MOSI_SET;
        }
        else
        {
            EPD_MOSI_CLR;
        }
        data <<= 1;
        EPD_CLK_SET;
    }
}
//写入1字节命令
void EPD_WriteCMD(uint8_t cmd)
{
    EPD_CS_CLR;
    EPD_DC_CLR;
    EPD_Write(cmd);
    EPD_CS_SET;
}
//写入1字节数据
void EPD_WriteDATA(uint8_t data)
{
    EPD_CS_CLR;
    EPD_DC_SET;
    EPD_Write(data);
    EPD_CS_SET;
}
//等待退出忙碌，低电平表示忙碌
void EPD_CheckBusy(void)
{
    while(1)   
    {
        if(EPD_BUSY_READ == 1) break;
    }
}

//不准但能用
void EPD_Delay(uint32_t ms)
{
    volatile uint32_t i;
    // 简单延时，96MHz下大约每毫秒需要96000次循环
    while(ms--)
    {
        for(i = 0; i < 96000; i++);
    }
}