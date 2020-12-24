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

/**********************************************************************************************************************
 * File Name    : r_os_abstraction_stdio.c
 * Version      : see OS_LESS_RZ_HLD_VERSION_MAJOR.OS_LESS_RZ_HLD_VERSION_MINOR
 * Description  : OS-Less OS Abstraction Layer
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * History      : DD.MM.YYYY  Description
 *              : 21.05.2020  First issued
 *********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "r_typedefs.h"
#include "driver.h"
#include "r_devlink_wrapper.h"

/* compiler specific API header */
#include "r_compiler_abstraction_api.h"

/* OS abstraction specific API header */
#include "r_os_abstraction_api.h"


/**********************************************************************************************************************
 * Function Macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Typedefs
 *********************************************************************************************************************/



/**********************************************************************************************************************
 Function Prototypes
 *********************************************************************************************************************/
/* STDIO */
static int_t stdio_hld_open (st_stream_ptr_t p_stream);
static void  stdio_hld_close (st_stream_ptr_t p_stream);
static int_t stdio_hld_read (st_stream_ptr_t p_stream, uint8_t *p_buffer, uint32_t ui_count);
static int_t stdio_hld_write (st_stream_ptr_t p_stream, const uint8_t *p_buffer, uint32_t count);
static int_t stdio_hld_control (st_stream_ptr_t p_stream, uint32_t ctlCode, void *pCtlStruct);
static int_t stdio_hld_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *pVerInfo);

/**********************************************************************************************************************
 Constant Data
 *********************************************************************************************************************/

/* Define the driver function table for this implementation */
/* This is an empty driver. This driver is used to reserve a file handle. */
const st_r_driver_t g_stdio_driver =
{"STDIO Device Driver", stdio_hld_open, stdio_hld_close, stdio_hld_read, stdio_hld_write, stdio_hld_control, stdio_hld_get_version};


/**********************************************************************************************************************
 Global Data
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private Functions
 *********************************************************************************************************************/

/**
 * @brief  Opens the STDIO device driver
 * 
 * This function allows for a channel to be ready for
 * operational use. If a channel is unavailable or a
 * channel is already opened this function will return
 * a fail (-1)
 *
 * @param[in] p_stream Pointer to the file stream
 * 
 * @retval 0  DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 **/
static int_t stdio_hld_open (st_stream_ptr_t p_stream)
{
    int_t ret = DRV_SUCCESS;

    return (ret);
}
/**********************************************************************************************************************
 * End of Function stdio_hld_open
 *********************************************************************************************************************/

/**
 * @brief Closes the STDIO driver
 *
 * The STDIO driver's file stream can be closed once
 * the desired operation has been done, freeing any
 * memory or resources it uses
 * 
 * @param[in] p_stream Pointer to the file stream
 *
 * @return None
 */
static void stdio_hld_close (st_stream_ptr_t p_stream)
{
    return;
}
/**********************************************************************************************************************
 * End of Function stdio_hld_close
 *********************************************************************************************************************/

/**
 * @brief Translates the read operation to the private read function.
 *
 * The private read function requires to get the channel instead of the
 * file stream and therefore some translation is required.
 *
 * @param[in]  p_stream The file stream that contains the desired STDIO channel
 * @param[out] p_buffer The buffer to be populated with the data read
 * @param[in]  count    The size of p_buffer
 *
 * @retval  N Where N is the number of bytes read
 * @retval -1 DRV_ERROR 
 */
static int_t stdio_hld_read (st_stream_ptr_t p_stream, uint8_t *p_buffer, uint32_t count)
{
    /* read from channel */
    return (0);
}
/**********************************************************************************************************************
 * End of Function stdio_hld_read
 *********************************************************************************************************************/

/**
 * @brief Translates the write operation to the private write function
 *
 * The private write function requires to get the channel instead of the
 * file stream and therefore some translation is required
 *
 * @param[in] p_stream The file stream that contains the desired STDIO channel
 * @param[in] p_buffer The intended buffer of data intended to be written
 * @param[in] count    The amount of data in the p_buffer
 *
 * @retval >0 The number of bytes written
 * @retval -1 DRV_ERROR 
 */
static int_t stdio_hld_write (st_stream_ptr_t p_stream, const uint8_t *p_buffer, uint32_t count)
{
    /* write to channel */
    return (0);
}
/**********************************************************************************************************************
 * End of Function stdio_hld_write
 *********************************************************************************************************************/

/**
 * @brief stdio_hld_control handles custom controls for the STDIO device driver
 * 
 * Depending on the ctl_code given the control function will perform specific
 * tasks. For the STDIO this includes getting and setting the configuration.
 * If a invalid ctl_code is given the return value will be -1 and the trace will
 * print a message accordingly.
 *
 * @param[in] p_stream          Pointer to the file stream
 * @param[in] ctl_code          The custom control code
 * @param[in,out] p_ctl_struct  Pointer to the custom control structure
 *
 * @retval 0  DRV_SUCCESS
 * @retval -1 DRV_ERROR 
 */
static int_t stdio_hld_control (st_stream_ptr_t p_stream, uint32_t ctl_code, void *p_ctl_struct)
{
    int_t ret = DRV_SUCCESS;


    return (ret);
}
/**********************************************************************************************************************
 * End of Function stdio_hld_control()
 *********************************************************************************************************************/

/**
 * @brief Provides build information even if driver fails to open\n
 *        version information is updated by developer
 * 
 * Acquires the version information for both the high layer and
 * low layer driver. This allows for control of releases and debugging
 * code.
 *
 * @param stream_ptr  Pointer to the file stream
 * @param p_ver_info  Structure holding the version information
 *
 * @retval 0  DRV_SUCCESS (Never Fails)
 */
static int_t stdio_hld_get_version (st_stream_ptr_t stream_ptr, st_ver_info_t *p_ver_info)
{
    return (DRV_SUCCESS);
}
/**********************************************************************************************************************
 * End of Function stdio_hld_get_version
 *********************************************************************************************************************/

/**********************************************************************************************************************
 End Of File
 *********************************************************************************************************************/
