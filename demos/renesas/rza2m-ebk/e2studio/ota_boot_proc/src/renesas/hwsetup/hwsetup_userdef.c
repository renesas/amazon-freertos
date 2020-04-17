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
 * File Name    : hwsetup_userdef.c
 * Version      : 1.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 29.08.2019 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "rza_io_regrw.h"
#include "r_cpg_drv_api.h"
#include "hwsetup.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Comment it if you does not need to use pre hardware setup function */
#define R_PRV_USE_PRE_HWSETUP

/* Comment it if you does not need to use post hardware setup function */
#define R_PRV_USE_POST_HWSETUP

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
 * Function Name: Userdef_PreHardwareSetup
 * Description  : User defined hardware setup called earlier of R_SC_HardwareSetup.
 * Arguments    : None.
 * Return Value : None.
 *********************************************************************************************************************/
#if defined(R_PRV_USE_PRE_HWSETUP)
void Userdef_PreHardwareSetup(void)
{
    /* If the IOKEEP is 1, then clear to 0. */
    if(1 == RZA_IO_RegRead_16(&PMG.DSFR.WORD, PMG_DSFR_IOKEEP_SHIFT, PMG_DSFR_IOKEEP))
    {
        /* Clear IOKEEP bit */
        RZA_IO_RegWrite_16(&PMG.DSFR.WORD, 0, PMG_DSFR_IOKEEP_SHIFT, PMG_DSFR_IOKEEP);

        /* dummy read*/
        RZA_IO_RegRead_16(&PMG.DSFR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }
}
#endif
/**********************************************************************************************************************
 * End of function Userdef_PreHardwareSetup
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: Userdef_PostHardwareSetup
 * Description  : User defined hardware setup called later of R_SC_HardwareSetup.
 * Arguments    : None.
 * Return Value : None.
 *********************************************************************************************************************/
#if defined(R_PRV_USE_POST_HWSETUP)
void Userdef_PostHardwareSetup(void)
{
    /* Enable write access for the retention RAM (0x8000_0000 ~ 0x8001_FFFF) */
    RZA_IO_RegWrite_8(&CPG.SYSCR3.BYTE, 0xf, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* dummy read*/
    RZA_IO_RegRead_8(&CPG.SYSCR3.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
}
#endif
/**********************************************************************************************************************
 * End of function Userdef_PostHardwareSetup
 *********************************************************************************************************************/
