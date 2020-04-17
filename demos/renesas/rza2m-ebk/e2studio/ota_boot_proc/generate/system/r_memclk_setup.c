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
 * File Name    : r_memclk_setup.c
 * Device(s)    : RZ/A2M
 * Description  : setting memory clock before starting R_SC_HardwareSetup
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "r_memclk_setup.h"

/**********************************************************************************************************************
 * Function Name: r_memclk_setup
 * Description  : setting memory clock before starting R_SC_HardwareSetup function
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void r_memclk_setup(void)
{
    /* Call SPIBSC clock setup if defined */
    if (NULL != r_spibsc_memclk_setup)
    {
        r_spibsc_memclk_setup();
    }

    /* Call HYPERBUS clock setup if defined */
    if (NULL != r_hyperbus_memclk_setup)
    {
        r_hyperbus_memclk_setup();
    }

    /* Call OCTABUS clock setup if defined */
    if (NULL != r_octabus_memclk_setup)
    {
        r_octabus_memclk_setup();
    }
}
/**********************************************************************************************************************
 * End of function r_memclk_setup
 *********************************************************************************************************************/

/* End of File */
