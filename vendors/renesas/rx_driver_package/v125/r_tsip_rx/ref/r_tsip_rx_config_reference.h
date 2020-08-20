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
 * Copyright (C) 2015-2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_tsip_rx_if.h
 * Version      : 1.09
 * Description  : Interface definition for the r_tsip_rx module.
 *                TSIP means the "Trusted Secure IP" that is Renesas original security IP.
 *                Strong point 1:
 *                 TSIP can hide the "Critical Security Parameter (CSP)" inside the "Cryptographic Boundary".
 *                 These words are defined in NIST FIPS140-2.
 *                Strong point 2:
 *                 TSIP can support AES, SHA, DES, RSA, ECC, Key Wrap, TRNG(with DRBG),
 *                                                             already certified NIST CAVP test.
 *                 TSIP-Lite can support AES, TRNG(with DRBG), already certified NIST CAVP test.
 *                Strong point 3:
 *                 TSIP can accelerate some crypto operation.
 *                Supported Device:
 *                 TSIP = RX651, RX65N, RX66N, RX72M, RX72N
 *                 TSIP-Lite = RX231, RX23W, RX66T, RX72T
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 27.06.2015 1.00     First Release
 *         : 15.05.2017 1.01     Add AES-CMAC
 *         : 30.09.2017 1.03     Add Init/Update/Final API and SHA, RSA
 *         : 28.02.2018 1.04     Change Init/Update/Final API for RX231, add TLS function and 
 *         :                     return values change FIT rules.
 *         : 30.04.2018 1.05     Add TDES, MD5 and RSAES-PKCS1-v1_5 API
 *         : 28.09.2018 1.06     Add RSA Key Generation, AES, TDES, RSA Key update features, RX66T support
 *         : 28.12.2018 1.07     Add RX72T support
 *         : 30.09.2019 1.08     Added support for GCC and IAR compiler, ECC API, RX23W and RX72M
 *         : 31.03.2020 1.09     Added support for AES-CCM, HMAC key generation, ECDH, Key Wrap API, RX66N and RX72N
 *         : 31.03.2020 1.09-lib Release library version. Added supprot for ARC4
 *********************************************************************************************************************/
#ifndef R_TSIP_RX_CONFIG_HEADER_FILE
#define R_TSIP_RX_CONFIG_HEADER_FILE

/***********************************************************************************************************************
 Configuration Options
 **********************************************************************************************************************/
/*
	None
*/

#endif /* R_TSIP_RX_CONFIG_HEADER_FILE */
