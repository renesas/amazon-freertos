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
/*******************************************************************************
* System Name  : SDHI Driver
* File Name    : sd_mount.c
* Version      : 1.31
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Card mount
* Operation    :
* Limitations  : None
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 16.03.2018 1.00     First Release
*         : 14.12.2018 1.01     Changed the DMAC soft reset procedure.
*         : 28.12.2018 1.02     Support for OS
*         : 29.05.2019 1.20     Correspond to internal coding rules
*         : 17.09.2019 1.30     Support for SDIO
*         : 12.11.2019 1.31     Replaces the register access with iodefine
*         : 31.03.2020 1.50     Support high speed for SDIO
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "r_sdif.h"
#include "sd.h"
#include "iodefine.h"

#ifdef __CC_ARM
#pragma arm section code = "CODE_SDHI"
#pragma arm section rodata = "CONST_SDHI"
#pragma arm section rwdata = "DATA_SDHI"
#pragma arm section zidata = "BSS_SDHI"
#endif

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
static uint16_t s_stat_buff[NUM_PORT][64 / sizeof(uint16_t)];

static int32_t _sd_mount_error(st_sdhndl_t *p_hndl);
static int32_t _sd_card_init_get_rca(st_sdhndl_t *p_hndl);
static int32_t _sd_io_mount_error(st_sdhndl_t *p_hndl);
static int32_t _sd_mem_mount_error(st_sdhndl_t *p_hndl);
static int32_t _sd_read_byte_error(st_sdhndl_t *p_hndl);
static int32_t _sd_write_byte_error(st_sdhndl_t *p_hndl);

/******************************************************************************
 * Function Name: sd_mount
 * Description  : mount SD card.
 *              : mount SD memory card user area
 *              : can be access user area after this function is finished
 *              : without errors
 *              : turn on power
 *              :
 *              : following is available SD Driver mode
 *              : SD_MODE_POLL     : software polling
 *              : SD_MODE_HWINT    : hardware interrupt
 *              : SD_MODE_SW       : software data transfer (SD_BUF)
 *              : SD_MODE_DMA      : DMA data transfer (SD_BUF)
 *              : SD_MODE_MEM      : only memory cards
 *              : SD_MODE_IO       : memory and io cards
 *              : SD_MODE_COMBO    : memory ,io and combo cards
 *              : SD_MODE_DS       : only default speed
 *              : SD_MODE_VER1X    : ver1.1 host
 *              : SD_MODE_VER2X    : ver2.x host
 * Arguments    : int32_t sd_port  : channel no (0 or 1)
 *              : uint32_t mode    : SD Driver operation mode
 *              : uint32_t voltage : operation voltage
 * Return Value : p_hndl->error    : SD handle error value
 *              : SD_OK : end of succeed
 *              : other : end of error
 * Remark       : user area should be mounted
 *****************************************************************************/
