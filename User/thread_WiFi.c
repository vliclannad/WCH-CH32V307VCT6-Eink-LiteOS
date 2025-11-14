#include "user.h"
#include "los_task.h"
#include "ESP8266.h"
#include "ch32v30x_it.h"
#include "EPD.h"

extern EVENT_CB_S gWifiEventGroup;

// 处理接收到的图像数据
void ProcessImageData(uint8_t total_packets, uint8_t packet_num, const uint8_t* data, uint16_t len)
{
    static uint8_t last_packet = 0;
    
    printf("接收图像包：%d/%d, 长度: %d\r\n", packet_num, total_packets, len);
    
    if(packet_num == 1)
    {
        printf("第一包图像,唤醒屏幕\r\n");
        EPD_Init_Fast();
    }

    // 发送到墨水屏
    EPD_DisplayImage_Paragraph(data, len, packet_num);
    
    if(packet_num == total_packets)
    {
        printf("接收完成,开始刷新屏幕\r\n");
        EPD_Update();
        EPD_Sleep();
        last_packet = 0;
    }
    else
    {
        last_packet = packet_num;
    }
}

// 处理一帧完整数据
void ProcessFrame(FrameData_t* frame)
{
    switch(frame->cmd_type)
    {
        case CMD_TYPE_IMAGE:
            switch(frame->cmd)
            {
                case CMD_IMAGE_TRANSFER:
                    if(frame->data_len > 2)
                    {
                        uint8_t total_packets = frame->data_buffer[0];
                        uint8_t packet_num = frame->data_buffer[1];
                        uint16_t image_data_len = frame->data_len - 2;
                        ProcessImageData(total_packets, packet_num, &frame->data_buffer[2], image_data_len);
                    }
                    break;
            }
            break;
        
        default:
            printf("未知命令类型: %02X\r\n", frame->cmd_type);
            break;
    }
}

void thread_WIFI_Start(void)
{

    UINT32 uwRet;
     // 创建事件标志组
    printf("WIFI thread started.\n");
    //等待WIFI模块上电稳定
    LOS_TaskDelay(2000); // Delay for 2 seconds
     // 初始化接收状态机
    RecvFrame_Init();
    WIFI_Init(WIFI_STATION);
    printf("WIFI Initialized.\n");
    if(WIFI_LinkSSID((uint8_t *)"1302",(uint8_t *)"xiyukeji13+02") == WIFI_OK)
    {
        printf("Connected to WiFi successfully.\n");
    }
    else
    {
        printf("Failed to connect to WiFi.\n");
    }
    WIFI_TCPConnect((uint8_t *)"192.168.3.66", (uint8_t *)"11451");
    WIFI_SetTxMode(WIFI_PASS_THROUGH);//设置透传模式
    while(1)
    {
        // 等待接收事件
       uwRet = LOS_EventRead(&gWifiEventGroup, WIFI_EVENT_RECV, LOS_WAITMODE_AND|LOS_WAITMODE_CLR,LOS_WAIT_FOREVER);
        if (uwRet == WIFI_EVENT_RECV)
        {
            // 处理接收到的数据
            ProcessFrame(&g_frame_data);
        }

    }
}



