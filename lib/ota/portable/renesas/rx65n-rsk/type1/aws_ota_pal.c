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

/* Amazon FreeRTOS include. */
#include "FreeRTOS.h"
#include "aws_ota_pal.h"
#include "aws_ota_agent_internal.h"

/* Renesas RX platform includes */
#include "platform.h"

#include "r_flash_rx_if.h"
#include "r_cryptogram.h"
#include "r_tfat_lib.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"
#include "r_usb_basic_pinset.h"
#include "r_usb_hmsc_apl.h"
#include "r_usb_typedef.h"
#include "r_usb_rtos_apl.h"

/* Specify the OTA signature algorithm we support on this platform. */

const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha1-rsa";

/* The static functions below (prvPAL_CheckFileSignature and prvPAL_ReadAndAssumeCertificate) 
 * are optionally implemented. If these functions are implemented then please set the following macros in 
 * aws_test_ota_config.h to 1:
 * otatestpalCHECK_FILE_SIGNATURE_SUPPORTED
 * otatestpalREAD_AND_ASSUME_CERTIFICATE_SUPPORTED
 */

/**
 * @brief Verify the signature of the specified file.
 * 
 * This function should be implemented if signature verification is not offloaded
 * to non-volatile memory io functions.
 * 
 * This function is called from prvPAL_Close(). 
 * 
 * @param[in] C OTA file context information.
 * 
 * @return Below are the valid return values for this function.
 * kOTA_Err_None if the signature verification passes.
 * kOTA_Err_SignatureCheckFailed if the signature verification fails.
 * kOTA_Err_BadSignerCert if the if the signature verification certificate cannot be read.
 * 
 */
static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C );

/**
 * @brief Read the specified signer certificate from the filesystem into a local buffer.
 * 
 * The allocated memory returned becomes the property of the caller who is responsible for freeing it.
 * 
 * This function is called from prvPAL_CheckFileSignature(). It should be implemented if signature
 * verification is not offloaded to non-volatile memory io function.
 * 
 * @param[in] pucCertName The file path of the certificate file.
 * @param[out] ulSignerCertSize The size of the certificate file read.
 * 
 * @return A pointer to the signer certificate in the file system. NULL if the certificate cannot be read.
 * This returned pointer is the responsibility of the caller; if the memory is allocated the caller must free it.
 */
static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize );

/******************************************************************************
Typedef definitions
*******************************************************************************/

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_13
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_51

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
#define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_52

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 6

#define FLASH_INTERRUPT_PRIORITY 15 /* 0(low) - 15(high) */
/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/

#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)

#define FIRMWARE_UPDATE_STATE_INITIALIZE 0
#define FIRMWARE_UPDATE_STATE_ERASE 1
#define FIRMWARE_UPDATE_STATE_ERASE_WAIT_COMPLETE 2
#define FIRMWARE_UPDATE_STATE_READ_WAIT_COMPLETE 3
#define FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE 4
#define FIRMWARE_UPDATE_STATE_FINALIZE_COMPLETED 8
#define FIRMWARE_UPDATE_STATE_CAN_SWAP_BANK 100
#define FIRMWARE_UPDATE_STATE_WAIT_START 101
#define FIRMWARE_UPDATE_STATE_COMPLETED 102
#define FIRMWARE_UPDATE_STATE_ERROR 103

