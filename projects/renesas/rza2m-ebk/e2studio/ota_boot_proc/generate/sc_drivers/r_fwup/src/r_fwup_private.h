/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : r_fwup_rx_private.h
* Description  : Functions for using Firmware update on RX devices.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           xx.xx.2020 1.00    Initial Release.
***********************************************************************************************************************/

#ifndef FWUP_PRIVATE_H
#define FWUP_PRIVATE_H

/*****************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#include "platform.h"
#endif

/******************************************************************************
Macro definitions
******************************************************************************/
/* Set the implementation environment. */
#define FWUP_IMPLEMENTATION_BOOTLOADER  0   // Bootloader. (default)
#define FWUP_IMPLEMENTATION_NONEOS      1   // Firmware update w/o OS.
#define FWUP_IMPLEMENTATION_AFRTOS      2   // Firmware update with Amazon FreeRTOS(OTA).
#define FWUP_IMPLEMENTATION_OTHER_RTOS  3   // Firmware update with other FreeRTOS.

/* Set the communication function. */
#define FWUP_COMMUNICATION_SCI          0   // SCI connection. (default)
#define FWUP_COMMUNICATION_ETHERNET     1   // Ethernet connection.
#define FWUP_COMMUNICATION_USB          2   // USB connection.
#define FWUP_COMMUNICATION_SDHI         3   // SDHI connection.
#define FWUP_COMMUNICATION_QSPI         4   // QSPI connection.

/* Set the combination of "implementation environment" and "communication function". */
#define FWUP_COMM_SCI_BOOTLOADER        0   // Used for Bootloader with SCI connection from COM port.
#define FWUP_COMM_SCI_PRIMITIVE         1   // SCI connection from COM port using primitive R/W.
#define FWUP_COMM_USB_PRIMITIVE         2   // USB connection from COM port using primitive R/W.
#define FWUP_COMM_QSPI_PRIMITIVE        3   // Connect external storage (SD card) to QSPI using primitive R/W.
#define FWUP_COMM_SCI_AFRTOS            4   // Connect wireless module to SCI with Amazon FreeRTOS.
#define FWUP_COMM_ETHER_AFRTOS          5   // Connect Eathernet with Amazon FreeRTOS.
#define FWUP_COMM_USB_AFRTOS            6   // Connect LTE modem to USB with Amazon FreeRTOS.
#define FWUP_COMM_SDHI_AFRTOS           7   // Connect wireless module to SDHI with Amazon FreeRTOS.
#define FWUP_COMM_SCI_FS                8   // External storage (SD card + file system) connected to SCI.
#define FWUP_COMM_USB_FS                9   // External storage (USB memory + file system) connected to USB.
#define FWUP_COMM_SDHI_FS              10   // External storage (SD card + file system) connected to SDHI.
#define FWUP_COMM_QSPI_FS              11   // External storage (Serial flash + file system) connected to QSPI.

/* Set the algorithm of signature verification. */
#define FWUP_SIGNATURE_ECDSA            0   // ECDSA.

