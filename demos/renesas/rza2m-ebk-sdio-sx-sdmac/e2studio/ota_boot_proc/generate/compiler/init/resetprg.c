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
 * File Name    : resetprg.c
 * Device(s)    : RZ/A2M
 * Tool-Chain   : e2Studio Ver 7.5.0
 *              : GNU Arm Embedded Toolchain 6-2017-q2-update
 * OS           : None
 * H/W Platform : RZ/A2M Evaluation Board
 * Description  : RZ/A2M Sample Program - C library entry point
 *              : Variants of this file must be created for each compiler
 * Operation    :
 * Limitations  :
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "compiler_settings.h"
#include "r_compiler_abstraction_api.h"
#include "r_cache_lld_rza2m.h"
#include "r_intc_drv_api.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "rza_io_regrw.h"
#include "r_startup_config.h"
#include "resetprg.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define R_PRV_RTC_COUNT (2)
#define R_PRV_USB_COUNT (2)

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
extern uint32_t __vector_mirror_table_base;

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static void disable_rtc(uint32_t ch);
static void disable_usb(uint32_t ch);

/**********************************************************************************************************************
 * Function Name: resetprg
 * Description  : Program entry point after basic peripheral init.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void resetprg (void)
{
    /* Disable RTC if macro defined as 1 */
#if defined(STARTUP_CFG_DISABLE_RTC0) && (STARTUP_CFG_DISABLE_RTC0==1)
    disable_rtc(0);
#endif
#if defined(STARTUP_CFG_DISABLE_RTC1) && (STARTUP_CFG_DISABLE_RTC1==1)
    disable_rtc(1);
#endif

    /* Disable USB if macro defined as 1 */
#if defined(STARTUP_CFG_DISABLE_USB0) && (STARTUP_CFG_DISABLE_USB0==1)
    disable_usb(0);
#endif
#if defined(STARTUP_CFG_DISABLE_USB1) && (STARTUP_CFG_DISABLE_USB1==1)
    disable_usb(1);
#endif

    /*    Initialize INTC,
     *    Setup GIC.
     */
    if (R_INTC_Init() != INTC_SUCCESS)
    {
        while (1)
        {
            /* Spin here forever.. */
            R_COMPILER_Nop();
        }
    }

    /* Initial setting of the level 2 cache */
    R_CACHE_L2Init();

    /* Initial setting of the level 1 cache */
    R_CACHE_L1Init();

    /* change VBAR to use internal RAM */
    __set_vbar((uint32_t)&__vector_mirror_table_base);

    __enable_irq();
    __enable_fiq();

    /* call user program */
    main();

    /* Stops program from running off */
    while (1)
    {
        /* Spin here forever.. */
        R_COMPILER_Nop();
    }
}
/**********************************************************************************************************************
 End of function resetprg
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: disable_rtc
 * Description  : disable RTC module (See HM 16.5.6 and 52.4.2)
 * Arguments    : uint32_t ch : channel
 * Return Value : none
 *********************************************************************************************************************/
