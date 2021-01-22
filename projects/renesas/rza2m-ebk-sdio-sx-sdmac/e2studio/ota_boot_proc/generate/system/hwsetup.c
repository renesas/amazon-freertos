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
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : hwsetup.c
 * Device(s)    : RZ/A2M
 * OS           : None
 * Description  : Initialisation process done before section initialise
 * Operation    :
 * Limitations  :
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "hwsetup.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/*! Using DDR transfer for SPIBSC. Comment it if you want to use SDR transfer */
#define SPIBSC_PRV_DDR_SETTING

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_SC_HardwareSetup
 * @brief         Hardware setup, will done before section initialise
 * @param         None
 * @return        None
 *********************************************************************************************************************/
void R_SC_HardwareSetup(void)
{
    /** Call user defined pre-initialisation routine */
    if (NULL != Userdef_PreHardwareSetup)
    {
        Userdef_PreHardwareSetup();
    }

    /** call SPIBSC driver setup wrapper interface if defined */
    if (NULL != R_SPIBSC_Setup)
    {
#ifdef SPIBSC_PRV_DDR_SETTING
        R_SPIBSC_Setup(HWSETUP_SPIBSC_USE_DDR);
#else /* SPIBSC_PRV_DDR_SETTING */
        R_SPIBSC_Setup(HWSETUP_SPIBSC_USE_SDR);
#endif /* SPIBSC_PRV_DDR_SETTING */
    }

    /** call HyperBus driver setup wrapper interface if defined */
    if (NULL != R_HYPERBUS_Setup)
    {
        R_HYPERBUS_Setup();
    }

    /** call OctaBus driver setup wrapper interface if defined */
    if (NULL != R_OCTABUS_Setup)
    {
        R_OCTABUS_Setup();
    }

    /** Call user defined post-initialisation routine */
    if (NULL != Userdef_PostHardwareSetup)
    {
        Userdef_PostHardwareSetup();
    }
}
/**********************************************************************************************************************
 * End of function R_SC_HardwareSetup
 *********************************************************************************************************************/

/* End of File */
