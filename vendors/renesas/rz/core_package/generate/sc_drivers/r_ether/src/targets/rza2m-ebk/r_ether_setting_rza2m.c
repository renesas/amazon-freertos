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
* File Name    : r_ether_setting_rza2m.c
* Version      : 1.01
* Device       : RZA2M
* Description  : Ethernet module device driver
**********************************************************************************************************************/

/**********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
**********************************************************************************************************************/
#include "iodefine.h"
#include "iobitmask.h"

#include "r_ether_drv_sc_cfg.h"
#include "r_ether_rza2_if.h"
#include "r_ether_rza2_private.h"

/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/

/**********************************************************************************************************************
Typedef definitions
**********************************************************************************************************************/

/**********************************************************************************************************************
Imported global variables and functions (from other files)
**********************************************************************************************************************/

/**********************************************************************************************************************
Exported global variables (to be accessed by other files)
**********************************************************************************************************************/

/**********************************************************************************************************************
Private global variables and functions
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: ether_set_phy_mode
* Description  :
* Arguments    : connect -
*                    Ethernet channel number
* Return Value : none
**********************************************************************************************************************/
void ether_set_phy_mode(uint8_t connect)
{
    if (g_ether_sc_table[0].config.ether_ch0_en == 1)
    {
        if (PORT_CONNECT_ET0 == (connect & PORT_CONNECT_ET0))
        {
            if (g_ether_sc_table[0].config.ether_mode == 0)
            {
                /* CH0 MII */
                GPIO.PFENET.BIT.PHYMODE0 = 1;
            }
        }
        else if (g_ether_sc_table[0].config.ether_mode == 1)
        {
            /* CH0 RMII */
            GPIO.PFENET.BIT.PHYMODE0 = 0;
            GPIO.PMODEPFS.BIT.ET0_EXOUT_SEL = 0;
        }
        else
        {
            /* Do Nothing */
        }
    }

    if (g_ether_sc_table[0].config.ether_ch1_en == 1)
    {
        if (PORT_CONNECT_ET1 == (connect & PORT_CONNECT_ET1))
        {
            if (g_ether_sc_table[0].config.ether_mode == 0)
            {
                /* CH1 MII */
                GPIO.PFENET.BIT.PHYMODE1 = 1;
            }
            else if (g_ether_sc_table[0].config.ether_mode == 1)
            {
                /* CH1 RMII */
                GPIO.PFENET.BIT.PHYMODE1 = 0;
                GPIO.PMODEPFS.BIT.ET1_EXOUT_SEL = 0;
            }
            else
            {
                /* Do Nothing */
            }
        }
    }

} /* End of function ether_set_phy_mode() */

/* End of File */
