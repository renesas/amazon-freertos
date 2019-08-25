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

#include "r_flash_spi_if.h"
#include "r_flash_spi_config.h"

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

#define LIFECYCLE_STATE_BLANK             (0)
#define LIFECYCLE_STATE_ON_THE_MARKET     (1)
#define LIFECYCLE_STATE_UPDATING	      (2)

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */

#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_13
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_51

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
#define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_52

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 6

///#define INITIAL_FIRMWARE_FILE_NAME "userupprog.rsu"
/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/

#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)

#define SHA1_HASH_LENGTH_BYTE_SIZE 20

#define FIRMWARE_UPDATE_STATE_INITIALIZE 0
#define FIRMWARE_UPDATE_STATE_ERASE 1
#define FIRMWARE_UPDATE_STATE_ERASE_WAIT_COMPLETE 2
#define FIRMWARE_UPDATE_STATE_READ_WAIT_COMPLETE 3
#define FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE 4
#define FIRMWARE_UPDATE_STATE_FINALIZE 5
#define FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_ERASE_DATA_FLASH 6
#define FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_WRITE_DATA_FLASH 7
#define FIRMWARE_UPDATE_STATE_FINALIZE_COMPLETED 8
#define FIRMWARE_UPDATE_STATE_CAN_SWAP_BANK 100
#define FIRMWARE_UPDATE_STATE_WAIT_START 101
#define FIRMWARE_UPDATE_STATE_COMPLETED 102
#define FIRMWARE_UPDATE_STATE_ERROR 103
#define FIRMWARE_UPDATE_STATE_WRITE_COMPLETE 201
#define FIRMWARE_UPDATE_STATE_ERASE_COMPLETE 202

// select read size
/* set "1" or "45" */
#define FREAD_SIZE 	45

// flash bucket size
/* set "FLASH_CF_MIN_PGM_SIZE" or "FLASH_CF_MEDIUM_BLOCK_SIZE" */
#define FLASH_BUCKET_SIZE	 FLASH_CF_MEDIUM_BLOCK_SIZE

#if (FREAD_SIZE == 45U)
#define PARAM_DATA_LINE_NUM	35
#define DF_DATA_LINE_NUM	41
#define CF_DATA_LINE_NUM	45
#endif

#define BASE64_ENCODE_ONE_LINE_CHARACTER_LENGTH (24)
#define BASE64_ENCODE_EQUAL_CHARACTER_LENGTH (2)
#define BASE64_ENCODE_RETURN_CODE_LENGTH (2)
#define BASE64_ENCODE_CF_TOTAL_LENGTH (45)
#define BASE64_ENCODE_DF_TOTAL_LENGTH (41)
#define BASE64_ENCODE_SHA1_TOTAL_LENGTH (35)
#define BASE64_DECODE_TOTAL_LENGTH (16)
#define CODE_FLASH_WRITE_SIZE (128)
#define CODE_FLASH_WRITE_BUFFER_SIZE (512)
#define CODE_FLASH_16BYTE_MASK (0x00000070u)
#define CODE_FLASH_128BYTE_ALIGN (0xffffff80u)
#define BOOT_LOADER_MIRROR_FIRST_ADDRESS FLASH_CF_BLOCK_37

#define SERIAL_FLASH_BLOCK_ERASE_64KB_SIZE (65536)
#define SERIAL_FLASH_FIT_COUNT (128)

typedef struct _load_firmware_control_block {
	uint32_t status;
	uint8_t file_name[256];
	uint32_t flash_buffer[FLASH_BUCKET_SIZE / 4];
	uint32_t offset;
	volatile uint32_t flash_write_in_progress_flag;
	volatile uint32_t flash_erase_in_progress_flag;
	uint32_t progress;
	uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];
	uint32_t firmware_length;
}LOAD_FIRMWARE_CONTROL_BLOCK;

