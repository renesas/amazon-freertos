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
* File Name    : r_sdif.h
* Version      : 1.30
* Device(s)    : RZ/A2M
* Tool-Chain   : e2 studio (GCC ARM Embedded)
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : SD Memory card driver I/F
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
******************************************************************************/
#ifndef R_SDIF_H
#define R_SDIF_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#ifdef    __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/******************************************************************************
Macro definitions
******************************************************************************/
/* ==== define  ==== */
#define SD_SCC_IP0_BASE_ADDR      (0xE8227000uL)        /* Set the base address of SCC ch0. */
#define SD_SCC_IP1_BASE_ADDR      (0xE8229000uL)        /* Set the base address of SCC ch1. */

/* ---- SD Driver work buffer ---- */
#define SD_SIZE_OF_INIT           (856)                                                          /* SDIO */

/* ---- error code ---- */
#define SD_OK_LOCKED_CARD         (1)                   /* OK but card is locked status */
#define SD_OK                     (0)                   /* OK */
#define SD_ERR                    (-1)                  /* general error */
#define SD_ERR_WP                 (-2)                  /* write protect error */
/* 3 */
#define SD_ERR_RES_TOE            (-4)                  /* response time out error */
#define SD_ERR_CARD_TOE           (-5)                  /* card time out error */
#define SD_ERR_END_BIT            (-6)                  /* end bit error */
#define SD_ERR_CRC                (-7)                  /* CRC error */
#define SD_ERR_ILL_ACCESS         (-8)                  /* illegal access error */
#define SD_ERR_HOST_TOE           (-9)                  /* host time out error */
#define SD_ERR_CARD_ERASE         (-10)                 /* card erase error */
#define SD_ERR_CARD_LOCK          (-11)                 /* card lock error */
#define SD_ERR_CARD_UNLOCK        (-12)                 /* card unlock error */
#define SD_ERR_HOST_CRC           (-13)                 /* host CRC error */
#define SD_ERR_CARD_ECC           (-14)                 /* card internal ECC error */
#define SD_ERR_CARD_CC            (-15)                 /* card internal error */
#define SD_ERR_CARD_ERROR         (-16)                 /* unknown card error */
#define SD_ERR_CARD_TYPE          (-17)                 /* non support card type */
#define SD_ERR_NO_CARD            (-18)                 /* no card */
#define SD_ERR_ILL_READ           (-19)                 /* illegal buffer read */
#define SD_ERR_ILL_WRITE          (-20)                 /* illegal buffer write */
#define SD_ERR_AKE_SEQ            (-21)                 /* the sequence of authentication process */
#define SD_ERR_OVERWRITE          (-22)                 /* CID/CSD overwrite error */
/* 23-29 */
#define SD_ERR_CPU_IF             (-30)                 /* target CPU interface function error  */
#define SD_ERR_STOP               (-31)                 /* user stop */
/* 32-49 */
#define SD_ERR_CSD_VER            (-50)                 /* CSD register version error */
#define SD_ERR_SCR_VER            (-51)                 /* SCR register version error */
#define SD_ERR_FILE_FORMAT        (-52)                 /* CSD register file format error  */
#define SD_ERR_NOTSUP_CMD         (-53)                 /* not supported command  */
/* 54-59 */
#define SD_ERR_ILL_FUNC           (-60)                 /* invalid function request error */
#define SD_ERR_IO_VERIFY          (-61)                 /* direct write verify error */
#define SD_ERR_IO_CAPAB           (-62)                 /* IO capability error */
/* 63-69 */
#define SD_ERR_IFCOND_VER         (-70)                 /* Interface condition version error */
#define SD_ERR_IFCOND_VOLT        (-71)                 /* Interface condition voltage error */
#define SD_ERR_IFCOND_ECHO        (-72)                 /* Interface condition echo back pattern error */
/* 73-79 */
#define SD_ERR_OUT_OF_RANGE       (-80)                 /* the argument was out of range */
#define SD_ERR_ADDRESS_ERROR      (-81)                 /* misassigned address */
#define SD_ERR_BLOCK_LEN_ERROR    (-82)                 /* transfered block length is not allowed */
#define SD_ERR_ILLEGAL_COMMAND    (-83)                 /* Command not legal  */
#define SD_ERR_RESERVED_ERROR18   (-84)                 /* Reserved bit 18 Error */
#define SD_ERR_RESERVED_ERROR17   (-85)                 /* Reserved bit 17 Error */
#define SD_ERR_CMD_ERROR          (-86)                 /* SD_INFO2 bit  0 CMD error */
#define SD_ERR_CBSY_ERROR         (-87)                 /* SD_INFO2 bit 14 CMD Type Reg Busy error */
#define SD_ERR_NO_RESP_ERROR      (-88)                 /* SD_INFO1 bit  0 No Response error */
/* 89-95 */
#define SD_ERR_ERROR              (-96)                 /* SDIO ERROR */
#define SD_ERR_FUNCTION_NUMBER    (-97)                 /* SDIO FUNCTION NUMBER ERROR */
#define SD_ERR_COM_CRC_ERROR      (-98)                 /* SDIO CRC ERROR */
#define SD_ERR_INTERNAL           (-99)                 /* driver software internal error */

/* ---- driver mode ---- */
#define SD_MODE_POLL              (0x0000ul)            /* status check mode is software polling */
#define SD_MODE_HWINT             (0x0001ul)            /* status check mode is hardware interrupt */
#define SD_MODE_SW                (0x0000ul)            /* data transfer mode is software */
#define SD_MODE_DMA               (0x0002ul)            /* data transfer mode is DMA */

/* ---- support mode ---- */
#define SD_MODE_MEM               (0x0000ul)            /* memory cards only are supported */
#define SD_MODE_IO                (0x0010ul)            /* memory and io cards are supported */
#define SD_MODE_COMBO             (0x0030ul)            /* memory ,io and combo cards are supported */
#define SD_MODE_DS                (0x0000ul)            /* only default speed mode is supported */
#define SD_MODE_HS                (0x0040ul)            /* high speed mode is also supported */
#define SD_MODE_SDR12             (0x1000ul)            /* SDR12 mode is also supported */
#define SD_MODE_SDR25             (0x2000ul)            /* SDR25 mode is also supported */
#define SD_MODE_SDR50             (0x4000ul)            /* SDR50 mode is also supported */
#define SD_MODE_SDR104            (0x8000ul)            /* SDR104 mode is also supported */
#define SD_MODE_VER1X             (0x0000ul)            /* ver1.1 host */
#define SD_MODE_VER2X             (0x0080ul)            /* ver2.x host (high capacity and dual voltage) */
#define SD_MODE_1BIT              (0x0100ul)            /* SD Mode 1bit only is supported */
#define SD_MODE_4BIT              (0x0000ul)            /* SD Mode 1bit and 4bit is supported */

