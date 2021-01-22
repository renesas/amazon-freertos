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
* File Name    : r_eeprom_sample.c
* Device(s)    : RZ/A2M
* Tool-Chain   : e2Studio Ver 7.3.0
*              : GCC ARM Embedded 6.3.1.20170620
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : RZ/A2M Sample Program - EEPROM control by RIIC driver
* Operation    :
* Limitations  :
*******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"

#include "r_devlink_wrapper.h"
#include "r_riic_drv_sc_cfg.h"
#include "r_riic_drv_api.h"

#include "r_os_abstraction_api.h"
#include "r_compiler_abstraction_api.h"

#include "r_eeprom_sample.h"

/*******************************************************************************
 Macro definitions
 *******************************************************************************/
#define EEPROM_DUMMY_READ_BUF_SIZE (1)
/* max time to wait for ACK polling */
/* EEPROM write time is 5msec, so wait about 10msec*/
/* RIIC frequency 400KHz = 50000byte / sec, so do dummy reads of 100bytes to wait 10 msec */  
#define EEPROM_DUMMY_READ_MAX_TIME (100)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

static int_t riic_ackpoll(void);

/* RIIC driver handle */
static int_t handle;

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/*******************************************************************************
 * Function Name: Sample_RIIC_Eeprom_Init
 * Description  : Open RIIC driver for EEPROM control start
 * Arguments    : None
 * Return Value : int_t - error code
 *******************************************************************************/
int_t Sample_RIIC_Eeprom_Init(void)
{
    int_t ret = NO_ERROR;
    
    handle = open(RIIC_CH_EEPROM_RZA2M, O_RDWR);

    if (0 > handle)
    {
        ret = ERROR_FAILURE;
    }

    /* Successful initialisation */
    return ret;
}
/*****************************************************************************
 End of function Sample_RIIC_Eeprom_Init
 *****************************************************************************/

/*****************************************************************************
 * Function Name: Sample_RIIC_Eeprom_UnInit
 * Description  : Close RIIC driver for EEPROM control shutdown
 * Arguments    : None
 * Return Value : int_t - error code
 *****************************************************************************/
int_t Sample_RIIC_Eeprom_UnInit(void)
{
    close(handle);

    return NO_ERROR;
}
/*****************************************************************************
 End of function Sample_RIIC_Eeprom_UnInit
 *****************************************************************************/

/*****************************************************************************
 * Function Name: Sample_RIIC_Eeprom_Write
 * Description  : Write data to EEPROM memory
 * Arguments    : uint8_t addr - start address to write to EEPROM
 *              : const void *data - pointer to the buffer storing data to be written to EEPROM
 *              : size_t size - write data size
 * Return Value : int_t - error code
 *****************************************************************************/
int_t Sample_RIIC_Eeprom_Write(uint16_t addr, const void *data, uint32_t size)
{
    int_t error = NO_ERROR;
    uint32_t write_size;

    /* convert pointer to number */
    uint32_t write_addr = (uint32_t)addr;
    const uint8_t *p_data;
    bool_t first_loop = true;

    /* copy to auto variable */
    p_data = (const uint8_t *) data;
    while (size > 0)
    {
        if (true == first_loop)
        {
            /* writing data up to the EEPROM_PAGE_SIZE byte boundary address in the first page write process */
            write_size = (EEPROM_PAGE_SIZE - (write_addr % EEPROM_PAGE_SIZE));
            if (write_size > size)
            {
                write_size = size;
            }
            
            first_loop = false;
        }
        else
        {
            /* max write size is limited to page size(64byte) after second page write process */
            if (EEPROM_PAGE_SIZE < size)
            {
                write_size = EEPROM_PAGE_SIZE;
            }
            else
            {
                write_size = size;
            }
        }

        /* EEPROM page write */
        st_r_drv_riic_transfer_t i2c_write;
        i2c_write.device_address = EEPROM_DEV_ADDRESS;
        i2c_write.sub_address_type = RIIC_SUB_ADDR_WIDTH_16_BITS;
        i2c_write.sub_address = write_addr;
        i2c_write.number_of_bytes = write_size;

        /* set buffer */
        i2c_write.p_data_buffer = (uint8_t *) p_data;

        if (DRV_ERROR >= control(handle, CTL_RIIC_WRITE, &i2c_write))
        {
            error = ERROR_FAILURE;
            break;
        }
        else
        {
            /* update data pointer, size information */
            p_data += write_size;
            write_addr += write_size;
            size -= write_size;

            /* wait EEPROM write by ack polling of dummy RIIC read */
            error = riic_ackpoll();
            if (ERROR_FAILURE == error)
            {
                break;
            }
        }
    }

    /* Return status code */
    return (error);
}
/*****************************************************************************
 End of function Sample_RIIC_Eeprom_Write
 *****************************************************************************/

