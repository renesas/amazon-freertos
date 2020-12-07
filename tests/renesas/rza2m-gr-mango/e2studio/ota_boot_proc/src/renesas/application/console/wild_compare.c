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
 * Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : wild_compare.c
 * Version      : 1.01
 * Device(s)    : Renesas
 * Tool-Chain   : N/A
 * OS           : N/A
 * H/W Platform : EBK
 * Description  : Function to perform a string compare with * wild card.
 **********************************************************************************************************************
 * History      : DD.MM.YYYY Version Description
 *              : 04.02.2010 1.00    First Release
 *              : 10.06.2010 1.01    Updated type definitions
 *             : __#dd#__.__#mm#__.__#yyyy#__ __#ver_maj#__.__#ver_min#__ First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <string.h>
#include <ctype.h>

#include "wild_compare.h"
#include "r_typedefs.h"

/**********************************************************************************************************************
 Function Prototypes
 *********************************************************************************************************************/

static const char_t *wild_char(const char *pszString, char chNext);

/**********************************************************************************************************************
 Exported global variables and functions (to be accessed by other files)
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wild_compare
 * Description  : Function to check a string against a wild card string where the
 *                char '*' indicates any number of characters. The application
 *                of this is to match file names like "*.txt" in this MS demo.
 * Arguments    : IN  pszWildCard - Pointer to the string containing the wild
 *                                  card
 *                IN  pszString - Pointer to the string to compare
 * Return Value : true if the string matches the wild card string.
 *********************************************************************************************************************/
bool_t wild_compare(const char_t * pszWildCard, const char_t * pszString)
{
    /* Check to see if the string contains the wild card */
    if (!memchr(pszWildCard, '*', strlen(pszWildCard)))
    {
        /* if it does not then do a straight case free string compare */
        while (tolower(*pszWildCard) == tolower(*pszString))
        {
            /* Check for end of string */
            if (!*pszWildCard++)
            {
                /* Match */
                return (true);
            }

            pszString++;
        }

        return (false);
    }
    else
    {
        while ((*pszWildCard) && (*pszString))
        {
            /* Test for the wild card */
            if ('*' == (*pszWildCard))
            {
                /* Eat more than one */
                while ('*' == (*pszWildCard))
                {
                    pszWildCard++;
                }

                /* If there are more chars in the string */
                if (*pszWildCard)
                {
                    /* Search for the next char */
                    pszString = wild_char(pszString, *pszWildCard);

                    /* if it does not exist then the strings don't match */
                    if (!pszString)
                    {
                        return (false);
                    }
                }
                else
                {
                    if (*pszWildCard)
                    {
                        /* continue */
                        break;
                    }

                    return (true);
                }
            }
            else
            {
                /* Fail if they don't match */
                if (tolower(*pszWildCard) != tolower(*pszString))
                {
                    return (false);
                }
            }

            /* Bump both pointers */
            pszWildCard++;
            pszString++;
        }

        /* fail if different lengths */
        if ((*pszWildCard) != (*pszString))
        {
            return (false);
        }
    }

    return (true);
}
/**********************************************************************************************************************
 End of function  wild_compare
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private global variables and functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wild_char
 * Description  : Function to return a pointer to the next character in the
 *                string
 * Arguments    : IN  pszString - Pointer to the string to search
 *                IN  chNext - The next character to look for
 * Return Value : Pointer to the first occurrence of the next character or NULL
 *                if not found
 *********************************************************************************************************************/
static const char_t *wild_char(const char_t *pszString, char_t chNext)
{
    chNext = (char_t) tolower(chNext);

    while (*pszString)
    {
        if (tolower(*pszString) == chNext)
        {
            return (pszString);
        }

        pszString++;
    }

    return (NULL);
}
/**********************************************************************************************************************
 End of function wild_char
 *********************************************************************************************************************/

/**********************************************************************************************************************
 End Of File
 *********************************************************************************************************************/
