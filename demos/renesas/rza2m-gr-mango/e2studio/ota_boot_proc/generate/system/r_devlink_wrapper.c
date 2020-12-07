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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "r_typedefs.h"
#include "driver.h"
#include "r_devlink_wrapper.h"
/*  #include "r_os_abstraction_api.h"  */
#include "r_devlink_wrapper_cfg.h"
#include "application_cfg.h"

/******************************************************************************
 Macro Definitions
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/

#ifdef UNUSED_CODE_20200521
/* Functions provided by lowsrc.c */
static void close_all(char_t *p_stream_name);
static bool_t check_open(st_r_driver_t *pDeviceDriver, char *pszLinkName);
#endif

/* The string that is used to differentiate a device name from a file name */
static const char_t * const gsp_dev_ident = DEVICE_INDENTIFIER;

/* Define a driver function table for no device */
static st_r_driver_t gs_no_driver =
{ "No Device Driver", no_dev_open, no_dev_close, no_dev_read, no_dev_write, no_dev_control, no_dev_get_version, };

/*
 * Interface between devlink and lowsrc
 * Store the lookup table in devlink to allow both direct and posix interfaces to share the same table
 */
static st_stream_t gs_filestream[IOSTREAM];

/**********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
/* Dummy driver configuration */

/**********************************************************************************************************************
 * Function Name: get_first_free_direct_handle
 * Description  : Function to get the first free handle from the list maintained
 *                in the array gs_filestream.
 * Arguments    : IN pszFileName - Pointer to the file name. This is to reserve the file descriptors for the standard
 *                streams defined in stdio.h, stdin, stdout and stderr
 * Return Value : File number or -1 on error
 **********************************************************************************************************************/
static int_t get_first_free_direct_handle(const char *pszFileName)
{
    UNUSED_PARAM(pszFileName);

    /* Allocate all other file descriptors dynamically */
    int_t handle = DRV_ERROR;
    int_t findex;

    for (findex = FIRST_DIRECT_FILE; findex < IOSTREAM; findex++)
    {
        /* NULL signifies that the driver is not loaded */
        if (NULL == gs_filestream[findex].p_device_driver)
        {
            /* First free entry found */
            handle = findex;

            /* force the loop to terminate */
            findex = IOSTREAM + 1;
        }
    }
    return handle;
}
/***********************************************************************************************************************
 End of function get_first_free_direct_handle
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_first_free_call_stdio_handle
 * Description  : Function to get the first free handle from the list maintained
 *                in the array gs_filestream.
 * Arguments    : IN pszFileName - Pointer to the file name. This is to reserve the file descriptors for the standard
 *                streams defined in stdio.h, stdin, stdout and stderr
 * Return Value : File number or -1 on error
 **********************************************************************************************************************/
static int_t get_first_free_call_stdio_handle(const char *pszFileName)
{
    UNUSED_PARAM(pszFileName);

    /* Allocate all other file descriptors dynamically */
    int_t handle = DRV_ERROR;
    int_t findex;

    for (findex = FIRST_FILE; findex < IOSTREAM; findex++)
    {
        /* NULL signifies that the driver is not loaded */
        if (NULL == gs_filestream[findex].p_device_driver)
        {
            /* First free entry found */
            handle = findex;

            /* force the loop to terminate */
            findex = IOSTREAM + 1;
        }
    }
    return handle;
}
/***********************************************************************************************************************
 End of function get_first_free_call_stdio_handle
 **********************************************************************************************************************/

#ifdef UNUSED_CODE_20200521
/**********************************************************************************************************************
 * Function Name: check_open
 * Description  : Function to find out if a device driver is open
 * Arguments    : IN  pDeviceDriver - Pointer to the device driver
 *                IN  pszLinkName - Pointer to the link name
 * Return Value : true if the device is open
 **********************************************************************************************************************/
