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
/**
  ******************************************************************************
  * @file    entropy_hardware_poll.c
  *
  ******************************************************************************
  */

#include <string.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#define __LIT (1)

extern int32_t cryptogram_sample(int32_t seed_in);

void get_random_number(uint8_t *data, uint32_t len);

/**********************************************************************************************************************
 * Function Name: mbedtls_hardware_poll
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *********************************************************************************************************************/
int mbedtls_hardware_poll( void *data,
                    unsigned char *output, size_t len, size_t *olen )
{
    /* unused argument */
    ((void) data);
    /* unused argument */
    ((void) len);

    uint32_t random_number = 0;

    /* get random number  */
    get_random_number((uint8_t *)&random_number, sizeof(uint32_t));
    *olen = 0;

    memcpy(output, &random_number, sizeof(uint32_t));
    *olen = sizeof(uint32_t);

    return 0;
} /* End of function mbedtls_hardware_poll */

/******************************************************************************
Functions : random number generator(XorShift method)

WARNING: The random number generation solution presented in this application is 
    for demonstration purposes only. It is not recommended to go into production with 
    the logic presented here. The current solution takes entropy from the a
    temperature sensor on the board and from the current system time. For 
    production development, Renesas RZ/A2M customers are recommended to use the 
    TRNG included in the Trusted Secure IP Driver. Please see the following for more information
    on the Trusted Secure IP Driver:
    https://www.renesas.com/us/en/products/software-tools/software-os-middleware-driver/security-crypto/trusted-secure-ip-driver.html

******************************************************************************/
void get_random_number(uint8_t *data, uint32_t len)
{
    static uint32_t y = 2463534242;
    uint32_t res;
    uint32_t lp;
    uint8_t *p_data;
    y += xTaskGetTickCount();   /* randomness from system timer */
    y = cryptogram_sample(y);   //190325 modified for RZ/A2M. For random number generator

    res = len / 4;
    for (lp = 0; lp < res; lp++)
    {
        y = y ^ (y << 13);
        y = y ^ (y >> 17);
        y = y ^ (y << 5);
        /* set data pointer */
        p_data = (uint8_t*) & y;
#if __LIT
        /* change endian */
        *((uint32_t *)data) = (uint32_t)((*(p_data + 3) << 24) | (*(p_data + 2) << 16) | (*(p_data + 1) << 8) | *(p_data + 0));
#else
        *((uint32_t *)data) = y;
#endif
        data += 4;
    }
    y = y ^ (y << 13);
    y = y ^ (y >> 17);
    y = y ^ (y << 5);
    /* calc res */
    res = (uint32_t)len % 4;

    /* set data pointer */
    p_data = (uint8_t*) & y;
    switch (res)
    {
        case 3:
#if __LIT
            *data++ = p_data[3];
            *data++ = p_data[2];
            *data++ = p_data[1];
#else
            *data++ = p_data[0];
            *data++ = p_data[1];
            *data++ = p_data[2];
#endif
            break;

        case 2:
#if __LIT
            *data++ = p_data[3];
            *data++ = p_data[2];
#else
            *data++ = p_data[0];
            *data++ = p_data[1];
#endif
            break;

        case 1:
#if __LIT
            *data++ = p_data[3];
#else
            *data++ = p_data[0];
#endif
            break;

        default:
            /* no op */
            break;
    }

} /* End of get_random_number */