/* ---- media voltage ---- */
#define SD_VOLT_1_7               (0x00000010ul)        /* low voltage card minimum */
#define SD_VOLT_1_8               (0x00000020ul)
#define SD_VOLT_1_9               (0x00000040ul)
#define SD_VOLT_2_0               (0x00000080ul)
#define SD_VOLT_2_1               (0x00000100ul)        /* basic communication minimum */
#define SD_VOLT_2_2               (0x00000200ul)
#define SD_VOLT_2_3               (0x00000400ul)
#define SD_VOLT_2_4               (0x00000800ul)
#define SD_VOLT_2_5               (0x00001000ul)
#define SD_VOLT_2_6               (0x00002000ul)
#define SD_VOLT_2_7               (0x00004000ul)
#define SD_VOLT_2_8               (0x00008000ul)        /* memory access minimum */
#define SD_VOLT_2_9               (0x00010000ul)
#define SD_VOLT_3_0               (0x00020000ul)
#define SD_VOLT_3_1               (0x00040000ul)
#define SD_VOLT_3_2               (0x00080000ul)
#define SD_VOLT_3_3               (0x00100000ul)
#define SD_VOLT_3_4               (0x00200000ul)
#define SD_VOLT_3_5               (0x00400000ul)
#define SD_VOLT_3_6               (0x00800000ul)

/* ---- memory card write mode ---- */
#define SD_WRITE_WITH_PREERASE    (0x0000u)             /* pre-erease write */
#define SD_WRITE_OVERWRITE        (0x0001u)             /* overwrite  */

/* ---- io register write mode ---- */
#define SD_IO_SIMPLE_WRITE        (0x0000u)             /* just write */
#define SD_IO_VERIFY_WRITE        (0x0001u)             /* read after write */

/* ---- io operation code ---- */
#define SD_IO_FIXED_ADDR          (0x0000u)             /* R/W fixed address */
#define SD_IO_INCREMENT_ADDR      (0x0001u)             /* R/W increment address */
#define SD_IO_FORCE_BYTE          (0x0010u)             /* byte access only  */

/* ---- media type ---- */
#define SD_MEDIA_UNKNOWN          (0x0000u)             /* unknown media */
#define SD_MEDIA_MMC              (0x0010u)             /* MMC card */
#define SD_MEDIA_SD               (0x0020u)             /* SD Memory card */
#define SD_MEDIA_IO               (0x0001u)             /* SD IO card */
#define SD_MEDIA_MEM              (0x0030u)             /* Memory card */
#define SD_MEDIA_COMBO            (0x0021u)             /* SD COMBO card */

/* ---- write protect info --- */
#define SD_WP_OFF                 (0x0000u)             /* card is not write protect */
#define SD_WP_HW                  (0x0001u)             /* card is H/W write protect */
#define SD_WP_TEMP                (0x0002u)             /* card is TEMP_WRITE_PROTECT */
#define SD_WP_PERM                (0x0004u)             /* card is PERM_WRITE_PROTECT */
#define SD_WP_ROM                 (0x0010u)             /* card is SD-ROM */

/* ---- SD clock div ---- */    /* IMCLK is host controller clock */
#define SD_DIV_512                (0x0080u)             /* SDCLOCK = IMCLK/512 */
#define SD_DIV_256                (0x0040u)             /* SDCLOCK = IMCLK/256 */
#define SD_DIV_128                (0x0020u)             /* SDCLOCK = IMCLK/128 */
#define SD_DIV_64                 (0x0010u)             /* SDCLOCK = IMCLK/64 */
#define SD_DIV_32                 (0x0008u)             /* SDCLOCK = IMCLK/32 */
#define SD_DIV_16                 (0x0004u)             /* SDCLOCK = IMCLK/16 */
#define SD_DIV_8                  (0x0002u)             /* SDCLOCK = IMCLK/8 */
#define SD_DIV_4                  (0x0001u)             /* SDCLOCK = IMCLK/4 */
#define SD_DIV_2                  (0x0000u)             /* SDCLOCK = IMCLK/2 */
#define SD_DIV_1                  (0x00FFu)             /* SDCLOCK = IMCLK (option) */

/* ---- SD clock define ---- */    /* max frequency */
#define SD_CLK_400KHZ             (0x0000u)             /* 400kHz */
#define SD_CLK_1MHZ               (0x0001u)             /* 1MHz */
#define SD_CLK_5MHZ               (0x0002u)             /* 5MHz */
#define SD_CLK_10MHZ              (0x0003u)             /* 10MHz */
#define SD_CLK_20MHZ              (0x0004u)             /* 20MHz */
#define SD_CLK_25MHZ              (0x0005u)             /* 25MHz */
#define SD_CLK_50MHZ              (0x0006u)             /* 50MHz (phys spec ver1.10) */

/* ---- speed class ---- */
#define SD_SPEED_CLASS_0          (0x00u)               /* not defined, or less than ver2.0 */
#define SD_SPEED_CLASS_2          (0x01u)               /* 2MB/sec */
#define SD_SPEED_CLASS_4          (0x02u)               /* 4MB/sec */
#define SD_SPEED_CLASS_6          (0x03u)               /* 6MB/sec */
/* ---- IO initialize flags define ---- */    /* add for IO */
#define SD_IO_INT_ENAB            (0x10u)               /* interrupt enable */
#define SD_IO_POWER_INIT          (0x04u)               /* power on initialized */
#define SD_IO_MEM_INIT            (0x02u)               /* memory initialized */
#define SD_IO_FUNC_INIT           (0x01u)               /* io func initialized */

/* ---- IO function's information ---- */    /* add for IO */
#define SD_IO_FUNC_READY          (0x80u)               /* io redy */
#define SD_IO_FUNC_NUM            (0x70u)               /* number of io func */
#define SD_IO_FUNC_EXISTS         (0x04u)               /* memory present */

/* ---- SD port mode ---- */
#define SD_PORT_SERIAL            (0x0000u)             /* 1bit mode */
#define SD_PORT_PARALLEL          (0x0001u)             /* 4bits mode */

/* ---- SD Card detect port ---- */
#define SD_CD_SOCKET              (0x0000u)             /* CD pin */
#define SD_CD_DAT3                (0x0001u)             /* DAT3 pin */

/* ---- SD Card detect interrupt ---- */
#define SD_CD_INT_DISABLE         (0x0000u)             /* card detect interrupt disable */
#define SD_CD_INT_ENABLE          (0x0001u)             /* card detect interrupt enable */

/* ---- lock/unlock mode ---- */
#define SD_FORCE_ERASE            (0x08)
#define SD_LOCK_CARD              (0x04)
#define SD_UNLOCK_CARD            (0x00)
#define SD_CLR_PWD                (0x02)
#define SD_SET_PWD                (0x01)

