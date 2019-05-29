/***********************************************************************
*
*  FILE        : boot_loader.c
*  DATE        : 2019-05-18
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include <stdio.h>
#include <string.h>
#include "r_smc_entry.h"
#include "r_flash_rx_if.h"
#include "r_tfat_lib.h"
#include "r_usb_basic_if.h"
#include "r_usb_hmsc_if.h"
#include "r_sci_rx_if.h"
#include "base64_decode.h"
#include "r_cryptogram.h"

#include "r_sci_rx_pinset.h"
#include "r_usb_basic_pinset.h"

#define BOOT_LOADER_SUCCESS         (0)
#define BOOT_LOADER_FAIL            (-1)
#define BOOT_LOADER_GOTO_INSTALL    (-2)

#define LIFECYCLE_STATE_BLANK             (0)
#define LIFECYCLE_STATE_ON_THE_MARKET     (1)
#define LIFECYCLE_STATE_UPDATING          (2)

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/
/* R_FLASH_Write() arguments: specify "low address" and process to "high address" */
#define BOOT_LOADER_LOW_ADDRESS FLASH_CF_BLOCK_9
#define BOOT_LOADER_MIRROR_LOW_ADDRESS FLASH_CF_BLOCK_47

/* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
#define BOOT_LOADER_MIRROR_HIGH_ADDRESS FLASH_CF_BLOCK_38
#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS FLASH_CF_BLOCK_48

#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL 8
#define BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM 2

#define BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS FLASH_DF_BLOCK_8
#define BOOT_LOADER_CONST_DATA_BLOCK_NUM 8

#define INITIAL_FIRMWARE_FILE_NAME "userprog.rsu"
/*------------------------------------------ firmware update configuration (end) --------------------------------------------*/

#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS FLASH_CF_LO_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS FLASH_CF_HI_BANK_LO_ADDR
#define BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM)
#define BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER (FLASH_NUM_BLOCKS_DF - BOOT_LOADER_CONST_DATA_BLOCK_NUM)
#define USER_RESET_VECTOR_ADDRESS (BOOT_LOADER_LOW_ADDRESS - 4)

#define MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT 3
#define SHA1_HASH_LENGTH_BYTE_SIZE 20


typedef struct _load_firmware_control_block {
    uint32_t flash_buffer[FLASH_CF_MEDIUM_BLOCK_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
    uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];
    uint32_t firmware_length;
}LOAD_FIRMWARE_CONTROL_BLOCK;

typedef struct _load_const_data_control_block {
    uint32_t flash_buffer[FLASH_DF_BLOCK_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
}LOAD_CONST_DATA_CONTROL_BLOCK;

typedef struct _firmware_update_control_block_sub
{
    uint32_t user_program_max_cnt;
    uint32_t lifecycle_state;
    uint8_t program_hash0[SHA1_HASH_LENGTH_BYTE_SIZE];
    uint8_t program_hash1[SHA1_HASH_LENGTH_BYTE_SIZE];
}FIRMWARE_UPDATE_CONTROL_BLOCK_SUB;

typedef struct _firmware_update_control_block
{
    FIRMWARE_UPDATE_CONTROL_BLOCK_SUB data;
    uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];
}FIRMWARE_UPDATE_CONTROL_BLOCK;

void main(void);
static int32_t secure_boot(void);
static int32_t firm_block_read(uint32_t *firmware, uint32_t offset);
static int32_t const_data_block_read(uint32_t *const_data, uint32_t offset);
static void bank_swap_with_software_reset(void);
static void update_dataflash_data_from_image(void);
static void update_dataflash_data_mirror_from_image(void);
static void check_dataflash_area(uint32_t retry_counter);
static void software_reset(void);
static void my_sci_callback(void *pArgs);

extern int32_t usb_main(void);
extern void lcd_open(void);
extern void lcd_close(void);

