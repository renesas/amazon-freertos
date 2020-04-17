/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only
 * intended for use with Renesas products. No other uses are authorized. This
 * software is owned by Renesas Electronics Corporation and is protected under
 * all applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
 * LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
 * TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
 * ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
 * FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
 * ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software
 * and to discontinue the availability of this software. By using this software,
 * you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_cbuffer.c
 * Version      : 1.03
 * Description  : Circular Buffering Routines
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 04.02.2010 1.00    First Release
 *              : 10.06.2010 1.01    Updated type definitions
 *              : 07.03.2011 1.02    Added Memory Type Parameter
 *              : 02.05.2019 1.03    Added buffer pointer validation
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "r_cbuffer.h"
#include "r_typedefs.h"

/* OS abstraction specific API header */
#include "r_os_abstraction_api.h"

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/* Define the structure of a buffer */
typedef struct _cbuff_t
{
    /* a pointer to the base of the memory */
    uint8_t *p_base;

    /* The input index */
    size_t  input_index;

    /* The output index */
    size_t  output_index;

    /* The length of the circular buffer */
    size_t  length;
} st_cbuff_t;

/* Define the structure of a buffer including alignment */
typedef struct _cobject_t
{
    st_cbuff_t cbuffer;
    uint32_t   ulalign;
} st_cobject_t;

typedef struct _cobject_t *st_pcobject_t;

/**********************************************************************************************************************
 Function Prototypes
 *********************************************************************************************************************/

static void cbAdjustOutIndex (st_pcbuff_t pcBuffer, size_t length);
static void cbAdjustInIndex (st_pcbuff_t pcBuffer, size_t length);

/**********************************************************************************************************************
 Exported global variables and functions (to be accessed by other files)
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbCreate
 * Description  : Function to create a circular buffer
 * Arguments    : IN  stBufferSize - The size of the circular buffer required
 * Return Value : Pointer to the circular buffer or NULL on failure
 *********************************************************************************************************************/
