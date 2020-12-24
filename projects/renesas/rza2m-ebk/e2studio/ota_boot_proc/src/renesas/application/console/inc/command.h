/**********************************************************************************************************************
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
 **********************************************************************************************************************
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * @headerfile     command.h
 * @brief          The entry point of the main command handler and associated
 *                 commands
 * @version        1.00
 * @date           27.06.2018
 * H/W Platform    RZ/A2M
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 30.06.2018 1.00 First Release
 *********************************************************************************************************************/

/******************************************************************************************************************//**
 * @ingroup RENESAS_APPLICATION_SOFTWARE_PACKAGE Software Package
 * @defgroup R_SW_PKG_93_CONSOLE Console
 * @brief Console implementation for RZA2M Application.
 * @anchor R_SW_PKG_93_CONSOLE_SUMMARY
 * @par Summary
 * @brief Console implementation for the RZA2M Application.<br>
 *
 * @anchor R_SW_PKG_93_CONSOLE_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA2M Software Package.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 * @{
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "console.h"

/* Multiple inclusion prevention macro */
#ifndef COMMAND_H_
#define COMMAND_H_

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Variable External definitions and Function External definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 *********************************************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    FILE *p_in;
    FILE *p_out;
} st_iostr_t;

typedef st_iostr_t * pst_iostr_t;

/**
 * @fn    cmd_console
 * @brief Function to process the console commands
 * @param pIn - Pointer to the input file stream
 * @param pOut - Pointer to the file stream for echo of command input
 * @param pszPrompt - Pointer to a command prompt
 */
void cmd_console(FILE* pIn, FILE *pOut, char_t *pszPrompt);

#ifdef __cplusplus
}
#endif

#endif /* COMMAND_H_ */
/******************************************************************************************************************//**
 * @} (end addtogroup)
 *********************************************************************************************************************/
/**********************************************************************************************************************
 End  Of File
 *********************************************************************************************************************/
