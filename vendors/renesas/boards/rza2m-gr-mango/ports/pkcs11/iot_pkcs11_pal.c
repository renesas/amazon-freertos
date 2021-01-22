/*
 * Amazon FreeRTOS PKCS #11 PAL for RZ/A2M rtk
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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/**
 * @file aws_pkcs11_pal.c
 * @brief Device specific helpers for PKCS11 Interface.
 */

/* Amazon FreeRTOS Includes. */
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"

#include "FreeRTOS.h"
#include "task.h"
#include "mbedtls/sha256.h"

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* Renesas RZ platform includes */
#include "flash_api.h"
#include "r_eeprom_sample.h"

typedef struct _pkcs_data
{
    CK_ATTRIBUTE Label;
    uint32_t local_storage_index;
    uint32_t ulDataSize;
    uint32_t status;
    CK_OBJECT_HANDLE xHandle;
} PKCS_DATA;

#define PKCS_STORAGE_ADDRESS        (0x51000000 - 0x4000)
#define PKCS_STORAGE_ADDRESS_MIRROR (0x51000000 - 0x2000)

#define PKCS_DATA_STATUS_EMPTY 0
#define PKCS_DATA_STATUS_REGISTERED 1
#define PKCS_DATA_STATUS_HIT 2

#define PKCS_HANDLES_LABEL_MAX_LENGTH 40
#define PKCS_OBJECT_HANDLES_NUM 5
#define PKCS_SHA256_LENGTH 32

#define MAX_CHECK_EEPROM_AREA_RETRY_COUNT 30
#define CHECK_EEPROM_DATA_MODE 4

#define PKCS_CONTROL_BLOCK_INITIAL_DATA \
    {\
        /* uint8_t local_storage[((FLASH_DF_BLOCK_SIZE * FLASH_NUM_BLOCKS_DF) / 4) - (sizeof(PKCS_DATA) * PKCS_OBJECT_HANDLES_NUM) - PKCS_SHA256_LENGTH]; */\
        {0x00},\
        /* PKCS_DATA pkcs_data[PKCS_OBJECT_HANDLES_NUM]; */\
        {0x00},\
    },\
    /* uint8_t hash_sha256[PKCS_SHA256_LENGTH]; */\
    {0xea, 0x57, 0x12, 0x9a, 0x18, 0x10, 0x83, 0x80, 0x88, 0x80, 0x40, 0x1f, 0xae, 0xb2, 0xd2, 0xff, 0x1c, 0x14, 0x5e, 0x81, 0x22, 0x6b, 0x9d, 0x93, 0x21, 0xf8, 0x0c, 0xc1, 0xda, 0x29, 0x61, 0x64},


typedef struct _pkcs_storage_control_block_sub
{
    uint8_t local_storage[8192 - (sizeof(PKCS_DATA) * PKCS_OBJECT_HANDLES_NUM) - PKCS_SHA256_LENGTH]; /* 8KB */
    PKCS_DATA pkcs_data[PKCS_OBJECT_HANDLES_NUM];
} PKCS_STORAGE_CONTROL_BLOCK_SUB;

typedef struct _PKCS_CONTROL_BLOCK
{
    PKCS_STORAGE_CONTROL_BLOCK_SUB data;
    uint8_t hash_sha256[PKCS_SHA256_LENGTH];
} PKCS_CONTROL_BLOCK;

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey,
    //eAwsRootCertificate
};

uint8_t object_handle_dictionary[PKCS_OBJECT_HANDLES_NUM][PKCS_HANDLES_LABEL_MAX_LENGTH] =
{
    "",
    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
    pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
    pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
    pkcs11configLABEL_CODE_VERIFICATION_KEY,
    //pkcs11configLABEL_ROOT_CERTIFICATE,
};

static PKCS_CONTROL_BLOCK pkcs_control_block_data_image;        /* 8KB */
static PKCS_CONTROL_BLOCK pkcs_control_block_data;              /* 8KB */
static PKCS_CONTROL_BLOCK pkcs_control_block_data_mirror;       /* 8KB */

//R_ATTRIB_SECTION_CHANGE(C, _PKCS11_STORAGE, 1)
static const PKCS_CONTROL_BLOCK pkcs_control_block_data_org = {PKCS_CONTROL_BLOCK_INITIAL_DATA};
//R_ATTRIB_SECTION_CHANGE_END

