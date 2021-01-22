/*
 * Amazon FreeRTOS OTA PAL V1.0.0
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* C Runtime includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "r_fwup_config.h"  /* Firmware update config definitions */

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == 0) /* Bootloader */
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#include "r_smc_entry.h"
#include "r_flash_rx_if.h"
#include "r_sys_time_rx_if.h"
#elif (MCU_SERIES_RZA2)
#include "r_ostm_drv_api.h"
#else
/* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */

#if (FWUP_CFG_COMMUNICATION_FUNCTION == 0) /* FWUP_COMMUNICATION_SCI */
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#include "r_sci_rx_if.h"
#include "r_sci_rx_pinset.h"
#elif (MCU_SERIES_RZA2)
#include "r_scifa_drv_api.h"
#else
/* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */
#endif

#if (FWUP_CFG_USE_EXMEM != 0)
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#include "r_flash_spi_if.h"
#elif (MCU_SERIES_RZA2)
#include "flash_api.h"
#else
/* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */
#endif /* (FWUP_CFG_USE_EXMEM != 0) */

#if (FWUP_CFG_SIGNATURE_VERIFICATION == 0) /* FWUP_SIGNATURE_ECDSA */
#include "base64_decode.h"
#include "code_signer_public_key.h"

/* tinycrypt */
#include "tinycrypt/sha256.h"
#include "tinycrypt/ecc.h"
#include "tinycrypt/ecc_dsa.h"
#include "tinycrypt/constants.h"
#endif
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == 1) /* FWUP_IMPLEMENTATION_NONEOS */
#include "r_smc_entry.h"
#include "r_flash_rx_if.h"
#include "r_sys_time_rx_if.h"

#if (FWUP_CFG_COMMUNICATION_FUNCTION == 0) /* FWUP_COMMUNICATION_SCI */
#include "r_sci_rx_if.h"
#include "r_sci_rx_pinset.h"
#endif

#if (FWUP_CFG_USE_EXMEM != 0)
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#include "r_flash_spi_if.h"
#elif (MCU_SERIES_RZA2)
#include "flash_api.h"
#endif /* (MCU_SERIES_RZA2) */
#endif /* (FWUP_CFG_USE_EXMEM != 0) */

#if (FWUP_CFG_SIGNATURE_VERIFICATION == 0) /* FWUP_SIGNATURE_ECDSA */
#include "base64_decode.h"
#include "code_signer_public_key.h"

/* tinycrypt */
#include "tinycrypt/sha256.h"
#include "tinycrypt/ecc.h"
#include "tinycrypt/ecc_dsa.h"
#include "tinycrypt/constants.h"
#endif
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == 2) /* FWUP_IMPLEMENTATION_AFRTOS */
/* Amazon FreeRTOS include. */
#include "FreeRTOS.h"
#include "aws_iot_ota_agent.h"

#if (FWUP_CFG_SIGNATURE_VERIFICATION == 0) /* FWUP_SIGNATURE_ECDSA */
#include "iot_crypto.h"
#include "iot_pkcs11.h"
#include "aws_ota_codesigner_certificate.h"
#include "aws_ota_agent_config.h"
#endif

/* Renesas RX Driver Package include */
#include "platform.h"
#include "r_flash_rx_if.h"
#include "r_sys_time_rx_if.h"
#endif

#include "r_fwup_if.h"
#include "r_fwup_private.h"


#if (FWUP_CFG_SIGNATURE_VERIFICATION == FWUP_SIGNATURE_ECDSA)
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
const uint8_t code_signer_public_key[] = CODE_SIGNER_PUBLIC_KEY_PEM;
const uint32_t code_signer_public_key_length = sizeof(code_signer_public_key);
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
/* Specify the OTA signature algorithm we support on this platform. */
//const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha1-rsa";   /* FIX ME. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";   /* FIX ME. */
#else
	/* Fix me for other OS environment */
#endif
#endif

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
#if (FWUP_CFG_SIGNATURE_VERIFICATION == FWUP_SIGNATURE_ECDSA)
int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize);
extern void my_sci_callback(void *pArgs);
#endif
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
OTA_Err_t R_FWUP_CreateFileForRx( OTA_FileContext_t * const C );
OTA_Err_t R_FWUP_Abort( OTA_FileContext_t * const C );
int16_t R_FWUP_WriteBlock( OTA_FileContext_t * const C, uint32_t ulOffset, uint8_t * const pacData, uint32_t ulBlockSize );
OTA_Err_t R_FWUP_CloseFile( OTA_FileContext_t * const C );
OTA_Err_t R_FWUP_CheckFileSignature( OTA_FileContext_t * const C );
uint8_t * R_FWUP_ReadAndAssumeCertificate( const uint8_t * const pucCertName, uint32_t * const ulSignerCertSize );
OTA_Err_t R_FWUP_ResetDevice( void );
OTA_Err_t R_FWUP_ActivateNewImage( void );
OTA_Err_t R_FWUP_SetPlatformImageState( OTA_ImageState_t eState );
OTA_PAL_ImageState_t R_FWUP_GetPlatformImageState( void );
OTA_Err_t R_FWUP_GetVersion(void);

static flash_err_t ota_flashing_task( uint8_t *block, uint32_t ulOffset, uint32_t length );
#if (FWUP_CFG_SIGNATURE_VERIFICATION == FWUP_SIGNATURE_ECDSA)
int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize);
#endif
static int32_t ota_context_validate( OTA_FileContext_t * C );
static int32_t ota_context_update_user_firmware_header( OTA_FileContext_t * C );
static void ota_context_close( OTA_FileContext_t * C );
static void ota_flashing_callback(void *event);
static void ota_header_flashing_callback(void *event);
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
static void ota_flashing_task( void * pvParameters );
static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle );
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize );
static int32_t ota_context_validate( OTA_FileContext_t * C );
static int32_t ota_context_update_user_firmware_header( OTA_FileContext_t * C );
static void ota_context_close( OTA_FileContext_t * C );
static void ota_flashing_callback(void *event);
static void ota_header_flashing_callback(void *event);
#else
    /* Fix me for other OS environment */
#endif


/* Abstraction function. */
iflash_err_t fwup_flash_open(void);
iflash_err_t fwup_flash_close(void);
iflash_err_t fwup_flash_set_callback(void *pcallback);
iflash_err_t fwup_flash_get_bank_info(uint32_t *);
void fwup_flash_set_bank_toggle(void);
iflash_err_t fwup_flash_erase(uint32_t, uint32_t);
iflash_err_t fwup_flash_write(uint32_t, uint32_t, uint32_t);
comm_err_t fwup_communication_open(void);
comm_err_t fwup_communication_close(void);
comm_err_t fwup_communication_receive(uint8_t *, uint16_t const);
static void fwup_communication_callback(void *pArgs);
void fwup_update_status(fwup_state_t state);
fwup_state_t fwup_get_status(void);
state_monitoring_err_t fwup_state_monitoring_open(void);
state_monitoring_err_t fwup_state_monitoring_start(void);
state_monitoring_err_t fwup_state_monitoring_close(void);
state_monitoring_flag_t fwup_state_monitoring_is_error(void);
void fwup_state_monitoring_callback(void);
void fwup_interrupts_disable(void);
void fwup_interrupts_enable(void);
void fwup_register_protect_disable(void);
#if (FWUP_CFG_USE_EXMEM != 0)
exmem_err_t fwup_flash_spi_open(void);
exmem_err_t fwup_flash_spi_close(void);
exmem_err_t fwup_flash_spi_erase(uint32_t, uint32_t);
exmem_err_t fwup_flash_spi_write(uint8_t *, uint32_t, uint32_t);
exmem_err_t fwup_flash_spi_read(uint32_t, uint32_t, uint8_t *);
exmem_err_t fwup_flash_spi_polling_erase(void);
exmem_err_t fwup_flash_spi_polling_write(void);
#endif /* (FWUP_CFG_USE_EXMEM != 0) */
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
static void fwup_flash_accesswindow_set(uint32_t, uint32_t)
bool fwup_get_boot_protect(void);
void fwup_set_boot_protect(void);
#endif // FWUP_CFG_BOOT_PROTECT_ENABLE
#else  /* Setting other than Bootloader */
static void fwup_software_delay_ms(uint32_t delay);
#endif  /* FWUP_IMPLEMENTATION_BOOTLOADER */

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT != FWUP_IMPLEMENTATION_BOOTLOADER)  /* Setting other than Bootloader */
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_insert(FRAGMENTED_FLASH_BLOCK_LIST *head, uint32_t offset, uint8_t *binary, uint32_t length);
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_delete(FRAGMENTED_FLASH_BLOCK_LIST *head, uint32_t offset);
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_print(FRAGMENTED_FLASH_BLOCK_LIST *head);
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_assemble(FRAGMENTED_FLASH_BLOCK_LIST *head, FLASH_BLOCK *flash_block);

static volatile LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list;
static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank0 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;
static volatile uint32_t gs_header_flashing_task;

#endif  /* FWUP_IMPLEMENTATION_BOOTLOADER */

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
static sci_hdl_t s_fwup_communication_handle;
static SCI_RECEIVE_CONTROL_BLOCK sci_receive_control_block;
static SCI_BUFFER_CONTROL sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_TOTAL_NUM];
#elif (MCU_SERIES_RZA2)
int_t scifa_handle;
int_t ostm_handle;
scifa_config_t my_scifa_config;
#else
/* Fix me for other MCU series */
#endif
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank1 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;
static bool gs_is_opened = false;
static OTA_FileContext_t g_file_context;
static sci_hdl_t s_fwup_communication_handle;
static SCI_RECEIVE_CONTROL_BLOCK sci_receive_control_block;
static SCI_BUFFER_CONTROL sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_TOTAL_NUM];
#elif (FWUP_ENV_COMMUNICATION_FUNCTION == FWUP_COMM_ETHER_AFRTOS)
static QueueHandle_t xQueue;
static TaskHandle_t xTask;
static xSemaphoreHandle xSemaphoreFlashig;
static xSemaphoreHandle xSemaphoreWriteBlock;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue1;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue2;
#else
    /* Fix me for other OS environment */
#endif

static fwup_state_t fwup_state = FWUP_STATE_INITIALIZING;
static fwup_state_transition_monitoring_t state_transit;
#if (FWUP_CFG_USE_EXMEM != 0)
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
static uint8_t dev_no_serial_flash = FLASH_SPI_DEV0;
#endif /* (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200) */
#endif  /* (FWUP_CFG_USE_EXMEM != 0) */

/*-----------------------------------------------------------*/
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
/***********************************************************************************************************************
 * Function Name: R_FWUP_Open
 *******************************************************************************************************************//**
 * @brief   Firmware Update module Initialization Processing.
 * @retval  FWUP_SUCCESS                Firmware Update module was opened successfully.
 * @retval  FWUP_ERR_ALREADY_OPEN       Firmware Update module is in use by another process.
 * @retval  FWUP_ERR_LESS_MEMORY        The size of the RAM area used by the Firmware Update module is insufficient.
 * @retval  FWUP_ERR_IMAGE_STATE        Platform image status not suitable for firmware update.
 * @retval  FWUP_ERR_FLASH              Detect error of r_flash_rx module.
 * @retval  FWUP_ERR_COMM               Detect error of communication module.
 * @retval  FWUP_ERR_STATE_MONITORING   Detect error of state monitoring module.
 * @details This function provides setting Firmware Update module initialization.
 *          When this function is executed, Firmware Update module is ready to be used.
 */
fwup_err_t R_FWUP_Open(void)
{
    fwup_err_t ret = FWUP_SUCCESS;
    comm_err_t comm_api_error_code = COMM_SUCCESS;
    state_monitoring_err_t state_monitoring_api_error_code = MONI_SUCCESS;
    OTA_Err_t ota_error_code = kOTA_Err_Uninitialized;

    /* Check that the R_FWUP_Open has been executed. */
    if (true == gs_is_opened)
    {
        return FWUP_ERR_ALREADY_OPEN;
    }

    /* Initialize receive buffer */
    sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_A].buffer_full_flag = FWUP_SCI_RECEIVE_BUFFER_EMPTY;
    sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_B].buffer_full_flag = FWUP_SCI_RECEIVE_BUFFER_EMPTY;
    sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_A];
    sci_receive_control_block.current_state = FWUP_SCI_CONTROL_BLOCK_A;

    /* Initialization of Communication module. */
    comm_api_error_code = fwup_communication_open();
    if (COMM_SUCCESS != comm_api_error_code)
    {
        ret = FWUP_ERR_COMM;
    }

    /* Check the Platform image state */
    if (eOTA_PAL_ImageState_Valid != R_FWUP_GetPlatformImageState())
    {
        return FWUP_ERR_IMAGE_STATE;
    }

    /* Set up the configuration of System-timer for check the status transition. */
    state_monitoring_api_error_code = fwup_state_monitoring_open();
    if (MONI_SUCCESS != state_monitoring_api_error_code)
    {
        comm_api_error_code = fwup_communication_close();  // Closing the Communication module.
        return FWUP_ERR_STATE_MONITORING;
    }

    /* Initialization of Flash module. */
    ota_error_code = R_FWUP_CreateFileForRx(&g_file_context);
    if (kOTA_Err_None != ota_error_code)
    {
        comm_api_error_code = fwup_communication_close();                   // Closing the Communication module.
        state_monitoring_api_error_code = fwup_state_monitoring_close();    // Closing the State monitoring module.
        return FWUP_ERR_FLASH;
    }

    fwup_update_status(FWUP_STATE_DATA_RECEIVE_START);
    gs_is_opened = true;
    return ret;
}

