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

/* Renesas RX Driver Package include */
#include "platform.h"
#include "r_flash_rx_if.h"

/* Specify the OTA signature algorithm we support on this platform. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha1-rsa";   /* FIX ME. */
//const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";   /* FIX ME. */


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

//#define otaconfigMAX_NUM_BLOCKS_REQUEST        	128U	/* this value will be appeared after 201908.00 in aws_ota_agent_config.h */
#define otaconfigMAX_NUM_BLOCKS_REQUEST        	64U	/* this value will be appeared after 201908.00 in aws_ota_agent_config.h */

#define BOOT_LOADER_CUT_OFF_FOR_OTA_OFFSET	0x200

#define LIFECYCLE_STATE_BLANK		(0xff)
#define LIFECYCLE_STATE_TESTING		(0xfe)
#define LIFECYCLE_STATE_VALID		(0xfc)
#define LIFECYCLE_STATE_INVALID		(0xf8)

typedef struct _load_firmware_control_block {
	uint32_t status;
	uint32_t processed_byte;
	OTA_ImageState_t eSavedAgentState;
	OTA_FileContext_t OTA_FileContext;
}LOAD_FIRMWARE_CONTROL_BLOCK;

typedef struct _packet_block_for_queue
{
	uint32_t ulOffset;
	uint32_t length;
	uint8_t	packet[(1 << otaconfigLOG2_FILE_BLOCK_SIZE)];
}PACKET_BLOCK_FOR_QUEUE;

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
    uint8_t reserved2[236];
}FIRMWARE_UPDATE_CONTROL_BLOCK;

static bool ota_context_validate( OTA_FileContext_t * C );
static void ota_flashing_task( void * pvParameters );
static void ota_flashing_callback(void *event);

static QueueHandle_t xQueue;
static TaskHandle_t xTask;
static xSemaphoreHandle xSemaphore;
static volatile LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue1;
static PACKET_BLOCK_FOR_QUEUE packet_block_for_queue2;

