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
#include "base64_decode.h"

/* Specify the OTA signature algorithm we support on this platform. */
//const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha256-ecdsa";   /* FIX ME. */
const char cOTA_JSON_FileSignatureKey[ OTA_FILE_SIG_KEY_STR_MAX_LENGTH ] = "sig-sha1-rsa";   /* for Renesas implementation */


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
 * This function is called\r\n from prvPAL_Close().
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
 * This function is called\r\n from prvPAL_CheckFileSignature(). It should be implemented if signature
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

/* Renesas specific implementation data */
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

#define FLASH_INTERRUPT_PRIORITY 15	/* 0(low) - 15(high) */
/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/

#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)

#define SHA1_HASH_LENGTH_BYTE_SIZE 20

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

#define FRAGMENTED_PACKET_TYPE_HEAD 0
#define FRAGMENTED_PACKET_TYPE_TAIL 1

#define FRAGMENTED_FLASH_BLOCK_TYPE_HEAD 0
#define FRAGMENTED_FLASH_BLOCK_TYPE_TAIL 1

#define ONE_COMMAND_BINARY_LENGH 16
#define HASH_SHA1_LENGTH 20

typedef struct _load_firmware_control_block {
	uint32_t status;
	uint8_t *flash_buffer_for_erase_unit;
	uint8_t *flash_buffer_program_unit;
	uint32_t processed_byte;
	volatile uint32_t flash_write_in_progress_flag;
	volatile uint32_t flash_erase_in_progress_flag;
	uint32_t progress;
	uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];
	OTA_ImageState_t eSavedAgentState;
	OTA_FileContext_t OTA_FileContext;
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

typedef struct _fragmented_packet
{
	uint32_t packet_number;
	uint8_t type;	/* 0: head, 1: tail */
	uint8_t	*string;
	uint8_t length;
}FRAGMENTED_PACKET;

typedef struct _fragmented_packet_list
{
	FRAGMENTED_PACKET content;
	struct _fragmented_packet_list *next;
}FRAGMENTED_PACKET_LIST;

typedef struct _fragmented_flash_block
{
	uint32_t address;
	uint8_t	*binary;
	uint8_t length;
}FRAGMENTED_FLASH_BLOCK;

typedef struct _fragmented_flash_block_list
{
	FRAGMENTED_FLASH_BLOCK content;
	struct _fragmented_flash_block_list *next;
}FRAGMENTED_FLASH_BLOCK_LIST;

static LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_head;
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_head;

static void bank_swap(void);
static uint32_t *flash_copy_vector_table(void);
static void dummy_int(void);
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_add(FRAGMENTED_PACKET_LIST *head, uint32_t packet_number, uint8_t type, uint8_t *string, uint8_t length);
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_delete(FRAGMENTED_PACKET_LIST *head, uint32_t packet_number, uint8_t type);
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_search(FRAGMENTED_PACKET_LIST *head, uint32_t packet_number, uint8_t type);
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_print(FRAGMENTED_PACKET_LIST *head);

