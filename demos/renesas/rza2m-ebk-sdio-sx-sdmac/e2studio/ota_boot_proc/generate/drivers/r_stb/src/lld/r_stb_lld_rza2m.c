/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_stb_lld_rza2m.c
* Device(s)    : RZ/A2M
* Tool-Chain   : e2Studio Ver 7.5.0
*              : GNU Arm Embedded Toolchain 6-2017-q2-update
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : RZ/A2M Sample Program - API function of STB module
* Operation    :
* Limitations  :
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <string.h>
#include <ctype.h>
#include "r_typedefs.h"
#include "r_stb_drv_link.h"
#include "rza_io_regrw.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "compiler_settings.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define STB_PRV_RRAM_AREA_3 (*(volatile unsigned long *)0x80010000)
#define STB_PRV_RRAM_AREA_2 (*(volatile unsigned long *)0x80008000)
#define STB_PRV_RRAM_AREA_1 (*(volatile unsigned long *)0x80004000)
#define STB_PRV_RRAM_AREA_0 (*(volatile unsigned long *)0x80000000)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    e_stb_module_t   module;
    volatile void  * reg_address;
    uint8_t          mask;
} st_r_prv_reg_info_t;


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
/*! Version Information */
static const st_drv_info_t gs_lld_info =
{
    {
        ((STDIO_STB_RZ_LLD_VERSION_MAJOR << 16) + STDIO_STB_RZ_LLD_VERSION_MINOR)
    },
    STDIO_STB_RZ_LLD_BUILD_NUM,
    STDIO_STB_RZ_LLD_DRV_NAME
};