/* Check the combination of "implementation environment" and "communication function"
   and set value to FWUP_ENV_COMMUNICATION_FUNCTION.
    0 = Used for Bootloader with SCI connection from COM port.
    1 = SCI connection from COM port using primitive R/W.
    2 = USB connection from COM port using primitive R/W.
    3 = Connect external storage (SD card) to QSPI using primitive R/W.
    4 = Connect wireless module to SCI with Amazon FreeRTOS.
    5 = Connect Eathernet with Amazon FreeRTOS.
    6 = Connect LTE modem to USB with Amazon FreeRTOS.
    7 = Connect wireless module to SDHI with Amazon FreeRTOS.
    8 = External storage (SD card + file system) connected to SCI.
    9 = External storage (USB memory + file system) connected to USB.
   10 = External storage (SD card + file system) connected to SDHI.
   11 = External storage (Serial flash + file system) connected to QSPI.
*/
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)  // Case of Bootloader.
#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_SCI_BOOTLOADER
#else
#error "Illegal combination. Check the CFG value of r_fwup_config.h"
#endif /* FWUP_CFG_COMMUNICATION_FUNCTION */
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)  // Case of Firmware update w/o OS.
#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_SCI_PRIMITIVE
#elif (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_USB)  // Case of USB.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_USB_PRIMITIVE
#elif (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_QSPI)  // Case of QSPI.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_QSPI_PRIMITIVE
#else
#error "Illegal combination. Check the CFG value of r_fwup_config.h"
#endif /* FWUP_CFG_COMMUNICATION_FUNCTION */
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)  // Case of Firmware update with Amazon FreeRTOS(OTA).
#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_SCI_AFRTOS
#elif (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_ETHERNET)  // Case of Ethernet.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_ETHER_AFRTOS
#elif (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_USB)  // Case of USB.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_USB_AFRTOS
#elif (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SDHI)  // Case of SDHI.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_SDHI_AFRTOS
#else
#error "Illegal combination. Check the CFG value of r_fwup_config.h"
#endif /* FWUP_CFG_COMMUNICATION_FUNCTION */
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_OTHER_RTOS)  // Case of Firmware update with other FreeRTOS.
#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_SCI_FS
#elif (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_USB)  // Case of USB.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_USB_FS
#elif (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SDHI)  // Case of SDHI.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_SDHI_FS
#elif (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_QSPI)  // Case of QSPI.
#define FWUP_ENV_COMMUNICATION_FUNCTION     FWUP_COMM_QSPI_FS
#else
#error "Illegal combination. Check the CFG value of r_fwup_config.h"
#endif  /* FWUP_ENV_COMMUNICATION_FUNCTION */
#endif  /* FWUP_CFG_IMPLEMENTATION_ENVIRONMENT */

/* Check the bank mode (Dual bank mode or Linear mode) of this environment
   and set value to FWUP_ENV_CODE_FLASH_BANK_MODE.
    0 = Dual bank mode
    1 = Linear mode
 */
#if !defined(BSP_CFG_CODE_FLASH_BANK_MODE) || (BSP_CFG_CODE_FLASH_BANK_MODE == 1)
/* Linear mode */
#define	FWUP_ENV_CODE_FLASH_BANK_MODE (1)
#else
/* Dual bank mode */
#define	FWUP_ENV_CODE_FLASH_BANK_MODE (0)
#endif /* (BSP_CFG_CODE_FLASH_BANK_MODE) */

/* Set the macro of use External memory.
   - FWUP_ENV_USE_EXMEM_TEMPORARY : Use External memory for Temporary area.
   - FWUP_ENV_USE_EXMEM_EXECUTE   : Use External memory for Execute area.
   */
#if (FWUP_CFG_USE_EXMEM == 0)
/* External flash not use */
#define	FWUP_ENV_USE_EXMEM_TEMPORARY	(0)
#define	FWUP_ENV_USE_EXMEM_EXECUTE		(0)
#elif (FWUP_CFG_USE_EXMEM == 1)
/* External flash use for Temporary area */
#define	FWUP_ENV_USE_EXMEM_TEMPORARY	(1)
#define	FWUP_ENV_USE_EXMEM_EXECUTE		(0)
#elif (FWUP_CFG_USE_EXMEM == 2)
/* External flash use for Execute area */
#define	FWUP_ENV_USE_EXMEM_TEMPORARY	(0)
#define	FWUP_ENV_USE_EXMEM_EXECUTE		(1)
#elif (FWUP_CFG_USE_EXMEM == 3)
/* External flash use for Temporary area and Execute area */
#define	FWUP_ENV_USE_EXMEM_TEMPORARY	(1)
#define	FWUP_ENV_USE_EXMEM_EXECUTE		(1)
#endif

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
#if (BSP_MCU_SERIES_RX600)
#if 0  // DEBUG  -- Bootloader = 256KB
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_13
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_51

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
#define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_52

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 6

#define BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS FLASH_DF_BLOCK_0
#define BOOT_LOADER_CONST_DATA_BLOCK_NUM 256

#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH 0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH 0x100
#define INITIAL_FIRMWARE_FILE_NAME "userprog.rsu"

#define FLASH_INTERRUPT_PRIORITY 14	/* 0(low) - 15(high) */
#define SCI_INTERRUPT_PRIORITY 15	/* 0(low) - 15(high) */

// #define FLASH_INTERRUPT_PRIORITY configMAX_SYSCALL_INTERRUPT_PRIORITY    /* 0(low) - 15(high) */
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)
#define BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_DF - BOOT_LOADER_CONST_DATA_BLOCK_NUM)
#define USER_RESET_VECTOR_ADDRESS (BOOT_LOADER_LOW_ADDRESS - 4)