int32_t sd_mount(int32_t sd_port, uint32_t mode, uint32_t voltage)
{
    st_sdhndl_t *p_hndl;
    uint64_t    info1_back;
    uint8_t     io_buff;
    uint16_t    sd_spec;
    uint16_t    sd_spec3;

    if ((0 != sd_port) && (1 != sd_port))
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initilized */
    }

    /* ==== check work buffer is allocated ==== */
    if (0 == p_hndl->p_rw_buff)
    {
        return SD_ERR;  /* not allocated yet */
    }

    if (mode & SD_MODE_IO)
    {
        if ( (p_hndl->sup_card & 0x30u) == (mode & 0x30u) )
        {
            /* support SDIO card */
            if ((SD_MEDIA_IO == p_hndl->media_type) || (SD_MEDIA_COMBO == p_hndl->media_type))
            {
                /* media has SDIO */
                if (p_hndl->io_flag & SD_IO_POWER_INIT)    /* already supplied power */
                {
                    /* ==== transfer idle state (issue CMD52) ==== */
                    /* data:08'h func:0 address:06'h verify write */
                    io_buff = 0x08;
                    if (sdio_write_direct(sd_port, &io_buff, 0, 0x06, SD_IO_VERIFY_WRITE) != SD_OK)
                    {
                        return SD_ERR;
                    }
                }
            }
        }
    }

    /* ==== initialize parameter ==== */
    _sd_init_hndl(p_hndl, mode, voltage);
    p_hndl->error = SD_OK;

    /* ==== is card inserted? ==== */
    if (_sd_check_media(p_hndl) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_NO_CARD);
        return p_hndl->error;     /* not inserted */
    }

    /* ==== power on sequence ==== */
    /* ---- turn on voltage ---- */
    if (sddev_power_on(sd_port) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return _sd_mount_error(p_hndl);
    }

    /* ---- set single port ---- */
    _sd_set_port(p_hndl, SD_PORT_SERIAL);

    /* ---- supply clock (card-identification ratio) ---- */
    if (_sd_set_clock(p_hndl, SD_CLK_400KHZ, SD_CLOCK_ENABLE) != SD_OK)
    {
        return p_hndl->error;     /* not inserted */
    }

    sddev_int_wait(sd_port, 2); /* add wait function  */

    sddev_loc_cpu(sd_port);

    /* Cast to an appropriate type */
    info1_back = SDMMC.SD_INFO1.LONGLONG;
    
    /* Cast to an appropriate type */
    info1_back &= (uint64_t)0xfff8;

    /* Cast to an appropriate type */
    SDMMC.SD_INFO1.LONGLONG = (uint64_t)info1_back;
    
    /* Cast to an appropriate type */
    SDMMC.SD_INFO2.LONGLONG = (uint64_t)0;
    
    /* Clear DMA Enable because of CPU Transfer */
    SDMMC.CC_EXT_MODE.LONGLONG = (uint64_t)(SDMMC.CC_EXT_MODE.LONGLONG & ~CC_EXT_MODE_DMASDRW); /* disable DMA  */

    sddev_unl_cpu(sd_port);

    /* ==== initialize card and distinguish card type ==== */
    if (_sd_card_init(p_hndl) != SD_OK)
    {
        return _sd_mount_error(p_hndl);  /* failed card initialize */
    }

    if (p_hndl->media_type & SD_MEDIA_MEM)    /* with memory part */
    {
        /* ==== check card registers ==== */
        /* ---- check CSD register ---- */
        if (_sd_check_csd(p_hndl) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }

        /* ---- no check other registers (to be create) ---- */

        /* get user area size */
        if (_sd_get_size(p_hndl, SD_USER_AREA) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }

        /* check write protect */
        p_hndl->write_protect |= (uint8_t)_sd_iswp(p_hndl);
    }

    if (p_hndl->media_type & SD_MEDIA_IO)   /* with IO part */
    {
        if (_sd_io_mount(p_hndl) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }
    }

    if (p_hndl->media_type & SD_MEDIA_MEM)    /* with memory part */
    {
        if (_sd_mem_mount(p_hndl) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }
        if (SD_ERR_CARD_LOCK == p_hndl->error)
        {
            p_hndl->mount = (SD_CARD_LOCKED | SD_MOUNT_LOCKED_CARD);

            /* ---- halt clock ---- */
            _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
            return SD_OK_LOCKED_CARD;
        }
    }

    /* if SD memory card, get SCR register */
    if (p_hndl->media_type & SD_MEDIA_SD)
    {
        if (_sd_card_get_scr(p_hndl) != SD_OK)
        {
            return _sd_mount_error(p_hndl);
        }

        if (SD_SPEC_20 == p_hndl->sd_spec)
        {
            /* Cast to an appropriate type */
            sd_spec = (uint16_t)(p_hndl->scr[0] & SD_SPEC_REGISTER_MASK);

            /* Cast to an appropriate type */
            sd_spec3 = (uint16_t)(p_hndl->scr[1] & SD_SPEC_30_REGISTER);
            if ((SD_SPEC_20_REGISTER == sd_spec) && (SD_SPEC_30_REGISTER == sd_spec3))
            {
                /* ---- more than phys spec ver3.00 ---- */
                p_hndl->sd_spec = SD_SPEC_30;
            }
            else     /* ---- phys spec ver2.00 ---- */
            {
                p_hndl->sd_spec = SD_SPEC_20;
            }
        }
        else
        {
            /* Cast to an appropriate type */
            sd_spec = (uint16_t)(p_hndl->scr[0] & SD_SPEC_REGISTER_MASK);
            if (SD_SPEC_11_REGISTER == sd_spec)   /* ---- phys spec ver1.10 ---- */
            {
                p_hndl->sd_spec = SD_SPEC_11;
            }
            else    /* ---- phys spec ver1.00 or ver1.01 ---- */
            {
                p_hndl->sd_spec = SD_SPEC_10;
            }
        }

        /* Cast to an appropriate type */
        (void)_sd_calc_erase_sector(p_hndl);
    }
    /* if io or combo, set io part speed */
    if (p_hndl->media_type & SD_MEDIA_IO)
    {
        if (SD_MEDIA_COMBO != p_hndl->media_type)
        {
                if ((p_hndl->sup_speed & SD_MODE_HS) != 0)
                {
                    if (_sd_set_io_speed(p_hndl) != SD_OK)
                    {
                        return _sd_mount_error(p_hndl);
                    }
                }
        }
        /* Enable SDIO interrupt */
        SDMMC.SDIO_MODE.LONGLONG = (uint64_t)(SDMMC.SDIO_MODE.LONGLONG | SDIO_MODE_IOMOD);
    }

    /* ---- set mount flag ---- */
    p_hndl->mount = SD_MOUNT_UNLOCKED_CARD;

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
    return p_hndl->error;
}
/******************************************************************************
 End of function sd_mount
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_mount_error
 * Description  : mount SD card error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : p_hndl->error  : SD handle error value
 *              : SD_OK : end of succeed
 *              : other : end of error
 *****************************************************************************/
