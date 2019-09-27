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
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : console.c
* Version      : 1.01
* Device(s)    : Renesas
* Tool-Chain   : N/A
* OS           : N/A
* H/W Platform : RSK+
* Description  : Simple command line console implemenation
*******************************************************************************
* History      : DD.MM.YYYY Version Description
*              : 04.02.2010 1.00    First Release
*              : 10.06.2010 1.01    Updated type definitions
******************************************************************************/



/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wild_compare.h>

#include "compiler_settings.h"
#include "r_os_abstraction_api.h"

#include "console.h"
#include "driver.h"
#include "version.h"

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/

/******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

static int16_t con_print_prompt(pst_comset_t pCom);
static e_cmderr_t con_parse_command(pst_comset_t pCom);
static e_cmderr_t process_ordinary_char(pst_comset_t pCom, char chChar, _Bool *pbfCommand);

/* Terminal window escape sequences */
static const char * const gsp_clear_screen = "\x1b[2J";
static const char * const gsp_cursor_home = "\x1b[H";

/******************************************************************************
 Public Functions
 ******************************************************************************/

/******************************************************************************
 * Function Name: show_welcome_msg
 * Description  : Function to display initial welcome message
 * Arguments    : IN  pCom - Pointer to the command object data
 *              : IN  clear_screen - clear screen before message  true/false
 * Return Value : None
 *****************************************************************************/
void show_welcome_msg (FILE *p_out, bool_t clear_screen)
{
    st_os_abstraction_info_t ver_info;

	fprintf(p_out,"RZ/A2M blinky_sample for GCC Ver. %u.%u\r\n", APPLICATION_INFO_VERSION, APPLICATION_INFO_RELEASE);
	fprintf(p_out,"Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.\r\n");
	fprintf(p_out,"Build Info Date %s at %s \r\n", __DATE__, __TIME__);

	if(R_OS_GetVersion(&ver_info) == 0)
	{
		fprintf(p_out,"%s Version %d.%d\r\n", ver_info.p_szdriver_name, ver_info.version.sub.major, ver_info.version.sub.minor);
	}

    fflush(p_out);
}
/******************************************************************************
 End of function show_welcome_msg
 ******************************************************************************/

/******************************************************************************
 Function Name: console
 Description:   Function to implement a simple command console using the ANSI C
                run time library IO functions
 Arguments:     IN  pCom - Pointer to the command object data
                IN  pCmdFunctions - Pointer to a table of command tables
                IN  pIn - Pointer to the file stream for input
                IN  pOut - Pointer to the file stream for output
                IN  pszPrompt - Pointer to the prompt string
 Return value:  0 for success otherwise error code
 ******************************************************************************/
e_cmderr_t console(pst_comset_t pCom, cpst_command_table_t *ppComFunctions, int32_t iNumTables, FILE *pIn, FILE *pOut, char *pszPrompt)
{
    /* Initialise our variables */
    memset(pCom, 0, sizeof(st_comset_t));
    pCom->p_in = pIn;
    pCom->p_out = pOut;
    pCom->p_function = ppComFunctions;
    pCom->num_tables = (int16_t)iNumTables;
    pCom->p_prompt = pszPrompt;
    pCom->default_prompt = pszPrompt;
    pCom->working_drive = -1;

    /* Initialise the last command with the help string */
    strcpy(pCom->va.command, "?");

    /* Print a prompt */
    con_print_prompt(pCom);
    fflush(pOut);

    /* Disable buffering for single character input */
    setvbuf(pIn, NULL, _IONBF, 0);

    /* Until there is an error */
    while (true)
    {
        _Bool bf_command = false;

        /* cast 0 to CMDERR */
        e_cmderr_t error_code = (e_cmderr_t) 0;
        int_t data;

        /* Read a character from the input stream */
        data = fgetc(pIn);
        if (EOF == data)
        {
            /* cast to void */
            clearerr(pIn);
            return CMD_ERROR_IN_IO;
        }

        pCom->va.data = (int16_t)data;

        /* Bump the read count */
        pCom->va.read_count++;

        /* Process the character */
        fflush(pOut);

        /* cast integer to char */
        error_code = con_process_char(pCom, (char) pCom->va.data, &bf_command);

        /* If an error occurs then return it */
        if (error_code > CMD_UNKNOWN)
        {
            return error_code;
        }

        /* If a command has been received then print the prompt if required */
        if ((bf_command) && (CMD_NO_PROMPT != error_code))
        {
            if (con_print_prompt(pCom) < 0)
            {
                return CMD_ERROR_IN_IO;
            }
        }

        fflush(pOut);
    }

    return CMD_OK;
}
/******************************************************************************
 End of function console
 ******************************************************************************/

