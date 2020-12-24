/*******************************************************************************
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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
 * @headerfile     r_ostm_drv_api.h
 * @brief          OSTM Driver API file - include for use of OSTM driver.
 * @version        1.00
 * @date           16.05.2018
 *******************************************************************************/
/*******************************************************************************
 * History       : DD.MM.YYYY Version Description
 * 				 : dd.mm.2018 1.00    Release
 *******************************************************************************/
#ifndef R_OSTM_DRV_API_H_
#define R_OSTM_DRV_API_H_

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_devlink_wrapper.h"
#include "r_ostm_drv_link.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/

#define STDIO_OSTM_HLD_DRV_NAME        ("HLD OSTM")

/** Major Version Number of API.
 * Updated by product owner */
#define STDIO_OSTM_HLD_VERSION_MAJOR   (1)

/** Minor Version Number of API.
 * Updated by developer */
#define STDIO_OSTM_HLD_VERSION_MINOR   (2)

/** Build Number of API.
 * Generated during customer release */
#define STDIO_OSTM_HLD_BUILD_NUM       (0)

/** Unique ID. Assigned by requirements */
#define STDIO_OSTM_HLD_UID             (37)


/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum
{
    CTRL_OSTM_START_TIMER = 0,       /*!< Start timer control function */
    CTRL_OSTM_STOP_TIMER,            /*!< Stop timer control function  */
    CTRL_OSTM_RECONFIGURE,           /*!< Stop timer control function  */
	CTRL_OSTM_READ_COUNTER           /*!< Read timer counter control function  */
} e_ctrl_code_ostm_t;


/******************************************************************************
Constant Data
******************************************************************************/

extern const st_r_driver_t g_ostm_driver;

#endif  /* R_OSTM_DRV_API_H_ */


/*******************************************************************************************************************//**
  End  Of File
 ***********************************************************************************************************************/