static int32_t _sd_mount_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_mount_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_card_init
 * Description  : initialize card.
 *              : initialize card from idle state to stand-by
 *              : distinguish card type (SD, MMC, IO or COMBO)
 *              : get CID, RCA, CSD from the card
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_card_init(st_sdhndl_t *p_hndl)
{
    int32_t  ret;
    int32_t  i;
    int32_t  just_sdio_flag;
    uint16_t if_cond_0;
    uint16_t if_cond_1;

    p_hndl->media_type = SD_MEDIA_UNKNOWN;
    if_cond_0 = p_hndl->if_cond[0];
    if_cond_1 = p_hndl->if_cond[1];

    if (p_hndl->sup_card & SD_MODE_IO)
    {
        just_sdio_flag = 0;                             /* basically treate as Combo */
        if (sddev_cmd0_sdio_mount(p_hndl->sd_port) == SD_OK)
        {
            ret = _sd_send_cmd(p_hndl, CMD0);
            if (SD_OK != ret)
            {
                p_hndl->error = SD_OK;
                just_sdio_flag = 1;                     /* treate as just I/O */
            }
        }
        else
        {
            just_sdio_flag = 1;                         /* treate as just I/O */
        }

        if (sddev_cmd8_sdio_mount(p_hndl->sd_port) == SD_OK )
        {
            if (SD_MODE_VER2X == p_hndl->sup_ver)
            {
                ret = _sd_card_send_cmd_arg(p_hndl, CMD8, SD_RSP_R7, if_cond_0, if_cond_1);
                if (SD_OK == ret)
                {
                    /* check R7 response */
                    if (p_hndl->if_cond[0] & 0xf000)
                    {
                        p_hndl->error = SD_ERR_IFCOND_VER;
                        return SD_ERR;
                    }
                    if ((p_hndl->if_cond[1] & 0x00ff) != 0x00aa)
                    {
                        p_hndl->error = SD_ERR_IFCOND_ECHO;
                        return SD_ERR;
                    }
                    p_hndl->sd_spec = SD_SPEC_20;         /* cmd8 have response.              */

                    /* because of (phys spec ver2.00)   */
                }
                else
                {
                    /* ==== clear illegal command error for CMD8 ==== */
                    if (sddev_cmd0_sdio_mount(p_hndl->sd_port) == SD_OK )
                    {
                        for (i = 0; i < 3; i++)
                        {
                            ret = _sd_send_cmd(p_hndl, CMD0);
                            if (SD_OK == ret)
                            {
                                break;
                            }
                        }
                    }
                    p_hndl->error = SD_OK;
                    p_hndl->sd_spec = SD_SPEC_10;         /* cmd8 have no response.                   */

                    /* because of (phys spec ver1.01 or 1.10)   */
                }
            }
            else
            {
                p_hndl->sd_spec = SD_SPEC_10;             /* cmd8 have response.                      */

                /* because of (phys spec ver1.01 or 1.10)   */
            }
        }
        else
        {
            just_sdio_flag = 1;                         /* treate as just I/O */
        }

        /* ==== distinguish card and read OCR (issue CMD5) ==== */
        ret = _sd_card_send_ocr(p_hndl, SD_MEDIA_UNKNOWN);
        if (SD_OK == ret)
        {
            /* set OCR (issue CMD5) */
            if (_sd_card_send_ocr(p_hndl, SD_MEDIA_IO) != SD_OK)
            {
                return SD_ERR;
            }

            p_hndl->io_flag |= SD_IO_FUNC_INIT;

            /* Cast to an appropriate type */
            p_hndl->io_info = (uint8_t)(p_hndl->io_ocr[0] >> 8);

            /* is memory present */
            p_hndl->media_type = SD_MEDIA_IO;

            if ( 0 == just_sdio_flag )
            {
                /* is not memory present */
                if ((p_hndl->io_info & 0x08) == 0)     /* just IO */
                {
                    return _sd_card_init_get_rca(p_hndl);
                }
            }
            else
            {
                return _sd_card_init_get_rca(p_hndl); /* just IO */
            }
        }
        else
        {
            /* clear error due to card distinction */
            p_hndl->error = SD_OK;
        }
    }

    /* ==== transfer idle state (issue CMD0) ==== */
    if (SD_MEDIA_UNKNOWN == p_hndl->media_type)
    {
        for (i = 0; i < 3; i++)
        {
            ret = _sd_send_cmd(p_hndl, CMD0);
            if (SD_OK == ret)
            {
                break;
            }
        }

        if (SD_OK != ret)
        {
            return SD_ERR;  /* error for CMD0 */
        }

        /* clear error by reissuing CMD0 */
        p_hndl->error = SD_OK;

        p_hndl->media_type |= SD_MEDIA_SD;



        if (SD_MODE_VER2X == p_hndl->sup_ver)
        {
            ret = _sd_card_send_cmd_arg(p_hndl, CMD8, SD_RSP_R7, if_cond_0, if_cond_1);
            if (SD_OK == ret)
            {
                /* check R7 response */
                if (p_hndl->if_cond[0] & 0xf000)
                {
                    p_hndl->error = SD_ERR_IFCOND_VER;
                    return SD_ERR;
                }
                if ((p_hndl->if_cond[1] & 0x00ff) != 0x00aa)
                {
                    p_hndl->error = SD_ERR_IFCOND_ECHO;
                    return SD_ERR;
                }
                p_hndl->sd_spec = SD_SPEC_20;         /* cmd8 have response.              */

                /* because of (phys spec ver2.00)   */
            }
            else
            {
                /* ==== clear illegal command error for CMD8 ==== */
                for (i = 0; i < 3; i++)
                {
                    ret = _sd_send_cmd(p_hndl, CMD0);
                    if (SD_OK == ret)
                    {
                        break;
                    }
                }
                p_hndl->error = SD_OK;
                p_hndl->sd_spec = SD_SPEC_10;         /* cmd8 have no response.                   */

                /* because of (phys spec ver1.01 or 1.10)   */
            }
        }
        else
        {
            p_hndl->sd_spec = SD_SPEC_10;             /* cmd8 have response.                      */

            /* because of (phys spec ver1.01 or 1.10)   */
        }
    }

    /* set OCR (issue ACMD41) */
    ret = _sd_card_send_ocr(p_hndl, (int32_t)p_hndl->media_type);

    /* clear error due to card distinction */
    p_hndl->error = SD_OK;

    if (SD_OK != ret)
    {
        /* softreset for error clear (issue CMD0) */
        for (i = 0; i < 3; i++)
        {
            ret = _sd_send_cmd(p_hndl, CMD0);
            if (SD_OK == ret)
            {
                break;
            }
        }
        if (SD_OK != ret)
        {
            return SD_ERR;  /* error for CMD0 */
        }

        /* clear error by reissuing CMD0 */
        p_hndl->error = SD_OK;

        /* ---- get OCR (issue CMD1) ---- */
        if (SD_OK == (ret = _sd_card_send_ocr(p_hndl, SD_MEDIA_MMC)))
        {
            /* MMC */
            p_hndl->media_type = SD_MEDIA_MMC;
            p_hndl->error = SD_OK;
        }
        else
        {
            /* unknown card */
            p_hndl->media_type = SD_MEDIA_UNKNOWN;
            _sd_set_err(p_hndl, SD_ERR_CARD_TYPE);
            return SD_ERR;
        }
    }

    /* ---- get CID (issue CMD2) ---- */
    if (_sd_card_send_cmd_arg(p_hndl, CMD2, SD_RSP_R2_CID, 0, 0) != SD_OK)
    {
        return SD_ERR;
    }
    return _sd_card_init_get_rca(p_hndl);
}
/******************************************************************************
 End of function _sd_card_init
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_card_init_get_rca
 * Description  : initialize card.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_card_init_get_rca(st_sdhndl_t *p_hndl)
{
    int32_t  i;

    /* ---- get RCA (issue CMD3) ---- */
    if (p_hndl->media_type & SD_MEDIA_COMBO)  /* IO or SD */
    {
        for (i = 0; i < 3; i++)
        {
            if (_sd_card_send_cmd_arg(p_hndl, CMD3, SD_RSP_R6, 0, 0) != SD_OK)
            {
                return SD_ERR;
            }
            if (0x00 != p_hndl->rca[0])
            {
                if (p_hndl->media_type & SD_MEDIA_IO)
                {
                    p_hndl->io_flag |= SD_IO_POWER_INIT;
                }
                break;
            }
        }

        /* illegal RCA */
        if (3 == i)
        {
            _sd_set_err(p_hndl, SD_ERR_CARD_CC);
            return SD_ERR;
        }
    }
    else
    {
        p_hndl->rca[0] = 1;   /* fixed 1 */
        if (_sd_card_send_cmd_arg(p_hndl, CMD3, SD_RSP_R1, p_hndl->rca[0], 0x0000)
                != SD_OK)
        {
            return SD_ERR;
        }
    }

    /* ==== stand-by state  ==== */

    if (SD_MEDIA_IO == p_hndl->media_type)
    {
        return SD_OK;
    }

    /* ---- get CSD (issue CMD9) ---- */
    if (_sd_card_send_cmd_arg(p_hndl, CMD9, SD_RSP_R2_CSD, p_hndl->rca[0], 0x0000)
            != SD_OK)
    {
        return SD_ERR;
    }

    p_hndl->dsr[0] = 0x0000;

    if (p_hndl->media_type & SD_MEDIA_MEM)
    {
        /* is DSR implimented? */
        if (p_hndl->csd[3] & 0x0010u) /* implimented */
        {
            /* set DSR (issue CMD4) */
            p_hndl->dsr[0] = 0x0404;
            if (_sd_card_send_cmd_arg(p_hndl, CMD4, SD_RSP_NON, p_hndl->dsr[0], 0x0000)
                    != SD_OK)
            {
                return SD_ERR;
            }
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_card_init_get_rca
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_io_mount
 * Description  : mount io part from stand-by to command or transfer state
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_io_mount(st_sdhndl_t *p_hndl)
{
    int32_t  i;
    uint8_t  io_buff;
    uint16_t len;

    /* ==== data-transfer mode ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD7, SD_RSP_R1B, p_hndl->rca[0], 0x0000)
            != SD_OK)
    {
        return _sd_io_mount_error(p_hndl);
    }

    /* ---- get card capability (include LSC and 4BLS) ---- */
    /* func:0 address:08'h read */
    if (_sdio_direct(p_hndl, &io_buff, 0, 0x08, 0, 0) != SD_OK)
    {
        return _sd_io_mount_error(p_hndl);
    }

    if (io_buff & 0x40u)     /* low speed card */
    {
        p_hndl->csd_tran_speed = SD_CLK_400KHZ;
    }
    else    /* high speed card */
    {
        p_hndl->csd_tran_speed = SD_CLK_25MHZ;
    }

    /* ---- supply clock (data-transfer ratio) ---- */
    _sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE);

    /* set bus width and clear pull-up DAT3 */
    if ((io_buff & 0x40u) && ((io_buff & 0x80u) == 0))   /* not support 4bits */
    {
        _sd_set_port(p_hndl, SD_PORT_SERIAL);
    }
    else
    {
        _sd_set_port(p_hndl, p_hndl->sup_if_mode);
    }

    /* ---- get CCCR value ---- */
    if (_sdio_read_byte(p_hndl, p_hndl->io_reg[0], 0, 0, SDIO_INTERNAL_REG_SIZE,
                        SD_IO_INCREMENT_ADDR) != SD_OK)
    {
        return _sd_io_mount_error(p_hndl);
    }

    /* save io function 0 block length */
    if ((p_hndl->io_reg[0][0x08] & 0x02) != 0)
    {
        len  = p_hndl->io_reg[0][0x11];
        len <<= 8;
        len |= (p_hndl->io_reg[0][0x10] & 0x00ff);

        switch (len)
        {
            case 32:
            case 64:
            case 128:
            case 256:
            case 512:

                /* len is OK */
                p_hndl->io_len[0] = len;              /* already and supported */
                break;
            default:
                p_hndl->io_len[0] = 0xffff;           /* already but not supported */
                break;
        }

        for ( i = 1; i < 8; i++ )
        {
            p_hndl->io_len[i] = 0;                /* not yet */
        }
    }
    else
    {
        for ( i = 0; i < 8; i++ )
        {
            p_hndl->io_len[i] = 0xffff;           /* already but not supported */
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_io_mount
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_io_mount_error
 * Description  : mount io card error
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_io_mount_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_io_mount_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_mem_mount
 * Description  : mount memory card.
 *              : mount memory part from stand-by to transfer state
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_mem_mount(st_sdhndl_t *p_hndl)
{
    /* case of combo, already supplied data transfer clock */
    if ((p_hndl->media_type & SD_MEDIA_IO) == 0)
    {
        /* ---- supply clock (data-transfer ratio) ---- */
        if ( p_hndl->csd_tran_speed > SD_CLK_25MHZ )
        {
            p_hndl->csd_tran_speed = SD_CLK_25MHZ;

            /* Herein after, if switch-function(cmd6) is pass,      */
            /* p_hndl->csd_tran_speed is set to SD_CLK_50MHz          */
        }

        /* Cast to an appropriate type */
        if (_sd_set_clock(p_hndl, (int32_t)p_hndl->csd_tran_speed, SD_CLOCK_ENABLE) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }
    }

    /* ==== data-transfer mode(Transfer State) ==== */
    if (_sd_card_send_cmd_arg(p_hndl, CMD7, SD_RSP_R1B, p_hndl->rca[0], 0x0000)
            != SD_OK)
    {
        return _sd_mem_mount_error(p_hndl);
    }

    if ((p_hndl->resp_status & 0x02000000))
    {
        _sd_set_err(p_hndl, SD_ERR_CARD_LOCK);
        return SD_OK;
    }

    /* ---- set block length (issue CMD16) ---- */
    if (_sd_card_send_cmd_arg(p_hndl, CMD16, SD_RSP_R1, 0x0000, 0x0200) != SD_OK)
    {
        return _sd_mem_mount_error(p_hndl);
    }

    /* if 4bits transfer supported (SD memory card mandatory), change bus width 4bits */
    if (p_hndl->media_type & SD_MEDIA_SD)
    {
        _sd_set_port(p_hndl, p_hndl->sup_if_mode);
    }

    /* clear pull-up DAT3 */
    if (p_hndl->media_type & SD_MEDIA_SD)
    {
        if (_sd_send_acmd(p_hndl, ACMD42, 0, 0) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }

        /* check R1 resp */
        if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }
    }

    /* if SD memory card, get SD Status */
    if (p_hndl->media_type & SD_MEDIA_SD)
    {
        if (_sd_card_get_status(p_hndl) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }

        /* get protect area size */
        if (_sd_get_size(p_hndl, SD_PROT_AREA) != SD_OK)
        {
            return _sd_mem_mount_error(p_hndl);
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_mem_mount
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_mem_mount_error
 * Description  : mount memory card error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_mem_mount_error(st_sdhndl_t *p_hndl)
{
    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);
    return p_hndl->error;
}
/******************************************************************************
 End of function _sd_mem_mount_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_set_io_speed
 * Description  : query high speed supported
 *                transfer card high speed mode
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *                SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_set_io_speed(st_sdhndl_t *p_hndl)
{
    uint8_t io_buff;

    /* is CCCR/FBR version 2.00? */
    if ((p_hndl->io_reg[0][0] & 0x0F) >= 0x02)
    {
        /* is high speed supported? */
        if (p_hndl->io_reg[0][0x13] & 0x01)
        {
            p_hndl->speed_mode |= SD_SUP_HIGH_SPEED;

            io_buff = 0x02;
            if (_sdio_direct(p_hndl, &io_buff, 0, 0x13, 1, SD_IO_VERIFY_WRITE)
                    != SD_OK)
            {
                return SD_ERR;
            }
            p_hndl->io_reg[0][0x13] = io_buff;
            if (io_buff & 0x02)  /* high speed mode */
            {
                /* force set high-speed mode */
                p_hndl->csd_tran_speed = SD_CLK_50MHZ;
                p_hndl->speed_mode |= SD_CUR_HIGH_SPEED;
            }
        }
        else
        {
            /* Cast to an appropriate type */
            p_hndl->speed_mode &= (uint8_t)~SD_SUP_HIGH_SPEED;
        }
    }

    return SD_OK;
}
/******************************************************************************
 End of function _sd_set_io_speed
 *****************************************************************************/
/******************************************************************************
 * Function Name: sd_unmount
 * Description  : unmount card.
 *              : turn off power
 * Arguments    : int32_t sd_port : channel no (0 or 1)
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t sd_unmount(int32_t sd_port)
{
    st_sdhndl_t  *p_hndl;
    uint8_t      io_buff;

    if ( (0 != sd_port) && (1 != sd_port) )
    {
        return SD_ERR;
    }

    p_hndl = SD_GET_HNDLS(sd_port);
    if (0 == p_hndl)
    {
        return SD_ERR;  /* not initialized */
    }

    if ( (SD_MEDIA_IO == p_hndl->media_type) || (SD_MEDIA_COMBO == p_hndl->media_type) )
    {
        /* media has SDIO */
        if (p_hndl->io_flag & SD_IO_POWER_INIT)   /* already supplied power */
        {
            /* ==== transfer idle state (issue CMD52) ==== */
            /* data:08'h func:0 address:06'h verify write */
            io_buff = 0x08;
            sdio_write_direct(sd_port, &io_buff, 0, 0x06, SD_IO_VERIFY_WRITE);

            /* dont care error */
        }
    }

    /* ---- clear mount flag ---- */
    p_hndl->mount = SD_UNMOUNT_CARD;

    /* ---- halt clock ---- */
    _sd_set_clock(p_hndl, 0, SD_CLOCK_DISABLE);

    /* ---- set single port ---- */
    sddev_set_port(sd_port, SD_PORT_SERIAL);

    /* ---- turn off power ---- */
    if (sddev_power_off(sd_port) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return p_hndl->error;
    }

    /* ---- initilaize SD handle ---- */
    _sd_init_hndl(p_hndl, 0, p_hndl->voltage);

    return SD_OK;
}
/******************************************************************************
 End of function sd_unmount
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_card_get_status
 * Description  : get SD Status (issue ACMD13)
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 *****************************************************************************/
int32_t _sd_card_get_status(st_sdhndl_t *p_hndl)
{
    int32_t  ret;
    int32_t  i;
    uint8_t  *p_rw_buff;

    /* Cast to an appropriate type */
    p_rw_buff = (uint8_t *)&s_stat_buff[p_hndl->sd_port][0];

    /* ---- get SD Status (issue ACMD13) ---- */
    if (_sd_read_byte(p_hndl, ACMD13, 0, 0, p_rw_buff, SD_STATUS_BYTE) != SD_OK)
    {
        return SD_ERR;
    }

    /* ---- distinguish SD ROM card ---- */
    if ((p_rw_buff[2] & 0xffu) == 0x00) /* [495:488] = 0x00 */
    {
        ret = SD_OK;
        if ((p_rw_buff[3] & 0xffu) == 0x01)
        {
            p_hndl->write_protect |= SD_WP_ROM;
        }
    }
    else
    {
        ret = SD_ERR;
        _sd_set_err(p_hndl, SD_ERR_CARD_ERROR);
    }

    p_hndl->speed_class = p_rw_buff[8];
    p_hndl->perform_move = p_rw_buff[9];

    /* ---- save SD STATUS ---- */
    for (i = 0; i < (16 / sizeof(uint16_t)); i++)
    {
        p_hndl->sdstatus[i] = (s_stat_buff[p_hndl->sd_port][i] << 8) | (s_stat_buff[p_hndl->sd_port][i] >> 8);
    }

    return ret;
}
/******************************************************************************
 End of function _sd_card_get_status
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_card_get_scr
 * Description  : get SCR register (issue ACMD51).
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *****************************************************************************/
int32_t _sd_card_get_scr(st_sdhndl_t *p_hndl)
{
    uint8_t  *p_rw_buff;

    /* Cast to an appropriate type */
    p_rw_buff = (uint8_t *)&s_stat_buff[p_hndl->sd_port][0];

    /* ---- get SCR register (issue ACMD51) ---- */
    if (_sd_read_byte(p_hndl, ACMD51, 0, 0, p_rw_buff, SD_SCR_REGISTER_BYTE) != SD_OK)
    {
        return SD_ERR;
    }

    /* ---- save SCR register ---- */
    p_hndl->scr[0] = (s_stat_buff[p_hndl->sd_port][0] << 8) | (s_stat_buff[p_hndl->sd_port][0] >> 8);
    p_hndl->scr[1] = (s_stat_buff[p_hndl->sd_port][1] << 8) | (s_stat_buff[p_hndl->sd_port][1] >> 8);
    p_hndl->scr[2] = (s_stat_buff[p_hndl->sd_port][2] << 8) | (s_stat_buff[p_hndl->sd_port][2] >> 8);
    p_hndl->scr[3] = (s_stat_buff[p_hndl->sd_port][3] << 8) | (s_stat_buff[p_hndl->sd_port][3] >> 8);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_card_get_scr
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_read_byte
 * Description  : read byte data from card
 *              : issue byte data read command and read data from SD_BUF
 *              : using following commands
 *              : SD STATUS(ACMD13),SCR(ACMD51),NUM_WRITE_BLOCK(ACMD22),
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint16_t cmd        : command code
 *              : uint16_t h_arg      : command argument high [31:16]
 *              : uint16_t l_arg      : command argument low [15:0]
 *              : uint8_t *readbuff   : read data buffer
 *              : uint16_t byte       : the number of read bytes
 * Return Value : SD_OK : end of succeed
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_read_byte(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg,
                        uint16_t l_arg, uint8_t *readbuff, uint16_t byte)
{
    /* ---- disable SD_SECCNT ---- */
    SDMMC.SD_STOP.LONGLONG = 0x0000;
    
    /* ---- set transfer bytes ---- */
    SDMMC.SD_SIZE.LONGLONG = (uint64_t)byte;
    
    /* ---- issue command ---- */
    if (cmd & 0x0040u)  /* ACMD13, ACMD22 and ACMD51 */
    {
        if (_sd_send_acmd(p_hndl, cmd, h_arg, l_arg) != SD_OK)
        {
            if ((SD_ERR_END_BIT == p_hndl->error) ||
                    (SD_ERR_CRC == p_hndl->error))
            {
                /* continue */
                ;
            }
            else
            {
                return _sd_read_byte_error(p_hndl);
            }
        }
    }
    else
    {
        _sd_set_arg(p_hndl, h_arg, l_arg);
        if (_sd_send_cmd(p_hndl, cmd) != SD_OK)
        {
            return SD_ERR;
        }
    }

    /* ---- check R1 response ---- */
    if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
    {
        return _sd_read_byte_error(p_hndl);
    }

    /* enable All end, BRE and errors */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

    /* ---- wait BRE interrupt ---- */
    if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_read_byte_error(p_hndl);
    }

    /* ---- check errors ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return _sd_read_byte_error(p_hndl);
    }

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, 0x0000, SD_INFO2_MASK_RE); /* clear BRE bit */

    /* transfer data */

    if (sddev_read_data(p_hndl->sd_port, readbuff, (uint32_t)(&SDMMC.SD_BUF0.LONGLONG), (int32_t)byte) != SD_OK)

    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return _sd_read_byte_error(p_hndl);
    }

    /* wait All end interrupt */
    if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_RESP) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_read_byte_error(p_hndl);
    }

    /* ---- check errors ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return _sd_read_byte_error(p_hndl);
    }

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_ERR); /* clear All end bit */

    /* disable all interrupts */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_read_byte
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_read_byte_error
 * Description  : read byte data error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle.
 * Return Value : SD_ERR: end of error.
 *****************************************************************************/