typedef struct _load_firmware_control_block {
    uint32_t status;
    uint32_t flash_buffer[FLASH_CF_MEDIUM_BLOCK_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
    uint32_t firmware_length;
    OTA_ImageState_t eSavedAgentState;
    OTA_FileContext_t OTA_FileContext;
}LOAD_FIRMWARE_CONTROL_BLOCK;

/******************************************************************************
 External variables
 ******************************************************************************/

/******************************************************************************
 Private global variables
 ******************************************************************************/
static LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;

/******************************************************************************
 External functions
 ******************************************************************************/

/*******************************************************************************
 Private global variables and functions
********************************************************************************/
static void ota_firmware_update_request(OTA_FileContext_t * const C);
static void firmware_update_status_initialize(void);
static int32_t firm_block_read(uint32_t *firmware, uint32_t offset);
static uint32_t load_firmware_process(void);
static void flash_load_firmware_callback_function(void *event);
static int16_t file_write_process(uint8_t *buf, int16_t write_size, uint32_t Offset);

extern uint8_t     g_isFileWrite;

FIL file = {0};

/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CreateFileForRx" );

    OTA_Err_t eResult = kOTA_Err_Uninitialized; /* For MISRA mandatory. */

    if( C != NULL )
    {
        /* No need for pacFilepath, we write directly to ROM instead of file system
         * But still check for its existence because it is required by AWS */
        if ( C->pucFilePath != NULL )
        {
            FRESULT ret = TFAT_FR_OK;
            ret = R_tfat_f_open(&file, (const char *)C->pucFilePath, (TFAT_FA_OPEN_ALWAYS | TFAT_FA_READ | TFAT_FA_WRITE));
            if (TFAT_FR_OK == ret)
            {
                C->pucFile = (uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;
                eResult = kOTA_Err_None;
                OTA_LOG_L1( "[%s] Firmware update initialized.\r\n", OTA_METHOD_NAME );
            }
            else
            {
                eResult = kOTA_Err_RxFileCreateFailed;
                OTA_LOG_L1( "[%s] ERROR - Failed to start operation: already active!\r\n", OTA_METHOD_NAME );
            }
        }
        else
        {
            eResult = kOTA_Err_RxFileCreateFailed;
            OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
        }
    }
    else
    {
        eResult = kOTA_Err_RxFileCreateFailed;
        OTA_LOG_L1( "[%s] ERROR - Invalid context provided.\r\n", OTA_METHOD_NAME );
    }

    return eResult;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_Abort( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_Abort" );
    
    OTA_LOG_L1("Function call: [%s].\r\n", OTA_METHOD_NAME);

    /* Set default return status to uninitialized. */
    OTA_Err_t eResult = kOTA_Err_Uninitialized;

    if( NULL != C )
    {
        if( NULL != C->pucFile )
        {
            C->pucFile = NULL;
        }
            eResult = kOTA_Err_None;
    }
    else /* Context was not valid. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_FileAbort;
    }

    return eResult;
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{

    DEFINE_OTA_METHOD_NAME( "prvPAL_WriteBlock" );

    static FILINFO filinfo;
    int16_t lReturnVal = 0;

    int16_t  err = -1;

    err = file_write_process(pacData, (int16_t) ulBlockSize, ulOffset);
    if (err != 0)
    {
        OTA_LOG_L1( "[%s] ERROR - File writting FAIL.\r\n", OTA_METHOD_NAME );
        return -1;
    }

    if( (C->ulBlocksRemaining) == 1U)
    {
        load_firmware_control_block.firmware_length = (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
        if(TFAT_FR_OK == R_tfat_f_stat((const char*)C->pucFilePath, &filinfo))
        {
            R_FLASH_Open();

            /* Transition to FIRMWARE_UPDATE_STATE_INITIALIZE state */
            firmware_update_status_initialize();

            /* 1st call, Transition to FIRMWARE_UPDATE_STATE_INITIALIZE state */
            ota_firmware_update_request(C);

            /* 2nd call, Transition to FIRMWARE_UPDATE_STATE_ERASE state
               Execute update firmware write area initialization */
            ota_firmware_update_request(C);

            while(FIRMWARE_UPDATE_STATE_COMPLETED != load_firmware_control_block.status)
            {
                load_firmware_process();
            }

            return lReturnVal;
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR - File Not Found. %s.\r\n", OTA_METHOD_NAME, C->pucFilePath);
            while(1);
        }

        return lReturnVal;
    }

    return lReturnVal;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

    /* Note: Updated firmware signature verification not implemented */
    OTA_Err_t eResult = kOTA_Err_None;

    if( C == NULL )
    {
        eResult = kOTA_Err_FileClose;
    }

    if( kOTA_Err_None == eResult )
    {
        if( C->pxSignature != NULL )
        {
            /* Verify the file signature, close the file and return the signature verification result. */
            /* Note: skip Verify the file signature. */
//            eResult = prvPAL_CheckFileSignature( C );
            eResult = kOTA_Err_None;
        }
        else
        {
            eResult = kOTA_Err_SignatureCheckFailed;
        }

        FRESULT ret = TFAT_FR_OK;
        ret = R_tfat_f_close(&file);
        if (TFAT_FR_OK != ret)
        {
            OTA_LOG_L1( "[%s] ERROR - Failed to close OTA update file.\r\n", OTA_METHOD_NAME );
            eResult = kOTA_Err_FileClose;
        }

        if( eResult == kOTA_Err_None )
        {
//                OTA_LOG_L1( "[%s] %s signature verification passed.\r\n", OTA_METHOD_NAME, pcOTA_JSON_FileSignatureKey );
        }
        else
        {
            OTA_LOG_L1( "[%s] ERROR - Failed to pass %s signature verification: %d.\r\n", OTA_METHOD_NAME,
                        cOTA_JSON_FileSignatureKey, eResult );

            /* If we fail to verify the file signature that means the image is not valid. We need to set the image state to aborted. */
            prvPAL_SetPlatformImageState( eOTA_ImageState_Aborted );
        }
    }

    return eResult;
}
/*-----------------------------------------------------------*/


