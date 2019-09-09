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

/* Amazon FreeRTOS include. */
#include "FreeRTOS.h"
#include "aws_ota_pal.h"
#include "aws_ota_agent_internal.h"

/* Renesas RX Driver Package include */
#include "platform.h"
#include "r_flash_rx_if.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";   /* FIX ME. */


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

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_13
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_51

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
#define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_52

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 6

#define FLASH_INTERRUPT_PRIORITY 15	/* 0(low) - 15(high) */
/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/

#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)

#define FIRMWARE_UPDATE_STATE_CLOSED 0
#define FIRMWARE_UPDATE_STATE_INITIALIZED 1
#define FIRMWARE_UPDATE_STATE_ERASE 2
#define FIRMWARE_UPDATE_STATE_ERASE_WAIT_COMPLETE 3
#define FIRMWARE_UPDATE_STATE_READ_WAIT_COMPLETE 4
#define FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE 5
#define FIRMWARE_UPDATE_STATE_FINALIZE 6
#define FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_ERASE_DATA_FLASH 7
#define FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_WRITE_DATA_FLASH 8
#define FIRMWARE_UPDATE_STATE_FINALIZE_COMPLETED 9
#define FIRMWARE_UPDATE_STATE_CAN_SWAP_BANK 100
#define FIRMWARE_UPDATE_STATE_WAIT_START 101
#define FIRMWARE_UPDATE_STATE_COMPLETED 102
#define FIRMWARE_UPDATE_STATE_ERROR 103
#define FIRMWARE_UPDATE_STATE_WRITE_COMPLETE 201
#define FIRMWARE_UPDATE_STATE_ERASE_COMPLETE 202

#define otaconfigMAX_NUM_BLOCKS_REQUEST        	64U	/* this value will be appeared after 201908.00 in aws_ota_agent_config.h */

typedef struct _load_firmware_control_block {
	uint32_t status;
	uint32_t processed_byte;
	OTA_ImageState_t eSavedAgentState;
	OTA_FileContext_t OTA_FileContext;
}LOAD_FIRMWARE_CONTROL_BLOCK;

typedef struct _packet_block_for_queue
{
	uint32_t ulOffset;
	uint8_t	packet[(2 << otaconfigLOG2_FILE_BLOCK_SIZE)];
}PACKET_BLOCK_FOR_QUEUE;

static void ota_flashing_task(void);
static void ota_flashing_callback(void *event);

static QueueHandle_t xQueue;
static TaskHandle_t xTask;
static xSemaphoreHandle xSemaphore;
static volatile LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue;

/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CreateFileForRx( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CreateFileForRx" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);
    OTA_LOG_L1("Compiled in [%s] [%s].\r\n", __DATE__, __TIME__);
    OTA_Err_t eResult = kOTA_Err_Uninitialized;
    static uint8_t *dummy_file_pointer;
    static uint8_t dummy_file;
    dummy_file_pointer = &dummy_file;
    flash_err_t flash_err;
    flash_interrupt_config_t cb_func_info;

    if( C != NULL )
    {
        if( C->pucFilePath != NULL )
        {
			C->pucFile = dummy_file_pointer;

			/* create task/queue/semaphore for flashing */
			xTaskCreate(ota_flashing_task, "OTA_FLASHING_TASK", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, &xTask);
			xQueue = xQueueCreate(otaconfigMAX_NUM_BLOCKS_REQUEST, (2 << otaconfigLOG2_FILE_BLOCK_SIZE));	/* 128KB = 128 * (2 ^ 10): otaconfigMAX_NUM_BLOCKS_REQUEST = 128, otaconfigLOG2_FILE_BLOCK_SIZE = 10 */
			xSemaphore = xSemaphoreCreateMutex();

			flash_err = R_FLASH_Open();
			cb_func_info.pcallback = ota_flashing_callback;
			cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
			R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);

			if(flash_err == FLASH_SUCCESS)
			{
				flash_err = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
				if(flash_err == FLASH_SUCCESS)
				{
					eResult = kOTA_Err_None;
					OTA_LOG_L1( "[%s] Receive file created.\r\n", OTA_METHOD_NAME );
				}
				else
				{
					eResult = kOTA_Err_RxFileCreateFailed;
					OTA_LOG_L1( "[%s] ERROR - R_FLASH_Erase() returns error.\r\n", OTA_METHOD_NAME );
				}
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

    /* FIX ME. */
    return kOTA_Err_FileAbort;
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_WriteBlock" );

    /* FIX ME. */
    return -1;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

    /* FIX ME. */
    return kOTA_Err_FileClose;
}
/*-----------------------------------------------------------*/


static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CheckFileSignature" );

    /* FIX ME. */
    return kOTA_Err_SignatureCheckFailed;
}
/*-----------------------------------------------------------*/

static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ReadAndAssumeCertificate" );

    /* FIX ME. */
    return NULL;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ResetDevice");

    /* FIX ME. */
    return kOTA_Err_ResetNotSupported;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ActivateNewImage");

    /* FIX ME. */
    return kOTA_Err_Uninitialized;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );

    /* FIX ME. */
    return kOTA_Err_BadImageState;
}
/*-----------------------------------------------------------*/

OTA_PAL_ImageState_t prvPAL_GetPlatformImageState( void )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_GetPlatformImageState" );

    /* FIX ME. */
    return eOTA_ImageState_Unknown;
}
/*-----------------------------------------------------------*/

/* Provide access to private members for testing. */
#ifdef AMAZON_FREERTOS_ENABLE_UNIT_TESTS
    #include "aws_ota_pal_test_access_define.h"
#endif


static void ota_flashing_task(void)
{
	flash_err_t flash_err;

	while(load_firmware_control_block.status != FIRMWARE_UPDATE_STATE_ERASE_COMPLETE)
	{
		vTaskDelay(1);
	}

	while(1)
	{
		xQueueReceive(xQueue, &packet_block_for_queue, portMAX_DELAY);
		xSemaphoreTake(xSemaphore, portMAX_DELAY);
		load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE;
		flash_err = R_FLASH_Write((uint32_t)packet_block_for_queue.packet, packet_block_for_queue.ulOffset + BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MIN_PGM_SIZE);
		if(flash_err != FLASH_SUCCESS)
		{
			nop();
		}
	}
}

static void ota_flashing_callback(void *event)
{
	uint32_t event_code;
	event_code = *((uint32_t*)event);

	static portBASE_TYPE xHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);

    if(event_code == FLASH_INT_EVENT_WRITE_COMPLETE)
    {
		if(FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE == load_firmware_control_block.status)
		{
			load_firmware_control_block.processed_byte += FLASH_CF_MIN_PGM_SIZE;
			load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_WRITE_COMPLETE;
    	}
    }
    else if(event_code == FLASH_INT_EVENT_ERASE_COMPLETE)
    {
		if(FIRMWARE_UPDATE_STATE_ERASE_WAIT_COMPLETE == load_firmware_control_block.status)
		{
			load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERASE_COMPLETE;
		}
		else
		{
			load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
		}
    }
    else
    {
		load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
    }
}