/******************************************************************************
Typedef definitions
******************************************************************************/
/* ---- User Configuration ---- */
/* SD card detection option */
typedef enum
{
    SD_CD_ENABLED,      /* SD card detection is enabled. */
    SD_CD_DISABLED      /* When SD card detection is disabled,
                           the status is always loading. */
} e_sd_cd_layout_t;

/* SD write protection signal detection option */
typedef enum
{
    SD_WP_ENABLED,      /* Write protection signal detection is enabled. */
    SD_WP_DISABLED      /* When write protection signal detection is disabled,
                           the status is always write protection signal off. */
} e_sd_wp_layout_t;

typedef int32_t (*p_intCallbackFunc)(int32_t sd_port, int32_t cd);
typedef int32_t (*p_fmtCallbackFunc)(uint32_t secno, uint32_t size);
typedef int32_t (*p_intIoCallbackFunc)(int32_t sd_port);

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
/* ==== API prototype ===== */
/* ---- access library I/F ---- */
/* Function Name: sd_init */
/**************************************************************************//**
 * @fn            int32_t sd_init(int32_t sd_port, uint32_t base, void *workarea, int32_t cd_port)
 * @brief         initialize SD Driver (more than 2ports). <br>
 *                initialize SD Driver work memory started from SDHI register
 *                base <br>
 *                address specified by argument (base) <br>
 *                initialize port specified by argument (cd_port) <br>
 *                work memory is allocated octlet boundary
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    uint32_t base   : SDHI register base address
 * @param [in]    void *workarea  : SD Driver work memory
 * @param [in]    int32_t cd_port : card detect port <br>
 *                  SD_CD_SOCKET  : card detect by CD pin <br>
 *                  SD_CD_DAT3    : card detect by DAT3 pin
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 * @retval        SD_ERR_CPU_IF : CPU-IF function error
 *****************************************************************************/
int32_t sd_init(int32_t sd_port, uint32_t base, void *workarea, int32_t cd_port);

/* Function Name: sd_cd_int */
/**************************************************************************//**
 * @fn            int32_t sd_cd_int(int32_t sd_port, int32_t enable, int32_t (*callback)(int32_t, int32_t))
 * @brief         set card detect interrupt <br>
 *                if select SD_CD_INT_ENABLE, detect interrupt is enbled and
 *                it is possible register callback function <br>
 *                if select SD_CD_INT_DISABLE, detect interrupt is disabled
 * @warning       .
 * @param [in]    int32_t sd_port               : channel no (0 or 1)
 * @param [in]    int32_t enable                : is enable or disable card detect interrupt?
 * @param [in]    int32_t (*callback)(int32_t, int32_t)
 *                                              : interrupt callback function
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_cd_int(int32_t sd_port, int32_t enable, int32_t (*callback)(int32_t, int32_t));

/* Function Name: sd_check_media */
/**************************************************************************//**
 * @fn            int32_t sd_check_media(int32_t sd_port)
 * @brief         check card insertion <br>
 *                if card is inserted, return SD_OK <br>
 *                if card is not inserted, return SD_ERR <br>
 *                if SD handle is not initialized, return SD_ERR
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : card is inserted
 * @retval        SD_ERR : card is not inserted
 *****************************************************************************/
int32_t sd_check_media(int32_t sd_port);

/* Function Name: sd_mount */
/**************************************************************************//**
 * @fn            int32_t sd_mount(int32_t sd_port, uint32_t mode, uint32_t voltage)
 * @brief         mount SD card. <br>
 *                mount SD memory card user area
 *                can be access user area after this function is finished
 *                without errors <br>
 *                turn on power <br>
 * <br>
 *                following is available SD Driver mode <br>
 *                SD_MODE_POLL     : software polling <br>
 *                SD_MODE_HWINT    : hardware interrupt <br>
 *                SD_MODE_SW       : software data transfer (SD_BUF) <br>
 *                SD_MODE_DMA      : DMA data transfer (SD_BUF) <br>
 *                SD_MODE_MEM      : only memory cards <br>
 *                SD_MODE_DS       : only default speed <br>
 *                SD_MODE_VER1X    : ver1.1 host <br>
 *                SD_MODE_VER2X    : ver2.x host
 * @warning       user area should be mounted
 * @param [in]    int32_t sd_port  : channel no (0 or 1)
 * @param [in]    uint32_t mode    : SD Driver operation mode
 * @param [in]    uint32_t voltage : operation voltage
 * @retval        p_hndl->error    : SD handle error value
 * @retval        SD_OK : end of succeed
 * @retval        other : end of error
 *****************************************************************************/
int32_t sd_mount(int32_t sd_port, uint32_t mode, uint32_t voltage);

/* Function Name: sd_read_sect */
/**************************************************************************//**
 * @fn            int32_t sd_read_sect(int32_t sd_port, uint8_t *buff, uint32_t psn, int32_t cnt)
 * @brief         read sector data from card. <br>
 *                read sector data from physical sector number (=psn) by the
 *                number of sectors (=cnt) <br>
 *                if SD Driver mode is SD_MODE_SW, data transfer by
 *                sddev_read_data function <br>
 *                if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [out]   uint8_t *buff   : read data buffer
 * @param [in]    uint32_t psn    : read physical sector number
 * @param [in]    int32_t cnt     : number of read sectors
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_read_sect(int32_t sd_port, uint8_t *buff, uint32_t psn, int32_t cnt);

/* Function Name: sd_write_sect */
/**************************************************************************//**
 * @fn            int32_t sd_write_sect(int32_t sd_port, uint8_t *buff, uint32_t psn, int32_t cnt, int32_t writemode)
 * @brief         write sector data to card. <br>
 *                write sector data from physical sector number (=psn) by the
 *                number of sectors (=cnt) <br>
 *                if SD Driver mode is SD_MODE_SW, data transfer by
 *                sddev_read_data function <br>
 *                if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1).
 * @param [out]   uint8_t *buff     : write data buffer
 * @param [in]    uint32_t psn      : write physical sector number
 * @param [in]    int32_t cnt       : number of write sectors
 * @param [in]    int32_t writemode : memory card write mode <br>
 *                  SD_WRITE_WITH_PREERASE : pre-erease write <br>
 *                  SD_WRITE_OVERWRITE     : overwrite
 * @retval        SD_OK  : end of succeed.
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_write_sect(int32_t sd_port, uint8_t *buff, uint32_t psn, int32_t cnt, int32_t writemode);

/* Function Name: sd_get_type */
/**************************************************************************//**
 * @fn            int32_t sd_get_type(int32_t sd_port, uint16_t *type, uint16_t *speed, uint8_t *capa)
 * @brief         get card type. <br>
 *                get mounting card type, current and supported speed mode
 *                and capacity type <br>
 *                (if SD memory card) <br>
 *                following card types are defined <br>
 *                SD_MEDIA_UNKNOWN : unknown media <br>
 *                SD_MEDIA_MMC     : MMC card <br>
 *                SD_MEDIA_SD      : SD Memory card
 * @warning       if pointer has NULL ,the value isn't returned <br>
 *                only SD memory card, speed mode has meaning
 * @param [in]    int32_t  sd_port : channel no (0 or 1)
 * @param [out]   uint16_t *type   : mounting card type
 * @param [out]   uint16_t *speed  : speed mode
 * @param [out]   uint8_t  *capa   : card capacity <br>
 *                  Standard capacity : 0, High capacity : 1
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_type(int32_t sd_port, uint16_t *type, uint16_t *speed, uint8_t *capa);

/* Function Name: sd_get_size */
/**************************************************************************//**
 * @fn            int32_t sd_get_size(int32_t sd_port, uint32_t *user, uint32_t *protect)
 * @brief         get card size. <br>
 *                get total sectors of user area calculated from CSD <br>
 *                get total sectors of protect area calculated from CSD and
 *                SD STAUS
 * @warning       if pointer has NULL ,the value isn't returned <br>
 *                return just the number of all sectors <br>
 *                only SD memory card, protect area size has meaning
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [out]   uint32_t *user    : user area size
 * @param [out]   uint32_t *protect : protect area size
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_size(int32_t sd_port, uint32_t *user, uint32_t *protect);

/* Function Name: sd_iswp */
/**************************************************************************//**
 * @fn            int32_t sd_iswp(int32_t sd_port)
 * @brief         check hardware write protect. <br>
 *                check hardware write protect refer to SDHI register <br>
 *                if WP pin is disconnected to SDHI, return value has no
 *                meaning
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        p_hndl->write_protect : write protected state
 *****************************************************************************/