static bool_t check_open(st_r_driver_t *pDeviceDriver, char *pszLinkName)
{
    int_t index = FOPEN_MAX;

    /* For each file stream */
    while (index--)
    {
        if (gs_filestream[index].p_device_driver == pDeviceDriver)
        {
            if (strcmp(gs_filestream[index].p_stream_name, pszLinkName) == 0)
            {
                return true;
            }
        }
    }
    return false;
}
/**********************************************************************************************************************
 End of function  check_open
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: close_all
 * Description  : Function to close all files of a given name
 * Arguments    : IN  pszStreamName - Pointer to the stream name to close
 * Return Value : none
 **********************************************************************************************************************/
static void close_all(char_t *pszStreamName)
{
    int_t index = FOPEN_MAX;

    /* For each file stream */
    while (index--)
    {
        /* If the device is open */
        if ((gs_filestream[index].p_device_driver)

                /* Check the link name */
                && (strcmp(pszStreamName, gs_filestream[index].p_stream_name) == 0))
        {
            /* Previously this directly hit the low
             * level close function, but left the OS level file
             * open creating a synchronising issue. Modified 020615 to call
             * the OS level close function
             * Previous code line: low_close(filestream[iindex].file_number);*/

            direct_close(gs_filestream[index].file_number);
        }
    }
}
/**********************************************************************************************************************
 End of function  close_all
 **********************************************************************************************************************/
#endif /* UNUSED_CODE_20200521 */

/***********************************************************************************************************************
 * Function Name: dev_get_pointer
 * Description  : Function to get a pointer to the open, close, read, write, control and get_version functions
 * Arguments    : IN  pszLinkName - Pointer to the link name
 *                OUT p_stream_name - Pointer to the device link name
 * Return Value : Pointer to the device or NULL on error
 ***********************************************************************************************************************/
static st_r_driver_t * dev_get_pointer(char *pszLinkName, char **p_stream_name, uint32_t *sc_index_ptr)
{
    /* NULL signifies that the driver is not loaded */
    st_r_driver_t * p_device_driver = NULL;
    int_t device = (sizeof(gs_mount_table)) / sizeof(struct _mount_table_t);

    /* First, search through all the static devices in the mount table */
    while (device--)
    {
        if (strcmp (pszLinkName, gs_mount_table[device].p_stream_name) == 0)
        {
            /* remove the const is necessary for assignment */
            *p_stream_name = (char *) gs_mount_table[device].p_stream_name;
            *sc_index_ptr = gs_mount_table[device].sc_config_index;

            /* const needs removing */
            return (st_r_driver_t *) gs_mount_table[device].p_device_driver;
        }
    }

    /* Device not found */
    return (p_device_driver);
}
/***********************************************************************************************************************
 End of function dev_get_pointer
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_device_name
 * Description  : Function to see if the file name supplied is for a device
 * Arguments    : IN  pszFileName - Pointer to the file name
 * Return Value : Pointer to the device link name or NULL on failure
 **********************************************************************************************************************/
static char_t * get_device_name(const char *pszFileName)
{
    /* NULL signifies that the driver name is not found */
    char_t * p_name = NULL;

    if (strstr (pszFileName, gsp_dev_ident) == pszFileName)
    {
        /* NULL signifies that the driver name is not found */
        p_name = (char_t *) (pszFileName += (sizeof(gsp_dev_ident)));
    }

    return (p_name);
}
/**********************************************************************************************************************
 End of function  get_device_name
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: get_device_stream_from_handle
 * Description  : Function to see if the file name supplied is for a device
 * Arguments    : [IN] handle - The index to the driver to check taken from the gs_filestream
 * Return Value : DRV_SUCCESS - Hand is valid and in use
 *                DRV_ERROR - handle not in use
 **********************************************************************************************************************/
