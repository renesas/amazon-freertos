/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"         /* Obtains integer types */
#include "diskio.h"     /* Declarations of disk functions */

#include "r_typedefs.h"
#include "device.h"

/* Definitions of physical drive number for each drive */
#define DEV_SD0     (0)
#define DEV_SD1     (1)

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS ret;
    
    ret = (STA_NODISK | STA_NOINIT);

    switch (pdrv) {
    case DEV_SD0:
    case DEV_SD1:
        ret = sd_disk_status(pdrv);
        break;
    default:

        /* DO NOTHING */
        ;
        break;
    }
    return ret;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    DSTATUS ret;
    
    ret = (STA_NODISK | STA_NOINIT);

    switch (pdrv) {
    case DEV_SD0:
    case DEV_SD1:
        ret = sd_disk_initialize(pdrv);
        break;
    default:

        /* DO NOTHING */
        ;
        break;
    }
    return ret;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    DRESULT ret;
    
    ret = RES_PARERR;

    switch (pdrv) {
    case DEV_SD0:
    case DEV_SD1:
        ret = sd_disk_read(pdrv, buff, sector, count);
        break;
    default:

        /* DO NOTHING */
        ;
        break;
    }
    return ret;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    DRESULT ret;
    
    ret = RES_PARERR;

    switch (pdrv) {
    case DEV_SD0:
    case DEV_SD1:
        ret = sd_disk_write(pdrv, buff, sector, count);
        break;
    default:

        /* DO NOTHING */
        ;
        break;
    }
    return ret;
}
#endif



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    return RES_OK;
}

