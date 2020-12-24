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
* File Name    : r_pinset.c
* Version      : 1.01
* Device(s)    : R7S921053
* Tool-Chain   : GCC
* Description  : Setting of port and mpc registers
* Creation Date: 2018-12-06
**********************************************************************************************************************/

/**********************************************************************************************************************
Includes
**********************************************************************************************************************/
#include "r_pinset.h"
#include "iodefine.h"
#include "r_ether_drv_sc_cfg.h"
#include "driver.h"              /* Device Driver common header */

/**********************************************************************************************************************
Global variables and functions
**********************************************************************************************************************/
/**********************************************************************************************************************
* Function Name: R_ETHER_PinSet_ETHERC_RMII
* Description  : This function initializes pins for r_ether_rza2 module
* Arguments    : none
* Return Value : none
**********************************************************************************************************************/
void R_ETHER_PinSet_ETHERC_RMII(void)
{
    /* NULL type is (void*) */
    if ((g_ether_sc_table[0].pin.p_config_table != NULL) && (g_ether_sc_table[0].pin.count > 0))
    {
        int_t gpio_handle = (-1);
        gpio_handle = direct_open("gpio", 0);

        if ((-1) != gpio_handle)
        {
            /* direct_control as direct_control assumes parameter 3 is i/o,
             * but using the parameter in input only mode is acceptable */
            direct_control(gpio_handle,
                            CTL_GPIO_INIT_BY_TABLE,

                            /* cast to (st_r_drv_gpio_pin_init_table_t *) */
                            (st_r_drv_gpio_pin_init_table_t *) &g_ether_sc_table[0].pin);
            direct_close(gpio_handle);
        }
    }
}  /* End of function R_ETHER_PinSet_ETHERC_RMII() */