static st_stream_ptr_t get_device_stream_from_handle(const int_t h)
{
    /* NULL signifies that the driver is not loaded */
    st_stream_ptr_t pstream = NULL;

    if ((h >= 0) && (h < IOSTREAM))
    {
        /* NULL signifies that the driver is not loaded */
        if (NULL != gs_filestream[h].p_device_driver)
        {
            /* handle found */
            pstream = &gs_filestream[h];
        }
    }
    return (pstream);
}
/***********************************************************************************************************************
 End of function get_device_stream_from_handle
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: check_valid_handle
 * Description  : Function to ensure that the handle that is presented is in use in the gs_filestream table
 * Arguments    : [IN] handle - The index to the driver to check taken from the gs_filestream
 * Return Value : DRV_SUCCESS - Hand is valid and in use
 *                DRV_ERROR - handle not in use
 **********************************************************************************************************************/
static int_t check_valid_handle(const int_t h)
{
    int_t res = (DRV_ERROR);

    if ((h >= 0) && (h < IOSTREAM))
    {
        /* NULL signifies that the driver is not loaded */
        if (NULL != gs_filestream[h].p_device_driver)
        {
            /* handle found */
            res = DRV_SUCCESS;
        }
    }
    return (res);
}
/***********************************************************************************************************************
 End of function check_valid_handle
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Public Functions
 **********************************************************************************************************************/

#ifdef UNUSED_CODE_20200521
/***********************************************************************************************************************
 * Function Name: get_no_driver
 * Description  : Returns address of dummy driver configuration, used
 * Arguments    : none
 * Return Value : dummy driver configuration
 **********************************************************************************************************************/
st_r_driver_t * get_no_driver(void)
{
    return (&gs_no_driver);
}
/***********************************************************************************************************************
 End of function get_no_driver
 **********************************************************************************************************************/
#endif

/***********************************************************************************************************************
 * Function Name: no_dev_open
 * Description  : Function open stub
 * Arguments    : IN  pStream - Pointer to the file stream
 * Return Value : 0 for success otherwise -1
 ***********************************************************************************************************************/
int_t no_dev_open(st_stream_ptr_t pStream)
{
    /* avoid unused parameter warning */
    (void) pStream;

    return (DRV_ERROR);
}
/***********************************************************************************************************************
 End of function  no_dev_open
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: no_dev_close
 * Description  : Function close stub
 * Arguments    : IN  pStream - Pointer to the file stream
 * Return Value : none
 ***********************************************************************************************************************/
void no_dev_close(st_stream_ptr_t pStream)
{
    /* avoid unused parameter warning */
    (void) pStream;
}
/***********************************************************************************************************************
 End of function  no_dev_close
 ***********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: no_dev_read
 * Description  : Function read stub
 * Arguments    : IN  pStream - Pointer to the file stream
 *                IN  pbyBuffer - Pointer to the destination memory
 *                IN  uiCount - The number of bytes to read
 * Return Value : 0 for success -1 on error
 **********************************************************************************************************************/
int_t no_dev_read(st_stream_ptr_t pStream, uint8_t *pbyBuffer, uint32_t uiCount)
{
    /* avoid unused parameter warning */
    (void) pStream;

    /* avoid unused parameter warning */
    (void) pbyBuffer;

    /* avoid unused parameter warning */
    (void) uiCount;

    return (DRV_ERROR);
}
/**********************************************************************************************************************
 End of function  no_dev_read
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: no_dev_write
 * Description  : Function write stub
 * Arguments    : IN  pStream - Pointer to the file stream
 *                IN  pbyBuffer - Pointer to the destination memory
 *                IN  uiCount - The number of bytes to read
 * Return Value : 0 for success -1 on error
 **********************************************************************************************************************/
int_t no_dev_write(st_stream_ptr_t pStream, const uint8_t *pbyBuffer, uint32_t uiCount)
{
    /* avoid unused parameter warning */
    (void) pStream;

    /* avoid unused parameter warning */
    (void) pbyBuffer;

    /* avoid unused parameter warning */
    (void) uiCount;

    return (DRV_ERROR);
}
/**********************************************************************************************************************
 End of function  no_dev_write
 **********************************************************************************************************************/