#define FIRMWARE_UPDATE_CONTROL_BLOCK_INITIAL_DATA \
        /* FIRMWARE_UPDATE_CONTROL_BLOCK_SUB data; */\
        {\
            /* uint32_t user_program_max_cnt; */\
            0,\
            /* uint32_t lifecycle_state; */\
            LIFECYCLE_STATE_BLANK,\
            /* uint8_t program_hash0[SHA1_HASH_LENGTH_BYTE_SIZE]; */\
            {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},\
            /* uint8_t program_hash1[SHA1_HASH_LENGTH_BYTE_SIZE]; */\
			{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},\
        },\
        /* uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE]; */\
        {0xc1, 0x7f, 0xd9, 0x26, 0x82, 0xca, 0x5b, 0x30, 0x4A, 0xc7, 0x10, 0x74, 0xb5, 0x58, 0xdd, 0xa9, 0xe8, 0xeb, 0x4d, 0x66},

#pragma section _FIRMWARE_UPDATE_CONTROL_BLOCK
static const FIRMWARE_UPDATE_CONTROL_BLOCK firmware_update_control_block_data = {FIRMWARE_UPDATE_CONTROL_BLOCK_INITIAL_DATA};
#pragma section

#pragma section _FIRMWARE_UPDATE_CONTROL_BLOCK_MIRROR
static const FIRMWARE_UPDATE_CONTROL_BLOCK firmware_update_control_block_data_mirror = {FIRMWARE_UPDATE_CONTROL_BLOCK_INITIAL_DATA};
#pragma section

static FIRMWARE_UPDATE_CONTROL_BLOCK firmware_update_control_block_image = {0};
static LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static LOAD_CONST_DATA_CONTROL_BLOCK load_const_data_control_block;
static FATFS       g_fatfs;

/* Handle storage. */
sci_hdl_t     my_sci_handle;

const char lifecycle_string[][64] = {
        {"LIFECYCLE_STATE_BLANK"},
        {"LIFECYCLE_STATE_ON_THE_MARKET"},
        {"LIFECYCLE_STATE_UPDATING"},
};

