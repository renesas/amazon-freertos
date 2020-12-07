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
* File Name : r_mmu_lld_rza2m.c
* Description : Memory Management Unit Low Layer Driver
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_mmu_drv_link.h"
#include "r_mmu_drv_sc_cfg.h"
#include "r_compiler_abstraction_api.h"

/******************************************************************************
Private global variables and functions
******************************************************************************/
/*! Version Information */
static const st_drv_info_t gs_lld_info =
{
    {
        ((STDIO_MMU_RZ_LLD_VERSION_MAJOR << 16) + STDIO_MMU_RZ_LLD_VERSION_MINOR)
    },
    STDIO_MMU_RZ_LLD_BUILD_NUM,
    STDIO_MMU_RZ_LLD_DRV_NAME
};

static uint32_t * mmu_get_ttb_entry(uint32_t address);


/******************************************************************************
* Function Name: R_MMU_WriteTbl
* Description  : Write MMU entry data
* Arguments    : uint32_t   vaddress  ; I : virtual address to be mapped
*              : uint32_t   paddress  ; I : physical address to be mapped
*              : uint32_t   size      ; I : size of 1M sections
*              : uint32_t   entry     ; I : initial data of fill pattern
* Return Value : MMU_SUCCESS      : successful
*                MMU_ERR_OVERFLOW : overflow of virtual or physical area
******************************************************************************/
e_mmu_err_t R_MMU_WriteTbl(uint32_t vaddress, uint32_t paddress, uint32_t size, uint32_t entry)
{
    uint32_t * p_ttb;
    uint32_t dataindex;

    /* virtual address space check */
    dataindex = (vaddress >> 20) + size;
    if (dataindex > 4096)
    {
        return (MMU_ERR_OVERFLOW);
    }

    /* physical address space check */
    dataindex = (paddress >> 20) + size;
    if (dataindex > 4096)
    {
        return (MMU_ERR_OVERFLOW);
    }

    p_ttb = mmu_get_ttb_entry(vaddress);
    entry = (entry & 0x000FFFFFuL) | (paddress & 0xFFF00000uL);

    while (0 != size)
    {
        *p_ttb = entry;
        p_ttb++;
        entry += 0x00100000uL;
        size--;
    }

    return (MMU_SUCCESS);
}
/******************************************************************************
 * End of function R_MMU_WriteTbl
 ******************************************************************************/

/******************************************************************************
* Function Name: R_MMU_ReadTbl
* Description  : Read MMU entry data
* Arguments    : uint32_t   address  ; I : virtual address to be read
* Return Value : MMU table entry value
******************************************************************************/
uint32_t R_MMU_ReadTbl(uint32_t address)
{
    uint32_t * p_ttb;
    p_ttb = mmu_get_ttb_entry(address);

    return *p_ttb;
}
/******************************************************************************
 * End of function R_MMU_ReadTbl
 ******************************************************************************/

/******************************************************************************
* Function Name: mmu_get_ttb_entry
* Description  : Get TTB entry address of specified address
* Arguments    : uint32_t   address   ; I : query address
* Return Value : TTB entry address
******************************************************************************/
static uint32_t * mmu_get_ttb_entry(uint32_t address)
{
    uint32_t ttbcr;
    uint32_t ttbr;
    uint32_t mask;
    uint32_t res;

    ttbcr = r_mmu_get_ttbcr();
    ttbr = r_mmu_get_ttbr0();
    mask = (0xFFFFC000uL) << (ttbcr&7);
    res = (ttbr & mask) + ((address >> 18) & 0xFFFFFFFCuL);

    /* Cast to an appropriate type */
    return ((uint32_t *) res);
}
/******************************************************************************
 * End of function mmu_get_ttb_entry
 ******************************************************************************/