#ifdef UNUSED_CODE_20200521
/**********************************************************************************************************************
 * Function Name: no_dev_io
 * Description  : Function read / write stub
 * Arguments    : IN  pStream - Pointer to the file stream
 *                IN  pbyBuffer - Pointer to the destination memory
 *                IN  uiCount - The number of bytes to read
 * Return Value : 0 for success -1 on error
 **********************************************************************************************************************/
int_t no_dev_io(st_stream_ptr_t pStream, uint8_t *pbyBuffer, uint32_t uiCount)
{
    /* avoid unused parameter warning */
    (void)pStream;

    /* avoid unused parameter warning */
    (void)pbyBuffer;

    /* avoid unused parameter warning */
    (void)uiCount;

    return (DRV_ERROR);
}
/**********************************************************************************************************************
 End of function  no_dev_io
 **********************************************************************************************************************/
#endif

/***********************************************************************************************************************
 * Function Name: no_dev_control
 * Description  : Function to control stub
 * Arguments    : IN  pStream - Pointer to the file stream
 *                IN  ctlCode - The custom control code
 *                IN  pCtlStruct - Pointer to the custorm control structure
 * Return Value : 0 for success -1 on error
 ***********************************************************************************************************************/
int_t no_dev_control(st_stream_ptr_t pStream, uint32_t ctlCode, void *pCtlStruct)
{
    /* avoid unused parameter warning */
    (void) pStream;

    /* avoid unused parameter warning */
    (void) ctlCode;

    /* avoid unused parameter warning */
    (void) pCtlStruct;

    return (DRV_ERROR);
}
/***********************************************************************************************************************
 End of function  no_dev_control
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: no_dev_get_version
 * Description  : Function to version control stub
 * Arguments    :
 *
 *  IN  pStream - Pointer to the file stream
 *                IN  st_ver_info_t - The custom control code
 *                IN  pCtlStruct - Pointer to the custorm control structure
 *
 *                st_ver_info_t
 * Return Value : 0 for success -1 on error
 ***********************************************************************************************************************/
int_t no_dev_get_version(st_stream_ptr_t pStream, st_ver_info_ptr_t info_ptr)
{
    /* avoid unused parameter warning */
    (void) pStream;

    /* avoid unused parameter warning */
    (void) info_ptr;

    return DRV_ERROR;
}
/***********************************************************************************************************************
 End of function no_dev_get_version
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: direct_open
 * Description  : devlink direct API open driver function, opens the named driver assuming the name is found
 *                and a handle is free.
 *                direct functions do not use R_OS_Abstraction interface as they may be called before
 *                memory manager is initialised.
 * Arguments    : [IN] p_driver_name - Pointer to the file name (NULL terminated)
 *                [IN] file_flag - the file access mode parameter
 * Return Value : 0 for success -1 on error
 **********************************************************************************************************************/
