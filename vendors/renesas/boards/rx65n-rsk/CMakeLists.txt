#Run this before make all C:/Renesas/RX/3_2_0/setccrx_sample.bat

# Set variables.
set(renesas_dir "${AFR_VENDORS_DIR}/renesas")
set(rx65nrsk_dir "${AFR_VENDORS_DIR}/renesas/boards/rx65n-rsk")
set(rx65nrsk_ports_dir "${rx65nrsk_dir}/ports")
set(rx_driver_package_dir "${AFR_VENDORS_DIR}/renesas/rx_driver_package/v125")

if(AFR_IS_TESTING)
    set(rx65nrsk_aws_dir "${rx65nrsk_dir}/aws_tests")
    set(exe_target aws_tests)
else()
    set(rx65nrsk_aws_dir "${rx65nrsk_dir}/aws_demos")
    set(exe_target aws_demos)
endif()

# -------------------------------------------------------------------------------------------------
# FreeRTOS Console metadata
# -------------------------------------------------------------------------------------------------

afr_set_board_metadata(ID "Renesas-RX65N-RSK")
afr_set_board_metadata(DISPLAY_NAME "Renesas Starter Kit+ for RX65N-2MB")
afr_set_board_metadata(DESCRIPTION "The Renesas Starter Kit+ for RX65N-2MB connects to AWS IoT Cloud using Ethernet")
afr_set_board_metadata(VENDOR_NAME "Renesas")
afr_set_board_metadata(FAMILY_NAME "RX65N")
afr_set_board_metadata(IS_ACTIVE "TRUE")
afr_set_board_metadata(CODE_SIGNER "AmazonFreeRTOS-Default")
afr_set_board_metadata(DATA_RAM_MEMORY "640KB")
afr_set_board_metadata(PROGRAM_MEMORY "2MB")
afr_set_board_metadata(SUPPORTED_IDE "E2Studio")
afr_set_board_metadata(RECOMMENDED_IDE "E2Studio")
afr_set_board_metadata(IDE_E2Studio_NAME "e2 studio")
afr_set_board_metadata(IDE_E2Studio_COMPILER "CCRX")
afr_set_board_metadata(KEY_IMPORT_PROVISIONING "TRUE")

afr_set_board_metadata(IDE_E2Studio_PROJECT_LOCATION "${AFR_ROOT_DIR}/projects/renesas/rx65n-rsk/e2studio/aws_demos")
afr_set_board_metadata(AWS_DEMOS_CONFIG_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}/aws_demos/config_files")

# -------------------------------------------------------------------------------------------------
# Compiler settings
#
# Note1: Cmake doesn't support cc-rx compiler. The settings are commented out.
# Note2: The settings below are NOT test yet. Some flags might be wrong and some might be missing.
# -------------------------------------------------------------------------------------------------
afr_mcu_port(compiler)

set(compiler_flags
    -isa=rxv2 -lang=c99 -branch=32  -preinclude=${renesas_dir}/amazon_freertos_common/compiler_support/ccrx/implicitlyinclude.h -define=MBEDTLS_CONFIG_FILE=<aws_mbedtls_config.h$<ANGLE-R>,CONFIG_MEDTLS_USE_AFR_MEMORY -nomessage=11174,21644,20010,23034,23035,20177,23033 -output=obj -debug -nologo
)

# Compiler flags.
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${compiler_flags}>
)

set(assembler_flags
    -isa=rxv2 -debug -nologo
)

# Assembler flags
target_compile_options(
    AFR::compiler::mcu_port
    INTERFACE
        $<$<COMPILE_LANGUAGE:ASM>:${assembler_flags}>
)

SET(LINKER_OPTIONS "-start=SU,SI,R_1,R_2,R,RPFRAM2*/04,C_PKCS11_STORAGE*/0100800,C_SYSTEM_CONFIG*/0104800,B_ETHERNET_BUFFERS_1,B_RX_DESC_1,B_TX_DESC_1,B,B_1,B_2/0800000,C_1,C_2,C,C$$*,D*,W*,L,P*/0FFF00300,EXCEPTVECT/0FFFFFF80,RESETVECT/0FFFFFFFC")

