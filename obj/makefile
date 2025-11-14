################################################################################
# MRS Version: 1.9.2
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Peripheral/inc/subdir.mk
-include LiteOS/utils/subdir.mk
-include LiteOS/third_party/bounds_checking_function/src/subdir.mk
-include LiteOS/kernel/src/mm/subdir.mk
-include LiteOS/kernel/src/subdir.mk
-include LiteOS/kernel/arch/risc-v/V4A/gcc/subdir.mk
-include LiteOS/components/power/subdir.mk
-include LiteOS/components/cpup/subdir.mk
-include LiteOS/components/backtrace/subdir.mk
-include Fonts/subdir.mk
-include Device/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
WCH-CH32V307VCT6-Eink-LiteOS.hex \

SECONDARY_LIST += \
WCH-CH32V307VCT6-Eink-LiteOS.lst \

SECONDARY_SIZE += \
WCH-CH32V307VCT6-Eink-LiteOS.siz \


# 所有目标
all: WCH-CH32V307VCT6-Eink-LiteOS.elf secondary-outputs

# 工具调用
WCH-CH32V307VCT6-Eink-LiteOS.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -T "D:\WorkingProject\WCH-CH32V307VCT6-Eink-LiteOS\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"WCH-CH32V307VCT6-Eink-LiteOS.map" --specs=nano.specs --specs=nosys.specs -o "WCH-CH32V307VCT6-Eink-LiteOS.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
WCH-CH32V307VCT6-Eink-LiteOS.hex: WCH-CH32V307VCT6-Eink-LiteOS.elf
	@	riscv-none-embed-objcopy -O ihex "WCH-CH32V307VCT6-Eink-LiteOS.elf"  "WCH-CH32V307VCT6-Eink-LiteOS.hex"
	@	@
WCH-CH32V307VCT6-Eink-LiteOS.lst: WCH-CH32V307VCT6-Eink-LiteOS.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "WCH-CH32V307VCT6-Eink-LiteOS.elf" > "WCH-CH32V307VCT6-Eink-LiteOS.lst"
	@	@
WCH-CH32V307VCT6-Eink-LiteOS.siz: WCH-CH32V307VCT6-Eink-LiteOS.elf
	@	riscv-none-embed-size --format=berkeley "WCH-CH32V307VCT6-Eink-LiteOS.elf"
	@	@
# 其他目标
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) WCH-CH32V307VCT6-Eink-LiteOS.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