int32_t sd_iswp(int32_t sd_port);

/* Function Name: sd_unmount */
/**************************************************************************//**
 * @fn            int32_t sd_unmount(int32_t sd_port)
 * @brief         unmount card. <br>
 *                turn off power
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK : end of succeed
 *****************************************************************************/
int32_t sd_unmount(int32_t sd_port);

/* Function Name: sd_stop */
/**************************************************************************//**
 * @fn            void    sd_stop(int32_t sd_port);
 * @brief         stop operations compulsory. <br>
 *                set flag (=SD handle member stop) stop operations compulsory <br>
 *                if this flag is set, read, write operations is stopped <br>
 *                this flag is used for card detect/removal interrupt detection
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        none
 *****************************************************************************/
void    sd_stop(int32_t sd_port);

/* Function Name: sd_set_intcallback */
/**************************************************************************//**
 * @fn            int32_t sd_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t))
 * @brief         register SD_INFO1 or SD_INFO2 interrupt callback function. <br>
 *                register callback function <br>
 *                if SD_INFO1 or SD_INFO2 interrupt are occured, call callback
 *                function
 * @warning       .
 * @param [in]    int32_t sd_port                       : channel no (0 or 1)
 * @param [in]    int32_t (*callback)(int32_t, int32_t) : callback function
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t));

/* Function Name: sd_set_dma_intcallback */
/**************************************************************************//**
 * @fn            int32_t sd_set_dma_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t))
 * @brief         register DM_CM_INFO1 or DM_CM_INFO2 interrupt callback function. <br>
 *                register callback function <br>
 *                if DM_CM_INFO1 or DM_CM_INFO2 interrupt are occured, call callback
 *                function
 * @warning       .
 * @param [in]    int32_t sd_port                       : channel no (0 or 1)
 * @param [in]    int32_t (*callback)(int32_t, int32_t) : callback function
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_set_dma_intcallback(int32_t sd_port, int32_t (*callback)(int32_t, int32_t));

/* Function Name: sd_int_handler */
/**************************************************************************//**
 * @fn            void    sd_int_handler(int32_t sd_port)
 * @brief         SD_INFO1 and SD_INFO2 interrupt handler <br>
 *                examine the relevant elements (without masked) <br>
 *                save those elements to int_info1 or int_info2 <br>
 *                if a callback function is registered, call it
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        none
 *****************************************************************************/
void    sd_int_handler(int32_t sd_port);

/* Function Name: sd_get_error */
/**************************************************************************//**
 * @fn            int32_t sd_get_error(int32_t sd_port)
 * @brief         get SD Driver errors (=p_hndl->error) and return it
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        p_hndl->error
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_error(int32_t sd_port);

/* Function Name: sd_check_int */
/**************************************************************************//**
 * @fn            int32_t sd_check_int(int32_t sd_port)
 * @brief         check SD_INFO1 and SD_INFO2 interrupt elements <br>
 *                if any interrupt is detected, return SD_OK <br>
 *                if no interrupt is detected, return SD_ERR
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_check_int(int32_t sd_port);

/* Function Name: sd_int_dm_handler */
/**************************************************************************//**
 * @fn            void    sd_int_dm_handler(int32_t sd_port)
 * @brief         DM_CM_INFO1 and DM_CM_INFO2 interrupt handler <br>
 *                examine the relevant elements (without masked) <br>
 *                save those elements to int_dm_info1 or int_dm_info2 <br>
 *                if a callback function is registered, call it
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        none
 *****************************************************************************/
void    sd_int_dm_handler(int32_t sd_port);

/* Function Name: sd_check_int_dm */
/**************************************************************************//**
 * @fn            int32_t sd_check_int_dm(int32_t sd_port)
 * @brief         check DM_CM_INFO1 and DM_CM_INFO2 interrupt elements <br>
 *                if any interrupt is detected, return SD_OK <br>
 *                if no interrupt is detected, return SD_ERR
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : any interrupt occured
 * @retval        SD_ERR : no interrupt occured
 *****************************************************************************/
int32_t sd_check_int_dm(int32_t sd_port);