int_t direct_open(char *p_driver_name, int_t file_flag)
{
    int_t handle = DRV_ERROR;
    int_t local_handle;
    int_t open_result = DRV_ERROR;

    /* Get the next free handle between  values of FIRST_DIRECT_FILE and IOSTREAM, or DRV_ERROR */
    local_handle = get_first_free_direct_handle (p_driver_name);

    if (DRV_ERROR != local_handle)
    {
        char_t link_name[32];

        /* pre-fix the driver ID onto the device_name allowing direct and stdio calls to share code */
        sprintf (link_name, "%s%s", DEVICE_INDENTIFIER, p_driver_name);

        char_t * p_link_name = get_device_name (link_name);

        /* check to see if this is a driver NULL signifies driver name not found */
        if (p_link_name)
        {
            char_t * p_stream_name;
            uint32_t sc_index;

            st_r_driver_t * p_device_driver = dev_get_pointer (p_link_name, &p_stream_name, &sc_index);

            gs_filestream[local_handle].p_device_driver = &gs_no_driver;

            if (p_device_driver)
            {
                gs_filestream[local_handle].p_stream_name = p_stream_name;
                gs_filestream[local_handle].p_device_driver = p_device_driver;
                gs_filestream[local_handle].sc_config_index = sc_index;
            }
            else
            {
                open_result = DRV_ERROR;
                handle = DRV_ERROR;
            }

            /* If we found a device or a file then open it */
            if ((gs_filestream[local_handle].p_device_driver)
                    && ((&gs_no_driver) != gs_filestream[local_handle].p_device_driver))
            {
                st_stream_ptr_t pstream = &gs_filestream[local_handle];

                pstream->file_number = local_handle;

                /* check for read and write */
                if ((file_flag & O_RDWR) > 0)
                {
                    pstream->file_mode = (__SRD | __SWR);
                }

                /* check for write only */
                else if ((file_flag & O_WRONLY) > 0)
                {
                    pstream->file_mode = __SWR;
                }
                else
                {
                    /* If a open is requested with unrecognised flags such as 'rb', grant read only access */
                    /* to allow a level of functionality */
                    pstream->file_mode = __SRD;
                }

                /* check for create & add write */
                if ((file_flag & O_CREAT) > 0)
                {
                    pstream->file_mode |= __SWR;
                }

                pstream->file_flag = file_flag;

                open_result = pstream->p_device_driver->open (pstream);
            }

            if (DRV_ERROR == open_result)
            {
                /* Device failed to open */
                memset (&gs_filestream[local_handle], 0x00, sizeof(st_stream_t));

                /* NULL signifies that the driver is not loaded */
                gs_filestream[local_handle].p_device_driver = NULL;
            }
            else
            {
                /* Device opened successfully assign the handle */
                handle = local_handle;
            }
        }
    }

    return (handle);
}
/***********************************************************************************************************************
 End of function direct_open
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: direct_close
 * Description  : devlink direct api close driver function, closes the driver and frees any resources
 * Arguments    : [IN] handle - The index to the driver to close taken from the gs_filestream
 * Return Value : 0 for success, DRV_ERROR on error
 **********************************************************************************************************************/
int_t direct_close(int_t handle)
{
    int_t res;

    res = check_valid_handle (handle);

    if (DRV_ERROR != res)
    {
        st_stream_ptr_t pstream = get_device_stream_from_handle (handle);

        /* driver has closed successfully, release the resource in the devlink (st_stream_t) table */
        gs_filestream[handle].p_device_driver->close (pstream);
        memset (&gs_filestream[handle], 0x00, sizeof(st_stream_t));

        /* Table entry set to NULL to signify no driver loaded */
        gs_filestream[handle].p_device_driver = NULL;
    }
    return (res);
}
/***********************************************************************************************************************
 End of function direct_close
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: direct_read
 * Description  : devlink direct api read data driver function, reads from the driver
 * Arguments    : [IN]  handle - The file index
 *                [IN]  count - The number of characters to read
 *                [OUT] buff_ptr - The pointer to the buffer
 * Return Value : Number of characters read, -1 on error
 ***********************************************************************************************************************/
int_t direct_read(int handle, uint8_t *buff_ptr, uint32_t count)
{
    int_t res;

    res = check_valid_handle (handle);

    if (DRV_ERROR == res)
    {
        return res;
    }

    res = DRV_ERROR;

    /* File open check */
    if (gs_filestream[handle].p_device_driver)
    {
        st_stream_t * p_stream = &gs_filestream[handle];

        /* File access rights check */
        if ((p_stream->file_flag == O_RDONLY) || (p_stream->file_flag == O_RDWR))
        {
            res = p_stream->p_device_driver->read (p_stream, buff_ptr, count);
        }
    }

    return (res);
}
/***********************************************************************************************************************
 End of function direct_read
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: direct_write
 * Description  : devlink direct api write data driver function, writes to the driver
 * Arguments    : [IN]  handle - The file index
 *                [IN]  buff_ptr - The pointer to the buffer
 *                [IN]  count - The number of characters to read
 * Return Value : Number of characters read, -1 on error
 ***********************************************************************************************************************/