static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    /* Note: Updated firmware signature verification not implemented */
    DEFINE_OTA_METHOD_NAME( "prvPAL_CheckFileSignature" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

    /* not implemented */
    return kOTA_Err_SignatureCheckFailed;
}
/*-----------------------------------------------------------*/

static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ReadAndAssumeCertificate" );

    /* Note: We use the aws_ota_codesigner_certificate.h instead of pucCertName */

    //uint8_t * pucCertData;
    uint32_t ulCertSize = 0U;
    uint8_t * pucSignerCert = NULL;

    OTA_LOG_L1( "[%s] : Using aws_ota_codesigner_certificate.h.\r\n", OTA_METHOD_NAME);

    /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
    /// ulCertSize = sizeof( signingcredentialSIGNING_CERTIFICATE_PEM );
    /// pucSignerCert = pvPortMalloc( ulCertSize + 1 );                       /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
    /// pucCertData = ( uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

    if( pucSignerCert != NULL )
    {
    ///     memcpy( pucSignerCert, pucCertData, ulCertSize );
        /* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
    ///     pucSignerCert[ ulCertSize ] = 0U;
    ///     *ulSignerCertSize = ulCertSize + 1U;
    }
    else
    {
        OTA_LOG_L1( "[%s] Error: No memory for certificate of size %d!\r\n", OTA_METHOD_NAME, ulCertSize );
    }

    return pucSignerCert;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ResetDevice");

    OTA_LOG_L1( "[%s] Resetting the device.\r\n", OTA_METHOD_NAME );

    /* Software reset issued (Not bank swap) */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    SYSTEM.SWRR = 0xa501;
    while(1);   /* software reset */

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */
    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ActivateNewImage");

    OTA_LOG_L1( "[%s] Changing the Startup Bank\r\n", OTA_METHOD_NAME );

    /* reset for self testing */
    vTaskDelay(5000);
    prvPAL_ResetDevice();   /* no return from this function */

    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );

    OTA_LOG_L1("Function call: prvPAL_SetPlatformImageState: [%d]\r\n", eState);

    OTA_Err_t eResult = kOTA_Err_None;

    if( eState != eOTA_ImageState_Unknown && eState <= eOTA_LastImageState )
    {
        /* Save the image state to eSavedAgentState. */
        load_firmware_control_block.eSavedAgentState = eState;
    }
    else /* Image state invalid. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid image state provided.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_BadImageState;
    }

    return eResult;
}
/*-----------------------------------------------------------*/

OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_GetPlatformImageState" );
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

    OTA_LOG_L1("Function call: prvPAL_GetPlatformImageState: [%d]\r\n", ePalState);
    return ePalState; /*lint !e64 !e480 !e481 I/O calls and return type are used per design. */
}
/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif

static void ota_firmware_update_request(OTA_FileContext_t * const C)
{
    if(FIRMWARE_UPDATE_STATE_COMPLETED == load_firmware_control_block.status)
    {
        load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_INITIALIZE;
    }
    if(FIRMWARE_UPDATE_STATE_INITIALIZE == load_firmware_control_block.status)
    {
        load_firmware_process();
    }
    if(FIRMWARE_UPDATE_STATE_CAN_SWAP_BANK < load_firmware_control_block.status)
    {
        load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERASE;
    }
}

static void firmware_update_status_initialize(void)
{
    load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_INITIALIZE;
}