//R_ATTRIB_SECTION_CHANGE(C, _PKCS11_STORAGE_MIRROR, 1)
static const PKCS_CONTROL_BLOCK pkcs_control_block_data_mirror_org = {PKCS_CONTROL_BLOCK_INITIAL_DATA};
//R_ATTRIB_SECTION_CHANGE_END

static void update_eeprom_data_from_image(void);
static void update_eeprom_data_mirror_from_image(void);
static int_t check_eeprom_area(uint32_t retry_counter);
static int_t write_octaflash_data(uint32_t addr, void* data, size_t size);

extern CK_RV prvMbedTLS_Initialize( void );

//CK_RV C_Initialize( CK_VOID_PTR pvInitArgs )
CK_RV PKCS11_PAL_Initialize( void )
{
    int_t ret = NO_ERROR;

    memcpy(&pkcs_control_block_data,        (void*)PKCS_STORAGE_ADDRESS,        sizeof(pkcs_control_block_data));
    memcpy(&pkcs_control_block_data_mirror, (void*)PKCS_STORAGE_ADDRESS_MIRROR, sizeof(pkcs_control_block_data_mirror));

    /* data check the hash */
    ret = check_eeprom_area(4);
    if (ERROR_FAILURE == ret) {
        /* copy data from ram to storage*/
        memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data_org, sizeof(pkcs_control_block_data_image));
        update_eeprom_data_from_image();
        memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data_mirror_org, sizeof(pkcs_control_block_data_image));
        update_eeprom_data_mirror_from_image();
    }

    /* check the hash */
    check_eeprom_area(0);

    /* copy data from storage to ram */
    memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data, sizeof(pkcs_control_block_data_image));

    return CKR_OK;
}

