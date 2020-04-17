/******************************************************************************
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
 * Renesas reserves the right, without notice, to make changes to this
 * software and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
 * File Name    : resetprg.h
 * Version      : 1.0
 * Description  : header file for resetprg.c
 *****************************************************************************/
/******************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 24.05.2019 1.00     First Release
 *****************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 *****************************************************************************/
#include "r_typedefs.h"

/******************************************************************************
 Macro definitions
 *****************************************************************************/

#ifndef COMPILER_INC_RESETPRG_H_
#define COMPILER_INC_RESETPRG_H_

/******************************************************************************
 Global Typedef definitions
 *****************************************************************************/

/******************************************************************************
 External global variables
 *****************************************************************************/

/******************************************************************************
 Exported global functions
 *****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/******************************************************************************
 * @fn            resetprg
 * @brief         Initialise routine before branching main()
 *****************************************************************************/
extern void resetprg (void);

/******************************************************************************
 * @fn            main
 * @brief         Application entry function
 * @retval        ignored
 *****************************************************************************/
extern int_t main(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COMPILER_INC_RESETPRG_H_ */
