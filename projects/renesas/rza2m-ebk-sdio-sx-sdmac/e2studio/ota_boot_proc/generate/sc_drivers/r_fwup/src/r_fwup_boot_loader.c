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
#include <stdbool.h>
#include "r_fwup_config.h"  /* Firmware update config definitions */
#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == 2) /* FWUP_IMPLEMENTATION_AFRTOS */
#include "aws_iot_ota_agent.h"
#endif
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#include "r_smc_entry.h"
#include "r_flash_rx_if.h"
#include "r_sci_rx_if.h"
#include "r_sys_time_rx_if.h"
#include "r_fwup_if.h"
#include "r_fwup_private.h"
#if (FWUP_CFG_USE_EXMEM != 0)
#include "r_flash_spi_if.h"
#endif /* (FWUP_CFG_USE_EXMEM != 0) */
#elif (MCU_SERIES_RZA2)
#include "r_typedefs.h"
#include "r_os_abstraction_api.h"
#include "r_cpg_drv_api.h"
#include "r_ostm_drv_api.h"
#include "r_gpio_drv_api.h"
#include "r_devlink_wrapper.h"
#include "resetprg.h"
#include "command.h"
#include "r_os_abstraction_api.h"
#include "r_compiler_abstraction_api.h"
#include "version.h"
#if (FWUP_CFG_USE_EXMEM != 0)
#include "flash_api.h"
#endif /* (FWUP_CFG_USE_EXMEM != 0) */
#include "r_scifa_drv_api.h"
#include "r_fwup_if.h"
#include "r_fwup_private.h"
#endif /* (MCU_SERIES_RZA2) */

#if (FWUP_CFG_IMPLEMENTATION_ENVIRONMENT == FWUP_IMPLEMENTATION_BOOTLOADER)
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#include "r_sci_rx_pinset.h"

#include "base64_decode.h"
#include "code_signer_public_key.h"

/* tinycrypt */
#include "tinycrypt/sha256.h"
#include "tinycrypt/ecc.h"
#include "tinycrypt/ecc_dsa.h"
#include "tinycrypt/constants.h"
#elif (MCU_SERIES_RZA2)
#include "base64_decode.h"
#include "code_signer_public_key.h"

/* tinycrypto */
#include "sha256.h"

#include "r_cache_lld_rza2m.h"
#include "r_cache_l1_rza2m_asm.h"
#include "ecc_dsa.h"
#include "r_mmu_lld_rza2m.h"
#else
    /* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */

#define BOOT_LOADER_SCI_CONTROL_BLOCK_A (0)
#define BOOT_LOADER_SCI_CONTROL_BLOCK_B (1)
#define BOOT_LOADER_SCI_CONTROL_BLOCK_TOTAL_NUM (2)

#define BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY (0)
#define BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL  (1)

#define MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT 3
#define SHA1_HASH_LENGTH_BYTE_SIZE 20

#define FLASH_DF_TOTAL_BLOCK_SIZE (FLASH_DF_BLOCK_INVALID - FLASH_DF_BLOCK_0)

#define INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE "hash-sha256"
#define INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE "sig-sha256-ecdsa"

#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
#if !defined(MY_BSP_CFG_SERIAL_TERM_SCI)
#error "Error! Need to define MY_BSP_CFG_SERIAL_TERM_SCI in r_bsp_config.h"
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (0)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI0()
#define SCI_CH_serial_term          SCI_CH0
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (1)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI1()
#define SCI_CH_serial_term          SCI_CH1
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (2)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI2()
#define SCI_CH_serial_term          SCI_CH2
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (3)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI3()
#define SCI_CH_serial_term          SCI_CH3
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (4)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI4()
#define SCI_CH_serial_term          SCI_CH4
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (5)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI5()
#define SCI_CH_serial_term          SCI_CH5
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (6)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI6()
#define SCI_CH_serial_term          SCI_CH6
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (7)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI7()
#define SCI_CH_serial_term          SCI_CH7
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (8)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI8()
#define SCI_CH_serial_term          SCI_CH8
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (9)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI9()
#define SCI_CH_serial_term          SCI_CH9
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (10)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI10()
#define SCI_CH_serial_term          SCI_CH10
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (11)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI11()
#define SCI_CH_serial_term          SCI_CH11
#elif MY_BSP_CFG_SERIAL_TERM_SCI == (12)
#define R_SCI_PinSet_serial_term()  R_SCI_PinSet_SCI12()
#define SCI_CH_serial_term          SCI_CH12
#else
#error "Error! Invalid setting for MY_BSP_CFG_SERIAL_TERM_SCI in r_bsp_config.h"
#endif /* (MY_BSP_CFG_SERIAL_TERM_SCI) */
#endif /* (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200) */

typedef struct _load_const_data_control_block {
	uint32_t flash_buffer[FWUP_WRITE_BLOCK_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
}LOAD_CONST_DATA_CONTROL_BLOCK;

void my_sci_callback(void *pArgs);

// static int32_t secure_boot(void);
static void software_reset(void);
static void bank_swap_with_software_reset(void);
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
static int32_t firm_block_read(uint32_t *firmware, uint32_t offset);
static int32_t const_data_block_read(uint32_t *const_data, uint32_t offset);
#elif (MCU_SERIES_RZA2)
static comm_err_t fwup_download_fw_to_exmem(void);
static void fwup_copy_fw_to_exe_area(void);
#endif /* (MCU_SERIES_RZA2) */
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
static void my_flash_callback(void *event);
#endif /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
static const uint8_t *get_status_string(uint8_t status);

extern iflash_err_t fwup_flash_open(void);
extern iflash_err_t fwup_flash_close(void);
extern iflash_err_t fwup_flash_set_callback(void *);
extern iflash_err_t fwup_flash_get_bank_info(uint32_t *);
extern void fwup_flash_set_bank_toggle(void);
extern iflash_err_t fwup_flash_erase(uint32_t, uint32_t);
extern iflash_err_t fwup_flash_write(uint32_t, uint32_t, uint32_t);
extern comm_err_t fwup_communication_open(void);
extern comm_err_t fwup_communication_close(void);
extern comm_err_t fwup_communication_receive(uint8_t *, uint16_t const);
extern void fwup_update_status(fwup_state_t state);
extern fwup_state_t fwup_get_status(void);
extern state_monitoring_err_t fwup_state_monitoring_open(void);
extern state_monitoring_err_t fwup_state_monitoring_start(void);
extern state_monitoring_err_t fwup_state_monitoring_close(void);
extern state_monitoring_flag_t fwup_state_monitoring_is_error(void);
extern void fwup_interrupts_disable(void);
extern void fwup_interrupts_enable(void);
extern void fwup_register_protect_disable(void);
extern bool fwup_get_boot_protect(void);
extern void fwup_set_boot_protect(void);
#if (FWUP_CFG_USE_EXMEM != 0)
extern exmem_err_t fwup_flash_spi_open(void);
extern exmem_err_t fwup_flash_spi_close(void);
extern exmem_err_t fwup_flash_spi_erase(uint32_t, uint32_t);
extern exmem_err_t fwup_flash_spi_write(uint8_t *, uint32_t, uint32_t);
extern exmem_err_t fwup_flash_spi_read(uint32_t, uint32_t, uint8_t *);
extern exmem_err_t fwup_flash_spi_polling_erase(void);
extern exmem_err_t fwup_flash_spi_polling_write(void);
#endif /* (FWUP_CFG_USE_EXMEM != 0) */
extern void my_sw_charget_function(void);
extern void my_sw_charput_function(uint8_t data);

static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank0 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;
static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank1 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;
static LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static LOAD_CONST_DATA_CONTROL_BLOCK load_const_data_control_block;
static uint32_t flash_error_code;
static bool boot_protect_flag = false;
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
static uint32_t block_buffer[FLASH_CF_MEDIUM_BLOCK_SIZE / 4];
#elif (MCU_SERIES_RZA2)
static uint8_t hyper[0x800000] __attribute((section("HYPER_RAM")));
static uint32_t block_buffer[SF_SECTOR_SIZE / 4];
static uint32_t downloaded_image_size = BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH + BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH;
static uint32_t image_size = BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH;
#else
/* Fix me for other MCU series */
#endif

/* Handle storage. */
// sci_hdl_t     my_sci_handle;
SCI_RECEIVE_CONTROL_BLOCK sci_receive_control_block;
SCI_BUFFER_CONTROL sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_TOTAL_NUM];
static const uint8_t *pboot_loader_magic_code = (uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;

extern int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize);
const uint8_t code_signer_public_key[] = CODE_SIGNER_PUBLIC_KEY_PEM;
const uint32_t code_signer_public_key_length = sizeof(code_signer_public_key);

