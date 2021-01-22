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
 * File Name    : r_adc_drv_link.h
 * Description  : Link high-low level driver include file
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 19.02.2019 1.00     First Release
 *         : 25.06.2019 1.01     Fixed Inclusion Guard
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

/******************************************************************************************************************//**
 * @ingroup Interface_Library
 * @defgroup RZA2M_RSK_ADC_LINK RSK+RZA2M ADC driver link configuration
 * @brief This interface manages the link between the high and low layer drivers (HLD & LLD).
 *
 * @section RZA2M_RSK_ADC_LINK_SUMMARY Summary
 * @brief The ADC driver for the RSK+RZA2M is comprised of the following components ...
 *
 * @section RZA2M_RSK_ADC_LINK_API_LIMITATIONS Known Limitations
 * NONE_YET
 *
 * @section RZA2M_RSK_ADC_LINK_API_INSTANCES Known Implementations
 * @ref NONE_YET
 *
 * @section RZA2M_RSK_ADC_LINK_API_RELATED Related modules
 * @ingroup RZA2M_RSK_ADC_DRIVER
 * See also:
 * @ref RZA1H_RSK_ADC_DRIVER
 * @ref RZA1H_RSK_ADC
 * @{
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes
 *********************************************************************************************************************/

#include "r_adc_lld_rza2m.h"       /* low layer to high layer API */
#include "r_intc_lld_rza2m.h"      /* low layer API for INTC */

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef R_ADC_INC_R_ADC_DRV_LINK_H_
#define R_ADC_INC_R_ADC_DRV_LINK_H_

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 *********************************************************************************************************************/

#endif /* R_ADC_INC_R_ADC_DRV_LINK_H_ */

/**********************************************************************************************************************
 End  Of File
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @} (end addtogroup RZA2M_RSK_ADC_DRV_LINK)
 *********************************************************************************************************************/