/*****************************************************************************
 * Function Name: Sample_RIIC_Eeprom_Read
 * Description  : Read data from EEPROM memory
 * Arguments    : uint8_t addr - start address to read from EEPROM
 *              : const void *data - pointer of the buffer to store the data read from EEPROM
 *              : size_t size - read data size
 * Return Value : int_t - error code
 *****************************************************************************/
int_t Sample_RIIC_Eeprom_Read (uint16_t addr, void *data, uint32_t size)
{
    int_t error = NO_ERROR;
    uint8_t *p_data;
    
    /* copy to auto variable */
    p_data = (uint8_t *)data;
    /* EEPROM sequential read */
    st_r_drv_riic_transfer_t i2c_read;
    i2c_read.device_address = EEPROM_DEV_ADDRESS;
    i2c_read.sub_address_type = RIIC_SUB_ADDR_WIDTH_16_BITS;
    /* convert pointer to number */
    i2c_read.sub_address = (uint32_t)addr;
    i2c_read.number_of_bytes = size;
    /* set the buffer */
    i2c_read.p_data_buffer = (uint8_t *) p_data;

    if (DRV_ERROR >= control(handle, CTL_RIIC_READ, &i2c_read))
    {
        error = ERROR_FAILURE;
    }

    /* Return status code */
    return (error);
}
/*****************************************************************************
 End of function Sample_RIIC_Eeprom_Read
 *****************************************************************************/

/****************************************************************************
 * Function Name: riic_ackpoll
 * Description  : Polling ACK from EEPROM by dummy read
 * Arguments    : none
 * Return Value : int_t - error code
 *****************************************************************************/
static int_t riic_ackpoll(void)
{
    int_t error = NO_ERROR;
    uint32_t dummy_size = EEPROM_DUMMY_READ_BUF_SIZE;
    uint32_t dummy_addr = 0x0000;
    uint8_t dummy_buf[EEPROM_DUMMY_READ_BUF_SIZE];
    uint32_t error_cnt;

    st_r_drv_riic_transfer_t i2c_dummy_read;
    i2c_dummy_read.device_address = EEPROM_DEV_ADDRESS;
    i2c_dummy_read.sub_address_type = RIIC_SUB_ADDR_WIDTH_16_BITS;
    i2c_dummy_read.sub_address = dummy_addr;
    i2c_dummy_read.number_of_bytes = dummy_size;
    i2c_dummy_read.p_data_buffer = dummy_buf;

    error_cnt = 0;
    while (1)
    {
        /* wait for ACK from EEPROM */
        /* Since NACK is returned until writing is completed, an error occurs */
        if (DRV_ERROR == control(handle, CTL_RIIC_READ, &i2c_dummy_read))
        {
            error_cnt++;
            if (EEPROM_DUMMY_READ_MAX_TIME < error_cnt)
            {
                error = ERROR_FAILURE;
                break;
            }
        }
        else
        {
            break;
        }
    }

    /* Return status code */
    return (error);
}
/*****************************************************************************
 End of function riic_ackpoll
 *****************************************************************************/