/******************************************************************************
* Function Name: R_MMU_Init
* Description  : Initialize MMU register
* Arguments    : none
* Return Value : none
******************************************************************************/
void R_MMU_Init(void)
{
    /* ttbr0:RGN =b01 : outer cacheable Write-back cached, Write-allocate
     * ttbr0:S   = b0 : translation table walk to non-shared memory
     * ttbr0:IRGN=b01 : inner cacheability for the translation table walk is Write-back Write-allocate
     */
    uint32_t ttbr0 = (uint32_t) & (__mmu_page_table_base) | 0x48;

    int_t index;
    int_t count;
    int_t mmu_error;

    R_MMU_Disable();

    /* All area use TTBR0 entry */
    r_mmu_set_ttbcr(0);

    /* Set TTBR0 */
    r_mmu_set_ttbr0(ttbr0);

    /* All domain sets to client */
    r_mmu_set_dacr(0x55555555);

    /* Create MMU table from SC-configured table  */
    count = (sizeof(MMU_SC_TABLE)) / (sizeof(MMU_SC_TABLE[0]));
    for (index = 0; index < count; index++)
    {
        mmu_error = R_MMU_WriteTbl(
                MMU_SC_TABLE[index].virtual_address,
                MMU_SC_TABLE[index].physical_address,
                MMU_SC_TABLE[index].page_count,
                MMU_SC_TABLE[index].page_attribute);

        if (MMU_SUCCESS != mmu_error)
        {
            while (1)
            {
                /* Spin here forever.. */
            	R_COMPILER_Nop();
            }
        }
    }
}
/******************************************************************************
 * End of function R_MMU_Init
 ******************************************************************************/

/******************************************************************************
* Function Name: R_MMU_Disable
* Description  : Disable MMU
* Arguments    : none
* Return Value : none
******************************************************************************/
void R_MMU_Disable(void)
{
    uint32_t sctlr;
    sctlr = r_mmu_get_sctlr();
    sctlr &= (~(0x0001uL));
    r_mmu_set_sctlr(sctlr);
}
/******************************************************************************
 * End of function R_MMU_Disable
 ******************************************************************************/

/******************************************************************************
* Function Name: R_MMU_Enable
* Description  : Enable MMU
* Arguments    : none
* Return Value : none
******************************************************************************/
void R_MMU_Enable(void)
{
    uint32_t sctlr;
    sctlr = r_mmu_get_sctlr();
    sctlr |= 0x0001uL;
    r_mmu_set_sctlr(sctlr);
}
/******************************************************************************
 * End of function R_MMU_Enable
 ******************************************************************************/

/******************************************************************************
* Function Name: R_MMU_VAtoPA
* Description  : Convert virtual address to physical address
* Arguments    : uint32_t   vaddress  ; I : virtual address to be convert
*              : uint32_t * paddress  ; O : physical address
* Return Value : MMU_SUCCESS         : successful
*                MMU_ERR_TRANSLATION : translation error
******************************************************************************/
e_mmu_err_t R_MMU_VAtoPA(uint32_t vaddress, uint32_t * paddress)
{
    uint32_t ttb;
    e_mmu_err_t err;

    ttb = R_MMU_ReadTbl(vaddress);
    if ((ttb & (0x40000 | 0x3)) == 2)
    {
        *paddress = (ttb & 0xfff00000uL) | (vaddress & 0xfffff);
        err = MMU_SUCCESS;
    }
    else
    {
        err = MMU_ERR_TRANSLATION;
    }

    return (err);
}
/******************************************************************************
 * End of function R_MMU_VAtoPA
 ******************************************************************************/

/******************************************************************************
* Function Name: R_MMU_TLBIALL
* Description  : execute TLBIALL
* Arguments    : none
* Return Value : none
******************************************************************************/
void R_MMU_TLBIALL( void )
{
    r_mmu_tlbiall();
}
/******************************************************************************
 * End of function R_MMU_TLBIALL
 ******************************************************************************/

/******************************************************************************
* Function Name: R_MMU_TLBIMVAA
* Description  : execute TLBIMVAA
* Arguments    : vaddress: MVA to be invalidate TLB entry
* Return Value : none
******************************************************************************/
void R_MMU_TLBIMVAA(uint32_t vaddress)
{
    r_mmu_tlbimvaa(vaddress);
}
/******************************************************************************
 * End of function R_MMU_TLBIMVAA
 ******************************************************************************/

/**
 * @fn             R_MMU_GetVersion
 * @brief          Obtains driver specific version information which is used for
 *                 reporting
 * @param[out]     p_ver_info: driver version information is populated into
 *                               this structure
 *                               @note the structure resources are created by
 *                                     the application not this function.
 * @retval         DRV_SUCCESS  Successful operation (always)
 */
int_t R_MMU_GetVersion(st_ver_info_t *p_ver_info)
{
    p_ver_info->lld.p_szdriver_name = gs_lld_info.p_szdriver_name;
    p_ver_info->lld.version.sub.major = gs_lld_info.version.sub.major;
    p_ver_info->lld.version.sub.minor = gs_lld_info.version.sub.minor;
    p_ver_info->lld.build = gs_lld_info.build;

    return (DRV_SUCCESS);
}
/******************************************************************************
 * End of function R_MMU_GetVersion
 ******************************************************************************/

/* End of File */