static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_insert(FRAGMENTED_FLASH_BLOCK_LIST *head, uint32_t address, uint8_t *binary, uint8_t length);
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_delete(FRAGMENTED_FLASH_BLOCK_LIST *head, uint32_t address);
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_print(FRAGMENTED_FLASH_BLOCK_LIST *head);
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_assemble(FRAGMENTED_FLASH_BLOCK_LIST *head, uint8_t *flash_block, uint32_t *target_address);
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_sort(FRAGMENTED_FLASH_BLOCK_LIST *head);

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

    uint8_t *test = 0;
    for(int i = 0; i < 256; i++)
    {
    	test[i] = 0x55;
    }


    if( C != NULL )
    {
        if( C->pucFilePath != NULL )
        {
        	if((load_firmware_control_block.status != FIRMWARE_UPDATE_STATE_CLOSED)
        			|| (load_firmware_control_block.status != FIRMWARE_UPDATE_STATE_FINALIZE_COMPLETED)
        			|| (load_firmware_control_block.status != FIRMWARE_UPDATE_STATE_COMPLETED)
        			|| (load_firmware_control_block.status != FIRMWARE_UPDATE_STATE_ERROR))
        	{
				memset(&load_firmware_control_block, 0, sizeof(LOAD_FIRMWARE_CONTROL_BLOCK));
				load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_INITIALIZED;
				load_firmware_control_block.eSavedAgentState = eOTA_ImageState_Unknown;
				memcpy(&load_firmware_control_block.OTA_FileContext, C, sizeof(OTA_FileContext_t));
				C->pucFile = dummy_file_pointer;
				fragmented_packet_list_head = NULL;
				fragmented_flash_block_list_head = NULL;
				eResult = kOTA_Err_None;
				OTA_LOG_L1( "[%s] Receive file created.\r\n", OTA_METHOD_NAME );
        	}
        	else
        	{
                eResult = kOTA_Err_RxFileCreateFailed;
                OTA_LOG_L1( "[%s] ERROR - now in progress.\r\n", OTA_METHOD_NAME );
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
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

    /* Set default return status to uninitialized. */
    OTA_Err_t eResult = kOTA_Err_Uninitialized;

    if( NULL != C )
    {
		memset(&load_firmware_control_block, 0, sizeof(LOAD_FIRMWARE_CONTROL_BLOCK));
		load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_ERROR;
        OTA_LOG_L1( "[%s] OK\r\n", OTA_METHOD_NAME );
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
    /* Set default return status to uninitialized. */
    OTA_Err_t eResult = kOTA_Err_Uninitialized;

    uint8_t arg1[32], arg2[32], arg3[32];
    uint8_t *current_index;
    uint8_t *next_index;
    uint8_t *assembled_packet_buffer;
    uint32_t assembled_packet_buffer_size = ulBlockSize;
    uint32_t address;
    uint8_t binary[ONE_COMMAND_BINARY_LENGH];
    uint8_t flash_block[FLASH_CF_MIN_PGM_SIZE];
    uint32_t length;
    uint32_t head_fragmented_size = 0, tail_fragmented_size = 0;
    uint8_t pacData_string[(1 << otaconfigLOG2_FILE_BLOCK_SIZE) + 1]; /* +1 means string terminator 0 */
    uint8_t hash_sha1[HASH_SHA1_LENGTH];

    FRAGMENTED_PACKET_LIST *previous_fragmented_packet, *next_fragmented_packet;
    uint32_t specified_packet_number = (ulOffset / (1 << otaconfigLOG2_FILE_BLOCK_SIZE));

    pacData_string[(1 << otaconfigLOG2_FILE_BLOCK_SIZE)] = 0;
    memcpy(pacData_string, pacData, ulBlockSize);
    pacData_string[ulBlockSize] = 0; /* string terminator */

	current_index = pacData_string;

	/*----------- pre-process for fragmented line on head/tail of pacData ----------*/
	/* calculate assembled_packet_buffer size for head of pacData */
	previous_fragmented_packet = fragmented_packet_list_search(fragmented_packet_list_head, specified_packet_number - 1, FRAGMENTED_PACKET_TYPE_TAIL);
	if(previous_fragmented_packet != NULL)
	{
		assembled_packet_buffer_size += previous_fragmented_packet->content.length;
	}
	/* cutting off the fragment of head of pacData and add this into list */
	else
	{
		if(specified_packet_number != 0)
		{
			next_index = (uint8_t*)strstr((char*)current_index, "\n");
			next_index += 1; /* +1 means LF */
			head_fragmented_size = next_index - current_index;
			assembled_packet_buffer = pvPortMalloc(head_fragmented_size + 1); /* +1 means string terminator 0 */
			assembled_packet_buffer[head_fragmented_size] = 0;
			memcpy(assembled_packet_buffer, current_index, head_fragmented_size);
			fragmented_packet_list_head = fragmented_packet_list_add(fragmented_packet_list_head, specified_packet_number, FRAGMENTED_PACKET_TYPE_HEAD, assembled_packet_buffer, strlen((char *)assembled_packet_buffer));
			vPortFree(assembled_packet_buffer);
			assembled_packet_buffer_size -= head_fragmented_size;
			current_index = next_index;
		}
		/* no more fragment data before head of the pacData */
		else
		{
			/* nothing to do */
		}
	}
	/* calculate assembled_packet_buffer size for tail of pacData */
	next_fragmented_packet = fragmented_packet_list_search(fragmented_packet_list_head, specified_packet_number + 1, FRAGMENTED_PACKET_TYPE_HEAD);
	if(next_fragmented_packet != NULL)
	{
		assembled_packet_buffer_size += next_fragmented_packet->content.length;
	}
	/* cutting off the fragment of tail of pacData and add this into list */
	else
	{
		/* search last 1 line */
		while(1)
		{
			next_index = (uint8_t*)strstr((char*)current_index, "\n");
			if(next_index == NULL)
			{
				break;
			}
			next_index += 1; /* +1 means LF */
			current_index = next_index;
		}
		tail_fragmented_size = strlen((char *)current_index);
		assembled_packet_buffer = pvPortMalloc(tail_fragmented_size + 1); /* +1 means string terminator 0 */
		assembled_packet_buffer[tail_fragmented_size] = 0;
		memcpy(assembled_packet_buffer, current_index, tail_fragmented_size);
		fragmented_packet_list_head = fragmented_packet_list_add(fragmented_packet_list_head, specified_packet_number, FRAGMENTED_PACKET_TYPE_TAIL, assembled_packet_buffer, strlen((char *)assembled_packet_buffer));
		vPortFree(assembled_packet_buffer);
		assembled_packet_buffer_size -= tail_fragmented_size;
		pacData_string[ulBlockSize - tail_fragmented_size] = 0;
	}

	/* allocate the assembled_packet_buffer memory */
	assembled_packet_buffer = pvPortMalloc(assembled_packet_buffer_size + 1); /* +1 means string terminator 0 */
	assembled_packet_buffer[assembled_packet_buffer_size] = 0;
	assembled_packet_buffer[0] = 0;

	/* if previous fragment packet would be found, it is needed to be combined into head of pacData */
	if(previous_fragmented_packet != NULL)
	{
		strcat((char *)assembled_packet_buffer, (char *)previous_fragmented_packet->content.string);
		fragmented_packet_list_head = fragmented_packet_list_delete(fragmented_packet_list_head, specified_packet_number - 1, FRAGMENTED_PACKET_TYPE_TAIL);
	}
	strcat((char *)assembled_packet_buffer, (char *)&pacData_string[head_fragmented_size]);

	/* if next fragment packet would be found, it is needed to be combined into tail of pacData */
	if(next_fragmented_packet != NULL)
	{
		strcat((char *)assembled_packet_buffer, (char *)next_fragmented_packet->content.string);
		fragmented_packet_list_head = fragmented_packet_list_delete(fragmented_packet_list_head, specified_packet_number + 1, FRAGMENTED_PACKET_TYPE_HEAD);
	}
	/*----------- process each commands on assembled_packet_buffer ----------*/
    current_index = assembled_packet_buffer;
	next_index = (uint8_t*)strstr((char*)current_index, "\n");
	if(next_index != NULL)
	{
		next_index += 1; /* +1 means LF */
	    while(1)
	    {
	        /* extract 1 line*/
	    	sscanf((char*)current_index, "%32s %32s %32s", (char*)arg1, (char*)arg2, (char*)arg3);
			if(!strcmp((char*)arg1, "upprogram"))
			{
				sscanf((char *)arg2, "%x", &address);
				length = base64_decode(arg3, binary, strlen((char *)arg3));
				fragmented_flash_block_list_head = fragmented_flash_block_list_insert(fragmented_flash_block_list_head, address, binary, length);
			}
			else if(!strcmp((char*)arg1, "sha1"))
			{
				base64_decode(arg2, hash_sha1, strlen((char *)arg2));
				memcpy(load_firmware_control_block.hash_sha1, hash_sha1, HASH_SHA1_LENGTH);
			}

			next_index = (uint8_t*)strstr((char*)current_index, "\n");
			next_index += 1; /* +1 means LF */
			current_index = next_index;
			/* exit if no more command would be found */
			if(!strlen((char *)current_index))
			{
				eResult = kOTA_Err_None;
				break;
			}
	    }
	}
	else
	{
		OTA_LOG_L1("illegal data format [%s].\r\n", current_index);
		eResult = kOTA_Err_OutOfMemory;
    	/* todo: リスト全消去 */
	}
	/*----------- write (request to queue) flash phase ----------*/
	if(fragmented_flash_block_list_head != NULL)
	{
		while(1)
		{
			fragmented_flash_block_list_head = fragmented_flash_block_list_assemble(fragmented_flash_block_list_head, flash_block, &address);
			if(address != NULL)
			{
				/* todo: write request and continue */
			}
			else
			{
				break;
			}
		}
	}
	/*----------- finalize phase ----------*/
	fragmented_packet_list_print(fragmented_packet_list_head);
	fragmented_flash_block_list_print(fragmented_flash_block_list_head);
	vPortFree(assembled_packet_buffer);
	return eResult;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_CloseFile( OTA_FileContext_t * const C )
{
    DEFINE_OTA_METHOD_NAME( "prvPAL_CloseFile" );
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

    OTA_Err_t eResult = kOTA_Err_None;
    if( NULL != C )
    {
    	if(load_firmware_control_block.status != FIRMWARE_UPDATE_STATE_FINALIZE_COMPLETED)
    	{
			memset(&load_firmware_control_block, 0, sizeof(LOAD_FIRMWARE_CONTROL_BLOCK));
			load_firmware_control_block.status = FIRMWARE_UPDATE_STATE_CLOSED;
			OTA_LOG_L1( "[%s] OK\r\n", OTA_METHOD_NAME );
			eResult = kOTA_Err_None;
    	}
		else
		{
			OTA_LOG_L1( "[%s] ERROR - Invalid status.\r\n", OTA_METHOD_NAME );
			eResult = kOTA_Err_FileClose;
		}
    }
    else /* Context was not valid. */
    {
        OTA_LOG_L1( "[%s] ERROR - Invalid context.\r\n", OTA_METHOD_NAME );
        eResult = kOTA_Err_FileAbort;
    }
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
    OTA_LOG_L1("[%s] is called.\r\n", OTA_METHOD_NAME);

    return kOTA_Err_None;
}
/*-----------------------------------------------------------*/

OTA_Err_t prvPAL_ActivateNewImage( void )
{
    DEFINE_OTA_METHOD_NAME("prvPAL_ActivateNewImage");

    OTA_LOG_L1( "[%s] Changing the Startup Bank\r\n", OTA_METHOD_NAME );

    /* Bank swap processing */
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

/******************************************************************************
 Function Name   : bank_swap()
 Description     :
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void bank_swap(void)
{
	flash_err_t flash_err;
	if(FIRMWARE_UPDATE_STATE_CAN_SWAP_BANK == load_firmware_control_block.status)
	{
		R_BSP_CpuInterruptLevelWrite(FLASH_INTERRUPT_PRIORITY - 1);
		flash_copy_vector_table();
		flash_err = R_FLASH_Control(FLASH_CMD_BANK_TOGGLE, NULL);
		if(FLASH_SUCCESS == flash_err)
		{
			while(1);	/* wait software reset in RAM */
		}
		while(1); /* death loop */
	}
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
    static uint32_t g_ram_vector_table[256];      // RAM space to hold the vector table

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

#pragma section FRAM2
#pragma interrupt (dummy_int)
static void dummy_int(void)
{
	/* nothing to do */
}
#pragma section

/***********************************************************************************************************************
* Function Name: fragmented_packet_list_add
* Description  : Add the fragmented packet into the list
* Arguments    : head - current head of list
*                packet_number - packet number of block
*                type - fragment direction
*                string - character string
*                length - string length
* Return Value : FRAGMENTED_PACKET_LIST* - new head of list (returns same value as specified head in normally,
*                                                            new head would be returned when head would be specified as NULL.)
***********************************************************************************************************************/
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_add(FRAGMENTED_PACKET_LIST *head, uint32_t packet_number, uint8_t type, uint8_t *string, uint8_t length)
{
	FRAGMENTED_PACKET_LIST *tmp, *new;

    new = pvPortMalloc(sizeof(FRAGMENTED_PACKET_LIST));
    new->content.string = pvPortMalloc(length + 1); /* +1 means string terminator 0 */
    new->content.string[0] = 0;
    new->content.string[length] = 0;

    if((new != NULL) && (new->content.string != NULL))
    {
        strcpy((char *)new->content.string, (char *)string);
    	new->content.packet_number = packet_number;
    	new->content.type = type;
    	new->content.length = length;
    	new->next = NULL;

    	/* new head would be returned when head would be specified as NULL. */
    	if(head == NULL)
    	{
    		tmp = new;
    	}
    	else
    	{
        	/* store the allocated memory "new" into "tail of list" */
	    	tmp = head;
			while(tmp->next != NULL)
			{
				tmp = tmp->next;
			}
	    	tmp->next = new;

	    	/* returns same value as specified head in normally */
	    	tmp = head;
    	}
    }
    else
    {
    	tmp = NULL;
    	/* todo: リスト全消去 */
    }
    return tmp;
}

/***********************************************************************************************************************
* Function Name: fragmented_packet_list_delete
* Description  : Delete the fragmented packet into the list
* Arguments    : head - current head of list
*                packet_number - packet number of block
*                type - fragment direction
*
* Return Value : FRAGMENTED_PACKET_LIST* - new head of list (returns same value as specified head in normally,
*                                                            new head would be returned when head would be deleted.
*                                                            NULL would be returned when all list are deleted.)
***********************************************************************************************************************/
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_delete(FRAGMENTED_PACKET_LIST *head, uint32_t packet_number, uint8_t type)
{
	FRAGMENTED_PACKET_LIST *tmp = head, *previous = NULL;

	if(head != NULL)
	{
		while(1)
		{
			if((tmp->content.packet_number == packet_number) && (tmp->content.type == type))
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
				vPortFree(tmp->content.string);
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
				vPortFree(head->content.string);
				vPortFree(head);
			}
		}
	}
	return tmp;
}

static FRAGMENTED_PACKET_LIST *fragmented_packet_list_search(FRAGMENTED_PACKET_LIST *head, uint32_t packet_number, uint8_t type)
{
	FRAGMENTED_PACKET_LIST *tmp = head;

	while(1)
	{
		if((tmp->content.packet_number == packet_number) && (tmp->content.type == type))
		{
			break;
		}
		if(tmp->next == NULL)
		{
			tmp = NULL;
			break;
		}
		tmp = tmp->next;
	}
	return tmp;
}

static FRAGMENTED_PACKET_LIST *fragmented_packet_list_print(FRAGMENTED_PACKET_LIST *head)
{
	FRAGMENTED_PACKET_LIST *tmp = head;
	uint32_t total_heap_length, total_list_count;

	total_heap_length = 0;
	total_list_count = 0;

	if(head != NULL)
	{
		while(1){
			total_heap_length += sizeof(FRAGMENTED_PACKET_LIST);
			total_heap_length += tmp->content.length;
			total_list_count++;
			if(tmp->next == NULL)
			{
				break;
			}
			tmp = tmp->next;
		};
	}
    OTA_LOG_L2("FRAGMENTED_PACKET_LIST: total_heap_length = [%d], total_list_count = [%d].\r\n", total_heap_length, total_list_count);

	return tmp;
}

/***********************************************************************************************************************
* Function Name: fragmented_flash_block_list_insert
* Description  : Add the fragmented packet into the list
* Arguments    : head - current head of list
*                block_number - packet number of block
*                type - fragment direction
*                binary - binary array
*                length - binary length
* Return Value : FRAGMENTED_PACKET_LIST* - new head of list (returns same value as specified head in normally,
*                                                            new head would be returned when head would be specified as NULL.)
***********************************************************************************************************************/
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_insert(FRAGMENTED_FLASH_BLOCK_LIST *head, uint32_t address, uint8_t *binary, uint8_t length)
{
	FRAGMENTED_FLASH_BLOCK_LIST *tmp, *current, *previous, *new;
	static FRAGMENTED_FLASH_BLOCK_LIST *tail;

    new = pvPortMalloc(sizeof(FRAGMENTED_PACKET_LIST));
    new->content.binary = pvPortMalloc(length);

    if((new != NULL) && (new->content.binary != NULL))
    {
        memcpy(new->content.binary, binary, length);
    	new->content.address = address;
    	new->content.length = length;
    	new->next = NULL;

    	/* new head would be returned when head would be specified as NULL. */
    	if(head == NULL)
    	{
    		tmp = new;
    		tail = new;
    	}
    	else
    	{
    		if(new->content.address > tail->content.address)
    		{
    			tail->next = new;
    			tail = new;
    			tmp = head;
    		}
    		else
    		{
				/* search the list to insert new node */
				current = head;
				while(1)
				{
					if((new->content.address < current->content.address) || (current == NULL))
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
    }
    else
    {
    	tmp = NULL;
    	/* todo: リスト全消去 */
    }
    return tmp;
}

/***********************************************************************************************************************
* Function Name: fragmented_flash_block_delete
* Description  : Delete the fragmented packet into the list
* Arguments    : head - current head of list
*                block_number - packet number of block
*                type - fragment direction
*
* Return Value : FRAGMENTED_PACKET_LIST* - new head of list (returns same value as specified head in normally,
*                                                            new head would be returned when head would be deleted.
*                                                            NULL would be returned when all list are deleted.)
***********************************************************************************************************************/
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_delete(FRAGMENTED_FLASH_BLOCK_LIST *head, uint32_t address)
{
	FRAGMENTED_FLASH_BLOCK_LIST *tmp = head, *previous = NULL;

	if(head != NULL)
	{
		while(1)
		{
			if(tmp->content.address == address)
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

/***********************************************************************************************************************
* Function Name: fragmented_flash_block_list_assemble
* Description  : Assemble the fragmented flash block from the list
* Arguments    : head - current head of list
*                flash_block - assembled flash_block[FLASH_CF_MIN_PGM_SIZE] from the list
*                target_address - top of actual physical address of the flash_block, if this value would be NULL, no target flash data exists.
* Return Value : FRAGMENTED_PACKET_LIST* - new head of list (returns same value as specified head in normally,
*                                                            new head would be returned when head would be deleted.
*                                                            NULL would be returned when all list are deleted.)
***********************************************************************************************************************/
static FRAGMENTED_FLASH_BLOCK_LIST *fragmented_flash_block_list_assemble(FRAGMENTED_FLASH_BLOCK_LIST *head, uint8_t *flash_block, uint32_t *target_address)
{
	FRAGMENTED_FLASH_BLOCK_LIST *tmp = head;
	FRAGMENTED_FLASH_BLOCK_LIST *flash_block_candidate[FLASH_CF_MIN_PGM_SIZE/ONE_COMMAND_BINARY_LENGH];
	uint32_t assembled_length = 0;
	uint32_t loop_counter;

	/* search aligned FLASH_CF_MIN_PGM_SIZE top address */
	while(1)
	{
		if(!(tmp->content.address % FLASH_CF_MIN_PGM_SIZE))
		{
			/* find out flash_block candidate */
			assembled_length = 0;
			loop_counter = 0;
			for(uint32_t i = 0; i < (FLASH_CF_MIN_PGM_SIZE / ONE_COMMAND_BINARY_LENGH); i++)
			{
				if(tmp->content.address + tmp->content.length == tmp->next->content.address)
				{
					assembled_length += tmp->content.length;
					flash_block_candidate[loop_counter++] = tmp;
					tmp = tmp->next;
				}
				else if(loop_counter == ((FLASH_CF_MIN_PGM_SIZE / ONE_COMMAND_BINARY_LENGH) - 1))
				{
					if((assembled_length + tmp->next->content.length) == FLASH_CF_MIN_PGM_SIZE)
					{
						assembled_length += tmp->next->content.length;
						flash_block_candidate[loop_counter++] = tmp;
						tmp = tmp->next;
						break;
					}
				}
				else
				{
					tmp = tmp->next;
					break;
				}
			}
		}
		else
		{
			tmp = tmp->next;
		}
		if((assembled_length == FLASH_CF_MIN_PGM_SIZE) || (tmp->next == NULL))
		{
			break;
		}
	}

	tmp = head;
	*target_address = NULL;
	/* remove flash_block from list */
	if(assembled_length == FLASH_CF_MIN_PGM_SIZE)
	{
		*target_address = flash_block_candidate[0]->content.address;
		for(uint32_t i = 0; i < (FLASH_CF_MIN_PGM_SIZE/ONE_COMMAND_BINARY_LENGH); i++)
		{
			memcpy(&flash_block[i * ONE_COMMAND_BINARY_LENGH], tmp->content.binary, tmp->content.length);
			tmp = fragmented_flash_block_list_delete(tmp, flash_block_candidate[i]->content.address);
		}
	}
	return tmp;

}
