#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
图像帧分包工具
功能：将image.h中的图像数组按照帧协议进行组帧分包，并生成.c文件
作者：AI Assistant
日期：2025-11-07
"""

import re
import os
import sys
from datetime import datetime

# 帧协议定义
FRAME_HEADER1 = 0xA5
FRAME_HEADER2 = 0x5A                               
CMD_TYPE_IMAGE = 0xD1
CMD_IMAGE_TRANSFER = 0x01
<<<<<<< HEAD
MAX_IMAGE_DATA_LEN = 1022  # 最大图片数据长度（总包数1 + 包序号1 + 图像数据1022 = 1024字节）


def crc16_calculate(data):
    """
    CRC-16校验计算函数(MODBUS多项式)
    
    Args:
        data: 字节列表或字节串
    
    Returns:
        int: CRC校验值
    """
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc 
=======
MAX_IMAGE_DATA_LEN = 1022  # 最大图片数据长度(1024-2) 
>>>>>>> 46d6483c1bfe9628b6953e5b2c01064a49835570


def extract_image_array(file_path):
    """
    从image.h文件中提取图像数组
    
    Args:
        file_path: image.h文件路径
    
    Returns:
        dict: {'name': 数组名称, 'data': 字节数据列表, 'size': 数据大小}
    """
    print(f"正在读取文件: {file_path}")
    
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # 匹配图像数组定义
    pattern = r'const\s+unsigned\s+char\s+(\w+)\[(\d+)\]\s*=\s*\{([^}]+)\}'
    matches = re.findall(pattern, content, re.DOTALL)
    
    if not matches:
        raise ValueError("未找到图像数组定义")
    
    images = []
    for match in matches:
        name = match[0]
        size = int(match[1])
        data_str = match[2]
        
        # 提取所有十六进制数值
        hex_values = re.findall(r'0x([0-9A-Fa-f]{2})', data_str)
        data = [int(x, 16) for x in hex_values]
        
        print(f"找到图像数组: {name}, 大小: {size} 字节, 实际数据: {len(data)} 字节")
        
        images.append({
            'name': name,
            'size': size,
            'data': data
        })
    
    return images


def create_frame(total_packets, packet_num, data):
    """
    创建一帧数据
    
    Args:
        total_packets: 总包数
        packet_num: 当前包序号(从1开始)
        data: 图片数据(字节列表)
    
    Returns:
        list: 完整的帧数据(字节列表)
    """
    frame = []
    
    # 1. 帧头
    frame.append(FRAME_HEADER1)
    frame.append(FRAME_HEADER2)
    
    # 2. 命令类型
    frame.append(CMD_TYPE_IMAGE)
    
    # 3. 命令
    frame.append(CMD_IMAGE_TRANSFER)
    
    # 4. 有效数据长度(高字节在前) 
    # 长度 = 总包数(1) + 包序号(1) + 图片数据(可变) = 最大1024字节
    payload_len = len(data) + 2  # +2 for 总包数和包序号（不包含CRC）
    frame.append((payload_len >> 8) & 0xFF)
    frame.append(payload_len & 0xFF)
    
    # 5. 有效数据：总包数
    frame.append(total_packets)
    
    # 6. 有效数据：包序号
    frame.append(packet_num)
    
    # 7. 有效数据：图片数据
    frame.extend(data)
    
    # 8. 计算CRC（对命令类型、命令、长度和有效数据进行计算）
    crc_data = [CMD_TYPE_IMAGE, CMD_IMAGE_TRANSFER, (payload_len >> 8) & 0xFF, payload_len & 0xFF]
    crc_data.extend(frame[6:])  # 添加有效数据（总包数、包序号、图片数据）
    crc_value = crc16_calculate(crc_data)
    
    # 9. 添加CRC（高字节在前）
    frame.append((crc_value >> 8) & 0xFF)
    frame.append(crc_value & 0xFF)
    
    return frame


def pack_image_to_frames(image_data):
    """
    将图像数据打包成帧
    
    Args:
        image_data: 图像字节数据列表
    
    Returns:
        list: 帧数据列表，每个元素是一帧(字节列表)
    """
    total_size = len(image_data)
    total_packets = (total_size + MAX_IMAGE_DATA_LEN - 1) // MAX_IMAGE_DATA_LEN
    
    print(f"图像大小: {total_size} 字节")
    print(f"分包数量: {total_packets}")
    
    frames = []
    for i in range(total_packets):
        packet_num = i + 1
        offset = i * MAX_IMAGE_DATA_LEN
        data_len = min(MAX_IMAGE_DATA_LEN, total_size - offset)
        packet_data = image_data[offset:offset + data_len]
        
        frame = create_frame(total_packets, packet_num, packet_data)
        frames.append(frame)
        
        print(f"  包 {packet_num}/{total_packets}: 图片数据 {data_len} 字节, 帧总长度 {len(frame)} 字节")
    
    return frames


def generate_hex_frames_file(image_name, frames, output_path):
    """
    生成分包的十六进制字符串文件
    
    Args:
        image_name: 图像名称
        frames: 帧数据列表
        output_path: 输出文件路径
    """
    txt_file = os.path.join(output_path, f"{image_name}_frames.txt")
    print(f"生成十六进制分包文件: {txt_file}")
    
    with open(txt_file, 'w') as f:
        # 为每个包生成一行十六进制字符串
        for i, frame in enumerate(frames):
            hex_str = ''.join([f"{byte:02X}" for byte in frame])
            f.write(f"Frame {i+1:02d}: {hex_str}\n")
            print(f"  包 {i+1}: 写入 {len(frame)} 字节")


def generate_c_file(image_name, frames, output_path):
    """
    生成包含分帧数据的C文件
    
    Args:
        image_name: 图像名称
        frames: 帧数据列表
        output_path: 输出文件路径
    """
    c_file = os.path.join(output_path, f"{image_name}_frames.c")
    print(f"生成C文件: {c_file}")
    
    with open(c_file, 'w', encoding='utf-8') as f:
        # 文件头注释
        f.write(f"/*\n")
        f.write(f" * 图像帧数据 - {image_name}\n")
        f.write(f" * 自动生成于: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f" * 总帧数: {len(frames)}\n")
        f.write(f" * 帧协议: 帧头(0xA5 0x5A) + 命令类型(0xD1) + 命令(0x01) + 长度(2字节) + 数据\n")
        f.write(f" */\n\n")
        
        f.write(f"#include <stdint.h>\n\n")
        
        # 写入帧数量定义
        f.write(f"// 总帧数\n")
        f.write(f"#define {image_name.upper()}_FRAME_COUNT {len(frames)}\n\n")
        
        # 写入每一帧的数据
        for i, frame in enumerate(frames):
            frame_num = i + 1
            f.write(f"// 帧 {frame_num}/{len(frames)}\n")
            f.write(f"const uint8_t {image_name}_frame_{frame_num}[{len(frame)}] = {{\n")
            
            # 每行16个字节
            for j in range(0, len(frame), 16):
                line_data = frame[j:j+16]
                hex_str = ', '.join([f"0x{byte:02X}" for byte in line_data])
                f.write(f"    {hex_str}")
                if j + 16 < len(frame):
                    f.write(",")
                f.write("\n")
            
            f.write(f"}};\n\n")
        
        # 写入帧指针数组
        f.write(f"// 帧指针数组\n")
        f.write(f"const uint8_t* {image_name}_frames[{len(frames)}] = {{\n")
        for i in range(len(frames)):
            frame_num = i + 1
            f.write(f"    {image_name}_frame_{frame_num}")
            if i < len(frames) - 1:
                f.write(",")
            f.write(f"  // 帧 {frame_num}\n")
        f.write(f"}};\n\n")
        
        # 写入帧长度数组
        f.write(f"// 帧长度数组\n")
        f.write(f"const uint16_t {image_name}_frame_sizes[{len(frames)}] = {{\n")
        for i, frame in enumerate(frames):
            f.write(f"    {len(frame)}")
            if i < len(frames) - 1:
                f.write(",")
            f.write(f"  // 帧 {i+1}\n")
        f.write(f"}};\n\n")
        
        # 写入发送函数
        f.write(f"/*\n")
        f.write(f" * 发送所有帧的示例函数\n")
        f.write(f" * 使用方法：调用此函数并传入UART发送函数指针和延时函数指针\n")
        f.write(f" */\n")
        f.write(f"void Send_{image_name}_Frames(\n")
        f.write(f"    void (*send_func)(const uint8_t* data, uint16_t len),\n")
        f.write(f"    void (*delay_func)(uint32_t ms))\n")
        f.write(f"{{\n")
        f.write(f"    for(int i = 0; i < {image_name.upper()}_FRAME_COUNT; i++) {{\n")
        f.write(f"        send_func({image_name}_frames[i], {image_name}_frame_sizes[i]);\n")
        f.write(f"        if(i < {image_name.upper()}_FRAME_COUNT - 1) {{\n")
        f.write(f"            delay_func(10);  // 每帧间隔10ms\n")
        f.write(f"        }}\n")
        f.write(f"    }}\n")
        f.write(f"}}\n")


def generate_h_file(image_name, frame_count, output_path):
    """
    生成对应的头文件
    
    Args:
        image_name: 图像名称
        frame_count: 帧数量
        output_path: 输出文件路径
    """
    h_file = os.path.join(output_path, f"{image_name}_frames.h")
    print(f"生成头文件: {h_file}")
    
    guard_name = f"__{image_name.upper()}_FRAMES_H__"
    
    with open(h_file, 'w', encoding='utf-8') as f:
        f.write(f"/*\n")
        f.write(f" * 图像帧数据头文件 - {image_name}\n")
        f.write(f" * 自动生成于: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f" */\n\n")
        
        f.write(f"#ifndef {guard_name}\n")
        f.write(f"#define {guard_name}\n\n")
        
        f.write(f"#include <stdint.h>\n\n")
        
        f.write(f"// 总帧数\n")
        f.write(f"#define {image_name.upper()}_FRAME_COUNT {frame_count}\n\n")
        
        f.write(f"// 帧指针数组\n")
        f.write(f"extern const uint8_t* {image_name}_frames[{frame_count}];\n\n")
        
        f.write(f"// 帧长度数组\n")
        f.write(f"extern const uint16_t {image_name}_frame_sizes[{frame_count}];\n\n")
        
        f.write(f"/**\n")
        f.write(f" * @brief 发送所有帧数据\n")
        f.write(f" * @param send_func 串口发送函数指针\n")
        f.write(f" * @param delay_func 延时函数指针\n")
        f.write(f" */\n")
        f.write(f"void Send_{image_name}_Frames(\n")
        f.write(f"    void (*send_func)(const uint8_t* data, uint16_t len),\n")
        f.write(f"    void (*delay_func)(uint32_t ms));\n\n")
        
        f.write(f"#endif /* {guard_name} */\n")


def main():
    """主函数"""
    print("=" * 60)
    print("图像帧分包工具")
    print("=" * 60)
    
    # 默认路径
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_dir = os.path.dirname(script_dir)
    input_file = os.path.join(project_dir, 'Device', 'image.h')
    output_dir = os.path.join(project_dir, 'Device', 'generated')
    
    # 如果有命令行参数，使用命令行参数
    if len(sys.argv) > 1:
        input_file = sys.argv[1]
    if len(sys.argv) > 2:
        output_dir = sys.argv[2]
    
    # 检查输入文件是否存在
    if not os.path.exists(input_file):
        print(f"错误: 输入文件不存在: {input_file}")
        return 1
    
    # 创建输出目录
    os.makedirs(output_dir, exist_ok=True)
    print(f"输出目录: {output_dir}\n")
    
    try:
        # 1. 提取图像数组
        images = extract_image_array(input_file)
        
        # 2. 处理每个图像
        for image_info in images:
            print(f"\n处理图像: {image_info['name']}")
            print("-" * 60)
            
            # 2.1 分包
            frames = pack_image_to_frames(image_info['data'])
            
            # 2.2 生成十六进制分包文件
            generate_hex_frames_file(image_info['name'], frames, output_dir)
            
            # 2.3 生成C文件和头文件
            generate_c_file(image_info['name'], frames, output_dir)
            generate_h_file(image_info['name'], len(frames), output_dir)
            
            print(f"✓ 完成")
        
        print("\n" + "=" * 60)
        print("所有图像处理完成!")
        print("=" * 60)
        return 0
        
    except Exception as e:
        print(f"\n错误: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == '__main__':
    sys.exit(main())
