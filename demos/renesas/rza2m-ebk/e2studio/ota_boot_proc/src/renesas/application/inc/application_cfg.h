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
 * and to discontinue the availability of this software. By using this software,
 * you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 **********************************************************************************************************************
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.

 **********************************************************************************************************************
 * File Name    : application_cfg.h
 * Device(s)    : RZ/A2M
 * H/W Platform : RZ/A2M Evaluation Board Kit
 * Description  : Overall control of system state
 **********************************************************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 07.02.2018 1.00 First Release
 *********************************************************************************************************************/
#ifndef RENESAS_CONFIGURATION_APPLICATION_CFG_H_
#define RENESAS_CONFIGURATION_APPLICATION_CFG_H_

/* Common function controls */
#define R_OPTION_ENABLE     (1)
#define R_MSG_ENABLED       ("ENABLED")

#define R_OPTION_DISABLE    (0)
#define R_MSG_DISABLED      ("DISABLED")

/* Enable Trace Macro here allows individual source files to enable Trace macro */
/* Removing this define disables trace macro for all files regardless of file setting */
#define _TRACE_ON_

#endif /* RENESAS_CONFIGURATION_APPLICATION_CFG_H_ */