/******************************************************************************
 Function Name: con_get_last_command_line
 Description:   Function to bring up the last command line for editing
 Arguments:     IN  pCom - pointer to the command object
 Return value:  0 for success otherwise error code
 ******************************************************************************/
void con_get_last_command_line(pst_comset_t pCom)
{
    strcpy(pCom->va.buffer, pCom->va.command);
    fprintf(pCom->p_out, "%s", pCom->va.buffer);

    /* cast size_t to uint32_t */
    pCom->va.buffer_index = (uint32_t) strlen(pCom->va.command);
}
/******************************************************************************
 End of function con_get_last_command_line
 ******************************************************************************/

/******************************************************************************
 Function Name: con_do_last_command_line
 Description  : Function to get do the last command again
 Arguments    : IN  pCom - pointer to command table
 Return Value : 0 for success otherwise error code
 ******************************************************************************/
e_cmderr_t con_do_last_command_line(pst_comset_t pCom)
{
    con_get_last_command_line(pCom);
    fprintf(pCom->p_out, "\r\n");

    return con_parse_command(pCom);
}
/******************************************************************************
End of function con_do_last_command_line
******************************************************************************/

/******************************************************************************
Function Name: process_ordinary_char
Description:   Function to process an ordinary character
Arguments:     IN  pCom - pointer to the command object
               IN  chChar - The character to process
               IN  pbfCommand - Pointer to a flag that is set when a
                                command is received
Return value:  0 for success otherwise error code
******************************************************************************/
static e_cmderr_t process_ordinary_char(pst_comset_t pCom, char chChar, _Bool *pbfCommand)
{
    if (pCom->va.buffer_index < (CMD_READER_LINE_SIZE - 1))
    {
        /* buffer */
        pCom->va.buffer[pCom->va.buffer_index++] = chChar;

        /* Two character escape sequence termination tests - F Keys */
        if ((ESC_ESCAPE_SEQUENCE == pCom->va.escape_sequence) && (2 == pCom->va.buffer_index) && ('O' == (*pCom->va.buffer)))
        {
            pCom->va.buffer[pCom->va.buffer_index] = '\0';
            *pbfCommand = true;

            /* Parse the command line */
            return con_parse_command(pCom);
        }
        /* Arrow keys */
        else if ((ESC_ESCAPE_SEQUENCE == pCom->va.escape_sequence) && (2 == pCom->va.buffer_index) && ('[' == (*pCom->va.buffer)) && ('1' != chChar))
        {
            pCom->va.buffer[pCom->va.buffer_index] = '\0';
            *pbfCommand = 1;
            return con_parse_command(pCom);
        }
        else
        {
            /* do nothing */
            __asm ("nop");
        }
    }
    else
    {
        return CMD_LINE_TOO_LONG;
    }

#ifdef SERIAL
    /* echo to console */
    if ((ESC_NO_ESCAPE == pCom->va.escape_sequence) && ('@' != pCom->va.buffer[0]))
    {
        if (pCom->secrecy)
        {
            fputc('*', pCom->p_out);
        }
        else
        {
            fputc(chChar, pCom->p_out);
        }
    }
#endif

    return CMD_OK;
}
/******************************************************************************
 End of function process_ordinary_char
 ******************************************************************************/

/******************************************************************************
 Function Name: terminate_command
 Description:   Function to process an ordinary character
 Arguments:     IN  pCom - pointer to the command object
                IN  chChar - The character to process
                IN  pbfCommand - Pointer to a flag that is set when a
                                 command is received
 Return value:  0 for success otherwise error code
 ******************************************************************************/
static e_cmderr_t terminate_command(pst_comset_t pCom, char chChar, _Bool *pbfCommand)
{
    (void)chChar;

    if ((ESC_NO_ESCAPE == pCom->va.escape_sequence) && ('@' != pCom->va.buffer[0]))
    {
#ifdef SERIAL
        fprintf(pCom->p_out, "\r\n");
#endif
    }

    /* Terminate the string */
    pCom->va.buffer[pCom->va.buffer_index] = (uint8_t) '\0';
    *pbfCommand = true;

    /* Parse the command line */
    return con_parse_command(pCom);
}
/******************************************************************************
 End of function terminate_command
 ******************************************************************************/

/******************************************************************************
 Function Name: con_process_char
 Description:   Function to process a character
 Arguments:     IN  pCom - pointer to the command object
                IN  chChar - The character to process
                IN  pbfCommand - Pointer to a flag that is set when a
                                 command is received
 Return value:  0 for success otherwise error code
 ******************************************************************************/
