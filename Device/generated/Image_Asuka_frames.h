/*
 * 图像帧数据头文件 - Image_Asuka
 * 自动生成于: 2025-11-14 16:13:36
 */

#ifndef __IMAGE_ASUKA_FRAMES_H__
#define __IMAGE_ASUKA_FRAMES_H__

#include <stdint.h>

// 总帧数
#define IMAGE_ASUKA_FRAME_COUNT 8

// 帧指针数组
extern const uint8_t* Image_Asuka_frames[8];

// 帧长度数组
extern const uint16_t Image_Asuka_frame_sizes[8];

/**
 * @brief 发送所有帧数据
 * @param send_func 串口发送函数指针
 * @param delay_func 延时函数指针
 */
void Send_Image_Asuka_Frames(
    void (*send_func)(const uint8_t* data, uint16_t len),
    void (*delay_func)(uint32_t ms));

#endif /* __IMAGE_ASUKA_FRAMES_H__ */
