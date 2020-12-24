/******************************************************************************
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
 * Portions copyright (C) 2011  Swarm Systems Limited. All rights reserved.
 ******************************************************************************/
/*******************************************************************************
 * File Name    : r_cbuffer.h
 * Version      : 1.03
 * Description  : Circular Buffering Routines
 ******************************************************************************/
/*******************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 04.02.2010 1.00    First Release
 *              : 10.06.2010 1.01    Updated type definitions
 *              : 07.03.2011 1.02    Added Memory Type Parameter
 *              : 02.05.2019 1.03    Added buffer pointer validation
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include "r_typedefs.h"
#include "malloc.h"

#ifndef CBUFFER_H
#define CBUFFER_H

/******************************************************************************
Typedef definitions
******************************************************************************/

/* Hide the structure of the circular buffer to prevent access to the members
   directly. This is called encapsulation. */
typedef struct _cbuff_t *st_pcbuff_t;

/******************************************************************************
Function Prototypes
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


/** Version Number of API. */
    #define STDIO_CBUFFER_RZ_LLD_DRV_NAME ("LLD EBK_RZA2M CBUFFER")
/** Major Version Number of API. */
    #define STDIO_CBUFFER_RZ_LLD_VERSION_MAJOR      (1)
/** Minor Version Number of API. */
    #define STDIO_CBUFFER_RZ_LLD_VERSION_MINOR      (1)
/** Minor Version Number of API. */
    #define STDIO_CBUFFER_RZ_LLD_BUILD_NUM          (0)
/** Unique ID. */
    #define STDIO_CBUFFER_RZ_LLD_UID                (211)



/******************************************************************************
 * Function Name: cbCreate
 * Description  : Function to create a circular buffer
 * Arguments    : IN  stBufferSize - The size of the circular buffer required
 * Return Value : Pointer to the circular buffer or NULL on failure
 ******************************************************************************/

extern  st_pcbuff_t cbCreate(size_t stBufferSize);

/******************************************************************************
 * Function Name: cbDestroy
 * Description  : Function to destroy a circular buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer to destroy
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int_t cbDestroy(st_pcbuff_t pcBuffer);

/******************************************************************************
 * Function Name: cbPut
 * Description:   Put a uint8_t in the Buffer
 * Arguments:     IN  pcBuffer - pointer to the circular buffer
 *                IN  byData - The data to put
 * Return value:  true if the byte was entered into the buffer
 *                false if the byte was not entered because the buffer was full
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int_t cbPut(st_pcbuff_t pcBuffer, uint8_t byData);

/******************************************************************************
 * Function Name: cbGet
 * Description:   Get a byte from the Buffer
 * Arguments:     IN  pcBuffer - pointer to the circular buffer
 *                OUT pbyData - pointer to the destination byte of data
 * Return value:  true if a byte was retrieved
 *                false if the buffer was empty
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int_t cbGet(st_pcbuff_t pcBuffer, uint8_t *pbyData);

/******************************************************************************
 * Function Name: cbUsed
 * Description:   Function to return the number of bytes in the buffer
 * Arguments:     IN  pcBuffer - Pointer to the buffer
 * Return value:  The number of bytes in the buffer
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int32_t cbUsed(st_pcbuff_t pcBuffer);

/******************************************************************************
 * Function Name: cbFree
 * Description  : Function to return the number of free space in the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : The number of bytes that can be put in the buffer
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int32_t cbFree(st_pcbuff_t pcBuffer);

/******************************************************************************
 * Function Name: cbFull
 * Description  : Function to return true when the buffer is full
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : true if the buffer is full
 *                false if it is not full
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int_t cbFull(st_pcbuff_t pcBuffer);

/*****************************************************************************
 * Function Name: cbClear
 * Description  : Function to clear the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer to clear
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int_t cbClear(st_pcbuff_t pcBuffer);

/******************************************************************************
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
 ******************************************************************************/

extern  int_t cbGetPacket(st_pcbuff_t pcBuffer, size_t stPacketLength, void *pDest);

/******************************************************************************
 * Function Name: cbCheckOut
 * Description  : Function to check the data out of the data following a call to
 *                cbGetPacket
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 *                IN  stPacketLength - the size of the data to check out
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int_t cbCheckOut(st_pcbuff_t pcBuffer, size_t stPacketLength);

/******************************************************************************
 * Function Name: cbPutPacket
 * Description  : Function to put a packet of data into the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 *                IN  stPacketLength - The length of the packet to put
 *                IN  pSrc - Pointer to the data to put
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int_t cbPutPacket(st_pcbuff_t pcBuffer, size_t stPacketLength, void *pSrc);

/******************************************************************************
 * Function Name: cbCheckIn
 * Description  : Function to check a packet into the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 *                IN  stPacketLength - The length of data to check in
 * Return Value : DRV_SUCCESS on success
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int_t cbCheckIn(st_pcbuff_t pcBuffer, size_t stPacketLength);

/******************************************************************************
 * Function Name: cbLinOut
 * Description  : Function to calculate how much can be delivered in one hit
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : The number of bytes until the end of the data or the buffer
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int32_t cbLinOut(st_pcbuff_t pcBuffer);

/******************************************************************************
 * Function Name: cbLinIn
 * Description  : Function to return how much data can be placed in the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : The length of data until the end of the data or the buffer
 *                DRV_ERROR if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  int32_t cbLinIn(st_pcbuff_t pcBuffer);

/******************************************************************************
 * Function Name: cbInPointer
 * Description  : Function to return a pointer to the input of the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : Pointer to the buffer input
 *                NULL if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  void *cbInPointer(st_pcbuff_t pcBuffer);

/******************************************************************************
 * Function Name: cbOutPointer
 * Description  : Function to return a pointer to the output of the buffer
 * Arguments    : IN  pcBuffer - Pointer to the buffer
 * Return Value : Pointer to the buffer input
 *                NULL if the pcBuffer pointer is invalid
 ******************************************************************************/

extern  void *cbOutPointer(st_pcbuff_t pcBuffer);

#ifdef __cplusplus
}
#endif

#endif /* CBUFFER_H_INCLUDED */

/******************************************************************************
End  Of File CBUFFER_H
******************************************************************************/
