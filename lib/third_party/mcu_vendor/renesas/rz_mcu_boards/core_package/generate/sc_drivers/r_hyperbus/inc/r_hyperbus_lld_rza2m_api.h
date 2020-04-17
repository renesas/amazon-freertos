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
 * File Name    : r_hyperbus_lld_rza2m_api.h
 * Version      : 1.0
 * Description  : .
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2019 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include "r_typedefs.h"
#include "driver.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef SC_DRIVERS_R_HYPERBUS_INC_R_HYPERBUS_LLD_RZA2M_API_H_
#define SC_DRIVERS_R_HYPERBUS_INC_R_HYPERBUS_LLD_RZA2M_API_H_

/* Version Number of API */

#define STDIO_HYPERBUS_RZ_LLD_DRV_NAME ("LLD RZA2M.HYPERBUS")

/** Major Version Number of API */
#define STDIO_HYPERBUS_RZ_LLD_VERSION_MAJOR      (1)
/** Minor Version Number of API */
#define STDIO_HYPERBUS_RZ_LLD_VERSION_MINOR      (0)
/** Minor Version Number of API */
#define STDIO_HYPERBUS_RZ_LLD_BUILD_NUM          (0)
/** Unique ID */
#define STDIO_HYPERBUS_RZ_LLD_UID                (0)

/*!
 * @enum           e_hyperbus_access_area_t
 *                 emum of access area setting
 */
typedef enum
{
    HYPERBUS_CS0_AREA = (0), /*!< Access CS0 area */
    HYPERBUS_CS1_AREA = (1), /*!< Access CS1 area */
} e_hyperbus_access_area_t;

/*!
 * @enum           e_hyperbus_space_select_t
 *                 emum of access space setting
 */
typedef enum
{
    HYPERBUS_MEMORY_SPACE   = (0), /*!< Select memory   space */
    HYPERBUS_REGISTER_SPACE = (1), /*!< Select register space */
} e_hyperbus_space_select_t;

/*!
 * @enum           e_hyperbus_init_control_t
 *                 emum of Execute initialize function project setting
 */
typedef enum
{
    HYPERBUS_NO_INIT        = (0), /*!< Not execute initialize function*/
    HYPERBUS_INIT_AT_LOADER = (1), /*!< Execute initialize function on loader */
    HYPERBUS_INIT_AT_APP    = (2), /*!< Execute initialize function on application */
} e_hyperbus_init_control_t;

/*!
 * @enum           e_hyperbus_maxen_t
 *                 emum of MAXEN bit of MCR
 */
typedef enum
{
    HYPERBUS_MAXEN_OFF = (0), /*!< Maximun time setting enable(default value)) */
    HYPERBUS_MAXEN_ON  = (1), /*!< Maximun time setting enable */
} e_hyperbus_maxen_t;

/*!
 * @enum           e_hyperbus_cshi_t
 *                 emum of RCSHI/WCSHI field of MTR
 */
typedef enum
{
    HYPERBUS_CSHI_1P5_CYCLE  = (0),  /*!<  1.5 clock cycle(default value) */
    HYPERBUS_CSHI_2P5_CYCLE  = (1),  /*!<  2.5 clock cycle */
    HYPERBUS_CSHI_3P5_CYCLE  = (2),  /*!<  3.5 clock cycle */
    HYPERBUS_CSHI_4P5_CYCLE  = (3),  /*!<  4.5 clock cycle */
    HYPERBUS_CSHI_5P5_CYCLE  = (4),  /*!<  5.5 clock cycle */
    HYPERBUS_CSHI_6P5_CYCLE  = (5),  /*!<  6.5 clock cycle */
    HYPERBUS_CSHI_7P5_CYCLE  = (6),  /*!<  7.5 clock cycle */
    HYPERBUS_CSHI_8P5_CYCLE  = (7),  /*!<  8.5 clock cycle */
    HYPERBUS_CSHI_9P5_CYCLE  = (8),  /*!<  9.5 clock cycle */
    HYPERBUS_CSHI_10P5_CYCLE = (9),  /*!< 10.5 clock cycle */
    HYPERBUS_CSHI_11P5_CYCLE = (10), /*!< 11.5 clock cycle */
    HYPERBUS_CSHI_12P5_CYCLE = (11), /*!< 12.5 clock cycle */
    HYPERBUS_CSHI_13P5_CYCLE = (12), /*!< 13.5 clock cycle */
    HYPERBUS_CSHI_14P5_CYCLE = (13), /*!< 14.5 clock cycle */
    HYPERBUS_CSHI_15P5_CYCLE = (14), /*!< 15.5 clock cycle */
    HYPERBUS_CSHI_16P5_CYCLE = (15)  /*!< 16.5 clock cycle */
} e_hyperbus_cshi_t;

