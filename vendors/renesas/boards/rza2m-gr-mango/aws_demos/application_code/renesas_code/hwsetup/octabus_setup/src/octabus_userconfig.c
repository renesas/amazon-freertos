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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : octabus_userconfig.c
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "r_typedefs.h"

#include "r_startup_config.h"
#include "r_octabus_lld_rza2m_api.h"
#include "octabus_ram_rza2m.h"

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
 * Function Name: OctaBus_UserConfig
 * Description  : User defined device initialisation routine for OctaBus
 * Arguments    : p_cfg       : Pointer to the configuration table
 * Return Value : DRV_SUCCESS   Successful operation.
 * Precautions  : This function cannot be assigned to execute from OctaFlash or OctaRAM.
 *              : This function must be assigned to an area other than OctaFlash or OctaRAM.
 *********************************************************************************************************************/
int_t OctaBus_UserConfig(const st_octabus_cfg_t *p_cfg)
{
    /* p_cfg not used */
    UNUSED_PARAM(p_cfg);

    st_octabus_configmode_t reg_config;
    uint32_t mode_value;

    /* Set reg_config members for Mode Register Write */
    reg_config.dcr_value  = 0x00004000;   /* Mode Register Write command(40h) */
    reg_config.dar_value  = 0x00040000;   /* RAM Mode Register Address */
    reg_config.dcsr_value = 0x04A80002;   /* [28]    ACDA          = 0    : Register Access
                                             [27]    DOPI          = 0    : 1clock - 2byte
                                             [26:24] ADLEN[2:0]    = 100  : Address Length 4byte
                                             [23]    DAOR          = 1    : byte1, byte0, byte3, byte2
                                             [22:20] CMDLEN[2:0]   = 010  : Command length 2byte
                                             [19]    ACDV          = 1    : Target Device1
                                             [15: 8] DMLEN[7:0]    = 0x00 : Dummy cycle Length 0 clocks
                                             [ 7: 0] DALEN[7:0]    = 10   : Data Length 2bytes */
    reg_config.dcstr_value = 0x00002A00;  /* [15:14] DVSELLO[1:0]  = 00   : 1.5 clock cycles
                                             [13:11] DVSELHI[2:0]  = 101  : 6.5 clock cycles
                                             [10: 8] DVSELCMD[2:0] = 010  : 7 clock cycles */

    /* Set write data for Mode Register of OctaRAM */
    mode_value = 0x0000F021;              /* [15]    Deep Power Down Enable = 1    : Normal Operation
                                             [14:12] Drive Strength         = 111  : 25ohms 010  : 50ohms
                                             [11:8]  Reserved               = 0000
                                             [7:4]   Latency Code           = 0010 : 5 clocks
                                             [3]     Latency Type           = 0    : Variable Latency
                                             [2]     Burst Type             = 0    : Wrapped
                                             [1:0]   Burst Length           = 01   : 64 bytes */

    /* Write to Mode register of OctaRAM */
    R_OCTABUS_WriteConfigMode(&reg_config, mode_value);

    return DRV_SUCCESS;
}

/**********************************************************************************************************************
 * End of function OctaBus_UserConfig
 *********************************************************************************************************************/

/* End of File */