#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH 0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH 0x100

#define BOOT_LOADER_TOTAL_UPDATE_SIZE   (uint32_t)(BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER * FLASH_CF_MEDIUM_BLOCK_SIZE)
#else  // DEBUG  -- Bootloader = 64KB
#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) /* Dual mode */
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_7
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_45

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
#define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_46

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 0

#define BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS FLASH_DF_BLOCK_0
#define BOOT_LOADER_CONST_DATA_BLOCK_NUM 256

#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH 0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH 0x100
#define INITIAL_FIRMWARE_FILE_NAME "userprog.rsu"

#define FLASH_INTERRUPT_PRIORITY 14	/* 0(low) - 15(high) */
#define SCI_INTERRUPT_PRIORITY 15	/* 0(low) - 15(high) */

// #define FLASH_INTERRUPT_PRIORITY configMAX_SYSCALL_INTERRUPT_PRIORITY    /* 0(low) - 15(high) */
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)
#define BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_DF - BOOT_LOADER_CONST_DATA_BLOCK_NUM)
#define USER_RESET_VECTOR_ADDRESS (BOOT_LOADER_LOW_ADDRESS - 4)

#define BOOT_LOADER_TOTAL_UPDATE_SIZE   (uint32_t)(BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER * FLASH_CF_MEDIUM_BLOCK_SIZE)
#define SF_SECTOR_SIZE	0x1000
#else /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) Linear mode */
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_7
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_38

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
// #define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_39

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 0

#define BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS FLASH_DF_BLOCK_0
#define BOOT_LOADER_CONST_DATA_BLOCK_NUM 256

#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH 0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH 0x100
#define INITIAL_FIRMWARE_FILE_NAME "userprog.rsu"

#define FLASH_INTERRUPT_PRIORITY 14	/* 0(low) - 15(high) */
#define SCI_INTERRUPT_PRIORITY 15	/* 0(low) - 15(high) */

// #define FLASH_INTERRUPT_PRIORITY configMAX_SYSCALL_INTERRUPT_PRIORITY    /* 0(low) - 15(high) */
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_BLOCK_69
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)
#define BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_DF - BOOT_LOADER_CONST_DATA_BLOCK_NUM)
#define USER_RESET_VECTOR_ADDRESS (BOOT_LOADER_LOW_ADDRESS - 4)

#define BOOT_LOADER_TOTAL_UPDATE_SIZE   (uint32_t)(BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER * FLASH_CF_MEDIUM_BLOCK_SIZE)
#define SF_SECTOR_SIZE	0x1000
#endif /* FWUP_ENV_CODE_FLASH_BANK_MODE */
#endif  // DEBUG
#elif (MCU_SERIES_RZA2) /* for RZ/A2M */
#define BOOT_LOADER_IMAGE_SIZE_BOT_ADR 0x218
#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH 0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH 0x100
#define INITIAL_FIRMWARE_FILE_NAME "userprog.rsu"
#define SF_SECTOR_SIZE	0x1000

#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS 	(0x20A00000)	// 更新アプリケーション領域 (temporary area) の先頭アドレス			// RZ/A2M OTA 2020.03.19 //
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS		(0x20200000)	// 実行アプリケーション領域 (exec area) の先頭アドレス				// RZ/A2M OTA 2020.03.19 //
#define USER_RESET_VECTOR_ADDRESS (0x20200300)	// 0x300まで.rsuファイルのヘッダー部分が入るため このアドレスに移動		// RZ/A2M OTA 2020.03.19 //
#define BOOT_LOADER_TOTAL_UPDATE_SIZE (0x800000 - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH)

#define     __DSB()  asm volatile ("DSB")
#else
    /* Fix me for other MCU series */
#endif /* BSP_MCU_SERIES */

#if (FWUP_CFG_USE_EXMEM != 0)
#define	FWUP_FLASH_SPI_DEV	FLASH_SPI_DEV0
#define BOOT_LOADER_SERIAL_FLASH_START_ADDRESS 0x000000
#define	BOOT_LOADER_SERIAL_FLASH_BLOCK_SIZE 65536  /* 64KB */
#endif /* (FWUP_CFG_USE_EX_MEM != 0) */
/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/


