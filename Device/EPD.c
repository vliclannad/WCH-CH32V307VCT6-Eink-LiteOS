#include "EPD.h"
#include "EPD_SPI.h"


void EPD_Init(void)
{

    //配置寄存器
    EPD_Delay(20);
    EPD_RST_CLR;//EPD复位
    EPD_Delay(40);
    EPD_RST_SET;
    EPD_Delay(50);
    EPD_CheckBusy();
    EPD_WriteCMD(0xE9);
    EPD_WriteDATA(0x01);
    EPD_WriteCMD(0x04);
    EPD_CheckBusy();
} 

//睡眠状态唤醒
void EPD_Init_Fast(void)
{
   EPD_Delay(100);//At least 100ms delay 	
	EPD_RST_CLR;		// Module reset
	EPD_Delay(10);//At least 10ms delay 
	EPD_RST_SET;
	EPD_Delay(10);//At least 10ms delay 
	EPD_CheckBusy();

	EPD_WriteCMD(0xe0);
	EPD_WriteDATA(0x02);

	EPD_WriteCMD(0x04);
	EPD_CheckBusy();
}
//进入休眠
void EPD_Sleep(void)
{
	EPD_WriteCMD(0X02);  	//power off
	EPD_WriteDATA(0x00);
	EPD_CheckBusy();          //waiting for the electronic paper IC to release the idle signal

	EPD_WriteCMD(0X07);  	//deep sleep
	EPD_WriteDATA(0xA5);
}
//更新显示
void EPD_Update(void)
{
    EPD_WriteCMD(0x12); //Display Update Control
	EPD_WriteDATA(0x00);
    EPD_CheckBusy();
}

//清屏
void EPD_Clean(uint8_t color)
{
    uint32_t i;
    uint8_t data = 0x00;
    switch (color)
    {
    case EPD_COLOR_WHITE:
        data = 0x55;
        break;
    case EPD_COLOR_BLACK:
        data = 0x00;
        break;
    case EPD_COLOR_YELLOW:
        data = 0xaa;
        break;
    case EPD_COLOR_RED:
        data = 0xff;
        break;
    default:
        break;
    }
    EPD_WriteCMD(0x10);
    for(i=0;i<EPD_BUFFER_SIZE;i++)
    {
        EPD_WriteDATA(data);
    }
    EPD_Update();

}
uint8_t Color_get(uint8_t color)
{
	uint8_t datas;
	switch(color)
	{
		case 0x00:  // BLACK
			datas = EPD_COLOR_BLACK;	
			break;		
		case 0x01:  // WHITE
			datas = EPD_COLOR_WHITE;
			break;
		case 0x02:  // YELLOW
			datas = EPD_COLOR_YELLOW;
			break;		
		case 0x03:  // RED
			datas = EPD_COLOR_RED;
			break;			
		default:
			datas = EPD_COLOR_WHITE;  // 默认白色
			break;			
	}
	return datas;
}
void EPD_DisplayImage(const uint8_t* picData)
{
    uint32_t i;
	uint8_t temp1;
	uint8_t data_H1,data_H2,data_L1,data_L2,data;
	 
	EPD_WriteCMD(0x10);	       
  for(i=0;i<EPD_BUFFER_SIZE;i++)  
	{
			temp1=picData[i]; 
			data_H1=Color_get(temp1>>6&0x03)<<6;			
			data_H2=Color_get(temp1>>4&0x03)<<4;
			data_L1=Color_get(temp1>>2&0x03)<<2;
			data_L2=Color_get(temp1&0x03);
			
			data=data_H1|data_H2|data_L1|data_L2;
      EPD_WriteDATA(data);
		
  }	
	 //update
    EPD_Update();
}

void EPD_DisplayImage_Paragraph(const uint8_t* picData, uint32_t len,uint8_t number)
{
	uint32_t i;
	uint8_t temp1;
	uint8_t data_H1,data_H2,data_L1,data_L2,data;
	if(number == 1) EPD_WriteCMD(0x10);      
  for(i=0;i<len;i++)  
	{
			temp1=picData[i]; 
			data_H1=Color_get(temp1>>6&0x03)<<6;			
			data_H2=Color_get(temp1>>4&0x03)<<4;
			data_L1=Color_get(temp1>>2&0x03)<<2;
			data_L2=Color_get(temp1&0x03);
			
			data=data_H1|data_H2|data_L1|data_L2;
	  EPD_WriteDATA(data);
		
  }	
}
