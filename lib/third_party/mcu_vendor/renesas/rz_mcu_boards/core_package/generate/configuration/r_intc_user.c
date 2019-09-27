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
 * File Name   : r_intc_user.c
 * Description : INTC driver (User modifiable code)
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_typedefs.h"
#include "compiler_settings.h"
#include "r_compiler_abstraction_api.h"
#include "r_intc_drv_api.h"
#include "iodefine.h"

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: Userdef_INTC_Pre_Interrupt
 * Description  : This function is the user-defined function called by the
 *              : INTC driver. Write the processing to be executed
 *              : before registered interrupt function is called
 * Arguments    : e_r_drv_intc_intid_t int_id    : Interrupt ID
 * Return Value : INTC_SUCCESS        : Success
 *********************************************************************************************************************/
e_r_drv_intc_err_t Userdef_INTC_Pre_Interrupt(e_r_drv_intc_intid_t int_id)
{
    e_r_drv_intc_err_t err;
    err = INTC_SUCCESS;

    /* int_id not used */
    UNUSED_PARAM(int_id);

    /* Do Nothing */

    return err;
}
/**********************************************************************************************************************
 * End of function Userdef_INTC_Pre_Interrupt
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: Userdef_INTC_Post_Interrupt
 * Description  : This function is the user-defined function called by the
 *              : INTC driver. Write the processing to be executed
 *              : after registered interrupt function is called
 * Arguments    : e_r_drv_intc_intid_t int_id    : Interrupt ID
 * Return Value : INTC_SUCCESS        : Success
 *********************************************************************************************************************/
e_r_drv_intc_err_t Userdef_INTC_Post_Interrupt(e_r_drv_intc_intid_t int_id)
{
    e_r_drv_intc_err_t err;
    err = INTC_SUCCESS;

    /* int_id not used */
    UNUSED_PARAM(int_id);

    /* Do Nothing */

    return err;
}
/**********************************************************************************************************************
 * End of function Userdef_INTC_Post_Interrupt
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: Userdef_INTC_UndefId
 * Description  : This function is the user-defined function called by the
 *              : INTC driver. Write the processing to be executed
 *              : when an unsupported interrupt ID is received.
 * Arguments    : e_r_drv_intc_intid_t int_id    : Interrupt ID
 * Return Value : INTC_SUCCESS        : Success
 *********************************************************************************************************************/
e_r_drv_intc_err_t Userdef_INTC_UndefId(e_r_drv_intc_intid_t int_id)
{
    e_r_drv_intc_err_t err;
    err = INTC_SUCCESS;

    /* int_id not used */
    UNUSED_PARAM(int_id);

    /* Execute processing when receiving interrupt ID which is not supported */
    while (1)
    {
        /* Spin here forever.. */
        R_COMPILER_Nop();
    }

    return err;
}
/**********************************************************************************************************************
 * End of function Userdef_INTC_UndefId
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: Userdef_INTC_UnregisteredID
 * Description  : This function is the user-defined function called by the
 *              : INTC driver. Write the processing to be executed
 *              : when interrupt ID who never registered by R_INTC_RegistIntFunc
 *              : is received.
 * Arguments    : e_r_drv_intc_intid_t int_id    : Interrupt ID
 * Return Value : INTC_SUCCESS        : Success
 *********************************************************************************************************************/
e_r_drv_intc_err_t Userdef_INTC_UnregisteredID(e_r_drv_intc_intid_t int_id)
{
    e_r_drv_intc_err_t err;
    err = INTC_SUCCESS;

    /* int_id not used */
    UNUSED_PARAM(int_id);

    /* Execute processing when receiving interrupt ID whicn handler is not registered */
    __disable_irq();

    while (1)
    {
        /* Spin here forever.. */
        R_COMPILER_Nop();
    }

    return err;
}
/**********************************************************************************************************************
 * End of function Userdef_INTC_UnregisteredID
 *********************************************************************************************************************/

/* END of File */