//#define otaconfigMAX_NUM_BLOCKS_REQUEST           128U    /* this value will be appeared after 201908.00 in aws_ota_agent_config.h */

#define LIFECYCLE_STATE_BLANK       (0xff)
#define LIFECYCLE_STATE_TESTING     (0xfe)
#define LIFECYCLE_STATE_INSTALLING  (0xfc)
#define LIFECYCLE_STATE_VALID       (0xf8)
#define LIFECYCLE_STATE_INVALID     (0xf0)
#define LIFECYCLE_STATE_EOL         (0xe0)

#define R_OTA_ERR_NONE              (0)
#define R_OTA_ERR_INVALID_CONTEXT   (-1)
#define R_OTA_ERR_QUEUE_SEND_FAIL   (-2)

#define OTA_FLASHING_IN_PROGRESS    (0)
#define OTA_FLASHING_COMPLETE       (1)

#define OTA_SIGUNATURE_SEQUENCE_TOP_VALUE                   (0x30)
#define OTA_SIGUNATURE_INTEGER_VALUE                        (0x02)
#define OTA_SIGUNATURE_NOT_INCLUDE_NEGATIVE_NUMBER_VALUE    (0x20)
#define OTA_SIGUNATURE_INCLUDE_NEGATIVE_NUMBER_VALUE        (0x21)
#define OTA_SIGUNATURE_DATA_HALF_LENGTH                     (32)
#define OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH                 (2)
#define OTA_SIGUNATURE_SKIP                                 (2)

#define OTA_FLASH_MIN_PGM_SIZE_MASK (0xFFFFFFFF - FLASH_CF_MIN_PGM_SIZE + 1)

#define FWUP_SCI_CONTROL_BLOCK_A (0)
#define FWUP_SCI_CONTROL_BLOCK_B (1)
#define FWUP_SCI_CONTROL_BLOCK_TOTAL_NUM (2)

#define FWUP_SCI_RECEIVE_BUFFER_EMPTY (0)
#define FWUP_SCI_RECEIVE_BUFFER_FULL  (1)

#define MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT 3
#define SHA1_HASH_LENGTH_BYTE_SIZE 20

#define	MONITORING_STATUS_INTERVAL	1
#define	MONITORING_STATUS_COUNT		6000

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
#define FWUP_WRITE_BLOCK_SIZE       1024
/* For reduce the ROM size by control printf */
#if (FWUP_CFG_PRINTF_DISABLE == 1)
# define DEBUG_LOG(str)
# define DEBUG_LOG2(fmt, ...)
#else
# define DEBUG_LOG(fmt)  		printf(fmt)
# define DEBUG_LOG2(fmt, ...)	printf(fmt, __VA_ARGS__)
#endif /* FWUP_CFG_PRINTF_DISABLE */
#else
#define FWUP_WRITE_BLOCK_SIZE       1024
#endif /* FWUP_CFG_IMPLEMENTATION_ENVIRONMENT */

#define FLASH_DF_TOTAL_BLOCK_SIZE (FLASH_DF_BLOCK_INVALID - FLASH_DF_BLOCK_0)

#define INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE "hash-sha256"
#define INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE "sig-sha256-ecdsa"

#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#if !defined(MY_BSP_CFG_SERIAL_TERM_SCI)
#error "Error! Need to define MY_BSP_CFG_SERIAL_TERM_SCI in r_bsp_config.h"
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (0)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI0()
#define SCI_CH_serial_term          SCI_CH0
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (1)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI1()
#define SCI_CH_serial_term          SCI_CH1
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (2)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI2()
#define SCI_CH_serial_term          SCI_CH2
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (3)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI3()
#define SCI_CH_serial_term          SCI_CH3
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (4)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI4()
#define SCI_CH_serial_term          SCI_CH4
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (5)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI5()
#define SCI_CH_serial_term          SCI_CH5
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (6)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI6()
#define SCI_CH_serial_term          SCI_CH6
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (7)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI7()
#define SCI_CH_serial_term          SCI_CH7
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (8)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI8()
#define SCI_CH_serial_term          SCI_CH8
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (9)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI9()
#define SCI_CH_serial_term          SCI_CH9
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (10)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI10()
#define SCI_CH_serial_term          SCI_CH10
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (11)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI11()
#define SCI_CH_serial_term          SCI_CH11
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (12)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI12()
#define SCI_CH_serial_term          SCI_CH12
#else
#error "Error! Invalid setting for MY_BSP_CFG_SERIAL_TERM_SCI in r_bsp_config.h"
#endif  // MY_BSP_CFG_SERIAL_TERM_SCI
#endif  // (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#endif  // FWUP_CFG_COMMUNICATION_FUNCTION

