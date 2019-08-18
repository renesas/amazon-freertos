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

#define OTA_FRAGMENT_MAX_LENGTH 128

#define FRAGMENTED_PACKET_TYPE_HEAD 0
#define FRAGMENTED_PACKET_TYPE_TAIL 1

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
	uint32_t block_number;
	uint8_t type;	/* 0: head, 1: tail */
	uint8_t	*string;
	uint8_t length;
}FRAGMENTED_PACKET;

typedef struct _fragmented_packet_list
{
	FRAGMENTED_PACKET content;
	struct _fragmented_packet_list *next;
}FRAGMENTED_PACKET_LIST;

static LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_head;

static void bank_swap(void);
static uint32_t *flash_copy_vector_table(void);
static void dummy_int(void);
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_add(FRAGMENTED_PACKET_LIST *head, uint32_t block_number, uint8_t type, uint8_t *string, uint8_t length);
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_search(FRAGMENTED_PACKET_LIST *head, uint32_t block_number, uint8_t type);
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_delete(FRAGMENTED_PACKET_LIST *head, uint32_t block_number, uint8_t type);
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_print(FRAGMENTED_PACKET_LIST *head);

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

    uint8_t arg1[16], arg2[16], arg3[32];
    uint8_t *current_index;
    uint8_t *next_index;
    uint8_t *buffer;
    uint32_t buffer_size = ulBlockSize;
    uint32_t fragmented_size = 0;
    uint8_t pacData_string[(1 << otaconfigLOG2_FILE_BLOCK_SIZE) + 1]; /* +1 means string terminator 0 */

    FRAGMENTED_PACKET_LIST *previous_fragmented_packet, *next_fragmented_packet;
    uint32_t specified_block_number = (ulOffset / (1 << otaconfigLOG2_FILE_BLOCK_SIZE));

    pacData_string[(1 << otaconfigLOG2_FILE_BLOCK_SIZE)] = 0;
    memcpy(pacData_string, pacData, ulBlockSize);

    /* pre-process for fragmented line on head/tail of pacData */
	/* calculate buffer size for head of pacData */
	previous_fragmented_packet = fragmented_packet_list_search(fragmented_packet_list_head, specified_block_number - 1, FRAGMENTED_PACKET_TYPE_TAIL);
	if(previous_fragmented_packet != NULL)
	{
		buffer_size += previous_fragmented_packet->content.length;
	}
	/* cutting off the fragment of head of pacData and add this into list */
	else
	{
		if(specified_block_number != 0)
		{
			current_index = pacData_string;
			next_index = (uint8_t*)strstr((char*)current_index, "\n");
			next_index += 1; /* +1 means LF */
			fragmented_size = next_index - current_index;
			buffer = pvPortMalloc(fragmented_size + 1); /* +1 means string terminator 0 */
			buffer[fragmented_size] = 0;
			pacData_string[fragmented_size] = 0;
			memcpy(buffer, current_index, fragmented_size);
			fragmented_packet_list_head = fragmented_packet_list_add(fragmented_packet_list_head, specified_block_number, FRAGMENTED_PACKET_TYPE_HEAD, buffer, strlen((char *)buffer));
			vPortFree(buffer);
			buffer_size -= fragmented_size;
		}
		/* no more fragment data before head of the pacData */
		else
		{
			/* nothing to do */
		}
	}
	/* calculate buffer size for tail of pacData */
	next_fragmented_packet = fragmented_packet_list_search(fragmented_packet_list_head, specified_block_number + 1, FRAGMENTED_PACKET_TYPE_HEAD);
	if(next_fragmented_packet != NULL)
	{
		buffer_size += next_fragmented_packet->content.length;
	}
	/* cutting off the fragment of tail of pacData and add this into list */
	else
	{
		current_index = pacData_string;
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
		fragmented_size = strlen((char *)current_index);
		buffer = pvPortMalloc(fragmented_size + 1); /* +1 means string terminator 0 */
		buffer[fragmented_size] = 0;
		memcpy(buffer, current_index, fragmented_size);
		fragmented_packet_list_head = fragmented_packet_list_add(fragmented_packet_list_head, specified_block_number, FRAGMENTED_PACKET_TYPE_TAIL, buffer, strlen((char *)buffer));
		vPortFree(buffer);
		buffer_size -= fragmented_size;
		pacData_string[ulBlockSize - fragmented_size] = 0;
	}

	/* allocate the buffer memory */
	buffer = pvPortMalloc(buffer_size + 1); /* +1 means string terminator 0 */
	buffer[buffer_size] = 0;
	buffer[0] = 0;

	/* if previous fragment packet would be found, it is needed to be combined into head of pacData */
	if(previous_fragmented_packet != NULL)
	{
		strcat((char *)buffer, (char *)previous_fragmented_packet->content.string);
		fragmented_packet_list_head = fragmented_packet_list_delete(fragmented_packet_list_head, specified_block_number - 1, FRAGMENTED_PACKET_TYPE_TAIL);
	}
	strcat((char *)buffer, (char *)pacData_string);

	/* if next fragment packet would be found, it is needed to be combined into tail of pacData */
	if(next_fragmented_packet != NULL)
	{
		strcat((char *)buffer, (char *)next_fragmented_packet->content.string);
		fragmented_packet_list_head = fragmented_packet_list_delete(fragmented_packet_list_head, specified_block_number + 1, FRAGMENTED_PACKET_TYPE_HEAD);
	}

    current_index = buffer;
	next_index = (uint8_t*)strstr((char*)current_index, "\n");
	if(next_index != NULL)
	{
	    while(1)
	    {
	        /* extract 1 line*/
	    	sscanf((char*)current_index, "%16s %16s %32s", (char*)arg1, (char*)arg2, (char*)arg3);
			if(!strcmp((char*)arg1, "upprogram"))
			{

			}
			else if(!strcmp((char*)arg1, "sha1"))
			{

			}

			/* exit if no CRLF would be found */
			next_index = (uint8_t*)strstr((char*)current_index, "\n");
			if(next_index == NULL)
			{
				eResult = kOTA_Err_None;
				break;
			}
			else
			{
				next_index += 1; /* +1 means LF */
				current_index = next_index;
			}
	    }
	}
	else
	{
		OTA_LOG_L1("illegal data format [%s].\r\n", current_index);
		eResult = kOTA_Err_OutOfMemory;
    	/* todo: リスト全消去 */
	}
	vPortFree(buffer);
	fragmented_packet_list_print(fragmented_packet_list_head);
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

