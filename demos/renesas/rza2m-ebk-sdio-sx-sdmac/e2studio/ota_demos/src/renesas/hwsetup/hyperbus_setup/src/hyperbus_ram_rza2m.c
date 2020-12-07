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
 * File Name    : hyperbus_ram_rza2m.c
 * Version      : 1.0
 * Description  : API function of HyperRAM
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
#include "hyperbus_ram_rza2m.h"

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
 * Function Name: HyperRAM_ReadID0
 * Description  : Read ID0 register of HyperRAM
 * Arguments    : baddr        : Base address of HyperRAM
 * Return Value : ID0 value
 *********************************************************************************************************************/
uint16_t HyperRAM_ReadID0(uint32_t baddr)
{
    uint16_t read_id0;

    /** Set to register base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_REGISTER_SPACE);
    
    /** Read ID0 register */
    read_id0 = *(volatile uint16_t *)(baddr);

    /** Set to memory base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_MEMORY_SPACE);

    return read_id0;
}
/**********************************************************************************************************************
 * End of function HyperRAM_ReadID0
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperRAM_ReadID1
 * Description  : Read ID1 register of HyperRAM
 * Arguments    : baddr        : Base address of HyperRAM
 * Return Value : ID1 value
 *********************************************************************************************************************/
uint16_t HyperRAM_ReadID1(uint32_t baddr)
{
    uint16_t read_id1;

    /** Set to register base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_REGISTER_SPACE);

    /** Read ID1 register */
    read_id1 = *(volatile uint16_t *)(baddr + 0x2);

    /** Set to memory base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_MEMORY_SPACE);

    return read_id1;
}
/**********************************************************************************************************************
 * End of function HyperRAM_ReadID1
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperRAM_ReadCR0
 * Description  : Read CR0 register of HyperRAM
 * Arguments    : baddr        : Base address of HyperRAM
 * Return Value : CR0 value
 *********************************************************************************************************************/
uint16_t HyperRAM_ReadCR0(uint32_t baddr)
{
    uint16_t read_cr0;

    /** Set to register base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_REGISTER_SPACE);

    /** Read CR0 register
        bit 3222 2222 2221 1111 1111 1000 0000 000
            0987 6543 2109 8765 4321 0987 6543 210
        val 0000 0000 0000 0000 0001 0000 0000 0000 */
    read_cr0 = *(volatile uint16_t *)(baddr + 0x00001000);

    /** Set to memory base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_MEMORY_SPACE);

    return read_cr0;
}
/**********************************************************************************************************************
 * End of function HyperRAM_ReadCR0
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperRAM_WriteCR0
 * Description  : Write CR0 register of HyperRAM
 * Arguments    : baddr        : Base address of HyperRAM
 *              : wdata       : write data to CR0
 * Return Value : DRV_SUCCESS : Successful operation (always)
 *********************************************************************************************************************/
int_t HyperRAM_WriteCR0(uint32_t baddr, uint16_t wdata)
{

    /** Set to register base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_REGISTER_SPACE);

    /** Write CR0 register
        bit 3222 2222 2221 1111 1111 1000 0000 000
            0987 6543 2109 8765 4321 0987 6543 210
        val 0000 0000 0000 0000 0001 0000 0000 0000 */
    *(volatile uint16_t *)(baddr + 0x00001000) = wdata;

    /** Set to memory base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_MEMORY_SPACE);

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of function HyperRAM_WriteCR0
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperRAM_ReadCR1
 * Description  : Read CR1 register of HyperRAM
 * Arguments    : baddr        : Base address of HyperRAM
 * Return Value : CR1 value
 *********************************************************************************************************************/
uint16_t HyperRAM_ReadCR1(uint32_t baddr)
{
    uint16_t read_cr1;

    /** Set to register base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_REGISTER_SPACE);

    /** Read CR1 register
        bit 3222 2222 2221 1111 1111 1000 0000 000
            0987 6543 2109 8765 4321 0987 6543 210
        val 0000 0000 0000 0000 0001 0000 0000 0010 */
    read_cr1 = *(volatile uint16_t *)(baddr + 0x00001002);

    /** Set to memory base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_MEMORY_SPACE);

    return read_cr1;
}
/**********************************************************************************************************************
 * End of function HyperRAM_ReadCR1
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: HyperRAM_WriteCR1
 * Description  : Write CR1 register of HyperRAM
 * Arguments    : baddr       : Base address of HyperRAM
 *              : wdata       : write data to CR1
 * Return Value : DRV_SUCCESS : Successful operation (always)
 *********************************************************************************************************************/
int_t HyperRAM_WriteCR1(uint32_t baddr, uint16_t wdata)
{

    /** Set to register base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_REGISTER_SPACE);

    /** Write CR1 register
        bit 3222 2222 2221 1111 1111 1000 0000 000
            0987 6543 2109 8765 4321 0987 6543 210
        val 0000 0000 0000 0000 0001 0000 0000 0010 */
    *(volatile uint16_t *)(baddr + 0x00001002) = wdata;

    /** Set to memory base access */
    R_HYPERBUS_SelectSpace(HYPERBUS_CS1_AREA, HYPERBUS_MEMORY_SPACE);

    return DRV_SUCCESS;
}
/**********************************************************************************************************************
 * End of function HyperRAM_WriteCR1
 *********************************************************************************************************************/
