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
 * File Name    : r_hyperbus_lld_rza2m_api.c
 * Version      : 1.0
 * Description  : API function of HyperBus Controller module
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2019 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "r_typedefs.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "rza_io_regrw.h"
#include "driver.h"

#include "r_startup_config.h"
#include "r_hyperbus_lld_rza2m_api.h"
#include "r_hyperbus_drv_sc_cfg.h"
#include "hwsetup.h"
#include "r_stb_drv_api.h"
#include "r_compiler_abstraction_api.h"

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

/*! Version Information */
static const st_drv_info_t gs_lld_info =
{
    {
        ((STDIO_HYPERBUS_RZ_LLD_VERSION_MAJOR << 16) + STDIO_HYPERBUS_RZ_LLD_VERSION_MINOR)
    },
    STDIO_HYPERBUS_RZ_LLD_BUILD_NUM,
    STDIO_HYPERBUS_RZ_LLD_DRV_NAME
};

static void hyperbus_io_regwrite_16(volatile uint16_t * ioreg, uint16_t write_value, uint16_t shift, uint32_t mask);
static void hyperbus_io_regwrite_32(volatile uint32_t * ioreg, uint32_t write_value, uint32_t shift, uint32_t mask);
static uint16_t hyperbus_io_regread_16(volatile uint16_t * ioreg, uint16_t shift, uint32_t mask);
static uint32_t hyperbus_io_regread_32(volatile uint32_t * ioreg, uint32_t shift, uint32_t mask);
static int_t hyperbus_init(const st_hyperbus_cfg_t *p_cfg);

/**********************************************************************************************************************
 * Function Name: hyperbus_io_regwrite_16
 * Description  : IO register 16-bit write
 * Arguments    : volatile uint16_t * ioreg : IO register for writing
 *              :                           : Use register definition name of the
 *              :                           : iodefine.h
 *              : uint16_t write_value      : Write value for the IO register
 *              : uint16_t shift            : The number of left shifts to the
 *              :                          : target bit
 *              : uint32_t mask             : Mask value for the IO register
 *              :                          : (Target bit : "1")
 * Return Value : None
 *********************************************************************************************************************/
static void hyperbus_io_regwrite_16(volatile uint16_t * ioreg, uint16_t write_value, uint16_t shift, uint32_t mask)
{
    uint16_t reg_value;

    if ( IOREG_NONMASK_ACCESS != mask )
    {
        reg_value = *ioreg;                                         /* Read from register */
        reg_value = (uint16_t)((reg_value & (~mask)) | (unsigned)(write_value << shift)); /* Modify value       */
    }
    else
    {
        reg_value = write_value;
    }
    *ioreg    = reg_value;                                      /* Write to register  */
}
/**********************************************************************************************************************
 * End of function hyperbus_io_regwrite_16
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: hyperbus_io_regwrite_32
 * Description  : IO register 32-bit write
 * Arguments    : volatile uint32_t * ioreg : IO register for writing
 *              :                           : Use register definition name of the
 *              :                           : iodefine.h
 *              : uint32_t write_value      : Write value for the IO register
 *              : uint32_t shift            : The number of left shifts to the
 *              :                           : target bit
 *              : uint32_t mask             : Mask value for the IO register
 *              :                           : (Target bit : "1")
 * Return Value : None
 *********************************************************************************************************************/
static void hyperbus_io_regwrite_32(volatile uint32_t * ioreg, uint32_t write_value, uint32_t shift, uint32_t mask)
{
    uint32_t reg_value;
    
    if ( IOREG_NONMASK_ACCESS != mask )
    {
        reg_value = *ioreg;                                         /* Read from register */
        reg_value = (reg_value & (~mask)) | (unsigned)(write_value << shift); /* Modify value       */
    }
    else
    {
        reg_value = write_value;
    }
    *ioreg    = reg_value;                                      /* Write to register  */
}
/**********************************************************************************************************************
 * End of function hyperbus_io_regwrite_32
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: hyperbus_io_regread_16
 * Description  : IO register 16-bit read
 * Arguments    : volatile uint16_t * ioreg : IO register for reading
 *              :                           : Use register definition name of the
 *              :                           : iodefine.h
 *              : uint16_t shift            : The number of right shifts to the
 *              :                           : target bit
 *              : uint32_t mask             : Mask bit for the IO register
 *              :                           : (Target bit: "1")
 * Return Value : uint16_t : Value of the obtained target bit
 *********************************************************************************************************************/