/***********************************************************************************************************************
* Function Name: fragmented_packet_list_add
* Description  : Add the fragmented packet into the list
* Arguments    : head - current head of list
*                block_number - packet number of block
*                type - fragment direction
*                string - character string
*                length - string length
* Return Value : FRAGMENTED_PACKET_LIST* - new head of list (returns same value as specified head in normally,
*                                                            new head would be returned when head would be specified as NULL.)
***********************************************************************************************************************/
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_add(FRAGMENTED_PACKET_LIST *head, uint32_t block_number, uint8_t type, uint8_t *string, uint8_t length)
{
	FRAGMENTED_PACKET_LIST *tmp, *new;

    new = pvPortMalloc(sizeof(FRAGMENTED_PACKET_LIST));
    new->content.string = pvPortMalloc(length + 1); /* +1 means string terminator 0 */
    new->content.string[0] = 0;
    new->content.string[length] = 0;

    if((new != NULL) && (new->content.string != NULL))
    {
        strcpy((char *)new->content.string, (char *)string);
    	new->content.block_number = block_number;
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
*                block_number - packet number of block
*                type - fragment direction
*
* Return Value : FRAGMENTED_PACKET_LIST* - new head of list (returns same value as specified head in normally,
*                                                            new head would be returned when head would be deleted.
*                                                            NULL would be returned when all list are deleted.)
***********************************************************************************************************************/
static FRAGMENTED_PACKET_LIST *fragmented_packet_list_delete(FRAGMENTED_PACKET_LIST *head, uint32_t block_number, uint8_t type)
{
	FRAGMENTED_PACKET_LIST *tmp = head, *previous = NULL;

	if(head != NULL)
	{
		while(1)
		{
			if((tmp->content.block_number == block_number) && (tmp->content.type == type))
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

static FRAGMENTED_PACKET_LIST *fragmented_packet_list_search(FRAGMENTED_PACKET_LIST *head, uint32_t block_number, uint8_t type)
{
	FRAGMENTED_PACKET_LIST *tmp = head;

	while(1)
	{
		if((tmp->content.block_number == block_number) && (tmp->content.type == type))
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
    OTA_LOG_L1("FRAGMENTED_PACKET_LIST: total_heap_length = [%d], total_list_count = [%d].\r\n", total_heap_length, total_list_count);

	return tmp;
}