/***********************************************************************************************************************
 * Function Name: R_FWUP_Close
 *******************************************************************************************************************//**
 * @brief   Firmware Update module Close Processing.
 * @retval  FWUP_SUCCESS                Firmware Update module was closed successfully.
 * @retval  FWUP_ERR_NOT_OPEN           Firmware Update module is not open.
 * @retval  FWUP_ERR_FLASH              Detect error of r_flash_rx module.
 * @retval  FWUP_ERR_COMM               Detect error of communication module.
 * @retval  FWUP_ERR_STATE_MONITORING   Detect error of state monitoring module.
 * @details This function provides close resouces of Firmware Update module.
 */
fwup_err_t R_FWUP_Close(void)
{
    fwup_err_t ret = FWUP_SUCCESS;

    /* Check that the R_FWUP_Open has not been executed. */
    if (false == gs_is_opened)
    {
        return FWUP_ERR_NOT_OPEN;
    }

    gs_is_opened = false;
    return ret;
}

/***********************************************************************************************************************
 * Function Name: R_FWUP_Operation
 *******************************************************************************************************************//**
 * @brief   Operation of Firmware update module for OS-less environment.
 * @retval  FWUP_SUCCESS                Firmware update completed successfully.
 * @retval  FWUP_FAIL                   Firmware update error occurred.
 * @retval  FWUP_IN_PROGRESS            Firmware update in progress.
 * @retval  FWUP_ERR_NOT_OPEN           Firmware Update module is not open.
 * @retval  FWUP_ERR_IMAGE_STATE        Platform image status not suitable for firmware update.
 * @retval  FWUP_ERR_STATE_MONITORING   The firmware update status has not changed for a certain period of time.
 * @details Operation of Firmware update module for OS-less environment.
 *          - Obtaining firmware data for update from the set communication path
 *          - Programming to flash
 *          - Signature verification
 *          If the return value is "FWUP_IN_PROGRESS", the firmware update is in progress, so call this function again.
 *          If the return value is "FWUP_SUCCESS", the firmware update is complete. The process is transferred to the
 *          updated firmware by performing a software reset.
 *          If the return value is "FWUP_FAIL", it means that the firmware update has failed.
 *          Cancel the error and call this function again.
 */
fwup_err_t R_FWUP_Operation(void)
{
    fwup_err_t ret = FWUP_IN_PROGRESS;
    state_monitoring_err_t state_monitoring_api_error_code = MONI_SUCCESS;
    uint8_t firm_data[FWUP_WRITE_BLOCK_SIZE];
    int32_t i_bytes_written;
    bool write_flag = false;

    DEFINE_OTA_METHOD_NAME( "R_FWUP_Operation" );

    /* Check that the Firmware update module has been opened. */
    if (false == gs_is_opened)
    {
        fwup_update_status(FWUP_STATE_FATAL_ERROR);
        OTA_LOG_L1("[%s] ERROR: Not Open.\r\n", OTA_METHOD_NAME);
        return FWUP_ERR_NOT_OPEN;
    }

    /* Check State transit monitoring flag */
    if (STATE_MONITORING_ERROR == fwup_state_monitoring_is_error())
    {
        fwup_update_status(FWUP_STATE_FATAL_ERROR);
        OTA_LOG_L1("[%s] ERROR: Transit state did not change for more than 1 min.\r\n", OTA_METHOD_NAME);
        return FWUP_ERR_STATE_MONITORING;
    }

    if (FWUP_STATE_DATA_RECEIVE_START == fwup_get_status())
    {
        /* Check the Platform image state */
        if (eOTA_PAL_ImageState_Valid != R_FWUP_GetPlatformImageState())
        {
            fwup_update_status(FWUP_STATE_FATAL_ERROR);
            OTA_LOG_L1("[%s] ERROR: Illegal Image State.\r\n", OTA_METHOD_NAME);
            return FWUP_ERR_IMAGE_STATE;
        }

        /* Start the state monitoring */
        state_monitoring_api_error_code = fwup_state_monitoring_start();
        if (MONI_SUCCESS == state_monitoring_api_error_code)
        {
            fwup_update_status(FWUP_STATE_DATA_RECEIVE);
            OTA_LOG_L1("-------------------------------------------------\r\n");
            OTA_LOG_L1("Firmware update user program\r\n");
            OTA_LOG_L1("-------------------------------------------------\r\n");
            OTA_LOG_L1("Send Update firmware via UART.\r\n");
        }
        else
        {
            return FWUP_ERR_STATE_MONITORING;
        }
    }

    switch (fwup_get_status())
    {
        case FWUP_STATE_INITIALIZING:
            /* Firmware Update module is not open */
            fwup_update_status(FWUP_STATE_FATAL_ERROR);
            OTA_LOG_L1("[%s] ERROR: Not Open.\r\n", OTA_METHOD_NAME);
            ret = FWUP_ERR_NOT_OPEN;
            break;
        case FWUP_STATE_DATA_RECEIVE_START:
            /* Obtaining firmware data for update from the communication module */
            ret = FWUP_IN_PROGRESS;
            break;
        case FWUP_STATE_DATA_RECEIVE:
            /* Write firmware data to Flash */
            if (FWUP_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_A].buffer_full_flag)
            {
                memcpy(firm_data, sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_A].buffer, FWUP_WRITE_BLOCK_SIZE);
                sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_A].buffer_full_flag = FWUP_SCI_RECEIVE_BUFFER_EMPTY;
                write_flag = true;
            }
            else if  (FWUP_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_B].buffer_full_flag)
            {
                memcpy(firm_data, sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_B].buffer, FWUP_WRITE_BLOCK_SIZE);
                sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_B].buffer_full_flag = FWUP_SCI_RECEIVE_BUFFER_EMPTY;
                write_flag = true;
            }

            if (true == write_flag)
            {
                uint32_t u_offset;
                fwup_update_status(FWUP_STATE_FLASH_WRITE_WAIT);
                u_offset = load_firmware_control_block.total_image_length;
                i_bytes_written = R_FWUP_WriteBlock(&g_file_context, load_firmware_control_block.total_image_length ,firm_data, FWUP_WRITE_BLOCK_SIZE);
                if (i_bytes_written < 0)
                {
                    OTA_LOG_L1( "[%s] ERROR: (%d) writing file block\r\n", OTA_METHOD_NAME, i_bytes_written );
                    ret = FWUP_FAIL;
                }
                else
                {
                    OTA_LOG_L1("[%s] Flash Write: Address = 0x%X, length = %d ... ", OTA_METHOD_NAME, (BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + u_offset) ,FWUP_WRITE_BLOCK_SIZE);
                    ret = FWUP_IN_PROGRESS;
                }
            }
            break;
        case FWUP_STATE_FLASH_WRITE_WAIT:
            /* Waiting for writing to Flash */
            ret = FWUP_IN_PROGRESS;
            break;
        case FWUP_STATE_FLASH_WRITE_COMPLETE:
            OTA_LOG_L1("OK\r\n");
            /* Writing to Flash completed */
            if (BOOT_LOADER_TOTAL_UPDATE_SIZE == load_firmware_control_block.total_image_length)
            {
                fwup_update_status(FWUP_STATE_CHECK_SIGNATURE);
                OTA_LOG_L1("[%s] Firmware update to Flash is complete.\r\n", OTA_METHOD_NAME);
            }
            else
            {
                fwup_update_status(FWUP_STATE_DATA_RECEIVE);
            }
            ret = FWUP_IN_PROGRESS;
            break;
        case FWUP_STATE_CHECK_SIGNATURE:
            /* Update Signature information to OTA_FileContext */
            FIRMWARE_UPDATE_CONTROL_BLOCK * p_block_header;
            Sig256_t p_sig;
            p_block_header = (FIRMWARE_UPDATE_CONTROL_BLOCK *)firmware_update_control_block_bank1;
            p_sig.usSize = p_block_header->signature_size;
            memcpy(p_sig.ucData, p_block_header->signature, kOTA_MaxSignatureSize);
            g_file_context.pxSignature = &p_sig;

            /* Perform signature verification, and close process */
            if (kOTA_Err_None == R_FWUP_CloseFile(&g_file_context))
            {
                OTA_LOG_L1("[%s] Check signature of update firmware is complete.\r\n", OTA_METHOD_NAME);
            	R_FWUP_SetPlatformImageState(eOTA_ImageState_Testing);
                fwup_update_status(FWUP_STATE_FINALIZE);
                ret = FWUP_SUCCESS;
            }
            else
            {
                fwup_update_status(FWUP_STATE_FATAL_ERROR);
                ret = FWUP_FAIL;
            }
            break;
        case FWUP_STATE_FINALIZE:
            /* Already Firmware update completed */
            ret = FWUP_SUCCESS;
            break;
        default:
            break;
    }
    return ret;
}

/***********************************************************************************************************************
 * Function Name: R_FWUP_SetEndOfLife
 *******************************************************************************************************************//**
 * @brief   Start End Of Life (EOL) process.
 * @retval  FWUP_SUCCESS                Firmware update completed successfully.
 * @retval  FWUP_ERR_NOT_OPEN           Firmware Update module is not open.
 * @retval  FWUP_ERR_IMAGE_STATE        Platform image status not suitable for EOL process.
 * @retval  FWUP_ERR_FLASH              Detect error of r_flash_rx module.
 * @retval  FWUP_ERR_COMM               Detect error of communication module.
 * @details Start End Of Life (EOL) process.
 */
fwup_err_t R_FWUP_SetEndOfLife(void)
{
	fwup_err_t ret = FWUP_SUCCESS;
    comm_err_t comm_api_error_code = COMM_SUCCESS;
    flash_interrupt_config_t cb_func_info;
	uint8_t block[BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH];
	FIRMWARE_UPDATE_CONTROL_BLOCK * p_block_header;
	uint32_t length;
    flash_err_t flash_err;
	uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
    struct tc_sha256_state_struct xCtx;

    DEFINE_OTA_METHOD_NAME( "R_FWUP_SetEndOfLife" );

    /* Initialization of Communication module. */
    comm_api_error_code = fwup_communication_open();
    if (COMM_SUCCESS != comm_api_error_code)
    {
        ret = FWUP_ERR_COMM;
    }

    fwup_update_status(FWUP_STATE_DATA_RECEIVE);
    OTA_LOG_L1("-------------------------------------------------\r\n");
    OTA_LOG_L1("End Of Life (EOL) process of user program\r\n");
    OTA_LOG_L1("-------------------------------------------------\r\n");

    /* Erase temporary area */
    flash_err = fwup_flash_open();
	if(FLASH_SUCCESS != flash_err)
	{
		ret = FWUP_ERR_FLASH;
	}

	if (FWUP_SUCCESS == ret)
	{
		flash_err = fwup_flash_set_callback((void *)&ota_header_flashing_callback);
		if(FLASH_SUCCESS == flash_err)
		{
			gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;

			if (fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) == FLASH_SUCCESS)
			{
				while (OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
				OTA_LOG_L1( "[%s] erase install area (code flash):OK\r\n", OTA_METHOD_NAME );
			}
			else
			{
				OTA_LOG_L1( "[%s] erase install area (code flash):NG\r\n", OTA_METHOD_NAME );
				ret = FWUP_ERR_FLASH;
			}
		}
		else
		{
			ret = FWUP_ERR_FLASH;
		}
	}

	if (FWUP_SUCCESS == ret)
	{
		/* Set EOL to image flag of bank1. */
		memcpy(block, (const void *)firmware_update_control_block_bank1, BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
		p_block_header = (FIRMWARE_UPDATE_CONTROL_BLOCK *)block;
		p_block_header->image_flag = LIFECYCLE_STATE_EOL;

		/* Write new image_flag and new signature to Header. */
		gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
		length = BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH;
		flash_err = fwup_flash_write((uint32_t)block, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, length);
		if(FLASH_SUCCESS != flash_err)
		{
			ret = FWUP_ERR_FLASH;
		}
		else
		{
			while (OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
			OTA_LOG_L1( "[%s] update bank1 LIFECYCLE_STATE to [LIFECYCLE_STATE_EOL]\r\n", OTA_METHOD_NAME );
#if 0  // DEBUG
    	    OTA_LOG_L1( "[%s] Swap bank...\r\n", OTA_METHOD_NAME );
        	fwup_flash_set_bank_toggle();
#else
        	load_firmware_control_block.eSavedAgentState = eOTA_ImageState_EOL;
#endif // DEBUG
		}
	}
    return ret;
}

/***********************************************************************************************************************
 * Function Name: R_FWUP_SoftwareReset
 *******************************************************************************************************************//**
 * @brief   Execute a software reset.
 * @details Execute a software reset.
 */
void R_FWUP_SoftwareReset(void)
{
    (void)R_FWUP_ActivateNewImage();
}

#endif  // FWUP_IMPLEMENTATION_NONEOS
/*-----------------------------------------------------------*/

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT != FWUP_IMPLEMENTATION_BOOTLOADER)
OTA_Err_t R_FWUP_CreateFileForRx( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "R_FWUP_CreateFileForRx" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);
    OTA_Err_t eResult = kOTA_Err_Uninitialized;
    flash_interrupt_config_t cb_func_info;

    if( C != NULL )
    {
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
        fwup_flash_close();
        if(fwup_flash_open() == FLASH_SUCCESS)
        {
            fwup_flash_set_callback((void *)&ota_header_flashing_callback);
            gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
            if (fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) != FLASH_SUCCESS)
            {
                eResult = kOTA_Err_RxFileCreateFailed;
                OTA_LOG_L1( "[%s] ERROR - R_FLASH_Erase() returns error.\r\n", OTA_METHOD_NAME );
            }
            while(OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
            fwup_flash_close();
            fwup_flash_open();
            fwup_flash_set_callback((void *)&ota_flashing_callback);
            load_firmware_control_block.OTA_FileContext = C;
            load_firmware_control_block.total_image_length = 0;
            load_firmware_control_block.eSavedAgentState = eOTA_ImageState_Unknown;
            OTA_LOG_L1( "[%s] Receive file created.\r\n", OTA_METHOD_NAME );
            C->pucFile = (uint8_t *)&load_firmware_control_block;
            eResult = kOTA_Err_None;
        }
        else
        {
            eResult = kOTA_Err_RxFileCreateFailed;
            OTA_LOG_L1( "[%s] ERROR - R_FLASH_Open() returns error.\r\n", OTA_METHOD_NAME );
        }
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
        if( C->pucFilePath != NULL )
        {
			/* create task/queue/semaphore for flashing */
			xQueue = xQueueCreate(otaconfigMAX_NUM_BLOCKS_REQUEST, sizeof(PACKET_BLOCK_FOR_QUEUE));
			xTaskCreate(ota_flashing_task, "OTA_FLASHING_TASK", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, &xTask);
			xSemaphoreFlashig = xSemaphoreCreateBinary();
			xSemaphoreGive(xSemaphoreFlashig);
			xSemaphoreWriteBlock = xSemaphoreCreateMutex();
			xSemaphoreGive(xSemaphoreWriteBlock);
			fragmented_flash_block_list = NULL;

			fwup_flash_close();
			if(fwup_flash_open() == FLASH_SUCCESS)
			{
				fwup_flash_set_callback((void *)&ota_header_flashing_callback);
				gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
                if (fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) != FLASH_SUCCESS)
				{
					eResult = kOTA_Err_RxFileCreateFailed;
					OTA_LOG_L1( "[%s] ERROR - R_FLASH_Erase() returns error.\r\n", OTA_METHOD_NAME );
				}
				while(OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
				fwup_flash_close();
				fwup_flash_open();
                fwup_flash_set_callback((void *)&ota_flashing_callback);
				load_firmware_control_block.OTA_FileContext = C;
				load_firmware_control_block.total_image_length = 0;
				load_firmware_control_block.eSavedAgentState = eOTA_ImageState_Unknown;
				OTA_LOG_L1( "[%s] Receive file created.\r\n", OTA_METHOD_NAME );
				C->pucFile = (uint8_t *)&load_firmware_control_block;
				fwup_state_monitoring_open();  /* Open State monitoring module */
				fwup_state_monitoring_start();  /* Start State monitoring module */
				fwup_update_status(FWUP_STATE_DATA_RECEIVE_START); /* Update the firmware update status */
				eResult = kOTA_Err_None;
			}
			else
			{
				eResult = kOTA_Err_RxFileCreateFailed;
				OTA_LOG_L1( "[%s] ERROR - R_FLASH_Open() returns error.\r\n", OTA_METHOD_NAME );
			}
		}
        else
        {
            eResult = kOTA_Err_RxFileCreateFailed;
            OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
        }
#else
    /* Fix me for other OS environment */
#endif
    }
    else
    {
        eResult = kOTA_Err_RxFileCreateFailed;
        OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
    }

    return eResult;
}
/*-----------------------------------------------------------*/

