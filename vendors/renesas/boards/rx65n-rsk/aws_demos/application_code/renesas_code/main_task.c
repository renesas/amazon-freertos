  
/**********************************************************************************************************************
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
 * Copyright (C) 2014(2016) Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : main_task.c
 * Description  : main task
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 29.12.2019 1.00 First Release
 *********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

/* for using C standard library */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* for using FIT Module */
#include "platform.h"
#include "r_pinset.h"
#include "r_flash_rx_if.h"
#include "r_simple_filesystem_on_dataflash_if.h"
#include "Pin.h"



/* for using Amazon FreeRTOS */
#include "FreeRTOS.h"
#include "aws_application_version.h"



/**********************************************************************************************************************
Typedef definitions
**********************************************************************************************************************/
//#define RX72N_ENVISION_KIT_TASKS_STACK	1024

/******************************************************************************
 External variables
 ******************************************************************************/
xSemaphoreHandle xSemaphoreFlashAccess;

/******************************************************************************
 Private global variables
 ******************************************************************************/


/******************************************************************************

 External functions
 ******************************************************************************/


void main_task(void);

/******************************************************************************
 Function Name   : main
 Description     : Main task
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void main_task(void)
{
    uint32_t bank_info;

    /* enable MCU pins */
    R_Pins_Create();


    /* flash initialization */
    R_FLASH_Open();
//    R_FLASH_Control(FLASH_CMD_BANK_GET, &bank_info);
    R_SFD_Open();

    /* flash access semaphore creation */
    xSemaphoreFlashAccess = xSemaphoreCreateMutex();
    xSemaphoreGive(xSemaphoreFlashAccess);

//    task_info.main_task_handle = xTaskGetCurrentTaskHandle();


    /* main loop */
    while(1)
    {
    	vTaskDelay(10000);
    }
}
/******************************************************************************
 End of function main_task()
 ******************************************************************************/


/******************************************************************************
 End  Of File
 ******************************************************************************/