static void disable_rtc(uint32_t ch)
{
    uint8_t dummy8;
    uint32_t mask;
    uint8_t shift;
    static volatile struct st_rtc * const rtc[R_PRV_RTC_COUNT] =
    {
        &RTC0, &RTC1
    };
    static const uint8_t mstp_mask[R_PRV_RTC_COUNT] =
    {
        CPG_STBCR5_MSTP53, CPG_STBCR5_MSTP52
    };
    static const uint8_t mstp_shift[R_PRV_RTC_COUNT] =
    {
        CPG_STBCR5_MSTP53_SHIFT, CPG_STBCR5_MSTP52_SHIFT
    };
    static const uint16_t rtcxtalsel_mask[R_PRV_RTC_COUNT] =
    {
        PMG_RTCXTALSEL_RTC0XT, PMG_RTCXTALSEL_RTC1XT
    };
    static const uint8_t rtcxtalsel_shift[R_PRV_RTC_COUNT] =
    {
        PMG_RTCXTALSEL_RTC0XT_SHIFT, PMG_RTCXTALSEL_RTC1XT_SHIFT
    };

    /* channel check */
    if (ch >= R_PRV_RTC_COUNT)
    {
        return;
    }

    /* 1: select RTCXTAL for RTC (RCR4.RCKSEL = 0) */
    RZA_IO_RegWrite_8(&rtc[ch]->RCR4.BYTE, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegRead_8(&rtc[ch]->RCR4.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* 2: disable RTC clock (RCR3.RTCEN = 0) */
    RZA_IO_RegWrite_8(&rtc[ch]->RCR3.BYTE, 0, RTC_RCR3_RTCEN_SHIFT, RTC_RCR3_RTCEN);

    /* Wait for successfully disabled */
    dummy8 = 1;
    while (0 != dummy8)
    {
        dummy8 = RZA_IO_RegRead_8(&rtc[ch]->RCR3.BYTE, RTC_RCR3_RTCEN_SHIFT, RTC_RCR3_RTCEN);
    }

    /* 3: disable RTC clock while standby mode */
    mask  = rtcxtalsel_mask[ch];
    shift = rtcxtalsel_shift[ch];
    RZA_IO_RegWrite_16(&PMG.RTCXTALSEL.WORD, 0, shift, mask);
    RZA_IO_RegRead_16(&PMG.RTCXTALSEL.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* 4: Stop RTC module */
    mask  = mstp_mask[ch];
    shift = mstp_shift[ch];
    RZA_IO_RegWrite_8(&CPG.STBCR5.BYTE, 1, shift, mask);
    RZA_IO_RegRead_8(&CPG.STBCR5.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

}
/**********************************************************************************************************************
 End of function disable_rtc
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: disable_usb
 * Description  : disable USB module (See HM 32.2.4.4, 32.2.4.5, 32.2.20, 33.2.20, 52.2.6 and 52.4.3)
 * Arguments    : uint32_t ch : channel
 * Return Value : none
 *********************************************************************************************************************/
static void disable_usb(uint32_t ch)
{
    uint8_t dummy8;
    uint32_t mask;
    uint8_t shift;

    /* USB Host IO reg Top Address(ch0, ch1) */
    static volatile struct st_usb00 * const usb00_host[R_PRV_USB_COUNT] =
    {
        &USB00, &USB10
    };

    /* USB Function IO reg Top Address(ch0, ch1) */
    static volatile struct st_usb01 * const usb01_func[R_PRV_USB_COUNT] =
    {
        &USB01, &USB11
    };

    /* MSTP */
    static const uint8_t mstp_mask[R_PRV_USB_COUNT] =
    {
        CPG_STBCR6_MSTP61, CPG_STBCR6_MSTP60
    };
    static const uint8_t mstp_shift[R_PRV_USB_COUNT] =
    {
        CPG_STBCR6_MSTP61_SHIFT, CPG_STBCR6_MSTP60_SHIFT
    };

    /* STBREQ */
    static const uint8_t stbreq_mask[R_PRV_USB_COUNT] =
    {
        (CPG_STBREQ3_STBRQ31 | CPG_STBREQ3_STBRQ30), (CPG_STBREQ3_STBRQ33 | CPG_STBREQ3_STBRQ32)
    };
    static const uint8_t stbreq_shift[R_PRV_USB_COUNT] =
    {
        CPG_STBREQ3_STBRQ30_SHIFT, CPG_STBREQ3_STBRQ32_SHIFT
    };

    /* STBACK */
    static const uint8_t stback_mask[R_PRV_USB_COUNT] =
    {
        (CPG_STBACK3_STBAK31 | CPG_STBACK3_STBAK30), (CPG_STBACK3_STBAK33 | CPG_STBACK3_STBAK32)
    };
    static const uint8_t stback_shift[R_PRV_USB_COUNT] =
    {
        CPG_STBACK3_STBAK30_SHIFT, CPG_STBACK3_STBAK32_SHIFT
    };

    /* channel check */
    if (ch >= R_PRV_USB_COUNT)
    {
        return;
    }

    /* 1: Start USB module */

    /* MSTP = 0 */
    mask  = mstp_mask[ch];
    shift = mstp_shift[ch];
    RZA_IO_RegWrite_8(&CPG.STBCR6.BYTE, 0, shift, mask);
    RZA_IO_RegRead_8(&CPG.STBCR6.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* STBREQ = 0 */
    mask  = stbreq_mask[ch];
    shift = stbreq_shift[ch];
    RZA_IO_RegWrite_8(&CPG.STBREQ3.BYTE, 0x0, shift, mask);
    RZA_IO_RegRead_8(&CPG.STBREQ3.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* check STBACK = 0 */
    mask  = stback_mask[ch];
    shift = stback_shift[ch];
    dummy8 = 0x3;
    while (0x0 != dummy8)
    {
        dummy8 = RZA_IO_RegRead_8(&CPG.STBACK3.BYTE, shift, mask);
    }

    /* 2: Set the clock supplied to USBPHY to EXTAL clock (PHYCLK_CTRL.UCLKSEL = 0) */
    RZA_IO_RegWrite_32(&usb00_host[ch]->PHYCLK_CTRL.LONG, 0, USB_PHYCLK_CTRL_UCLKSEL_SHIFT, USB_PHYCLK_CTRL_UCLKSEL);

    /* 3: It can recover from deep standby by DP, DM change (PHYIF_CTRL.FIXPHY = 1) */
    RZA_IO_RegWrite_32(&usb00_host[ch]->PHYIF_CTRL.LONG, 1, USB_PHYIF_CTRL_FIXPHY_SHIFT, USB_PHYIF_CTRL_FIXPHY);

    /* 4: UTMI+PHY Normal Mode (LPSTS.SUSPM = 1) */
    RZA_IO_RegWrite_16(&usb01_func[ch]->LPSTS.WORD, 1, USB_LPSTS_SUSPM_SHIFT, USB_LPSTS_SUSPM);

    /* 5: UTMI + reset release (USBCTR.PLL_RST = 0) */
    RZA_IO_RegWrite_32(&usb00_host[ch]->USBCTR.LONG, 0, USB_USBCTR_PLL_RST_SHIFT, USB_USBCTR_PLL_RST);

    /* 6: wait 200us delay(Waiting for oscillation stabilization of USBPHY built-in PLL) */
    R_SoftwareDelay(STARTUP_CFG_USB_PHY_PLL_DELAY_COUNT);

    /* 7: Pulldown resistance control is effective (LINECTRL1 = 0x000A0000) */
    RZA_IO_RegWrite_32(
            &usb00_host[ch]->LINECTRL1.LONG,
            (USB_LINECTRL1_DPRPD_EN | USB_LINECTRL1_DMRPD_EN),
            IOREG_NONSHIFT_ACCESS,
            IOREG_NONMASK_ACCESS);

    /* 8: USBPHY standby mode (USBCTR.DIRPD = 1) */
    RZA_IO_RegWrite_32(&usb00_host[ch]->USBCTR.LONG, 1, USB_USBCTR_DIRPD_SHIFT, USB_USBCTR_DIRPD);

    /* 9: Stop USB module */

    /* STBREQ = 1 */
    mask  = stbreq_mask[ch];
    shift = stbreq_shift[ch];
    RZA_IO_RegWrite_8(&CPG.STBREQ3.BYTE, 0x3, shift, mask);
    RZA_IO_RegRead_8(&CPG.STBREQ3.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* check STBACK = 1 */
    mask  = stback_mask[ch];
    shift = stback_shift[ch];
    dummy8 = 0x0;
    while (0x3 != dummy8)
    {
        dummy8 = RZA_IO_RegRead_8(&CPG.STBACK3.BYTE, shift, mask);
    }

    /* MSTP = 1 */
    mask  = mstp_mask[ch];
    shift = mstp_shift[ch];
    RZA_IO_RegWrite_8(&CPG.STBCR6.BYTE, 1, shift, mask);
    RZA_IO_RegRead_8(&CPG.STBCR6.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

}
/**********************************************************************************************************************
 End of function disable_usb
 *********************************************************************************************************************/

/* End of File */