/**
* @brief Writes a file to local storage.
*
* Port-specific file write for crytographic information.
*
* @param[in] pxLabel       Label of the object to be saved.
* @param[in] pucData       Data buffer to be written to file
* @param[in] ulDataSize    Size (in bytes) of data to be saved.
*
* @return The file handle of the object that was stored.
*/
/*
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        uint8_t * pucData,
                                        uint32_t ulDataSize )
*/
CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR pxLabel,
                                        CK_BYTE_PTR pucData,
                                        CK_ULONG ulDataSize )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    int i;
    uint8_t hash_sha256[PKCS_SHA256_LENGTH];
    mbedtls_sha256_context ctx;
    int_t ret = NO_ERROR;

    mbedtls_sha256_init(&ctx);

    memcpy(&pkcs_control_block_data,        (void*)PKCS_STORAGE_ADDRESS,        sizeof(pkcs_control_block_data));
    memcpy(&pkcs_control_block_data_mirror, (void*)PKCS_STORAGE_ADDRESS_MIRROR, sizeof(pkcs_control_block_data_mirror));

    /* data check the hash */
    ret = check_eeprom_area(4);
    if (ERROR_FAILURE == ret) {
        /* copy data from ram to storage*/
        memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data_org, sizeof(pkcs_control_block_data_image));
        update_eeprom_data_from_image();
        memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data_mirror_org, sizeof(pkcs_control_block_data_image));
        update_eeprom_data_mirror_from_image();
    }

    /* check the hash */
    check_eeprom_area(0);

    /* copy data from storage to ram */
    memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data, sizeof(pkcs_control_block_data_image));

    /* search specified label value from object_handle_dictionary */
    for (i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
    {
        if (!strcmp((char * )&object_handle_dictionary[i], pxLabel->pValue))
        {
            xHandle = i;
        }
    }

    if (xHandle != eInvalidHandle)
    {

        /* pre-calculate -> total stored data size : pre-calculate phase */
        uint32_t total_stored_data_size = 0;

        for (int i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
        {
            if (pkcs_control_block_data_image.data.pkcs_data[i].status == PKCS_DATA_STATUS_REGISTERED)
            {
                total_stored_data_size += pkcs_control_block_data_image.data.pkcs_data[i].ulDataSize;
            }
        }

        /* remove current xHandle from pkcs_data */
        if (pkcs_control_block_data_image.data.pkcs_data[xHandle].status == PKCS_DATA_STATUS_REGISTERED)
        {

            uint32_t move_target_xHandle = 0, move_target_index = 0;

            uint32_t delete_target_index = pkcs_control_block_data_image.data.pkcs_data[xHandle].local_storage_index;
            uint32_t delete_target_data_size = pkcs_control_block_data_image.data.pkcs_data[xHandle].ulDataSize;

            /* Search move target index and handle  */
            for (int i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
            {

                if ((pkcs_control_block_data_image.data.pkcs_data[i].status == PKCS_DATA_STATUS_REGISTERED)
                        && (pkcs_control_block_data_image.data.pkcs_data[i].local_storage_index == (delete_target_index + delete_target_data_size)))
                {
                    move_target_xHandle = i;
                    move_target_index = pkcs_control_block_data_image.data.pkcs_data[i].local_storage_index;
                    break;
                }

            }

            if (move_target_xHandle != 0)
            {

                /* Move target index to delete target index */
                memmove(
                    (void * )&pkcs_control_block_data_image.data.local_storage[delete_target_index],
                    (void * )&pkcs_control_block_data_image.data.local_storage[move_target_index],
                    (size_t )total_stored_data_size - move_target_index);

                /* Fix index of all moved data  */
                for (int i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
                {

                    if (pkcs_control_block_data_image.data.pkcs_data[i].local_storage_index > delete_target_index)
                    {
                        pkcs_control_block_data_image.data.pkcs_data[i].local_storage_index -= delete_target_data_size;
                    }

                }

            }

            /* Recalculate the end of data storage  */
            total_stored_data_size -= delete_target_data_size;

            pkcs_control_block_data_image.data.pkcs_data[xHandle].local_storage_index = 0;
            pkcs_control_block_data_image.data.pkcs_data[xHandle].ulDataSize = 0;

        }

        pkcs_control_block_data_image.data.pkcs_data[xHandle].Label.type = pxLabel->type;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].Label.ulValueLen = pxLabel->ulValueLen;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].local_storage_index = total_stored_data_size;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].ulDataSize = ulDataSize;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].status = PKCS_DATA_STATUS_REGISTERED;
        pkcs_control_block_data_image.data.pkcs_data[xHandle].xHandle = xHandle;
        memcpy(&pkcs_control_block_data_image.data.local_storage[total_stored_data_size], pucData, ulDataSize);

        /* update the hash */
        mbedtls_sha256_starts_ret(&ctx, 0); /* 0 means SHA256 context */
        mbedtls_sha256_update_ret(&ctx, (unsigned char *)&pkcs_control_block_data_image.data, sizeof(pkcs_control_block_data.data));
        mbedtls_sha256_finish_ret(&ctx, hash_sha256);
        memcpy(pkcs_control_block_data_image.hash_sha256, hash_sha256, sizeof(hash_sha256));

        /* update data from ram to storage */
        update_eeprom_data_from_image();
        update_eeprom_data_mirror_from_image();
    }

    Sample_RIIC_Eeprom_UnInit();

    return xHandle;

}

/**
* @brief Translates a PKCS #11 label into an object handle.
*
* Port-specific object handle retrieval.
*
*
* @param[in] pLabel         Pointer to the label of the object
*                           who's handle should be found.
* @param[in] usLength       The length of the label, in bytes.
*
* @return The object handle if operation was successful.
* Returns eInvalidHandle if unsuccessful.
*/
/*
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
                                        uint8_t usLength )
*/
CK_OBJECT_HANDLE PKCS11_PAL_FindObject( CK_BYTE_PTR pxLabel,
                                        CK_ULONG usLength )
{
    /* Avoid compiler warnings about unused variables. */
    UNUSED_VARIABLE(usLength);

    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    int i;

    for(i = 1; i < PKCS_OBJECT_HANDLES_NUM; i++)
    {
        if(!strcmp((char *)&object_handle_dictionary[i], (char *)pxLabel))

        {
            if(pkcs_control_block_data_image.data.pkcs_data[i].status == PKCS_DATA_STATUS_REGISTERED)
            {
                xHandle = i;
            }
        }
    }

    return xHandle;
}