/* Function Name: sd_get_reg */
/**************************************************************************//**
 * @fn            int32_t sd_get_reg(int32_t sd_port, uint8_t *ocr, uint8_t *cid,
 *                      uint8_t *csd, uint8_t *dsr, uint8_t *scr)
 * @brief         get card register. <br>
 *                get card register value
 * @warning       if pointer has NULL ,the register value isn't returned
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [out]   uint8_t *ocr    : OCR register address
 * @param [out]   uint8_t *cid    : CID register address
 * @param [out]   uint8_t *csd    : CSD register address
 * @param [out]   uint8_t *dsr    : DSR register address
 * @param [out]   uint8_t *scr    : SCR register address
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_reg(int32_t sd_port, uint8_t *ocr, uint8_t *cid, uint8_t *csd, uint8_t *dsr, uint8_t *scr);

/* Function Name: sd_get_rca */
/**************************************************************************//**
 * @fn            int32_t sd_get_rca(int32_t sd_port, uint8_t *rca)
 * @brief         get rca register. <br>
 *                get card register value
 * @warning       if pointer has NULL ,the register value isn't returned
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [out]   uint8_t *rca    : RCA register address
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_rca(int32_t sd_port, uint8_t *rca);

/* Function Name: sd_get_sdstatus */
/**************************************************************************//**
 * @fn            int32_t sd_get_sdstatus(int32_t sd_port, uint8_t *sdstatus)
 * @brief         get SD Status register value
 * @warning       if pointer has NULL ,the register value isn't returned
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [out]   uint8_t *sdstatus : SD STATUS address
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_sdstatus(int32_t sd_port, uint8_t *sdstatus);

/* Function Name: sd_get_speed */
/**************************************************************************//**
 * @fn            int32_t sd_get_speed(int32_t sd_port, uint8_t *clss, uint8_t *move)
 * @brief         get card speed. <br>
 *                get card speed class and performance move
 * @warning       if pointer has NULL ,the register value isn't returned
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [out]   uint8_t *clss   : speed class
 * @param [out]   uint8_t *move   : performance move
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_speed(int32_t sd_port, uint8_t *clss, uint8_t *move);

/* Function Name: sd_finalize */
/**************************************************************************//**
 * @fn            int32_t sd_finalize(int32_t sd_port)
 * @brief         do finish operation of SD Driver (2ports). <br>
 *                finish SD Driver <br>
 *                reset SDHI include card detection/removal
 * @warning       after this function finished, SD handle is unavailable
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_finalize(int32_t sd_port);

/* Function Name: sd_set_seccnt */
/**************************************************************************//**
 * @fn            int32_t sd_set_seccnt(int32_t sd_port, int16_t sectors)
 * @brief         set maximum block count per multiple command
 * @warning       maximam block count is constrained from 3 to 32767(0x7fff)
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int16_t sectors : block count
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_set_seccnt(int32_t sd_port, int16_t sectors);

/* Function Name: sd_get_seccnt */
/**************************************************************************//**
 * @fn            int32_t sd_get_seccnt(int32_t sd_port)
 * @brief         get block count. <br>
 *                get maximum block count per multiple command
 * @warning       maximam block count are constrained from 1 to 65535
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        not less than 0 : block count
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_seccnt(int32_t sd_port);

/* Function Name: sd_get_ver */
/**************************************************************************//**
 * @fn            int32_t sd_get_ver(int32_t sd_port, uint16_t *sdhi_ver, char_t *sddrv_ver)
 * @brief         get SDHI and SD Driver versions. <br>
 *                get SDHI version from VERSION register <br>
 *                get SD Driver version from constant DRIVER NAME
 * @warning       if pointer has NULL ,the value isn't returned
 * @param [in]    int32_t sd_port    : channel no (0 or 1)
 * @param [out]   uint16_t *sdhi_ver : SDHI version
 * @param [out]   char_t *sddrv_ver  : SD Driver version
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_get_ver(int32_t sd_port, uint16_t *sdhi_ver, char_t *sddrv_ver);

/* Function Name: sd_set_cdtime */
/**************************************************************************//**
 * @fn            int32_t sd_set_cdtime(int32_t sd_port, uint16_t cdtime)
 * @brief         set card detect time. <br>
 *                set card detect time equal to IMCLK*2^(10+cdtime)
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    uint16_t cdtime : card detect time interval
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_set_cdtime(int32_t sd_port, uint16_t cdtime);

/* Function Name: sd_set_responsetime */
/**************************************************************************//**
 * @fn            int32_t sd_set_responsetime(int32_t sd_port, uint16_t responsetime)
 * @brief         set response timeout. <br>
 *                set response timeout equal to IMCLK*2^(13+cdtime)
 * @warning       .
 * @param [in]    int32_t sd_port       : channel no (0 or 1)
 * @param [in]    uint16_t responsetime : response timeout interval
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_set_responsetime(int32_t sd_port, uint16_t responsetime);

/* Function Name: sd_set_buffer */
/**************************************************************************//**
 * @fn            int32_t sd_set_buffer(int32_t sd_port, void *buff, uint32_t size)
 * @brief         initialize SD driver work buffer. <br>
 *                this buffer is used for mainly MKB process
 * @warning       if applied to CPRM, allocating more than 8K bytes
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    void *buff      : work buffer address
 * @param [in]    uint32_t size   : work buffer size
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_set_buffer(int32_t sd_port, void *buff, uint32_t size);

/* Function Name: sd_standby */
/**************************************************************************//**
 * @fn            int32_t sd_standby(int32_t sd_port)
 * @brief         transfer card to stand-by state. <br>
 *                transfer card from transfer state to stand-by state
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_standby(int32_t sd_port);

/* Function Name: sd_active */
/**************************************************************************//**
 * @fn            int32_t sd_active(int32_t sd_port)
 * @brief         transfer card to transfer state. <br>
 *                transfer card from stand-by state to transfer state
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_active(int32_t sd_port);

/* Function Name: sd_inactive */
/**************************************************************************//**
 * @fn            int32_t sd_inactive(int32_t sd_port)
 * @brief         transfer card to inactive state. <br>
 *                transfer card from any state to inactive state
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_inactive(int32_t sd_port);

/* Function Name: sd_reget_reg */
/**************************************************************************//**
 * @fn            int32_t sd_reget_reg(int32_t sd_port, uint8_t *reg, int32_t is_csd)
 * @brief         reget register. <br>
 *                reget CID or CSD
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [out]   uint8_t *reg    : reget CID or CSD register address
 * @param [in]    int32_t is_csd  : CID(=0) or CSD(=1)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_reget_reg(int32_t sd_port, uint8_t *reg, int32_t is_csd);

/* Function Name: sd_set_tmpwp */
/**************************************************************************//**
 * @fn            int32_t sd_set_tmpwp(int32_t sd_port, int32_t is_set)
 * @brief         set tempolary write protect. <br>
 *                set temporary write protect programing csd
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int32_t is_set  : set or clear
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_set_tmpwp(int32_t sd_port, int32_t is_set);

/* Function Name: sd_lock_unlock */
/**************************************************************************//**
 * @fn            int32_t sd_lock_unlock(int32_t sd_port, uint8_t code, uint8_t *pwd, uint8_t len)
 * @brief         lock/unlock. <br>
 *                lock/unlock operation <br>
 *                passward length is up to 16 bytes <br>
 *                case of cahnge passward, total length is 32 bytes,that is
 *                old and new passward maximum length
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    uint8_t code    : operation code
 * @param [in]    uint8_t *pwd    : passward
 * @param [in]    uint8_t len     : passward length
 * @retval        SD_OK  : end of succeed.
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sd_lock_unlock(int32_t sd_port, uint8_t code, uint8_t *pwd, uint8_t len);
/* Function Name: sdio_read_direct */
/**************************************************************************//**
 * @fn            int32_t sdio_read_direct(int32_t sd_port, uint8_t *buff,uint32_t func,uint32_t adr)
 * @brief         direct read io register from specified address (=adr)
 *                using CMD52
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [out]   uint8_t *buff   : data buffer
 * @param [in]    uint32_t func   : access function number
 * @param [in]    uint32_t adr    : access register address
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_read_direct(int32_t sd_port, uint8_t *buff, uint32_t func, uint32_t adr);

/* Function Name: sdio_write_direct */
/**************************************************************************//**
 * @fn            int32_t sdio_write_direct(int32_t sd_port, uint8_t *buff, 
 *                          uint32_t func, uint32_t adr, uint32_t raw_flag);
 * @brief         direct write io register from specified address (=adr)
 *                using CMD52
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [out]   uint8_t *buff     : data buffer
 * @param [in]    uint32_t func     : access function number
 * @param [in]    uint32_t adr      : access register address
 * @param [in]    uint32_t raw_flag : write mode
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_write_direct(int32_t sd_port, uint8_t *buff, uint32_t func, uint32_t adr, uint32_t raw_flag);

/* Function Name: sdio_check_int */
/**************************************************************************//**
 * @fn            int32_t sdio_check_int(int32_t sd_port)
 * @brief         check SDIO_INFO1 interrupt elements
 *                if any interrupt is detected, return SD_OK
 *                if no interrupt is detected, return SD_ERR
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_check_int(int32_t sd_port);

/* Function Name: sdio_int_handler */
/**************************************************************************//**
 * @fn            void    sdio_int_handler(int32_t sd_port)
 * @brief         SDIO_INFO1 interrupt handler
 *                examine the relevant elements (without masked)
 *                save those elements to int_io_info
 *                if a callback function is registered, call it
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        none
 *****************************************************************************/