int_t direct_write(int handle, uint8_t *buff_ptr, uint32_t count)
{
    int_t res;

    res = check_valid_handle (handle);

    if (DRV_ERROR == res)
    {
        return res;
    }

    res = DRV_ERROR;

    /* File open check */
    if (gs_filestream[handle].p_device_driver)
    {
        st_stream_t * p_stream = &gs_filestream[handle];

        /* File access rights check */
        if ((p_stream->file_flag & O_WRONLY) || (p_stream->file_flag == O_RDWR))
        {
            res = p_stream->p_device_driver->write (p_stream, buff_ptr, count);
        }
    }

    return (res);
}
/***********************************************************************************************************************
 End of function direct_write
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: direct_control
 * Description  : devlink direct api write data driver function, writes to the driver
 * Arguments    : [IN]  handle - The file index
 *                [IN]  ctlCode - The control code selected
 *                [IN]  pCtlStruct - The pointer to the control structures associated with the control code
 * Return Value : Number of characters read, -1 on error
 ***********************************************************************************************************************/
int_t direct_control(int handle, uint32_t ctlCode, void *pCtlStruct)
{
    int_t res;

    res = check_valid_handle (handle);

    if (DRV_ERROR != res)
    {
        /* File open check */
        if (gs_filestream[handle].p_device_driver)
        {
            st_stream_t * p_stream = &gs_filestream[handle];
            res = p_stream->p_device_driver->control (p_stream, ctlCode, pCtlStruct);
        }
    }
    return (res);
}
/***********************************************************************************************************************
 End of function direct_control
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: direct_get_version
 * Description  : devlink direct api version function, retrieve the driver version number.
 * Arguments    : [IN]  p_driver_name - Pointer to the file name (NULL terminated)
 *                [OUT] info - Pointer to the version_info structure
 * Return Value : 0 for success -1 on error
 **********************************************************************************************************************/
int_t direct_get_version(const char *p_driver_name, st_ver_info_t *info)
{
    e_err_code_t res = DRV_ERROR;
    char_t link_name[32];

    /* pre-fix the driver ID onto the device_name allowing direct and stdio calls to share code */
    sprintf (link_name, "%s%s", DEVICE_INDENTIFIER, p_driver_name);

    char_t * p_link_name = get_device_name (link_name);

    /* Check to see if this driver is a driver and not something else */
    if (p_link_name)
    {
        char_t * p_stream_name;
        uint32_t sc_index;

        st_r_driver_t * p_device_driver = dev_get_pointer (p_link_name, &p_stream_name, &sc_index);

        /* NULL signifies that the driver is not loaded so do not call */
        if (NULL != p_device_driver)
        {
            /*  R_OS_SysWaitAccess();  */

            /* Driver may not be open so must cast st_stream_ptr_t to NULL */
            res = p_device_driver->get_version (NULL, info);

            /*  R_OS_SysReleaseAccess();  */
        }
    }
    return (res);
}
/***********************************************************************************************************************
 End of function direct_get_version
 **********************************************************************************************************************/

#if R_USE_ANSI_STDIO_MODE_CFG
/* Enable STDIO interface */

/******************************************************************************
 * Function Name: low_open
 * Description  : ANSI library low level open function
 * Arguments    : IN  pszFileName - Pointer to the file name
 *                IN  file_flag - Passed on to open function
 * Return Value : File number or -1 on error
 ******************************************************************************/