typedef struct _firmware_update_control_block_sub
{
	uint32_t user_program_max_cnt;
	uint32_t lifecycle_state;
	uint8_t hash0_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];
	uint8_t hash1_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];
}FIRMWARE_UPDATE_CONTROL_BLOCK_SUB;

typedef struct _firmware_update_control_block
{
	FIRMWARE_UPDATE_CONTROL_BLOCK_SUB data;
	uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];
}FIRMWARE_UPDATE_CONTROL_BLOCK;

/******************************************************************************
 External variables
 ******************************************************************************/

/******************************************************************************
 Private global variables
 ******************************************************************************/
static uint32_t g_ram_vector_table[256];      // RAM space to hold the vector table
static LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static FIRMWARE_UPDATE_CONTROL_BLOCK firmware_update_control_block_image = {0};

OTA_ImageState_t eSavedAgentState = eOTA_ImageState_Unknown;
SemaphoreHandle_t  OTASemaphoreHandle;

/******************************************************************************
 External functions
 ******************************************************************************/
extern uint32_t base64_decode(uint8_t *source, uint8_t *result, uint32_t size);

/*******************************************************************************
 Private global variables and functions
********************************************************************************/
static void dummy_int(void);
static void flash_load_firmware_callback_function(void *event);
static uint32_t *flash_copy_vector_table(void);

void bank_swap(void);
void ota_firmware_update_request(OTA_FileContext_t * const C);
bool is_firmupdating(void);
bool is_firmupdatewaitstart(void);

uint32_t load_firmware_process(void);
uint32_t get_update_data_size(void);
void flash_bank_swap_callback_function(void *event);
void load_firmware_status(uint32_t *now_status, uint32_t *finish_status);
void firmware_update_status_initialize(void);

static int32_t serial_flash_firm_block_read(uint32_t *firmware, uint32_t offset);
int16_t serial_flash_write_process(uint8_t *buf, int16_t write_size, uint32_t Offset);

extern uint8_t     g_isFileWrite;

uint32_t g_on_the_fly_start;

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
            flash_spi_erase_info_t Flash_Info_E = {0};
            uint8_t status = 0;
            uint32_t serial_flash_address;
            uint32_t update_target_block_bytes =  FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER;
            
            C->pucFile = (uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;
            vSemaphoreCreateBinary(OTASemaphoreHandle);
            if (OTASemaphoreHandle == NULL)
            {
                eResult = kOTA_Err_RxFileCreateFailed;
                OTA_LOG_L1( "[%s] ERROR - The semaphore was created fail.\r\n", OTA_METHOD_NAME );
            }
            /* Serial flash initial setting */
            R_FLASH_SPI_Open(0);
            #if 0 /* Setting is required for QSPI Quad communication */
            R_FLASH_SPI_Quad_Enable(0);
            while (FLASH_SPI_SUCCESS_BUSY == R_FLASH_SPI_Polling(0, FLASH_SPI_MODE_REG_WRITE_POLL))
            {
                vTaskDelay(5);
            }
            #endif
            R_FLASH_SPI_Set_Write_Protect(0,0);
            while (FLASH_SPI_SUCCESS_BUSY == R_FLASH_SPI_Polling(0, FLASH_SPI_MODE_REG_WRITE_POLL))
            {
                vTaskDelay(5);
            }
            /* Chip erase */
            Flash_Info_E.addr   = 0;
            Flash_Info_E.mode   = FLASH_SPI_MODE_C_ERASE;
            R_FLASH_SPI_Erase(0, &Flash_Info_E);
            while (FLASH_SPI_SUCCESS_BUSY == R_FLASH_SPI_Polling(0, FLASH_SPI_MODE_ERASE_POLL))
            {
                vTaskDelay(5);
            }
            eResult = kOTA_Err_None;
            OTA_LOG_L1( "[%s] Firmware update initialized.\r\n", OTA_METHOD_NAME );
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
    int16_t  err = -1;

    err = serial_flash_write_process(pacData, (int16_t) ulBlockSize, ulOffset);
    if (err != 0)
    {
        printf("Serial Flash writting FAIL\r\n");
    	return -1;
    }

    if( (C->ulBlocksRemaining) == 1U)
	{
    	load_firmware_control_block.firmware_length = (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);

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
	}
    return 0;
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

    /* Avoid compiler warnings about unused variables for a release including source code. */
    R_INTERNAL_NOT_USED(OTA_METHOD_NAME);
    R_INTERNAL_NOT_USED(C);

    /* FIX ME. */
    return kOTA_Err_SignatureCheckFailed;
}
/*-----------------------------------------------------------*/

