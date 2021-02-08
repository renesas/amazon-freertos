#Run this before make all C:/Renesas/RX/3_2_0/setccrx_sample.bat

cmake_minimum_required(VERSION 3.16)

# set the project name
project(bootloader)
enable_language(C ASM)

#--------------------------------------------------------------------------------------------------
# This cmake file is for building bootloader only
#=-------------------------------------------------------------------------------------------------

add_executable(bootloader)

set(CMAKE_EXECUTABLE_SUFFIX "abs")

get_filename_component(__root_dir "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)
set(AFR_ROOT_DIR ${__root_dir} CACHE INTERNAL "FreeRTOS source root.")
include("${AFR_ROOT_DIR}/tools/cmake/afr.cmake")

# Set variables
set(rx65nrsk_boot_dir "${AFR_ROOT_DIR}/projects/renesas/rx65n-rsk/e2studio/boot_loader")
set(renesas_dir "${AFR_VENDORS_DIR}/renesas")
set(rx65nrsk_dir "${AFR_VENDORS_DIR}/renesas/boards/rx65n-rsk")
set(rx65nrsk_ports_dir "${rx65nrsk_dir}/ports")
set(rx65nrsk_aws_dir "${rx65nrsk_dir}/boot_loader")
set(rx_driver_package_dir "${AFR_VENDORS_DIR}/renesas/rx_driver_package/v125")
set(renesas_amazon_freertos_common_dir "${renesas_dir}/amazon_freertos_common")

#MESSAGE(WARNING "${LINK_FLAGS}")
# -------------------------------------------------------------------------------------------------
# Compiler settings
# -------------------------------------------------------------------------------------------------

# Compiler flags. (C_FLAGS)
set(compiler_flags  -lang=c99 -isa=rxv2 -fpu -utf8 -branch=32  -nomessage=11174,21644,20010,23034,23035,20177,23033 -output=obj -debug -nologo -preinclude=${renesas_dir}/amazon_freertos_common/compiler_support/ccrx/implicitlyinclude.h "-define=CONFIG_MEDTLS_USE_AFR_MEMORY,MBEDTLS_CONFIG_FILE=<aws_mbedtls_config.h$<ANGLE-R>"
)

# Compiler flags.
target_compile_options(
        ${CMAKE_PROJECT_NAME}
	    PRIVATE
        $<$<COMPILE_LANGUAGE:C>:${compiler_flags}>
)

# Assembler flags (ASM_FLAGS)
set(assembler_flags -isa=rxv2 -fpu -debug -nologo
)

# Assembler flags
target_compile_options(
        ${CMAKE_PROJECT_NAME}
	    PRIVATE
        $<$<COMPILE_LANGUAGE:ASM>:${assembler_flags}>
)

SET (LINKER_OPTIONS_BOOTLOADER -start=SU,SI,B_1,R_1,B_2,R_2,B,R,RPFRAM2/0100,C_BOOTLOADER_KEY_STORAGE*/0100000,C_BOOT_LOADER_MAGIC_CODE_1,C_1,C_2,C,C$$*,D*,W*,L,P*/0FFFC0000,EXCEPTVECT/0FFFFFF80,RESETVECT/0FFFFFFFC)
SET (CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} ${LINKER_OPTIONS_BOOTLOADER} -form=absolute <OBJECTS> -output=bootloader.abs -library=bootloader.lib -noprelink -nomessage -vect=_undefined_interrupt_source_isr -list=bootloader.map -show=all -nooptimize -rom=PFRAM2=RPFRAM2,D=R,D_1=R_1,D_2=R_2 -nologo")

#set(linker_flags "${LINKER_OPTIONS_BOOTLOADER} -library=bootloader.lib <OBJECTS> -output=bootloader.abs -noprelink -nomessage -list=bootloader.map -show=all -nooptimize -rom=D=R,D_1=R_1,D_2=R_2,PFRAM2=RPFRAM2 -nologo")

# Linker flags
target_link_options(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${linker_flags}
)

# bootloader src
afr_glob_src(smc_gen_src DIRECTORY "${rx65nrsk_boot_dir}/src/smc_gen" RECURSE)

set(bootloader_src
    "${rx65nrsk_boot_dir}/src/boot_loader.c"
    "${rx65nrsk_boot_dir}/src/src/base64_decode.c"
    "${rx65nrsk_boot_dir}/src/src/r_simple_filesystem_on_dataflash.c"
    "${AFR_3RDPARTY_DIR}/tinycrypt/lib/source/ecc_dsa.c"
    "${AFR_3RDPARTY_DIR}/tinycrypt/lib/source/ecc.c"
    "${AFR_3RDPARTY_DIR}/tinycrypt/lib/source/sha256.c"
    "${AFR_3RDPARTY_DIR}/tinycrypt/lib/source/utils.c"
)

set(bootloader_include
    "${AFR_3RDPARTY_DIR}/tinycrypt/lib/include"
    "${rx65nrsk_boot_dir}/src"
    "${rx65nrsk_boot_dir}/src/src"
    "${rx65nrsk_boot_dir}/src/src/sha1/include"
    "${rx65nrsk_boot_dir}/src/src/tinycrypt/lib/include"
    "${rx65nrsk_boot_dir}/src/key"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_config"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_byteq"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_byteq/src"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_bsp"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_flash_rx"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_flash_rx/src"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_flash_rx/src/flash_type_1"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_flash_rx/src/flash_type_3"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_flash_rx/src/flash_type_4"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_flash_rx/src/targets"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_sci_rx"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_sci_rx/src"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_sys_time_rx"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_sys_time_rx/src"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_cmt_rx"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_cmt_rx/src"
    "${rx65nrsk_boot_dir}/src/smc_gen/general"
    "${rx65nrsk_boot_dir}/src/smc_gen/r_pincfg"
)

target_sources(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
        ${smc_gen_src}
        ${bootloader_src}
)
target_include_directories(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${smc_gen_include}>
    $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${bootloader_include}>
)

# -------------------------------------------------------------------------------------------------
# Additional build configurations
# -------------------------------------------------------------------------------------------------

# Set C Library Generator Commands
add_custom_command(
              TARGET ${CMAKE_PROJECT_NAME}
              PRE_LINK
              COMMENT "\nExecuting: GENERATING LIBRARY FILE" #this command is not mandatory.
              COMMAND lbgrx -isa=rxv2 -fpu -branch=32 -lang=c99 -head=runtime,ctype,math,mathf,stdarg,stdio,stdlib,string,new,fenv,inttypes,wchar,wctype -output=${CMAKE_PROJECT_NAME}.lib -nologo
              VERBATIM
)

# Set Output File Type:
SET(Convert_Command "-form=stype") #to output .mot file
#SET(Convert_Command "-form=hexadecimal") # to output .hex file
#SET(Convert_Command "-form=binary") # to output .bin file

# Set Converter Command
add_custom_command (
	TARGET ${CMAKE_PROJECT_NAME}
	COMMAND ${CMAKE_CONVERTER_EXECUTABLE} ${CMAKE_PROJECT_NAME}.abs ${Convert_Command}
	COMMENT "Converter:"
	VERBATIM
)

# add command for building with X Converter
add_custom_command(
	TARGET ${CMAKE_PROJECT_NAME}
	COMMENT "\nExecuting: ${CMAKE_COMMAND} ${CMAKE_PROJECT_NAME}.abs ${CMAKE_PROJECT_NAME}.x" #this command is not mandatory.
	COMMAND ${CMAKE_COMMAND} ${CMAKE_PROJECT_NAME}.abs ${CMAKE_PROJECT_NAME}.x
	VERBATIM
)