int_t low_open(char *pszFileName, int file_flag)
{
    int_t handle = DRV_ERROR;
    int_t local_handle;
    int_t open_result = DRV_ERROR;

    /*  R_OS_SysWaitAccess();  */

    /* Get the fist available file number */
    local_handle = get_first_free_call_stdio_handle (pszFileName);
    if (DRV_ERROR != local_handle)
    {
        /* First look for a device */
        char_t * p_link_name = get_device_name (pszFileName);

        /* Initialise the driver pointer */
        gs_filestream[local_handle].p_device_driver = &gs_no_driver;

        /* Check to see if this is a device driver not a file */
        if (p_link_name)
        {
            char_t * p_stream_name;
            uint32_t sc_index;

            st_r_driver_t * p_device_driver = dev_get_pointer (p_link_name, &p_stream_name, &sc_index);

            if (p_device_driver)
            {
                gs_filestream[local_handle].p_stream_name = p_stream_name;
                gs_filestream[local_handle].p_device_driver = p_device_driver;
                gs_filestream[local_handle].sc_config_index = sc_index;
            }
            else
            {
                open_result = DRV_ERROR;
            }
        }

#if INCLUDE_FILE_SYSTEM
        /* If it was not a device look for a file */
        if ((gs_filestream[local_handle].p_device_driver == NULL)
                || (gs_filestream[local_handle].p_device_driver == (&gs_no_driver)))
        {
            st_stream_ptr_t pStream = &gs_filestream[local_handle];
            st_r_driver_t* p_device_driver;

            /* Set the file number for access to _File if required */
            pStream->file_number = local_handle;

            /* Try to load a driver for this file */
            p_device_driver = fileLoadDriver(pStream, pszFileName, file_flag);
            if (p_device_driver)
            {

                /* Set the driver */
                pStream->p_device_driver = p_device_driver;

            }
        }
#endif

        /* If we found a device or a file then open it */
        if ((gs_filestream[local_handle].p_device_driver)
                && ((&gs_no_driver) != gs_filestream[local_handle].p_device_driver))
        {
            st_stream_ptr_t pstream = &gs_filestream[local_handle];

            pstream->file_number = local_handle;

            /* Check for read and write */
            if (file_flag & O_RDWR)
            {
                pstream->file_mode = (__SRD | __SWR);
            }

            /* Check for write only */
            else if (file_flag & (O_WRONLY))
            {
                pstream->file_mode = __SWR;
            }
            else
            {
                /* If a open is requested with unrecognised flags
                 * such as 'rb', grant read only to allow  level
                 * of functionality.
                 */
                /* Always grant read only mode */
                pstream->file_mode = __SRD;
            }

            /* Check for create & add write */
            if (file_flag & O_CREAT)
            {
                pstream->file_mode |= __SWR;
            }
            pstream->file_flag = file_flag;

            open_result = pstream->p_device_driver->open (pstream);
        }
    }

    if (DRV_ERROR == open_result)
    {
        /* Device failed to open */
        memset (&gs_filestream[local_handle], 0x00, sizeof(st_stream_t));

        /* NULL signifies that the driver is not loaded */
        gs_filestream[local_handle].p_device_driver = NULL;
    }
    else
    {
        /* Device opened successfully assign the handle */
        handle = local_handle;
    }

    /*  R_OS_SysReleaseAccess();  */

    return (handle);
}
/******************************************************************************
 End of function low_open
 ******************************************************************************/

/******************************************************************************
 * Function Name: low_close
 * Description  : ANSI library low level close function
 * Arguments    : IN  file_number - The file number to close
 * Return Value : 0 for success, -1 on error
 ******************************************************************************/
int_t low_close(int_t file_number)
{
    return direct_close ((file_number));
}
/******************************************************************************
 End of function low_close
 ******************************************************************************/

/******************************************************************************
 * Function Name: low_read
 * Description  : ANSI library low level read function
 * Arguments    : IN  file_number - The file index
 *                IN  pbyBuffer - The pointer to the buffer
 *                IN  iCount - The number of characters to read
 * Return Value : Number of characters read, -1 on error
 ******************************************************************************/
int_t low_read(int file_number, uint8_t *pbyBuffer, uint32_t iCount)
{
    return direct_read ((file_number), pbyBuffer, iCount);
}
/******************************************************************************
 End of function low_read
 ******************************************************************************/

/******************************************************************************
 * Function Name: low_write
 * Description  : ANSI library low level read function
 * Arguments    : IN  file_number - The file index
 *                IN  pbyBuffer - The pointer to the buffer
 *                IN  iCount - The number of characters to write
 * Return Value : Number of characters written, -1 on error
 ******************************************************************************/
