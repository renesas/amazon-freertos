/******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized.
* This software is owned by Renesas Electronics Corporation and is  protected
* under all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR  A
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE  EXPRESSLY
* DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE  LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this
* software and to discontinue the availability of this software.
* By using this software, you agree to the additional terms and
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
*******************************************************************************
* Copyright (C) 2010 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************
* File Name    : r_devlink_wrapper.h
* Version      : 2.00
* Description  : Functions to link devices into the C standard IO run time
*                library
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 05.08.2010 1.00    First Release
*         : 14.12.2010 1.01    Added devGetFromList
*         : 25.02.2018 2.00    Revised dwvlink layer to support stdio & direct
******************************************************************************/




/*********************************************************************************************************************
 * @ingroup Interface_Library
 * @defgroup DS_DEVLINK_BASE STDIO Driver Interface Definition
 * @brief Defines the interface for an application to interact with a driver.
 *
 * @anchor RZA2M_DS_DEVLINK_API_EX
 * @par Examples
 *
 * @anchor RZA2M_DS_DEVLINK_API_LIMITATIONS
 * @par Known Limitations
 * NONE
 *
 * @anchor RZA2M_DS_DEVLINK_API_INSTANCES
 * @par Known Implementations
 * @ref NONE_YET
 *
 * @anchor RZA2M_DS_DEVLINK_API_RELATED
 * @par Related modules
 * @{
 **********************************************************************************************************************/


#include "r_typedefs.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <reent.h>
#include <fcntl.h>

#include "driver.h"

#ifndef DEVLINK_H_INCLUDED
#define DEVLINK_H_INCLUDED

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/


/* CUSTOMER NOTICE
 * Very Important information use of the R_USE_ANSI_STDIO_MODE_CFG define
 * R_USE_ANSI_STDIO_MODE_CFG must be defined as (1)
 * The workspace REQUIRES in implementation of the <stdio.h> library to
 * operate correctly.
 * Setting R_USE_ANSI_STDIO_MODE_CFG (0) will configure the software to build
 * WITHOUT the stdio library.
 * The consequences of this may be harmful to the software operation.
 */
#define R_USE_ANSI_STDIO_MODE_CFG (1)


/** Define the maximum number of devices that are expected to be dynamically
   attached. The USB on chip peripheral can only support 10 devices */
#define DEVICE_LINK_MAX_DEVICES     (10)

/** Define the maximum number of dynamic devices that are expected to be dynamically
   attached. The USB on chip peripheral can only support 10 devices */
#define DRIVER_LINK_TABLE_SIZE      (6)

/** The first file direct descriptor available to the user note 0,1,2 are reserved */
#define FIRST_DIRECT_FILE           (3)

/** The first file descriptor available to the user */
#define FIRST_FILE                  (0)

/** The maximum number of driver descriptor available to the user */
#define DEV_MAX_DRIVER_DEVICES      (64)

/** Define the maximum string size for the device information strings */
#define DEVICE_MAX_STRING_SIZE      (128)

/** Define the size of the devlink table, if using posix stdio this size is determined by FOPEN_MAX
 * otherwise the linited is defined in this file */
#ifndef FOPEN_MAX

/** <stdio.h> is not included so define the limit here */
#define IOSTREAM (DEV_MAX_DRIVER_DEVICES)
#define __SLBF  0x0001      /* line buffered */
#define __SNBF  0x0002      /* unbuffered */
#define __SRD   0x0004      /* OK to read */
#define __SWR   0x0008      /* OK to write */

#else

/** FOPEN_MAX should have been defined in <stdio.h> */
#define IOSTREAM (FOPEN_MAX + DEV_MAX_DRIVER_DEVICES)

#endif


/******************************************************************************
Typedef definitions
******************************************************************************/
#pragma pack(1)

