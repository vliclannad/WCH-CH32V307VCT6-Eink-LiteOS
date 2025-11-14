################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Fonts/font12.c \
../Fonts/font12CN.c \
../Fonts/font16.c \
../Fonts/font20.c \
../Fonts/font24.c \
../Fonts/font24CN.c \
../Fonts/font8.c 

OBJS += \
./Fonts/font12.o \
./Fonts/font12CN.o \
./Fonts/font16.o \
./Fonts/font20.o \
./Fonts/font24.o \
./Fonts/font24CN.o \
./Fonts/font8.o 

C_DEPS += \
./Fonts/font12.d \
./Fonts/font12CN.d \
./Fonts/font16.d \
./Fonts/font20.d \
./Fonts/font24.d \
./Fonts/font24CN.d \
./Fonts/font8.d 


# Each subdirectory must supply rules for building sources it contributes
Fonts/%.o: ../Fonts/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Debug" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Core" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\User" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Peripheral\inc" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\backtrace" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\cpup" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\power" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\arch\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\arch\risc-v\V4A\gcc" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\src" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\src\mm" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\utils\internal" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\utils" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\third_party\bounds_checking_function\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\third_party\bounds_checking_function\src" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Device" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Fonts" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

