################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Device/EPD.c \
../Device/EPD_SPI.c \
../Device/ESP8266.c \
../Device/GUI_Paint.c 

OBJS += \
./Device/EPD.o \
./Device/EPD_SPI.o \
./Device/ESP8266.o \
./Device/GUI_Paint.o 

C_DEPS += \
./Device/EPD.d \
./Device/EPD_SPI.d \
./Device/ESP8266.d \
./Device/GUI_Paint.d 


# Each subdirectory must supply rules for building sources it contributes
Device/%.o: ../Device/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Debug" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Core" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\User" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Peripheral\inc" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\backtrace" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\cpup" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\power" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\arch\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\arch\risc-v\V4A\gcc" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\src" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\src\mm" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\utils\internal" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\utils" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\third_party\bounds_checking_function\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\third_party\bounds_checking_function\src" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Device" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Fonts" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