st_pcbuff_t cbCreate (size_t stBufferSize)
{
    st_pcbuff_t cbuffer;

    /* prevent allocation of zero size buffer or buffer that would overflow max capacity of int32_t */
    if ((0 == stBufferSize) || (0 != (stBufferSize & 0x80000000ul)))
    {
        return NULL;
    }

    /* Allocate the buffer with enough memory for alignment and extra byte for signalling */
    cbuffer = (st_pcbuff_t) R_OS_Malloc(stBufferSize + sizeof(st_cobject_t) + sizeof(uint8_t), R_MEMORY_REGION_DEFAULT);

    if (cbuffer)
    {
        /* Initialise the buffer variables */
        cbuffer->p_base = (uint8_t *) &((st_pcobject_t) cbuffer)->ulalign;
        cbuffer->length = (size_t) (stBufferSize + sizeof(uint8_t));
        cbuffer->input_index = 0;
        cbuffer->output_index = 0;
    }

    return cbuffer;
}
/**********************************************************************************************************************
 End of function cbCreate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbDestroy
 * Description  : Function to destroy a circular buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer to destroy
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbDestroy (st_pcbuff_t pcBuffer)
{
    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    R_OS_Free((void **) &pcBuffer);

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 End of function cbDestroy
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbPut
 * Description:   Put a uint8_t in the Buffer
 * Arguments:     IN  pcBuffer - pointer to the circular buffer
 *                IN  byData - The data to put
 * Return value:  true if the byte was entered into the buffer
 *                false if the byte was not entered because the buffer was full
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbPut (st_pcbuff_t pcBuffer, uint8_t byData)
{
    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    size_t l_input_index = (pcBuffer->input_index + sizeof(uint8_t));

    /* Check for top of Buffer */
    if (l_input_index >= pcBuffer->length)
    {
        l_input_index = 0;
    }

    /* If there is room */
    if (l_input_index != pcBuffer->output_index)
    {
        /* Put the byte in the buffer */
        *(pcBuffer->p_base + pcBuffer->input_index) = byData;

        /* Update the index */
        pcBuffer->input_index = l_input_index;
        return true;
    }

    return false;
}
/**********************************************************************************************************************
 End of function cbPut
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbGet
 * Description:   Get a byte from the Buffer
 * Arguments:     IN  pcBuffer - pointer to the circular buffer
 *                OUT pbyData - pointer to the destination byte of data
 * Return value:  true if a byte was retrieved
 *                false if the buffer was empty
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbGet (st_pcbuff_t pcBuffer, uint8_t *pbyData)
{
    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    /* Check that the buffer is not empty */
    if (pcBuffer->input_index != pcBuffer->output_index)
    {
        size_t l_output_index;

        /* Get Data from Buffer, bump in index */
        l_output_index = pcBuffer->output_index;
        *pbyData = *(pcBuffer->p_base + l_output_index);
        l_output_index++;

        /* Check for top of Buffer */
        if (l_output_index >= pcBuffer->length)
        {
            l_output_index = 0;
        }

        /* Update the index */
        pcBuffer->output_index = l_output_index;
        return true;
    }

    return false;
}
/**********************************************************************************************************************
 End of function cbGet
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbUsed
 * Description:   Function to return the number of bytes in the buffer
 * Arguments:     IN  pcBuffer - Pointer to the buffer
 * Return value:  The number of bytes in the buffer
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int32_t cbUsed (st_pcbuff_t pcBuffer)
{
    size_t l_input_index;
    size_t l_output_index;
    size_t used;

    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    l_input_index = pcBuffer->input_index;
    l_output_index = pcBuffer->output_index;

    if (l_input_index < l_output_index)
    {
        used = (pcBuffer->length - (l_output_index - l_input_index));
    }
    else
    {
        used = (l_input_index - l_output_index);
    }

    return (int32_t) used;
}
/**********************************************************************************************************************
 End of function  cbUsed
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbFree
 * Description  : Function to return the number of free space in the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : The number of bytes that can be put in the buffer
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int32_t cbFree (st_pcbuff_t pcBuffer)
{
    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    return (int32_t) (pcBuffer->length - ((size_t) cbUsed(pcBuffer) + sizeof(uint8_t)));
}
/**********************************************************************************************************************
 End of function cbFree
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbFull
 * Description  : Function to return true when the buffer is full
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : true if the buffer is full
 *                false if it is not full
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbFull (st_pcbuff_t pcBuffer)
{
    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    size_t l_input_index = (pcBuffer->input_index + sizeof(uint8_t));

    /* Check for top of Buffer */
    if (l_input_index >= pcBuffer->length)
    {
        l_input_index = 0;
    }

    /* Now test for full */
    if (l_input_index == pcBuffer->output_index)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**********************************************************************************************************************
 End of function cbFull
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbClear
 * Description  : Function to clear the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer to clear
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbClear (st_pcbuff_t pcBuffer)
{
    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    pcBuffer->input_index = pcBuffer->output_index = 0;

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 End of function  cbClear
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbGetPacket
 * Description  : Function to get data from the buffer but not check it out
 *                cbUsed should be called before calls to this function to
 *                ensure that there is sufficient data in the buffer
 * Arguments    : IN  pcBuffer - pointer to the IO buffer to get the data from
 *                IN  stPacketLength - the length of data to read from the
 *                                     buffer
 *                IN  pDest - the destination buffer address
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbGetPacket (st_pcbuff_t pcBuffer, size_t stPacketLength, void *pDest)
{
    uint8_t *p_data;
    uint8_t *p_temp;
    size_t block_length;

    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    /* Get the pointer to the data */
    p_data = (pcBuffer->p_base + pcBuffer->output_index);

    /* Get the length of the linear chunk */
    block_length = (size_t) cbLinOut(pcBuffer);

    /* Check to see if this can be done in one hit */
    if (block_length >= stPacketLength)
    {
        /* All in one block */
        memcpy(pDest, p_data, stPacketLength);
    }
    else
    {
        /* Splits into two blocks */
        memcpy(pDest, p_data, block_length);
        p_data = pcBuffer->p_base;

        /* re-calculate the address */
        p_temp = ((uint8_t *) pDest) + block_length;

        /* calculate size of second block */
        block_length = (stPacketLength - block_length);
        memcpy(p_temp, p_data, block_length);
    }

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 End of function cbGetPacket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbCheckOut
 * Description  : Function to check the data out of the data following a call to
 *                cbGetPacket
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 *                IN  stPacketLength - the size of the data to check out
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbCheckOut (st_pcbuff_t pcBuffer, size_t stPacketLength)
{
    size_t block_length;

    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    /* Get the length of the linear chunk */
    block_length = (size_t) cbLinOut(pcBuffer);

    /* Check to see if this can be done in one hit */
    if (block_length >= stPacketLength)
    {
        /* All in one block */
        cbAdjustOutIndex(pcBuffer, stPacketLength);
    }
    else
    {
        /* Two blocks */
        cbAdjustOutIndex(pcBuffer, block_length);

        /* Calculate size of second block */
        block_length = (stPacketLength - block_length);
        cbAdjustOutIndex(pcBuffer, block_length);
    }

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 End of function cbCheckOut
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbPutPacket
 * Description  : Function to put a packet of data into the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 *                IN  stPacketLength - The length of the packet to put
 *                IN  pSrc - Pointer to the data to put
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbPutPacket (st_pcbuff_t pcBuffer, size_t stPacketLength, void *pSrc)
{
    uint8_t *p_data;
    uint8_t *p_temp;
    size_t block_length;

    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    /* How much linear space is there */
    block_length = (size_t) cbLinIn(pcBuffer);
    p_data = (pcBuffer->p_base + pcBuffer->input_index);

    /* Check for one or two blocks */
    if (block_length >= stPacketLength)
    {
        /* All in one block */
        memcpy(p_data, pSrc, stPacketLength);
    }
    else
    {
        /* Splits into two blocks */
        memcpy(p_data, pSrc, block_length);
        p_data = pcBuffer->p_base;

        /* Re-calculate the address */
        p_temp = ((uint8_t *) pSrc) + block_length;

        /* Calculate size of second block */
        block_length = (stPacketLength - block_length);
        memcpy(p_data, p_temp, block_length);
    }

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 End of function cbPutPacket
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbCheckIn
 * Description  : Function to check a packet into the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 *                IN  stPacketLength - The length of data to check in
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int_t cbCheckIn (st_pcbuff_t pcBuffer, size_t stPacketLength)
{
    size_t block_length;

    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    /* Find out the free linear space there is in the buffer */
    block_length = (size_t) cbLinIn(pcBuffer);

    /* Test to see if it fits or if it splits */
    if (block_length >= stPacketLength)
    {
        /* All in one block */
        cbAdjustInIndex(pcBuffer, stPacketLength);
    }
    else
    {
        /* Two blocks */
        cbAdjustInIndex(pcBuffer, block_length);

        /* Calculate size of second block */
        block_length = (stPacketLength - block_length);
        cbAdjustInIndex(pcBuffer, block_length);
    }

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 End of function cbCheckIn
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbLinOut
 * Description  : Function to calculate how much can be delivered in one hit
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : The number of bytes until the end of the data or the buffer
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int32_t cbLinOut (st_pcbuff_t pcBuffer)
{
    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    if (pcBuffer->input_index == pcBuffer->output_index)
    {
        /* When empty nothing to take out */
        return 0;
    }
    else
    {
        size_t l_input_index;
        size_t l_output_index;

        /* Snap shot of Buffer controls as  ISR may be updating in background */
        l_input_index = pcBuffer->input_index;
        l_output_index = pcBuffer->output_index;

        /* Test for buffer wrap */
        if (l_output_index > l_input_index)
        {
            /* Until the end (wrapped) */
            return (int32_t) (pcBuffer->length - l_output_index);
        }
        else
        {
            /* Until the in index */
            return (int32_t) (l_input_index - l_output_index);
        }
    }
}
/**********************************************************************************************************************
 End of function cbLinOut
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbLinIn
 * Description  : Function to return how much data can be placed in the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : The length of data until the end of the data or the buffer
 *                DRV_ERROR if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
int32_t cbLinIn (st_pcbuff_t pcBuffer)
{
    if (NULL == pcBuffer)
    {
        return DRV_ERROR;
    }

    if (cbFull(pcBuffer) == false)
    {
        size_t l_input_index;
        size_t l_output_index;

        /* Snap shot of Buffer controls as */
        /* ISR may be updating in background */
        l_input_index = pcBuffer->input_index;
        l_output_index = pcBuffer->output_index;

        /* Test for buffer wrap */
        if (l_input_index < l_output_index)
        {
            /* Lin in is between out and in */
            return (int32_t) ((l_output_index - sizeof(uint8_t)) - l_input_index);
        }
        else if (l_output_index)
        {
            /* Lin in until top but room at bottom */
            return (int32_t) (pcBuffer->length - l_input_index);
        }
        else
        {
            /* lin in to the end - 1 */
            return (int32_t) ((pcBuffer->length - sizeof(uint8_t)) - l_input_index);
        }
    }

    return 0;
}
/**********************************************************************************************************************
 End of function cbLinIn
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbInPointer
 * Description  : Function to return a pointer to the input of the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : Pointer to the buffer input
 *                NULL if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
void *cbInPointer (st_pcbuff_t pcBuffer)
{
    if (NULL == pcBuffer)
    {
        return NULL;
    }

    return (pcBuffer->p_base + pcBuffer->input_index);
}
/**********************************************************************************************************************
 End of function cbInPointer
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbOutPointer
 * Description  : Function to return a pointer to the output of the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : Pointer to the buffer input
 *                NULL if the pcBuffer pointer is invalid
 *********************************************************************************************************************/
void *cbOutPointer (st_pcbuff_t pcBuffer)
{
    if (NULL == pcBuffer)
    {
        return NULL;
    }

    return (pcBuffer->p_base + pcBuffer->output_index);
}
/******************************************************************************
 End of function cbOutPointer
 ******************************************************************************/

/**********************************************************************************************************************
 Static Functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbAdjustOutIndex
 * Description  : Function to adjust the out index of the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 *                IN  length - The length of data to adjust for
 * Return Value : none
 *********************************************************************************************************************/
static void cbAdjustOutIndex (st_pcbuff_t pcBuffer, size_t length)
{
    size_t l_output_index = (pcBuffer->output_index + length);

    /* Check for top of Buffer */
    if (l_output_index >= pcBuffer->length)
    {
        l_output_index = (l_output_index - pcBuffer->length);
    }

    pcBuffer->output_index = l_output_index;
}
/**********************************************************************************************************************
 End of function cbAdjustOutIndex
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: cbAdjustInIndex
 * Description  : Function to adjust the in index of the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 *                IN  length - The length of data to adjust for
 * Return Value : none
 *********************************************************************************************************************/
static void cbAdjustInIndex (st_pcbuff_t pcBuffer, size_t length)
{
    size_t l_input_index = (pcBuffer->input_index + length);

    /* Check for top of Buffer */
    if (l_input_index >= pcBuffer->length)
    {
        l_input_index = (l_input_index - pcBuffer->length);
    }

    pcBuffer->input_index = l_input_index;
}
/**********************************************************************************************************************
 End of function cbAdjustInIndex
 ******************************************************************************/

/**********************************************************************************************************************
 End Of File
 *********************************************************************************************************************/