int32_t R_FWUP_SecureBoot(void)
{
    iflash_err_t flash_api_error_code = IFLASH_SUCCESS;
    int32_t secure_boot_error_code = FWUP_IN_PROGRESS;
    state_monitoring_err_t monitoring_error_code = MONI_SUCCESS;
    comm_err_t   my_comm_err;
#if (FWUP_CFG_USE_EXMEM != 0)
    exmem_err_t exmem_error_code = EXMEM_SUCCESS;
#endif /* (FWUP_CFG_USE_EXMEM != 0) */
#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0)  /* Dual bank mode */
    uint32_t bank_info = 255;
#endif  /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) */
	FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_tmp = (FIRMWARE_UPDATE_CONTROL_BLOCK*)block_buffer;
	int32_t verification_result = -1;

    /* If state transit error has occurred, return FWUP_FAIL to main function */
    if (STATE_MONITORING_ERROR == fwup_state_monitoring_is_error())
    {
        DEBUG_LOG("Boot status has not changed for 1 mitute.\r\n");
        DEBUG_LOG("system error.\r\n");
		fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	secure_boot_error_code = FWUP_FAIL;
    }

    switch(fwup_get_status())
    {
    	case FWUP_STATE_INITIALIZING:
#if (MCU_SERIES_RZA2)
    	    if (!R_OS_AbstractionLayerInit())
    	    {
    	        /* stop execution */
    	        while (true)
    	        {
    	            /* Spin here forever.. */
    	            R_COMPILER_Nop();
    	        }
    	    }
#else
		    /* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */

    	    /* OPEN ASYNC CHANNEL
    	    *  Provide address of the configure structure,
    	    *  the callback function to be assigned,
    	    *  and the location for the handle to be stored.*/
    	    my_comm_err = fwup_communication_open();

    	    /* If there were an error this would demonstrate error detection of API calls. */
    	    if (COMM_SUCCESS != my_comm_err)
    	    {
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
				break;
    	    }

    	    load_firmware_control_block.progress = 0;
    	    load_firmware_control_block.offset = 0;

    	    flash_api_error_code = fwup_flash_open();
    	    if (IFLASH_SUCCESS != flash_api_error_code)
    	    {
    	        DEBUG_LOG2("R_FLASH_Open() returns error. %d.\r\n", flash_api_error_code);
    	        DEBUG_LOG("system error.\r\n");
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
    	    }

#if (FWUP_CFG_USE_EXMEM != 0)
    	    exmem_error_code = fwup_flash_spi_open();
    	    if (EXMEM_SUCCESS != exmem_error_code)
    	    {
    	        DEBUG_LOG2("R_FLASH_SPI_Open() returns error. %d.\r\n", exmem_error_code);
    	        DEBUG_LOG("system error.\r\n");
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
    	    }
#endif /* (FWUP_CFG_USE_EXMEM != 0) */

    	    /* Set up the configuration of System-timer for check the status transition. */
    	    monitoring_error_code = fwup_state_monitoring_open();
    	    if (MONI_SUCCESS != monitoring_error_code)
    	    {
    	        DEBUG_LOG2("R_SYS_TIME_Open() returns error. %d.\r\n", monitoring_error_code);
    	        DEBUG_LOG("system error.\r\n");
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
    	    }

    	    /* startup system */
    	    DEBUG_LOG("-------------------------------------------------\r\n");
    	    DEBUG_LOG("secure boot program\r\n");
    	    DEBUG_LOG("-------------------------------------------------\r\n");

    	    DEBUG_LOG("Checking flash ROM status.\r\n");

    	    DEBUG_LOG2("bank 0 status = 0x%x [%s]\r\n", firmware_update_control_block_bank0->image_flag, get_status_string(firmware_update_control_block_bank0->image_flag));
    	    DEBUG_LOG2("bank 1 status = 0x%x [%s]\r\n", firmware_update_control_block_bank1->image_flag, get_status_string(firmware_update_control_block_bank1->image_flag));

#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0)  /* Dual bank mode */
    	    fwup_flash_get_bank_info(&bank_info);
    	    DEBUG_LOG2("bank info = %d. (start bank = %d)\r\n", bank_info, (bank_info ^ 0x01));
#endif  /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) */

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
    		fwup_flash_set_callback((void *)&my_flash_callback);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */

    	    /* Set System-timer for check status */
    	    monitoring_error_code = fwup_state_monitoring_start();
    	    if (MONI_SUCCESS != monitoring_error_code)
    	    {
    	        DEBUG_LOG2("R_SYS_TIME_RegisterPeriodicCallback() returns error. %d.\r\n", monitoring_error_code);
    	        DEBUG_LOG("system error.\r\n");
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
    	    }
    	    fwup_update_status(FWUP_STATE_BANK1_CHECK);
    		break;

    	case FWUP_STATE_BANK1_CHECK:
    		if(firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_TESTING)
    		{
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
    	    	memcpy(block_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
#elif (MCU_SERIES_RZA2)
				memcpy(block_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, SF_SECTOR_SIZE);

				// RZ/A2M OTA 2020.03.19 // -->>
				image_size = firmware_update_control_block_bank1->image_size;
				if(image_size > BOOT_LOADER_TOTAL_UPDATE_SIZE)
				{
					image_size = BOOT_LOADER_TOTAL_UPDATE_SIZE;
				}
				// RZ/A2M OTA 2020.03.19 // <<--
#else
			    /* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */

    	    	DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
    	    	DEBUG_LOG("bank1(temporary area) on code flash integrity check...");

    	    	/* Firmware verification for the signature type. */
				if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
				{
					uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
				    /* Hash message */
				    struct tc_sha256_state_struct xCtx;
				    tc_sha256_init(&xCtx);
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
				    tc_sha256_update(&xCtx,
				    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
							(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
#elif (MCU_SERIES_RZA2)
				    tc_sha256_update(&xCtx,
				    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
//							APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);	// RZ/A2M OTA 2020.03.19 //
							image_size);	// RZ/A2M OTA 2020.03.19 //
#else
			    /* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */
				    tc_sha256_final(hash_sha256, &xCtx);
	    	        verification_result = memcmp(firmware_update_control_block_bank1->signature, hash_sha256, sizeof(hash_sha256));
	    	    }
	    	    else if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
	    	    {
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
					verification_result = firmware_verification_sha256_ecdsa(
														(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
														(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
														firmware_update_control_block_bank1->signature,
														firmware_update_control_block_bank1->signature_size);
#elif (MCU_SERIES_RZA2)
					verification_result = firmware_verification_sha256_ecdsa(
														(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
//														APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,	// RZ/A2M OTA 2020.03.19 //
														image_size,	// RZ/A2M OTA 2020.03.19 //
														firmware_update_control_block_bank1->signature,
														firmware_update_control_block_bank1->signature_size);
#else
			    /* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */
				}
				else
				{
					verification_result = -1;
				}

    	        if(0 == verification_result)
    	        {
    	            DEBUG_LOG("OK\r\n");
    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_VALID;
    	        }
    	        else
    	        {
    	            DEBUG_LOG("NG\r\n");
    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_INVALID;
    	        }
    	    	DEBUG_LOG2("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
    	    	DEBUG_LOG("bank1(temporary area) block0 erase (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
    			/* Erase 1 block of Temporary area of Code flash (BGO) */
    	    	flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
    	        if (FLASH_SUCCESS != flash_api_error_code)
    	        {
    	            DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
    	            DEBUG_LOG("system error.\r\n");
					fwup_update_status(FWUP_STATE_FATAL_ERROR);
					secure_boot_error_code = FWUP_FAIL;
    	            break;
    	        }
    			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT);
#else
#if (FWUP_ENV_USE_EXMEM_TEMPORARY == 1)
    			/* Erase 1 sector of Temporary area of External memory */
    			fwup_interrupts_disable();
    			exmem_error_code = fwup_flash_spi_erase(BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, SF_SECTOR_SIZE);
    			fwup_interrupts_enable();
    	        if (EXMEM_SUCCESS != exmem_error_code)
    	        {
    	            DEBUG_LOG2("R_FLASH_SPI_Erase() returns error. %d.\r\n", exmem_error_code);
    	            DEBUG_LOG("system error.\r\n");
					fwup_update_status(FWUP_STATE_FATAL_ERROR);
					secure_boot_error_code = FWUP_FAIL;
    	            break;
    	        }
    	        flash_error_code = IFLASH_SUCCESS;
    			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE);
#else /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 1) */
    			/* Erase 1 block of Temporary area of Code flash (blocking) */
    			fwup_interrupts_disable();
    	    	flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, 1);
    	        fwup_interrupts_enable();
    	        if (FLASH_SUCCESS != flash_api_error_code)
    	        {
    	            DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
    	            DEBUG_LOG("system error.\r\n");
					fwup_update_status(FWUP_STATE_FATAL_ERROR);
					secure_boot_error_code = FWUP_FAIL;
    	            break;
    	        }
    	        flash_error_code = IFLASH_SUCCESS;
    			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE);
#endif /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 1) */
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
    		}
#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) /* Linear mode */
    		//********************************************************************************************************
    		// CAUTION : 実装未完
    		//   処理フロー中の、非デュアルバンク動作の処理は実装未完。
    		//   ・(FWUP_ENV_CODE_FLASH_BANK_MODE == 1) で区切った箇所が、非デュアルバンク動作実装箇所
    		//********************************************************************************************************
    		else if(firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_VALID)
    		{
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
    	    	memcpy(block_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
#elif (MCU_SERIES_RZA2)
				memcpy(block_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, SF_SECTOR_SIZE);

				// RZ/A2M OTA 2020.03.19 // -->>
				image_size = firmware_update_control_block_bank1->image_size;
				if(image_size > BOOT_LOADER_TOTAL_UPDATE_SIZE)
				{
					image_size = BOOT_LOADER_TOTAL_UPDATE_SIZE;
				}
				// RZ/A2M OTA 2020.03.19 // <<--
#else
			    /* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */

    	    	DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
    	    	DEBUG_LOG("temporary area on code flash integrity check...");

    	    	/* Firmware verification for the signature type. */
				if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
				{
					uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
				    /* Hash message */
				    struct tc_sha256_state_struct xCtx;
				    tc_sha256_init(&xCtx);
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
				    tc_sha256_update(&xCtx,
				    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
							(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
#elif (MCU_SERIES_RZA2)
				    tc_sha256_update(&xCtx,
				    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
//							APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);	// RZ/A2M OTA 2020.03.19 //
							image_size);	// RZ/A2M OTA 2020.03.19 //
#endif /* (MCU_SERIES_RZA2) */
				    tc_sha256_final(hash_sha256, &xCtx);
	    	        verification_result = memcmp(firmware_update_control_block_bank1->signature, hash_sha256, sizeof(hash_sha256));
	    	    }
	    	    else if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
	    	    {
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
					verification_result = firmware_verification_sha256_ecdsa(
														(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
														(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
														firmware_update_control_block_bank1->signature,
														firmware_update_control_block_bank1->signature_size);
#elif (MCU_SERIES_RZA2)
					verification_result = firmware_verification_sha256_ecdsa(
														(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
//														APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,	// RZ/A2M OTA 2020.03.19 //
														image_size,	// RZ/A2M OTA 2020.03.19 //
														firmware_update_control_block_bank1->signature,
														firmware_update_control_block_bank1->signature_size);
#endif /* (MCU_SERIES_RZA2) */
				}
				else
				{
					verification_result = -1;
				}

    	        if(0 == verification_result)
    	        {
    	            DEBUG_LOG("OK\r\n");
    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_VALID;
    	        }
    	        else
    	        {
    	        	/*
    	        	 * VALID更新失敗。Temporaryを消去しリセット
					 * Executeエリア={BLANK or VALID}, Temporaryエリア=BLANK となる
					 */
    	            DEBUG_LOG("NG\r\n");
    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_INVALID;
    	        }

    	        /* 非デュアルバンク動作時の実装箇所						*/
    	        /* - Temporaryエリア -> Executeエリア へFWをコピーする		*/
    	        /*   (Temporaryエリア -> HyperRAM -> Executeエリア)	*/
    	        /* - Executeエリアを署名検証する（コピー結果の確認）		*/
    	        /* - Temporaryエリアの消去 (image_flag = EOL)		*/
    	        /* - Executeエリアのプログラムに制御を移す				*/

    			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE);
    		}
    		else if(firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_EOL)
    		{
    	        /* 非デュアルバンク動作時のEOL処理実装箇所				*/
    			/* Case of EOL process */
    		}
#endif /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) */
    		else
    		{
				if (firmware_update_control_block_bank0->image_flag == LIFECYCLE_STATE_VALID)
				{
					fwup_update_status(FWUP_STATE_BANK0_UPDATE_CHECK);
	    		}
	    		else
	    		{
	    			fwup_update_status(FWUP_STATE_BANK0_CHECK);
				}
    		}
			break;

    	case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT:
    		/* this state will be update by flash callback */
    		break;

    	case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE:
	        if (IFLASH_SUCCESS == flash_error_code)
	        {
	            DEBUG_LOG("OK\r\n");
	        }
	        else
	        {
	            DEBUG_LOG2("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
	            DEBUG_LOG("system error.\r\n");
	            fwup_update_status(FWUP_STATE_FATAL_ERROR);
	            break;
	        }
	        DEBUG_LOG("bank1(temporary area) block0 write (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
	        flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
			if (FLASH_SUCCESS != flash_api_error_code)
			{
				DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
				DEBUG_LOG("system error.\r\n");
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
				break;
			}
			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT);
#else
#if (FWUP_ENV_USE_EXMEM_TEMPORARY)
			/* Write 1 sector data to Temporary area of External memory */
			fwup_interrupts_disable();
			exmem_error_code = fwup_flash_spi_write((uint8_t *)firmware_update_control_block_tmp,BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,SF_SECTOR_SIZE);
	        fwup_interrupts_enable();
	        if (EXMEM_SUCCESS != exmem_error_code)
			{
				printf("flash_program_page() returns error.\r\n");
				printf("system error.\r\n");
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
				break;
			}
	        flash_error_code = IFLASH_SUCCESS;
			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE);
#else
			/* Write 1 block data to Temporary area of Code flash (blocking) */
	        fwup_interrupts_disable();
	        flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
	        fwup_interrupts_enable();
			if (FLASH_SUCCESS != flash_api_error_code)
			{
				DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
				DEBUG_LOG("system error.\r\n");
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
				break;
			}
			flash_error_code = IFLASH_SUCCESS;
			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE);
#endif /* (FWUP_ENV_USE_EXMEM_TEMPORARY) */
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
			break;

    	case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT:
    		/* this state will be update by flash callback */
    		break;

    	case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE:
	        if (IFLASH_SUCCESS == flash_error_code)
	        {
	            DEBUG_LOG("OK\r\n");
	        }
	        else
	        {
	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
	            DEBUG_LOG("system error.\r\n");
				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				secure_boot_error_code = FWUP_FAIL;
	            break;
	        }
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
        	if (LIFECYCLE_STATE_VALID == firmware_update_control_block_tmp->image_flag)
        	{
    	        DEBUG_LOG("swap bank...\r\n");
    			R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
    			bank_swap_with_software_reset();
    			while(1);
        	}
        	else
        	{
    	        DEBUG_LOG("software reset...\r\n");
    			R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
                software_reset();
    			while(1);
        	}
#elif (MCU_SERIES_RZA2)
			// RZ/A2M OTA 2020.03.19 // -->>
        	if (LIFECYCLE_STATE_VALID == firmware_update_control_block_tmp->image_flag)
        	{
    			// 更新アプリケーション(bank1)のフラグがVALID以外の時はデータ異常のため、
    			// RXではバンクスワップしてbank0(元のbank1)のフラグが異常、
    			// 再度スワップして元のbank0に戻るようになっている。
    			// RZ/A2Mではバンクスワップするとbank1のアプリケーションは削除されるため、
    			// 元のbank0に戻すような処理はできない。
    			// このため、bank1のフラグがVALID以外の場合はスワップせず、
    			// bank1 のフラグを 0xff にして無効にする処置をし、そのまま bank0 で動作するようにする。
        		DEBUG_LOG("copy from temporary area to execute area...");
	//			R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
				R_SoftwareDelay(7500000);	//3s wait
				/* ソフトの入れ替え処理で実施 */
				fwup_copy_fw_to_exe_area();
	            DEBUG_LOG("OK\r\n");
 				DEBUG_LOG("software reset...\r\n");
				software_reset();
			}
			else
			{
				DEBUG_LOG("illegal status\r\n");
				DEBUG_LOG("not copy to execute area...");

				image_size = firmware_update_control_block_bank1->image_size;
				if(image_size > BOOT_LOADER_TOTAL_UPDATE_SIZE)
				{
					image_size = BOOT_LOADER_TOTAL_UPDATE_SIZE;
				}
				// RZ/A2M OTA 2020.03.19 // <<--

				fwup_interrupts_disable();
    			exmem_error_code = fwup_flash_spi_erase(BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, image_size);
    	        fwup_interrupts_enable();
    	        if (EXMEM_SUCCESS != exmem_error_code)
    	        {
    	            DEBUG_LOG2("R_FLASH_SPI_Erase() returns error. %d.\r\n", exmem_error_code);
    	            DEBUG_LOG("system error.\r\n");
					fwup_update_status(FWUP_STATE_FATAL_ERROR);
					secure_boot_error_code = FWUP_FAIL;
    	            break;
    	        }

//   	        R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
				R_SoftwareDelay(7500000);	//3s wait
				software_reset();
        	}
			while(1);
			// RZ/A2M OTA 2020.03.19 //  <<--
#endif /* (MCU_SERIES_RZA2) */
			break;

    	case FWUP_STATE_BANK0_CHECK:
    	case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
    	case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE:
    	case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
    	case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1:
    	case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
    	case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2:
    	case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
    	case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE:
    	case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
    	case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE:
    	case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT:
    	case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_COMPLETE:
    	case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
    	case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
    	case FWUP_STATE_INSTALL_DATA_FLASH_READ_WAIT:
    	case FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE:
    	case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
    	case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE:
		case FWUP_STATE_BANK0_UPDATE_CHECK:
		case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
		case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE:
		case FWUP_STATE_EOL_BANK1_ERASE_WAIT:
		case FWUP_STATE_EOL_BANK1_ERASE_COMPLETE:
		case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT:
		case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE:
		case FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT:
		case FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE:
    	case FWUP_STATE_FINALIZE:
    	    switch(firmware_update_control_block_bank0->image_flag)
    	    {
    	        case LIFECYCLE_STATE_BLANK:
    	        	switch(fwup_get_status())
					{
    	        		case FWUP_STATE_BANK0_CHECK:
    	        			if(firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_BLANK)
    	        			{
								DEBUG_LOG("start installing user program.\r\n");
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
								boot_protect_flag = fwup_get_boot_protect();
#endif /* (FWUP_CFG_BOOT_PROTECT_ENABLE == 1) */
								if (false == boot_protect_flag)
								{
									/* No Boot Protected */
#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) /* Dual bank mode */
									DEBUG_LOG("erase bank1 secure boot mirror area...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
									flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
									if(IFLASH_SUCCESS != flash_api_error_code)
									{
										DEBUG_LOG("NG\r\n");
										DEBUG_LOG2("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
										fwup_update_status(FWUP_STATE_FATAL_ERROR);
										secure_boot_error_code = FWUP_FAIL;
										break;
									}
									fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
							        fwup_interrupts_disable();
									flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
							        fwup_interrupts_enable();
									if(IFLASH_SUCCESS != flash_api_error_code)
									{
										DEBUG_LOG("NG\r\n");
										DEBUG_LOG2("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
										fwup_update_status(FWUP_STATE_FATAL_ERROR);
										secure_boot_error_code = FWUP_FAIL;
										break;
									}
									flash_error_code = IFLASH_SUCCESS;
									fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO) */
#else /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) Linear mode */
									/* Do not FLASH erase because the Bootloader is not copied */
									/* Skip to next status */
									flash_error_code = IFLASH_SUCCESS;
									fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
#endif /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) Linear mode */
								}
								else
								{
									/* Already Boot Protected */
									/* Skip boot copy process */
									DEBUG_LOG("boot protected: skip copy secure boot from bank0 to bank1...");
									flash_error_code = IFLASH_SUCCESS;
									fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
								}
    	        			}
#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) /* Dual bank mode */
    	        			else if(firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_VALID)
    	        			{
    	        	            DEBUG_LOG("bank0(current) is blank, but bank1(previous) is still alive.\r\n");
    	        	            DEBUG_LOG("swap bank...");
    	        	            R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
    	        				bank_swap_with_software_reset();
    	        				while(1);
    	        			}
#endif /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) */
    	        			else
    	        			{
								DEBUG_LOG("user program installation failed.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
    	        			}
							break;

#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) /* Dual bank mode */
    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE:
							if (firmware_update_control_block_bank1->image_flag != LIFECYCLE_STATE_INSTALLING)
							{
	    	        	        if (IFLASH_SUCCESS == flash_error_code)
	    	        	        {
	    	        	            DEBUG_LOG("OK\r\n");
	    	        	        }
	    	        	        else
	    	        	        {
	    	        	            DEBUG_LOG2("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
	    	        	            DEBUG_LOG("system error.\r\n");
	    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
	    	        				secure_boot_error_code = FWUP_FAIL;
	    	        	            break;
	    	        	        }
	    	        	    }
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
    	        	        DEBUG_LOG("copy secure boot (part1) from bank0 to bank1...");
#if 0  // DEBUG  -- Bootloaderのコピーを2回に分けて実施
    	        	        flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE);
							if(IFLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG("NG\r\n");
								DEBUG_LOG2("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1);
#else  // DEBUG  -- Bootloaderのコピーを1回で実施
    	        	        flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, (((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE) + (((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL) * FLASH_CF_SMALL_BLOCK_SIZE));
							if(IFLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG("NG\r\n");
								DEBUG_LOG2("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2);
#endif  // DEBUG
#else /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
					        fwup_interrupts_disable();
    	        	        flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, (((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE) + (((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL) * FLASH_CF_SMALL_BLOCK_SIZE));
					        fwup_interrupts_enable();
							if(IFLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG("NG\r\n");
								DEBUG_LOG2("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							flash_error_code = IFLASH_SUCCESS;
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
#endif  /* FLASH_CFG_CODE_FLASH_BGO */
							break;

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1:
    	        	        if (IFLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            DEBUG_LOG("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }
							if(BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM > 0)
							{
	    	        	        DEBUG_LOG("copy secure boot (part2) from bank0 to bank1...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
								flash_api_error_code = fwup_flash_write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL * FLASH_CF_SMALL_BLOCK_SIZE);
								if(IFLASH_SUCCESS != flash_api_error_code)
								{
									DEBUG_LOG("NG\r\n");
									DEBUG_LOG2("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
									secure_boot_error_code = FWUP_FAIL;
									break;
								}
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2);
#else
						        fwup_interrupts_disable();
								flash_api_error_code = fwup_flash_write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL * FLASH_CF_SMALL_BLOCK_SIZE);
						        fwup_interrupts_enable();
								if(IFLASH_SUCCESS != flash_api_error_code)
								{
									DEBUG_LOG("NG\r\n");
									DEBUG_LOG2("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
									fwup_update_status(FWUP_STATE_FATAL_ERROR);
									secure_boot_error_code = FWUP_FAIL;
									break;
								}
								flash_error_code = IFLASH_SUCCESS;
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
							}
							else
							{
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
							}
							break;

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
    	            		/* this state will be update by flash callback */
    	        			break;

#endif /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) Dual bank mode */

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2:
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
    	        	        if (IFLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            DEBUG_LOG("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }
#endif /* (MCU_SERIES_RZA2) */
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
							if (false == boot_protect_flag)
							{
								/* Set boot protect */
								fwup_set_boot_protect();
								DEBUG_LOG("set boot protect.\r\n");
							}
#endif
							DEBUG_LOG("========== install user program phase ==========\r\n");
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
							DEBUG_LOG("erase install area (data flash): ");
							/* Erase data flash */
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
							flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT);
#else  /* (FLASH_CFG_DATA_FLASH_BGO == 0) */
			    	        fwup_interrupts_disable();
							flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
			    	        fwup_interrupts_enable();
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							flash_error_code = IFLASH_SUCCESS;
							fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE);
#endif  /* FLASH_CFG_DATA_FLASH_BGO */
							break;

    	        		case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE:
    	        	        if (IFLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            DEBUG_LOG("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }
							DEBUG_LOG("erase install area (code flash): ");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
							flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
#if (FWUP_ENV_USE_EXMEM_TEMPORARY == 1)
							fwup_interrupts_disable();
							exmem_error_code = fwup_flash_spi_erase(BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, BOOT_LOADER_TOTAL_UPDATE_SIZE);
							fwup_interrupts_enable();
							if(EXMEM_SUCCESS != exmem_error_code)
							{
								DEBUG_LOG2("R_FLASH_SPI_Erase() returns error. %d.\r\n", exmem_error_code);
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							flash_error_code = IFLASH_SUCCESS;
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE);
#else /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 0) */
			    	        fwup_interrupts_disable();
							flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
			    	        fwup_interrupts_enable();
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							flash_error_code = IFLASH_SUCCESS;
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE);
#endif /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 0) */
#endif /* FLASH_CFG_CODE_FLASH_BGO */
#elif (MCU_SERIES_RZA2)
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE);
#endif /* (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200) */
							break;

    	        		case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE:
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX600)
    	        	        if (IFLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            DEBUG_LOG("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }

    	        	        sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
    	        	        sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
							sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A];
							sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_A;
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT);
#elif (MCU_SERIES_RZA2)
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE);
#endif /* (MCU_SERIES_RZA2) */
    	        	        DEBUG_LOG2("send \"%s\" via UART.\r\n", INITIAL_FIRMWARE_FILE_NAME);
    	        	        break;

#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX600)
    	        		case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT:
							/* install code flash area */
							if(!firm_block_read(load_firmware_control_block.flash_buffer, load_firmware_control_block.offset))
							{
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
								flash_api_error_code = fwup_flash_write((uint32_t)load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
								if (FLASH_SUCCESS != flash_api_error_code)
								{
									DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
									DEBUG_LOG("system error.\r\n");
									secure_boot_error_code = FWUP_FAIL;
									break;
								}
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT);
#else  /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
#if (FWUP_ENV_USE_EXMEM_TEMPORARY == 1)
								exmem_error_code = fwup_flash_spi_write((uint8_t)&load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
								if(EXMEM_SUCCESS != exmem_error_code)
								{
									DEBUG_LOG("NG\r\n");
									DEBUG_LOG2("R_FLASH_SPI_Erase() returns error code = %d.\r\n", exmem_error_code);
									fwup_update_status(FWUP_STATE_FATAL_ERROR);
									secure_boot_error_code = FWUP_FAIL;
									break;
								}
								flash_error_code = IFLASH_SUCCESS;
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE);
#else /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 0) */
								/* Interrupt disabled by Receive callback function */
								flash_api_error_code = fwup_flash_write((uint32_t)load_firmware_control_block.flash_buffer, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + load_firmware_control_block.offset, sizeof(load_firmware_control_block.flash_buffer));
				    	        fwup_interrupts_enable();
								if (FLASH_SUCCESS != flash_api_error_code)
								{
									DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
									DEBUG_LOG("system error.\r\n");
									secure_boot_error_code = FWUP_FAIL;
									break;
								}
								flash_error_code = IFLASH_SUCCESS;
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE);
#endif /* (FWUP_ENV_USE_EXMEM_TEMPORARY = 0) */
#endif  /* (FLASH_CFG_CODE_FLASH_BGO = 0) */
							}
							break;

    	        		case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
    	        	        if (IFLASH_SUCCESS != flash_error_code)
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }

							load_firmware_control_block.offset += FWUP_WRITE_BLOCK_SIZE;
							load_firmware_control_block.progress = (uint32_t)(((float)(load_firmware_control_block.offset)/(float)((FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))*100));
							DEBUG_LOG2("installing firmware...%d%(%d/%dKB).\r", load_firmware_control_block.progress, load_firmware_control_block.offset/1024, (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER)/1024);
							if(load_firmware_control_block.offset < (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
							{
								/* one more loop */
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT);
							}
							else if(load_firmware_control_block.offset == (FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER))
							{
								DEBUG_LOG("\n");
								DEBUG_LOG("completed installing firmware.\r\n");
				    	    	DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
			    	            DEBUG_LOG("bank1(temporary area) on code flash integrity check...");

								/* Firmware verification for the signature type. */
#if (FWUP_ENV_USE_EXMEM_TEMPORARY == 1)
			    	            //*************************************************************
			    	            // CAUTION : Unfinished implementation.
			    	            //   Signing verification of serial flash data by ECDSA.
			    	            //   How to verify the FW data of serial flash.
			    	            //*************************************************************
								verification_result = -1;
#else /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 1) */
			    	            if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
								{
									uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
								    /* Hash message */
								    struct tc_sha256_state_struct xCtx;
								    tc_sha256_init(&xCtx);
								    tc_sha256_update(&xCtx,
								    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
											(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
								    tc_sha256_final(hash_sha256, &xCtx);
	    							verification_result = memcmp(firmware_update_control_block_bank1->signature, hash_sha256, sizeof(hash_sha256));
					    	    }
					    	    else if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
					    	    {
									verification_result = firmware_verification_sha256_ecdsa(
																		(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																		(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																		firmware_update_control_block_bank1->signature,
																		firmware_update_control_block_bank1->signature_size);
								}
								else
								{
									verification_result = -1;
								}
#endif /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 1) */

			    	            if(0 == verification_result)
								{
									DEBUG_LOG("OK\r\n");
									load_const_data_control_block.offset = 0;
									fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_READ_WAIT);
								}
								else
								{
									DEBUG_LOG("NG\r\n");
	    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
	    	        				secure_boot_error_code = FWUP_FAIL;
								}
							}
							else
							{
								DEBUG_LOG("\n");
								DEBUG_LOG("fatal error occurred.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
							}

#elif (MCU_SERIES_RZA2)
    	        		case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
							/* Download FW data from UART, and write to serial flash via HyperRAM */
							if (COMM_ERROR == fwup_download_fw_to_exmem())
							{
								DEBUG_LOG("system error.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
		    	            DEBUG_LOG("completed installing firmware.\r\n");
			    	    	DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
		    	            DEBUG_LOG("bank1(temporary area) on code flash integrity check...");

#if (MCU_SERIES_RZA2)
		    	            image_size = firmware_update_control_block_bank1->image_size;
		    	            if(image_size > BOOT_LOADER_TOTAL_UPDATE_SIZE)
		    	            {
		    	            	image_size = BOOT_LOADER_TOTAL_UPDATE_SIZE;
		    	            }
#endif /* (MCU_SERIES_RZA2) */

/* Firmware verification for the signature type. */
							if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
							{
								uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
							    /* Hash message */
							    struct tc_sha256_state_struct xCtx;
							    tc_sha256_init(&xCtx);
							    tc_sha256_update(&xCtx,
							    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
			//							APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);	// RZ/A2M OTA 2020.03.19 //
										image_size);	// RZ/A2M OTA 2020.03.19 //
							    tc_sha256_final(hash_sha256, &xCtx);
				    	        verification_result = memcmp(firmware_update_control_block_bank1->signature, hash_sha256, sizeof(hash_sha256));
				    	    }
				    	    else if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
				    	    {
								verification_result = firmware_verification_sha256_ecdsa(
																	(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
			//														APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,	// RZ/A2M OTA 2020.03.19 //
																	image_size,	// RZ/A2M OTA 2020.03.19 //
																	firmware_update_control_block_bank1->signature,
																	firmware_update_control_block_bank1->signature_size);
							}
							else
							{
								verification_result = -1;
							}

							if(0 == verification_result)
							{
								DEBUG_LOG("OK\r\n");
								load_const_data_control_block.offset = 0;
								fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_READ_WAIT);
							}
							else
							{
								DEBUG_LOG("NG\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
							}
#endif /* (MCU_SERIES_RZA2) */
							break;

						case FWUP_STATE_INSTALL_DATA_FLASH_READ_WAIT:
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
							/* install data flash area */
							if(!const_data_block_read(load_const_data_control_block.flash_buffer, load_const_data_control_block.offset))
							{
								fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE);
							}
							break;

						case FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE:
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
							flash_api_error_code = fwup_flash_write((uint32_t)&load_const_data_control_block.flash_buffer[load_const_data_control_block.offset/4], (uint32_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS + load_const_data_control_block.offset, FLASH_DF_BLOCK_SIZE);
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT);
#else /* (FLASH_CFG_DATA_FLASH_BGO == 0) */
			    	        fwup_interrupts_disable();
							flash_api_error_code = fwup_flash_write((uint32_t)&load_const_data_control_block.flash_buffer[load_const_data_control_block.offset/4], (uint32_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS + load_const_data_control_block.offset, FLASH_DF_BLOCK_SIZE);
			    	        fwup_interrupts_enable();
							if (IFLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							flash_error_code = IFLASH_SUCCESS;
							fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE);
#endif  /* (FLASH_CFG_DATA_FLASH_BGO == 1) */
							break;

						case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
    	            		/* this state will be update by flash callback */
							break;
						
						case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE:
    	        	        if (IFLASH_SUCCESS != flash_error_code)
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }

							load_const_data_control_block.offset += FLASH_DF_BLOCK_SIZE;
							load_const_data_control_block.progress = (uint32_t)(((float)(load_const_data_control_block.offset)/(float)((FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))*100));
							static uint32_t previous_offset = 0;
							if(previous_offset != (load_const_data_control_block.offset/1024))
							{
								DEBUG_LOG2("installing const data...%d%(%d/%dKB).\r", load_const_data_control_block.progress, load_const_data_control_block.offset/1024, (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER)/1024);
								previous_offset = load_const_data_control_block.offset/1024;
							}
							if(load_const_data_control_block.offset < (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))
							{
								/* one more loop */
								fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_READ_COMPLETE);
							}
							else if(load_const_data_control_block.offset == (FLASH_DF_BLOCK_SIZE * BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER))
							{
								DEBUG_LOG("\n");
								DEBUG_LOG("completed installing const data.\r\n");
								DEBUG_LOG("software reset...\r\n");
								R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
								software_reset();
							}
							else
							{
								DEBUG_LOG("\n");
								DEBUG_LOG("fatal error occurred.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
							}
							break;
#elif (MCU_SERIES_RZA2)
							/* Skip install data flash area */
							DEBUG_LOG("software reset...\r\n");
							R_SoftwareDelay(7500000);	//3s wait // RZ/A2M OTA 2020.03.19 //
							software_reset();
							break;
#endif
						default:
							break;
					}
    				break;
    	        case LIFECYCLE_STATE_TESTING:
    	            DEBUG_LOG("illegal status\r\n");
    	            DEBUG_LOG("swap bank...");
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
    	            R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
#elif (MCU_SERIES_RZA2)
					R_SoftwareDelay(7500000);	//3s wait // RZ/A2M OTA 2020.03.19 //
#endif
    	            bank_swap_with_software_reset();
    	            while(1);
    	            break;

    	        case LIFECYCLE_STATE_INSTALLING:
    	        	switch(fwup_get_status())
					{
    	        		case FWUP_STATE_BANK0_CHECK:
			    	    	DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank0->signature_type);
		    	            DEBUG_LOG("bank0(execute area) on code flash integrity check...");
#if (MCU_SERIES_RZA2)
		    	            image_size = firmware_update_control_block_bank0->image_size;
		    	            if(image_size > BOOT_LOADER_TOTAL_UPDATE_SIZE)
		    	            {
		    	            	image_size = BOOT_LOADER_TOTAL_UPDATE_SIZE;
		    	            }
#endif /* (MCU_SERIES_RZA2) */
							/* Firmware verification for the signature type. */
							if (!strcmp((const char *)firmware_update_control_block_bank0->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
							{
							    /* Hash message */
								uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
							    struct tc_sha256_state_struct xCtx;
							    tc_sha256_init(&xCtx);
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
							    tc_sha256_update(&xCtx,
							    		(uint8_t*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
										(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
#elif (MCU_SERIES_RZA2)
							    tc_sha256_update(&xCtx,
							    		(uint8_t*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
			//							APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);	// RZ/A2M OTA 2020.03.19 //
										image_size);	// RZ/A2M OTA 2020.03.19 //
#endif
							    tc_sha256_final(hash_sha256, &xCtx);
				    	        verification_result = memcmp(firmware_update_control_block_bank0->signature, hash_sha256, sizeof(hash_sha256));
				    	    }
				    	    else if (!strcmp((const char *)firmware_update_control_block_bank0->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
				    	    {
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
								verification_result = firmware_verification_sha256_ecdsa(
																	(const uint8_t *)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																	(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																	firmware_update_control_block_bank0->signature,
																	firmware_update_control_block_bank0->signature_size);
#elif (MCU_SERIES_RZA2)
								verification_result = firmware_verification_sha256_ecdsa(
																	(const uint8_t *)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
			//														APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,	// RZ/A2M OTA 2020.03.19 //
																	image_size,	// RZ/A2M OTA 2020.03.19 //
																	firmware_update_control_block_bank0->signature,
																	firmware_update_control_block_bank0->signature_size);
#endif
							}
							else
							{
								verification_result = -1;
							}

							if(0 == verification_result)
		    	            {
		    	                DEBUG_LOG("OK\r\n");
		    	            	if (!strcmp((const char *)pboot_loader_magic_code, BOOT_LOADER_MAGIC_CODE))
		    	            	{
									fwup_update_status(FWUP_STATE_FINALIZE);
								}
								else
								{
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
									boot_protect_flag = fwup_get_boot_protect();
#endif /* (FWUP_CFG_BOOT_PROTECT_ENABLE == 1) */
									if (false == boot_protect_flag)
									{
										/* No Boot Protected */
#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) /* Dual bank mode */
										DEBUG_LOG("erase bank1 secure boot mirror area...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
										DEBUG_LOG("erase bank1 secure boot mirror area...");
										flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
										if(FLASH_SUCCESS != flash_api_error_code)
										{
											DEBUG_LOG("NG\r\n");
											DEBUG_LOG2("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
											fwup_update_status(FWUP_STATE_FATAL_ERROR);
											secure_boot_error_code = FWUP_FAIL;
											break;
										}
										fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
								        fwup_interrupts_disable();
										flash_api_error_code = fwup_flash_erase(BOOT_LOADER_MIRROR_HIGH_ADDRESS, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL + BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
								        fwup_interrupts_enable();
										if(IFLASH_SUCCESS != flash_api_error_code)
										{
											DEBUG_LOG("NG\r\n");
											DEBUG_LOG2("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
											fwup_update_status(FWUP_STATE_FATAL_ERROR);
											secure_boot_error_code = FWUP_FAIL;
											break;
										}
										flash_error_code = IFLASH_SUCCESS;
										fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE);
#endif  /* FLASH_CFG_CODE_FLASH_BGO */
#else /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) Linear mode */
										/* Do not FLASH erase because the Bootloader is not copied */
										/* Skip to next status */
										flash_error_code = IFLASH_SUCCESS;
										fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
#endif /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 1) Linear mode */
									}
									else
									{
										/* Already Boot Protected */
										/* Skip boot copy process */
										DEBUG_LOG("boot protected: skip copy secure boot from bank0 to bank1...");
										flash_error_code = IFLASH_SUCCESS;
										fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
									}
								}
		    	            }
		    	            else
		    	            {
		    					DEBUG_LOG("NG.\r\n");
		    					DEBUG_LOG("Code flash is completely broken.\r\n");
		    					DEBUG_LOG("Please erase all code flash.\r\n");
		    					DEBUG_LOG("And, write secure boot using debugger.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
		    					secure_boot_error_code = FWUP_FAIL;
		    	            }
							break;

#if (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) /* Dual bank mode */
    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE:
    	        	        if (IFLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            DEBUG_LOG("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }
    	        	        DEBUG_LOG("copy secure boot (part1) from bank0 to bank1...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
    	        	        flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE);
							if(FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG("NG\r\n");
								DEBUG_LOG2("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
					        fwup_interrupts_disable();
    	        	        flash_api_error_code = fwup_flash_write((uint32_t)BOOT_LOADER_LOW_ADDRESS, (uint32_t)BOOT_LOADER_MIRROR_LOW_ADDRESS, ((uint32_t)BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM) * FLASH_CF_MEDIUM_BLOCK_SIZE);
					        fwup_interrupts_enable();
							if(IFLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG("NG\r\n");
								DEBUG_LOG2("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							flash_error_code = IFLASH_SUCCESS;
							fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1);
#endif  /* FLASH_CFG_CODE_FLASH_BGO */
							break;

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
    	            		/* this state will be update by flash callback */
    	        			break;

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1:
    	        	        if (IFLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            DEBUG_LOG("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }
							if(BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM > 0)
							{
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
	    	        	        DEBUG_LOG("copy secure boot (part2) from bank0 to bank1...");
								flash_api_error_code = fwup_flash_write((uint32_t)FLASH_CF_BLOCK_7, (uint32_t)FLASH_CF_BLOCK_45, BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL * FLASH_CF_SMALL_BLOCK_SIZE);
								if(FLASH_SUCCESS != flash_api_error_code)
								{
									DEBUG_LOG("NG\r\n");
									DEBUG_LOG2("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
									secure_boot_error_code = FWUP_FAIL;
									break;
								}
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
								/* Do not FLASH write because the Bootloader is not copied */
								/* Skip to next status */
								flash_error_code = IFLASH_SUCCESS;
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
#endif  /* FLASH_CFG_CODE_FLASH_BGO */
							}
							else
							{
								flash_error_code = IFLASH_SUCCESS;
								fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
							}
							break;

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
    	            		/* this state will be update by flash callback */
    	        			break;
#endif /* (FWUP_ENV_CODE_FLASH_BANK_MODE == 0) Dual bank mode */

    	        		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2:
    	        	        if (IFLASH_SUCCESS == flash_error_code)
    	        	        {
    	        	            DEBUG_LOG("OK\r\n");
    	        	        }
    	        	        else
    	        	        {
    	        	            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
    	        	            DEBUG_LOG("system error.\r\n");
    	        				fwup_update_status(FWUP_STATE_FATAL_ERROR);
    	        				secure_boot_error_code = FWUP_FAIL;
    	        	            break;
    	        	        }
#if (FWUP_CFG_BOOT_PROTECT_ENABLE == 1)
							if (false == fwup_get_boot_protect())
							{
								/* Set boot protect */
								fwup_set_boot_protect();
								DEBUG_LOG("set boot protect.\r\n");
							}
#endif
							fwup_update_status(FWUP_STATE_FINALIZE);
							break;

						case FWUP_STATE_FINALIZE:
	    	                DEBUG_LOG("jump to user program\r\n");
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
	    	                R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
#elif (MCU_SERIES_RZA2)
							R_SoftwareDelay(2500000);	//1s wait // RZ/A2M OTA 2020.03.19 //
#endif
	    	                secure_boot_error_code = FWUP_SUCCESS;
		    	        	break;
		    	    }
   	            	break;

    	    	case LIFECYCLE_STATE_VALID:
					switch(fwup_get_status())
					{
						case FWUP_STATE_BANK0_UPDATE_CHECK:
			    	    	DEBUG_LOG2("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank0->signature_type);
		    	            DEBUG_LOG("bank0(execute area) on code flash integrity check...");
#if (MCU_SERIES_RZA2)
		    	            image_size = firmware_update_control_block_bank0->image_size;
		    	            if(image_size > BOOT_LOADER_TOTAL_UPDATE_SIZE)
		    	            {
		    	            	image_size = BOOT_LOADER_TOTAL_UPDATE_SIZE;
		    	            }
#endif /* (MCU_SERIES_RZA2) */
							/* Firmware verification for the signature type. */
							if (!strcmp((const char *)firmware_update_control_block_bank0->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
							{
							    /* Hash message */
								uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
							    struct tc_sha256_state_struct xCtx;
							    tc_sha256_init(&xCtx);
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
							    tc_sha256_update(&xCtx,
							    		(uint8_t*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
										(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
#elif (MCU_SERIES_RZA2)
							    tc_sha256_update(&xCtx,
							    		(uint8_t*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
			//							APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);	// RZ/A2M OTA 2020.03.19 //
										image_size);	// RZ/A2M OTA 2020.03.19 //
#endif /* (MCU_SERIES_RZA2) */
							    tc_sha256_final(hash_sha256, &xCtx);
				    	        verification_result = memcmp(firmware_update_control_block_bank0->signature, hash_sha256, sizeof(hash_sha256));
				    	    }
				    	    else if (!strcmp((const char *)firmware_update_control_block_bank0->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
				    	    {
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
								verification_result = firmware_verification_sha256_ecdsa(
																	(const uint8_t *)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																	(FLASH_CF_MEDIUM_BLOCK_SIZE * BOOT_LOADER_UPDATE_TARGET_BLOCK_NUMBER) - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
																	firmware_update_control_block_bank0->signature,
																	firmware_update_control_block_bank0->signature_size);
#elif (MCU_SERIES_RZA2)
								verification_result = firmware_verification_sha256_ecdsa(
																	(const uint8_t *)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
			//														APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,	// RZ/A2M OTA 2020.03.19 //
																	image_size,	// RZ/A2M OTA 2020.03.19 //
																	firmware_update_control_block_bank0->signature,
																	firmware_update_control_block_bank0->signature_size);
#endif /* (MCU_SERIES_RZA2) */
							}
							else
							{
								verification_result = -1;
							}

							if(0 == verification_result)
		    	            {
		    	                DEBUG_LOG("OK\r\n");
		    	            	if(firmware_update_control_block_bank1->image_flag != LIFECYCLE_STATE_BLANK)
		    	            	{
		    	                    DEBUG_LOG("erase install area (code flash): ");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
		    	                    flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
		    	                    if (IFLASH_SUCCESS != flash_api_error_code)
		    	                    {
		    	                        DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
		    	                        DEBUG_LOG("system error.\r\n");
										fwup_update_status(FWUP_STATE_FATAL_ERROR);
										secure_boot_error_code = FWUP_FAIL;
					    	            break;
					    	        }
									fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
#if (FWUP_ENV_USE_EXMEM_TEMPORARY == 1)
									fwup_interrupts_disable();
									exmem_error_code = fwup_flash_spi_erase(BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, BOOT_LOADER_TOTAL_UPDATE_SIZE);
									fwup_interrupts_enable();
									if(EXMEM_SUCCESS != exmem_error_code)
									{
										DEBUG_LOG2("R_FLASH_SPI_Erase() returns error. %d.\r\n", exmem_error_code);
		    	                        DEBUG_LOG("system error.\r\n");
										fwup_update_status(FWUP_STATE_FATAL_ERROR);
										secure_boot_error_code = FWUP_FAIL;
										break;
									}
									flash_error_code = IFLASH_SUCCESS;
									fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE);
#else /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 0) */
					    	        fwup_interrupts_disable();
		    	                    flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
					    	        fwup_interrupts_enable();
		    	                    if (IFLASH_SUCCESS != flash_api_error_code)
		    	                    {
		    	                        DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
		    	                        DEBUG_LOG("system error.\r\n");
										fwup_update_status(FWUP_STATE_FATAL_ERROR);
										secure_boot_error_code = FWUP_FAIL;
					    	            break;
					    	        }
		    	                    flash_error_code = IFLASH_SUCCESS;
									fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE);
#endif /* FWUP_ENV_USE_EXMEM_TEMPORARY */
#endif  /* FLASH_CFG_CODE_FLASH_BGO */
								}
								else
								{
									fwup_update_status(FWUP_STATE_FINALIZE);
								}
		    	            }
		    	            else
		    	            {
		    					DEBUG_LOG("NG.\r\n");
		    					DEBUG_LOG("Code flash is completely broken.\r\n");
		    					DEBUG_LOG("Please erase all code flash.\r\n");
		    					DEBUG_LOG("And, write secure boot using debugger.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
		    					secure_boot_error_code = FWUP_FAIL;
		    	            }
		    	            break;

				    	case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
				    		break;

		    	        case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE:
		    	        	if (IFLASH_SUCCESS == flash_error_code)
		    	        	{
		    	        		DEBUG_LOG("OK\r\n");
								fwup_update_status(FWUP_STATE_FINALIZE);
		    	        	}
		    	        	else
		    	        	{
					            DEBUG_LOG2("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
		    	        		DEBUG_LOG("system error.\r\n");
		    	        		fwup_update_status(FWUP_STATE_FATAL_ERROR);
		    	        		secure_boot_error_code = FWUP_FAIL;
		    	        		break;
		    	        	}
		    	        	break;
		    	        
		    	        case FWUP_STATE_FINALIZE:
	    	                DEBUG_LOG("jump to user program\r\n");
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
	    	                R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
#elif (MCU_SERIES_RZA2)
							R_SoftwareDelay(2500000);	//1s wait // RZ/A2M OTA 2020.03.19 //
#endif
	    	                secure_boot_error_code = FWUP_SUCCESS;
		    	        	break;
		    	    }
   	            	break;
   	           	case LIFECYCLE_STATE_EOL:
					switch(fwup_get_status())
					{
						case FWUP_STATE_BANK0_CHECK:
							if (LIFECYCLE_STATE_EOL == firmware_update_control_block_bank1->image_flag)
							{
								/* The status of bank1(temporary area) is already EOL */
	    	                    DEBUG_LOG("check bank1 status: ");
								flash_error_code = IFLASH_SUCCESS;
								fwup_update_status(FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE);
							}
							else
							{
								/* The status of bank1(temporary area) is NOT EOL */
	    	                    DEBUG_LOG("erase install area (code flash): ");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
	    	                    flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
	    	                    if (FLASH_SUCCESS != flash_api_error_code)
	    	                    {
			    					DEBUG_LOG("NG.\r\n");
	    	                        DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
	    	                        DEBUG_LOG("system error.\r\n");
									fwup_update_status(FWUP_STATE_FATAL_ERROR);
									secure_boot_error_code = FWUP_FAIL;
				    	            break;
				    	        }
								fwup_update_status(FWUP_STATE_EOL_BANK1_ERASE_WAIT);
#else
#if (FWUP_ENV_USE_EXMEM_TEMPORARY == 1)
								fwup_interrupts_disable();
								exmem_error_code = fwup_flash_spi_erase(BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, BOOT_LOADER_TOTAL_UPDATE_SIZE);
								fwup_interrupts_enable();
								if(EXMEM_SUCCESS != exmem_error_code)
								{
									DEBUG_LOG2("R_FLASH_SPI_Erase() returns error. %d.\r\n", exmem_error_code);
	    	                        DEBUG_LOG("system error.\r\n");
									fwup_update_status(FWUP_STATE_FATAL_ERROR);
									secure_boot_error_code = FWUP_FAIL;
									break;
								}
								flash_error_code = IFLASH_SUCCESS;
								fwup_update_status(FWUP_STATE_EOL_BANK1_ERASE_COMPLETE);
#else /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 0) */
				    	        fwup_interrupts_disable();
	    	                    flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_HIGH_ADDRESS, FLASH_NUM_BLOCKS_CF - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_SMALL - BOOT_LOADER_MIRROR_BLOCK_NUM_FOR_MEDIUM);
				    	        fwup_interrupts_enable();
	    	                    if (FLASH_SUCCESS != flash_api_error_code)
	    	                    {
			    					DEBUG_LOG("NG.\r\n");
	    	                        DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
	    	                        DEBUG_LOG("system error.\r\n");
									fwup_update_status(FWUP_STATE_FATAL_ERROR);
									secure_boot_error_code = FWUP_FAIL;
				    	            break;
				    	        }
	    	                    flash_error_code = IFLASH_SUCCESS;
								fwup_update_status(FWUP_STATE_EOL_BANK1_ERASE_COMPLETE);
#endif /* FWUP_ENV_USE_EXMEM_TEMPORARY */
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
							}
		    	            break;

				    	case FWUP_STATE_EOL_BANK1_ERASE_WAIT:
    	            		/* this state will be update by flash callback */
				    		break;

		    	        case FWUP_STATE_EOL_BANK1_ERASE_COMPLETE:
					        if (IFLASH_SUCCESS == flash_error_code)
					        {
					            DEBUG_LOG("OK\r\n");
					        }
					        else
					        {
		    					DEBUG_LOG("NG.\r\n");
					            DEBUG_LOG2("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
					            DEBUG_LOG("system error.\r\n");
					            fwup_update_status(FWUP_STATE_FATAL_ERROR);
					        }

#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
				   	    	memcpy(block_buffer, (void*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
#elif (MCU_SERIES_RZA2)
				   	    	memcpy(block_buffer, (void*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
#else
				   	    	/* Fix me for other MCU series */
#endif /* (MCU_SERIES_RZA2) */
							firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_EOL;
				   	    	DEBUG_LOG2("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
					        DEBUG_LOG("bank1(temporary area) block0 write (to update LIFECYCLE_STATE)...");
#if (FLASH_CFG_CODE_FLASH_BGO == 1)
					        flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT);
#else /* (FLASH_CFG_CODE_FLASH_BGO == 0) */
#if (FWUP_ENV_USE_EXMEM_TEMPORARY == 1)
							fwup_interrupts_disable();
							exmem_error_code = fwup_flash_spi_write((uint8_t *)&firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);
							fwup_interrupts_enable();
							if(EXMEM_SUCCESS != exmem_error_code)
							{
								DEBUG_LOG("NG\r\n");
								DEBUG_LOG2("R_FLASH_SPI_Erase() returns error code = %d.\r\n", exmem_error_code);
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							flash_error_code = IFLASH_SUCCESS;
							fwup_update_status(FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE);
#else /* (FWUP_ENV_USE_EXMEM_TEMPORARY == 0) */
			    	        fwup_interrupts_disable();
					        flash_api_error_code = fwup_flash_write((uint32_t)firmware_update_control_block_tmp, (uint32_t)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, FLASH_CF_MEDIUM_BLOCK_SIZE);
			    	        fwup_interrupts_enable();
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Write() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							flash_error_code = IFLASH_SUCCESS;
							fwup_update_status(FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE);
#endif /* FWUP_ENV_USE_EXMEM_TEMPORARY */
#endif  /* (FLASH_CFG_CODE_FLASH_BGO == 1) */
							break;

				    	case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT:
    	            		/* this state will be update by flash callback */
				    		break;

				    	case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE:
					        if (IFLASH_SUCCESS == flash_error_code)
					        {
					            DEBUG_LOG("OK\r\n");
					    	    DEBUG_LOG2("bank 0 status = 0x%x [%s]\r\n", firmware_update_control_block_bank0->image_flag, get_status_string(firmware_update_control_block_bank0->image_flag));
					    	    DEBUG_LOG2("bank 1 status = 0x%x [%s]\r\n", firmware_update_control_block_bank1->image_flag, get_status_string(firmware_update_control_block_bank1->image_flag));
					        }
					        else
					        {
					            DEBUG_LOG2("R_FLASH_Write() callback error. %d.\r\n", flash_error_code);
					            DEBUG_LOG("system error.\r\n");
								fwup_update_status(FWUP_STATE_FATAL_ERROR);
								secure_boot_error_code = FWUP_FAIL;
					            break;
					        }

		   	           		/* Erase Data Flash */
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
							DEBUG_LOG("erase install area (data flash): ");
#if (FLASH_CFG_DATA_FLASH_BGO == 1)
							flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
							if (FLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT);
#else
			    	        fwup_interrupts_disable();
							flash_api_error_code = fwup_flash_erase((flash_block_address_t)BOOT_LOADER_USER_CONST_DATA_LOW_ADDRESS, BOOT_LOADER_UPDATE_CONST_DATA_TARGET_BLOCK_NUMBER);
			    	        fwup_interrupts_enable();
							if (IFLASH_SUCCESS != flash_api_error_code)
							{
								DEBUG_LOG2("R_FLASH_Erase() returns error. %d.\r\n", flash_error_code);
								DEBUG_LOG("system error.\r\n");
								secure_boot_error_code = FWUP_FAIL;
								break;
							}
							fwup_update_status(FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE);
#endif  /* (FLASH_CFG_DATA_FLASH_BGO == 1) */
							break;

				   		case FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT:
				       		/* this state will be update by flash callback */
				   			break;

				   		case FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE:
				   	        if (IFLASH_SUCCESS == flash_error_code)
				   	        {
				  	            DEBUG_LOG("OK\r\n");
				   	        }
				   	        else
				   	        {
				   	            DEBUG_LOG2("R_FLASH_Erase() callback error. %d.\r\n", flash_error_code);
				   	            DEBUG_LOG("system error.\r\n");
				   				fwup_update_status(FWUP_STATE_FATAL_ERROR);
				   				secure_boot_error_code = FWUP_FAIL;
				   	            break;
				   	        }
#endif /* (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200) */

		   	           		/* End of the EOL process, infinity loop on Bootloader */
	    	                DEBUG_LOG("End Of Life process finished.\r\n");
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
	    	                R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
#elif (MCU_SERIES_RZA2)
	    	                R_SoftwareDelay(2500000);	//1s wait // RZ/A2M OTA 2020.03.19 //
#endif
	    	                secure_boot_error_code = FWUP_END_OF_LIFE;
		    	        	break;
		    	    }
   	           		break;

    	        default:
    	            DEBUG_LOG2("illegal flash rom status code 0x%x.\r\n", firmware_update_control_block_bank0->image_flag);
					DEBUG_LOG("Code flash is completely broken.\r\n");
					DEBUG_LOG("Please erase all code flash.\r\n");
					DEBUG_LOG("And, write secure boot using debugger.\r\n");
	   				fwup_update_status(FWUP_STATE_FATAL_ERROR);
	   				secure_boot_error_code = FWUP_FAIL;
    	            break;
    	    }
    }
    if (secure_boot_error_code == FWUP_SUCCESS ||
    		secure_boot_error_code == FWUP_FAIL ||
			secure_boot_error_code == FWUP_END_OF_LIFE)
    {
        /* Close System-timer if boot process will end */
    	monitoring_error_code = fwup_state_monitoring_close();
    	if (MONI_SUCCESS != monitoring_error_code)
    	{
			DEBUG_LOG2("R_SYS_TIME_Close() returns error. %d.\r\n", monitoring_error_code);
			DEBUG_LOG("system error.\r\n");
			fwup_update_status(FWUP_STATE_FATAL_ERROR);
			secure_boot_error_code = FWUP_FAIL;
    	}

	    my_comm_err = fwup_communication_close();
	    if (COMM_SUCCESS != my_comm_err)
	    {
			DEBUG_LOG2("R_SCI_Close() returns error. %d.\r\n", my_comm_err);
			DEBUG_LOG("system error.\r\n");
			fwup_update_status(FWUP_STATE_FATAL_ERROR);
			secure_boot_error_code = FWUP_FAIL;
	    }

	    flash_api_error_code = fwup_flash_close();
	    if (IFLASH_SUCCESS != flash_api_error_code)
	    {
	        DEBUG_LOG2("R_FLASH_Close() returns error. %d.\r\n", flash_api_error_code);
	        DEBUG_LOG("system error.\r\n");
			fwup_update_status(FWUP_STATE_FATAL_ERROR);
			secure_boot_error_code = FWUP_FAIL;
	    }

#if (FWUP_CFG_USE_EXMEM != 0)
	    exmem_error_code = fwup_flash_spi_close();
	    if (EXMEM_SUCCESS != exmem_error_code)
	    {
	    	DEBUG_LOG2("R_FLASH_SPI_Close() returns error. %d.\r\n", exmem_error_code);
	    	DEBUG_LOG("system error.\r\n");
	    	fwup_update_status(FWUP_STATE_FATAL_ERROR);
	    	secure_boot_error_code = FWUP_FAIL;
   		}
#endif /* (FWUP_CFG_USE_EXMEM != 0) */

#if (MCU_SERIES_RZA2)
		/* stop all interrupt completely */
        /* ==== System Lock ==== */
        R_OS_SysLock();

        /* ==== Cleaning and invalidation of the L1 data cache ==== */
        R_CACHE_L1DataCleanInvalidAll();
        __DSB();

        /* ==== Cleaning and invalidation of the L2 cache ==== */
        R_CACHE_L2CleanInvalidAll();

        /* ==== Invalidate all TLB entries ==== */
        r_mmu_tlbiall();

        /* ==== Invalidate the L1 instruction cache ==== */
        r_cache_l1_i_inv_all();
#endif /* (MCU_SERIES_RZA2) */
    }
    return secure_boot_error_code;
}

void R_FWUP_ExecuteFirmware(void)
{
#if (BSP_MCU_SERIES_RX600||BSP_MCU_SERIES_RX200)
	uint32_t addr;

	/* stop all interrupt completely */
	R_BSP_SET_PSW(0);
	addr = *(uint32_t*)USER_RESET_VECTOR_ADDRESS;
	((void (*)())addr)();
#elif (MCU_SERIES_RZA2)
	/* address jump */	// RZ/A2M OTA 2020.03.19 //
	((void(*)(void))USER_RESET_VECTOR_ADDRESS)();
#else
    /* Fix me for other MCU family */
#endif
}

static void software_reset(void)
{
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
	/* stop all interrupt completely */
	R_BSP_SET_PSW(0);
    fwup_interrupts_disable();
    fwup_register_protect_disable();
    R_BSP_SoftwareReset();
    while(1);   /* software reset */
#elif (MCU_SERIES_RZA2)
	volatile uint16_t data;
	WDT.WTCNT.WORD = 0x5A00;
	data = WDT.WRCSR.WORD;
	WDT.WTCNT.WORD = 0x5A00;
	WDT.WRCSR.WORD = 0xA500;
	WDT.WTCSR.WORD = 0xA578;
	WDT.WRCSR.WORD = 0x5A40;
	while(1){}
#endif
}

static void bank_swap_with_software_reset(void)
{
#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
	/* stop all interrupt completely */
	R_BSP_SET_PSW(0);
    fwup_interrupts_disable();
    fwup_flash_set_bank_toggle();
    fwup_register_protect_disable();
    R_BSP_SoftwareReset();
    while(1);   /* software reset */
#elif (MCU_SERIES_RZA2)
    software_reset();
#endif
}

#if (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200)
/***********************************************************************************************************************
* Function Name: firm_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static int32_t firm_block_read(uint32_t *firmware, uint32_t offset)
{
	int32_t error_code = -1;
	if (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag)
	{
		memcpy(firmware, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer, FWUP_WRITE_BLOCK_SIZE);
		sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
		error_code = 0;
	}
	else if  (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag)
	{
		memcpy(firmware, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer, FWUP_WRITE_BLOCK_SIZE);
		sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
		error_code = 0;
	}
	else
	{
		error_code = -1;
	}
	return error_code;
}

/***********************************************************************************************************************
* Function Name: const_data_block_read
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static int32_t const_data_block_read(uint32_t *const_data, uint32_t offset)
{
	int32_t error_code = -1;
	if (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag)
	{
		memcpy(const_data, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer, FWUP_WRITE_BLOCK_SIZE);
		sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
		error_code = 0;
	}
	else if  (BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL == sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag)
	{
		memcpy(const_data, sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer, FWUP_WRITE_BLOCK_SIZE);
		sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B].buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_EMPTY;
		error_code = 0;
	}
	else
	{
		error_code = -1;
	}
	return error_code;
}

/*****************************************************************************
* Function Name: my_sci_callback
* Description  : This is a template for an SCI Async Mode callback function.
* Arguments    : pArgs -
*                pointer to sci_cb_p_args_t structure cast to a void. Structure
*                contains event and associated data.
* Return Value : none
******************************************************************************/
uint32_t error_count1 = 0;
uint32_t error_count2 = 0;
uint32_t rcv_count1 = 0;
uint32_t rcv_count2 = 0;

void my_sci_callback(void *pArgs)
{
    sci_cb_args_t   *p_args;

    p_args = (sci_cb_args_t *)pArgs;

    if (SCI_EVT_RX_CHAR == p_args->event)
    {
        /* From RXI interrupt; received character data is in p_args->byte */
    	if(sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size < sizeof(sci_receive_control_block.p_sci_buffer_control->buffer))
    	{
			R_SCI_Receive(p_args->hdl, &sci_receive_control_block.p_sci_buffer_control->buffer[sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size++], 1);
			if (sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size == sizeof(sci_receive_control_block.p_sci_buffer_control->buffer))
			{
#if (FLASH_CFG_CODE_FLASH_BGO == 0)
				fwup_interrupts_disable();  /* In case of Blocking, interrupts are disabled until the flash write is completed. */
#endif /* FLASH_CFG_CODE_FLASH_BGO == 0 */
				sci_receive_control_block.p_sci_buffer_control->buffer_occupied_byte_size = 0;
				sci_receive_control_block.p_sci_buffer_control->buffer_full_flag = BOOT_LOADER_SCI_RECEIVE_BUFFER_FULL;
				sci_receive_control_block.total_byte_size += FWUP_WRITE_BLOCK_SIZE;
				if (BOOT_LOADER_SCI_CONTROL_BLOCK_A == sci_receive_control_block.current_state)
				{
					sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_B;
					sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_B];
				}
				else
				{
					sci_receive_control_block.current_state = BOOT_LOADER_SCI_CONTROL_BLOCK_A;
					sci_receive_control_block.p_sci_buffer_control = &sci_buffer_control[BOOT_LOADER_SCI_CONTROL_BLOCK_A];
				}
			}
        	rcv_count1++;
    	}
    	rcv_count2++;
    }
    else if (SCI_EVT_RXBUF_OVFL == p_args->event)
    {
        /* From RXI interrupt; rx queue is full; 'lost' data is in p_args->byte
           You will need to increase buffer size or reduce baud rate */
    	nop();
    	error_count1++;
    }
    else if (SCI_EVT_OVFL_ERR == p_args->event)
    {
        /* From receiver overflow error interrupt; error data is in p_args->byte
           Error condition is cleared in calling interrupt routine */
    	nop();
    	error_count2++;
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
#endif /* (BSP_MCU_SERIES_RX600 || BSP_MCU_SERIES_RX200) */

#if (FLASH_CFG_CODE_FLASH_BGO == 1)
/***********************************************************************************************************************
* Function Name: my_flash_callback
* Description  :
* Arguments    :
* Return Value :
***********************************************************************************************************************/
static void my_flash_callback(void *event)
{
	uint32_t event_code = FLASH_ERR_FAILURE;
	event_code = *((uint32_t*)event);

	flash_error_code = IFLASH_ERR_FAILURE;

    if((event_code == FLASH_INT_EVENT_WRITE_COMPLETE) || (event_code == FLASH_INT_EVENT_ERASE_COMPLETE))
    {
    	flash_error_code = IFLASH_SUCCESS;
    }

	switch(fwup_get_status())
	{
		case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_WAIT:
			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE);
			break;
		case FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_WAIT:
			fwup_update_status(FWUP_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE);
			break;
		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_WAIT:
			fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_ERASE_COMPLETE);
			break;
		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT1:
			fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE1);
			break;
		case FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_WAIT2:
			fwup_update_status(FWUP_STATE_BANK0_INSTALL_SECURE_BOOT_WRITE_COMPLETE2);
			break;
		case FWUP_STATE_INSTALL_DATA_FLASH_ERASE_WAIT:
			fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_ERASE_COMPLETE);
			break;
		case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_WAIT:
			fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE);
			break;
		case FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_WAIT:
			fwup_update_status(FWUP_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE);
			break;
		case FWUP_STATE_INSTALL_DATA_FLASH_WRITE_WAIT:
			fwup_update_status(FWUP_STATE_INSTALL_DATA_FLASH_WRITE_COMPLETE);
			break;
		case FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_WAIT:
			fwup_update_status(FWUP_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE);
			break;
		case FWUP_STATE_EOL_BANK1_ERASE_WAIT:
			fwup_update_status(FWUP_STATE_EOL_BANK1_ERASE_COMPLETE);
			break;
		case FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_WAIT:
			fwup_update_status(FWUP_STATE_EOL_BANK1_LIFECYCLE_WRITE_COMPLETE);
			break;
		case FWUP_STATE_EOL_DATA_FLASH_ERASE_WAIT:
			fwup_update_status(FWUP_STATE_EOL_DATA_FLASH_ERASE_COMPLETE);
			break;
		default:
			break;
	}
}
#endif /* (FLASH_CFG_CODE_FLASH_BGO == 1) */

#if (MCU_SERIES_RZA2)
static comm_err_t fwup_download_fw_to_exmem(void)
{
	comm_err_t ret;
    uint32_t k, cntcnt;
	int32_t fl_ret;
	uint32_t i;

    cntcnt = 0;
    for (k = 0; k < downloaded_image_size; k++)
    {
        ret = fwup_communication_receive(&hyper[k], 1);
        if (COMM_ERROR == ret)
        {
        	return ret;
        }
        cntcnt ++;
        if (cntcnt >= 0x1000)
        {
        	DEBUG_LOG2("downloaded:0x%08x\n\r", k);
            cntcnt = 0;
        }
        if( k == BOOT_LOADER_IMAGE_SIZE_BOT_ADR )
        {
        	FIRMWARE_UPDATE_CONTROL_BLOCK* p_header = (FIRMWARE_UPDATE_CONTROL_BLOCK*)&hyper;
        	if( p_header->image_size > BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH
        	 && p_header->image_size <= BOOT_LOADER_TOTAL_UPDATE_SIZE )
            {
				image_size = p_header->image_size;
            	downloaded_image_size = image_size + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH;
            	DEBUG_LOG2("DownLoad Image Size:0x%08x\n\r", downloaded_image_size);
            }
        }
    }

	if(downloaded_image_size == (BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH + BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH))
	{
		DEBUG_LOG("Image Size error.\r\n");
		ret = COMM_ERROR;
//		software_reset();
	}

	fwup_communication_close();
	// RZ/A2M OTA 2020.03.19 //  <<--

	uint8_t* pflash = (uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;

	/* temporary area clear */	// RZ/A2M OTA 2020.03.19 //
	for (i = 0; i < downloaded_image_size; i += SF_SECTOR_SIZE)
	{
		flash_erase_sector(NULL, BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + i);
	}

	/* Hyper RAM -> temporary area write*/	// RZ/A2M OTA 2020.03.19 //
	fl_ret = flash_program_page(NULL,BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,hyper,downloaded_image_size);

	if(fl_ret == -1)
	{
		printf("flash_program_page() returns error.\r\n");
		printf("system error.\r\n");
		ret = COMM_ERROR;
	}

	return ret;
}

/* ソフト処理で領域の入れ替えを行う */	// RZ/A2M OTA 2020.03.19 //
static void fwup_copy_fw_to_exe_area(void)
{
	uint32_t i;
	uint8_t* pflash = (uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;
	int32_t  fl_ret;

    /* Hyper RAM clear */	// RZ/A2M OTA 2020.03.19 //
    memset(hyper, 0x00, sizeof(hyper));

    /* exec area clear */	// RZ/A2M OTA 2020.03.19 //
//    for (i = 0; i < APP_SIZE; i+=SF_SECTOR_SIZE)	// RZ/A2M OTA 2020.03.19 //
    for (i = 0; i < (image_size + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH); i+=SF_SECTOR_SIZE)	// RZ/A2M OTA 2020.03.19 //
    {
    	flash_erase_sector(NULL, BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + i);
    }

    /* temporary area -> HyperRAM */	// RZ/A2M OTA 2020.03.19 //
//    for (i = 0; i < DLALLSIZE; i++)	// RZ/A2M OTA 2020.03.19 //
    for (i = 0; i < (image_size + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH); i++)	// RZ/A2M OTA 2020.03.19 //
    {
    	hyper[i] = *(pflash + i);
    }

    /* Hyper RAM -> exec area */	// RZ/A2M OTA 2020.03.19 //
    fl_ret = flash_program_page(NULL, BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS, hyper, (image_size + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH));	// RZ/A2M OTA 2020.03.19 //

	if(fl_ret == -1)
	{
		printf("R_FLASH_Write() returns error.\r\n");
		printf("system error.\r\n");
		fwup_update_status(FWUP_STATE_FATAL_ERROR);
	}
	else
	{
	    /* temporary area clear */	// RZ/A2M OTA 2020.03.19 //
	    for (i = 0; i < (image_size + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH); i+=SF_SECTOR_SIZE)
	    {
	    	flash_erase_sector(NULL, BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + i);
	    }
	}
//  software_reset();
}
#endif /* (MCU_SERIES_RZA2) */

static const uint8_t *get_status_string(uint8_t status)
{
	static const uint8_t status_string[][64] = {{"LIFECYCLE_STATE_BLANK"},
	                                            {"LIFECYCLE_STATE_TESTING"},
	                                            {"LIFECYCLE_STATE_INSTALLING"},
	                                            {"LIFECYCLE_STATE_VALID"},
	                                            {"LIFECYCLE_STATE_INVALID"},
	                                            {"LIFECYCLE_STATE_EOL"},
	                                            {"LIFECYCLE_STATE_UNKNOWN"}};
	const uint8_t *tmp;

	if(status == LIFECYCLE_STATE_BLANK)
	{
		tmp = status_string[0];
	}
	else if(status == LIFECYCLE_STATE_TESTING)
	{
		tmp = status_string[1];
	}
	else if(status == LIFECYCLE_STATE_INSTALLING)
	{
		tmp = status_string[2];
	}
	else if(status == LIFECYCLE_STATE_VALID)
	{
		tmp = status_string[3];
	}
	else if(status == LIFECYCLE_STATE_INVALID)
	{
		tmp = status_string[4];
	}
	else if(status == LIFECYCLE_STATE_EOL)
	{
		tmp = status_string[5];
	}
	else
	{
		tmp = status_string[6];
	}
	return tmp;
}
#endif  /* FWUP_IMPLEMENTATION_BOOTLOADER */
