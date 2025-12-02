#include "EPD.h"
#include "GUI_Paint.h"
#include "user.h"
#include "image.h"
#include "los_task.h"
#include "EPD_SPI.h"
#include "stdlib.h"

typedef enum {
    BUTTON_STATE_IDLE = 0,      // 空闲状态
    BUTTON_STATE_PRESSED,       // 按下状态
    BUTTON_STATE_WAIT_RELEASE   // 等待释放状态
} ButtonState_t;

// 按钮结构体
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    ButtonState_t state;
    uint8_t debounce_counter;
} Button_t;

// 按钮实例
Button_t button1 = {GPIOE, GPIO_Pin_4, BUTTON_STATE_IDLE, 0};
Button_t button2 = {GPIOE, GPIO_Pin_5, BUTTON_STATE_IDLE, 0};

// 按钮扫描函数，返回1表示检测到有效按下
uint8_t Button_Scan(Button_t* btn)
{
    uint8_t triggered = 0;
    uint8_t current_state = GPIO_ReadInputDataBit(btn->port, btn->pin);
    
    switch(btn->state)
    {
        case BUTTON_STATE_IDLE:
            if(current_state == RESET)
            {
                btn->debounce_counter = 0;
                btn->state = BUTTON_STATE_PRESSED;
            }
            break;
            
        case BUTTON_STATE_PRESSED:
            if(current_state == RESET)
            {
                btn->debounce_counter++;
                if(btn->debounce_counter >= 2)
                {
                    triggered = 1;
                    btn->state = BUTTON_STATE_WAIT_RELEASE;
                    btn->debounce_counter = 0;
                }
            }
            else
            {
                btn->state = BUTTON_STATE_IDLE;
            }
            break;
            
        case BUTTON_STATE_WAIT_RELEASE:
            if(current_state == SET)
            {
                btn->debounce_counter++;
                if(btn->debounce_counter >= 2)
                {
                    btn->state = BUTTON_STATE_IDLE;
                    btn->debounce_counter = 0;
                }
            }
            else
            {
                btn->debounce_counter = 0;
            }
            break;
    }
    
    return triggered;
}


void Button_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void Display_First(uint8_t *image)
{

    Paint_NewImage(image, EPD_WIDTH, EPD_HEIGHT, ROTATE_270, WHITE);
    Paint_SetScale(4);  
    Paint_SelectImage(image);
    Paint_Clear(WHITE);
    Paint_DrawRectangle(2, 2, 248, 120, BLACK, DOT_PIXEL_2X2, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(5, 5, 245, 117, YELLOW, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(8, 8, 35, 114, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawString_CN(45, 15, "苏州析羽信息", &Font24CN, BLACK, WHITE);
    Paint_DrawLine(45, 58, 240, 58, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(45, 105, 240, 105, YELLOW, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
    
}



void thread_Eink_Start(void)
{
    printf("Eink thread started.\n");
    EPD_SPI_Init();
    EPD_Init();
    Button_Init();
    printf("EPD Initialized.\n");
    uint8_t *image_test = (uint8_t *)malloc(EPD_BUFFER_SIZE);
    Display_First(image_test);
    printf("First image prepared.\n");
    //EPD_DisplayImage(image_test);
    printf("First image displayed.\n");
    EPD_Sleep();
    free(image_test);
    while(1)
    {
		if(Button_Scan(&button1))
		{
           EPD_Init_Fast();//先唤醒
           EPD_DisplayImage((const uint8_t*)image_test);
		   EPD_Sleep();
		}

		if(Button_Scan(&button2))
		{
		   EPD_Init_Fast();//先唤醒
           EPD_DisplayImage((const uint8_t*)Image_Asuka);
		   EPD_Sleep();
		}

		LOS_TaskDelay(10);
	}
}