OTA_Err_t R_FWUP_Abort( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "R_FWUP_Abort" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

    OTA_Err_t eResult = kOTA_Err_None;

    if( ota_context_validate(C) == R_OTA_ERR_INVALID_CONTEXT )
    {
        eResult = kOTA_Err_FileClose;
    }

	if (kOTA_Err_None == eResult)
	{
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
		/* delete task/queue for flashing */
		if (NULL != xTask)
		{
			vTaskDelete(xTask);
			xTask = NULL;
		}
		if (NULL != xQueue)
		{
			vQueueDelete(xQueue);
			xQueue = NULL;
		}
		if (NULL != xSemaphoreFlashig)
		{
			vSemaphoreDelete(xSemaphoreFlashig);
			xSemaphoreFlashig = NULL;
		}
		if (NULL != xSemaphoreWriteBlock)
		{
			vSemaphoreDelete(xSemaphoreWriteBlock);
			xSemaphoreWriteBlock = NULL;
		}
#else
    /* Fix me for other OS environment */
#endif

		fwup_flash_close();
	}

	ota_context_close(C);
    return eResult;
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
int16_t R_FWUP_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    int16_t sNumBytesWritten = R_OTA_ERR_QUEUE_SEND_FAIL;
	FLASH_BLOCK flash_block;
    static uint8_t flash_block_array[FLASH_CF_MIN_PGM_SIZE];
	uint8_t *packet_buffer;

    DEFINE_OTA_METHOD_NAME( "R_FWUP_WriteBlock" );
	OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
    /* Check State transit monitoring flag */
    if (STATE_MONITORING_ERROR == fwup_state_monitoring_is_error())
    {
        fwup_update_status(FWUP_STATE_FATAL_ERROR);
        OTA_LOG_L1("[%s] ERROR: Transit state did not change for more than 1 min.\r\n", OTA_METHOD_NAME);
        return sNumBytesWritten;
    }
	xSemaphoreTake(xSemaphoreWriteBlock, portMAX_DELAY);
#else
    /* Fix me for other OS environment */
#endif

	if ((!(ulOffset % FLASH_CF_MIN_PGM_SIZE)) && (!(ulBlockSize % FLASH_CF_MIN_PGM_SIZE)))
	{
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
        packet_buffer = malloc(ulBlockSize);
        memcpy(packet_buffer, pacData, ulBlockSize);
	    if (FLASH_SUCCESS == ota_flashing_task(packet_buffer, ulOffset, ulBlockSize))
	    {
            sNumBytesWritten = ( int16_t ) ulBlockSize;
	    }
	    else
	    {
	        free(packet_buffer);
            OTA_LOG_L1("OTA flashing queue send error.\r\n");
            sNumBytesWritten = R_OTA_ERR_QUEUE_SEND_FAIL;
	    }
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
	    fwup_update_status(FWUP_STATE_FLASH_WRITE_WAIT);   /* Update the firmware update status */
	    packet_buffer = pvPortMalloc(ulBlockSize);
	    memcpy(packet_buffer, pacData, ulBlockSize);
	    packet_block_for_queue1.p_packet = packet_buffer;
	    packet_block_for_queue1.ulOffset = ulOffset;
	    packet_block_for_queue1.length = ulBlockSize;
		if(xQueueSend(xQueue, &packet_block_for_queue1, NULL) == pdPASS)
		{
			sNumBytesWritten = ( int16_t ) ulBlockSize;
		}
		else
		{
			vPortFree(packet_block_for_queue1.p_packet);
			OTA_LOG_L1("OTA flashing queue send error.\r\n");
			sNumBytesWritten = R_OTA_ERR_QUEUE_SEND_FAIL;
		}
#else
    /* Fix me for other OS environment */
#endif
	}
	else
	{
		flash_block.binary = flash_block_array;

		fragmented_flash_block_list = fragmented_flash_block_list_insert(fragmented_flash_block_list, ulOffset, pacData, ulBlockSize);

		if (fragmented_flash_block_list != NULL)
		{
			while(1)
			{
				fragmented_flash_block_list = fragmented_flash_block_list_assemble(fragmented_flash_block_list, &flash_block);
				if (flash_block.binary != NULL)
				{
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
                    packet_buffer = malloc(flash_block.length);
                    memcpy(packet_buffer, flash_block.binary, flash_block.length);
			        if (FLASH_SUCCESS != ota_flashing_task(packet_buffer, flash_block.offset, flash_block.length))
			        {
			            free(packet_buffer);
			            OTA_LOG_L1("OTA flashing queue send error.\r\n");
			            sNumBytesWritten = R_OTA_ERR_QUEUE_SEND_FAIL;
			            break;
			        }
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
				    fwup_update_status(FWUP_STATE_FLASH_WRITE_WAIT);   /* Update the firmware update status */
					packet_buffer = pvPortMalloc(flash_block.length);
				    memcpy(packet_buffer, flash_block.binary, flash_block.length);
				    packet_block_for_queue1.p_packet = packet_buffer;
				    packet_block_for_queue1.ulOffset = flash_block.offset;
				    packet_block_for_queue1.length   = flash_block.length;
					if(xQueueSend(xQueue, &packet_block_for_queue1, NULL) != pdPASS)
					{
						vPortFree(packet_block_for_queue1.p_packet);
						OTA_LOG_L1("OTA flashing queue send error.\r\n");
						sNumBytesWritten = R_OTA_ERR_QUEUE_SEND_FAIL;
						break;
					}
#else
    /* Fix me for other OS environment */
#endif
				}
				else
				{
					sNumBytesWritten = ( int16_t ) ulBlockSize;
					break;
				}
			}
			/*----------- finalize phase ----------*/
			fragmented_flash_block_list_print(fragmented_flash_block_list);
		}
		else
		{
			sNumBytesWritten = ( int16_t ) ulBlockSize;
		}
	}

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
	xSemaphoreGive(xSemaphoreWriteBlock);
#else
    /* Fix me for other OS environment */
#endif

	return sNumBytesWritten;
}
/*-----------------------------------------------------------*/

OTA_Err_t R_FWUP_CloseFile( OTA_FileContext_t * const C )
{
	OTA_Err_t eResult = kOTA_Err_None;

    DEFINE_OTA_METHOD_NAME( "R_FWUP_CloseFile" );

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
    /* Check State transit monitoring flag */
    if (STATE_MONITORING_ERROR == fwup_state_monitoring_is_error())
    {
        fwup_update_status(FWUP_STATE_FATAL_ERROR);
        OTA_LOG_L1("[%s] ERROR: Transit state did not change for more than 1 min.\r\n", OTA_METHOD_NAME);
        return kOTA_Err_Panic;
    }
    else
    {
        fwup_update_status(FWUP_STATE_CHECK_SIGNATURE);   /* Update the firmware update status */
    }
#endif
    if( ota_context_validate(C) == R_OTA_ERR_INVALID_CONTEXT )
    {
        eResult = kOTA_Err_FileClose;
    }

    if( C->pxSignature != NULL )
    {
		eResult = R_FWUP_CheckFileSignature(C);
	}
	else
    {
        eResult = kOTA_Err_SignatureCheckFailed;
    }
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
    fwup_flash_close();
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
	if (kOTA_Err_None == eResult)
	{
		/* Update the user firmware header. */
		if (ota_context_update_user_firmware_header(C) == R_OTA_ERR_NONE)
		{
			OTA_LOG_L1( "[%s] User firmware header updated.\r\n", OTA_METHOD_NAME );
		}
		else
		{
			OTA_LOG_L1( "[%s] ERROR: Failed to update the image header.\r\n", OTA_METHOD_NAME );
			eResult = kOTA_Err_FileClose;
		}
		/* delete task/queue for flashing */
		if (NULL != xTask)
		{
			vTaskDelete(xTask);
			xTask = NULL;
		}
		if (NULL != xQueue)
		{
			do
			{
				if (errQUEUE_FULL == xQueueReceive(xQueue, &packet_block_for_queue2, 0))
				{
					break;
				}
				else
				{
					vPortFree(packet_block_for_queue2.p_packet);
				}
			}
			while(1);
			vQueueDelete(xQueue);
			xQueue = NULL;
		}
		if (NULL != xSemaphoreFlashig)
		{
			vSemaphoreDelete(xSemaphoreFlashig);
			xSemaphoreFlashig = NULL;
		}
		if (NULL != xSemaphoreWriteBlock)
		{
			vSemaphoreDelete(xSemaphoreWriteBlock);
			xSemaphoreWriteBlock = NULL;
		}

	    /* Check State transit monitoring flag */
	    if (STATE_MONITORING_ERROR == fwup_state_monitoring_is_error())
	    {
	        fwup_update_status(FWUP_STATE_FATAL_ERROR);
	        OTA_LOG_L1("[%s] ERROR: Transit state did not change for more than 1 min.\r\n", OTA_METHOD_NAME);
	        eResult = kOTA_Err_Panic;
	    }
	    else
	    {
		    fwup_update_status(FWUP_STATE_FINALIZE);   /* Update the firmware update status */
	    }
		fwup_flash_close();
		fwup_state_monitoring_close();
	}
	else
	{
		load_firmware_control_block.eSavedAgentState = eOTA_ImageState_Rejected;
	}
#else
    /* Fix me for other OS environment */
#endif

	ota_context_close(C);
	return eResult;
}
/*-----------------------------------------------------------*/

OTA_Err_t R_FWUP_CheckFileSignature( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "R_FWUP_CheckFileSignature" );

    OTA_Err_t eResult;