SET (CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} ${LINKER_OPTIONS} -form=absolute <OBJECTS> -library=afr_common.a -library=afr_core_json.a -library=afr_core_mqtt.a -library=afr_core_mqtt_demo_dependencies.a -library=afr_crypto.a -library=afr_defender.a -library=afr_dev_mode_key_provisioning.a -library=afr_device_shadow.a -library=afr_device_shadow_demo_dependencies.a -library=afr_freertos_plus_tcp.a -library=afr_greengrass.a -library=afr_https.a -library=afr_kernel.a -library=afr_logging.a -library=afr_mqtt.a -library=afr_ota.a -library=afr_pkcs11.a -library=afr_pkcs11_implementation.a -library=afr_pkcs11_utils.a -library=afr_platform.a -library=afr_retry_utils.a -library=afr_secure_sockets.a  -library=afr_serializer.a -library=afr_tls.a -library=afr_transport_interface_secure_sockets.a -library=libraries/3rdparty/libafr_3rdparty_mbedtls.a -library=libraries/3rdparty/libafr_3rdparty_tinycbor.a afr_kernel.a(resetprg.c) afr_kernel.a(dbsct.c) afr_kernel.a(vecttbl.c) -output=${exe_target}.abs -library=${exe_target}.lib -noprelink -nomessage -vect=_undefined_interrupt_source_isr -list=${exe_target}.map -show=all -nooptimize -rom=D=R,D_1=R_1,D_2=R_2,PFRAM2=RPFRAM2 -nologo")


#set(linker_flags
#    -noprelink -form=absolute -nomessage -vect=_undefined_interrupt_source_isr -list=${exe_target}.map
#    -nooptimize -rom=D=R,D_1=R_1,D_2=R_2 -nologo
#)

target_link_options(
    AFR::compiler::mcu_port
    INTERFACE
        ${linker_flags}
)