void    sdio_int_handler(int32_t sd_port);

/* Function Name: sdio_set_intcallback */
/**************************************************************************//**
 * @fn            int32_t sdio_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t))
 * @brief         register SDIO_INFO1 callback function
 *                if SDIO_INFO1 interrupt are occured, call callback function
 * @warning       .
 * @param [in]    int32_t sd_port              : channel no (0 or 1)
 * @param [in]    int32_t (*callback)(int32_t) : callback function
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_set_intcallback(int32_t sd_port, int32_t (*callback)(int32_t));

/* Function Name: sdio_enable_int */
/**************************************************************************//**
 * @fn            int32_t sdio_enable_int(int32_t sd_port)
 * @brief         enable SDHI detect interrupt from SDIO (=IRQ)
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 *****************************************************************************/
int32_t sdio_enable_int(int32_t sd_port);

/* Function Name: sdio_disable_int */
/**************************************************************************//**
 * @fn            int32_t sdio_disable_int(int32_t sd_port)
 * @brief         disable SDHI detect interrupt from SDIO (=IRQ)
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 *****************************************************************************/
int32_t sdio_disable_int(int32_t sd_port);

/* Function Name: sdio_read */
/**************************************************************************//**
 * @fn            int32_t sdio_read(int32_t sd_port, uint8_t *buff, 
 *                  uint32_t func, uint32_t adr, int32_t cnt, uint32_t op_code)
 * @brief         read io register from specified address (=adr) by the
 *                number of bytes or blocks (=cnt)
 *                if SD Driver mode is SD_MODE_SW, data transfer by
 *                sddev_read_data function
 *                if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * @warning       .
 * @param [in]    int32_t sd_port  : channel no (0 or 1)
 * @param [out]   uint8_t *buff    : read data buffer
 * @param [in]    uint32_t func    : access function number
 * @param [in]    uint32_t adr     : read register address
 * @param [in]    int32_t cnt      : number of read registers (byte)
 * @param [in]    uint32_t op_code : operation code
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_read(int32_t sd_port, uint8_t *buff, uint32_t func, uint32_t adr, int32_t cnt, uint32_t op_code);

/* Function Name: sdio_write */
/**************************************************************************//**
 * @fn            int32_t sdio_write(int32_t sd_port, uint8_t *buff, uint32_t func, 
 *                  uint32_t adr, int32_t cnt, uint32_t op_code)
 * @brief         write io register from (or at) the address (=adr) by the
 *                number of bytes or blocks (=cnt)
 *                if SD Driver mode is SD_MODE_SW, data transfer by
 *                sddev_read_data function
 *                if SD Driver mode is SD_MODE_DMA, data transfer by DMAC
 * @warning       .
 * @param [in]    int32_t sd_port  : channel no (0 or 1)
 * @param [out]   uint8_t *buff    : write data buffer
 * @param [in]    uint32_t func    : access function number
 * @param [in]    uint32_t adr     : read register address
 * @param [in]    int32_t cnt      : number of write registers (byte)
 * @param [in]    uint32_t op_code : operation code
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_write(int32_t sd_port, uint8_t *buff, uint32_t func, uint32_t adr, int32_t cnt, uint32_t op_code);

/* Function Name: sdio_reset */
/**************************************************************************//**
 * @fn            int32_t sdio_reset(int32_t sd_port)
 * @brief         Reset SDIO Function
 * @warning       if pointer has NULL ,the register value isn't returned
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_reset(int32_t sd_port);

/* Function Name: sdio_get_ioocr */
/**************************************************************************//**
 * @fn            int32_t sdio_get_ioocr(int32_t sd_port, uint32_t *ioocr)
 * @brief         get io ocr register value
 * @warning       if pointer has NULL ,the register value isn't returned
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    uint32_t *ioocr : IO OCR register address
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_get_ioocr(int32_t sd_port, uint32_t *ioocr);

/* Function Name: sdio_get_ioinfo */
/**************************************************************************//**
 * @fn            int32_t sdio_get_ioinfo(int32_t sd_port, uint8_t *ioinfo)
 * @brief         get io information(IO OCR[31:24])
 * @warning       if pointer has NULL ,the register value isn't returned
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [out]   uint8_t *ioinfo : io information(IO OCR[31:24])
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_get_ioinfo(int32_t sd_port, uint8_t *ioinfo);

/* Function Name: sdio_get_cia */
/**************************************************************************//**
 * @fn            int32_t sdio_get_cia(int32_t sd_port, uint8_t *reg, uint8_t *cis, 
 *                  uint32_t func_num, int32_t cnt)
 * @brief         get cia value
 * @warning       if pointer has NULL ,the register value isn't returned
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [out]   uint8_t *reg      : CCCR or FBR register address
 * @param [out]   uint8_t *cis      : CIS register address
 * @param [in]    uint32_t func_num : function number (0 to 7, 0 means Common)
 * @param [in]    int32_t cnt       : size of CIS to read
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_get_cia(int32_t sd_port, uint8_t *reg, uint8_t *cis, uint32_t func_num, int32_t cnt);

/* Function Name: sdio_set_enable */
/**************************************************************************//**
 * @fn            int32_t sdio_set_enable(int32_t sd_port, uint8_t func_bit)
 * @brief         set io functions access raedy state
 * @warning       .
 * @param [in]    int32_t sd_port  : channel no (0 or 1)
 * @param [in]    uint8_t func_bit : specify function by bit map
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_set_enable(int32_t sd_port, uint8_t func_bit);

/* Function Name: sdio_get_ready */
/**************************************************************************//**
 * @fn            int32_t sdio_get_ready(int32_t sd_port, uint8_t *func_bit)
 * @brief         inquire io function's access ready status
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [out]   uint8_t *func_bit : access enable function by bit map
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_get_ready(int32_t sd_port, uint8_t *func_bit);

/* Function Name: sdio_set_int */
/**************************************************************************//**
 * @fn            int32_t sdio_set_int(int32_t sd_port, uint8_t func_bit, int32_t enab)
 * @brief         enable or disable interrupt from io functions
 * @warning       .
 * @param [in]    int32_t sd_port  : channel no (0 or 1)
 * @param [in]    uint8_t func_bit : specify enable function by bit map
 * @param [in]    int32_t enab     : control enable or disable
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_set_int(int32_t sd_port, uint8_t func_bit, int32_t enab);

/* Function Name: sdio_get_int */
/**************************************************************************//**
 * @fn            int32_t sdio_get_int(int32_t sd_port, uint8_t *func_bit, int32_t *enab)
 * @brief         inquire io functions's interrupt status
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [out]   uint8_t *func_bit : ready function by bit map
 * @param [out]   int32_t *enab     : control enable or disable
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_get_int(int32_t sd_port, uint8_t *func_bit, int32_t *enab);

/* Function Name: sdio_set_blocklen */
/**************************************************************************//**
 * @fn            int32_t sdio_set_blocklen(int32_t sd_port, uint16_t len, uint32_t func_num)
 * @brief         set io function's block length
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [in]    uint16_t len      : block length
 * @param [in]    uint32_t func_num : specify function by number (from 0 to 7)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_set_blocklen(int32_t sd_port, uint16_t len, uint32_t func_num);

/* Function Name: sdio_get_blocklen */
/**************************************************************************//**
 * @fn            int32_t sdio_get_blocklen(int32_t sd_port, uint16_t *len, uint32_t func_num)
 * @brief         inquire io function's block length
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [out]   uint16_t *len     : block length
 * @param [in]    uint32_t func_num : specify function by number (from 0 to 7)
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_get_blocklen(int32_t sd_port, uint16_t *len, uint32_t func_num);

/* Function Name: sdio_abort */
/**************************************************************************//**
 * @fn            void    sdio_abort(int32_t sd_port, uint32_t func_num)
 * @brief         set flag (=SD handle member stop) stop operations compulsory
 *                if this flag is set, read, write, format operations is
 *                stopped
 *                this flag is used for card detect/removal interrupt detection
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [in]    uint32_t func_num : function number
 * @retval        none
 *****************************************************************************/
