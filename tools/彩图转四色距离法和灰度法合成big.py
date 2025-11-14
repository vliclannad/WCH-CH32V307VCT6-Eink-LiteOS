from pathlib import Path

import numpy as np
from PIL import Image
from tqdm import tqdm
import random

COLORS = {
    "black": ([0, 0, 0],128),
    "white": ([255, 255, 255],128),
    "red": ([255, 0, 0],224),
    "green": ([0, 255, 0],230),
    "blue": ([0, 0, 255],230),
    "yellow": ([255, 255, 0],230),
    "magenta": ([255, 0, 255],230),
    "cyan": ([0, 255, 255],230),
    "dark_yellow":([56, 56, 0],200),
}

def floyd_steinberg_dither(image, threshold=128):
    """带误差扩散的Floyd-Steinberg抖动算法"""
    kernel = np.array([[0, 0, 7],
                       [3, 5, 1]]) / 16
    error = np.zeros_like(image, dtype=np.float32)

    img_array = np.array(image, dtype=np.float32)
    height, width = img_array.shape

    for y in range(height):
        for x in range(width):
            old_pixel = img_array[y, x] + error[y, x]
            new_pixel = 0 if old_pixel < threshold else 255
            img_array[y, x] = new_pixel
            quant_error = old_pixel - new_pixel

            # 扩散误差到相邻像素
            if x < width - 1:
                error[y, x + 1] += quant_error * 7 / 16
            if y < height - 1:
                if x > 0:
                    error[y + 1, x - 1] += quant_error * 3 / 16
                error[y + 1, x] += quant_error * 5 / 16
                if x < width - 1:
                    error[y + 1, x + 1] += quant_error * 1 / 16
    return img_array.astype(np.uint8)


def rgb_to_tricolor_distance(pixel,three_color ="red",four_color ="yellow"):
    r, g, b = pixel
    d_black = np.sqrt(r ** 2 + g ** 2 + b ** 2)
    d_white = np.sqrt((255 - r) ** 2 + (255 - g) ** 2 + (255 - b) ** 2)
    d_three_color = np.sqrt((COLORS[three_color][0][0] - r) ** 2 + (COLORS[three_color][0][1] - g) ** 2 + (COLORS[three_color][0][2] - b) ** 2)
    d_four_color = np.sqrt((COLORS[four_color][0][0] - r) ** 2 + (COLORS[four_color][0][1] - g) ** 2 + (COLORS[four_color][0][2] - b) ** 2)
    return d_black, d_white, d_three_color,d_four_color


def tricolor_dither(image, gamma=1.0, yellow_bias=1.0,three_color ="red",four_color = "yellow"):
    img_array = np.array(image, dtype=np.float32)
    height, width, _ = img_array.shape
    # 初始化记录矩阵：0=黑，1=白，2=红，3=黄
    color_choice = np.zeros((height, width), dtype=np.uint8)

    for y in range(height):
        for x in range(width):
            r, g, b = img_array[y, x]
            d_black, d_white, d_three_color, d_four_color = rgb_to_tricolor_distance([r, g, b],three_color=three_color,four_color=four_color)
            d_three_color = d_three_color * gamma * yellow_bias
            d_four_color = d_four_color * gamma * yellow_bias

            min_dist = min(d_black, d_white, d_three_color, d_four_color)
            if min_dist == d_black:
                chosen_color = np.array([0, 0, 0])
                color_choice[y, x] = 3  # 标记为黑色
            elif min_dist == d_white:
                chosen_color = np.array([255, 255, 255])
                color_choice[y, x] = 0  # 标记为白色
            elif min_dist == d_three_color:
                chosen_color = np.array(COLORS[three_color][0])
                color_choice[y, x] = 2  # 标记为红色
            else:
                chosen_color = np.array(COLORS[four_color][0])
                color_choice[y, x] = 1  # 标记为黄色


            error = img_array[y, x] - chosen_color
            if x < width - 1:
                img_array[y, x + 1] += error * 1 / 8
            if x < width - 2:
                img_array[y, x + 2] += error * 1 / 8
            if y < height - 1:
                if x > 0:
                    img_array[y + 1, x - 1] += error * 1 / 8
                img_array[y + 1, x] += error * 1 / 8
                if x < width - 1:
                    img_array[y + 1, x + 1] += error * 1 / 8
            if y < height - 2:
                img_array[y + 2, x] += error * 1 / 8

            # error[y, x + 1] += diff * 1 / 8
            # error[y, x + 2] += diff * 1 / 8
            # error[y + 1, x - 1] += diff * 1 / 8
            # error[y + 1, x] += diff * 1 / 8
            # error[y + 1, x + 1] += diff * 1 / 8
            # error[y + 2, x] += diff * 1 / 8

            img_array[y, x] = chosen_color  # 新增强制约束

    return img_array.astype(np.uint8), color_choice