static uint8_t * prvPAL_ReadAndAssumeCertificate( const uint8_t * const pucCertName,
                                                  uint32_t * const ulSignerCertSize )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_ReadAndAssumeCertificate" );

    /* Note: We use the aws_ota_codesigner_certificate.h instead of pucCertName */

    uint8_t * pucCertData;
    uint32_t ulCertSize;
    uint8_t * pucSignerCert = NULL;

    OTA_LOG_L1( "[%s] : Using aws_ota_codesigner_certificate.h.\r\n", OTA_METHOD_NAME);

    /* Allocate memory for the signer certificate plus a terminating zero so we can copy it and return to the caller. */
    ///	ulCertSize = sizeof( signingcredentialSIGNING_CERTIFICATE_PEM );
    ///	pucSignerCert = pvPortMalloc( ulCertSize + 1 );                       /*lint !e9029 !e9079 !e838 malloc proto requires void*. */
    ///	pucCertData = ( uint8_t * ) signingcredentialSIGNING_CERTIFICATE_PEM; /*lint !e9005 we don't modify the cert but it could be set by PKCS11 so it's not const. */

    if( pucSignerCert != NULL )
    {
    ///		memcpy( pucSignerCert, pucCertData, ulCertSize );
    	/* The crypto code requires the terminating zero to be part of the length so add 1 to the size. */
    ///		pucSignerCert[ ulCertSize ] = 0U;
    ///		*ulSignerCertSize = ulCertSize + 1U;
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

    /* Bank swap processing (Existing Firmware => Update Firmware) */
	bank_swap();
    while(1);

    /* We shouldn't actually get here if the board supports the auto reset.
     * But, it doesn't hurt anything if we do although someone will need to
     * reset the device for the new image to boot. */

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
    	eSavedAgentState = eState;
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

	OTA_PAL_ImageState_t ePalState = eOTA_PAL_ImageState_Unknown;

	switch (eSavedAgentState)
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

void ota_firmware_update_request(OTA_FileContext_t * const C)
{
    uint8_t     drvno = 0 ;

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
		strcpy((char *)load_firmware_control_block.file_name, C->pucFilePath);
		load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERASE;
	}
}

void firmware_update_status_initialize(void)
{
	load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_INITIALIZE;
}
uint32_t get_update_data_size(void)
{
	return (load_firmware_control_block.firmware_length * 4);
}

bool is_firmupdating(void)
{
	if(FIRMWARE_UPDATE_STATE_CAN_SWAP_BANK < load_firmware_control_block.status)
	{
		return false;
	}
	return true;
}

bool is_firmupdatewaitstart(void)
{
	if(FIRMWARE_UPDATE_STATE_WAIT_START != load_firmware_control_block.status)
	{
		return false;
	}
	return true;
}

