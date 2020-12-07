################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

-include makefile.init

RM := rm -rf *.lst *.lis *.lpp *.map

# All of the sources participating in the build are defined here
-include sources.mk
-include src/user_prog/subdir.mk
-include src/tinycrypt/lib/source/subdir.mk
-include src/tinycrypt/lib/subdir.mk
-include src/tinycrypt/subdir.mk
-include src/renesas/application/hwsetup/octabus_setup/src/subdir.mk
-include src/renesas/application/hwsetup/subdir.mk
-include src/renesas/application/console/subdir.mk
-include src/renesas/application/subdir.mk
-include gr_mango_boot/subdir.mk
-include generate/system/subdir.mk
-include generate/sc_drivers/r_scifa/src/lld/subdir.mk
-include generate/sc_drivers/r_scifa/src/hld/subdir.mk
-include generate/sc_drivers/r_ostm/src/lld/subdir.mk
-include generate/sc_drivers/r_ostm/src/hld/subdir.mk
-include generate/sc_drivers/r_octabus/src/lld/subdir.mk
-include generate/sc_drivers/r_cbuffer/src/subdir.mk
-include generate/os_abstraction/src/subdir.mk
-include generate/drivers/r_stb/src/lld/subdir.mk
-include generate/drivers/r_mmu/src/lld/subdir.mk
-include generate/drivers/r_intc/src/lld/subdir.mk
-include generate/drivers/r_gpio/src/lld/subdir.mk
-include generate/drivers/r_gpio/src/hld/subdir.mk
-include generate/drivers/r_cpg/src/lld/subdir.mk
-include generate/drivers/r_cpg/src/hld/subdir.mk
-include generate/drivers/r_cache/src/lld/subdir.mk
-include generate/configuration/subdir.mk
-include generate/compiler/init/subdir.mk
-include generate/compiler/asm/subdir.mk
-include generate/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_UPPER_DEPS)),)
-include $(CC_UPPER_DEPS)
endif
ifneq ($(strip $(C++_UPPER_DEPS)),)
-include $(C++_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_UPPER_DEPS)),)
-include $(CPP_UPPER_DEPS)
endif
ifneq ($(strip $(FSY_DEPS)),)
-include $(FSY_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
-include $(SRC_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
-include $(PP_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
-include $(JMP_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
-include $(SX_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(JMP_DEPS)),)
-include $(JMP_DEPS)
endif
ifneq ($(strip $(CXX_UPPER_DEPS)),)
-include $(CXX_UPPER_DEPS)
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
-include $(P_UPPER_DEPS)
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
-include $(FSY_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
ota_boot_proc.bin \

SECONDARY_SIZE += \
ota_boot_proc.siz \


# All Target
all:
	$(MAKE) --no-print-directory main-build
	$(MAKE) --no-print-directory post-build

# Main-build Target
main-build: ota_boot_proc.elf secondary-outputs

# Tool invocations
ota_boot_proc.elf: $(OBJS) $(USER_OBJS) $(LIBRARY_GENERATOR_OUTPUTTYPE_OUTPUTS)
	@	$(file > ota_boot_proc.elf.in,-mcpu=cortex-a9 -march=armv7-a -marm -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -O2 -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -Wnull-dereference -Wmaybe-uninitialized -g3 -Wstack-usage=100 -o "ota_boot_proc.elf" $(OBJS) $(USER_OBJS) -T "D:/Work/DeviceTester/amazon-freertos/tests/renesas/rza2m-gr-mango/e2studio/ota_boot_proc/generate/linker_script.ld"  -Wl,--start-group -Wl,--end-group -nostartfiles -Xlinker --gc-sections -Wl,-Map,"ota_boot_proc.map" -Wl,-eStart)
	@echo 'arm-none-eabi-g++ -mcpu=cortex-a9 -march=armv7-a -marm -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -O2 -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wpadded -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -Wnull-dereference -Wmaybe-uninitialized -g3 -Wstack-usage=100 -o "ota_boot_proc.elf" $(OBJS) $(USER_OBJS) -T "D:/Work/DeviceTester/amazon-freertos/tests/renesas/rza2m-gr-mango/e2studio/ota_boot_proc/generate/linker_script.ld"  -Wl,--start-group -Wl,--end-group -nostartfiles -Xlinker --gc-sections -Wl,-Map,"ota_boot_proc.map" -Wl,-eStart'

	arm-none-eabi-g++ @"ota_boot_proc.elf.in"
ota_boot_proc.bin: ota_boot_proc.elf
	arm-none-eabi-objcopy -O binary "ota_boot_proc.elf"  "ota_boot_proc.bin"
ota_boot_proc.siz: ota_boot_proc.elf
	arm-none-eabi-size --format=berkeley "ota_boot_proc.elf"
# Other Targets
clean:
ifneq ($(strip $(CC_UPPER_DEPS)),)
	-$(file > clean_CC_UPPER_DEPS.tmp, $(CC_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_CC_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_CC_UPPER_DEPS.tmp
endif
ifneq ($(strip $(C++_UPPER_DEPS)),)
	-$(file > clean_C++_UPPER_DEPS.tmp, $(C++_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_C++_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_C++_UPPER_DEPS.tmp
endif
ifneq ($(strip $(CPP_UPPER_DEPS)),)
	-$(file > clean_CPP_UPPER_DEPS.tmp, $(CPP_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_CPP_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_CPP_UPPER_DEPS.tmp
endif
ifneq ($(strip $(FSY_DEPS)),)
	-$(file > clean_FSY_DEPS.tmp, $(FSY_DEPS))
	-@ xargs -s 6000 -t -a clean_FSY_DEPS.tmp $(RM)
	-@$(RM) clean_FSY_DEPS.tmp
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
	-$(file > clean_C_UPPER_DEPS.tmp, $(C_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_C_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_C_UPPER_DEPS.tmp
endif
ifneq ($(strip $(SECONDARY_SIZE)),)
	-$(file > clean_SECONDARY_SIZE.tmp, $(SECONDARY_SIZE))
	-@ xargs -s 6000 -t -a clean_SECONDARY_SIZE.tmp $(RM)
	-@$(RM) clean_SECONDARY_SIZE.tmp
endif
ifneq ($(strip $(SRC_UPPER_DEPS)),)
	-$(file > clean_SRC_UPPER_DEPS.tmp, $(SRC_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_SRC_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_SRC_UPPER_DEPS.tmp
endif
ifneq ($(strip $(S_DEPS)),)
	-$(file > clean_S_DEPS.tmp, $(S_DEPS))
	-@ xargs -s 6000 -t -a clean_S_DEPS.tmp $(RM)
	-@$(RM) clean_S_DEPS.tmp
endif
ifneq ($(strip $(PP_UPPER_DEPS)),)
	-$(file > clean_PP_UPPER_DEPS.tmp, $(PP_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_PP_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_PP_UPPER_DEPS.tmp
endif
ifneq ($(strip $(P_DEPS)),)
	-$(file > clean_P_DEPS.tmp, $(P_DEPS))
	-@ xargs -s 6000 -t -a clean_P_DEPS.tmp $(RM)
	-@$(RM) clean_P_DEPS.tmp
endif
ifneq ($(strip $(JMP_UPPER_DEPS)),)
	-$(file > clean_JMP_UPPER_DEPS.tmp, $(JMP_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_JMP_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_JMP_UPPER_DEPS.tmp
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
	-$(file > clean_ASM_UPPER_DEPS.tmp, $(ASM_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_ASM_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_ASM_UPPER_DEPS.tmp
endif
ifneq ($(strip $(SX_UPPER_DEPS)),)
	-$(file > clean_SX_UPPER_DEPS.tmp, $(SX_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_SX_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_SX_UPPER_DEPS.tmp
endif
ifneq ($(strip $(CXX_DEPS)),)
	-$(file > clean_CXX_DEPS.tmp, $(CXX_DEPS))
	-@ xargs -s 6000 -t -a clean_CXX_DEPS.tmp $(RM)
	-@$(RM) clean_CXX_DEPS.tmp
endif
ifneq ($(strip $(JMP_DEPS)),)
	-$(file > clean_JMP_DEPS.tmp, $(JMP_DEPS))
	-@ xargs -s 6000 -t -a clean_JMP_DEPS.tmp $(RM)
	-@$(RM) clean_JMP_DEPS.tmp
endif
ifneq ($(strip $(CXX_UPPER_DEPS)),)
	-$(file > clean_CXX_UPPER_DEPS.tmp, $(CXX_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_CXX_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_CXX_UPPER_DEPS.tmp
endif
ifneq ($(strip $(P_UPPER_DEPS)),)
	-$(file > clean_P_UPPER_DEPS.tmp, $(P_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_P_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_P_UPPER_DEPS.tmp
endif
ifneq ($(strip $(FSY_UPPER_DEPS)),)
	-$(file > clean_FSY_UPPER_DEPS.tmp, $(FSY_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_FSY_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_FSY_UPPER_DEPS.tmp
endif
ifneq ($(strip $(C_DEPS)),)
	-$(file > clean_C_DEPS.tmp, $(C_DEPS))
	-@ xargs -s 6000 -t -a clean_C_DEPS.tmp $(RM)
	-@$(RM) clean_C_DEPS.tmp
endif
ifneq ($(strip $(SRC_DEPS)),)
	-$(file > clean_SRC_DEPS.tmp, $(SRC_DEPS))
	-@ xargs -s 6000 -t -a clean_SRC_DEPS.tmp $(RM)
	-@$(RM) clean_SRC_DEPS.tmp
endif
ifneq ($(strip $(PP_DEPS)),)
	-$(file > clean_PP_DEPS.tmp, $(PP_DEPS))
	-@ xargs -s 6000 -t -a clean_PP_DEPS.tmp $(RM)
	-@$(RM) clean_PP_DEPS.tmp
endif
ifneq ($(strip $(CC_DEPS)),)
	-$(file > clean_CC_DEPS.tmp, $(CC_DEPS))
	-@ xargs -s 6000 -t -a clean_CC_DEPS.tmp $(RM)
	-@$(RM) clean_CC_DEPS.tmp
endif
ifneq ($(strip $(SX_DEPS)),)
	-$(file > clean_SX_DEPS.tmp, $(SX_DEPS))
	-@ xargs -s 6000 -t -a clean_SX_DEPS.tmp $(RM)
	-@$(RM) clean_SX_DEPS.tmp
endif
ifneq ($(strip $(C++_DEPS)),)
	-$(file > clean_C++_DEPS.tmp, $(C++_DEPS))
	-@ xargs -s 6000 -t -a clean_C++_DEPS.tmp $(RM)
	-@$(RM) clean_C++_DEPS.tmp
endif
ifneq ($(strip $(OBJS)),)
	-$(file > clean_OBJS.tmp, $(OBJS))
	-@ xargs -s 6000 -t -a clean_OBJS.tmp $(RM)
	-@$(RM) clean_OBJS.tmp
endif
ifneq ($(strip $(SECONDARY_FLASH)),)
	-$(file > clean_SECONDARY_FLASH.tmp, $(SECONDARY_FLASH))
	-@ xargs -s 6000 -t -a clean_SECONDARY_FLASH.tmp $(RM)
	-@$(RM) clean_SECONDARY_FLASH.tmp
endif
ifneq ($(strip $(ASM_DEPS)),)
	-$(file > clean_ASM_DEPS.tmp, $(ASM_DEPS))
	-@ xargs -s 6000 -t -a clean_ASM_DEPS.tmp $(RM)
	-@$(RM) clean_ASM_DEPS.tmp
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
	-$(file > clean_S_UPPER_DEPS.tmp, $(S_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_S_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_S_UPPER_DEPS.tmp
endif
ifneq ($(strip $(CPP_DEPS)),)
	-$(file > clean_CPP_DEPS.tmp, $(CPP_DEPS))
	-@ xargs -s 6000 -t -a clean_CPP_DEPS.tmp $(RM)
	-@$(RM) clean_CPP_DEPS.tmp
endif
	-$(RM)  ota_boot_proc.elf
	-@echo ' '

post-build:
	-copy ota_boot_proc.bin ..
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents  post-build

-include ../makefile.targets