void    sdio_abort(int32_t sd_port, uint32_t func_num);

/* Function Name: sdio_set_blkcnt */
/**************************************************************************//**
 * @fn            int32_t sdio_set_blkcnt(int32_t sd_port, int16_t blocks)
 * @brief         set maximum block count per multiple command
 * @warning       maximam block count is constrained from 3 to 32767(0x7fff)
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int16_t blocks  : block count
 * @retval        SD_OK  : end of succeed
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_set_blkcnt(int32_t sd_port, int16_t blocks);

/* Function Name: sdio_get_blkcnt */
/**************************************************************************//**
 * @fn            int32_t sdio_get_blkcnt(int32_t sd_port)
 * @brief         get maximum block count per multiple command
 * @warning       maximam block count are constrained from 1 to 65535
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        not less than 0 : block count
 * @retval        SD_ERR : end of error
 *****************************************************************************/
int32_t sdio_get_blkcnt(int32_t sd_port);

/* ---- target CPU I/F ---- */
/* Function Name: sddev_init */
/**************************************************************************//**
 * @fn            int32_t sddev_init(int32_t sd_port)
 * @brief         Initialize H/W to use SDHI
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_init(int32_t sd_port);

/* Function Name: sddev_power_on */
/**************************************************************************//**
 * @fn            int32_t sddev_power_on(int32_t sd_port)
 * @brief         Power-on H/W to use SDHI
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_power_on(int32_t sd_port);

/* Function Name: sddev_power_off */
/**************************************************************************//**
 * @fn            int32_t sddev_power_off(int32_t sd_port)
 * @brief         Power-off H/W to use SDHI
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_power_off(int32_t sd_port);

/* Function Name: sddev_read_data */
/**************************************************************************//**
 * @fn            int32_t sddev_read_data(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num)
 * @brief         read from SDHI buffer FIFO
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [out]   uint8_t *buff     : buffer addrees to store reading datas
 * @param [in]    uint32_t reg_addr : SDIP FIFO address
 * @param [in]    int32_t num       : counts to read(unit : byte)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_read_data(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num);

/* Function Name: sddev_write_data */
/**************************************************************************//**
 * @fn            int32_t sddev_write_data(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num)
 * @brief         write to SDHI buffer FIFO
 * @warning       .
 * @param [in]    int32_t sd_port   : channel no (0 or 1)
 * @param [in]    uint8_t *buff     : buffer addrees to store writting datas
 * @param [in]    uint32_t reg_addr : SDIP FIFO address
 * @param [in]    int32_t num       : counts to write(unit : byte)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_write_data(int32_t sd_port, uint8_t *buff, uint32_t reg_addr, int32_t num);

/* Function Name: sddev_get_clockdiv */
/**************************************************************************//**
 * @fn            uint32_t sddev_get_clockdiv(int32_t sd_port, int32_t clock)
 * @brief         Get clock div value.
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int32_t clock   : request clock frequency <br>
 *                  SD_CLK_50MHZ <br>
 *                  SD_CLK_25MHZ <br>
 *                  SD_CLK_20MHZ <br>
 *                  SD_CLK_10MHZ <br>
 *                  SD_CLK_5MHZ <br>
 *                  SD_CLK_1MHZ <br>
 *                  SD_CLK_400KHZ
 * @retval        clock div value <br>
 *                  SD_DIV_4   : 1/4   clock <br>
 *                  SD_DIV_8   : 1/8   clock <br>
 *                  SD_DIV_16  : 1/16  clock <br>
 *                  SD_DIV_32  : 1/32  clock <br>
 *                  SD_DIV_256 : 1/256 clock <br>
 *                  SD_DIV_512 : 1/512 clock
 *****************************************************************************/
