#ifndef __EPD_H
#define __EPD_H

#include "debug.h"


//屏幕信息
#define EPD_WIDTH       128
#define EPD_HEIGHT      250


#define EPD_BUFFER_SIZE (EPD_WIDTH * EPD_HEIGHT / 4)



//2BIT
#define EPD_COLOR_BLACK   0x00
#define EPD_COLOR_WHITE   0x01
#define EPD_COLOR_YELLOW  0x02
#define EPD_COLOR_RED     0x03






void EPD_Init(void);
void EPD_Init_Fast(void);
void EPD_Clean(uint8_t color);
void EPD_DisplayImage(const uint8_t* image);
void EPD_Sleep(void);
void EPD_DisplayImage_Paragraph(const uint8_t* picData, uint32_t len,uint8_t number);
void EPD_Update(void);
#endif