#define BOOT_LOADER_MAGIC_CODE "Renesas"
#define BOOT_LOADER_MAGIC_CODE_LENGTH 16

/* The OTA signature algorithm string is specified by the PAL. */
#define OTA_FILE_SIG_KEY_STR_MAX_LENGTH    32
extern const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ];

#define configMAX_SYSCALL_INTERRUPT_PRIORITY    4

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
#define DEFINE_OTA_METHOD_NAME( name )      \
static const char OTA_METHOD_NAME[] = name; \
( void ) OTA_METHOD_NAME;
#define OTA_LOG_L1         printf
#define DEFINE_OTA_METHOD_NAME_L2( name )
#define OTA_LOG_L2( ... )

#define pvPortMalloc    malloc
#define vPortFree       free

/**
 * @constantspage{ota,OTA library}
 *
 * @section ota_constants_err_codes OTA Error Codes
 * @brief OTA Agent error codes returned by OTA agent API.
 *
 * @snippet this define_ota_err_codes
 *
 * OTA agent error codes are in the upper 8 bits of the 32 bit OTA error word, OTA_Err_t.
 *
 * @section ota_constants_err_code_helpers OTA Error Code Helper constants
 * @brief OTA Error code helper constant for extracting the error code from the OTA error returned.
 *
 * @snippet this define_ota_err_code_helpers
 *
 * OTA error codes consist of an agent code in the upper 8 bits of a 32 bit word and sometimes
 * merged with a platform specific code in the lower 24 bits. You must refer to the platform PAL
 * layer in use to determine the meaning of the lower 24 bits.
 */

/* @[define_ota_err_codes] */
#define kOTA_Err_Panic                   0xfe000000UL     /*!< Unrecoverable FW error. Probably should log error and reboot. */
#define kOTA_Err_Uninitialized           0xff000000UL     /*!< The error code has not yet been set by a logic path. */
#define kOTA_Err_None                    0x00000000UL
#define kOTA_Err_SignatureCheckFailed    0x01000000UL     /*!< The signature check failed for the specified file. */
#define kOTA_Err_BadSignerCert           0x02000000UL     /*!< The signer certificate was not readable or zero length. */
#define kOTA_Err_OutOfMemory             0x03000000UL     /*!< General out of memory error. */
#define kOTA_Err_ActivateFailed          0x04000000UL     /*!< The activation of the new OTA image failed. */
#define kOTA_Err_CommitFailed            0x05000000UL     /*!< The acceptance commit of the new OTA image failed. */
#define kOTA_Err_RejectFailed            0x06000000UL     /*!< Error trying to reject the OTA image. */
#define kOTA_Err_AbortFailed             0x07000000UL     /*!< Error trying to abort the OTA. */
#define kOTA_Err_PublishFailed           0x08000000UL     /*!< Attempt to publish a MQTT message failed. */
#define kOTA_Err_BadImageState           0x09000000UL     /*!< The specified OTA image state was out of range. */
#define kOTA_Err_NoActiveJob             0x0a000000UL     /*!< Attempt to set final image state without an active job. */
#define kOTA_Err_NoFreeContext           0x0b000000UL     /*!< There wasn't an OTA file context available for processing. */
#define kOTA_Err_HTTPInitFailed          0x0c000000UL     /*!< Error initializing the HTTP connection. */
#define kOTA_Err_HTTPRequestFailed       0x0d000000UL     /*!< Error sending the HTTP request. */
#define kOTA_Err_FileAbort               0x10000000UL     /*!< Error in low level file abort. */
#define kOTA_Err_FileClose               0x11000000UL     /*!< Error in low level file close. */
#define kOTA_Err_RxFileCreateFailed      0x12000000UL     /*!< The PAL failed to create the OTA receive file. */
#define kOTA_Err_BootInfoCreateFailed    0x13000000UL     /*!< The PAL failed to create the OTA boot info file. */
#define kOTA_Err_RxFileTooLarge          0x14000000UL     /*!< The OTA receive file is too big for the platform to support. */
#define kOTA_Err_NullFilePtr             0x20000000UL     /*!< Attempt to use a null file pointer. */
#define kOTA_Err_MomentumAbort           0x21000000UL     /*!< Too many OTA stream requests without any response. */
#define kOTA_Err_DowngradeNotAllowed     0x22000000UL     /*!< Firmware version is older than the previous version. */
#define kOTA_Err_SameFirmwareVersion     0x23000000UL     /*!< Firmware version is the same as previous. New firmware could have failed to commit. */
#define kOTA_Err_JobParserError          0x24000000UL     /*!< An error occurred during job document parsing. See reason sub-code. */
#define kOTA_Err_FailedToEncodeCBOR      0x25000000UL     /*!< Failed to encode CBOR object. */
#define kOTA_Err_ImageStateMismatch      0x26000000UL     /*!< The OTA job was in Self Test but the platform image state was not. Possible tampering. */
#define kOTA_Err_GenericIngestError      0x27000000UL     /*!< A failure in block ingestion not caused by the PAL. See the error sub code. */
#define kOTA_Err_UserAbort               0x28000000UL     /*!< User aborted the active OTA. */
#define kOTA_Err_ResetNotSupported       0x29000000UL     /*!< We tried to reset the device but the device doesn't support it. */
#define kOTA_Err_TopicTooLarge           0x2a000000UL     /*!< Attempt to build a topic string larger than the supplied buffer. */
#define kOTA_Err_SelfTestTimerFailed     0x2b000000UL     /*!< Attempt to start self-test timer faield. */
#define kOTA_Err_EventQueueSendFailed    0x2c000000UL     /*!< Posting event message to the event queue failed. */
#define kOTA_Err_InvalidDataProtocol     0x2d000000UL     /*!< Job does not have a valid protocol for data transfer. */
/* @[define_ota_err_codes] */