static const st_r_prv_reg_info_t gs_stbcr[] =
{
    {MODULE_CORESIGHT, &CPG.STBCR2.BYTE,  CPG_STBCR2_MSTP20},
    {MODULE_OSTM0,     &CPG.STBCR3.BYTE,  CPG_STBCR3_MSTP36},
    {MODULE_OSTM1,     &CPG.STBCR3.BYTE,  CPG_STBCR3_MSTP35},
    {MODULE_OSTM2,     &CPG.STBCR3.BYTE,  CPG_STBCR3_MSTP34},
    {MODULE_MTU3,      &CPG.STBCR3.BYTE,  CPG_STBCR3_MSTP33},
    {MODULE_CANFD,     &CPG.STBCR3.BYTE,  CPG_STBCR3_MSTP32},
    {MODULE_ADC,       &CPG.STBCR5.BYTE,  CPG_STBCR5_MSTP57},
    {MODULE_GPT,       &CPG.STBCR3.BYTE,  CPG_STBCR3_MSTP30},
    {MODULE_SCIFA0,    &CPG.STBCR4.BYTE,  CPG_STBCR4_MSTP47},
    {MODULE_SCIFA1,    &CPG.STBCR4.BYTE,  CPG_STBCR4_MSTP46},
    {MODULE_SCIFA2,    &CPG.STBCR4.BYTE,  CPG_STBCR4_MSTP45},
    {MODULE_SCIFA3,    &CPG.STBCR4.BYTE,  CPG_STBCR4_MSTP44},
    {MODULE_SCIFA4,    &CPG.STBCR4.BYTE,  CPG_STBCR4_MSTP43},
    {MODULE_SCI0,      &CPG.STBCR4.BYTE,  CPG_STBCR4_MSTP42},
    {MODULE_SCI1,      &CPG.STBCR4.BYTE,  CPG_STBCR4_MSTP41},
    {MODULE_IrDA,      &CPG.STBCR4.BYTE,  CPG_STBCR4_MSTP40},
    {MODULE_CEU,       &CPG.STBCR5.BYTE,  CPG_STBCR5_MSTP56},
    {MODULE_RTC0,      &CPG.STBCR5.BYTE,  CPG_STBCR5_MSTP53},
    {MODULE_RTC1,      &CPG.STBCR5.BYTE,  CPG_STBCR5_MSTP52},
    {MODULE_JCU,       &CPG.STBCR5.BYTE,  CPG_STBCR5_MSTP51},
    {MODULE_VIN,       &CPG.STBCR6.BYTE,  CPG_STBCR6_MSTP66},
    {MODULE_ETHER,     &CPG.STBCR6.BYTE,  CPG_STBCR6_MSTP65|CPG_STBCR6_MSTP64|CPG_STBCR6_MSTP63|CPG_STBCR6_MSTP62},
    {MODULE_USB0,      &CPG.STBCR6.BYTE,  CPG_STBCR6_MSTP61},
    {MODULE_USB1,      &CPG.STBCR6.BYTE,  CPG_STBCR6_MSTP60},
    {MODULE_IMR2,      &CPG.STBCR7.BYTE,  CPG_STBCR7_MSTP77},
    {MODULE_DRW,       &CPG.STBCR7.BYTE,  CPG_STBCR7_MSTP76},
    {MODULE_MIPI,      &CPG.STBCR7.BYTE,  CPG_STBCR7_MSTP75},
    {MODULE_SSIF0,     &CPG.STBCR7.BYTE,  CPG_STBCR7_MSTP73},
    {MODULE_SSIF1,     &CPG.STBCR7.BYTE,  CPG_STBCR7_MSTP72},
    {MODULE_SSIF2,     &CPG.STBCR7.BYTE,  CPG_STBCR7_MSTP71},
    {MODULE_SSIF3,     &CPG.STBCR7.BYTE,  CPG_STBCR7_MSTP70},
    {MODULE_I2C0,      &CPG.STBCR8.BYTE,  CPG_STBCR8_MSTP87},
    {MODULE_I2C1,      &CPG.STBCR8.BYTE,  CPG_STBCR8_MSTP86},
    {MODULE_I2C2,      &CPG.STBCR8.BYTE,  CPG_STBCR8_MSTP85},
    {MODULE_I2C3,      &CPG.STBCR8.BYTE,  CPG_STBCR8_MSTP84},
    {MODULE_SPIBSC,    &CPG.STBCR8.BYTE,  CPG_STBCR8_MSTP83},
    {MODULE_VDC6,      &CPG.STBCR8.BYTE,  CPG_STBCR8_MSTP81},
    {MODULE_RSPI0,     &CPG.STBCR9.BYTE,  CPG_STBCR9_MSTP97},
    {MODULE_RSPI1,     &CPG.STBCR9.BYTE,  CPG_STBCR9_MSTP96},
    {MODULE_RSPI2,     &CPG.STBCR9.BYTE,  CPG_STBCR9_MSTP95},
    {MODULE_HYPERBUS,  &CPG.STBCR9.BYTE,  CPG_STBCR9_MSTP93},
    {MODULE_OCTAMEM,   &CPG.STBCR9.BYTE,  CPG_STBCR9_MSTP92},
    {MODULE_RSPDIF,    &CPG.STBCR9.BYTE,  CPG_STBCR9_MSTP91},
    {MODULE_DRP,       &CPG.STBCR9.BYTE,  CPG_STBCR9_MSTP90},
    {MODULE_TSIP,      &CPG.STBCR10.BYTE, CPG_STBCR10_MSTP107},
    {MODULE_NAND,      &CPG.STBCR10.BYTE, CPG_STBCR10_MSTP104},
    {MODULE_SDMMC0,    &CPG.STBCR10.BYTE, CPG_STBCR10_MSTP103|CPG_STBCR10_MSTP102},
    {MODULE_SDMMC1,    &CPG.STBCR10.BYTE, CPG_STBCR10_MSTP101|CPG_STBCR10_MSTP100},
};