/*!
 * @enum           e_hyperbus_css_t
 *                 emum of RCSS/WCSS field of MTR
 */
typedef enum
{
    HYPERBUS_CSS_1_CYCLE  = (0),  /*!<  1 clock cycle(default value) */
    HYPERBUS_CSS_2_CYCLE  = (1),  /*!<  2 clock cycle */
    HYPERBUS_CSS_3_CYCLE  = (2),  /*!<  3 clock cycle */
    HYPERBUS_CSS_4_CYCLE  = (3),  /*!<  4 clock cycle */
    HYPERBUS_CSS_5_CYCLE  = (4),  /*!<  5 clock cycle */
    HYPERBUS_CSS_6_CYCLE  = (5),  /*!<  6 clock cycle */
    HYPERBUS_CSS_7_CYCLE  = (6),  /*!<  7 clock cycle */
    HYPERBUS_CSS_8_CYCLE  = (7),  /*!<  8 clock cycle */
    HYPERBUS_CSS_9_CYCLE  = (8),  /*!<  9 clock cycle */
    HYPERBUS_CSS_10_CYCLE = (9),  /*!< 10 clock cycle */
    HYPERBUS_CSS_11_CYCLE = (10), /*!< 11 clock cycle */
    HYPERBUS_CSS_12_CYCLE = (11), /*!< 12 clock cycle */
    HYPERBUS_CSS_13_CYCLE = (12), /*!< 13 clock cycle */
    HYPERBUS_CSS_14_CYCLE = (13), /*!< 14 clock cycle */
    HYPERBUS_CSS_15_CYCLE = (14), /*!< 15 clock cycle */
    HYPERBUS_CSS_16_CYCLE = (15)  /*!< 16 clock cycle */
} e_hyperbus_css_t;

/*!
 * @enum           e_hyperbus_csh_t
 *                 emum of RCSH/WCSH field of MTR
 */
typedef enum
{
    HYPERBUS_CSH_1_CYCLE  = (0),  /*!<  1 clock cycle(default value) */
    HYPERBUS_CSH_2_CYCLE  = (1),  /*!<  2 clock cycle */
    HYPERBUS_CSH_3_CYCLE  = (2),  /*!<  3 clock cycle */
    HYPERBUS_CSH_4_CYCLE  = (3),  /*!<  4 clock cycle */
    HYPERBUS_CSH_5_CYCLE  = (4),  /*!<  5 clock cycle */
    HYPERBUS_CSH_6_CYCLE  = (5),  /*!<  6 clock cycle */
    HYPERBUS_CSH_7_CYCLE  = (6),  /*!<  7 clock cycle */
    HYPERBUS_CSH_8_CYCLE  = (7),  /*!<  8 clock cycle */
    HYPERBUS_CSH_9_CYCLE  = (8),  /*!<  9 clock cycle */
    HYPERBUS_CSH_10_CYCLE = (9),  /*!< 10 clock cycle */
    HYPERBUS_CSH_11_CYCLE = (10), /*!< 11 clock cycle */
    HYPERBUS_CSH_12_CYCLE = (11), /*!< 12 clock cycle */
    HYPERBUS_CSH_13_CYCLE = (12), /*!< 13 clock cycle */
    HYPERBUS_CSH_14_CYCLE = (13), /*!< 14 clock cycle */
    HYPERBUS_CSH_15_CYCLE = (14), /*!< 15 clock cycle */
    HYPERBUS_CSH_16_CYCLE = (15), /*!< 16 clock cycle */
} e_hyperbus_csh_t;

/*!
 * @enum           e_hyperbus_ltcy_t
 *                 emum of LTCY field of MTR
 */
