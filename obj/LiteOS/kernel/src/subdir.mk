################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LiteOS/kernel/src/los_event.c \
../LiteOS/kernel/src/los_init.c \
../LiteOS/kernel/src/los_mux.c \
../LiteOS/kernel/src/los_queue.c \
../LiteOS/kernel/src/los_sched.c \
../LiteOS/kernel/src/los_sem.c \
../LiteOS/kernel/src/los_sortlink.c \
../LiteOS/kernel/src/los_swtmr.c \
../LiteOS/kernel/src/los_task.c \
../LiteOS/kernel/src/los_tick.c 

OBJS += \
./LiteOS/kernel/src/los_event.o \
./LiteOS/kernel/src/los_init.o \
./LiteOS/kernel/src/los_mux.o \
./LiteOS/kernel/src/los_queue.o \
./LiteOS/kernel/src/los_sched.o \
./LiteOS/kernel/src/los_sem.o \
./LiteOS/kernel/src/los_sortlink.o \
./LiteOS/kernel/src/los_swtmr.o \
./LiteOS/kernel/src/los_task.o \
./LiteOS/kernel/src/los_tick.o 

C_DEPS += \
./LiteOS/kernel/src/los_event.d \
./LiteOS/kernel/src/los_init.d \
./LiteOS/kernel/src/los_mux.d \
./LiteOS/kernel/src/los_queue.d \
./LiteOS/kernel/src/los_sched.d \
./LiteOS/kernel/src/los_sem.d \
./LiteOS/kernel/src/los_sortlink.d \
./LiteOS/kernel/src/los_swtmr.d \
./LiteOS/kernel/src/los_task.d \
./LiteOS/kernel/src/los_tick.d 


# Each subdirectory must supply rules for building sources it contributes
LiteOS/kernel/src/%.o: ../LiteOS/kernel/src/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Debug" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Core" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\User" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Peripheral\inc" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\backtrace" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\cpup" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\components\power" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\arch\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\arch\risc-v\V4A\gcc" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\src" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\kernel\src\mm" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\utils\internal" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\utils" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\third_party\bounds_checking_function\include" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\LiteOS\third_party\bounds_checking_function\src" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Device" -I"D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Fonts" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