uint32_t sddev_get_clockdiv(int32_t sd_port, int32_t clock);

/* Function Name: sddev_set_port */
/**************************************************************************//**
 * @fn            int32_t sddev_set_port(int32_t sd_port, int32_t mode)
 * @brief         setting ports to use SDHI
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int32_t mode    : SD_PORT_PARALLEL : 4bit mode <br>
 *                                : SD_PORT_SERIAL   : 1bit mode
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_set_port(int32_t sd_port, int32_t mode);

/* Function Name: sddev_int_wait */
/**************************************************************************//**
 * @fn            int32_t sddev_int_wait(int32_t sd_port, int32_t time)
 * @brief         Waitting for SDHI Interrupt
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int32_t time    : time out value to wait interrupt
 * @retval        get interrupt : SD_OK
 * @retval        time out      : SD_ERR
 *****************************************************************************/
int32_t sddev_int_wait(int32_t sd_port, int32_t time);

/* Function Name: sddev_init_dma */
/**************************************************************************//**
 * @fn            int32_t sddev_init_dma(int32_t sd_port, uint32_t buff, int32_t dir)
 * @brief         Initialize DMAC to transfer data from SDHI FIFO
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    uint32_t buff   : buffer addrees to transfer datas
 * @param [in]    int32_t dir     : direction to transfer <br>
 *                                :   0 : FIFO -> buffer <br>
 *                                :   1 : buffer -> FIFO
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_init_dma(int32_t sd_port, uint32_t buff, int32_t dir);

/* Function Name: sddev_wait_dma_end */
/**************************************************************************//**
 * @fn            int32_t sddev_wait_dma_end(int32_t sd_port, int32_t cnt)
 * @brief         Wait to complete DMAC transfer
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int32_t cnt     : counts to transfer(unit : byte)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_wait_dma_end(int32_t sd_port, int32_t cnt);

/* Function Name: sddev_disable_dma */
/**************************************************************************//**
 * @fn            int32_t sddev_disable_dma(int32_t sd_port)
 * @brief         Disable DMAC transfer
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_disable_dma(int32_t sd_port);

/* Function Name: sddev_reset_dma */
/**************************************************************************//**
 * @fn            int32_t sddev_reset_dma(int32_t sd_port)
 * @brief         Reset of the SDHI module built-in DMAC.
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_reset_dma(int32_t sd_port);

/* Function Name: sddev_finalize_dma */
/**************************************************************************//**
 * @fn            int32_t sddev_finalize_dma(int32_t sd_port)
 * @brief         Finalize of the SDHI module built-in DMAC.
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_finalize_dma(int32_t sd_port);

/* Function Name: sddev_finalize */
/**************************************************************************//**
 * @fn            int32_t sddev_finalize(int32_t sd_port)
 * @brief         finalize SDHI
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_finalize(int32_t sd_port);

/* Function Name: sddev_loc_cpu */
/**************************************************************************//**
 * @fn            int32_t sddev_loc_cpu(int32_t sd_port)
 * @brief         lock cpu to disable interrupt
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_loc_cpu(int32_t sd_port);

/* Function Name: sddev_unl_cpu */
/**************************************************************************//**
 * @fn            int32_t sddev_unl_cpu(int32_t sd_port)
 * @brief         unlock cpu to enable interrupt
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_unl_cpu(int32_t sd_port);

/* Function Name: sddev_cmd0_sdio_mount */
/**************************************************************************//**
 * @fn            int32_t sddev_cmd0_sdio_mount(int32_t sd_port)
 * @brief         Select to issue CMD0 before SDIO Mount
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : issue CMD0
 * @retval        SD_ERR : not issue CMD0
 *****************************************************************************/
int32_t sddev_cmd0_sdio_mount(int32_t sd_port);

/* Function Name: sddev_cmd8_sdio_mount */
/**************************************************************************//**
 * @fn            int32_t sddev_cmd8_sdio_mount(int32_t sd_port)
 * @brief         Select to issue CMD8 before SDIO Mount
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        SD_OK  : issue CMD8
 * @retval        SD_ERR : not issue CMD8
 *****************************************************************************/
int32_t sddev_cmd8_sdio_mount(int32_t sd_port);
/* Function Name: sddev_cd_layout */
/**************************************************************************//**
 * @fn            int32_t sddev_cd_layout(int32_t sd_port)
 * @brief         CD Terminal Support Confirmation
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        Support     : SD_OK
 * @retval        Not Support : SD_ERR
 *****************************************************************************/
int32_t sddev_cd_layout(int32_t sd_port);

/* Function Name: sddev_wp_layout */
/**************************************************************************//**
 * @fn            int32_t sddev_wp_layout(int32_t sd_port)
 * @brief         WP Terminal Support Confirmation
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        Support     : SD_OK
 * @retval        Not Support : SD_ERR
 *****************************************************************************/
int32_t sddev_wp_layout(int32_t sd_port);

/* Function Name: sddev_get_sc_table_config_ch */
/******************************************************************************
 * @fn            int32_t sddev_get_sc_table_config_ch(int32_t sd_port, int32_t *p_sc_port)
 * @brief         Get the channel number of the table generated by the smart
 *                configurator.
 * @warning       .
 * @param [in]    int32_t sd_port    : channel no (0 or 1)
 * @param [out]   int32_t *p_sc_port : pointer to sc channel no
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t sddev_get_sc_table_config_ch(int32_t sd_port, int32_t *p_sc_port);

/* Function Name: SD_status_callback_function */
/**************************************************************************//**
 * @fn            int32_t SD_status_callback_function(int32_t sd_port, int32_t cd)
 * @brief         Callback interrupt function for SDHI protocol status control
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int32_t cd      : no used
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t SD_status_callback_function(int32_t sd_port, int32_t cd);

/* Function Name: SD_dma_end_callback_function */
/**************************************************************************//**
 * @fn            int32_t SD_dma_end_callback_function(int32_t sd_port, int32_t cd)
 * @brief         Callback interrupt function for complete DMA transfer control
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @param [in]    int32_t cd      : no used
 * @retval        success : SD_OK
 * @retval        fail    : SD_ERR
 *****************************************************************************/
int32_t SD_dma_end_callback_function(int32_t sd_port, int32_t cd);

/* Function Name: SD_confirm_semaphore */
/**************************************************************************//**
 * @fn            void    SD_confirm_semaphore(int32_t sd_port)
 * @brief         Confirm semaphore status
 * @warning       .
 * @param [in]    int32_t sd_port : channel no (0 or 1)
 * @retval        none
 *****************************************************************************/
void    SD_confirm_semaphore(int32_t sd_port);

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

#ifdef    __cplusplus
}
#endif    /* __cplusplus    */

#endif    /* R_SDIF_H */

/* End of File */