/* @[define_ota_err_code_helpers] */
#define kOTA_PAL_ErrMask             0xffffffUL       /*!< The PAL layer uses the signed low 24 bits of the OTA error code. */
#define kOTA_Main_ErrMask            0xff000000UL     /*!< Mask out all but the OTA Agent error code (high 8 bits). */
#define kOTA_MainErrShiftDownBits    24U              /*!< The OTA Agent error code is the highest 8 bits of the word. */
/* @[define_ota_err_code_helpers] */

/**
 * @brief Log base 2 of the size of the file data block message (excluding the header).
 *
 * 10 bits yields a data block size of 1KB.
 */
#define otaconfigLOG2_FILE_BLOCK_SIZE           10UL

#define bool_t    uint8_t

#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
#endif /* FWUP_CFG_IMPLEMENTATION_ENVIRONMENT */


/*****************************************************************************
Typedef definitions
******************************************************************************/
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
/**
 * @ingroup ota_datatypes_enums
 * @brief OTA Image states.
 *
 * After an OTA update image is received and authenticated, it is logically moved to
 * the Self Test state by the OTA agent pending final acceptance. After the image is
 * activated and tested by your user code, you should put it into either the Accepted
 * or Rejected state by calling @ref ota_function_setimagestate( eOTA_ImageState_Accepted ) or
 * @ref ota_function_setimagestate( eOTA_ImageState_Rejected ). If the image is accepted, it becomes
 * the main firmware image to be booted from then on. If it is rejected, the image is
 * no longer valid and shall not be used, reverting to the last known good image.
 *
 * If you want to abort an active OTA transfer, you may do so by calling the API
 * @ref ota_function_setimagestate( eOTA_ImageState_Aborted ).
 */
typedef enum
{
    eOTA_ImageState_Unknown = 0,  /*!< The initial state of the OTA MCU Image. */
    eOTA_ImageState_Testing = 1,  /*!< The state of the OTA MCU Image post successful download and reboot. */
    eOTA_ImageState_Accepted = 2, /*!< The state of the OTA MCU Image post successful download and successful self_test. */
    eOTA_ImageState_Rejected = 3, /*!< The state of the OTA MCU Image when the job has been rejected. */
    eOTA_ImageState_Aborted = 4,  /*!< The state of the OTA MCU Image after a timeout publish to the stream request fails.
                                   *   Also if the OTA MCU image is aborted in the middle of a stream. */
    eOTA_ImageState_EOL = 5,	  /*!< The state of the OTA MCU Image when the EOL process. */
    eOTA_LastImageState = eOTA_ImageState_EOL
} OTA_ImageState_t;