/***********************************************************************************************************************
* Function Name: flash_copy_vector_table
* Description  : Moves the relocatable vector table to RAM. This is only needed if ROM operations are performed.
*                ROM cannot be accessed during operations. The vector table is located in ROM and will be accessed
*                if an interrupt occurs.
* Arguments    : none
* Return Value : uint32_t *flash_vect
***********************************************************************************************************************/
static uint32_t *flash_copy_vector_table(void)
{
    uint32_t *flash_vect;
    uint32_t   i;

    /* Get address of variable vector table in ROM */
    flash_vect = (uint32_t *)get_intb();

    /* Copy over variable vector table to RAM */
    for(i = 0; i < 256; i++ )
    {
    	g_ram_vector_table[i] = (uint32_t)dummy_int;      // Copy over entry
    }

    g_ram_vector_table[VECT_FCU_FIFERR] = flash_vect[VECT_FCU_FIFERR];
    g_ram_vector_table[VECT_FCU_FRDYI] = flash_vect[VECT_FCU_FRDYI];

    /* Set INTB to ram address */
#if __RENESAS_VERSION__ >= 0x01010000
    set_intb((void *)&g_ram_vector_table[0] );
#else
    set_intb( (uint32_t)&g_ram_vector_table[0] );
#endif
    return flash_vect;
}

/******************************************************************************
 Function Name   : bank_swap()
 Description     :
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void bank_swap(void)
{
	uint32_t level;
	flash_err_t flash_err;
	if(FIRMWARE_UPDATE_STATE_CAN_SWAP_BANK < load_firmware_control_block.status)
	{
		level = R_BSP_CpuInterruptLevelRead();
		R_BSP_CpuInterruptLevelWrite(14);
		flash_copy_vector_table();
		flash_err = R_FLASH_Control(FLASH_CMD_BANK_TOGGLE, NULL);
		if(FLASH_SUCCESS == flash_err)
		{
			while(1);	/* wait software reset in RAM */
		}
		while(1); /* death loop */
	}
}

void load_firmware_status(uint32_t *now_status, uint32_t *finish_status)
{
	*now_status    = load_firmware_control_block.status;
	*finish_status = FIRMWARE_UPDATE_STATE_COMPLETED;
}