static const st_r_prv_reg_info_t gs_stbreq[] =
{
    {MODULE_CORESIGHT, &CPG.STBREQ1.BYTE, CPG_STBREQ1_STBRQ15},
    {MODULE_CEU,       &CPG.STBREQ1.BYTE, CPG_STBREQ1_STBRQ10},
    {MODULE_JCU,       &CPG.STBREQ1.BYTE, CPG_STBREQ1_STBRQ13},
    {MODULE_VIN,       &CPG.STBREQ2.BYTE, CPG_STBREQ2_STBRQ27},
    {MODULE_ETHER,     &CPG.STBREQ2.BYTE, CPG_STBREQ2_STBRQ26},
    {MODULE_USB0,      &CPG.STBREQ3.BYTE, CPG_STBREQ3_STBRQ31|CPG_STBREQ3_STBRQ30},
    {MODULE_USB1,      &CPG.STBREQ3.BYTE, CPG_STBREQ3_STBRQ33|CPG_STBREQ3_STBRQ32},
    {MODULE_IMR2,      &CPG.STBREQ2.BYTE, CPG_STBREQ2_STBRQ23},
    {MODULE_DRW,       &CPG.STBREQ2.BYTE, CPG_STBREQ2_STBRQ21|CPG_STBREQ2_STBRQ20},
    {MODULE_VDC6,      &CPG.STBREQ2.BYTE, CPG_STBREQ2_STBRQ25},
    {MODULE_DRP,       &CPG.STBREQ2.BYTE, CPG_STBREQ2_STBRQ24},
    {MODULE_NAND,      &CPG.STBREQ2.BYTE, CPG_STBREQ2_STBRQ22},
    {MODULE_SDMMC0,    &CPG.STBREQ1.BYTE, CPG_STBREQ1_STBRQ12},
    {MODULE_SDMMC1,    &CPG.STBREQ1.BYTE, CPG_STBREQ1_STBRQ11},
};

static const st_r_prv_reg_info_t gs_stback[] =
{
    {MODULE_CORESIGHT, &CPG.STBACK1.BYTE, CPG_STBACK1_STBAK15},
    {MODULE_CEU,       &CPG.STBACK1.BYTE, CPG_STBACK1_STBAK10},
    {MODULE_JCU,       &CPG.STBACK1.BYTE, CPG_STBACK1_STBAK13},
    {MODULE_VIN,       &CPG.STBACK2.BYTE, CPG_STBACK2_STBAK27},
    {MODULE_ETHER,     &CPG.STBACK2.BYTE, CPG_STBACK2_STBAK26},
    {MODULE_USB0,      &CPG.STBACK3.BYTE, CPG_STBACK3_STBAK31|CPG_STBACK3_STBAK30},
    {MODULE_USB1,      &CPG.STBACK3.BYTE, CPG_STBACK3_STBAK33|CPG_STBACK3_STBAK32},
    {MODULE_IMR2,      &CPG.STBACK2.BYTE, CPG_STBACK2_STBAK23},
    {MODULE_DRW,       &CPG.STBACK2.BYTE, CPG_STBACK2_STBAK21|CPG_STBACK2_STBAK20},
    {MODULE_VDC6,      &CPG.STBACK2.BYTE, CPG_STBACK2_STBAK25},
    {MODULE_DRP,       &CPG.STBACK2.BYTE, CPG_STBACK2_STBAK24},
    {MODULE_NAND,      &CPG.STBACK2.BYTE, CPG_STBACK2_STBAK22},
    {MODULE_SDMMC0,    &CPG.STBACK1.BYTE, CPG_STBACK1_STBAK12},
    {MODULE_SDMMC1,    &CPG.STBACK1.BYTE, CPG_STBACK1_STBAK11},
};

