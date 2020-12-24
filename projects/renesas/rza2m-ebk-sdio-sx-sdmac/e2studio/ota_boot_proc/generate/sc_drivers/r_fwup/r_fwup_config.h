/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013-2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name     : r_fwup_config.h
* Description   : Configures the Firmware update module.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           30.11.2020 1.00    Initial Release
***********************************************************************************************************************/
#ifndef FWUP_CONFIG_H
#define FWUP_CONFIG_H

#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#include "platform.h"
#else
// RXブートローダ機能をRZ/A2M FWアップデート環境へ移植するにあたり、
// RZ/A2Mの動作切り替えに、本マクロを使用する。
// 本実装時は、適切な識別子による動作切り替えが必要。
#define MCU_SERIES_RZA2	(1)
#endif

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Select the implementation environment.
    0 = Bootloader. (default)
    1 = Firmware update w/o OS.
    2 = Firmware update with Amazon FreeRTOS(OTA).
    3 = Firmware update with other FreeRTOS.
*/
#define FWUP_CFG_IMPLEMENTATION_ENVIRONMENT     (0)

/* Select the communication function.
    0 = SCI connection. (default)
    1 = Ethernet connection.
    2 = USB connection.
    3 = SDHI connection.
    4 = QSPI connection.
*/
#define FWUP_CFG_COMMUNICATION_FUNCTION			(0)

/* Set whether to use external memory (ex. serial flash) to update firmware.
    0x0 = Not use. (default)
    0x1 = Use for Temporary area.
    0x2 = Use for Execute area.
    0x3 = Use for Execute area and Temporary area. (ex. RZ/A2M)
 */
#define FWUP_CFG_USE_EXMEM						(0x3)

/* Select the algorithm of signature verification.
    0 = ECDSA. (default)
*/
#define FWUP_CFG_SIGNATURE_VERIFICATION			(0)

/* Enable Boot Protect Setting.
	0 =　Disable.(Prohibit) （default）
	1 =　Enable.(Allow)     [Note]

	[Note]
	When enabled (1), FAW.FSPR bit = 0 is set. After this setting,
	the area other than the area specified in FAW can never be rewritten.
	Be careful when setting this variable.
 */
#define FWUP_CFG_BOOT_PROTECT_ENABLE     (0)

/* Disable Printf Output Setting.
   Disables the character output by printf to the terminal software.
	0 =　Enable. （default）
	1 =　Disable.
 */
#define FWUP_CFG_PRINTF_DISABLE     (0)

#endif /* FWUP_CONFIG_H */
