################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/sc_drivers/r_ostm/src/lld/r_ostm_lld_rza2m.c 

C_DEPS += \
./generate/sc_drivers/r_ostm/src/lld/r_ostm_lld_rza2m.d 

OBJS += \
./generate/sc_drivers/r_ostm/src/lld/r_ostm_lld_rza2m.o 


# Each subdirectory must supply rules for building sources it contributes
generate/sc_drivers/r_ostm/src/lld/r_ostm_lld_rza2m.o: D:/Work/DeviceTester/amazon-freertos/lib/third_party/mcu_vendor/renesas/rz_mcu_boards/core_package/generate/sc_drivers/r_ostm/src/lld/r_ostm_lld_rza2m.c
	arm-none-eabi-gcc -mcpu=cortex-a9 -march=armv7-a -marm -mthumb-interwork -mlittle-endian -mfloat-abi=hard -mfpu=neon -mno-unaligned-access -Os -ffunction-sections -fdata-sections -Wnull-dereference -g -Wstack-usage=100 -D__RZ=1 -DUNITY_INCLUDE_CONFIG_H -DAMAZON_FREERTOS_ENABLE_UNIT_TESTS -D__AWS_FREERTOS_STRUCTURE__ -DTARGET_GR_MANGO -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\compiler\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\unity\src" -I"D:\Work\DeviceTester\amazon-freertos\tests\common\include" -I"D:\Work\DeviceTester\amazon-freertos\tests\common\ota" -I"D:\Work\DeviceTester\amazon-freertos\demos\common\include" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_adc\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\unity\extras\fixture\src" -I"D:\Work\DeviceTester\amazon-freertos\tests\renesas\rza2m-gr-mango\common\config_files" -I"D:\Work\DeviceTester\amazon-freertos\lib\FreeRTOS-Plus-TCP\source\portable\Compiler\GCC" -I"D:\Work\DeviceTester\amazon-freertos\lib\FreeRTOS\portable\GCC\ARM_CA9_RZA2M" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mbedtls\include" -I"D:\Work\DeviceTester\amazon-freertos\tests\renesas\rza2m-gr-mango\common\application_code\application\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\pkcs11" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\tinycbor" -I"D:\Work\DeviceTester\amazon-freertos\lib\ota" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\jsmn" -I"D:\Work\DeviceTester\amazon-freertos\lib\include" -I"D:\Work\DeviceTester\amazon-freertos\lib\FreeRTOS-Plus-TCP\include" -I"D:\Work\DeviceTester\amazon-freertos\lib\include\private" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\amazon_freertos_common\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\amazon_freertos_common\Flash\rza2m-gr-mango\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\configuration" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\drivers\r_cache\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\drivers\r_cpg\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\drivers\r_gpio\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\drivers\r_intc\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\drivers\r_mmu\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\drivers\r_stb\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\os_abstraction\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_cbuffer\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_ostm\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_riic\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_scifa\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\system\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\system\iodefine" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\system\iodefine\iodefines" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\system\iodefine\iobitmasks" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_ether_gr_mango" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_ether_gr_mango\inc" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_ether_gr_mango\src\targets\rza2m-gr-mango" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_ether_gr_mango\src" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_ether_gr_mango\src\phy" -I"D:\Work\DeviceTester\amazon-freertos\lib\third_party\mcu_vendor\renesas\rz_mcu_boards\core_package\generate\sc_drivers\r_octabus\inc" -I"D:\Work\DeviceTester\amazon-freertos\tests\renesas\rza2m-gr-mango\common\application_code\application\hwsetup\octabus_setup\inc" -Wa,-adhlns="$@.lst" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