static int32_t _sd_read_byte_error(st_sdhndl_t *p_hndl)
{
    /* Cast to an appropriate type */
    SDMMC.SD_STOP.LONGLONG = (uint64_t)0x0001;                       /* stop data transfer   */
    
    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_ERR); /* clear All end bit    */

    /* disable all interrupts */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BRE);

    return SD_ERR;
}
/******************************************************************************
 End of function _sd_read_byte_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_write_byte
 * Description  : write byte data to card
 *              : issue byte data write command and write data to SD_BUF
 *              : using following commands
 *              : (CMD27 and CMD42)
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 *              : uint16_t cmd        : command code
 *              : uint16_t h_arg      : command argument high [31:16]
 *              : uint16_t l_arg      : command argument low [15:0]
 *              : uint8_t *writebuff  : write data buffer
 *              : uint16_t byte       : the number of write bytes
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_write_byte(st_sdhndl_t *p_hndl, uint16_t cmd, uint16_t h_arg,
                        uint16_t l_arg, uint8_t *writebuff, uint16_t byte)
{
    int32_t time_out;

    /* ---- disable SD_SECCNT ---- */
    SDMMC.SD_STOP.LONGLONG = 0x0000;
    
    /* ---- set transfer bytes ---- */
    SDMMC.SD_SIZE.LONGLONG = (uint64_t)byte;
    
    /* ---- issue command ---- */
    _sd_set_arg(p_hndl, h_arg, l_arg);
    if (_sd_send_cmd(p_hndl, cmd) != SD_OK)
    {
        return SD_ERR;
    }

    /* ---- check R1 response ---- */
    if (_sd_get_resp(p_hndl, SD_RSP_R1) != SD_OK)
    {
        if (SD_ERR_CARD_LOCK == p_hndl->error)
        {
            p_hndl->error = SD_OK;
        }
        else
        {
            return _sd_write_byte_error(p_hndl);
        }
    }

    /* enable All end, BWE and errors */
    _sd_set_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

    /* ---- wait BWE interrupt ---- */
    if (sddev_int_wait(p_hndl->sd_port, SD_TIMEOUT_MULTIPLE) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_write_byte_error(p_hndl);
    }

    /* ---- check errors ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        return _sd_write_byte_error(p_hndl);
    }

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, 0x0000, SD_INFO2_MASK_WE); /* clear BWE bit */

    /* transfer data */

    if (sddev_write_data(p_hndl->sd_port, writebuff, (uint32_t)(&SDMMC.SD_BUF0.LONGLONG), (int32_t)byte) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_CPU_IF);
        return _sd_write_byte_error(p_hndl);
    }

    /* wait All end interrupt */
    if ( (CMD42 == cmd) && (1 == byte) )
    {
        /* force erase timeout  */
        time_out = SD_TIMEOUT_ERASE_CMD;
    }
    else
    {
        time_out = SD_TIMEOUT_RESP;
    }

    if (sddev_int_wait(p_hndl->sd_port, time_out) != SD_OK)
    {
        _sd_set_err(p_hndl, SD_ERR_HOST_TOE);
        return _sd_write_byte_error(p_hndl);
    }

    /* ---- check errors but for timeout ---- */
    if (p_hndl->int_info2 & SD_INFO2_MASK_ERR)
    {
        _sd_check_info2_err(p_hndl);
        if ( SD_TIMEOUT_ERASE_CMD == time_out )
        {
            /* force erase  */
            if (SD_ERR_CARD_TOE == p_hndl->error)
            {
                /* force erase timeout  */
                _sd_clear_info(p_hndl, SD_INFO1_MASK_TRNS_RESP, SD_INFO2_MASK_ERR);
                if (_sd_wait_rbusy(p_hndl, 10000000) != SD_OK)
                {
                    return _sd_write_byte_error(p_hndl);
                }
            }
            else
            {
                return _sd_write_byte_error(p_hndl);
            }
        }
        else
        {
            return _sd_write_byte_error(p_hndl);
        }
    }

    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);  /* clear All end bit */

    /* disable all interrupts */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

    return SD_OK;
}
/******************************************************************************
 End of function _sd_write_byte
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_write_byte_error
 * Description  : write byte data error.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_ERR: end of error
 *****************************************************************************/