def pixels_to_byte(pixel_group, bit_order='big'):
    """ 将4个像素的二进制码组合为1个字节 """
    binary_str = ''.join(pixel_group)
    return int(binary_str, 2).to_bytes(1, byteorder=bit_order)

def image_array_to_c_array(image_array, array_name, bit_order='big'):
    """将图像数据转换为C语言数组"""
    COLOR_MAP = {
        (0, 0, 0): "00",  # 黑 (R,G,B=0,0,0)
        (255, 255, 255): "01",  # 白 (R,G,B=255,255,255)
        (255, 255, 0): "10",  # 黄 (R,G,B=255,255,0)
        (255, 0, 0): "11"  # 红 (R,G,B=255,0,0)
    }

    # print(image_array.shape, type(image_array))

    r = image_array[:, :, 0].flatten()
    g = image_array[:, :, 1].flatten()
    b = image_array[:, :, 2].flatten()

    # 创建颜色编码映射函数
    color_to_bits = np.vectorize(lambda r, g, b: COLOR_MAP.get((r, g, b), "00"), otypes=[object])

    # 将整个图像转换为二进制码数组
    binary_array = color_to_bits(r, g, b)

    # 将像素按4个一组打包为字节（自动处理非4倍数宽的情况）
    byte_data = b''.join([
        pixels_to_byte(binary_array[i:i + 4])
        for i in range(0, len(binary_array), 4)
    ])


    # 使用gzip压缩
    # import gzip
    # c_array = gzip.compress(byte_data)
    # hex_str = ', '.join([f'0x{b:02X}' for b in c_array])
    c_array = byte_data
    
    # 格式化为每行16个字节的换行格式
    hex_values = [f'0x{b:02X}' for b in c_array]
    hex_lines = []
    for i in range(0, len(hex_values), 16):
        line = ', '.join(hex_values[i:i+16])
        hex_lines.append(f"  {line}")
    hex_str = ',\n'.join(hex_lines)
    
    return f"\nconst unsigned char {array_name}[{len(c_array)}] = {{\n{hex_str}\n}};\n", len(c_array)