static uint16_t hyperbus_io_regread_16(volatile uint16_t * ioreg, uint16_t shift, uint32_t mask)
{
    uint16_t reg_value;

    reg_value = *ioreg;                         /* Read from register            */
    if ( IOREG_NONMASK_ACCESS != mask )
    {
        /* Casting the value of mask to uint16_t is valid because
         * mask is common size declaration of io_regread function */
        reg_value = (uint16_t)((reg_value & mask) >> shift);    /* Clear other bit and Bit shift */
    }

    return reg_value;
}
/**********************************************************************************************************************
 * End of function hyperbus_io_regread_16
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: hyperbus_io_regread_32
 * Description  : IO register 32-bit read
 * Arguments    : volatile uint32_t * ioreg : IO register for reading
 *              :                           : Use register definition name of the
 *              :                           : iodefine.h
 *              : uint32_t shift            : The number of right shifts to the
 *              :                           : target bit
 *              : uint32_t mask             : Mask bit for the IO register
 *              :                           : (Target bit: "1")
 * Return Value : uint32_t : Value of the obtained target bit
 *********************************************************************************************************************/
static uint32_t hyperbus_io_regread_32(volatile uint32_t * ioreg, uint32_t shift, uint32_t mask)
{
    uint32_t reg_value;

    reg_value = *ioreg;                         /* Read from register            */
    if ( IOREG_NONMASK_ACCESS != mask )
    {
        reg_value = (reg_value & mask) >> shift;    /* Clear other bit and Bit shift */
    }

    return reg_value;
}
/**********************************************************************************************************************
 * End of function hyperbus_io_regread_32
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: hyperbus_init
 * Description  : Initialize HyperBus controller of RZ/A2M
 * Arguments    : p_cfg       : Pointer to the configuration table
 * Return Value : DRV_SUCCESS  Successful operation (always)
 *********************************************************************************************************************/