typedef enum
{
    HYPERBUS_LTCY_5_CYCLE  = (0),   /*!<  5 clock cycle */
    HYPERBUS_LTCY_6_CYCLE  = (1),   /*!<  6 clock cycle(HyperRAM default value) */
    HYPERBUS_LTCY_7_CYCLE  = (2),   /*!<  7 clock cycle */
    HYPERBUS_LTCY_8_CYCLE  = (3),   /*!<  8 clock cycle */
    HYPERBUS_LTCY_9_CYCLE  = (4),   /*!<  9 clock cycle */
    HYPERBUS_LTCY_10_CYCLE = (5),   /*!<  10 clock cycle */
    HYPERBUS_LTCY_11_CYCLE = (6),   /*!<  11 clock cycle */
    HYPERBUS_LTCY_12_CYCLE = (7),   /*!<  12 clock cycle */
    HYPERBUS_LTCY_13_CYCLE = (8),   /*!<  13 clock cycle */
    HYPERBUS_LTCY_14_CYCLE = (9),   /*!<  14 clock cycle */
    HYPERBUS_LTCY_15_CYCLE = (10),  /*!<  15 clock cycle */
    HYPERBUS_LTCY_16_CYCLE = (11),  /*!<  16 clock cycle(HyperFlash default value) */
} e_hyperbus_ltcy_t;

typedef struct
{
    e_hyperbus_init_control_t init_flag0;    /*!< Initialize procedure excute project          */
    e_hyperbus_maxen_t        maxen0;        /*!< Maximum time setting enable bit of MCR0      */
    uint16_t                  maxlen0;       /*!< Maximum time setting field of MCR0           */
    e_hyperbus_cshi_t         rcshi0;        /*!< Read  chip select timing field of MTR0       */
    e_hyperbus_cshi_t         wcshi0;        /*!< Write chip select timing field of MTR0       */
    e_hyperbus_css_t          rcss0;         /*!< Read  chip select setup timing field of MTR0 */
    e_hyperbus_css_t          wcss0;         /*!< Write chip select setup timing field of MTR0 */
    e_hyperbus_csh_t          rcsh0;         /*!< Read  chip select hold  timing field of MTR0 */
    e_hyperbus_csh_t          wcsh0;         /*!< Write chip select hold  timing field of MTR0 */
    e_hyperbus_ltcy_t         operate_ltcy0; /*!< Latency cycle number on operate setting      */
    e_hyperbus_init_control_t init_flag1;    /*!< Initialize procedure excute project          */
    e_hyperbus_maxen_t        maxen1;        /*!< Maximum time setting enable bit of MCR1      */
    uint16_t                  maxlen1;       /*!< Maximum time setting field of MCR1           */
    e_hyperbus_cshi_t         rcshi1;        /*!< Read  chip select timing field of MTR1       */
    e_hyperbus_cshi_t         wcshi1;        /*!< Write chip select timing field of MTR1       */
    e_hyperbus_css_t          rcss1;         /*!< Read  chip select setup timing field of MTR1 */
    e_hyperbus_css_t          wcss1;         /*!< Write chip select setup timing field of MTR1 */
    e_hyperbus_csh_t          rcsh1;         /*!< Read  chip select hold  timing field of MTR1 */
    e_hyperbus_csh_t          wcsh1;         /*!< Write chip select hold  timing field of MTR1 */
    e_hyperbus_ltcy_t         operate_ltcy1; /*!< Latency cycle number on operate setting      */
} st_hyperbus_cfg_t;

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @fn         R_HYPERBUS_Setup
 *
 * @brief      Initialize HyperBus controller and connected device via user code
 *
 * @param[in]  none
 * @retval     none
 */
void R_HYPERBUS_Setup(void);

/**
 * @fn         R_HYPERBUS_SelectSpace
 *
 * @brief      Set CRT field of MCR
 *
 * @param[in]  area : area select(CS0 or CS1)
 * @param[in]  space: space select(Memory or Register)
 * @retval     DRV_SUCCESS  Successful operation (always)
 */
int_t R_HYPERBUS_SelectSpace(e_hyperbus_access_area_t area, e_hyperbus_space_select_t space);

/**
 * @fn         R_HYPERBUS_GetVersion
 *
 * @brief      Obtains driver specific version information which is used for
 *             reporting
 *
 * @param[out] pVerInfo:  driver version information is populated into this
 *                        structure.
 *                        @note the structure resources are created
 *                              by the application not this function
 *
 * @retval     DRV_SUCCESS   Successful operation.
 */
int_t R_HYPERBUS_GetVersion(st_ver_info_t *p_ver_info);

/**
 * @fn         HyperBus_UserConfig
 *
 * @brief      User defined device initialisation routine interface
 *
 * @param[in]  p_cfg       : Pointer to the configuration table
 *
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     Error condition occurs.
 */
extern int_t HyperBus_UserConfig(const st_hyperbus_cfg_t *p_cfg) __attribute__((weak));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SC_DRIVERS_R_HYPERBUS_INC_R_HYPERBUS_LLD_RZA2M_API_H_ */
