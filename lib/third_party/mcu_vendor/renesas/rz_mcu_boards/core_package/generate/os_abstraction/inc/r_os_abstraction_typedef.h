/***********************************************************************************************************************
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
 * File Name    : r_os_abstraction_typedef.h
 * Description  : see Detailed description (below)
 *********************************************************************************************************************/
/******************************************************************************************************************//**
 * @ingroup Interface_Library
 * @defgroup OS_ABSTRACT Operating System abstraction module
 * @brief Provides type defines for OS abstraction.
 *********************************************************************************************************************/
#include "r_typedefs.h"

#ifndef SRC_RENESAS_APPLICATION_INC_R_OS_ABSTRACTION_TYPEDEF_H_
#define SRC_RENESAS_APPLICATION_INC_R_OS_ABSTRACTION_TYPEDEF_H_

/* Version Information for drivers (high or low level little endian) */
typedef struct
{
    union
    {
        uint32_t full;      /*!< Major + Minor combined as 1 uint32_t data member  */
        struct
        {
          uint16_t minor;    /*!< Version, modified by Product Owner  */
          uint16_t major;    /*!< Version, modified by developer  */
        } sub;
    } version;

    uint32_t build;          /*!< Build Number Generated during the release   */

    /* The driver name */
    const char   *p_szdriver_name;
} st_os_abstraction_info_t;

/** Event state object */
typedef enum _event_state_t
{
    EV_RESET = 0,   /*!< Event Reset state*/
    EV_SET,         /*!< Event Set state */
    EV_INVALID      /*!< Event Invalid state */
} e_event_state_t;


/*Memory Regions*/
#ifndef REPLACE_MEMORY_REGION_ENUM
typedef enum
{
    R_MEMORY_REGION_DEFAULT = 0, /*!< This is to be appended upon future update */
} e_memory_region_t;
#endif

/** Message structure */
typedef void *p_os_msg_t;

/**Message queue handle */
typedef void *p_os_msg_queue_handle_t;

/** semaphore handle object */
typedef uint32_t* p_semaphore_t;

/* mutex handle object */
typedef void* p_mutex_t;

/* pointer to mutex handle object*/
typedef p_mutex_t* pp_mutex_t;

/** event handle object */
typedef void *p_event_t;

/** pointer to event handle object */
typedef p_event_t* pp_event_t;

/** pointer to event handle object pointer */
typedef pp_event_t* ppp_event_t;

/** system time object */
typedef uint32_t systime_t;

/** task handle object */
typedef void os_task_t;

/** task body prototype */
typedef void (*os_task_code_t)(void *params);

#endif /* SRC_RENESAS_APPLICATION_INC_R_OS_ABSTRACTION_TYPEDEF_H_ */

/******************************************************************************************************************//**
 * @} (end addtogroup OS_ABSTRACT)
 *********************************************************************************************************************/
