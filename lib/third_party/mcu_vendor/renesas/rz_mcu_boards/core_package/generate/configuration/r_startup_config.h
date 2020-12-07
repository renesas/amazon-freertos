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
/******************************************************************************
* File Name    : r_startup_config.h
* Description  : The file r_startup_config.h is used to configure your STARTUP.
******************************************************************************/
#ifndef R_STARTUP_CONFIG_H
#define R_STARTUP_CONFIG_H

/* example of configured macro definitions */

/* select whether RTC0(secure) use or not
 * 0 : Use RTC0 (do not touch RTC0 in startup)
 * 1 : Not use RTC0 (startup routine disables RTC0 for low-power and stable operation)
 */
#define STARTUP_CFG_DISABLE_RTC0 (1)

/* select whether RTC1(normal) use or not
 * 0 : Use RTC1 (do not touch RTC1 in startup)
 * 1 : Not use RTC1 (startup routine disables RTC0 for low-power and stable operation)
 */
#define STARTUP_CFG_DISABLE_RTC1 (1)

/* select whether USB0 use or not
 * 0 : Use USB0
 * 1 : Not use USB0
 */
#define STARTUP_CFG_DISABLE_USB0 (1)

/* select whether USB1 use or not
 * 0 : Use USB1
 * 1 : Not use USB1
 */
#define STARTUP_CFG_DISABLE_USB1 (1)

/* Waiting for oscillation stabilization of USBPHY built-in PLL
 * [cache off] wait 200us(500) delay
 * [cache on ] wait 200us(105600) delay
 */
#define STARTUP_CFG_USB_PHY_PLL_DELAY_COUNT (500)

/* Boot mode setting
 * 0 : CS0 boot
 * 1 : SD boot
 * 2 : eMMC boot
 * 3 : SPI flash boot via SPI multi I/O bus
 * 4 : Octal-SPI flash boot via SPI multi I/O bus
 * 5 : Hyper flash boot via SPI multi I/O bus
 * 6 : Octa flash boot via Octa bus
 * 7 : Hyper flash boot via HyperBus(TM)
 */
#define STARTUP_CFG_BOOT_MODE (3)

/* Project Type setting
 * 0 : Loader Project
 * 1 : Application Project
 */
#define STARTUP_CFG_PROJECT_TYPE (1)

/* The following macro definition is not used in this project.
 */
#define STARTUP_CFG_PARAM_CHECKING_ENABLE               (1)

/* select CPU package
 * 0 : PLBG0176GA-A
 * 1 : PLBG0256KA-B
 * 2 : PRBG0272GA-A
 * 3 : PRBG0324GA-A
 */
#define STARTUP_CFG_MCU_PART_PACKAGE (3)

/* On-chip data retention RAM hardware is able to disable writing.
 * This macro controls this feature.
 * 0 : Disable writing on-chip data retention RAM
 * 1 : Enable writing on-chip data retention RAM
 */
#define STARTUP_CFG_RETENTION_RAM_WRITE_ENABLE (1)

/* Device Type setting connected to SPIBSC
 * 0 : SerialFlash x 1
 * 1 : SerialFlash x 2
 * 2 : Octal-SPI Flash
 * 3 : HyperFlash
 */
#define STARTUP_CFG_SPIBSC_CONNECT_DEVICE_TYPE (0)

#endif /* R_STARTUP_CONFIG_H */

/* End of File */