/**
 * @ingroup ota_datatypes_enums
 * @brief OTA Platform Image State.
 *
 * The image state set by platform implementation.
 */
typedef enum
{
    eOTA_PAL_ImageState_Unknown = 0,
    eOTA_PAL_ImageState_PendingCommit,
    eOTA_PAL_ImageState_Valid,
    eOTA_PAL_ImageState_Invalid,
} OTA_PAL_ImageState_t;

/**
 * @structs{ota,OTA library}
 */

/* A composite cryptographic signature structure able to hold our largest supported signature. */

#define kOTA_MaxSignatureSize    256        /* Max bytes supported for a file signature (2048 bit RSA is 256 bytes). */

typedef struct
{
    uint16_t usSize;                         /* Size, in bytes, of the signature. */
    uint8_t ucData[ kOTA_MaxSignatureSize ]; /* The binary signature data. */
} Sig256_t;

typedef struct OTA_FileContext
{
    uint8_t * pucFilePath; /*!< Local file pathname. */
    union
    {
        int32_t lFileHandle;    /*!< Device internal file pointer or handle.
                                 * File type is handle after file is open for write. */
        #if WIN32
            FILE * pxFile;      /*!< File type is stdio FILE structure after file is open for write. */
        #endif
        uint8_t * pucFile;      /*!< File type is RAM/Flash image pointer after file is open for write. */
    };
    uint32_t ulFileSize;        /*!< The size of the file in bytes. */
    uint32_t ulBlocksRemaining; /*!< How many blocks remain to be received (a code optimization). */
    uint32_t ulFileAttributes;  /*!< Flags specific to the file being received (e.g. secure, bundle, archive). */
    uint32_t ulServerFileID;    /*!< The file is referenced by this numeric ID in the OTA job. */
    uint8_t * pucJobName;       /*!< The job name associated with this file from the job service. */
    uint8_t * pucStreamName;    /*!< The stream associated with this file from the OTA service. */
    Sig256_t * pxSignature;     /*!< Pointer to the file's signature structure. */
    uint8_t * pucRxBlockBitmap; /*!< Bitmap of blocks received (for de-duping and missing block request). */
    uint8_t * pucCertFilepath;  /*!< Pathname of the certificate file used to validate the receive file. */
    uint8_t * pucUpdateUrlPath; /*!< Url for the file. */
    uint8_t * pucAuthScheme;    /*!< Authorization scheme. */
    uint32_t ulUpdaterVersion;  /*!< Used by OTA self-test detection, the version of FW that did the update. */
    bool_t xIsInSelfTest;       /*!< True if the job is in self test mode. */
    uint8_t * pucProtocols;     /*!< Authorization scheme. */
} OTA_FileContext_t;

/**
 * @functionpointers{ota,OTA library}
 */

/**
 * @brief OTA Error type.
 */
typedef uint32_t                 OTA_Err_t;

#elif (FWUP_ENV_COMMUNICATION_FUNCTION == FWUP_COMM_ETHER_AFRTOS)
#endif /* FWUP_CFG_IMPLEMENTATION_ENVIRONMENT */

typedef enum e_fwup_state
{
    FWUP_STATE_INITIALIZING = 1,
    FWUP_STATE_DATA_RECEIVE_START,
    FWUP_STATE_DATA_RECEIVE,
    FWUP_STATE_FLASH_WRITE_WAIT,
    FWUP_STATE_FLASH_WRITE_COMPLETE,
    FWUP_STATE_CHECK_SIGNATURE,
    FWUP_STATE_BANK1_CHECK,
    FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT,
    FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE,
    FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT,
    FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE,
    FWUP_STATE_BANK0_CHECK,
    FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT,
    FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE,
    FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1,
    FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1,
    FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2,
    FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2,
    FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT,
    FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE,
    FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT,
    FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE,
    FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT,
    FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_COMPLETE,
    FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT,
    FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE,
    FWUP_STATE_INSTALL_DATA_FLASH_READ_WAIT,
    FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE,
    FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT,
    FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE,
    FWUP_STATE_BANK0_UPDATE_CHECK,
    FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT,
    FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE,
	FWUP_STATE_EOL_BANK1_ERASE_WAIT,
	FWUP_STATE_EOL_BANK1_ERASE_COMPLETE,
	FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT,
	FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE,
	FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT,
	FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE,
	FWUP_STATE_FINALIZE,
    FWUP_STATE_FATAL_ERROR = 200
} fwup_state_t;