#if (FWUP_CFG_SIGNATURE_VERIFICATION == FWUP_SIGNATURE_ECDSA)
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
    int32_t verification_result = -1;
    uint32_t flash_aligned_address = 0;
    uint8_t assembled_flash_buffer[FLASH_CF_MIN_PGM_SIZE];
    flash_err_t flash_err;
    flash_interrupt_config_t cb_func_info;

    if (fragmented_flash_block_list != NULL)
    {
        FRAGMENTED_FLASH_BLOCK_LIST *tmp = fragmented_flash_block_list;
        do
        {
            /* Read one page from flash memory. */
            flash_aligned_address = (uint32_t)((tmp->content.offset & OTA_FLASH_MIN_PGM_SIZE_MASK) + BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS);
            memcpy((uint8_t *)assembled_flash_buffer, (uint8_t *)flash_aligned_address, FLASH_CF_MIN_PGM_SIZE);
            /* Replace length bytes from offset. */
            memcpy(&assembled_flash_buffer[tmp->content.offset], tmp->content.binary, tmp->content.length);
            /* Flashing memory. */
            fwup_flash_close();
            fwup_flash_open();
            fwup_flash_set_callback((void *)&ota_header_flashing_callback);
            gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
            flash_err = fwup_flash_write((uint32_t)assembled_flash_buffer, (uint32_t)flash_aligned_address, FLASH_CF_MIN_PGM_SIZE);
            if(flash_err != FLASH_SUCCESS)
            {
                nop();
            }
            while (OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
            load_firmware_control_block.total_image_length += tmp->content.length;
            tmp = fragmented_flash_block_list_delete(tmp, tmp->content.offset);
        }
        while(tmp != NULL);
    }

    /* Firmware verification for the signature type of bank 1. */
    verification_result = firmware_verification_sha256_ecdsa(
            (const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
            (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
            firmware_update_control_block_bank1->signature,
            firmware_update_control_block_bank1->signature_size);

    if(0 == verification_result)
    {
        eResult = kOTA_Err_None;
    }
    else
    {
        eResult = kOTA_Err_SignatureCheckFailed;
    }

#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
    uint32_t ulSignerCertSize;
    void * pvSigVerifyContext;
    uint8_t * pucSignerCert = NULL;
	uint32_t flash_aligned_address = 0;
	uint8_t assembled_flash_buffer[FLASH_CF_MIN_PGM_SIZE];
    flash_err_t flash_err;
    flash_interrupt_config_t cb_func_info;

	if (fragmented_flash_block_list != NULL)
	{
		FRAGMENTED_FLASH_BLOCK_LIST *tmp = fragmented_flash_block_list;
		do
		{
			/* Read one page from flash memory. */
			flash_aligned_address = ((tmp->content.offset & OTA_FLASH_MIN_PGM_SIZE_MASK) + BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
			memcpy((uint8_t *)assembled_flash_buffer, (uint8_t *)flash_aligned_address, FLASH_CF_MIN_PGM_SIZE);
			/* Replace length bytes from offset. */
			memcpy(&assembled_flash_buffer[tmp->content.offset], tmp->content.binary, tmp->content.length);
			/* Flashing memory. */
			xSemaphoreTake(xSemaphoreFlashig, portMAX_DELAY);
			fwup_flash_close();
			fwup_flash_open();
			cb_func_info.pcallback = ota_header_flashing_callback;
			cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
			R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);
			gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
			flash_err = fwup_flash_write((uint32_t)assembled_flash_buffer, (uint32_t)flash_aligned_address, FLASH_CF_MIN_PGM_SIZE);
			if(flash_err != FLASH_SUCCESS)
			{
				nop();
			}
			while (OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
			xSemaphoreGive(xSemaphoreFlashig);
			load_firmware_control_block.total_image_length += tmp->content.length;
			tmp = fragmented_flash_block_list_delete(tmp, tmp->content.offset);
		}
		while(tmp != NULL);
	}

    /* Verify an ECDSA-SHA256 signature. */
    if( CRYPTO_SignatureVerificationStart( &pvSigVerifyContext, cryptoASYMMETRIC_ALGORITHM_ECDSA,
                                           cryptoHASH_ALGORITHM_SHA256 ) == pdFALSE )
    {
        return kOTA_Err_SignatureCheckFailed;
    }
    else
    {
        OTA_LOG_L1( "[%s] Started %s signature verification, file: %s\r\n", OTA_METHOD_NAME,
                    cOTA_JSON_FileSignatureKey, ( const char * ) C->pucCertFilepath );
        pucSignerCert = R_FWUP_ReadAndAssumeCertificate( C->pucCertFilepath, &ulSignerCertSize );

        if( pucSignerCert == NULL )
        {
            eResult = kOTA_Err_BadSignerCert;
        }
        else
        {
            CRYPTO_SignatureVerificationUpdate( pvSigVerifyContext,
                                                (const uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
                                                load_firmware_control_block.total_image_length);

            if( CRYPTO_SignatureVerificationFinal( pvSigVerifyContext, ( char * ) pucSignerCert, ulSignerCertSize,
                                                   C->pxSignature->ucData, C->pxSignature->usSize ) == pdFALSE )
            {
                OTA_LOG_L1( "[%s] ERROR: Finished %s signature verification, but signature verification failed\r\n", OTA_METHOD_NAME,
                    cOTA_JSON_FileSignatureKey );
                eResult = kOTA_Err_SignatureCheckFailed;
            }
            else
            {
                OTA_LOG_L1( "[%s] PASS: Finished %s signature verification, signature verification passed\r\n", OTA_METHOD_NAME,
                    cOTA_JSON_FileSignatureKey );
                eResult = kOTA_Err_None;
            }
        }
    }

    /* Free the signer certificate that we now own after R_FWUP_ReadAndAssumeCertificate(). */
    if( pucSignerCert != NULL )
    {
        vPortFree( pucSignerCert );
    }
#else
    /* Fix me for other OS environment */
#endif
#else
    /* Fix me for another signature verification algorithm */
#endif
    return eResult;
}
/*-----------------------------------------------------------*/
#if (FWUP_CFG_SIGNATURE_VERIFICATION == FWUP_SIGNATURE_ECDSA)
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
uint8_t * R_FWUP_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    DEFINE_OTA_METHOD_NAME( "R_FWUP_ReadAndAssumeCertificate" );

    uint8_t * pucCertData;
    uint32_t ulCertSize;
    uint8_t * pucSignerCert = NULL;
    CK_RV xResult;

    xResult = prvGetCertificate( ( const char * ) pucCertName, &pucSignerCert, ulSignerCertSize );

    if( ( xResult == CKR_OK ) && ( pucSignerCert != NULL ) )
    {
        OTA_LOG_L1( "[%s] Using cert with label: %s OK\r\n", OTA_METHOD_NAME, ( const char * ) pucCertName );
    }
    else
    {
        OTA_LOG_L1( "[%s] No such certificate file: %s. Using aws_ota_codesigner_certificate.h.\r\n", OTA_METHOD_NAME,
                    ( const char * ) pucCertName );

        /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
        ulCertSize = sizeof( signingcredentialSIGNING_CERTIFICATE_PEM );
        pucSignerCert = pvPortMalloc( ulCertSize + 1 );                       /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
        pucCertData = ( uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

        if( pucSignerCert != NULL )
        {
            memcpy( pucSignerCert, pucCertData, ulCertSize );
            /* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
            pucSignerCert[ ulCertSize ] = 0U;
            *ulSignerCertSize = ulCertSize + 1U;
        }
        else
        {
            OTA_LOG_L1( "[%s] Error: No memory for certificate of size %d!\r\n", OTA_METHOD_NAME, ulCertSize );
        }
    }

    return pucSignerCert;
}
#else
    /* Fix me for other OS environment */
#endif
#else
    /* Fix me for another signature verification algorithm */
#endif

/*-----------------------------------------------------------*/

OTA_Err_t R_FWUP_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME("R_FWUP_ResetDevice");

    OTA_LOG_L1( "[%s] Resetting the device.\r\n", OTA_METHOD_NAME );
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
	if (eOTA_ImageState_EOL == load_firmware_control_block.eSavedAgentState)
	{
		/* If the status is rejected, aborted, or error, swap bank and return to the previous image.
		   Then the boot loader will start and erase the image that failed to update. */
	    OTA_LOG_L1( "[%s] Swap bank...\r\n", OTA_METHOD_NAME );
	    R_BSP_SET_PSW(0);
    	fwup_interrupts_disable();
    	fwup_flash_close();
    	fwup_flash_open();
    	fwup_flash_set_bank_toggle();
    	fwup_register_protect_disable();
	    R_BSP_SoftwareReset();
    	while(1);   /* software reset */
	}
	else
	{
	    /* Software reset issued (Not swap bank) */
	    R_BSP_SET_PSW(0);
	    fwup_interrupts_disable();
	    fwup_register_protect_disable();
	    R_BSP_SoftwareReset();
		while(1);	/* software reset */
	}
#else
    /* Software reset issued (Not swap bank) */
    R_BSP_SET_PSW(0);
    fwup_interrupts_disable();
    fwup_register_protect_disable();
    R_BSP_SoftwareReset();
	while(1);	/* software reset */
#endif  // FWUP_IMPLEMENTATION_NONEOS

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

OTA_Err_t R_FWUP_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME("R_FWUP_ActivateNewImage");

    OTA_LOG_L1( "[%s] Changing the Startup Bank\r\n", OTA_METHOD_NAME );

    /* reset for self testing */
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
    fwup_software_delay_ms(5000);
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
    vTaskDelay(5000);
#else
    /* Fix me for other OS environment */
#endif
	R_FWUP_ResetDevice();	/* no return from this function */

    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

OTA_Err_t R_FWUP_SetPlatformImageState( OTA_ImageState_t eState )
{
	flash_interrupt_config_t cb_func_info;

    DEFINE_OTA_METHOD_NAME( "R_FWUP_SetPlatformImageState" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

    OTA_Err_t eResult = kOTA_Err_Uninitialized;

	/* Save the image state to eSavedAgentState. */
	if (eOTA_ImageState_Testing == load_firmware_control_block.eSavedAgentState)
	{
		switch (eState)
		{
			case eOTA_ImageState_Accepted:
				fwup_flash_close();
				fwup_flash_open();
				fwup_flash_set_callback((void *)&ota_header_flashing_callback);
				gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
				if (fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) == FLASH_SUCCESS)
				{
					while (OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
					OTA_LOG_L1( "[%s] erase install area (code flash):OK\r\n", OTA_METHOD_NAME );
					OTA_LOG_L1( "[%s] Accepted and committed final image.\r\n", OTA_METHOD_NAME );
					eResult = kOTA_Err_None;
				}
				else
				{
					OTA_LOG_L1( "[%s] erase install area (code flash):NG\r\n", OTA_METHOD_NAME );
					OTA_LOG_L1( "[%s] Accepted final image but commit failed (%d).\r\n", OTA_METHOD_NAME );
					eResult = kOTA_Err_CommitFailed;
				}
				break;
			case eOTA_ImageState_Rejected:
				OTA_LOG_L1( "[%s] Rejected image.\r\n", OTA_METHOD_NAME );
				eResult = kOTA_Err_None;
				break;
			case eOTA_ImageState_Aborted:
				OTA_LOG_L1( "[%s] Aborted image.\r\n", OTA_METHOD_NAME );
				eResult = kOTA_Err_None;
				break;
			case eOTA_ImageState_Testing:
				OTA_LOG_L1( "[%s] Testing.\r\n", OTA_METHOD_NAME );
				eResult = kOTA_Err_None;
				break;
			default:
				OTA_LOG_L1( "[%s] Unknown state received %d.\r\n", OTA_METHOD_NAME, ( int32_t ) eState );
		        eResult = kOTA_Err_BadImageState;
				break;
		}
	}
	else
	{
		switch (eState)
		{
			case eOTA_ImageState_Accepted:
				OTA_LOG_L1( "[%s] Not in commit pending so can not mark image valid (%d).\r\n", OTA_METHOD_NAME);
				eResult = kOTA_Err_CommitFailed;
				break;
			case eOTA_ImageState_Rejected:
				OTA_LOG_L1( "[%s] Rejected image.\r\n", OTA_METHOD_NAME );
				eResult = kOTA_Err_None;
				break;
			case eOTA_ImageState_Aborted:
				OTA_LOG_L1( "[%s] Aborted image.\r\n", OTA_METHOD_NAME );
				eResult = kOTA_Err_None;
				break;
			case eOTA_ImageState_Testing:
				OTA_LOG_L1( "[%s] Testing.\r\n", OTA_METHOD_NAME );
				eResult = kOTA_Err_None;
				break;
			default:
				OTA_LOG_L1( "[%s] Unknown state received %d.\r\n", OTA_METHOD_NAME, ( int32_t ) eState );
		        eResult = kOTA_Err_BadImageState;
				break;
		}
	}

	load_firmware_control_block.eSavedAgentState = eState;

    return eResult;
}
/*-----------------------------------------------------------*/

OTA_PAL_ImageState_t R_FWUP_GetPlatformImageState( void )
{
    DEFINE_OTA_METHOD_NAME( "R_FWUP_GetPlatformImageState" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

	OTA_PAL_ImageState_t ePalState = eOTA_PAL_ImageState_Unknown;

	switch (load_firmware_control_block.eSavedAgentState)
	{
		case eOTA_ImageState_Testing:
			ePalState = eOTA_PAL_ImageState_PendingCommit;
			break;
		case eOTA_ImageState_Accepted:
			ePalState = eOTA_PAL_ImageState_Valid;
			break;
		case eOTA_ImageState_Unknown: // Uninitialized image state, assume a factory image
			ePalState = eOTA_PAL_ImageState_Valid;
			break;
		case eOTA_ImageState_Rejected:
		case eOTA_ImageState_Aborted:
		default:
			ePalState = eOTA_PAL_ImageState_Invalid;
			break;
	}

	OTA_LOG_L1("Function call: R_FWUP_GetPlatformImageState: [%d]\r\n", ePalState);
    return ePalState; /*lint !e64 !e480 !e481 I/O calls and return type are used per design. */
}
/*-----------------------------------------------------------*/

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif

static CK_RV prvGetCertificateHandle( CK_FUNCTION_LIST_PTR pxFunctionList,
                                      CK_SESSION_HANDLE xSession,
                                      const char * pcLabelName,
                                      CK_OBJECT_HANDLE_PTR pxCertHandle )
{
    CK_ATTRIBUTE xTemplate;
    CK_RV xResult = CKR_OK;
    CK_ULONG ulCount = 0;
    CK_BBOOL xFindInit = CK_FALSE;

    /* Get the certificate handle. */
    if( 0 == xResult )
    {
        xTemplate.type = CKA_LABEL;
        xTemplate.ulValueLen = strlen( pcLabelName ) + 1;
        xTemplate.pValue = ( char * ) pcLabelName;
        xResult = pxFunctionList->C_FindObjectsInit( xSession, &xTemplate, 1 );
    }

    if( 0 == xResult )
    {
        xFindInit = CK_TRUE;
        xResult = pxFunctionList->C_FindObjects( xSession,
                                                 ( CK_OBJECT_HANDLE_PTR ) pxCertHandle,
                                                 1,
                                                 &ulCount );
    }

    if( CK_TRUE == xFindInit )
    {
        xResult = pxFunctionList->C_FindObjectsFinal( xSession );
    }

    return xResult;
}

/* Note that this function mallocs a buffer for the certificate to reside in,
 * and it is the responsibility of the caller to free the buffer. */
static CK_RV prvGetCertificate( const char * pcLabelName,
                                uint8_t ** ppucData,
                                uint32_t * pulDataSize )
{
    /* Find the certificate */
    CK_OBJECT_HANDLE xHandle;
    CK_RV xResult;
    CK_FUNCTION_LIST_PTR xFunctionList;
    CK_SLOT_ID xSlotId;
    CK_ULONG xCount = 1;
    CK_SESSION_HANDLE xSession;
    CK_ATTRIBUTE xTemplate = { 0 };
    uint8_t * pucCert = NULL;
    CK_BBOOL xSessionOpen = CK_FALSE;

    xResult = C_GetFunctionList( &xFunctionList );

    if( CKR_OK == xResult )
    {
        xResult = xFunctionList->C_Initialize( NULL );
    }

    if( ( CKR_OK == xResult ) || ( CKR_CRYPTOKI_ALREADY_INITIALIZED == xResult ) )
    {
        xResult = xFunctionList->C_GetSlotList( CK_TRUE, &xSlotId, &xCount );
    }

    if( CKR_OK == xResult )
    {
        xResult = xFunctionList->C_OpenSession( xSlotId, CKF_SERIAL_SESSION, NULL, NULL, &xSession );
    }

    if( CKR_OK == xResult )
    {
        xSessionOpen = CK_TRUE;
        xResult = prvGetCertificateHandle( xFunctionList, xSession, pcLabelName, &xHandle );
    }

    if( ( xHandle != 0 ) && ( xResult == CKR_OK ) ) /* 0 is an invalid handle */
    {
        /* Get the length of the certificate */
        xTemplate.type = CKA_VALUE;
        xTemplate.pValue = NULL;
        xResult = xFunctionList->C_GetAttributeValue( xSession, xHandle, &xTemplate, xCount );

        if( xResult == CKR_OK )
        {
            pucCert = pvPortMalloc( xTemplate.ulValueLen );
        }

        if( ( xResult == CKR_OK ) && ( pucCert == NULL ) )
        {
            xResult = CKR_HOST_MEMORY;
        }

        if( xResult == CKR_OK )
        {
            xTemplate.pValue = pucCert;
            xResult = xFunctionList->C_GetAttributeValue( xSession, xHandle, &xTemplate, xCount );

            if( xResult == CKR_OK )
            {
                *ppucData = pucCert;
                *pulDataSize = xTemplate.ulValueLen;
            }
            else
            {
                vPortFree( pucCert );
            }
        }
    }
    else /* Certificate was not found. */
    {
        *ppucData = NULL;
        *pulDataSize = 0;
    }

    if( xSessionOpen == CK_TRUE )
    {
        ( void ) xFunctionList->C_CloseSession( xSession );
    }

    return xResult;
}
#else
    /* Fix me for other OS environment */
#endif

static int32_t ota_context_validate( OTA_FileContext_t * C )
{
	return ( NULL != C );
}

static int32_t ota_context_update_user_firmware_header( OTA_FileContext_t * C )
{
    int32_t ret = R_OTA_ERR_INVALID_CONTEXT;
	uint8_t block[BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH];
	FIRMWARE_UPDATE_CONTROL_BLOCK * p_block_header;
	uint32_t length;
    flash_err_t flash_err;
    flash_interrupt_config_t cb_func_info;
	uint8_t *source_pointer, *destination_pointer;
	uint8_t data_length = 0;

	memcpy(block, (const void *)firmware_update_control_block_bank0, BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
	p_block_header = (FIRMWARE_UPDATE_CONTROL_BLOCK *)block;

	/* Update image flag. */
	p_block_header->image_flag = LIFECYCLE_STATE_TESTING;

	/* Update signature type. */
	memcpy(p_block_header->signature_type, cOTA_JSON_FileSignatureKey,sizeof(cOTA_JSON_FileSignatureKey));

	/* Parse the signature and extract ECDSA-SHA256 signature data. */
	source_pointer = C->pxSignature->ucData;
	destination_pointer = p_block_header->signature;
	data_length = *(source_pointer + 1) + OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH;
	memset(destination_pointer, 0, sizeof(destination_pointer));
	if (OTA_SIGUNATURE_SEQUENCE_TOP_VALUE == *source_pointer)
	{
		source_pointer += OTA_SIGUNATURE_SEQUENCE_INFO_LENGTH;
		while(1)
		{
			if (OTA_SIGUNATURE_INTEGER_VALUE == *source_pointer)
			{
				source_pointer++;
				if (OTA_SIGUNATURE_INCLUDE_NEGATIVE_NUMBER_VALUE == *source_pointer)
				{
					source_pointer += OTA_SIGUNATURE_SKIP;
				}
				else
				{
					source_pointer++;
				}
				memcpy(destination_pointer, source_pointer, OTA_SIGUNATURE_DATA_HALF_LENGTH);
				source_pointer += OTA_SIGUNATURE_DATA_HALF_LENGTH;
				destination_pointer += OTA_SIGUNATURE_DATA_HALF_LENGTH;
				if ((source_pointer - C->pxSignature->ucData) == data_length)
				{
					ret = R_OTA_ERR_NONE;
					break;
				}
			}
			else
			{
				/* parsing error */
				break;
			}
		}
	}

	if (R_OTA_ERR_NONE == ret)
	{
		fwup_flash_close();
		fwup_flash_open();
		fwup_flash_set_callback((void *)&ota_header_flashing_callback);
		gs_header_flashing_task = OTA_FLASHING_IN_PROGRESS;
		length = BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH;
		flash_err = fwup_flash_write((uint32_t)block, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, length);
		if(flash_err != FLASH_SUCCESS)
		{
			nop();
		}
		while (OTA_FLASHING_IN_PROGRESS == gs_header_flashing_task);
	}

	return ret;
}

static void ota_context_close( OTA_FileContext_t * C )
{
    if( NULL != C )
    {
        C->pucFile = NULL;
    }
}

static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_insert(FRAGMENTED_FLASH_BLOCK_LIST *head, uint32_t offset, uint8_t *binary, uint32_t length)
{
	FRAGMENTED_FLASH_BLOCK_LIST *tmp, *current, *previous, *new;

    new = pvPortMalloc(sizeof(FLASH_BLOCK));
    new->content.binary = pvPortMalloc(length);

    if((new != NULL) && (new->content.binary != NULL))
    {
        memcpy(new->content.binary, binary, length);
    	new->content.offset = offset;
    	new->content.length = length;
    	new->next = NULL;

    	/* new head would be returned when head would be specified as NULL. */
    	if(head == NULL)
    	{
    		tmp = new;
    	}
    	else
    	{
			/* search the list to insert new node */
			current = head;
			while(1)
			{
				if((new->content.offset < current->content.offset) || (current == NULL))
				{
					break;
				}
				previous = current;
				current = current->next;
			}
			/* insert the searched node when current != head */
			if(current != head)
			{
				previous->next = new;
				new->next = current;
				tmp = head;
			}
			else
			{
				new->next = current;
				tmp = new;
			}
		}
    }
    else
    {
    	tmp = NULL;
    }
    return tmp;
}

static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_delete(FRAGMENTED_FLASH_BLOCK_LIST *head, uint32_t offset)
{
	FRAGMENTED_FLASH_BLOCK_LIST *tmp = head, *previous = NULL;

	if(head != NULL)
	{
		while(1)
		{
			if(tmp->content.offset == offset)
			{
				break;
			}
			if(tmp->next == NULL)
			{
				tmp = NULL;
				break;
			}
			previous = tmp;
			tmp = tmp->next;
		}
		if(tmp != NULL)
		{
			/* delete target exists on not head, remove specified and return head in this case */
			if(previous != NULL)
			{
				previous->next = tmp->next;
				vPortFree(tmp->content.binary);
				vPortFree(tmp);
				tmp = head;
			}
			else
			{
				/* delete target exists on head with subsequent data, remove head and return specified (as new head) in this case */
				if(head->next != NULL)
				{
					tmp = head->next;
				}
				/* delete target exists on head without subsequent data, remove head and return NULL in this case */
				else
				{
					tmp = NULL;
				}
				vPortFree(head->content.binary);
				vPortFree(head);
			}
		}
	}
	return tmp;
}

static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_print(FRAGMENTED_FLASH_BLOCK_LIST *head)
{
	FRAGMENTED_FLASH_BLOCK_LIST *tmp = head;
	uint32_t total_heap_length, total_list_count;

	total_heap_length = 0;
	total_list_count = 0;

	if(head != NULL)
	{
		while(1){
			total_heap_length += sizeof(FRAGMENTED_FLASH_BLOCK_LIST);
			total_heap_length += tmp->content.length;
			total_list_count++;
			if(tmp->next == NULL)
			{
				break;
			}
			tmp = tmp->next;
		};
	}
    OTA_LOG_L2("FRAGMENTED_FLASH_BLOCK_LIST: total_heap_length = [%d], total_list_count = [%d].\r\n", total_heap_length, total_list_count);

	return tmp;
}

static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_assemble(FRAGMENTED_FLASH_BLOCK_LIST *head, FLASH_BLOCK *flash_block)
{
	FRAGMENTED_FLASH_BLOCK_LIST *tmp = head;
	FRAGMENTED_FLASH_BLOCK_LIST *flash_block_candidate[FLASH_CF_MIN_PGM_SIZE];
	uint32_t assembled_length = 0;
	uint32_t fragmented_length = 0;
	uint32_t loop_counter = 0;
	uint32_t index = 0;

	/* search aligned FLASH_CF_MIN_PGM_SIZE top offset */
	while(1)
	{
		if(!(tmp->content.offset % FLASH_CF_MIN_PGM_SIZE))
		{
			/* extract continuous flash_block candidate */
			assembled_length = 0;
			loop_counter = 0;
			while(1)
			{
				if ((tmp != NULL) && (assembled_length < FLASH_CF_MIN_PGM_SIZE))
				{
					if(loop_counter < FLASH_CF_MIN_PGM_SIZE)
					{
						if((tmp->content.offset + tmp->content.length) > ((tmp->content.offset & OTA_FLASH_MIN_PGM_SIZE_MASK) + FLASH_CF_MIN_PGM_SIZE))
						{
							fragmented_length = (FLASH_CF_MIN_PGM_SIZE - assembled_length);
							assembled_length += fragmented_length;
							flash_block_candidate[loop_counter] = tmp;
					        tmp->next = fragmented_flash_block_list_insert(tmp->next, tmp->content.offset + fragmented_length, &tmp->content.binary[fragmented_length], tmp->content.length - fragmented_length);
							tmp->content.length = fragmented_length;
						}
						else
						{
							assembled_length += tmp->content.length;
							flash_block_candidate[loop_counter] = tmp;
						}
					}
					else
					{
						break;
					}
					tmp = tmp->next;
					loop_counter++;
				}
				else
				{
					break;
				}
			}
		}

		/* break if found completed flash_block_candidate or found end of list */
		if((assembled_length == FLASH_CF_MIN_PGM_SIZE) || (tmp == NULL))
		{
			break;
		}
		/* search next candidate */
		else
		{
			tmp = tmp->next;
		}
	}

	/* assemble flash_block */
	if(assembled_length == FLASH_CF_MIN_PGM_SIZE)
	{
		tmp = head;
		/* remove flash_block from list */
		flash_block->offset = flash_block_candidate[0]->content.offset;
		flash_block->length = assembled_length;
		for(uint32_t i = 0; i < loop_counter; i++)
		{
			memcpy(&flash_block->binary[index], flash_block_candidate[i]->content.binary, flash_block_candidate[i]->content.length);
			index = flash_block_candidate[i]->content.length;
			tmp = fragmented_flash_block_list_delete(tmp, flash_block_candidate[i]->content.offset);
		}
	}
	else
	{
		tmp = head;
		flash_block->offset = NULL;
		flash_block->binary = NULL;
		flash_block->length = NULL;
	}
	return tmp;
}

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
static flash_err_t ota_flashing_task( uint8_t *p_packet, uint32_t ulOffset, uint32_t length )
{
    flash_err_t flash_err;
    static uint8_t block[FWUP_WRITE_BLOCK_SIZE];

    DEFINE_OTA_METHOD_NAME( "ota_flashing_task" );

    memcpy(block, p_packet, length);
    flash_err = fwup_flash_write((uint32_t)block, (uint32_t)(ulOffset + BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS), length);
    if(length != FWUP_WRITE_BLOCK_SIZE)
    {
        nop();
    }
    if(flash_err != FLASH_SUCCESS)
    {
        nop();
    }
    load_firmware_control_block.total_image_length += length;
    free(p_packet);

//    OTA_LOG_L1("[%s] FLASH_Write: Address = 0x%X, length = %d, total_image_length = %d(0x%X).\r\n", OTA_METHOD_NAME, (ulOffset + BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS), length, load_firmware_control_block.total_image_length, load_firmware_control_block.total_image_length);
    return flash_err;
}
#elif (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
static void ota_flashing_task( void * pvParameters )
{
	flash_err_t flash_err;
	static uint8_t block[(1 << otaconfigLOG2_FILE_BLOCK_SIZE)];
	static uint32_t ulOffset;
	static uint32_t length;

	while(1)
	{
		xQueueReceive(xQueue, &packet_block_for_queue2, portMAX_DELAY);
		xSemaphoreTake(xSemaphoreFlashig, portMAX_DELAY);
		memcpy(block, packet_block_for_queue2.p_packet, packet_block_for_queue2.length);
		ulOffset = packet_block_for_queue2.ulOffset;
		length = packet_block_for_queue2.length;
		flash_err = fwup_flash_write((uint32_t)block, ulOffset + BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH, length);
		if(packet_block_for_queue2.length != 1024)
		{
			nop();
		}
		if(flash_err != FLASH_SUCCESS)
		{
			nop();
		}
		load_firmware_control_block.total_image_length += length;
		vPortFree(packet_block_for_queue2.p_packet);
	}
}
#else
    /* Fix me for other OS environment */
#endif

static void ota_flashing_callback(void *event)
{
	uint32_t event_code;
	event_code = *((uint32_t*)event);

    if((event_code != FLASH_INT_EVENT_WRITE_COMPLETE) || (event_code == FLASH_INT_EVENT_ERASE_COMPLETE))
    {
    	nop(); /* trap */
    }
    if (event_code == FLASH_INT_EVENT_WRITE_COMPLETE &&
    		FWUP_STATE_FLASH_WRITE_WAIT == fwup_get_status())
    {
    	fwup_update_status(FWUP_STATE_FLASH_WRITE_COMPLETE);  /* Update the firmware update status */
    }
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_AFRTOS)
    static portBASE_TYPE xHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xSemaphoreFlashig, &xHigherPriorityTaskWoken);
#else
    /* Fix me for other OS environment */
#endif
}

static void ota_header_flashing_callback(void *event)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
	uint32_t event_code;
	event_code = *((uint32_t*)event);

	gs_header_flashing_task = OTA_FLASHING_COMPLETE;

    if((event_code != FLASH_INT_EVENT_WRITE_COMPLETE) || (event_code == FLASH_INT_EVENT_ERASE_COMPLETE))
    {
    	nop(); /* trap */
    }
#else
    /* Fix me for other flash control library */
#endif
}
#endif  // !FWUP_IMPLEMENTATION_BOOTLOADER

/***********************************************************************************************************************
 * Function Name: fwup_flash_open
 *******************************************************************************************************************//**
 * @brief   The function initializes the Flash module.
 *          This function must be called before calling any other API functions.
 * @retval  IFLASH_SUCCESS      Operation successful.
 * @retval  IFLASH_ERROR		Error occured.
 * @details This function initializes the Flash module.
 *          This is a function for abstracting the flash access.
 *          Note that this function must be called before any other API function.
 */
iflash_err_t fwup_flash_open(void)
{
	iflash_err_t	ret = IFLASH_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_err_t my_flash_err = FLASH_SUCCESS;

    my_flash_err = R_FLASH_Open();

    /* If there were an error this would demonstrate error detection of API calls. */
    if (FLASH_SUCCESS != my_flash_err)
    {
        nop(); // Your error handling code would go here.
        ret = IFLASH_ERROR;
    }
#else
    /* Fix me for other flash control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_close
 *******************************************************************************************************************//**
 * @brief   The function closes the Flash module.
 * @retval  IFLASH_SUCCESS      Operation successful.
 * @retval  IFLASH_ERROR		Error occured.
 * @details This function closes the Flash module.
 *          It disables the flash interrupts (if enabled) and sets the driver to an uninitialized state.
 *          This is a function for abstracting the flash access.
 */
iflash_err_t fwup_flash_close(void)
{
	iflash_err_t	ret = IFLASH_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_err_t my_flash_err = FLASH_SUCCESS;

	ret = R_FLASH_Close();

	/* If there were an error this would demonstrate error detection of API calls. */
    if (FLASH_SUCCESS != my_flash_err)
    {
        nop(); // Your error handling code would go here.
        ret = IFLASH_ERROR;
    }
#else
    /* Fix me for other flash control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_set_callback
 *******************************************************************************************************************//**
 * @brief         Register the callback function.
 * @param[in]     *pcallback			Address of Callback function.
 * @retval        IFLASH_SUCCESS        Operation successful.
 * @retval        IFLASH_ERROR			Error occured.
 * @details       Register the callback function.
 *                This is a function for abstracting the flash access.
 */
iflash_err_t fwup_flash_set_callback(void *pcallback)
{
	iflash_err_t	ret = IFLASH_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_err_t my_flash_err = FLASH_SUCCESS;
    flash_interrupt_config_t cb_func_info;

	cb_func_info.pcallback = pcallback;
	cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
	my_flash_err = R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)cb_func_info);

    /* If there were an error this would demonstrate error detection of API calls. */
    if (FLASH_SUCCESS != my_flash_err)
    {
        nop(); // Your error handling code would go here.
        ret = IFLASH_ERROR;
    }
 #else
    /* Fix me for other flash control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_get_bank_info
 *******************************************************************************************************************//**
 * @brief         Read current BANKSEL value.
 * @param[out]    bank_info             Current bank information.
 * @retval        IFLASH_SUCCESS        Operation successful.
 * @retval        IFLASH_ERROR			Error occured.
 * @details       Read current BANKSEL value.
 *                This is a function for abstracting the flash access.
 */
iflash_err_t fwup_flash_get_bank_info(uint32_t *bank_info)
{
	iflash_err_t	ret = IFLASH_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
	flash_err_t ret;

	my_flash_err = R_FLASH_Control(FLASH_CMD_BANK_GET, (flash_bank_t *)bank_info);

    /* If there were an error this would demonstrate error detection of API calls. */
    if (FLASH_SUCCESS != my_flash_err)
    {
        nop(); // Your error handling code would go here.
        ret = IFLASH_ERROR;
    }
#else
    /* Fix me for other flash control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_set_bank_toggle
 *******************************************************************************************************************//**
 * @brief         Switch boot bank.
 * @details       Switch boot bank.
 *                This is a function for abstracting the flash access.
 */
void fwup_flash_set_bank_toggle(void)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    (void)R_FLASH_Control(FLASH_CMD_BANK_TOGGLE, NULL);
#else
    /* Fix me for other flash control library */
#endif
}

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
/***********************************************************************************************************************
 * Function Name: fwup_flash_accesswindow_set
 *******************************************************************************************************************//**
 * @brief         Switch boot bank.
 * @details       Switch boot bank.
 *                This is a function for abstracting the flash access.
 */
static void fwup_flash_accesswindow_set(uint32_t saddr, uint32_t eaddr)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
#if 0  // [Note] Set FSPR bit in R_FLASH_Control(FLASH_CMD_ACCESSWINDOW_SET) function.
	flash_access_window_config_t addr;

	addr.start_addr = saddr;
	addr.end_addr = eaddr;
	(void)R_FLASH_Control(FLASH_CMD_ACCESSWINDOW_SET, &addr);
#endif
#else
    /* Fix me for other flash control library */
#endif
}
#endif /* (FWUP_CFG_BOOT_PROTECT_ENABLE == 1) */
#else
/* Fix me for other OS environment */
#endif /* FWUP_IMPLEMENTATION_BOOTLOADER */

/***********************************************************************************************************************
* Function Name: fwup_flash_erase
 *******************************************************************************************************************//**
 * @brief     This function is used to erase the specified block in code flash or data flash.
 * @param[in] block_start_address Specifies the start address of block to erase.
 *                                The enum flash_block_address_t is defined in the corresponding MCU's
 *                                r_flash_rx\src\targets\mcu\r_flash_mcu.h file.
 *                                The blocks are labeled in the same fashion as they are
 *                                in the device's Hardware Manual.
 *                                For example, the block located at address 0xFFFFC000 is called Block 7
 *                                in the RX113 hardware manual, therefore "FLASH_CF_BLOCK_7" should be passed
 *                                for this parameter.
 *                                Similarly, to erase Data Flash Block 0 which is located at address 0x00100000,
 *                                this argument should be FLASH_DF_BLOCK_0.
 * @param[in] num_blocks          Specifies the number of blocks to be erased.
 *                                For type 1 parts, address + num_blocks cannot cross a 256K boundary.
 * @retval    FLASH_SUCCESS       Operation successful.
 *                                (if non-blocking mode is enabled, this means the operation was started successfully).
 * @retval    FLASH_ERR_BLOCKS    Invalid number of blocks specified.
 * @retval    FLASH_ERR_ADDRESS   Invalid address specified.
 * @retval    FLASH_ERR_BUSY      A different flash process is being executed, or the module is not initialized.
 * @retval    FLASH_ERR_FAILURE   Erasing failure. Sequencer has been reset.
 *                                Or callback function not registered (in non-blocking mode)
 * @details   Erases a contiguous number of code flash or data flash memory blocks.
 *            The block size varies depending on MCU types.
 *            The equates FLASH_CF_BLOCK_SIZE for code flash and FLASH_DF_BLOCK_SIZE
 *            for data flash are provided for these values.
 *            The enum flash_block_address_t is configured at compile time based on the memory configuration of
 *            the MCU device specified in the r_bsp module.
 *            This is a function for abstracting the flash access.
 */
iflash_err_t fwup_flash_erase(uint32_t block_start_address, uint32_t num_blocks)
{
	iflash_err_t	ret = IFLASH_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
	flash_err_t my_flash_err;

	my_flash_err = R_FLASH_Erase((flash_block_address_t)block_start_address, num_blocks);

    /* If there were an error this would demonstrate error detection of API calls. */
    if (FLASH_SUCCESS != my_flash_err)
    {
        nop(); // Your error handling code would go here.
        ret = IFLASH_ERROR;
    }
#else
    /* Fix me for other flash control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_write
 *******************************************************************************************************************//**
 * @brief     This function is used to write data to code flash or data flash.
 * @param[in] src_address       This is the first address  of the buffer containing the data to write to Flash.
 * @param[in] dest_address      This is the first address  of the code flash or data flash area to rewrite data.
 *                              The address specified must be divisible by the minimum programming size.
 *                              See Description below for important restrictions regarding this parameter.
 * @param[in] num_bytes         The number of bytes contained in the buffer specified with src_address.
 *                              This number must be a multiple of the minimum programming size
 *                              for memory area you are writing to.
 * @retval    FLASH_SUCCESS     Operation successful.
 *                              (in non-blocking mode, this means the operation was started successfully)
 * @retval    FLASH_ERR_FAILURE Programming failed.
 *                              Possibly the destination address under access window or lockbit control;
 *                              or callback function not present(in non-blocking mode)
 * @retval    FLASH_ERR_BUSY    A different flash process is being executed or the module is not initialized.
 * @retval    FLASH_ERR_BYTES   Number of bytes provided was not a multiple of the minimum programming size
 *                              or exceed the maximum range.
 * @retval    FLASH_ERR_ADDRESS Invalid address was input or address not divisible by the minimum programming size.
 * @details   Writes data to flash memory. Before writing to any flash area, the area must already be erased.
 *            When performing a write the user must make sure to start the write on an address divisible
 *            by the minimum programming size and make the number of bytes to write be a multiple of the minimum
 *            programming size. The minimum programming size differs depending on what MCU package is being used and
 *            whether the code flash or data flash is being written to.
 *            This is a function for abstracting the flash access.
 */
iflash_err_t fwup_flash_write(uint32_t src_address, uint32_t dest_address, uint32_t num_bytes)
{
	iflash_err_t	ret = IFLASH_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_err_t ret;

    my_flash_err = R_FLASH_Write(src_address, dest_address, num_bytes);

    /* If there were an error this would demonstrate error detection of API calls. */
    if (FLASH_SUCCESS != my_flash_err)
    {
        nop(); // Your error handling code would go here.
        ret = IFLASH_ERROR;
    }
#else
    /* Fix me for other flash control library */
#endif
    return ret;
}

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT != FWUP_IMPLEMENTATION_AFRTOS)
/***********************************************************************************************************************
 * Function Name: fwup_communication_open
 *******************************************************************************************************************//**
 * @brief   The function initializes the Communication module.
 *          This function must be called before calling any other API functions.
 * @retval  COMM_SUCCESS          Communication module initialized successfully.
 * @retval  COMM_ERROR            The communication module initialization has terminated Illegaly.
 * @details This function initializes the Communication module.
 *          This is a function for abstracting the Communication for firmware data download.
 *          Note that this function must be called before any other API function.
 */
comm_err_t fwup_communication_open(void)
{
    comm_err_t ret = COMM_SUCCESS;
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI.
    R_SCI_PinSet_serial_term();

    sci_cfg_t   my_sci_config;
    sci_err_t   my_sci_err;

    /* Set up the configuration data structure for asynchronous (UART) operation. */
    my_sci_config.async.baud_rate    = MY_BSP_CFG_SERIAL_TERM_SCI_BITRATE;
    my_sci_config.async.clk_src      = SCI_CLK_INT;
    my_sci_config.async.data_size    = SCI_DATA_8BIT;
    my_sci_config.async.parity_en    = SCI_PARITY_OFF;
    my_sci_config.async.parity_type  = SCI_EVEN_PARITY;
    my_sci_config.async.stop_bits    = SCI_STOPBITS_1;
    my_sci_config.async.int_priority = MY_BSP_CFG_SERIAL_TERM_SCI_INTERRUPT_PRIORITY;

    /* OPEN ASYNC CHANNEL
    *  Provide address of the configure structure,
    *  the callback function to be assigned,
    *  and the location for the handle to be stored.*/
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
    my_sci_err = R_SCI_Open(SCI_CH_serial_term, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &s_fwup_communication_handle);
#else
    my_sci_err = R_SCI_Open(SCI_CH_serial_term, SCI_MODE_ASYNC, &my_sci_config, fwup_communication_callback, &s_fwup_communication_handle);
#endif /* FWUP_IMPLEMENTATION_BOOTLOADER */

    /* If there were an error this would demonstrate error detection of API calls. */
    if (SCI_SUCCESS != my_sci_err)
    {
        nop(); // Your error handling code would go here.
        ret = COMM_ERROR;
    }
#else
    /* Fix me for other communication module */
#endif
#elif (MCU_SERIES_RZA2)
	/* UART経由のアプリケーションダウンロードに SCIFA4を使用 */	// RZ/A2M OTA 2020.03.19 //
	scifa_handle = direct_open("scifa4", 0);
	direct_control(scifa_handle, CTL_SCIFA_GET_CONFIGURATION, &my_scifa_config);

	my_scifa_config.baud_rate = 115200;

	direct_control(scifa_handle, CTL_SCIFA_SET_CONFIGURATION, &my_scifa_config);
#else
    /* Fix me for other MCU series */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_communication_close
 *******************************************************************************************************************//**
 * @brief   The function closes the Communication module.
 *          This function must be called before calling any other API functions.
 * @retval  COMM_SUCCESS          Communication module closed successfully.
 * @retval  COMM_ERROR            Closing the communication module has terminated Illegally.
 * @details This function closes the Communication module.
 *          This is a function for abstracting the Communication for firmware data download.
 */
comm_err_t fwup_communication_close(void)
{
    comm_err_t ret = COMM_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI.
    sci_err_t   my_sci_err;

    my_sci_err = R_SCI_Close(s_fwup_communication_handle);

    /* If there were an error this would demonstrate error detection of API calls. */
    if (SCI_SUCCESS != my_sci_err)
    {
        nop(); // Your error handling code would go here.
        ret = COMM_ERROR;
    }
#else
    /* Fix me for other communication module */
#endif
#elif (MCU_SERIES_RZA2)
	direct_close(scifa_handle);
#else
    /* Fix me for other MCU series */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_communication_receive
 *******************************************************************************************************************//**
 * @brief   The function to receive data from Communication module.
 * @retval  COMM_SUCCESS          Data received successfully from Communication module.
 * @retval  COMM_ERROR            Closing the communication module has terminated Illegally.
 * @details The function to receive data from Communication module.
 *          This is a function for abstracting the Communication for firmware data download.
 */
comm_err_t fwup_communication_receive(uint8_t *p_dst, uint16_t const length)
{
    comm_err_t ret = COMM_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI.
    sci_err_t   my_sci_err;

    my_sci_err = R_SCI_Close(s_fwup_communication_handle);

    /* If there were an error this would demonstrate error detection of API calls. */
    if (SCI_SUCCESS != my_sci_err)
    {
        nop(); // Your error handling code would go here.
        ret = COMM_ERROR;
    }
#else
    /* Fix me for other communication module */
#endif
#elif (MCU_SERIES_RZA2)
	direct_read(scifa_handle, p_dst, length);
#else
    /* Fix me for other MCU series */
#endif
    return ret;
}

static void fwup_communication_callback(void *pArgs)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;

#if (FWUP_CFG_COMMUNICATION_FUNCTION == FWUP_COMMUNICATION_SCI)  // Case of SCI.
    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        if(sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size < sizeof(sci_receive_control_block.p_sci_buffer_control->buffer) &&
                sci_receive_control_block.p_sci_buffer_control->buffer_full_flag == FWUP_SCI_RECEIVE_BUFFER_EMPTY)
        {
            R_SCI_Receive(p_args->hdl, &sci_receive_control_block.p_sci_buffer_control->buffer[sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size++], 1);
            if (sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size == sizeof(sci_receive_control_block.p_sci_buffer_control->buffer))
            {
                sci_receive_control_block.total_byte_size += sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size;
                sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size = 0;
                sci_receive_control_block.p_sci_buffer_control->buffer_full_flag = FWUP_SCI_RECEIVE_BUFFER_FULL;
                if (FWUP_SCI_CONTROL_BLOCK_A == sci_receive_control_block.current_state)
                {
                    sci_receive_control_block.current_state = FWUP_SCI_CONTROL_BLOCK_B;
                    sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_B];
                }
                else
                {
                    sci_receive_control_block.current_state = FWUP_SCI_CONTROL_BLOCK_A;
                    sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[FWUP_SCI_CONTROL_BLOCK_A];
                }
            }
        }
    }
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
        nop();
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
        nop();
    }
    else
    {
        /* Do nothing */
    }
#else
    /* Fix me for other communication module */
#endif
#else
    /* Fix me for other MCU series */
#endif
} /* End of function fwup_communication_callback() */
#else
	/* Fix me for other OS environment */
#endif

/***********************************************************************************************************************
 * Function Name: fwup_update_status
 *******************************************************************************************************************//**
 * @brief   Update the Firmware update status.
 * @details Update the Firmware update status.
 *          Also update state transit flag.
 */
void fwup_update_status(fwup_state_t state)
{
    fwup_state = state;
    state_transit.state_transit_flag = true;
}

/***********************************************************************************************************************
 * Function Name: fwup_get_status
 *******************************************************************************************************************//**
 * @brief   Return the current Firmware update status.
 * @brief   Return the current Firmware update status.
 */
fwup_state_t fwup_get_status(void)
{
    return fwup_state;
}

/***********************************************************************************************************************
 * Function Name: fwup_state_monitoring_open
 *******************************************************************************************************************//**
 * @brief   Open the State transit monitoring module.
 * @retval  MONI_SUCCESS          State transit monitoring module initialized successfully.
 * @retval  MONI_ERROR            The State transit monitoring module initialization has terminated Illegaly.
 * @details Open the State transit monitoring module, and some initial settings.
 *          This is a function for abstracting the State transit monitoring module.
 */
state_monitoring_err_t fwup_state_monitoring_open(void)
{
    state_monitoring_err_t ret = MONI_SUCCESS;
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    sys_time_err_t sys_time_api_error_code;

    sys_time_api_error_code = R_SYS_TIME_Open();
    if (SYS_TIME_SUCCESS != sys_time_api_error_code)
    {
        ret = MONI_ERROR;
    }
#else
    /* Fix me for other State transit monitoring module */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_state_monitoring_start
 *******************************************************************************************************************//**
 * @brief   Start status monitoring.
 * @retval  MONI_SUCCESS          State transit monitoring module started successfully.
 * @retval  MONI_ERROR            Start of the State transit monitoring module has terminated Illegaly.
 * @details Start the status monitoring by State transit monitoring module.
 *          This is a function for abstracting the State transit monitoring module.
 */
state_monitoring_err_t fwup_state_monitoring_start(void)
{
    state_monitoring_err_t ret = MONI_SUCCESS;
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    sys_time_err_t sys_time_api_error_code;

    /* Set System-timer for check status */
    state_transit.check_status_counter = 0;
    state_transit.state_transit_error_flag = STATE_MONITORING_IN_PROGRESS;
    state_transit.last_secure_boot_state = fwup_state;
    state_transit.state_transit_flag = true;
    sys_time_api_error_code = R_SYS_TIME_RegisterPeriodicCallback(fwup_state_monitoring_callback, MONITORING_STATUS_INTERVAL);
    if (SYS_TIME_SUCCESS != sys_time_api_error_code)
    {
        ret = MONI_ERROR;
    }
#elif (MCU_SERIES_RZA2)
    ostm_handle = direct_open("ostm0", 0);
#else
    /* Fix me for other State transit monitoring module */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_state_monitoring_close
 *******************************************************************************************************************//**
 * @brief   Close the all callback process of State transit monitoring module.
 * @retval  MONI_SUCCESS          State transit monitoring module closed successfully.
 * @retval  MONI_ERROR            Closing the State transit monitoring module has terminated Illegally.
 * @details Close the all callback process of State transit monitoring module.
 *          This is a function for abstracting the System timer module.
 */
state_monitoring_err_t fwup_state_monitoring_close(void)
{
    state_monitoring_err_t ret = MONI_SUCCESS;
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    sys_time_err_t sys_time_api_error_code;

    if (true == R_SYS_TIME_IsPeriodicCallbackRegistered(fwup_state_monitoring_callback))
    {
        sys_time_api_error_code = R_SYS_TIME_UnregisterPeriodicCallback(fwup_state_monitoring_callback);
        if (SYS_TIME_SUCCESS != sys_time_api_error_code)
        {
            ret = MONI_ERROR;
        }

        sys_time_api_error_code = R_SYS_TIME_Close();
        if (SYS_TIME_SUCCESS != sys_time_api_error_code)
        {
            ret = MONI_ERROR;
        }
    }
#elif (MCU_SERIES_RZA2)
    ostm_handle = direct_close("ostm0");
#else
    /* Fix me for other State transit monitoring module */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_state_monitoring_is_error
 *******************************************************************************************************************//**
 * @brief   Start status monitoring.
 * @retval  MONI_SUCCESS          State transit monitoring module started successfully.
 * @retval  MONI_ERROR            Start of the State transit monitoring module has terminated Illegaly.
 * @details Start the status monitoring by State transit monitoring module.
 *          This is a function for abstracting the State transit monitoring module.
 */
state_monitoring_flag_t fwup_state_monitoring_is_error(void)
{
    state_monitoring_flag_t ret = STATE_MONITORING_IN_PROGRESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    if (true == R_SYS_TIME_IsPeriodicCallbackRegistered(fwup_state_monitoring_callback))
    {
        ret = state_transit.state_transit_error_flag;
    }
#elif (MCU_SERIES_RZA2)
    ret = state_transit.state_transit_error_flag;
#else
    /* Fix me for other State transit monitoring module */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_state_monitoring_callback
 *******************************************************************************************************************//**
 * @brief   Callback function of State transit monitoring module.
 * @details Callback function of State transit monitoring module..
 */
void fwup_state_monitoring_callback(void)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200||MCU_SERIES_RZA2)
    if (fwup_state == state_transit.last_secure_boot_state &&
            false == state_transit.state_transit_flag &&
			FWUP_STATE_FINALIZE != fwup_get_status())
    {
        /* Status NOT changed */
        state_transit.check_status_counter++;
        if (state_transit.check_status_counter >= MONITORING_STATUS_COUNT)
        {
            state_transit.state_transit_error_flag = STATE_MONITORING_ERROR;
        }
    }
    else
    {
        /* Status chenged, or Status is same but changed */
        state_transit.last_secure_boot_state = fwup_get_status();
        state_transit.check_status_counter = 0;
        state_transit.state_transit_flag = false;
    }
#else
    /* Fix me for other State transit monitoring module */
#endif
}

/***********************************************************************************************************************
 * Function Name: fwup_interrupts_disable
 *******************************************************************************************************************//**
 * @brief   Set interrupt disable.
 * @details Set interrupt disable.
 */
void fwup_interrupts_disable(void)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    R_BSP_InterruptsDisable();
#elif (MCU_SERIES_RZA2)
    (void)R_OS_SysLock();
#else
    /* Fix me for other BSP module */
#endif
}

/***********************************************************************************************************************
 * Function Name: fwup_interrupts_enable
 *******************************************************************************************************************//**
 * @brief   Set interrupt enable.
 * @details Set interrupt enable.
 */
void fwup_interrupts_enable(void)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    R_BSP_InterruptsEnable();
#elif (MCU_SERIES_RZA2)
    (void)R_OS_SysUnlock();
#else
    /* Fix me for other BSP module */
#endif
}

/***********************************************************************************************************************
 * Function Name: fwup_register_protect_disable
 *******************************************************************************************************************//**
 * @brief   Callback function of State transit monitoring module.
 * @details Callback function of State transit monitoring module..
 */
void fwup_register_protect_disable(void)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
#else
    /* Fix me for other BSP module */
#endif
}

#if (FWUP_CFG_USE_EXMEM != 0x0)
/***********************************************************************************************************************
 * Function Name: fwup_flash_spi_open
 *******************************************************************************************************************//**
 * @brief   The function initializes the external memory.
 * @retval  EXMEM_SUCCESS   Operation successful.
 * @retval  EXMEM_ERROR		Error occured.
 * @details This function initializes the external memory.
 *			It also performs polling processing to confirm the completion of opening.
 *          This is a function for abstracting the external memory access.
 */
exmem_err_t fwup_flash_spi_open(void)
{
	exmem_err_t	ret = EXMEM_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_spi_status_t my_flash_err = FLASH_SPI_SUCCESS;

    my_flash_err = R_FLASH_SPI_Open(FWUP_FLASH_SPI_DEV);
    if (FLASH_SPI_SUCCESS == my_flash_err)
    {
        /* nothing to do */
    	do
    	{
    		my_flash_err = fwup_flash_spi_polling_erase();
    	}
    	while (FLASH_SPI_SUCCESS != my_flash_err);
    	ret = R_FLASH_SPI_Set_4byte_Address_Mode(FWUP_FLASH_SPI_DEV);
    }
    else
    {
        nop(); // Your error handling code would go here.
        ret = EXMEM_ERROR;
    }
#else
    /* Fix me for other flash spi control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_spi_close
 *******************************************************************************************************************//**
 * @brief   The function closes the external memory.
 * @retval  EXMEM_SUCCESS   Operation successful.
 * @retval  EXMEM_ERROR		Error occurred.
 * @details This function closes the external memory.
 *          This is a function for abstracting the external memory access.
 */
exmem_err_t fwup_flash_spi_close(void)
{
	exmem_err_t	ret = EXMEM_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_spi_status_t my_flash_err = FLASH_SPI_SUCCESS;

    my_flash_err = R_FLASH_SPI_Close(FWUP_FLASH_SPI_DEV);

	/* If there were an error this would demonstrate error detection of API calls. */
    if (FLASH_SPI_SUCCESS != my_flash_err)
    {
        nop(); // Your error handling code would go here.
        ret = EXMEM_ERROR;
    }
#else
    /* Fix me for other flash spi control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_spi_erase
 *******************************************************************************************************************//**
 * @brief   The function erase the external memory.
 * @param[in] addr			This is the erae start address of the external memory.
 * @param[in] cnt			Number of bytes to be erase.
 * @retval  EXMEM_SUCCESS   Operation successful.
 * @retval  EXMEM_ERROR		Error occurred.
 * @details This function erase the external memory.
 * 			After this function, polling process is required to confirm the completion of erasure.
 *          This is a function for abstracting the external memory access.
 */
exmem_err_t fwup_flash_spi_erase(uint32_t addr, uint32_t cnt)
{
	exmem_err_t	ret = EXMEM_SUCCESS;
    uint32_t i;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_spi_status_t my_flash_err = FLASH_SPI_SUCCESS;
	flash_spi_erase_info_t erase_info;

	erase_info.mode = FLASH_SPI_MODE_S_ERASE; /* Erase by Sector size (4KB) */

	for (i = 0; i < cnt; i += SF_SECTOR_SIZE)
	{
		/* Start of erase serial flash */
		erase_info.addr = addr + i;
		my_flash_err = R_FLASH_SPI_Erase(FWUP_FLASH_SPI_DEV, erase_info);

		/* Wait of erase serial flash */
	    if (FLASH_SPI_SUCCESS == my_flash_err)
	    {
	    	do
	    	{
	    		my_flash_err = R_FLASH_SPI_Polling(FWUP_FLASH_SPI_DEV, FLASH_SPI_MODE_ERASE_POLL);
	    	}
	    	while (FLASH_SPI_SUCCESS_BUSY == my_flash_err);

	    	/* Error check*/
	    	if (FLASH_SPI_SUCCESS != my_flash_err)
	    	{
	            ret = EXMEM_ERROR;
	            break;
	    	}
	    }
	    else
	    {
	    	ret = EXMEM_ERROR;
	    	break;
	    }
	}
#elif (MCU_SERIES_RZA2)
	for (i = 0; i < cnt; i += SF_SECTOR_SIZE)
	{
		flash_erase_sector(NULL, addr + i);
	}
#else
    /* Fix me for other flash spi control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_spi_write
 *******************************************************************************************************************//**
 * @brief   The function write the external memory.
 * @param[in] p_data		Write data storage buffer pointer.
 * @param[in] addr			This is the start address of the external memory.
 * @param[in] cnt			Number of bytes to be written.
 * @retval  EXMEM_SUCCESS   Operation successful.
 * @retval  EXMEM_ERROR		Error occured.
 * @details This function write the external memory.
 * 			After this function, polling process is required to confirm the completion of writing.
 *          This is a function for abstracting the external memory access.
 */
exmem_err_t fwup_flash_spi_write(uint8_t *p_data, uint32_t addr, uint32_t cnt)
{
	exmem_err_t	ret = EXMEM_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_spi_status_t my_flash_err = FLASH_SPI_SUCCESS;
	flash_spi_info_t info;

	info.addr = addr;
	info.cnt = cnt;
	info.p_data = p_data;
	info.op_mode = FLASH_SPI_SINGLE;
	my_flash_err = R_FLASH_SPI_Write_Data_Page(FWUP_FLASH_SPI_DEV, &info);

	/* Wait of write serial flash */
    if (FLASH_SPI_SUCCESS == my_flash_err)
    {
    	do
    	{
    		my_flash_err = R_FLASH_SPI_Polling(FWUP_FLASH_SPI_DEV, FLASH_SPI_MODE_ERASE_POLL);
    	}
    	while (FLASH_SPI_SUCCESS_BUSY == my_flash_err);

    	/* Error check*/
    	if (FLASH_SPI_SUCCESS != my_flash_err)
    	{
            ret = EXMEM_ERROR;
    	}
    }
    else
    {
    	ret = EXMEM_ERROR;
    }
#elif (MCU_SERIES_RZA2)
	int32_t fl_ret;

	fl_ret = flash_program_page(NULL, addr, p_data, cnt);
	if(fl_ret == -1)
	{
		ret = EXMEM_ERROR;
	}
#else
    /* Fix me for other flash spi control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_spi_read
 *******************************************************************************************************************//**
 * @brief   The function read the Flash SPI module.
 * @param[in] dev_no			Device No.
 * @param[in] addr				This is the start address of the serial flash.
 * @param[in] cnt				Number of bytes to be written.
 * @param[in] p_data			Read data storage buffer pointer.
 * @retval  SFLASH_SUCCESS      Operation successful.
 * @retval  SFLASH_ERROR		Error occured.
 * @details This function read the Flash SPI module.
 * 			After this function, polling process is required to confirm the completion of writing.
 *          This is a function for abstracting the flash SPI access.
 */
exmem_err_t fwup_flash_spi_read(uint32_t addr, uint32_t cnt, uint8_t *p_data)
{
	exmem_err_t	ret = EXMEM_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_spi_status_t my_flash_err = FLASH_SPI_SUCCESS;
	flash_spi_info_t info;

	info.addr = addr;
	info.cnt = cnt;
	info.p_data = p_data;
	info.op_mode = FLASH_SPI_SINGLE;
	my_flash_err = R_FLASH_SPI_Read_Data(FWUP_FLASH_SPI_DEV, &info);

	/* If there were an error this would demonstrate error detection of API calls. */
    if (FLASH_SPI_SUCCESS != my_flash_err)
    {
        nop(); // Your error handling code would go here.
        ret = EXMEM_ERROR;
    }
#else
    /* Fix me for other flash spi control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_spi_polling_erase
 *******************************************************************************************************************//**
 * @brief   The function checks erase busy the Flash SPI module.
 * @retval  FLASH_SPI_SUCCESS		Flash SPI module end successfully, and finished process.
 * @retval  FLASH_SPI_SUCCESS_BUSY	Flash SPI module end successfully, and not finished process.
 * @retval  FLASH_SPI_ERR_PARAM		Parameter error.
 * @retval  FLASH_SPI_ERR_HARD		Hardware error.
 * @retval  FLASH_SPI_ERR_OTHER		Other error.
 * @details This function checks erase busy the Flash SPI module.
 *          This is a function for abstracting the flash SPI access.
 */
exmem_err_t fwup_flash_spi_polling_erase(void)
{
	exmem_err_t	ret = EXMEM_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_spi_status_t my_flash_err = FLASH_SPI_SUCCESS;

    my_flash_err = R_FLASH_SPI_Polling(FWUP_FLASH_SPI_DEV, FLASH_SPI_MODE_ERASE_POLL);

	/* If there were an error this would demonstrate error detection of API calls. */
    switch (my_flash_err)
    {
    case FLASH_SPI_SUCCESS_BUSY:
    	ret = EXMEM_SUCCESS_BUSY;
    	break;
    default:
        nop(); // Your error handling code would go here.
        ret = EXMEM_ERROR;
        break;
    }
#else
    /* Fix me for other flash spi control library */
#endif
    return ret;
}

/***********************************************************************************************************************
 * Function Name: fwup_flash_spi_polling_write
 *******************************************************************************************************************//**
 * @brief   The function checks write busy the Flash SPI module.
 * @retval  FLASH_SPI_SUCCESS		Flash SPI module end successfully, and finished process.
 * @retval  FLASH_SPI_SUCCESS_BUSY	Flash SPI module end successfully, and not finished process.
 * @retval  FLASH_SPI_ERR_PARAM		Parameter error.
 * @retval  FLASH_SPI_ERR_HARD		Hardware error.
 * @retval  FLASH_SPI_ERR_OTHER		Other error.
 * @details This function checks write busy the Flash SPI module.
 *          This is a function for abstracting the flash SPI access.
 */
exmem_err_t fwup_flash_spi_polling_write(void)
{
	exmem_err_t	ret = EXMEM_SUCCESS;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    flash_spi_status_t my_flash_err = FLASH_SPI_SUCCESS;

    my_flash_err = R_FLASH_SPI_Polling(FWUP_FLASH_SPI_DEV, FLASH_SPI_MODE_PROG_POLL);

    /* If there were an error this would demonstrate error detection of API calls. */
    switch (my_flash_err)
    {
    case FLASH_SPI_SUCCESS_BUSY:
    	ret = EXMEM_SUCCESS_BUSY;
    	break;
    default:
        nop(); // Your error handling code would go here.
        ret = EXMEM_ERROR;
        break;
    }
#else
    /* Fix me for other flash spi control library */
#endif
    return ret;
}
#endif /* (FWUP_CFG_USE_EXMEM != 0) */

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_NONEOS)
/***********************************************************************************************************************
 * Function Name: fwup_software_delay_ms
 *******************************************************************************************************************//**
 * @brief   Software delay function.
 * @details Software delay function.
 */
static void fwup_software_delay_ms(uint32_t delay)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
    (void)R_BSP_SoftwareDelay(delay, BSP_DELAY_MILLISECS);
#else
    /* Fix me for other BSP module */
#endif
}
#else
	/* Fix me for other OS environment */
#endif

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
/***********************************************************************************************************************
 * Function Name: fwup_get_boot_protect
 *******************************************************************************************************************//**
 * @brief   Get setting of current Boot Protect.
 * @retval  true	Protection enabled.
 * @retval  false	Protection disabled.
 * @details Get setting of current Boot Protect.
 */
bool fwup_get_boot_protect(void)
{
	bool ret;
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
	fawreg_t faw;

	faw.LONG = FLASH.FAWMON.LONG;
	if (faw.BIT.FSPR == 1)
	{
		ret = false;
	}
	else
	{
		ret = true;
	}
#else
    /* Fix me for other protection scheme */
#endif
	return ret;
}

void fwup_set_boot_protect(void)
{
	uint32_t saddr, eaddr;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
	/* Set access window, Set FSPR bit for boot protection also */
#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) /* Dual bank mode */
	saddr = (uint32_t)FLASH_CF_LO_BANK_LO_ADDR;
	eaddr = (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS;
#else /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) Linear mode */
	saddr = (uint32_t)FLASH_CF_LO_BANK_LO_ADDR;
	eaddr = (uint32_t)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;
#endif /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) Linear mode */
	fwup_flash_accesswindow_set(saddr, eaddr);
#else
    /* Fix me for other protection scheme */
#endif
}
#endif  // FWUP_CFG_BOOT_PROTECT_ENABLE
#else
/* Fix me for other OS environment */
#endif  // FWUP_IMPLEMENTATION_BOOTLOADER

#if (FWUP_CFG_SIGNATURE_VERIFICATION == FWUP_SIGNATURE_ECDSA)
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT != FWUP_IMPLEMENTATION_AFRTOS)
int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize)
{
    int32_t xResult = -1;
    uint8_t pucHash[TC_SHA256_DIGEST_SIZE];
    uint8_t data_length;
    uint8_t public_key[64];
    uint8_t binary[256];
    uint8_t *head_pointer, *current_pointer, *tail_pointer;;

    /* Hash message */
    struct tc_sha256_state_struct xCtx;
    tc_sha256_init(&xCtx);
    tc_sha256_update(&xCtx, pucData, ulSize);
    tc_sha256_final(pucHash, &xCtx);

    /* extract public key from code_signer_public_key (pem format) */
    head_pointer = (uint8_t*)strstr((char *)code_signer_public_key, "-----BEGIN PUBLIC KEY-----");
    if(head_pointer)
    {
        head_pointer += strlen("-----BEGIN PUBLIC KEY-----");
        tail_pointer = (uint8_t*)strstr((char *)code_signer_public_key, "-----END PUBLIC KEY-----");
        base64_decode(head_pointer, binary, tail_pointer - head_pointer);
        current_pointer = binary;
        data_length = *(current_pointer + 1);
        while(1)
        {
            switch(*current_pointer)
            {
                case 0x30: /* found "SEQUENCE" */
                    current_pointer += 2;
                    break;
                case 0x03: /* found BIT STRING (maybe public key) */
                    if(*(current_pointer + 1) == 0x42)
                    {
                        memcpy(public_key, current_pointer + 4, 64);
                        /* Verify signature */
                        if(uECC_verify(public_key, pucHash, TC_SHA256_DIGEST_SIZE, pucSignature, uECC_secp256r1()))
                        {
                            xResult = 0;
                        }
                    }
                    current_pointer += *(current_pointer + 1) + 2;
                    break;
                default:
                    current_pointer += *(current_pointer + 1) + 2;
                    break;
            }
            if((current_pointer - binary) > data_length)
            {
                /* parsing error */
                break;
            }
        }
    }
    return xResult;
}
#endif  // ! FWUP_IMPLEMENTATION_AFRTOS
#endif  // FWUP_SIGNATURE_ECDSA

#if ( BSP_CFG_USER_CHARPUT_ENABLED == 1 )
/***********************************************************************************************************************
 * Function Name: my_sw_charput_function
 * Description  : char data output API
 * Arguments    : data -
 *                    Send data with SCI
 * Return Value : none
 **********************************************************************************************************************/
void my_sw_charput_function(uint8_t data)
{
    uint32_t arg = 0;
    /* do not call printf()->charput in interrupt context */
    do
    {
        /* Casting void pointer is used for address. */
        R_SCI_Control(s_fwup_communication_handle, SCI_CMD_TX_Q_BYTES_FREE, (void*)&arg);
    }
    while (SCI_CFG_CH8_TX_BUFSIZ != arg);
    /* Casting uint8_t pointer is used for address. */
    R_SCI_Send(s_fwup_communication_handle, (uint8_t*)&data, 1);

    return;
}
#endif /* BSP_CFG_USER_CHARPUT_ENABLED */

#if ( BSP_CFG_USER_CHARGET_ENABLED == 1 )
void my_sw_charget_function(void)
{

}
#endif /* BSP_CFG_USER_CHARGET_ENABLED */