e_cmderr_t con_process_char(pst_comset_t pCom, char chChar, _Bool *pbfCommand)
{
    *pbfCommand = false;

    switch (chChar)
    {
        /* Ignore escape sequence start */
        case CMD_ESCAPE_CHARACTER:
        {
            /* suppress echo */
            pCom->va.escape_sequence = ESC_ESCAPE_SEQUENCE;
            pCom->va.buffer_index = 0;
        }
        break;

        case '\b':                            /* White out on back space */
        case  0x7F:                            /* PuTTY sends non-ASCII back space */
        {
            if (0 != pCom->va.buffer_index)
            {
#ifdef SERIAL
                fprintf(pCom->p_out, "\b \b");
#endif
                pCom->va.buffer_index--;
            }
        }
        break;

#ifdef SERIAL
        case '\n':                            /* New line - ignore */
        {
            __asm ("nop");
        }
        break;
#endif

        /* End of escape sequence */
        case ';':
        {
            if (ESC_NO_ESCAPE == pCom->va.escape_sequence)
            {
                return process_ordinary_char(pCom, chChar, pbfCommand);
            }

               return terminate_command(pCom, chChar, pbfCommand);
        }

        case '~':
        {
            if (ESC_NO_ESCAPE == pCom->va.escape_sequence)
            {
                return process_ordinary_char(pCom, chChar, pbfCommand);
            }

            return terminate_command(pCom, chChar, pbfCommand);
        }

        case 0:
        {
            if (ESC_NO_ESCAPE == pCom->va.escape_sequence)
            {
                break;
            }

            return terminate_command(pCom, chChar, pbfCommand);
        }

#ifdef SERIAL
        case '\r':                            /* return - do function */
#else
        case '\n':
#endif
        {
            return terminate_command(pCom, chChar, pbfCommand);
        }

        /* All other characters */
        default:
        {
            return process_ordinary_char(pCom, chChar, pbfCommand);
        }
    }

    return CMD_OK;
}
/******************************************************************************
 End of function con_process_char
 ******************************************************************************/

/******************************************************************************
 Function Name: con_print_prompt
 Description  : Function to print the command prompt
 Arguments    : IN  pCom - pointer to the command object
 Return Value : 0 for success otherwise error code
 ******************************************************************************/
static int16_t con_print_prompt(pst_comset_t pCom)
{
    int16_t result = 0;

    /* Don't print a NULL prompt */
    if (pCom->p_prompt)
    {
        fflush(pCom->p_out);
        result = (int16_t)fprintf(pCom->p_out, "\r\n%s ",  pCom->p_prompt);
    }

    return result;
}
/******************************************************************************
 End of function con_print_prompt
 ******************************************************************************/

/******************************************************************************
 Function Name: con_split_line
 Description  : Function to split the command line in to the argument array
 Arguments    : IN  pszLine - Pointer to the command line
                IN  pszArguments - Pointer to the arguments
 Return Value : The number of arguments split
 ******************************************************************************/
static int con_split_line(char *pszLine, char **pszArguments)
{
    int16_t arg_count;
    int16_t count;
    char * p_argument;

    for (arg_count = 0; (arg_count < CMD_MAX_ARG) && (*pszLine); arg_count++)
    {
        p_argument = pszArguments[arg_count];

        /* Look for the first white space */
        while ((*pszLine) && (isspace(*pszLine)))
        {
            pszLine++;
        }

        /* Initialise the argument length counter */
        count = 0;

        /* Allow inverted commas to specify file names with spaces in */
        if ('\"' == (*pszLine))
        {
            pszLine++;

            /* Copy the string until the next inverted comma or end of line */
            while ((*pszLine) && ('\"' != (*pszLine)))
            {
                count++;
                if (count >= CMD_MAX_ARG_LENGTH)
                {
                    *p_argument = '\0';
                    break;
                }

                *p_argument++ = *pszLine++;
            }

            /* Skip the " character */
            pszLine++;
        }

        /* Copy and argument until the end of the line or the next space */
        while ((*pszLine) && (!isspace(*pszLine)))
        {
            count++;
            if (count >= CMD_MAX_ARG_LENGTH)
            {
                *p_argument = '\0';
                break;
            }

            *p_argument++ = *pszLine++;
        }
    }

    if (arg_count > CMD_MAX_ARG)
    {
        arg_count = CMD_MAX_ARG;
    }

    return arg_count;
}
/******************************************************************************
 End of function con_split_line
 ******************************************************************************/

/******************************************************************************
 Function Name: con_init_arg_list
 Description  : Function to initialise the argument list
 Arguments    : IN  pCom - pointer to the command object
 Return Value : none
 ******************************************************************************/