static int32_t _sd_write_byte_error(st_sdhndl_t *p_hndl)
{
    /* Cast to an appropriate type */
    SDMMC.SD_STOP.LONGLONG = (uint64_t)0x0001;               /* stop data transfer   */
    
    /* Cast to an appropriate type */
    _sd_clear_info(p_hndl, SD_INFO1_MASK_DATA_TRNS, 0x0000);  /* clear All end bit    */

    /* disable all interrupts */
    _sd_clear_int_mask(p_hndl, SD_INFO1_MASK_DATA_TRNS, SD_INFO2_MASK_BWE);

    return SD_ERR;
}
/******************************************************************************
 End of function _sd_write_byte_error
 *****************************************************************************/

/******************************************************************************
 * Function Name: _sd_calc_erase_sector
 * Description  : calculate erase sector.
 *              : This function calculate erase sector for SD Phy Ver2.0.
 * Arguments    : st_sdhndl_t *p_hndl : SD handle
 * Return Value : SD_OK : end of succeed
 *              : SD_ERR: end of error
 * Remark       : transfer type is PIO
 *****************************************************************************/
int32_t _sd_calc_erase_sector(st_sdhndl_t *p_hndl)
{
    uint16_t au;
    uint16_t erase_size;

    if ((p_hndl->scr[0] & 0x0f00) == 0x0200)
    {
        /* AU is not defined,set to fixed value */
        p_hndl->erase_sect = SD_ERASE_SECTOR;

        /* get AU size */
        au = p_hndl->sdstatus[5] >> 12;

        if ( (au > 0) && (au < 0x0a) )
        {
            /* get AU_SIZE(sectors) */
            p_hndl->erase_sect = ((8 * 1024) / 512) << au;

            /* get ERASE_SIZE */
            erase_size = (p_hndl->sdstatus[5] << 8) | (p_hndl->sdstatus[6] >> 8);
            if (0 != erase_size)
            {
                p_hndl->erase_sect *= erase_size;
            }
        }

    }
    else
    {
        /* If card is not Ver2.0,it use ERASE_BLK_LEN in CSD *//* DO NOTHING */
        ;
    }
    return SD_OK;
}
/******************************************************************************
 End of function _sd_calc_erase_sector
 *****************************************************************************/
/* End of File */