typedef enum e_iflash_err
{
    IFLASH_SUCCESS = 0,           // Normally terminated.
    IFLASH_ERROR,                 // Illegal terminated.
} iflash_err_t;

typedef enum e_exmem_err
{
    EXMEM_SUCCESS = 0,           // Normally terminated.
	EXMEM_SUCCESS_BUSY,			 // Normally terminated (Memory access is busy)
	EXMEM_ERROR,                 // Illegal terminated.
} exmem_err_t;

typedef enum e_comm_err
{
    COMM_SUCCESS = 0,           // Normally terminated.
    COMM_ERROR,                 // Illegal terminated.
} comm_err_t;

typedef enum e_state_monitoring_err
{
    MONI_SUCCESS = 0,           // Normally terminated.
    MONI_ERROR,                 // Illegal terminated.
} state_monitoring_err_t;

typedef enum e_state_monitoring_flag
{
    STATE_MONITORING_IN_PROGRESS = 0,
    STATE_MONITORING_ERROR = -1
} state_monitoring_flag_t;

typedef struct _fwup_state_transition_monitoring {
    uint32_t                check_status_counter;       // Counter for monitoring state.
    state_monitoring_flag_t state_transit_error_flag;   // State transition error flag.
    fwup_state_t            last_secure_boot_state;     // Keep last state.
    bool                    state_transit_flag;         // State transition execution flag.
} fwup_state_transition_monitoring_t;

typedef struct _sci_buffer_control {
   uint8_t buffer[FWUP_WRITE_BLOCK_SIZE];
   uint32_t buffer_occupied_byte_size;
   uint32_t buffer_full_flag;
}SCI_BUFFER_CONTROL;

typedef struct _sci_receive_control_block {
   SCI_BUFFER_CONTROL * p_sci_buffer_control;
   uint32_t total_byte_size;
   uint32_t current_state;
}SCI_RECEIVE_CONTROL_BLOCK;

typedef struct _firmware_update_control_block
{
	uint8_t magic_code[7];
    uint8_t image_flag;
    uint8_t signature_type[32];
    uint32_t signature_size;
    uint8_t signature[256];
    uint32_t dataflash_flag;
    uint32_t dataflash_start_address;
    uint32_t dataflash_end_address;
    uint8_t reserved1[200];
    uint32_t sequence_number;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t execution_address;
    uint32_t hardware_id;
    uint32_t image_size;	// RZ/A2M OTA 2020.03.19 //
//    uint8_t reserved2[236];	// RZ/A2M OTA 2020.03.19 //
    uint8_t reserved2[232];
}FIRMWARE_UPDATE_CONTROL_BLOCK;

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
typedef struct _load_firmware_control_block {
	uint32_t flash_buffer[FWUP_WRITE_BLOCK_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
}LOAD_FIRMWARE_CONTROL_BLOCK;
#else
typedef struct _load_firmware_control_block {
    uint32_t status;
    uint32_t processed_byte;
    uint32_t total_image_length;
    OTA_ImageState_t eSavedAgentState;
    OTA_FileContext_t * OTA_FileContext;
}LOAD_FIRMWARE_CONTROL_BLOCK;
#endif  /* FWUP_CFG_IMPLEMENTATION_ENVIRONMENT */

typedef struct _flash_block
{
    uint32_t offset;
    uint8_t *binary;
    uint32_t length;
}FLASH_BLOCK;

typedef struct _fragmented_flash_block_list
{
    FLASH_BLOCK content;
    struct _fragmented_flash_block_list *next;
}FRAGMENTED_FLASH_BLOCK_LIST;

typedef struct _packet_block_for_queue
{
    uint32_t ulOffset;
    uint32_t length;
    uint8_t *p_packet;
}PACKET_BLOCK_FOR_QUEUE;


#if defined (_UNIT_TEST)
#define FWUP_STATIC
#else
#define FWUP_STATIC static
#endif

#endif /* FWUP_PRIVATE_H */