static int_t hyperbus_init(const st_hyperbus_cfg_t *p_cfg)
{
    volatile uint32_t dummy_32;

    /** Select HyperBus controller */
    hyperbus_io_regwrite_32(&GPIO.PHMOM0.LONG, (uint32_t) 0, GPIO_PHMOM0_HOSEL_SHIFT, GPIO_PHMOM0_HOSEL);

    if (((0 == STARTUP_CFG_PROJECT_TYPE) && (HYPERBUS_INIT_AT_LOADER == p_cfg->init_flag0))
    ||  ((1 == STARTUP_CFG_PROJECT_TYPE) && (HYPERBUS_INIT_AT_APP == p_cfg->init_flag0)))
    {
        /** Make write data to MCR0  */
        dummy_32 = (uint32_t) ((p_cfg->maxen0  << HYPER_MCR0_MAXEN_SHIFT)
                    |          (p_cfg->maxlen0 << HYPER_MCR0_MAXLEN_SHIFT)
                    |           3);

        /** Write to MCR0 */
        hyperbus_io_regwrite_32(&HYPER.MCR0.LONG, dummy_32, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

        /** Make write data to MTR0 */
        dummy_32 = (uint32_t) ((p_cfg->rcshi0 << HYPER_MTR0_RCSHI_SHIFT)
                    |          (p_cfg->wcshi0 << HYPER_MTR0_WCSHI_SHIFT)
                    |          (p_cfg->rcss0  << HYPER_MTR0_RCSS_SHIFT)
                    |          (p_cfg->wcss0  << HYPER_MTR0_WCSS_SHIFT)
                    |          (p_cfg->rcsh0  << HYPER_MTR0_RCSH_SHIFT)
                    |          (p_cfg->wcsh0  << HYPER_MTR0_WCSH_SHIFT)
                    |           1);

        /** Write to MTR0 */
        hyperbus_io_regwrite_32(&HYPER.MTR0.LONG, dummy_32, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
        hyperbus_io_regread_32(&HYPER.MTR0.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    if (((0 == STARTUP_CFG_PROJECT_TYPE) && (HYPERBUS_INIT_AT_LOADER == p_cfg->init_flag1))
    ||  ((1 == STARTUP_CFG_PROJECT_TYPE) && (HYPERBUS_INIT_AT_APP == p_cfg->init_flag1)))
    {
        /** Make write data to MCR1 */
        dummy_32 = (uint32_t) ((p_cfg->maxen1  << HYPER_MCR1_MAXEN_SHIFT)
                    |          (p_cfg->maxlen1 << HYPER_MCR1_MAXLEN_SHIFT)
                    |          (0              << HYPER_MCR1_CRT_SHIFT)
                    |          (1              << HYPER_MCR1_DEVTYPE_SHIFT)
                    |           3);

        /** Write to MCR1 */
        hyperbus_io_regwrite_32(&HYPER.MCR1.LONG, dummy_32, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

        /** Make write data to MTR1 */
        dummy_32 = (uint32_t) ((p_cfg->rcshi1        << HYPER_MTR1_RCSHI_SHIFT)
                    |          (p_cfg->wcshi1        << HYPER_MTR1_WCSHI_SHIFT)
                    |          (p_cfg->rcss1         << HYPER_MTR1_RCSS_SHIFT)
                    |          (p_cfg->wcss1         << HYPER_MTR1_WCSS_SHIFT)
                    |          (p_cfg->rcsh1         << HYPER_MTR1_RCSH_SHIFT)
                    |          (p_cfg->wcsh1         << HYPER_MTR1_WCSH_SHIFT)
                    |          (p_cfg->operate_ltcy1 << HYPER_MTR1_LTCY_SHIFT));

        /** Write to MTR1 */
        hyperbus_io_regwrite_32(&HYPER.MTR1.LONG, dummy_32, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
        hyperbus_io_regread_32(&HYPER.MTR1.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of function hyperbus_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_HYPERBUS_Setup
 * Description  : Initialize HyperBus controller and connected device via user
 *                code
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void R_HYPERBUS_Setup(void)
{
    int_t ret;

    /* Hyperbus clock start */
    R_STB_StartModule(MODULE_HYPERBUS);

    /** initialise controller using smart configurator settings */
    ret = hyperbus_init(&HYPERBUS_SC_TABLE[0]);
    if ( DRV_SUCCESS != ret )
    {
        while(1)
        {
            /* spin here forever */
            R_COMPILER_Nop();
        }
    }

    /* checking if user function is avail */
    if ( NULL != HyperBus_UserConfig )
    {
        ret = HyperBus_UserConfig(&HYPERBUS_SC_TABLE[0]);
        if ( DRV_SUCCESS != ret )
        {
            while(1)
            {
                /* spin here forever */
                R_COMPILER_Nop();
            }
        }
    }

    return;
}
/**********************************************************************************************************************
 * End of function R_HYPERBUS_Setup
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_HYPERBUS_SelectSpace
 * Description  : Set latency field of MTR
 * Arguments    : area        : area select(CS0 or CS1)
 *              : space       : space select(Memory or Register)
 * Return Value : DRV_SUCCESS  Successful operation (always)
 *              : DRV_ERROR    CS0 area selected
 *********************************************************************************************************************/
int_t R_HYPERBUS_SelectSpace(e_hyperbus_access_area_t area, e_hyperbus_space_select_t space)
{
    /* area parameter check */
    if (HYPERBUS_CS1_AREA != area)
    {
        return DRV_ERROR;
    }

    /** Write to MCR1 */
    hyperbus_io_regwrite_32(&HYPER.MCR1.LONG, (uint32_t) space, HYPER_MCR1_CRT_SHIFT, HYPER_MCR1_CRT);

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_HYPERBUS_SelectSpace
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_HYPERBUS_GetVersion
 * Description  : Obtains driver specific version information which is used for
 *              : reporting
 * Arguments    : p_ver_info: driver version information is populated into
 *                            this structure
 *                            @note the structure resources are created by
 *                             the application not this function.
 * Return Value : DRV_SUCCESS  Successful operation (always)
 *********************************************************************************************************************/
int_t R_HYPERBUS_GetVersion(st_ver_info_t *p_ver_info)
{
    p_ver_info->lld.p_szdriver_name   = gs_lld_info.p_szdriver_name;
    p_ver_info->lld.version.sub.major = gs_lld_info.version.sub.major;
    p_ver_info->lld.version.sub.minor = gs_lld_info.version.sub.minor;
    p_ver_info->lld.build = gs_lld_info.build;

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of function R_HYPERBUS_GetVersion
 *********************************************************************************************************************/