/***********************************************************************************************************************
* Function Name: firm_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static int32_t firm_block_read(uint32_t *firmware, uint32_t offset)
{
    uint16_t size = 0;
    uint32_t current_buf_position = 0;
    uint32_t current_file_position = offset;

    FRESULT ret = TFAT_FR_OK;

    if (USB_APL_YES == g_isFileWrite)
    {
        R_USB_HmscGetSem();

        ret = R_tfat_f_lseek(&file, current_file_position);
        if (TFAT_FR_OK == ret)
        {
            while(1)
            {
                ret = R_tfat_f_read(&file, &firmware[current_buf_position], FLASH_CF_MEDIUM_BLOCK_SIZE, &size);
                if (TFAT_FR_OK == ret)
                {
                    if (0 == size)
                    {
                        break;
                    }
                    else
                    {
                        current_file_position += size;
                        current_buf_position += size;
                        if((current_buf_position) == FLASH_CF_MEDIUM_BLOCK_SIZE)
                        {
                            current_file_position = 0;
                            break;
                        }
                    }
                }
                else
                {
                    goto firm_block_read_error;
                    break;
                }
            }
        }

        R_USB_HmscRelSem();
        if (TFAT_FR_OK != ret)
        {
firm_block_read_error:
            g_isFileWrite = USB_APL_NO;
            return -1;
        }
    }
    return 0;
}

uint32_t load_firmware_process(void)
{
    flash_interrupt_config_t cb_func_info;

    switch(load_firmware_control_block.status)
    {
        case FIRMWARE_UPDATE_STATE_INITIALIZE: /* initialize */
            load_firmware_control_block.progress = 0;
            load_firmware_control_block.offset = 0;
            cb_func_info.pcallback = flash_load_firmware_callback_function;
            cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
            R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);
            load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_WAIT_START;
            break;
        case FIRMWARE_UPDATE_STATE_WAIT_START: /* wait start */
            /* this state will be changed by other process request using load_firmware_control_block.status */
            break;
        case FIRMWARE_UPDATE_STATE_ERASE: /* erase bank0 (0xFFE00000-0xFFEBFFFF) */
            R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
            load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERASE_WAIT_COMPLETE;
            break;
        case FIRMWARE_UPDATE_STATE_ERASE_WAIT_COMPLETE:
            /* this state will be changed by callback routine */
            break;
        case FIRMWARE_UPDATE_STATE_READ_WAIT_COMPLETE:
            if(!firm_block_read(load_firmware_control_block.flash_buffer, load_firmware_control_block.offset))
            {
                R_FLASH_Write((uint32_t)load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
                load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE;
            }
            else
            {
                load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
            }
            break;
        case FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE:
            /* this state will be changed by callback routine */
            break;
        case FIRMWARE_UPDATE_STATE_FINALIZE_COMPLETED:
            load_firmware_control_block.progress = 100;
            load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_COMPLETED;
            break;
        case FIRMWARE_UPDATE_STATE_COMPLETED:
            break;
        case FIRMWARE_UPDATE_STATE_ERROR:
            load_firmware_control_block.progress = 100;
            break;
        default:
            break;
    }
    return load_firmware_control_block.progress;
}

void flash_load_firmware_callback_function(void *event)
{
    uint32_t event_code;
    event_code = *((uint32_t*)event);

    switch(event_code)
    {
        case FLASH_INT_EVENT_ERASE_COMPLETE:
            if(FIRMWARE_UPDATE_STATE_ERASE_WAIT_COMPLETE == load_firmware_control_block.status)
            {
                load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_READ_WAIT_COMPLETE;
            }
            else
            {
                load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
            }
            break;
        case FLASH_INT_EVENT_WRITE_COMPLETE:
            if(FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE == load_firmware_control_block.status)
            {
                load_firmware_control_block.offset += FLASH_CF_MEDIUM_BLOCK_SIZE;
                load_firmware_control_block.progress = (uint32_t)(((float)(load_firmware_control_block.offset)/(float)((FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))*100));

                if( load_firmware_control_block.firmware_length == load_firmware_control_block.offset )
                {
                    load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_FINALIZE_COMPLETED;
                }
                else
                {
                    load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_READ_WAIT_COMPLETE;
                }
            }
            else
            {
                load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
            }
            break;
        case FLASH_INT_EVENT_TOGGLE_BANK:
            R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
            SYSTEM.SWRR = 0xa501;
            while(1);   /* software reset */
            break;
        default:
            nop();
            break;
    }
}

static int16_t file_write_process(uint8_t *buf, int16_t write_size, uint32_t Offset)
{
    uint16_t    size = 0;
    FRESULT     ret = TFAT_FR_OK;

    R_USB_HmscGetSem();
    ret = R_tfat_f_lseek(&file, Offset);
    if (TFAT_FR_OK == ret)
    {
        ret = R_tfat_f_write(&file, buf, (uint16_t)write_size, &size);
        if (TFAT_FR_OK == ret)
        {
            g_isFileWrite = USB_APL_YES;
        }
        else
        {
            g_isFileWrite = USB_APL_NO;
        }
        if ((uint16_t)write_size != size)
        {
            g_isFileWrite = USB_APL_NO;
        }
    }
    else
    {
        g_isFileWrite = USB_APL_NO;
    }
    R_USB_HmscRelSem();

    if (USB_APL_NO == g_isFileWrite)
    {
        return -1;
    }
    return 0;
}

/* end of file */