static const st_r_prv_reg_info_t gs_swrstcr[] =
{
    {MODULE_CEU,       &CPG.SWRSTCR2.BYTE, CPG_SWRSTCR2_SRST21},
    {MODULE_JCU,       &CPG.SWRSTCR1.BYTE, CPG_SWRSTCR1_SRST11},
    {MODULE_VIN,       &CPG.SWRSTCR2.BYTE, CPG_SWRSTCR2_SRST25},
    {MODULE_IMR2,      &CPG.SWRSTCR2.BYTE, CPG_SWRSTCR2_SRST24},
    {MODULE_DRW,       &CPG.SWRSTCR2.BYTE, CPG_SWRSTCR2_SRST22},
    {MODULE_MIPI,      &CPG.SWRSTCR2.BYTE, CPG_SWRSTCR2_SRST23},
    {MODULE_SPIBSC,    &CPG.SWRSTCR1.BYTE, CPG_SWRSTCR1_SRST10},
    {MODULE_VDC6,      &CPG.SWRSTCR1.BYTE, CPG_SWRSTCR1_SRST13},
    {MODULE_TSIP,      &CPG.SWRSTCR1.BYTE, CPG_SWRSTCR1_SRST12},
    {MODULE_NAND,      &CPG.SWRSTCR2.BYTE, CPG_SWRSTCR2_SRST26},

};

/**
 * @fn         get_stbcr_info
 * @brief      Get STBCR register information for specified module
 * @param[in]  module: module to check
 * @return     Pointer for register information or NULL if invalid module
 */
static const st_r_prv_reg_info_t * get_stbcr_info(e_stb_module_t module)
{
    size_t index;
    size_t count = (sizeof gs_stbcr) / (sizeof gs_stbcr[0]);

    for ( index = 0; index < count; index++ )
    {
        if ( module == gs_stbcr[index].module )
        {
            return &gs_stbcr[index];
        }
    }

    /* return NULL if not found */
    return NULL;
}
/******************************************************************************
 * End of function get_stbcr_info
 ******************************************************************************/

/**
 * @fn         get_stbreq_info
 * @brief      Get STBREQ register information for specified module
 * @param[in]  module: module to check
 * @return     Pointer for register information or NULL if invalid module
 */
static const st_r_prv_reg_info_t * get_stbreq_info(e_stb_module_t module)
{
    size_t index;
    size_t count = (sizeof gs_stbreq) / (sizeof gs_stbreq[0]);

    for ( index = 0; index < count; index++ )
    {
        if ( module == gs_stbreq[index].module )
        {
            return &gs_stbreq[index];
        }
    }

    /* return NULL if not found */
    return NULL;
}
/******************************************************************************
 * End of function get_stbreq_info
 ******************************************************************************/

/**
 * @fn         get_stback_info
 * @brief      Get STBACK register information for specified module
 * @param[in]  module: module to check
 * @return     Pointer for register information or NULL if invalid module
 */
static const st_r_prv_reg_info_t * get_stback_info(e_stb_module_t module)
{
    size_t index;
    size_t count = (sizeof gs_stback) / (sizeof gs_stback[0]);

    for ( index = 0; index < count; index++ )
    {
        if ( module == gs_stback[index].module )
        {
            return &gs_stback[index];
        }
    }

    /* return NULL if not found */
    return NULL;
}
/******************************************************************************
 * End of function get_stback_info
 ******************************************************************************/

/**
 * @fn         get_swrstcr_info
 * @brief      Get SWRSTCR register information for specified module
 * @param[in]  module: module to check
 * @return     Pointer for register information or NULL if invalid module
 */
static const st_r_prv_reg_info_t * get_swrstcr_info(e_stb_module_t module)
{
    size_t index;
    size_t count = (sizeof gs_swrstcr) / (sizeof gs_swrstcr[0]);

    for ( index = 0; index < count; index++ )
    {
        if ( module == gs_swrstcr[index].module )
        {
            return &gs_swrstcr[index];
        }
    }

    /* return NULL if not found */
    return NULL;
}
/******************************************************************************
 * End of function get_swrstcr_info
 ******************************************************************************/

/**
 * @fn         is_clock_supplied
 * @brief      Check if clock supplied for specified module
 * @param[in]  module: module to check
 * @retval     true Clock is supplied
 * @retval     false Clock not suppliked
 */