# 等比例缩放并添加白边
def resize_with_padding(img, target_size):
    original_width, original_height = img.size
    target_width, target_height = target_size

    # 计算缩放比例
    width_ratio = target_width / original_width
    height_ratio = target_height / original_height
    ratio = min(width_ratio, height_ratio)

    new_size = (int(original_width * ratio), int(original_height * ratio))
    img = img.resize(new_size, Image.Resampling.LANCZOS)

    # 创建新画布（白色背景）
    new_img = Image.new('RGB', target_size, (255, 255, 255))
    # 计算粘贴位置（居中）
    new_img.paste(img, ((target_size[0] - new_size[0]) // 2,
                        (target_size[1] - new_size[1]) // 2))
    return new_img


def process_image(input_path, output_prefix, size=(400, 300),three_color="red",four_color="yellow"):
    image = Image.open(input_path).convert("RGB")
    # image = image.rotate(180)
    width, height = image.size
    # print(height, width, image.size)
    if size[0] > size[1]:
        if height > width:
            image = image.rotate(90, expand=True)
    else:
        if width > height:
            image = image.rotate(90, expand=True)

    resized_image = resize_with_padding(image, size)
    gray_image = resized_image.convert("L")
    gray_array = np.array(gray_image)

    dither_three_color, color_choice = tricolor_dither(resized_image, yellow_bias=1,three_color=three_color,four_color=four_color)
    dither_gray = floyd_steinberg_dither(gray_array, 196)

    dither_gray_3C = np.stack((dither_gray, dither_gray, dither_gray), axis=-1)

    hight_brightness_mask = ((gray_array > COLORS[three_color][1]) & (gray_array > COLORS[four_color][1])).astype(np.uint8)
    # hight_brightness_mask = (color_choice == 1).astype(np.uint8)
    combined_img = np.where(hight_brightness_mask[..., None], dither_gray_3C, dither_three_color).astype(np.uint8)



    bw_mask = np.all(combined_img == [0, 0, 0], axis=-1)
    bw_mask_array = np.where(bw_mask[..., None], [0, 0, 0], [255, 255, 255]).astype(np.uint8)
    # print(bw_mask_array.shape)


    three_color_mask = np.all(combined_img == COLORS[three_color][0], axis=-1)
    three_color_mask_array = np.where(three_color_mask[..., None], [0, 0, 0], [255, 255, 255]).astype(np.uint8)

    four_color_mask = np.all(combined_img == COLORS[four_color][0], axis=-1)
    four_color_mask_array = np.where(four_color_mask[..., None], [0, 0, 0], [255, 255, 255]).astype(np.uint8)


    # 保存最终三色结果
    # Image.fromarray(dither_three_color).save(f"{output_prefix}_color.png")
    # resized_image.save(f"{output_prefix}_original.png")
    # Image.fromarray(dither_gray).convert("1").save(f"{output_prefix}_gray.png")
    # Image.fromarray(combined_img).save(f"{output_prefix}_combined.png")
    # Image.fromarray(bw_mask_array).convert("L").save(f"{output_prefix}_bw_mask.png")
    # Image.fromarray(yellow_mask_array).convert("L").save(f"{output_prefix}_{three_color}_mask.png")

    Image.fromarray(dither_three_color).save(f"{output_prefix}_color.jpg")
    resized_image.save(f"{output_prefix}_original.jpg")
    Image.fromarray(dither_gray).convert("1").save(f"{output_prefix}_gray.jpg")
    Image.fromarray(combined_img).save(f"{output_prefix}_combined.jpg")
    Image.fromarray(bw_mask_array).convert("L").save(f"{output_prefix}_bw_mask.jpg")
    Image.fromarray(three_color_mask_array).convert("L").save(f"{output_prefix}_{three_color}_mask.jpg")
    Image.fromarray(four_color_mask_array).convert("L").save(f"{output_prefix}_{four_color}_mask.jpg")

    return combined_img, three_color_mask

if __name__ == "__main__":
    # input_dir = Path(r"E:\images\pic_big_head")

    input_dir = Path(r"C:\Users\11\Desktop\4色转换\software")
    three_color = "red"
    four_color = "yellow"
    # three_color = "dark_yellow"
    # three_color = "dark_yellow"
    size = (128, 250)
    # size = (640, 384)
    # size = (400, 300)
    # size = (240, 416)
    #size = (800,480)
    output_dir = input_dir.parent / (input_dir.name +   f"big_{three_color}_{four_color}_{size[0]}x{size[1]}_output_jpg")
    image_c_array_dir = input_dir.parent / (input_dir.name +f"big_{three_color}_{four_color}_{size[0]}x{size[1]}_c_arrays")
    print(output_dir)
    print(image_c_array_dir)
    output_cfile = image_c_array_dir / "bigImageData.c"
    output_hfile = image_c_array_dir / "bigImageData.h"

    output_cstring = ""
    output_hstring = ""
    output_black_array = ""
    output_red_array = ""

    output_black_size_array = ""
    output_red_size_array = ""

    output_hstring += "#ifndef _BIGIMAGE_DATA_H\n"
    output_hstring += "#define _BIGIMAGE_DATA_H\n\n"

    output_black_array += "const unsigned char* big_images[] = {\n"


    output_black_size_array += "const unsigned int big_image_sizes[] = {\n"




    output_dir.mkdir(exist_ok=True)
    image_c_array_dir.mkdir(exist_ok=True)

    max_workers = 20

    files = list(input_dir.glob("*.jpg"))
    files_count = len(files)
    if files_count > max_workers:
        files_selected = random.sample(files, max_workers)
    else:
        files_selected = files

    random.shuffle(files_selected)
    total_size = 0

    pbar = tqdm(total=len(files_selected))
    for index,input_path in enumerate(files_selected):

        output_prefix = output_dir / f'P{index:03d}'
        color_image, yellow_mask = process_image(input_path, output_prefix, size=size, three_color=three_color)

        bw_name = f"BP{index:03d}bw"


        bw_c_array,bw_array_size = image_array_to_c_array(color_image, bw_name)




        output_cstring += bw_c_array


        output_black_array += f"    {bw_name},\n"


        output_black_size_array += f" {bw_array_size},\n"

        total_size += bw_array_size

        pbar.set_postfix_str(f'Processed {input_path.name},bw_size={bw_array_size},total_size={total_size}')

        output_hstring += f"extern const unsigned char {bw_name}[];\n"




        pbar.update(1)

    output_black_array += "};\n\n"
    # output_red_array += "};\n\n"

    output_black_size_array += "};\n\n"
    # output_red_size_array += "};\n\n"

    output_hstring += "extern const unsigned char*  big_images[];\n"
    # output_hstring += output_black_array

    # output_hstring += output_red_array
    output_hstring += f"extern const unsigned int big_image_sizes[];\n"


    output_hstring += f"#define BIGIMAGE_COUNT {len(files_selected)}\n\n"
    output_hstring += f"#define BIGIMAGE_WIDTH {size[0]}\n"
    output_hstring += f"#define BIGIMAGE_HEIGHT {size[1]}\n\n"

    output_hstring += "#endif // _BIGIMAGE_DATA_H\n"

    with open(output_cfile, "w") as f:
        f.write('#include "bigImageData.h"\n\n')
        f.write(output_cstring)
        f.write("\n")
        f.write(output_black_array)
        f.write(output_red_array)
        f.write(output_black_size_array)
        f.write(output_red_size_array)

    with open(output_hfile, "w") as f:
        # f.write(output_cstring)
        f.write(output_hstring)
        f.write("\n")

    pbar.close()
