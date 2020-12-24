
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
 * @headerfile     console.h
 * @brief          Simple command line console implementation
 * @version        1.00
 * @date           27.06.2018
 * H/W Platform    RZ/A1LU
 *****************************************************************************/
 /*****************************************************************************
 * History      : DD.MM.YYYY Ver. Description
 *              : 30.06.2018 1.00 First Release
 *****************************************************************************/

/* Multiple inclusion prevention macro */
#ifndef CONSOLE_H_
#define CONSOLE_H_

/**************************************************************************//**
 * @ingroup R_SW_PKG_93_CONSOLE
 * @defgroup R_SW_PKG_93_CONSOLE_HDR Console Internal header
 * @brief Console Implementation header.
 *
 * @anchor R_SW_PKG_93_CONSOLE_HDR_INSTANCES
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

/* Define default settings */
#ifndef CMD_READER_LINE_SIZE
#define CMD_READER_LINE_SIZE        (1040)
#endif

#ifndef CMD_MAX_PATH
#define CMD_MAX_PATH                (2600)
#endif

#ifndef CMD_MAX_ARG_LENGTH
#define CMD_MAX_ARG_LENGTH          (128)
#endif

#ifndef CMD_MAX_ARG
#define CMD_MAX_ARG                 (8)
#endif

#define CMD_ESCAPE_CHARACTER        (0x1B)

#define SERIAL

/** Function macro to remove the unused variable information in command processor functions */
#define AVOID_UNUSED_WARNING        (void) iArgCount; (void) ppszArgument; (void) pCom;

/******************************************************************************
 Typedef definitions
 *****************************************************************************/

typedef struct _CMDTAB /* CMDTAB */ st_command_table_t;
typedef struct _CMDTAB *pst_command_table_t;
typedef const struct _CMDTAB *cpst_command_table_t;

/** Enumerate the escape sequences states */
typedef enum
{
    ESC_NO_ESCAPE = 0, ESC_ESCAPE_SEQUENCE
} e_cmdesc_t;

/** Structure of Variables used by command reader These are the values that change as the command is read in */
typedef struct
{
    /** Buffer for bytes as parsed */
    char_t buffer[CMD_READER_LINE_SIZE];
    char_t command[CMD_READER_LINE_SIZE];

    /** Buffer for arguments */
    char_t arguments[(CMD_MAX_ARG * CMD_MAX_ARG_LENGTH)];

    /** The escape sequence state */
    e_cmdesc_t escape_sequence;

    /** Index into command line buffer */
    uint32_t /* uiBufIdx */ buffer_index;

    /** The destination data byte */
    int16_t data;

    /** The number of characters read */
    size_t /*  stReadCount */ read_count;
} st_conva_t;


/** Define the return codes */
typedef enum
{
    CMD_OK = 0,
    CMD_NO_PROMPT,
    CMD_LINE_TOO_LONG,
    CMD_ESCAPE_SEQUENCE,
    CMD_UNKNOWN,
    CMD_USER_EXIT,
    CMD_LOG_OUT,
    CMD_ERROR_IN_IO,
    CMD_USER_CODE_BASE
} e_cmderr_t;

/**
 * @brief Function prototype to handle commands passed to the parser
 * @param iArgCount - The number of argument strings in the array
 * @param ppszArgument - Pointer to the argument list
 * @param pCom - Pointer to the command object
 * @return  for success otherwise error code
 */
typedef int16_t (* const CMDFUNC) (int_t iArgCount, char_t **ppszArgument, pst_comset_t pCom);

/** Structure of a set of commands used by the command line parser
 * Each command set requires one of these */
typedef struct
{
    char_t *p_command;                     /* The command letters */
    const CMDFUNC function;                     /* Pointer to the handling function */
    char_t *p_command_description;         /* A pointer to the command description */
} st_cmdfnass_t;

typedef st_cmdfnass_t *pst_cmdfnass_t;

struct _CMDTAB
{
    uint8_t group_name[32];              /** Optional name for group */
    pst_cmdfnass_t command_list;
    uint32_t number_of_commands;        /** The number of commands in the table */
};

/** Define the escape key handler function type */
typedef e_cmdesc_t (* const PESCFN) (pst_comset_t);

/******************************************************************************
 Variable External definitions and Function External definitions
 *****************************************************************************/

/******************************************************************************
 Exported global functions (to be accessed by other files)
 *****************************************************************************/

/******************************************************************************
 Function Prototypes
 *****************************************************************************/


#endif /* CONSOLE_H_ */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
/******************************************************************************
 End  Of File
 *****************************************************************************/