static bool_t is_clock_supplied(e_stb_module_t module)
{
    bool_t result;
    const st_r_prv_reg_info_t * p_stbcr;
    uint8_t value;

    /* initialise as clock is not supplied */
    result = false;

    /* obtain the STBCR register info for specified module */
    p_stbcr = get_stbcr_info(module);
    if ( NULL != p_stbcr )
    {
        value = RZA_IO_RegRead_8(p_stbcr->reg_address, IOREG_NONSHIFT_ACCESS, p_stbcr->mask);
        if ( 0 == value )
        {
            /* return true if all related STBCR bit is zero */
            result = true;
        }
    }

    return result;
}
/******************************************************************************
 * End of function is_clock_supplied
 ******************************************************************************/

/**
 * @fn         R_STB_RequestModuleStart
 * @brief      Cleanup to start the module after supplying clock
 * @param[in]  module: module number
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 * @retval     STB_INVALID_STATE     Invalid state for operation
 * @retval     STB_AGAIN             This operation was not completed
 */
e_stb_err_t R_STB_RequestModuleStart( e_stb_module_t module )
{
    e_stb_err_t ret;
    const st_r_prv_reg_info_t * p_stbreq;
    const st_r_prv_reg_info_t * p_stback;
    uint8_t value;

    /* Initialise error state */
    ret = STB_SUCCESS;

    /* obtain the STBREQ and the STBACK information */
    p_stbreq = get_stbreq_info(module);
    p_stback = get_stback_info(module);

    /* check if unsupported module */
    if ( (NULL == p_stback) || (NULL == p_stbreq) )
    {
        ret = STB_INVALID_MODULE;
    }

    /* check if clock supplied or not */
    if ( STB_SUCCESS == ret )
    {
        if ( !is_clock_supplied(module) )
        {
            /* if clock does not supplied, the module can not start. */
            ret = STB_INVALID_STATE;
        }
    }

    if ( STB_SUCCESS == ret )
    {
        /* turned off standby request bit */
        RZA_IO_RegWrite_8(p_stbreq->reg_address, 0, IOREG_NONSHIFT_ACCESS, p_stbreq->mask);

        /* dummy read */
        RZA_IO_RegRead_8(p_stbreq->reg_address, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

        /* read start completion */
        value = RZA_IO_RegRead_8(p_stback->reg_address, IOREG_NONSHIFT_ACCESS, p_stback->mask);
        if ( 0 != value )
        {
            /* If at least one related bits still 1, start not completed.
             * In this case, return STB_AGAIN.
             */
            ret = STB_AGAIN;
        }
    }

    return ret;
}
/******************************************************************************
 * End of function R_STB_RequestModuleStart
 ******************************************************************************/

/**
 * @fn         R_STB_RequestModuleStop
 * @brief      Prepare to stop the module
 * @param[in]  module: module number
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 * @retval     STB_INVALID_STATE     Invalid state for operation
 * @retval     STB_AGAIN             This operation was not completed
 */
e_stb_err_t  R_STB_RequestModuleStop( e_stb_module_t module )
{
    e_stb_err_t ret;
    const st_r_prv_reg_info_t * p_stbreq;
    const st_r_prv_reg_info_t * p_stback;
    uint8_t value;

    /* Initialise error state */
    ret = STB_SUCCESS;

    /* obtain the STBREQ and the STBACK information */
    p_stbreq = get_stbreq_info(module);
    p_stback = get_stback_info(module);

    /* check if unsupported module */
    if ( (NULL == p_stback) || (NULL == p_stbreq) )
    {
        ret = STB_INVALID_MODULE;
    }

    /* check if clock supplied or not */
    if ( STB_SUCCESS == ret )
    {
        if ( !is_clock_supplied(module) )
        {
            /* if clock does not supplied, the module can not start. */
            ret = STB_INVALID_STATE;
        }
    }

    if ( STB_SUCCESS == ret )
    {
        /* turned on standby request bit */
        RZA_IO_RegWrite_8(p_stbreq->reg_address, p_stbreq->mask, IOREG_NONSHIFT_ACCESS, p_stbreq->mask);

        /* dummy read */
        RZA_IO_RegRead_8(p_stbreq->reg_address, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

        /* read stop completion */
        value = RZA_IO_RegRead_8(p_stback->reg_address, IOREG_NONSHIFT_ACCESS, p_stback->mask);
        if ( p_stback->mask != value )
        {
            /* If at least one related bits still 0, stop not completed.
             * In this case, return STB_AGAIN.
             */
            ret = STB_AGAIN;
        }
    }

    return ret;
}
/******************************************************************************
 * End of function R_STB_RequestModuleStart
 ******************************************************************************/

/**
 * @fn         R_STB_StopModule
 * @brief      Stops the module
 * @param[in]  module: module number
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 */
e_stb_err_t R_STB_StopModule( e_stb_module_t module )
{
    e_stb_err_t ret;
    const st_r_prv_reg_info_t * p_stbcr;

    /* Initialise error state */
    ret = STB_SUCCESS;

    /* obtain the STBCR information */
    p_stbcr = get_stbcr_info(module);

    /* check if unsupported module */
    if ( (NULL == p_stbcr) )
    {
        ret = STB_INVALID_MODULE;
    }

    if ( STB_SUCCESS == ret )
    {
        /* set module standby */
        RZA_IO_RegWrite_8(p_stbcr->reg_address, p_stbcr->mask, IOREG_NONSHIFT_ACCESS, p_stbcr->mask);

        /* dummy read */
        RZA_IO_RegRead_8(p_stbcr->reg_address, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    return ret;
}
/******************************************************************************
 * End of function R_STB_StopModule
 ******************************************************************************/

/**
 * @fn         R_STB_StartModule
 * @brief      Starts the module
 * @param[in]  module: module number
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 */
e_stb_err_t R_STB_StartModule( e_stb_module_t module )
{
    e_stb_err_t ret;
    const st_r_prv_reg_info_t * p_stbcr;

    /* Initialise error state */
    ret = STB_SUCCESS;

    /* obtain the STBCR information */
    p_stbcr = get_stbcr_info(module);

    /* check if unsupported module */
    if ( (NULL == p_stbcr) )
    {
        ret = STB_INVALID_MODULE;
    }

    if ( STB_SUCCESS == ret )
    {
        /* clear module standby */
        RZA_IO_RegWrite_8(p_stbcr->reg_address, 0, IOREG_NONSHIFT_ACCESS, p_stbcr->mask);

        /* dummy read */
        RZA_IO_RegRead_8(p_stbcr->reg_address, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    return ret;
}
/******************************************************************************
 * End of function R_STB_StartModule
 ******************************************************************************/

/**
 * @fn         R_STB_SetModuleResetState
 * @brief      Set module reset state
 * @note       this API also run the process described in the section 52.3.6
 *             of User's Manual.
 * @param[in]  module: module number
 * @param[in]  state: reset state
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 * @retval     STB_INVALID_STATE     Invalid state for operation
 */
e_stb_err_t R_STB_SetModuleResetState( e_stb_module_t module, e_stb_reset_state_t state )
{
    e_stb_err_t ret;
    const st_r_prv_reg_info_t * p_swrstcr;

    /* Initialise error state */
    ret = STB_SUCCESS;

    /* obtain the SWRSTCR information */
    p_swrstcr = get_swrstcr_info(module);

    /* check if unsupported module */
    if ( (NULL == p_swrstcr) )
    {
        ret = STB_INVALID_MODULE;
    }

    /* check if clock supplied */
    if ( (STB_SUCCESS == ret) && (!is_clock_supplied(module)) )
    {
        ret = STB_INVALID_STATE;
    }

    if ( STB_SUCCESS == ret )
    {
        if ( MODULE_RESET_STATE == state )
        {
            /* needs module stop request if state wants to be reset */
            /* wait while returns STB_AGAIN */
            do
            {
                ret = R_STB_RequestModuleStop(module);
            } while ( STB_AGAIN == ret );

            /*
             * check if the module successfully stop or the module does not support module stop */
            if ( (STB_INVALID_MODULE == ret) || (STB_SUCCESS == ret) )
            {
                /* set the module in reset state */
                RZA_IO_RegWrite_8(p_swrstcr->reg_address, p_swrstcr->mask, IOREG_NONSHIFT_ACCESS, p_swrstcr->mask);

                /* overwrite the return code as success */
                ret = STB_SUCCESS;
            }
        }
        else
        {
            /* turned off reset bit */
            RZA_IO_RegWrite_8(p_swrstcr->reg_address, 0, IOREG_NONSHIFT_ACCESS, p_swrstcr->mask);

            /* needs module stop request if state wants to be "not reset" */
            /* wait while returns STB_AGAIN */
            do
            {
                ret = R_STB_RequestModuleStart(module);
            } while ( STB_AGAIN == ret );

            /*
             * check if the module does not support module stop */
            if ( STB_INVALID_MODULE == ret )
            {
                /* overwrite the return code as success */
                ret = STB_SUCCESS;
            }
        }

        /* dummy read the SWRSTCR register */
        RZA_IO_RegRead_8(p_swrstcr->reg_address, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    return ret;
}
/******************************************************************************
 * End of function R_STB_SetModuleResetState
 ******************************************************************************/

/**
 * @fn         R_STB_GetModuleResetState
 * @brief      Get module reset state
 * @param[in]  module: module number
 * @param[out] p_state: reset state
 * @retval     STB_SUCCESS           Successful operation.
 * @retval     STB_INVALID_MODULE    Invalid module number or does not supported
 * @retval     STB_INVALID_STATE     Invalid state for operation
 */
e_stb_err_t R_STB_GetModuleResetState( e_stb_module_t module, e_stb_reset_state_t * p_state )
{
    e_stb_err_t ret;
    const st_r_prv_reg_info_t * p_swrstcr;
    uint8_t current_bit;

    /* Initialise error state */
    ret = STB_SUCCESS;

    /* obtain the SWRSTCR information */
    p_swrstcr = get_swrstcr_info(module);

    /* check if unsupported module */
    if ( (NULL == p_swrstcr) )
    {
        ret = STB_INVALID_MODULE;
    }

    /* check if clock supplied */
    if ( (STB_SUCCESS == ret) && (!is_clock_supplied(module)) )
    {
        ret = STB_INVALID_STATE;
    }

    if ( STB_SUCCESS == ret )
    {
        /* get current reset state */
        current_bit = RZA_IO_RegRead_8(p_swrstcr->reg_address, IOREG_NONSHIFT_ACCESS, p_swrstcr->mask);

        /* check if all bit listed in swrstcr_bit is zero */
        if ( 0 == current_bit )
        {
            /* store MODULE_NOT_RESET_STATE to p_state */
            *p_state = MODULE_NOT_RESET_STATE;
        }
        else
        {
            /* at least one bit is 1, it is still in the reset state */
            *p_state = MODULE_RESET_STATE;
        }
    }

    return ret;
}
/******************************************************************************
 * End of function R_STB_GetModuleResetState
 ******************************************************************************/

/**
 * @fn         R_STB_GetVersion
 * @brief      Get the Low Layer Driver specific version
 * @param[in]  p_ver_info: detailed driver information structure
 * @retval     STB_SUCCESS           Successful operation (always).
 */
int_t R_STB_GetVersion(st_ver_info_t *p_ver_info)
{
    p_ver_info->lld.p_szdriver_name = gs_lld_info.p_szdriver_name;
    p_ver_info->lld.version.sub.major = gs_lld_info.version.sub.major;
    p_ver_info->lld.version.sub.minor = gs_lld_info.version.sub.minor;
    p_ver_info->lld.build = gs_lld_info.build;

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of function R_STB_GetVersion
 ******************************************************************************/

/* End of File */
