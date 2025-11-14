################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Startup/startup_ch32v30x_D8C.S 

OBJS += \
./Startup/startup_ch32v30x_D8C.o 

S_UPPER_DEPS += \
./Startup/startup_ch32v30x_D8C.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/%.o: ../Startup/%.S
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -x assembler-with-cpp -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Startup" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\arch\risc-v\V4A\gcc" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Core" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Debug" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\power" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\cpup" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\backtrace" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\third_party\bounds_checking_function\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\third_party\bounds_checking_function\src" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\utils\internal" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\arch\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\src" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\src\mm" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

