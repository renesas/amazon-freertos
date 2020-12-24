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
 * and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * @headerfile     wild_compare.h
 * @brief          Function to perform a string compare with * wild card.
 * @version        1.00
 * @date           27.06.2018
 * H/W Platform    RZ/A1LU
 *****************************************************************************/
 /*****************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 30.06.2018 1.00 First Release
 *****************************************************************************/

/* Multiple inclusion prevention macro */
#ifndef WILD_COMPARE_H_
#define WILD_COMPARE_H_

/**************************************************************************//**
 * @ingroup R_SW_PKG_93_CONSOLE
 * @defgroup R_SW_PKG_93_CONSOLE_WILDCARD Console wildcard support
 * @brief Console wildcard support
 *
 * @anchor R_SW_PKG_93_CONSOLE_WILDCARD_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA2M Software Package.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 * @{
 *****************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 *****************************************************************************/
#include "r_typedefs.h"

/******************************************************************************
 Macro definitions
 *****************************************************************************/

/******************************************************************************
 Typedef definitions
 *****************************************************************************/

/******************************************************************************
 Variable External definitions and Function External definitions
 *****************************************************************************/

/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Check a string against a wild card string where the character '*'
 *        indicates any number of characters. The application of this is to
          match file names like "*.txt".
 * @param pszWildCard - Pointer to the string containing the wild card
 * @param pszString - Pointer to the string to compare
 * @return true if the string matches the wild card string.
 */
extern bool_t wild_compare (const char *pszWildCard, const char *pszString);

#ifdef __cplusplus
}
#endif

#endif /* WILD_COMPARE_H_ */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/******************************************************************************
 End  Of File
 *****************************************************************************/