void main(void)
{
    int32_t result_secure_boot;
    static FILINFO filinfo;
    usb_ctrl_t  ctrl;
    usb_cfg_t   cfg;
    uint16_t    event;
    uint16_t    previous_event;
    flash_err_t flash_error_code = FLASH_SUCCESS;
    uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];

    R_USB_PinSet_USB0_HOST();
    R_SCI_PinSet_SCI8();

    sci_cfg_t   my_sci_config;
    sci_err_t   my_sci_err;

    /* Set up the configuration data structure for asynchronous (UART) operation. */
    my_sci_config.async.baud_rate    = 115200;
    my_sci_config.async.clk_src      = SCI_CLK_INT;
    my_sci_config.async.data_size    = SCI_DATA_8BIT;
    my_sci_config.async.parity_en    = SCI_PARITY_OFF;
    my_sci_config.async.parity_type  = SCI_EVEN_PARITY;
    my_sci_config.async.stop_bits    = SCI_STOPBITS_1;
    my_sci_config.async.int_priority = 3;    // 1=lowest, 15=highest

    /* OPEN ASYNC CHANNEL
    *  Provide address of the configure structure,
    *  the callback function to be assigned,
    *  and the location for the handle to be stored.*/
    my_sci_err = R_SCI_Open(SCI_CH8, SCI_MODE_ASYNC, &my_sci_config, my_sci_callback, &my_sci_handle);

    /* If there were an error this would demonstrate error detection of API calls. */
    if (SCI_SUCCESS != my_sci_err)
    {
        nop(); // Your error handling code would go here.
    }

    load_firmware_control_block.progress = 0;
    load_firmware_control_block.offset = 0;

    flash_error_code = R_FLASH_Open();
    if (FLASH_SUCCESS == flash_error_code)
    {
        /* nothing to do */
    }
    else
    {
        printf("R_FLASH_Open() returns error. %d.\r\n", flash_error_code);
        printf("system error.\r\n");
        while(1);
    }

    /* startup system */
    printf("-------------------------------------------------\r\n");
    printf("RX65N secure boot program\r\n");
    printf("-------------------------------------------------\r\n");

    printf("Checking data flash...\r\n");
    check_dataflash_area(0);
    printf("OK\r\n");

    memset(&firmware_update_control_block_image, 0, sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK));
    memcpy(&firmware_update_control_block_image, &firmware_update_control_block_data, sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK));

    result_secure_boot = secure_boot();
    if (BOOT_LOADER_SUCCESS == result_secure_boot)
    {
    	/* stop all interrupt completely */
        set_psw(0);

    	uint32_t addr;
    	addr = *(uint32_t*)USER_RESET_VECTOR_ADDRESS;
    	((void (*)())addr)();
    	while(1); /* infinite loop */
    }
    else if (BOOT_LOADER_GOTO_INSTALL == result_secure_boot)
    {
        /* Set up the USB */
        ctrl.module     = USB_IP0;
        ctrl.type       = USB_HMSC;
        cfg.usb_speed   = USB_FS;
        cfg.usb_mode    = USB_HOST;
        R_USB_Open(&ctrl, &cfg);


        printf("========== install user program phase ==========\r\n");
        printf("insert USB memory includes \"%s\"\r\n", INITIAL_FIRMWARE_FILE_NAME);
        while(1)
        {
            event = R_USB_GetEvent(&ctrl);
            if(event != previous_event)
            {
                if(event == USB_STS_CONFIGURED)
                {
                    R_tfat_f_mount(0, &g_fatfs);
                    printf("usb memory attached.\r\n");
                    if(TFAT_FR_OK == R_tfat_f_stat(INITIAL_FIRMWARE_FILE_NAME, &filinfo))
                    {
                        printf("Detected file. %s.\r\n", INITIAL_FIRMWARE_FILE_NAME);

                        printf("erase install area (data flash): ");
                        flash_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
                        if (FLASH_SUCCESS == flash_error_code)
                        {
                            printf("OK\r\n");
                        }
                        else
                        {
                            printf("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
                            printf("system error.\r\n");
                            while(1);
                        }

                        printf("erase install area (code flash): ");
                        flash_error_code = R_FLASH_Erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
                        if (FLASH_SUCCESS == flash_error_code)
                        {
                            printf("OK\r\n");
                        }
                        else
                        {
                            printf("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
                            printf("system error.\r\n");
                            while(1);
                        }

                        while(1)
                        {
                            if(!const_data_block_read(load_const_data_control_block.flash_buffer, load_const_data_control_block.offset))
                            {
                            	flash_error_code = R_FLASH_Write((uint32_t)load_const_data_control_block.flash_buffer, (uint32_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS + load_const_data_control_block.offset, sizeof(load_const_data_control_block.flash_buffer));
                                if (FLASH_SUCCESS != flash_error_code)
                                {
                                    printf("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
                                    printf("system error.\r\n");
                                    while(1);
                                }
                                load_const_data_control_block.offset += FLASH_DF_BLOCK_SIZE;
                                load_const_data_control_block.progress = (uint32_t)(((float)(load_const_data_control_block.offset)/(float)((FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))*100));
                                static uint32_t previous_offset = 0;
                                if(previous_offset != (load_const_data_control_block.offset/1024))
                                {
                                    printf("installing const data...%d%(%d/%dKB).\r", load_const_data_control_block.progress, load_const_data_control_block.offset/1024, (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER)/1024);
                                    previous_offset = load_const_data_control_block.offset/1024;
                                }
                                if(load_const_data_control_block.offset < (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))
                                {
                                    /* one more loop */
                                }
                                else if(load_const_data_control_block.offset == (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))
                                {
                                    printf("\n");
                                    printf("completed installing const data.\r\n");
                                    break;
                                }
                                else
                                {
                                    printf("\n");
                                    printf("fatal error occurred.\r\n");
                                    break;
                                }
                            }
                            else
                            {
                                printf("\n");
                                printf("filesystem output error.\r\n");
                                break;
                            }
                        }

                        while(1)
                        {
                            if(!firm_block_read(load_firmware_control_block.flash_buffer, load_firmware_control_block.offset))
                            {
                                R_FLASH_Write((uint32_t)load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
                                load_firmware_control_block.offset += FLASH_CF_MEDIUM_BLOCK_SIZE;
                                load_firmware_control_block.progress = (uint32_t)(((float)(load_firmware_control_block.offset)/(float)((FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))*100));
                                printf("installing firmware...%d%(%d/%dKB).\r", load_firmware_control_block.progress, load_firmware_control_block.offset/1024, (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER)/1024);
                                if(load_firmware_control_block.offset < (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
                                {
                                    /* one more loop */
                                }
                                else if(load_firmware_control_block.offset == (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
                                {
                                    printf("\n");
                                    printf("completed installing firmware.\r\n");
                                    break;
                                }
                                else
                                {
                                    printf("\n");
                                    printf("fatal error occurred.\r\n");
                                    break;
                                }
                            }
                            else
                            {
                                printf("\n");
                                printf("filesystem output error.\r\n");
                                break;
                            }
                        }
                        printf("code flash hash check...");
                        
                        R_Sha1((uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, hash_sha1, FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                        if(0 == memcmp(hash_sha1, load_firmware_control_block.hash_sha1, SHA1_HASH_LENGTH_BYTE_SIZE))
                        {
                            printf("OK\r\n");

                            memcpy(firmware_update_control_block_image.data.program_hash1, hash_sha1, sizeof(hash_sha1));
                            firmware_update_control_block_image.data.lifecycle_state = LIFECYCLE_STATE_UPDATING;
                            firmware_update_control_block_image.data.user_program_max_cnt = load_firmware_control_block.firmware_length;

                            R_Sha1((uint8_t *)&firmware_update_control_block_image.data, hash_sha1, sizeof(firmware_update_control_block_image.data));
                            memcpy(firmware_update_control_block_image.hash_sha1, hash_sha1, sizeof(firmware_update_control_block_image.hash_sha1));
                            update_dataflash_data_from_image();
                            update_dataflash_data_mirror_from_image();

                            printf("swap bank...\r\n");
                            R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
                            R_USB_Close(&ctrl);
                            bank_swap_with_software_reset();
                            while(1);
                        }
                        else
                        {
                            printf("NG\r\n");
                            while(1);
                        }
                    }
                    else
                    {
                        printf("File Not Found. %s.\r\n", INITIAL_FIRMWARE_FILE_NAME);
                    }
                }
                else if(event == USB_STS_DETACH)
                {
                    printf("usb memory detached.\r\n");
                }
            }
            previous_event = event;
        }
    }
    else if (BOOT_LOADER_FAIL == result_secure_boot)
    {
        printf("secure boot sequence:");
        printf("fail.");
        while(1)
        {
            /* infinity loop */
        }
    }
    else
    {
        printf("unknown status.\n");
        while(1)
        {
            /* infinite loop */
        }
    }
}

static int32_t secure_boot(void)
{
    flash_err_t flash_error_code = FLASH_SUCCESS;
    int32_t secure_boot_error_code;
    uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];
    uint32_t bank_info = 255;

    printf("Checking flash ROM status.\r\n");

    printf("status = %s\r\n", lifecycle_string[firmware_update_control_block_image.data.lifecycle_state]);

    R_FLASH_Control(FLASH_CMD_BANK_GET, &bank_info);
    printf("bank info = %d. (start bank = %d)\r\n", bank_info, (bank_info ^ 0x01));

    switch(firmware_update_control_block_image.data.lifecycle_state)
    {
        case LIFECYCLE_STATE_BLANK:
            printf("start installing user program.\r\n");
            printf("erase bank1 secure boot mirror area...");
            flash_error_code = R_FLASH_Erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
            if(FLASH_SUCCESS == flash_error_code)
            {
                printf("OK\r\n");
            }
            else
            {
                printf("NG\r\n");
                printf("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
                secure_boot_error_code = BOOT_LOADER_FAIL;
                break;
            }

            printf("copy secure boot from bank0 to bank1...");
            if(BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM > 0)
            {
                flash_error_code = R_FLASH_Write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, 8 * FLASH_CF_SMALL_BLOCK_SIZE);
                flash_error_code = R_FLASH_Write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE);
            }
            else
            {
                flash_error_code = R_FLASH_Write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL * FLASH_CF_SMALL_BLOCK_SIZE);

            }
            if(FLASH_SUCCESS == flash_error_code)
            {
                printf("OK\r\n");
            }
            else
            {
                printf("NG\r\n");
                printf("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
                secure_boot_error_code = BOOT_LOADER_FAIL;
                break;
            }
            secure_boot_error_code = BOOT_LOADER_GOTO_INSTALL;
            break;
        case LIFECYCLE_STATE_UPDATING:
            printf("update data flash\r\n");
            memcpy(hash_sha1, firmware_update_control_block_image.data.program_hash1, sizeof(hash_sha1));
            memcpy(firmware_update_control_block_image.data.program_hash1, firmware_update_control_block_image.data.program_hash0, sizeof(hash_sha1));
            memcpy(firmware_update_control_block_image.data.program_hash0, hash_sha1, sizeof(hash_sha1));
            firmware_update_control_block_image.data.lifecycle_state = LIFECYCLE_STATE_ON_THE_MARKET;

            R_Sha1((uint8_t *)&firmware_update_control_block_image.data, hash_sha1, sizeof(firmware_update_control_block_data.data));
            memcpy(firmware_update_control_block_image.hash_sha1, hash_sha1, sizeof(hash_sha1));
            update_dataflash_data_from_image();
            update_dataflash_data_mirror_from_image();
            //break;    /* in this case, next state "LIFECYCLE_STATE_ON_THE_MARKET" is needed to execute */
        case LIFECYCLE_STATE_ON_THE_MARKET:
            check_dataflash_area(0);
            printf("code flash hash check...");
            R_Sha1((uint8_t*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, hash_sha1, FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
            if(!memcmp(firmware_update_control_block_data.data.program_hash0, hash_sha1, sizeof(hash_sha1)) || !memcmp(firmware_update_control_block_data.data.program_hash1, hash_sha1, sizeof(hash_sha1)))
            {
                printf("OK\r\n");
                printf("jump to user program\r\n");
                R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
                secure_boot_error_code = BOOT_LOADER_SUCCESS;
            }
            else
            {
                R_Sha1((uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, hash_sha1, FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER);
                if(!memcmp(firmware_update_control_block_data.data.program_hash0, hash_sha1, sizeof(hash_sha1)) || !memcmp(firmware_update_control_block_data.data.program_hash1, hash_sha1, sizeof(hash_sha1)) )
                {
                    printf("NG.\r\n");
                    printf("But other bank %d is still alive.\r\n", (bank_info ^ 0x01) ^ 0x01);
                    printf("swap bank...");
                    R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
                    bank_swap_with_software_reset();
                    while(1);
                }
                else
                {
                    printf("NG.\r\n");
                    printf("Code flash is completely broken.\r\n");
                    printf("Please erase all code flash.\r\n");
                    printf("And, write secure boot using debugger.\r\n");
                    secure_boot_error_code = BOOT_LOADER_FAIL;
                }
            }
            break;
        default:
            printf("illegal flash rom status code 0x%x.\r\n", firmware_update_control_block_image.data.lifecycle_state);
            check_dataflash_area(0);
            R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
            software_reset();
            while(1);
    }
    return secure_boot_error_code;
}

static void update_dataflash_data_from_image(void)
{
    uint32_t required_dataflash_block_num;
    flash_err_t flash_error_code = FLASH_SUCCESS;

    required_dataflash_block_num = sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK) / FLASH_DF_BLOCK_SIZE;
    if(sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK) % FLASH_DF_BLOCK_SIZE)
    {
        required_dataflash_block_num++;
    }

    printf("erase dataflash(main)...");
    flash_error_code = R_FLASH_Erase((flash_block_address_t)&firmware_update_control_block_data, required_dataflash_block_num);
    if(FLASH_SUCCESS == flash_error_code)
    {
        printf("OK\r\n");
    }
    else
    {
        printf("NG\r\n");
        printf("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
    }

    printf("write dataflash(main)...");
    flash_error_code = R_FLASH_Write((flash_block_address_t)&firmware_update_control_block_image, (flash_block_address_t)&firmware_update_control_block_data, FLASH_DF_BLOCK_SIZE * required_dataflash_block_num);
    if(FLASH_SUCCESS == flash_error_code)
    {
        printf("OK\r\n");
    }
    else
    {
        printf("NG\r\n");
        printf("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
        return;
    }
    return;
}

static void update_dataflash_data_mirror_from_image(void)
{
    uint32_t required_dataflash_block_num;
    flash_err_t flash_error_code = FLASH_SUCCESS;

    required_dataflash_block_num = sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK) / FLASH_DF_BLOCK_SIZE;
    if(sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK) % FLASH_DF_BLOCK_SIZE)
    {
        required_dataflash_block_num++;
    }

    printf("erase dataflash(mirror)...");
    flash_error_code = R_FLASH_Erase((flash_block_address_t)&firmware_update_control_block_data_mirror, required_dataflash_block_num);
    if(FLASH_SUCCESS == flash_error_code)
    {
        printf("OK\r\n");
    }
    else
    {
        printf("NG\r\n");
        printf("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
        return;
    }

    printf("write dataflash(mirror)...");
    flash_error_code = R_FLASH_Write((flash_block_address_t)&firmware_update_control_block_image, (flash_block_address_t)&firmware_update_control_block_data_mirror, FLASH_DF_BLOCK_SIZE * required_dataflash_block_num);
    if(FLASH_SUCCESS == flash_error_code)
    {
        printf("OK\r\n");
    }
    else
    {
        printf("NG\r\n");
        printf("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
        return;
    }
    if(!memcmp(&firmware_update_control_block_data, &firmware_update_control_block_data_mirror, sizeof(FIRMWARE_UPDATE_CONTROL_BLOCK)))
    {
        printf("data flash setting OK.\r\n");
    }
    else
    {
        printf("data flash setting NG.\r\n");
        return;
    }
    return;
}

static void check_dataflash_area(uint32_t retry_counter)
{
    uint8_t hash_sha1[SHA1_HASH_LENGTH_BYTE_SIZE];

    if(retry_counter)
    {
        printf("recover retry count = %d.\r\n", retry_counter);
        if(retry_counter == MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT)
        {
            printf("retry over the limit.\r\n");
            while(1);
        }
    }
    printf("data flash(main) hash check...");
    R_Sha1((uint8_t *)&firmware_update_control_block_data.data, hash_sha1, sizeof(firmware_update_control_block_data.data));
    if(!memcmp(firmware_update_control_block_data.hash_sha1, hash_sha1, sizeof(hash_sha1)))
    {
        printf("OK\r\n");
        printf("data flash(mirror) hash check...");
        R_Sha1((uint8_t *)&firmware_update_control_block_data_mirror.data, hash_sha1, sizeof(firmware_update_control_block_data.data));
        if(!memcmp(firmware_update_control_block_data_mirror.hash_sha1, hash_sha1, sizeof(hash_sha1)))
        {
            printf("OK\r\n");
        }
        else
        {
            printf("NG\r\n");
            printf("recover mirror from main.\r\n");
            memcpy(&firmware_update_control_block_image, &firmware_update_control_block_data, sizeof(firmware_update_control_block_data));
            update_dataflash_data_mirror_from_image();
            check_dataflash_area(retry_counter+1);
        }
    }
    else
    {
        printf("NG\r\n");
        printf("data flash(mirror) hash check...");
        R_Sha1((uint8_t *)&firmware_update_control_block_data_mirror.data, hash_sha1, sizeof(firmware_update_control_block_data_mirror.data));
        if(!memcmp(firmware_update_control_block_data_mirror.hash_sha1, hash_sha1, sizeof(hash_sha1)))
        {
            printf("OK\r\n");
            printf("recover main from mirror.\r\n");
            memcpy(&firmware_update_control_block_image, &firmware_update_control_block_data_mirror, sizeof(firmware_update_control_block_data_mirror));
            update_dataflash_data_from_image();
            check_dataflash_area(retry_counter+1);
        }
        else
        {
            printf("NG\r\n");
            printf("Data flash is completely broken.\r\n");
            printf("Please erase all code flash.\r\n");
            printf("And, write secure boot using debugger.\r\n");
            while(1);
        }
    }
}

static void software_reset(void)
{
    R_BSP_InterruptsDisable();
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    SYSTEM.SWRR = 0xa501;
    while(1);   /* software reset */
}

static void bank_swap_with_software_reset(void)
{
    R_BSP_InterruptsDisable();
    R_FLASH_Control(FLASH_CMD_BANK_TOGGLE, NULL);
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    SYSTEM.SWRR = 0xa501;
    while(1);   /* software reset */
}


/***********************************************************************************************************************
* Function Name: firm_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static int32_t firm_block_read(uint32_t *firmware, uint32_t offset)
{
    FRESULT ret = TFAT_FR_OK;
    uint8_t buf[256] = {0};
    uint8_t arg1[256] = {0};
    uint8_t arg2[256] = {0};
    uint8_t arg3[256] = {0};
    uint16_t size = 0;
    FIL file = {0};
    uint32_t read_size = 0;
    static uint32_t upprogram[4 + 1] = {0};
    static uint32_t current_char_position = 0;
    static uint32_t current_file_position = 0;
    static uint32_t previous_file_position = 0;

    if (0 == offset)
    {
        current_char_position = 0;
        current_file_position = 0;
        previous_file_position = 0;
        memset(upprogram,0,sizeof(upprogram));
    }

    ret = R_tfat_f_open(&file, INITIAL_FIRMWARE_FILE_NAME, TFAT_FA_READ | TFAT_FA_OPEN_EXISTING);
    if (TFAT_RES_OK == ret)
    {
        current_char_position = 0;
        memset(buf, 0, sizeof(buf));

        R_tfat_f_lseek(&file, previous_file_position);
        if (TFAT_RES_OK == ret)
        {
            while(1)
            {
                ret = R_tfat_f_read(&file, &buf[current_char_position++], 1, &size);
                if (TFAT_RES_OK == ret)
                {
                    if (0 == size)
                    {
                        break;
                    }
                    else
                    {
                        previous_file_position += size;

                        /* received 1 line */
                        if(strstr((char*)buf, "\r\n"))
                        {
                            sscanf((char*)buf, "%256s %256s %256s", (char*)arg1, (char*)arg2, (char*)arg3);
                            if (!strcmp((char *)arg1, "sha1"))
                            {
                                base64_decode(arg2, (uint8_t *)load_firmware_control_block.hash_sha1, strlen((char *)arg2));
                            }
                            if (!strcmp((char *)arg1, "max_cnt"))
                            {
                                sscanf((char*) arg2, "%x", load_firmware_control_block.firmware_length);
                            }
                            if (!strcmp((char *)arg1, "upprogram"))
                            {
                                base64_decode(arg3, (uint8_t *)upprogram, strlen((char *)arg3));
                                memcpy(&firmware[current_file_position], upprogram, 16);
                                current_file_position += 4;
                                read_size += 16;
                            }
                            if((current_file_position * 4) == FLASH_CF_MEDIUM_BLOCK_SIZE)
                            {
                                current_file_position = 0;
                                break;
                            }
                            current_char_position = 0;
                            memset(buf, 0, sizeof(buf));
                        }
                    }
                }
                else
                {
                    goto firm_block_read_error;
                    break;
                }
            }
        }
        else
        {
            goto firm_block_read_error;
        }
        if (TFAT_RES_OK == ret)
        {
            ret = R_tfat_f_close(&file);
            if (TFAT_RES_OK != ret)
            {
                goto firm_block_read_error;
            }
        }
    }
    else
    {
firm_block_read_error:
        return -1;
    }
    return 0;
}

/***********************************************************************************************************************
* Function Name: const_data_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static int32_t const_data_block_read(uint32_t *const_data, uint32_t offset)
{
    FRESULT ret = TFAT_FR_OK;
    uint8_t buf[256] = {0};
    uint8_t arg1[256] = {0};
    uint8_t arg2[256] = {0};
    uint8_t arg3[256] = {0};
    uint16_t size = 0;
    FIL file = {0};
    uint32_t read_size = 0;
    static uint32_t upconst[4 + 1] = {0};
    static uint32_t current_char_position = 0;
    static uint32_t current_file_position = 0;
    static uint32_t previous_file_position = 0;

    if (0 == offset)
    {
        current_char_position = 0;
        current_file_position = 0;
        previous_file_position = 0;
        memset(upconst,0,sizeof(upconst));
    }

    ret = R_tfat_f_open(&file, INITIAL_FIRMWARE_FILE_NAME, TFAT_FA_READ | TFAT_FA_OPEN_EXISTING);
    if (TFAT_RES_OK == ret)
    {
        current_char_position = 0;
        memset(buf, 0, sizeof(buf));

        R_tfat_f_lseek(&file, previous_file_position);
        if (TFAT_RES_OK == ret)
        {
            while(1)
            {
                ret = R_tfat_f_read(&file, &buf[current_char_position++], 1, &size);
                if (TFAT_RES_OK == ret)
                {
                    if (0 == size)
                    {
                        break;
                    }
                    else
                    {
                        previous_file_position += size;

                        /* received 1 line */
                        if(strstr((char*)buf, "\r\n"))
                        {
                            sscanf((char*)buf, "%256s %256s %256s", (char*)arg1, (char*)arg2, (char*)arg3);
                            if (!strcmp((char *)arg1, "upconst"))
                            {
                                base64_decode(arg3, (uint8_t *)upconst, strlen((char *)arg3));
                                memcpy(&const_data[current_file_position], upconst, 16);
                                current_file_position += 4;
                                read_size += 16;
                            }
                            if((current_file_position * 4) == FLASH_DF_BLOCK_SIZE)
                            {
                                current_file_position = 0;
                                break;
                            }
                            current_char_position = 0;
                            memset(buf, 0, sizeof(buf));
                        }
                    }
                }
                else
                {
                    goto const_data_block_read_error;
                    break;
                }
            }
        }
        else
        {
            goto const_data_block_read_error;
        }
        if (TFAT_RES_OK == ret)
        {
            ret = R_tfat_f_close(&file);
            if (TFAT_RES_OK != ret)
            {
                goto const_data_block_read_error;
            }
        }
    }
    else
    {
const_data_block_read_error:
        return -1;
    }
    return 0;
}