uint32_t load_firmware_process(void)
{
	flash_interrupt_config_t cb_func_info;
	uint32_t required_dataflash_block_num;
	uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];

	switch(load_firmware_control_block.status)
	{
		case FIRMWARE_UPDATE_STATE_INITIALIZE: /* initialize */
			load_firmware_control_block.progress = 0;
			load_firmware_control_block.offset = 0;
			memset(&firmware_update_control_block_image, 0, sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK));
			memcpy(&firmware_update_control_block_image, (void *)FLASH_DF_BLOCK_0, sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK));
			cb_func_info.pcallback = flash_load_firmware_callback_function;
		    cb_func_info.int_priority = 15;
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
			if(!serial_flash_firm_block_read(load_firmware_control_block.flash_buffer, load_firmware_control_block.offset))
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
		case FIRMWARE_UPDATE_STATE_FINALIZE: /* finalize */
            R_Sha1((uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, hash_sha1, FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);

		    if(!memcmp(hash_sha1, load_firmware_control_block.hash_sha1, SHA1_HASH_LENGTH_BYTE_SIZE))
		    {
		    	/* confirm which hash(hash0/hash1 on dataflash) is same as current executed area hash */
			    R_Sha1((uint8_t*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, hash_sha1, FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);

				if(!memcmp(hash_sha1, firmware_update_control_block_image.data.hash0_sha1, SHA1_HASH_LENGTH_BYTE_SIZE))
				{
					/* hash0_sha1 is for current executed area mac, should write to hash1_sha1 */
					memcpy(firmware_update_control_block_image.data.hash1_sha1, load_firmware_control_block.hash_sha1, sizeof(firmware_update_control_block_image.data.hash1_sha1));
				}
				else
				{
					/* program_mac1 is for current executed area mac, should write to hash0_sha1 */
					memcpy(firmware_update_control_block_image.data.hash0_sha1, load_firmware_control_block.hash_sha1, sizeof(firmware_update_control_block_image.data.hash0_sha1));
				}
            	firmware_update_control_block_image.data.lifecycle_state = LIFECYCLE_STATE_UPDATING;
    		    R_Sha1((uint8_t *)&firmware_update_control_block_image.data, hash_sha1, sizeof(firmware_update_control_block_image.data));
    		    memcpy(firmware_update_control_block_image.hash_sha1, hash_sha1, SHA1_HASH_LENGTH_BYTE_SIZE);
    		    required_dataflash_block_num = sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK) / FLASH_DF_BLOCK_SIZE;
    		    if(sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK) % FLASH_DF_BLOCK_SIZE)
    		    {
    		    	required_dataflash_block_num++;
    		    }
				R_FLASH_Erase((flash_block_address_t)FLASH_DF_BLOCK_0, required_dataflash_block_num);
				load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_ERASE_DATA_FLASH;
		    }
		    else
		    {
			    load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
		    }
		    break;
		case FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_ERASE_DATA_FLASH:
			/* this state will be changed by callback routine */
			break;
		case FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_WRITE_DATA_FLASH:
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
			else if(FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_ERASE_DATA_FLASH == load_firmware_control_block.status)
			{
				R_FLASH_Write((uint32_t)&firmware_update_control_block_image, FLASH_DF_BLOCK_0, sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK));
				load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_WRITE_DATA_FLASH;
			}
			else
			{
				load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
			}
			break;
		case FLASH_INT_EVENT_WRITE_COMPLETE:
			if(FIRMWARE_UPDATE_STATE_WRITE_WAIT_COMPLETE == load_firmware_control_block.status)
			{
				load_firmware_control_block.offset += FLASH_BUCKET_SIZE;
		    	load_firmware_control_block.progress = (uint32_t)(((float)(load_firmware_control_block.offset)/(float)((FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))*100));

		    	if( load_firmware_control_block.firmware_length == load_firmware_control_block.offset )
				{
					load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_FINALIZE;
				}
				else
				{
					load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_READ_WAIT_COMPLETE;
				}
			}
			else if(FIRMWARE_UPDATE_STATE_FINALIZE_WAIT_WRITE_DATA_FLASH == load_firmware_control_block.status)
			{
				load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_FINALIZE_COMPLETED;
			}
			else
			{
				load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
			}
			break;
		case FLASH_INT_EVENT_TOGGLE_BANK:
	        R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
	        SYSTEM.SWRR = 0xa501;
			while(1);	/* software reset */
			break;
		default:
			nop();
			break;
	}
}


