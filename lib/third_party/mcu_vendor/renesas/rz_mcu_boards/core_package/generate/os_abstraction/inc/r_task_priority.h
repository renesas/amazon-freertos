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
 * File Name    : r_task_priority.h
 * Version      : 1.0
 * Description  : The task priority definitions.
 *********************************************************************************************************************/

#ifndef TASKPRIORITY_H_INCLUDED
#define TASKPRIORITY_H_INCLUDED

/**********************************************************************************************************************
Enumerated Types
 *********************************************************************************************************************/

/* Software interrupt tasks - Give each task an individual priority */
/* Normal tasks - share priority 0..15*/
#define R_OS_TASK_MAIN_TASK_PRI     (6) /* Application main priority */
#define TASK_CONSOLE_TASK_PRI       (6) /* Console Application priority */

/* This is designed to soak CPU time but because it is low priority
   responsiveness of the system is not lost */
#define TASK_IDLE_TASK_PRI          (1)

#define TASK_NUMBER_OF_PRIORITIES   (20)

/**********************************************************************************************************************
Typedef definitions
 *********************************************************************************************************************/

#endif /* TASKPRIORITY_H_INCLUDED */

/**********************************************************************************************************************
End  Of File
 *********************************************************************************************************************/