int_t low_write(int file_number, uint8_t *pbyBuffer, uint32_t iCount)
{
    return (direct_write ((file_number), pbyBuffer, iCount));
}
/******************************************************************************
 End of function low_write
 ******************************************************************************/

/******************************************************************************
 * Function Name: control (NOT ANSI)
 * Description  : Function to perform a device control provided by a control code.
 *                This is not a standard ANSI function. It is provided as an extension
 *                to the ANSI architecture.
 * Arguments    : IN  file_number - The file number to control
 *                IN  ctlCode - The device specific control code
 *                IN  pCtlStruct - Pointer to the control structure
 * Return Value : 0 for success -1 on error
 ******************************************************************************/
int_t control(int file_number, uint32_t ctlCode, void *pCtlStruct)
{
    return (direct_control ((file_number), ctlCode, pCtlStruct));
}
/******************************************************************************
 End of function  control
 ******************************************************************************/

/******************************************************************************
 * Function Name: get_version (NOT ANSI)
 * Description  : Function to retrieve the driver version number.
 *                This is not a standard ANSI function. It is provided as an extension
 *                to the ANSI architecture.
 * Arguments    : IN  file_number - The file number to control
 *                IN  pCtlStruct - Pointer to the version_info structure
 * Return Value : 0 for success -1 on error
 ******************************************************************************/
int_t get_version(const char *p_driver_name, st_ver_info_t *info)
{
    return (direct_get_version (p_driver_name, info));
}
/******************************************************************************
 End of function  get_version
 ******************************************************************************/

/******************************************************************************
 * Function Name: low_lseek
 * Description  : ANSI library low level seek function
 * Arguments    : IN  file_number - The file index
 *                IN  lOffset - The offset
 *                IN  iBase - The base
 * Return Value : The offset in bytes to the current file position or -1 on error
 ******************************************************************************/
int_t low_lseek(int file_number, uint32_t lOffset, int iBase)
{
    /* File number range check */
    if ((file_number < 0) || ((FOPEN_MAX - 1) < file_number))
    {
        return (-1L);
    }

    /* File open check */
    if (gs_filestream[file_number].p_device_driver)
    {
        st_stream_ptr_t pstream = &gs_filestream[file_number];
        FILESEEK file_seek;
        file_seek.lOffset = (signed) lOffset;
        file_seek.iBase = iBase;
        file_seek.lResult = 0L;
        if ((pstream->p_device_driver->control (pstream, CTL_FILE_SEEK, &file_seek)) && (0L == file_seek.lResult))
        {
            file_seek.lResult = (-1L );
        }
        return file_seek.lResult;
    }
    return -1L;
}
/******************************************************************************
 End of function low_lseek
 ******************************************************************************/

#endif /* R_USE_ANSI_STDIO_MODE_CFG */

/***********************************************************************************************************************
 * Function Name: R_DEVLINK_Init
 * Description  : Initialisation function
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void R_DEVLINK_Init(void)
{
    /* Initialise the devlink tables */
    memset (gs_filestream, 0, sizeof(gs_filestream));
}
/***********************************************************************************************************************
 End of function R_DEVLINK_Init
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Function Name: R_DEVLINK_FilePtrDescriptor
 Description:   Function to convert a file pointer into a file descriptor
 Arguments:     IN  pFile - Pointer to the file stream;
 Return value:  The file descriptor
 **********************************************************************************************************************/
int_t R_DEVLINK_FilePtrDescriptor(FILE *pFile)
{
    int_t result = (-1);
    if (pFile)
    {
        /* defined in reent.h as struct __sFILE */
        struct __sFILE *p_internal = (struct __sFILE *) pFile;

        /* defined in reent.h short _file; fileno, if Unix descriptor, else -1 */
        result = (int_t) p_internal->_file;
    }
    return (result);
}
/***********************************************************************************************************************
 End of function R_DEVLINK_FilePtrDescriptor
 **********************************************************************************************************************/

/***********************************************************************************************************************
 End  Of File
 **********************************************************************************************************************/