/***********************************************************************************************************************
* Function Name: serial_flash_firm_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static int32_t serial_flash_firm_block_read(uint32_t *firmware, uint32_t offset)
{
    uint8_t buf[256] = {0};
    uint8_t arg1[256] = {0};
    uint8_t arg2[256] = {0};
    uint8_t arg3[256] = {0};
    uint32_t read_size = 0;
    static uint32_t upprogram[4 + 1] = {0};
    static uint32_t current_char_position = 0;
    static uint32_t current_file_position = 0;
    static uint32_t previous_file_position = 0;

    flash_spi_info_t Flash_Info_R;
    flash_spi_status_t Ret = FLASH_SPI_SUCCESS;

    if (0 == offset)
    {
        current_char_position = 0;
        current_file_position = 0;
        previous_file_position = 0;
        memset(upprogram,0,sizeof(upprogram));
    }

    xSemaphoreTake(OTASemaphoreHandle, portMAX_DELAY);

	current_char_position = 0;
	memset(buf, 0, sizeof(buf));

	while(1)
	{
        memset(buf, 0, sizeof(buf));
	    Flash_Info_R.addr   = previous_file_position;
	    Flash_Info_R.cnt    = FREAD_SIZE;
	    Flash_Info_R.p_data = &buf[current_char_position];
	    Flash_Info_R.op_mode = FLASH_SPI_SINGLE;
        Ret = R_FLASH_SPI_Read_Data(0, &Flash_Info_R);
        if (FLASH_SPI_SUCCESS == Ret)
		{
			previous_file_position += FREAD_SIZE;

			/* received 1 line */
			if(strstr((char*)buf, "\r\n"))
			{
				memset(arg1, 0, sizeof(arg1));
				memset(arg2, 0, sizeof(arg2));
				memset(arg3, 0, sizeof(arg3));
				
				sscanf((char*)buf, "%256s %256s %256s", (char*)arg1, (char*)arg2, (char*)arg3);

				if (!strcmp((char *)arg1, "sha1"))
				{
#if (FREAD_SIZE == 45U)
					previous_file_position -= (CF_DATA_LINE_NUM - PARAM_DATA_LINE_NUM);
					for(int i=PARAM_DATA_LINE_NUM; i < CF_DATA_LINE_NUM; i++)
					{
						arg2[i] = NULL;
					}
#endif
					base64_decode(arg2, (uint8_t *)load_firmware_control_block.hash_sha1, strlen((char *)arg2));
				}
				if (!strcmp((char *)arg1, "max_cnt"))
				{
					sscanf((char*) arg2, "%x", load_firmware_control_block.firmware_length);
				}
				if (!strcmp((char *)arg1, "upprogram"))
				{
            		base64_decode(arg3, (uint8_t *)upprogram, strlen((char *)arg3));
            		memcpy(&firmware[current_file_position], upprogram, 16);
                	current_file_position += 4;
                	read_size += 16;
				}
#if (FREAD_SIZE == 45U)
				if (!strcmp((char *)arg1, "upconst"))
				{
					// DATA FLASH data
					previous_file_position -= (CF_DATA_LINE_NUM - DF_DATA_LINE_NUM);
				}
#endif
				if((current_file_position * 4) >= FLASH_BUCKET_SIZE)
				{
					current_file_position = 0;
					break;
				}
				current_char_position = 0;
				memset(buf, 0, sizeof(buf));
			}
		}
		else
		{
			goto serial_flash_firm_block_read_error;
			break;
		}
	}

    xSemaphoreGive(OTASemaphoreHandle);
	if (FLASH_SPI_SUCCESS != Ret)
	{
serial_flash_firm_block_read_error:
		return -1;
	}

    return 0;
}

/******************************************************************************
Function Name   : on_the_fly_write_process
Description     : 
Arguments       : 
Return Value    : 
******************************************************************************/
int16_t serial_flash_write_process(uint8_t *buf, int16_t write_size, uint32_t Offset)
{
    flash_spi_info_t Flash_Info_W;
    flash_spi_status_t Ret = FLASH_SPI_SUCCESS;

    xSemaphoreTake(OTASemaphoreHandle, portMAX_DELAY);

    uint32_t internal_cnt = 0;

    do
    {
        Flash_Info_W.addr    = Offset + internal_cnt;
        Flash_Info_W.cnt     = SERIAL_FLASH_FIT_COUNT;
        Flash_Info_W.p_data  = (uint8_t *)&buf[internal_cnt];
        Flash_Info_W.op_mode = FLASH_SPI_SINGLE;
        Ret = R_FLASH_SPI_Write_Data_Page(0, &Flash_Info_W);
        if (FLASH_SPI_SUCCESS > Ret)
        {
            return -1;
        }
        while (FLASH_SPI_SUCCESS_BUSY == R_FLASH_SPI_Polling(0, FLASH_SPI_MODE_PROG_POLL))
        {
            vTaskDelay(5);
        }
        internal_cnt += SERIAL_FLASH_FIT_COUNT;
    }
    while (internal_cnt < write_size);

    xSemaphoreGive(OTASemaphoreHandle);

    return 0;
}

#pragma section FRAM2
#pragma interrupt (dummy_int)
static void dummy_int(void)
{
	/* nothing to do */
}


/* end of file */