/** Define the structure of the information that describes a device */
typedef struct _DEVINFO
{
    /** The device symbolic link name */
    char      pszDeviceLinkName[DEVICE_MAX_STRING_SIZE];

    /** The manufacturer string */
    char      pszManufacturer[DEVICE_MAX_STRING_SIZE];

    /** The product string */
    char      pszProduct[DEVICE_MAX_STRING_SIZE];

    /** The serial number string */
    char      pszSerialNumber[DEVICE_MAX_STRING_SIZE];

    /** The device vendor ID */
    uint16_t  wVID;

    /** The device product ID */
    uint16_t  wPID;

    /** The device version in BCD */
    uint16_t  wbcdVersion;

    /** The device class code */
    uint8_t   byDeviceClass;

    /** The device sub class code */
    uint8_t   byDeviceSubClass;

    /** The device protocol code */
    uint8_t   byProtocolCode;
} DEVINFO,
*PDEVINFO;

/* copy from other devlink.h */

typedef struct _DEVICE *PDEVICE;

typedef struct st_def_driver_t *st_driver_ptr_t;

typedef struct st_def_stream_t *st_stream_ptr_t;

typedef struct _devlink_table_t
{
    /* The link name passed to the open function */
    const char * p_class_link_name;

    /* The p_device_name member in the DEVICE structure */
    const char * p_driver_name;

} st_devlink_table_t;

/** Define the structure of a device which can be loaded at run time */
typedef struct _DYNDEV
{
    /** Pointer to the next on the list */
    struct _DYNDEV *pNext;

    /** Pointer to the device link name */
    char    *pszDeviceLinkName;

    /** Flag to show if the device is allocated */
    bool_t   bfAllocated;

    /** Pointer to functions that open, close, read, write and control the
       device */
    struct st_r_driver_intern_t *p_device_driver;

    /** The information that describes the device */
    DEVINFO Information;
} DYNDEV,
*PDYNDEV;


/** Control structure for CTL_FILE_SEEK */
typedef struct _FILESEEK
{
    long    lOffset;
    int     iBase;
    long    lResult;
} FILESEEK,
*PFILESEEK;

#pragma pack()

/******************************************************************************
Constant Data
******************************************************************************/

/******************************************************************************
Function Prototypes
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern int_t R_DEVLINK_FilePtrDescriptor(FILE *pFile);



/******************************************************************************
Function Name: devGetFromList
Description:   Function to check the device list for a device
Arguments:     IN  pszLinkName - Pointer to the device link name
Return value:  Pointer to the device or NULL if not found
******************************************************************************/


/******************************************************************************
* Function Name: R_SYS_InitIolib
* Description  : ANSI library low level initialisation function
* Arguments    : none
* Return Value : none
******************************************************************************/

extern void  R_DEVLINK_Init(void);

/* Direct Interface */

extern int_t direct_open(char *pszFileName, int_t param);

extern int_t direct_close(int_t handle);

extern int_t direct_read(int handle, uint8_t *buff_ptr, uint32_t count);

extern int_t direct_write(int handle, uint8_t *buff_ptr, uint32_t count);

extern int_t direct_control(int handle, uint32_t ctlCode, void *pCtlStruct);

extern int_t direct_get_version(const char *p_driver_name, st_ver_info_t *info);

#if R_USE_ANSI_STDIO_MODE_CFG
/* Enable STDIO interface */

extern int_t low_open(char *pszFileName, int_t param);

extern int_t low_close(int_t handle);

extern int_t low_read(int handle, uint8_t *buff_ptr, uint32_t count);

extern int_t low_write(int handle, uint8_t *buff_ptr, uint32_t count);

extern int_t control(int handle, uint32_t ctlCode, void *pCtlStruct);

extern int_t get_version(const char *p_driver_name, st_ver_info_t *info);

extern int_t low_lseek(int iFileNumber, uint32_t lOffset, int iBase);

#endif

#ifdef __cplusplus
}
#endif

#endif /* DEVLINK_H_INCLUDED */

/*******************************************************************************************************************//**
 * @} (end addtogroup DS_DEVLINK_BASE)
 ***********************************************************************************************************************/


/******************************************************************************
End  Of File
******************************************************************************/