/**
* @brief Gets the value of an object in storage, by handle.
*
* Port-specific file access for cryptographic information.
*
* This call dynamically allocates the buffer which object value
* data is copied into.  PKCS11_PAL_GetObjectValueCleanup()
* should be called after each use to free the dynamically allocated
* buffer.
*
* @sa PKCS11_PAL_GetObjectValueCleanup
*
* @param[in] pcFileName    The name of the file to be read.
* @param[out] ppucData     Pointer to buffer for file data.
* @param[out] pulDataSize  Size (in bytes) of data located in file.
* @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
*                          or exportable (CK_FALSE)
*
* @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
* no such object handle was found, CKR_DEVICE_MEMORY if memory for
* buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
* error.
*/
/*
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                 uint8_t ** ppucData,
                                 uint32_t * pulDataSize,
                                 CK_BBOOL * pIsPrivate )
*/
CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
                                      CK_BYTE_PTR * ppucData,
                                      CK_ULONG_PTR pulDataSize,
                                      CK_BBOOL * pIsPrivate )
{
    CK_RV xReturn = CKR_FUNCTION_FAILED;
    CK_OBJECT_HANDLE xHandleStorage = xHandle;


    if (xHandleStorage == eAwsDevicePublicKey)
    {
        xHandleStorage = eAwsDevicePrivateKey;
    }


    if (xHandle != eInvalidHandle)
    {
        *ppucData = &pkcs_control_block_data_image.data.local_storage[pkcs_control_block_data_image.data.pkcs_data[xHandleStorage].local_storage_index];
        *pulDataSize = pkcs_control_block_data_image.data.pkcs_data[xHandleStorage].ulDataSize;

        if (xHandle == eAwsDevicePrivateKey)
        {
            *pIsPrivate = CK_TRUE;
        }
        else
        {
            *pIsPrivate = CK_FALSE;
        }
        xReturn = CKR_OK;
    }

    return xReturn;

}


/**
* @brief Cleanup after PKCS11_GetObjectValue().
*
* @param[in] pucData       The buffer to free.
*                          (*ppucData from PKCS11_PAL_GetObjectValue())
* @param[in] ulDataSize    The length of the buffer to free.
*                          (*pulDataSize from PKCS11_PAL_GetObjectValue())
*/
/*
void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucData,
                                       uint32_t ulDataSize )
*/
void PKCS11_PAL_GetObjectValueCleanup( CK_BYTE_PTR pucData,
                                       CK_ULONG ulDataSize )
{
    /* Avoid compiler warnings about unused variables. */
    UNUSED_VARIABLE(pucData);
    UNUSED_VARIABLE(ulDataSize);

    /* todo: nothing to do in now. Now, pkcs_data exists as static. I will fix this function when I will port this to heap memory. (Renesas/Ishiguro) */
}

static void update_eeprom_data_from_image(void)
{
    uint32_t required_eeprom_block_num;
    int_t flash_error_code = NO_ERROR;

    configPRINTF(("write OCTA Flash(main)...\r\n"));
    
    flash_error_code = write_octaflash_data(PKCS_STORAGE_ADDRESS, &pkcs_control_block_data_image, sizeof(pkcs_control_block_data_image));
    if(NO_ERROR == flash_error_code)
    {
        memcpy((void *)&pkcs_control_block_data, (void*)PKCS_STORAGE_ADDRESS, sizeof(pkcs_control_block_data));
        configPRINTF(("OK\r\n"));
    }
    else
    {
        configPRINTF(("Failed\r\n"));
        return;
    }

    return;
}

static void update_eeprom_data_mirror_from_image(void)
{
    uint32_t required_eeprom_block_num;
    int_t flash_error_code = NO_ERROR;

    configPRINTF(("write OCTA Flash(mirror)...\r\n"));
    flash_error_code = write_octaflash_data(PKCS_STORAGE_ADDRESS_MIRROR, &pkcs_control_block_data_image, sizeof(pkcs_control_block_data_image));
    if(NO_ERROR == flash_error_code)
    {
        memcpy((void *)&pkcs_control_block_data_mirror, PKCS_STORAGE_ADDRESS_MIRROR, sizeof(pkcs_control_block_data_mirror));
        configPRINTF(("OK\r\n"));
    }
    else
    {
        configPRINTF(("Failed\r\n"));
        configPRINTF(("Sample_RIIC_Eeprom_Write() returns error code = %d.\r\n", flash_error_code));
        return;
    }

    if(!memcmp(&pkcs_control_block_data, &pkcs_control_block_data_mirror, sizeof(PKCS_CONTROL_BLOCK)))
    {
        configPRINTF(("OCTA Flash setting OK.\r\n"));
    }
    else
    {
        configPRINTF(("OCTA Flash setting Failed.\r\n"));
        return;
    }
    return;
}

