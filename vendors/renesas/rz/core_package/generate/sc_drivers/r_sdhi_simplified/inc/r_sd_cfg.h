/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/*******************************************************************************
* System Name  : SDHI Driver Sample Program
* File Name    : r_sd_cfg.h
* Version      : 1.30
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : SD Memory card driver configuration
* Operation    :
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 29.05.2019 1.20     Correspond to internal coding rules
*         : 12.11.2019 1.30     Support for SDIO
*         : 31.03.2020 1.50     Support high speed for SDIO
******************************************************************************/
#ifndef R_SD_CFG_H
#define R_SD_CFG_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
/* ------------------------------------------------------
  Set SDHI Base Address
--------------------------------------------------------*/
#define SD_CFG_IP0_BASE      (SD_SCC_IP0_BASE_ADDR)      /* Set the base address of SCC ch0. */
#define SD_CFG_IP1_BASE      (SD_SCC_IP1_BASE_ADDR)      /* Set the base address of SCC ch1. */

/* ------------------------------------------------------
  Set the method of check SD Status
--------------------------------------------------------*/
#define SD_CFG_HWINT
/* #define SD_CFG_POLL */

/* ------------------------------------------------------
  Set the method of data transfer
--------------------------------------------------------*/
#define SD_CFG_TRNS_DMA
/* #define SD_CFG_TRNS_SW */

/* ------------------------------------------------------
  Set the card type to support
--------------------------------------------------------*/
/* #define SD_CFG_MEM */
#define SD_CFG_IO

#ifdef SD_CFG_IO
/* ------------------------------------------------------
  Set the speed to support
--------------------------------------------------------*/
/* #define SD_CFG_DS */
#define SD_CFG_HS

#endif //SD_CFG_IO

/* ------------------------------------------------------
  Set the version to support
--------------------------------------------------------*/
/* #define SD_CFG_VER1X */            /* Version 1.1 */
#define SD_CFG_VER2X            /* Version 2.x */

/* ------------------------------------------------------
  Set the method to detect card
--------------------------------------------------------*/
#define SD_CFG_CD_INT

#ifdef SD_CFG_CD_INT
    #ifndef SD_CFG_HWINT
        #error    please define SD_CFG_HWINT
    #endif
#endif

    /* the SD bus width */
    #define SD_CFG_DRIVER_MODE_BUS_WIDTH    (SD_MODE_4BIT)

#if    defined(SD_CFG_HWINT)
    /* the method of check SD Status */
    #define SD_CFG_DRIVER_MODE_METHOD       (SD_MODE_HWINT)
#else
    /* the method of check SD Status */
    #define SD_CFG_DRIVER_MODE_METHOD       (SD_MODE_POLL)
#endif

#if    defined(SD_CFG_TRNS_DMA)
    /* the method of data transfer */
    #define SD_CFG_DRIVER_MODE_TRANSFER     (SD_MODE_DMA)
#else
    /* the method of data transfer */
    #define SD_CFG_DRIVER_MODE_TRANSFER     (SD_MODE_SW)
#endif

#if    defined(SD_CFG_MEM)
    /* the card type to support */
    #define SD_CFG_DRIVER_MODE_CARD_TYPE    (SD_MODE_MEM)
#else
    /* the card type to support */
    #define SD_CFG_DRIVER_MODE_CARD_TYPE    (SD_MODE_IO)
#endif

#ifdef SD_CFG_IO
 #if    defined(SD_CFG_HS)
    /* the speed to support */
    #define SD_CFG_DRIVER_MODE_SPEED        (SD_MODE_HS)
 #else
    /* the speed to support */
    #define SD_CFG_DRIVER_MODE_SPEED        (SD_MODE_DS)
 #endif

#else
    /* the speed to support */
    #define SD_CFG_DRIVER_MODE_SPEED        (SD_MODE_DS)
#endif //SD_CFG_IO

#if    defined(SD_CFG_VER2X)
    /* the version to support */
    #define SD_CFG_DRIVER_MODE_VER          (SD_MODE_VER2X)
#else
    /* the version to support */
    #define SD_CFG_DRIVER_MODE_VER          (SD_MODE_VER1X)
#endif

/* Set driver mode configration */
#define SD_CFG_DRIVER_MODE                  (SD_CFG_DRIVER_MODE_BUS_WIDTH | \
                                            SD_CFG_DRIVER_MODE_METHOD     | \
                                            SD_CFG_DRIVER_MODE_TRANSFER   | \
                                            SD_CFG_DRIVER_MODE_CARD_TYPE  | \
                                            SD_CFG_DRIVER_MODE_SPEED      | \
                                            SD_CFG_DRIVER_MODE_VER)

/* ==== end of the setting ==== */



/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

#endif /* R_SD_CFG_H    */

/* End of File */