static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank0 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;

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
			xQueue = xQueueCreate(otaconfigMAX_NUM_BLOCKS_REQUEST, sizeof(PACKET_BLOCK_FOR_QUEUE));
			xTaskCreate(ota_flashing_task, "OTA_FLASHING_TASK", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, &xTask);
			xSemaphore = xSemaphoreCreateMutex();

			flash_err = R_FLASH_Open();
			cb_func_info.pcallback = ota_flashing_callback;
			cb_func_info.int_priority = FLASH_INTERRUPT_PRIORITY;
			R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);

			if(flash_err == FLASH_SUCCESS)
			{
				eResult = kOTA_Err_None;
				OTA_LOG_L1( "[%s] Receive file created.\r\n", OTA_METHOD_NAME );
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

    OTA_Err_t eResult = kOTA_Err_None;

    if( ota_context_validate( C ) == ( bool_t ) pdFALSE )
    {
        eResult = kOTA_Err_FileClose;
    }

	if (kOTA_Err_None == eResult)
	{
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
		if (NULL != xSemaphore)
		{
			vSemaphoreDelete(xSemaphore);
			xSemaphore = NULL;
		}

		R_FLASH_Close();
	}

	OTA_LOG_L1( "[%s] OK\r\n", OTA_METHOD_NAME );
    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

/* Write a block of data to the specified file. */
int16_t prvPAL_WriteBlock( OTA_FileContext_t * const C,
                           uint32_t ulOffset,
                           uint8_t * const pacData,
                           uint32_t ulBlockSize )
{
    OTA_Err_t eResult = kOTA_Err_Uninitialized;

    DEFINE_OTA_METHOD_NAME( "prvPAL_WriteBlock" );
	OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);
    memcpy(packet_block_for_queue1.packet, pacData, ulBlockSize);
    packet_block_for_queue1.ulOffset = ulOffset + BOOT_LOADER_CUT_OFF_FOR_OTA_OFFSET;
    packet_block_for_queue1.length = ulBlockSize;
	if(xQueueSend(xQueue, &packet_block_for_queue1, NULL) == pdPASS)
	{
		eResult = ( int16_t ) ulBlockSize;
	}
	else
	{
		OTA_LOG_L1("OTA flashing queue send error.\r\n");
		eResult = kOTA_Err_OutOfMemory;
	}
	
	if( ulOffset == 0U)
	{
		/* Write from 0 to 0x200 of code flash.
		   Update image flag and signature before writing to code flash. */
		FIRMWARE_UPDATE_CONTROL_BLOCK * p_block;

	    memcpy(packet_block_for_queue1.packet, (const void *)firmware_update_control_block_bank0, BOOT_LOADER_CUT_OFF_FOR_OTA_OFFSET);
		p_block = (FIRMWARE_UPDATE_CONTROL_BLOCK *)packet_block_for_queue1.packet;
		p_block->image_flag = LIFECYCLE_STATE_TESTING;
	    memcpy(p_block->signature, C->pxSignature->ucData, sizeof(p_block->signature));
	    packet_block_for_queue1.ulOffset = 0;
	    packet_block_for_queue1.length = BOOT_LOADER_CUT_OFF_FOR_OTA_OFFSET;
		if(xQueueSend(xQueue, &packet_block_for_queue1, NULL) != pdPASS)
		{
			OTA_LOG_L1("OTA flashing queue send error.\r\n");
			eResult = kOTA_Err_OutOfMemory;
		}
	}
	
	return eResult;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
	OTA_Err_t eResult = kOTA_Err_None;
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );

    if( ota_context_validate( C ) == ( bool_t ) pdFALSE )
    {
        eResult = kOTA_Err_FileClose;
    }

    if( C->pxSignature == NULL )
    {
        eResult = kOTA_Err_SignatureCheckFailed;
    }

	if (kOTA_Err_None == eResult)
	{
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
		if (NULL != xSemaphore)
		{
			vSemaphoreDelete(xSemaphore);
			xSemaphore = NULL;
		}

		R_FLASH_Close();
	}

	OTA_LOG_L1( "[%s] OK\r\n", OTA_METHOD_NAME );
	eResult = kOTA_Err_None;
	return eResult;
}
/*-----------------------------------------------------------*/


static OTA_Err_t prvPAL_CheckFileSignature( OTA_FileContext_t * const C )
{
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
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

    /* not implemented */
    return NULL;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ResetDevice( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ResetDevice");

    OTA_LOG_L1( "[%s] Resetting the device.\r\n", OTA_METHOD_NAME );

    /* Software reset issued (Not bank swap) */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    SYSTEM.SWRR = 0xa501;
	while(1);	/* software reset */

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
	prvPAL_ResetDevice();	/* no return from this function */

    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_SetPlatformImageState( OTA_ImageState_t eState )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_SetPlatformImageState" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

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

static bool ota_context_validate( OTA_FileContext_t * C )
{
	return ( NULL != C );
}

static void ota_flashing_task( void * pvParameters )
{
	flash_err_t flash_err;
	static uint8_t block[(1 << otaconfigLOG2_FILE_BLOCK_SIZE)];
	static uint32_t ulOffset;
	static uint32_t length;

	while(1)
	{
		xQueueReceive(xQueue, &packet_block_for_queue2, portMAX_DELAY);
		xSemaphoreTake(xSemaphore, portMAX_DELAY);
		memcpy(block, packet_block_for_queue2.packet, sizeof(block));
		ulOffset = packet_block_for_queue2.ulOffset;
		length = packet_block_for_queue2.length;
		flash_err = R_FLASH_Write((uint32_t)block, ulOffset + (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, length);
		if(packet_block_for_queue2.length != 1024)
		{
			nop();
		}
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

    if((event_code != FLASH_INT_EVENT_WRITE_COMPLETE) || (event_code == FLASH_INT_EVENT_ERASE_COMPLETE))
    {
    	nop(); /* trap */
    }
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
}