target_link_libraries(
  AFR::compiler::mcu_port
  INTERFACE
 
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS portable layers
# -------------------------------------------------------------------------------------------------

set(renesas_amazon_freertos_common_dir "${renesas_dir}/amazon_freertos_common")
afr_glob_src(afr_common_src DIRECTORY "${renesas_amazon_freertos_common_dir}")
afr_glob_src(afr_common_compiler_src DIRECTORY "${renesas_amazon_freertos_common_dir}/compiler_support/ccrx")
afr_glob_src(afr_common_network_src DIRECTORY "${renesas_amazon_freertos_common_dir}/network_support/onchip_rx_ether")

set(afr_common_include
    "${renesas_amazon_freertos_common_dir}"
    "${renesas_amazon_freertos_common_dir}/compiler_support/ccrx"
    "${renesas_amazon_freertos_common_dir}/network_support/onchip_rx_ether"
)

# r_cmt_rx, r_riic_rx, r_sci_iic_rx are excluded
afr_glob_src(rbyteq_src DIRECTORY "${rx_driver_package_dir}/r_byteq" RECURSE)
afr_glob_src(retherrx_src DIRECTORY "${rx_driver_package_dir}/r_ether_rx" RECURSE)
afr_glob_src(rflashrx_src DIRECTORY "${rx_driver_package_dir}/r_flash_rx" RECURSE)
afr_glob_src(rs12adrx_src DIRECTORY "${rx_driver_package_dir}/r_s12ad_rx" RECURSE)

# select only RX65 for r_bsp
afr_glob_src(rbsp_mcu_src DIRECTORY "${rx_driver_package_dir}/r_bsp/mcu")
afr_glob_src(rbsp_board_genericrx65n_src DIRECTORY "${rx_driver_package_dir}/r_bsp/board/generic_rx65n")
afr_glob_src(rbsp_mcu_all_src DIRECTORY "${rx_driver_package_dir}/r_bsp/mcu/all")
afr_glob_src(rbsp_mcu_rx65n_src DIRECTORY "${rx_driver_package_dir}/r_bsp/mcu/rx65n")

list(REMOVE_ITEM rbsp_mcu_all_src "${renesas_dir}/rx_driver_package/v125/r_bsp/mcu/all/reset_program.S")

set(rbsp_src
    ${rbsp_mcu_src}
    ${rbsp_board_genericrx65n_src}
    ${rbsp_mcu_all_src}
    ${rbsp_mcu_rx65n_src}
)

# select only RX65 for r_sci_rx
afr_glob_src(rscirx_target_rx65n_src DIRECTORY "${rx_driver_package_dir}/r_sci_rx/src/targets/rx65n")

set(rscirx_src
    "${rx_driver_package_dir}/r_sci_rx/src/r_sci_rx.c"
    ${rscirx_target_rx65n_src}
)

set(rx_driver_package_include
    "${rx_driver_package_dir}"
    "${rx_driver_package_dir}/r_bsp"
    "${rx_driver_package_dir}/r_bsp/board/generic_rx65n"
    "${rx_driver_package_dir}/r_ether_rx"
    "${rx_driver_package_dir}/r_flash_rx"
    "${rx_driver_package_dir}/r_flash_rx/src"
    "${rx_driver_package_dir}/r_flash_rx/src/flash_type_1"
    "${rx_driver_package_dir}/r_flash_rx/src/flash_type_3"
    "${rx_driver_package_dir}/r_flash_rx/src/flash_type_4"
    "${rx_driver_package_dir}/r_flash_rx/src/targets"
    "${rx_driver_package_dir}/r_byteq"
    "${rx_driver_package_dir}/r_byteq/src"
    "${rx_driver_package_dir}/r_sci_rx"
    "${rx_driver_package_dir}/r_sci_rx/src"
    "${rx_driver_package_dir}/r_s12ad_rx"
    "${rx_driver_package_dir}/r_s12ad_rx/src"
    "${rx_driver_package_dir}/r_s12ad_rx/src/targets/rx65n"
)

set(lib_include
    "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include/private"
    "${AFR_ROOT_DIR}/libraries/c_sdk/standard/common/include"
    "${AFR_ROOT_DIR}/libraries/abstractions/platform/include"
    "${AFR_ROOT_DIR}/libraries/abstractions/platform/freertos/include"
    "${AFR_ROOT_DIR}/libraries/abstractions/platform/include/platform"
    "${AFR_ROOT_DIR}/libraries/abstractions/secure_sockets/include"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/freertos_plus_tcp/test"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/freertos_plus_tcp/include"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/freertos_plus_tcp/portable/Compiler/Renesas"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/tls/include"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/crypto/include"
    "${AFR_ROOT_DIR}/libraries/abstractions/pkcs11/corePKCS11/source/include"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/utils/include"
    "${AFR_ROOT_DIR}/libraries/logging/include"
    "${AFR_ROOT_DIR}/demos/dev_mode_key_provisioning/include"
    "${AFR_ROOT_DIR}/libraries/c_sdk/aws/defender/include"
    "${AFR_ROOT_DIR}/libraries/c_sdk/standard/mqtt/include"
    "${AFR_ROOT_DIR}/libraries/coreMQTT/source/include"
    "${AFR_ROOT_DIR}/libraries/coreMQTT/source/interface"
    "${AFR_ROOT_DIR}/libraries/abstractions/retry_utils"
    "${AFR_ROOT_DIR}/libraries/abstractions/transport/secure_sockets"
    "${AFR_ROOT_DIR}/libraries/c_sdk/standard/serializer/include"
    "${AFR_ROOT_DIR}/libraries/c_sdk/aws/shadow/include"
    "${AFR_ROOT_DIR}/libraries/c_sdk/standard/https/include"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/aws/greengrass/include"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/aws/ota/src"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/aws/ota/include"
    "${AFR_ROOT_DIR}/libraries/3rdparty/mbedtls/include"
    "${AFR_ROOT_DIR}/libraries/freertos_plus/standard/freertos_plus_cli/include"
    "${AFR_ROOT_DIR}/libraries/coreJSON/source/include"
    "${AFR_ROOT_DIR}/libraries/device_shadow_for_aws/source/include"
    "${AFR_ROOT_DIR}/libraries/3rdparty/pkcs11"
    "${AFR_ROOT_DIR}/libraries/abstractions/pkcs11/corePKCS11/source/portable/mbedtls/include"
    "${AFR_ROOT_DIR}/libraries/3rdparty/mbedtls/include/mbedtls"
    "${AFR_ROOT_DIR}/libraries/3rdparty/mbedtls_utils"
    "${AFR_ROOT_DIR}/libraries/3rdparty/mbedtls_config"
    "${AFR_ROOT_DIR}/libraries/3rdparty/tinycbor/src"
    "${AFR_ROOT_DIR}/libraries/3rdparty/http_parser"
    "${AFR_ROOT_DIR}/libraries/3rdparty/jsmn"
)

afr_glob_src(smc_gen_src DIRECTORY "${rx65nrsk_aws_dir}/src/smc_gen" RECURSE)

set(smc_gen_include
    "${rx65nrsk_aws_dir}/src/smc_gen/r_config"
    "${rx65nrsk_aws_dir}/src/smc_gen/general"
    "${rx65nrsk_aws_dir}/src/smc_gen/r_pincfg"
)

# Kernel
afr_mcu_port(kernel)

target_sources(
    AFR::kernel::mcu_port
    INTERFACE
        ${afr_common_src}
        ${afr_common_compiler_src}
        ${afr_common_network_src}
        ${rbsp_src}
        ${rbyteq_src}
        ${retherrx_src}
        ${rflashrx_src}
        ${rs12adrx_src}
        ${rscirx_src}
        ${smc_gen_src}
        "${AFR_KERNEL_DIR}/portable/MemMang/heap_4.c"
        "${AFR_KERNEL_DIR}/portable/Renesas/RX600v2/port_asm.src"
        "${AFR_KERNEL_DIR}/portable/Renesas/RX600v2/port.c"
        "${AFR_KERNEL_DIR}/portable/Renesas/RX600v2/portmacro.h"
)

set(kernel_inc_dirs
    ${afr_common_include}
    ${rx_driver_package_include}
    ${lib_include}
    ${smc_gen_include}
    "${AFR_KERNEL_DIR}/portable/Renesas/RX600v2"
    "${rx65nrsk_aws_dir}/config_files"
    "${rx65nrsk_aws_dir}/application_code/renesas_code"
    "${rx65nrsk_aws_dir}/application_code/renesas_code/utility"
    "$<IF:${AFR_IS_TESTING},${AFR_TESTS_DIR},${AFR_DEMOS_DIR}>/include"
)

target_include_directories(
    AFR::kernel::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
)

# PKCS11
afr_mcu_port(pkcs11_implementation)
target_sources(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        "${rx65nrsk_ports_dir}/pkcs11/core_pkcs11_pal.c"
)

target_include_directories(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
)

target_link_libraries(
    AFR::pkcs11_implementation::mcu_port
    INTERFACE
        3rdparty::mbedtls
        AFR::pkcs11_mbedtls
)

# FreeRTOS Plus TCP
afr_mcu_port(freertos_plus_tcp)
target_sources(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/BufferManagement/BufferAllocation_2.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/NetworkInterface/RX/ether_callback.c"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/NetworkInterface/RX/NetworkInterface.c"
        # Header files added to the target so that these are available in code downloaded from the FreeRTOS console.
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/Compiler/Renesas/pack_struct_end.h"
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/Compiler/Renesas/pack_struct_start.h"
)

target_include_directories(
    AFR::freertos_plus_tcp::mcu_port
    INTERFACE
        INTERFACE $<$<NOT:$<COMPILE_LANGUAGE:ASM>>:${kernel_inc_dirs}>
        "${AFR_MODULES_FREERTOS_PLUS_DIR}/standard/freertos_plus_tcp/portable/Compiler/Renesas"
)

# Secure sockets
afr_mcu_port(secure_sockets)
target_link_libraries(
    AFR::secure_sockets::mcu_port
    INTERFACE AFR::secure_sockets_freertos_plus_tcp
)

# OTA
afr_mcu_port(ota)
target_sources(
    AFR::ota::mcu_port
    INTERFACE "${rx65nrsk_ports_dir}/ota/aws_ota_pal.c"
)
target_link_libraries(
    AFR::ota::mcu_port
    INTERFACE
        AFR::crypto
        AFR::pkcs11
        AFR::ota_mqtt
        AFR::ota_http
)

# -------------------------------------------------------------------------------------------------
# FreeRTOS demos and tests
# -------------------------------------------------------------------------------------------------
set(CMAKE_EXECUTABLE_SUFFIX "abs")

afr_glob_src(board_code_src DIRECTORY "${rx65nrsk_aws_dir}/application_code/renesas_code")
afr_glob_src(board_code_util DIRECTORY "${rx65nrsk_aws_dir}/application_code/renesas_code/utility")
afr_glob_src(config_files_src DIRECTORY "${AFR_ROOT}/projects/renesas/rx65n-rsk/aws_demos/config_files")

# Do not add demos or tests if they're turned off.
if(AFR_ENABLE_DEMOS OR AFR_ENABLE_TESTS)
    add_executable(${exe_target}
        ${board_code_src}
        ${board_code_util}
        ${config_files_src}
        "${rx65nrsk_aws_dir}/application_code/main.c"
    )
    target_link_libraries(
        ${exe_target}
        PRIVATE
            AFR::freertos_plus_tcp
            AFR::utils
    )
endif()

# Set C Library Generator Commands
add_custom_command(
              TARGET ${exe_target}
              PRE_LINK
              COMMENT "\nExecuting: GENERATING LIBRARY FILE" #this command is not mandatory.
              COMMAND lbgrx -isa=rxv2 -fpu -branch=32 -lang=c99 -head=runtime,ctype,math,mathf,stdarg,stdio,stdlib,string,new,fenv,inttypes,wchar,wctype -output=${exe_target}.lib -nologo
              VERBATIM
)

# Set Output File Type:
SET(Convert_Command "-form=stype") #to output .mot file
#SET(Convert_Command "-form=hexadecimal") # to output .hex file
#SET(Convert_Command "-form=binary") # to output .bin file

# Set Converter Command
add_custom_command (
	TARGET ${exe_target}
	COMMAND ${CMAKE_CONVERTER_EXECUTABLE} ${exe_target}.abs ${Convert_Command}
	COMMENT "Converter:"
	VERBATIM
)

# add command for building with X Converter
add_custom_command(
	TARGET ${exe_target}
	COMMENT "\nExecuting: ${CMAKE_COMMAND} ${exe_target}.abs ${exe_target}.x" #this command is not mandatory.
	COMMAND ${CMAKE_COMMAND} "${exe_target}.abs" "${exe_target}.x"
	VERBATIM
)

#if(NOT AFR_METADATA_MODE)
#    message(FATAL_ERROR "CMake support for nxp is not complete yet.")
#endif()