/*****************************************************************************
* Function Name: my_sci_callback
* Description  : This is a template for an SCI Async Mode callback function.
* Arguments    : pArgs -
*                pointer to sci_cb_p_args_t structure cast to a void. Structure
*                contains event and associated data.
* Return Value : none
******************************************************************************/
static void my_sci_callback(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
        nop();
    }
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	nop();
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	nop();
    }
    else if (SCI_EVT_FRAMING_ERR == p_args->event)
    {
        /* From receiver framing error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	nop();
    }
    else if (SCI_EVT_PARITY_ERR == p_args->event)
    {
        /* From receiver parity error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	nop();
    }
    else
    {
        /* Do nothing */
    }

} /* End of function my_sci_callback() */

/***********************************************************************************************************************
 * Function Name: my_sw_charput_function
 * Description  : char data output API
 * Arguments    : data -
 *                    Send data with SCI
 * Return Value : none
 **********************************************************************************************************************/
void my_sw_charput_function(uint8_t data)
{
    uint32_t arg = 0;
    /* do not call printf()->charput in interrupt context */
    do
    {
        /* Casting void pointer is used for address. */
        R_SCI_Control(my_sci_handle, SCI_CMD_TX_Q_BYTES_FREE, (void*)&arg);
    }
    while (SCI_CFG_CH8_RX_BUFSIZ != arg);
    /* Casting uint8_t pointer is used for address. */
    R_SCI_Send(my_sci_handle, (uint8_t*)&data, 1);

    return;
}
/***********************************************************************************************************************
 End of function my_sw_charput_function
 **********************************************************************************************************************/

void my_sw_charget_function(void)
{

}
