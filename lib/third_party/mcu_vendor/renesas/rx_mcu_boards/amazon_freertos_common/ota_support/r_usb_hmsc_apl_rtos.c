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
 * Copyright (C) 2014(2019) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_hmsc_apl_rtos.c
 * Description  : USB Host MSC application code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 31.03.2018 1.23 Using Pin setting API.
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "platform.h"
#include "r_usb_hmsc_apl.h"

#include "r_usb_typedef.h"
#include "r_usb_rtos_apl.h"

/*******************************************************************************
 Macro definitions
 *******************************************************************************/
#if !defined(MY_BSP_CFG_USE_USBIP)
#error "Error! Need to define MY_BSP_CFG_USE_USBIP in r_bsp_config.h"
#elif MY_BSP_CFG_USE_USBIP == (0)
#define R_USB_PinSet()      R_USB_PinSet_USB0_HOST()
#define USB_IP              USB_IP0
#elif MY_BSP_CFG_USE_USBIP == (1)
#define R_USB_PinSet()      R_USB_PinSet_USB1_HOST()
#define USB_IP              USB_IP1
#else
#error "Error! Invalid setting for MY_BSP_CFG_USE_USBIP in r_bsp_config.h"
#endif

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
static  FATFS       g_file_object;

/******************************************************************************
 Exported global functions
 ******************************************************************************/
uint8_t g_isFileWrite;

/******************************************************************************
 Function Name   : usb_apl_callback
 Description     : Callback function for HMSC application program
 Arguments       : usb_ctrl_t *p_ctrl   : 
                   usb_hdl_t  cur_task  : 
                   uint8_t    usb_state : 
 Return value    : none
 ******************************************************************************/
void usb_apl_callback (usb_ctrl_t *p_ctrl, usb_hdl_t cur_task, uint8_t usb_state)
{
    USB_APL_SND_MSG(USB_APL_MBX, (usb_msg_t *)p_ctrl);
} /* End of function usb_apl_callback */

/******************************************************************************
 Function Name   : usb_init
 Description     : Host MSC application init process
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_init (void)
{
    usb_ctrl_t  ctrl;
    usb_cfg_t   cfg;

    R_USB_PinSet();
    ctrl.module     = USB_IP;
    ctrl.type       = USB_HMSC;
    cfg.usb_speed   = USB_FS;
    cfg.usb_mode    = USB_HOST;
    R_USB_Open(&ctrl, &cfg);
    R_USB_Callback((usb_callback_t *)usb_apl_callback);

} /* End of function usb_init */

/******************************************************************************
 Function Name   : usb_attached
 Description     : Host MSC application main process
 Arguments       : none
 Return Value    : (-1) Error
                    (0) None
                    (1) Attached
                    (2) Detached
 ******************************************************************************/
int32_t usb_attached (void)
{
    usb_ctrl_t  ctrl;
    usb_ctrl_t  *p_mess;
    uint8_t     drvno = 0;
    FRESULT     ret;
    int32_t     usb_status = 0;

    USB_APL_RCV_MSG(USB_APL_MBX, (usb_msg_t **)&p_mess);
    ctrl = *p_mess;

    switch (ctrl.event)
    {
        case USB_STS_CONFIGURED :
            R_USB_HmscGetDriveNo(&ctrl, &drvno);
            ret = R_tfat_f_mount(drvno, &g_file_object);
            if (TFAT_RES_OK != ret)
            {
                usb_status = -1;
            }
            else
            {
                usb_status = 1;
            }
        break;

        case USB_STS_DETACH :
            drvno = 0;
            g_isFileWrite = USB_APL_NO;
            usb_status  = 2;
        break;

        default :
            usb_status = 0;
        break;
    }

    return usb_status;
} /* End of function usb_attached */

/******************************************************************************
 Function Name   : usb_apl_rec_msg
 Description     : Receive a message to the specified id (mailbox).
 Argument        : uint8_t id        : ID number (mailbox).
                 : usb_msg_t** mess  : Message pointer
                 : usb_tm_t tm       : Timeout Value
 Return          : uint16_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_apl_rec_msg (uint8_t id, usb_msg_t** mess, usb_tm_t tm)
{
    BaseType_t err;
    QueueHandle_t handle;

    (void *)tm;

    if (NULL == mess)
    {
        return USB_APL_ERROR;
    }

    handle = (QueueHandle_t)(*(g_apl_mbx_table[id]));
    *mess  = NULL;

    /** Receive message from queue specified by *(mbx_table[id]) **/
    err = xQueueReceive(handle, (void *)mess, (TickType_t)(portMAX_DELAY));
    if ((pdTRUE == err) && (NULL != (*mess)))
    {
        return USB_APL_OK;
    }
    else
    {
        return USB_APL_ERROR;
    }
} /* End of function usb_apl_rec_msg */

/******************************************************************************
 Function Name   : usb_apl_snd_msg
 Description     : Send a message to the specified id (mailbox).
 Argument        : uint8_t id        : ID number (mailbox).
                 : usb_msg_t* mess   : Message pointer
 Return          : usb_er_t          : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_apl_snd_msg (uint8_t id, usb_msg_t* mess)
{
    BaseType_t err;
    QueueHandle_t handle;

    if (NULL == mess)
    {
        return USB_APL_ERROR;
    }

    handle = (QueueHandle_t)(*(g_apl_mbx_table[id]));

    /** Send message to queue specified by *(mbx_table[id]) **/
    err = xQueueSend(handle, (const void *)&mess, (TickType_t)(0));
    if (pdTRUE == err)
    {
        return USB_APL_OK;
    }
    else
    {
        return USB_APL_ERROR;
    }
} /* End of function usb_apl_snd_msg */

/******************************************************************************
 End of file
 ******************************************************************************/
