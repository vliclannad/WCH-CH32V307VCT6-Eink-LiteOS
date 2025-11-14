#ifndef __USER_H__
#define __USER_H__
#include "debug.h"
#include "los_event.h"
#include "ch32v30x_it.h"  // 引入帧数据结构定义

//定义事件标志组的控制块
#define WIFI_EVENT_RECV    0x01  // WiFi接收事件

// 帧协议定义
#define FRAME_HEADER1       0xA5
#define FRAME_HEADER2       0x5A

#define CMD_TYPE_IMAGE      0xD1  //命令类型
#define CMD_IMAGE_TRANSFER  0x01  //图像传输命令

#define MAX_DATA_LEN        1024    // 最大有效数据长度

// 接收状态机
typedef enum {
    RECV_STATE_HEADER1 = 0,     // 等待帧头1
    RECV_STATE_HEADER2,         // 等待帧头2
    RECV_STATE_CMD_TYPE,        // 等待命令类型
    RECV_STATE_CMD,             // 等待命令
    RECV_STATE_LEN_HIGH,        // 等待数据长度高字节
    RECV_STATE_LEN_LOW,         // 等待数据长度低字节
    RECV_STATE_DATA,            // 接收数据
    RECV_STATE_CRC_HIGH,        // 等待CRC高字节
    RECV_STATE_CRC_LOW          // 等待CRC低字节
} RecvState_t;

// 帧数据结构
typedef struct {
    uint8_t cmd_type;           // 命令类型
    uint8_t cmd;                // 命令
    uint16_t data_len;          // 有效数据长度
<<<<<<< HEAD
    uint8_t data_buffer[MAX_DATA_LEN]; // 数据缓冲区(不包含分包信息)
=======
    uint16_t data_index;        // 当前数据索引
    uint8_t data_buffer[MAX_DATA_LEN-2]; // 数据缓冲区(不包含分包信息)
>>>>>>> 46d6483c1bfe9628b6953e5b2c01064a49835570
    uint8_t total_packets;      // 分包总数
    uint8_t current_packet;     // 当前包序号
    uint16_t crc_received;      // 接收到的CRC值
} FrameData_t;




// 全局变量
extern EVENT_CB_S gWifiEventGroup;
extern FrameData_t g_frame_data;
extern uint8_t WIFI_CMD[128];

/* 函数声明 */
void thread_Eink_Start(void);    
void thread_WIFI_Start(void);

#endif /* __USER_H__ */