static void con_init_arg_list(pst_comset_t pCom)
{
    int8_t count = CMD_MAX_ARG;

    /* cast to char ** */
    char **p_arguments = (char **) pCom->va.arguments;

    /* cast to char * */
    char *p_argument = ((char *) pCom->va.arguments) + (CMD_MAX_ARG * sizeof(char *));

    /* Initialise all the data */
    memset(pCom->va.arguments, 0, (CMD_MAX_ARG * CMD_MAX_ARG_LENGTH));

    /* Set the pointers to the strings */
    while (count--)
    {
        *p_arguments++ = p_argument;
        p_argument += CMD_MAX_ARG_LENGTH;
    }
}
/******************************************************************************
 End of function con_init_arg_list
 ******************************************************************************/

/******************************************************************************
 Function Name: con_execute
 Description  : Function to execute the command function
 Arguments    : IN  pCom - pointer to the command object
                IN  pCmdList - pointer to a list of commands to match against
                OUT pbfValidCommand - pointer to a flag set true when command
                                      matched
 Return Value : 0 for success otherwise error code
 ******************************************************************************/
static e_cmderr_t con_execute(pst_comset_t pCom, cpst_command_table_t pCmdList, _Bool *pbfValidCommand)
{
    /* Check for a valid command list pointer */
    if (NULL != pCmdList)
    {
        uint32_t cmd_len;
        uint32_t length;
        int16_t func_index;
        int8_t arg_count;

        /* Initialise the argument list */
        con_init_arg_list(pCom);

        /* Split the command line */
        arg_count = (int8_t)con_split_line(pCom->va.buffer, (char **) pCom->va.arguments);

        /* Get the length of the command */
        cmd_len = (uint32_t) strlen((*(char **) pCom->va.arguments));

        /* check against list of command tokens */
        for (func_index = 0; func_index < (int) pCmdList->number_of_commands; func_index++)
        {
            /* Get pointer to the associated command */
            char * p_string = pCmdList->command_list[func_index].p_command;

            /* Calculate command length */
            length = (uint32_t) strlen(p_string);

            /* Check the length of the command */
            if (cmd_len == length)
            {
                /* Command line has been matched */
                if (wild_compare(p_string, (*(char **) pCom->va.arguments)))
                {
                    /* Show that we are processing a valid command */
                    *pbfValidCommand = true;

                    /* Execute the command */
                    return (e_cmderr_t) pCmdList->command_list[func_index].function(arg_count, (char **) pCom->va.arguments, pCom);
                }
            }
        }
    }

    /* No command matched */
    *pbfValidCommand = false;

    return CMD_OK;
}
/******************************************************************************
 End of function con_execute
 ******************************************************************************/

/******************************************************************************
 Function Name: con_parse_command
 Description  : Function to pars the command and call the handling function
 Arguments    : IN  pCom - pointer to command table
 Return Value : 0 for success otherwise error code
 ******************************************************************************/
static e_cmderr_t con_parse_command(pst_comset_t pCom)
{
    uint32_t eat_space_count = 0U;
    uint32_t length;
    _Bool valid_command = false;
    int16_t count = 0;
    e_cmderr_t error_code = CMD_OK;

    /* Eat any white space before the command */
    while ((' ' == pCom->va.buffer[eat_space_count]) || ('\t' == pCom->va.buffer[eat_space_count])
            || ('@' == pCom->va.buffer[eat_space_count]))
    {
        eat_space_count++;
    }

    length = pCom->va.buffer_index - eat_space_count;
    pCom->va.buffer_index = 0;

    /* Test for valid command or just CR */
    if (length >= 1)
    {
        /* If it is not an escape sequence */
        if (ESC_NO_ESCAPE == pCom->va.escape_sequence)
        {
            /* Copy to last command buffer */
            strcpy(pCom->va.command, pCom->va.buffer);
        }
        else
        {
            /* Cancel escape sequence */
            pCom->va.escape_sequence = ESC_NO_ESCAPE;
        }

        /* Execute the command function */
        while (count < pCom->num_tables)
        {
            error_code = con_execute(pCom, pCom->p_function[count], &valid_command);

            if (!valid_command)
            {
                count++;
            }
            else
            {
                break;
            }
        }

        /* Test to see if command is valid */
        if (!valid_command)
        {
            fprintf(pCom->p_out, "\r\n\"%s\" Unknown command\r\n", pCom->va.buffer);
        }
    }

    return error_code;
}
/******************************************************************************
 End of function con_parse_command
 ******************************************************************************/

/******************************************************************************
End  Of File
******************************************************************************/