static int_t check_eeprom_area(uint32_t retry_counter)
{
    uint8_t hash_sha256[PKCS_SHA256_LENGTH];
    mbedtls_sha256_context ctx;
    int_t ret = NO_ERROR;

    mbedtls_sha256_init(&ctx);

    if(retry_counter)
    {
        configPRINTF(("recover retry count = %d.\r\n", retry_counter));
        if(retry_counter == MAX_CHECK_EEPROM_AREA_RETRY_COUNT)
        {
            configPRINTF(("retry over the limit.\r\n"));
            while(1);
        }
    }
    configPRINTF(("OCTA Flash(main) hash check...\r\n"));
    mbedtls_sha256_starts_ret(&ctx, 0); /* 0 means SHA256 context */
    mbedtls_sha256_update_ret(&ctx, (unsigned char *)&pkcs_control_block_data.data, sizeof(pkcs_control_block_data.data));
    mbedtls_sha256_finish_ret(&ctx, hash_sha256);
    if(!memcmp(pkcs_control_block_data.hash_sha256, hash_sha256, sizeof(hash_sha256)))
    {
        configPRINTF(("OK\r\n"));
        configPRINTF(("OCTA Flash(mirror) hash check...\r\n"));
        mbedtls_sha256_starts_ret(&ctx, 0); /* 0 means SHA256 context */
        mbedtls_sha256_update_ret(&ctx, (unsigned char *)&pkcs_control_block_data_mirror.data, sizeof(pkcs_control_block_data_mirror.data));
        mbedtls_sha256_finish_ret(&ctx, hash_sha256);
        if(!memcmp(pkcs_control_block_data_mirror.hash_sha256, hash_sha256, sizeof(hash_sha256)))
        {
            configPRINTF(("OK\r\n"));
        }
        else
        {
            configPRINTF(("Failed\r\n"));
            configPRINTF(("recover mirror from main.\r\n"));
            memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data, sizeof(pkcs_control_block_data));
            update_eeprom_data_mirror_from_image();
            if (retry_counter == CHECK_EEPROM_DATA_MODE) {
                retry_counter = 0;
            }
            ret = check_eeprom_area(retry_counter + 1);
        }
    }
    else
    {
        configPRINTF(("Failed\r\n"));
        configPRINTF(("OCTA Flash(mirror) hash check...\r\n"));
        mbedtls_sha256_starts_ret(&ctx, 0); /* 0 means SHA256 context */
        mbedtls_sha256_update_ret(&ctx, (unsigned char *)&pkcs_control_block_data_mirror.data, sizeof(pkcs_control_block_data_mirror.data));
        mbedtls_sha256_finish_ret(&ctx, hash_sha256);
        if(!memcmp(pkcs_control_block_data_mirror.hash_sha256, hash_sha256, sizeof(hash_sha256)))
        {
            configPRINTF(("OK\r\n"));
            configPRINTF(("recover main from mirror.\r\n"));
            memcpy(&pkcs_control_block_data_image, (void *)&pkcs_control_block_data_mirror, sizeof(pkcs_control_block_data_mirror));
            update_eeprom_data_from_image();
            check_eeprom_area(retry_counter + 1);
        }
        else
        {
            configPRINTF(("Failed\r\n"));
            if (retry_counter == CHECK_EEPROM_DATA_MODE) {
                return ERROR_FAILURE;
            }
            while(1)
            {
                vTaskDelay(10000);
                configPRINTF(("------------------------------------------------\r\n"));
                configPRINTF(("OCTA Flash is completely broken.\r\n"));
//                configPRINTF(("Please erase all code flash.\r\n"));
//                configPRINTF(("And, write initial firmware using debugger/rom writer.\r\n"));
                configPRINTF(("------------------------------------------------\r\n"));
            }
        }
    }
    return ret;
}

static int_t write_octaflash_data(uint32_t addr, void* data, size_t size)
{
    uint32_t sector = addr;
    /* erase */
    for (sector = addr; sector < addr + size; sector += flash_get_sector_size(NULL,sector))
    {
        if (flash_erase_sector(NULL,sector) < 0)
        {
            return ERROR_FAILURE;
        }
    }
    /* write */
    if (flash_program_page(NULL, addr, data, size) < 0)
    {
        return ERROR_FAILURE;
    }
    return NO_ERROR;
}
