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
 * File Name    : hyperbus_flash_rza2m.c
 * Version      : 1.0
 * Description  : API function of HyperFlash Control
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2019 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "r_typedefs.h"
#include "driver.h"

#include "r_hyperbus_lld_rza2m_api.h"
#include "hyperbus_flash_rza2m.h"

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

static void hyperflash_commandwrite(uint32_t baddr, uint32_t caddr, uint16_t write_value);
static void hyperflash_datawrite(uint32_t baddr, uint32_t offset, uint16_t write_value);
static uint16_t hyperflash_responseread(uint32_t baddr);
static uint16_t hyperflash_readstatus(uint32_t baddr);

/**********************************************************************************************************************
 * Function Name: hyperflash_commandwrite
 * Description  : Command Write to HyperFlash
 * Arguments    : baddr       : Base address of HyperFlash
 *              : caddr       : command address
 *              : write_value : Write value for the IO register
 * Return Value : None
 *********************************************************************************************************************/
static void hyperflash_commandwrite(uint32_t baddr, uint32_t caddr, uint16_t write_value)
{
    /* 16 bits command write to HyperFlash */
    *(volatile uint16_t *)(baddr + (caddr << 1)) = write_value;
}
/**********************************************************************************************************************
 * End of function hyperflash_commandwrite
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: hyperflash_datawrite
 * Description  : Data Write to HyperFlash
 * Arguments    : baddr       : Base address of HyperFlash
 *              : offset      : offset address
 *              : write_value : Write value for the IO register
 * Return Value : None
 *********************************************************************************************************************/
static void hyperflash_datawrite(uint32_t baddr, uint32_t offset, uint16_t write_value)
{
    /* Write 16-bit data to HyperFlash */
    *(volatile uint16_t *)(baddr + offset) = write_value;
}
/**********************************************************************************************************************
 * End of function hyperflash_datawrite
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: hyperflash_responseread
 * Description  : Read from HyperFlash on command sequence
 * Arguments    : baddr     : response read address
 * Return Value : read data
 *********************************************************************************************************************/
static uint16_t hyperflash_responseread(uint32_t baddr)
{
    /* Read 16 bits data from HyperFlash */
    return *(volatile uint16_t *)(baddr);
}
/**********************************************************************************************************************
 * End of function hyperflash_responseread
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: hyperflash_readstatus
 * Description  : Read SR of HyperFlash
 * Arguments    : baddr : response read address
 * Return Value : SR value
 *********************************************************************************************************************/
static uint16_t hyperflash_readstatus(uint32_t baddr)
{
    uint16_t read_sr;
    
    hyperflash_commandwrite(baddr, 0x555, 0x0070);                     /** 1st bus cycle */
    read_sr = hyperflash_responseread(baddr); /** 2nd bus cycle */

    return read_sr;
}
/**********************************************************************************************************************
 * End of function hyperflash_readstatus
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperFlash_WriteWord
 * Description  : Write 16 bits data to HyperFlash
 * Arguments    : baddr       : Base address of HyperFlash
 *              : waddr       : Write address
 *              : wdata       : write data
 * Return Value : DRV_SUCCESS : Successful operation (always)
 *********************************************************************************************************************/
