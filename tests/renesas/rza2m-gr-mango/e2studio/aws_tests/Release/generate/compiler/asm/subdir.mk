################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/cpu.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/initsect.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/irq.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/irqfiq_handler.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/mmu_operation.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/r_cache_l1_rza2.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/reset_handler.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/vector_mirrortable.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/vector_table.asm \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/vfp_init.asm 

OBJS += \
./generate/compiler/asm/cpu.o \
./generate/compiler/asm/initsect.o \
./generate/compiler/asm/irq.o \
./generate/compiler/asm/irqfiq_handler.o \
./generate/compiler/asm/mmu_operation.o \
./generate/compiler/asm/r_cache_l1_rza2.o \
./generate/compiler/asm/reset_handler.o \
./generate/compiler/asm/vector_mirrortable.o \
./generate/compiler/asm/vector_table.o \
./generate/compiler/asm/vfp_init.o 

ASM_DEPS += \
./generate/compiler/asm/cpu.d \
./generate/compiler/asm/initsect.d \
./generate/compiler/asm/irq.d \
./generate/compiler/asm/irqfiq_handler.d \
./generate/compiler/asm/mmu_operation.d \
./generate/compiler/asm/r_cache_l1_rza2.d \
./generate/compiler/asm/reset_handler.d \
./generate/compiler/asm/vector_mirrortable.d \
./generate/compiler/asm/vector_table.d \
./generate/compiler/asm/vfp_init.d 


# Each subdirectory must supply rules for building sources it contributes
generate/compiler/asm/cpu.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/cpu.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/initsect.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/initsect.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/irq.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/irq.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/irqfiq_handler.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/irqfiq_handler.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/mmu_operation.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/mmu_operation.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/r_cache_l1_rza2.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/r_cache_l1_rza2.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/reset_handler.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/reset_handler.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/vector_mirrortable.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/vector_mirrortable.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/vector_table.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/vector_table.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
generate/compiler/asm/vfp_init.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/compiler/asm/vfp_init.asm
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -x assembler-with-cpp -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

