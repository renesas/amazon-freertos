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
 * File Name    : hwsetup.h
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
#include "iodefine.h"
#include "iobitmask.h"
#include "rza_io_regrw.h"
#include "r_startup_config.h"
#include "r_cpg_drv_api.h"
#include "hwsetup.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

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
        R_SPIBSC_Setup();
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

#if STARTUP_CFG_RETENTION_RAM_WRITE_ENABLE == 1

    /* Enable write access for the retention RAM (0x8000_0000 ~ 0x8001_FFFF) */
    RZA_IO_RegWrite_8(&CPG.SYSCR3.BYTE, 0xf, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* dummy read*/
    RZA_IO_RegRead_8(&CPG.SYSCR3.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

#endif

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