uint16_t HyperFlash_WriteWord(uint32_t baddr, uint32_t waddr, uint16_t wdata)
{
    uint16_t read_sr;

    /** Word program sequence */
    hyperflash_commandwrite(baddr, 0x555, 0x00aa); /** 1st bus cycle */
    hyperflash_commandwrite(baddr, 0x2aa, 0x0055); /** 2nd bus cycle */
    hyperflash_commandwrite(baddr, 0x555, 0x00a0); /** 3rd bus cycle */
    hyperflash_datawrite(baddr, waddr, wdata);     /** 4th bus cycle */

    /** Wait for device ready asserted */
    while (1)
    {
        /** Read status register */
        read_sr = hyperflash_readstatus(baddr);
        if ( 0 != (read_sr & 0x80))
        {
            break;
        }
    }

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of function HyperFlash_WriteWord
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperFlash_EraseSect
 * Description  : Sector erase of HyperFlash
 * Arguments    : baddr       : Base address of HyperFlash
 *              : saddr       : Sector address
 * Return Value : DRV_SUCCESS  Successful operation
 *              : DRV_ERROR    Failure operation
 *********************************************************************************************************************/
int_t HyperFlash_EraseSect(uint32_t baddr, uint32_t saddr)
{
    int_t err = DRV_SUCCESS;
    uint16_t read_sr;

    /** Word program sequence */
    hyperflash_commandwrite(baddr, 0x555, 0x00aa);  /** 1st bus cycle */
    hyperflash_commandwrite(baddr, 0x2aa, 0x0055);  /** 2nd bus cycle */

    hyperflash_commandwrite(baddr, 0x555, 0x0080);  /** 3rd bus cycle */
    hyperflash_commandwrite(baddr, 0x555, 0x00aa);  /** 4th bus cycle */
    hyperflash_commandwrite(baddr, 0x2aa, 0x0055);  /** 5th bus cycle */
    hyperflash_datawrite(baddr, saddr, 0x0030);     /** 6th bus cycle */

    /** Wait for device ready asserted */
    while (1)
    {
        /** Read status register */
        read_sr = hyperflash_readstatus(baddr);
        if ( 0 != (read_sr & 0x80))
        {
            break;
        }
    }

    /** Evaluate erase status */
    hyperflash_commandwrite(baddr, 0x555, 0x00d0);  /** 1st bus cycle */
    
    /** Wait for device ready asserted */
    while (1)
    {
        /** Read status register */
        read_sr = hyperflash_readstatus(baddr);
        if ( 0 != (read_sr & 0x80))
        {
            break;
        }
    }
    
    /** Sector erase error check */
    if ( 0 == (read_sr & 0x01))
    {
        err = DRV_ERROR; /** erase error occured */
    }

    return err;
}
/**********************************************************************************************************************
 * End of function HyperFlash_EraseSect
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperFlash_ReadVCR
 * Description  : Read VCR of HyperFlash
 * Arguments    : baddr       : Base address of HyperFlash
 * Return Value : VCR value
 *********************************************************************************************************************/
uint16_t HyperFlash_ReadVCR(uint32_t baddr)
{
    uint16_t read_vcr;

    hyperflash_commandwrite(baddr, 0x555, 0x00aa);    /** 1st bus cycle */
    hyperflash_commandwrite(baddr, 0x2aa, 0x0055);    /** 2nd bus cycle */
    hyperflash_commandwrite(baddr, 0x555, 0x00c7);    /** 3rd bus cycle */
    read_vcr = hyperflash_responseread(baddr); /** 4th bus cycle */

    return read_vcr;
}
/**********************************************************************************************************************
 * End of function HyperFlash_ReadVCR
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperFlash_WriteVCR
 * Description  : Write VCR of HyperFlash
 * Arguments    : baddr       : Base address of HyperFlash
 *              : wdata       : Write data
 * Return Value : DRV_SUCCESS   Successful operation (always)
 *********************************************************************************************************************/
int_t HyperFlash_WriteVCR(uint32_t baddr, uint16_t wdata)
{
    uint16_t read_sr;

    hyperflash_commandwrite(baddr, 0x555, 0x00aa); /** 1st bus cycle */
    hyperflash_commandwrite(baddr, 0x2aa, 0x0055); /** 2nd bus cycle */
    hyperflash_commandwrite(baddr, 0x555, 0x0038); /** 3rd bus cycle */
    hyperflash_commandwrite(baddr, 0x000, wdata);  /** 4th bus cycle */

    /** Wait for device ready asserted */
    while (1)
    {
        /** Read status register */
        read_sr = hyperflash_readstatus(baddr);
        if ( 0 != (read_sr & 0x80))
        {
            break;
        }
    }

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of function HyperFlash_WriteVCR
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperFlash_ReadROMInfo
 * Description  : Read Device ID and CFI of HyperFlash
 * Arguments    : baddr       : Base address of HyperFlash
 *              : idbuf       : Read buffer pointer
 * Return Value : None
 *********************************************************************************************************************/
void HyperFlash_ReadROMInfo(uint32_t baddr, uint16_t *idbuf)
{

    uint32_t i;

    /** CFI entry */
    hyperflash_commandwrite(baddr, 0x055, 0x0098); /** 1st bus cycle */

    for(i = 0; i < 256; i++)
    {
        idbuf[i] = hyperflash_responseread(baddr + (i << 1));
    }

    /** CFI exit */
    hyperflash_commandwrite(baddr, 0x000, 0x00f0); /** 1st bus cycle */

}
/**********************************************************************************************************************
 * End of function HyperFlash_ReadROMInfo
 *********************************************************************************************************************/
