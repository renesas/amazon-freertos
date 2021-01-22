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
 * File Name    : phy.h
 * Version      : 1.01
 * Description  : Ethernet PHY device driver
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
    #include <stdint.h>

/* Guards against multiple inclusion */
#ifndef PHY_H
    #define PHY_H

#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* PHY return definitions */
    #define R_PHY_OK        (0)
    #define R_PHY_ERROR     (-1)

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
typedef enum LinkStatE
{
    PHY_NO_LINK = 0, PHY_LINK_10H, PHY_LINK_10F, PHY_LINK_100H, PHY_LINK_100F
} linkstat_t;

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/*********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 *******************************************************************************************************************/
/**********************************************************************************************************************
 * @fn            phy_init
 * @brief         Resets Ethernet PHY device.
 * @warning       None.
 * @param [in]    uint32_t channel: Channel number.
 * @param [out]   None.
 * @retval        R_PHY_OK - success
 *                R_PHY_ERROR - error.
 *********************************************************************************************************************/
extern int16_t phy_init (uint32_t ether_channel);

/**********************************************************************************************************************
 * @fn            phy_start_autonegotiate
 * @brief         Starts auto-negotiate.
 * @warning       None.
 * @param [in]    uint32_t ether_channel - Ethernet channel number.
 *                uint8_t pause - Using state of pause frames
 * @param [out]   None.
 * @retval        None.
 *********************************************************************************************************************/
extern void phy_start_autonegotiate (uint32_t ether_channel, uint8_t pause);

/**********************************************************************************************************************
 * @fn            phy_set_autonegotiate
 * @brief         Reports the other side's physical capability.
 * @warning       The value returned to local_pause and patner_pause is used
 *                as it is as an argument of ether_pause_resolution function.
 * @param [in]    uint32_t ether_channel - Ethernet channel number.
 * @param [out]   uint16_t *pline_speed_duplex - a pointer to the location of both the line speed and the duplex.
 *                uint16_t *plocal_pause - a pointer to the location to store the local pause bits.
 *                uint16_t *ppartner_pause - a pointer to the location to store the partner pause bits.
 * @retval        R_PHY_OK.
 *                R_PHY_ERROR
 *********************************************************************************************************************/
extern int16_t phy_set_autonegotiate (uint32_t ether_channel, uint16_t *pline_speed_duplex, uint16_t *plocal_pause,
        uint16_t *ppartner_pause);

/**********************************************************************************************************************
 * @fn            phy_get_link_status
 * @brief         Returns the status of the physical link.
 * @warning       None.
 * @param [in]    uint32_t ether_channel - Ethernet channel number.
 * @param [out]   None.
 * @retval        -1 if links is down, 0 otherwise.
 *********************************************************************************************************************/
extern int16_t phy_get_link_status (uint32_t ether_channel);

#ifdef __cplusplus
}
#endif

#endif /* PHY_H */

