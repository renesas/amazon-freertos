/***********************************************************************
*
*  FILE        : main.c
*  DATE        : 2020-03-19
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include <stdio.h>
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
#include "flash_api.h"
#include "r_scifa_drv_api.h"

#include "base64_decode.h"
#include "code_signer_public_key.h"

/* tinycrypto */
#include "sha256.h"

#include "r_cache_lld_rza2m.h"
#include "r_cache_l1_rza2m_asm.h"
#include "ecc_dsa.h"
#include "r_mmu_lld_rza2m.h"

#define     __DSB()  asm volatile ("DSB")

/*------------------------------------------ firmware update configuration (start) --------------------------------------------*/

static uint8_t hyper[0x800000] __attribute((section("HYPER_RAM")));

#define BOOT_LOADER_IMAGE_SIZE_BOT_ADR 0x218
#define BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH 0x200
#define BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH 0x100
#define INITIAL_FIRMWARE_FILE_NAME "userprog.rsu"
#define SF_SECTOR_SIZE	0x1000

static uint32_t downloaded_image_size = BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH + BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH;
static uint32_t image_size = BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH;

#define BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS 	(0x20A00000)	// 更新アプリケーション領域 (temporary area) の先頭アドレス			// RZ/A2M OTA 2020.03.19 //
#define BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS		(0x20200000)	// 実行アプリケーション領域 (exec area) の先頭アドレス				// RZ/A2M OTA 2020.03.19 //
#define USER_RESET_VECTOR_ADDRESS (0x20200300)	// 0x300まで.rsuファイルのヘッダー部分が入るため このアドレスに移動		// RZ/A2M OTA 2020.03.19 //

#define BOOT_LOADER_USER_FIRMWARE_MAXSIZE (0x800000 - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH)

#define BOOT_LOADER_SUCCESS         (0)
#define BOOT_LOADER_FAIL            (-1)
#define BOOT_LOADER_GOTO_INSTALL    (-2)
#define BOOT_LOADER_IN_PROGRESS     (-3)

#define BOOT_LOADER_STATE_INITIALIZING								1
#define BOOT_LOADER_STATE_BANK1_CHECK								2
#define BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE		4
#define BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE		6
#define BOOT_LOADER_STATE_BANK0_CHECK								7
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE	17
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT		18
#define BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE	21
#define BOOT_LOADER_STATE_BANK0_UPDATE_CHECK						26
#define BOOT_LOADER_STATE_BANK1_UPDATE_CODE_FLASH_ERASE_COMPLETE	28
#define BOOT_LOADER_STATE_FINALIZE									29
#define BOOT_LOADER_STATE_FATAL_ERROR								200

#define BOOT_LOADER_SCIFA_CONTROL_BLOCK_A (0)
#define BOOT_LOADER_SCIFA_CONTROL_BLOCK_B (1)
#define BOOT_LOADER_SCIFA_CONTROL_BLOCK_TOTAL_NUM (2)

#define BOOT_LOADER_SCIFA_RECEIVE_BUFFER_EMPTY (0)
#define BOOT_LOADER_SCIFA_RECEIVE_BUFFER_FULL  (1)

#define LIFECYCLE_STATE_BLANK		(0xff)
#define LIFECYCLE_STATE_TESTING		(0xfe)
#define LIFECYCLE_STATE_VALID		(0xfc)
#define LIFECYCLE_STATE_INVALID		(0xf8)

#define INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE "hash-sha256"
#define INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE "sig-sha256-ecdsa"

#define TC_SHA256_DIGEST_SIZE (32)

typedef struct _load_firmware_control_block {
    uint32_t flash_buffer[SF_SECTOR_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
}LOAD_FIRMWARE_CONTROL_BLOCK;

typedef struct _sci_buffer_control {
   uint8_t buffer[SF_SECTOR_SIZE];
   uint32_t buffer_occupied_byte_size;
   uint32_t buffer_full_flag;
}SCI_BUFFER_CONTROL;

typedef struct _sci_receive_control_block {
   SCI_BUFFER_CONTROL * p_sci_buffer_control;
   uint32_t total_byte_size;
   uint32_t current_state;
}SCI_RECEIVE_CONTROL_BLOCK;

typedef struct _firmware_update_control_block
{
	uint8_t magic_code[7];
    uint8_t image_flag;
    uint8_t signature_type[32];
    uint32_t signature_size;
    uint8_t signature[256];
    uint32_t dataflash_flag;
    uint32_t dataflash_start_address;
    uint32_t dataflash_end_address;
    uint8_t reserved1[200];
    uint32_t sequence_number;
    uint32_t start_address;
    uint32_t end_address;
    uint32_t execution_address;
    uint32_t hardware_id;
    uint32_t image_size;	// RZ/A2M OTA 2020.03.19 //
//    uint8_t reserved2[236];	// RZ/A2M OTA 2020.03.19 // 
    uint8_t reserved2[232];
}FIRMWARE_UPDATE_CONTROL_BLOCK;

static int32_t secure_boot(void);
static void bank_swap_with_software_reset(void);
static void software_reset(void);
static const uint8_t *get_status_string(uint8_t status);

static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank0 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS;
static FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_bank1 = (FIRMWARE_UPDATE_CONTROL_BLOCK*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;
static LOAD_FIRMWARE_CONTROL_BLOCK load_firmware_control_block;
static uint32_t secure_boot_state = BOOT_LOADER_STATE_INITIALIZING;

static int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize);
const uint8_t code_signer_public_key[] = CODE_SIGNENR_PUBLIC_KEY_PEM;
const uint32_t code_signer_public_key_length = sizeof(code_signer_public_key);


#define MAIN_PRV_LED_ON     (1)
#define MAIN_PRV_LED_OFF    (0)
static uint32_t gs_main_led_flg;      /* LED lighting/turning off */
static int_t gs_my_gpio_handle;
static st_r_drv_gpio_pin_rw_t gs_p60_hi =
{
    GPIO_PORT_6_PIN_0,
    GPIO_LEVEL_HIGH,
    GPIO_SUCCESS
};
static st_r_drv_gpio_pin_rw_t gs_p60_lo =
{
    GPIO_PORT_6_PIN_0,
    GPIO_LEVEL_LOW,
    GPIO_SUCCESS
};
static const r_gpio_port_pin_t gs_led_pin_list[] =
{
    GPIO_PORT_6_PIN_0,
};


int_t main( void )
{
    int32_t result_secure_boot;

    int_t err;
    st_r_drv_gpio_pin_list_t pin_led;

    if (!R_OS_AbstractionLayerInit())
    {
        /* stop execution */
            while (true)
        {
            /* Spin here forever.. */
            R_COMPILER_Nop();
        }
    }

    while(1)
    {
    	result_secure_boot = secure_boot();
		if (BOOT_LOADER_SUCCESS == result_secure_boot)
		{
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

			/* address jump */	// RZ/A2M OTA 2020.03.19 //
			((void(*)(void))USER_RESET_VECTOR_ADDRESS)();

			while(1); /* infinite loop */
		}
		else if (BOOT_LOADER_FAIL == result_secure_boot)
		{
			while(1)
			{
				/* infinity loop */
			}
		}
		else if (BOOT_LOADER_IN_PROGRESS == result_secure_boot)
		{
			continue;
		}
		else
		{
			while(1)
			{
				/* infinite loop */
			}
		}
    }

    return 0;
}


static int32_t secure_boot(void)
{
	int_t scifa_handle;
	scifa_config_t my_scifa_config;
	int32_t secure_boot_error_code = BOOT_LOADER_IN_PROGRESS;
	int32_t flash_api_error_code = 0;
	int32_t fl_ret;
	FIRMWARE_UPDATE_CONTROL_BLOCK *firmware_update_control_block_tmp = (FIRMWARE_UPDATE_CONTROL_BLOCK*)load_firmware_control_block.flash_buffer;
	int32_t verification_result = -1;

	uint32_t i;

    uint32_t k, cnt, cntcnt;

 	 /* exec area と temporary area の Image Flags を初期化。 新しく実行アプリケーションを書き込む時に使用。 */	// RZ/A2M OTA 2020.03.19 //
/*
  	flash_erase_sector(NULL, BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS);
  	printf("exec area erase complete.\r\n");
  	flash_erase_sector(NULL, BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS);
    printf("temporary area erase complete.\r\n");
*/
	switch(secure_boot_state)
	{
		case BOOT_LOADER_STATE_INITIALIZING:
			/* UART経由のアプリケーションダウンロードに SCIFA4を使用 */	// RZ/A2M OTA 2020.03.19 //
			scifa_handle = direct_open("scifa4", 0);
			direct_control(scifa_handle, CTL_SCIFA_GET_CONFIGURATION, &my_scifa_config);

			my_scifa_config.baud_rate = 115200;

			direct_control(scifa_handle, CTL_SCIFA_SET_CONFIGURATION, &my_scifa_config);

			/* startup system */
			printf("-------------------------------------------------\r\n");
			printf("RZ/A2M secure boot program\r\n");
			printf("-------------------------------------------------\r\n");

    	    printf("Checking flash ROM status.\r\n");

    	    printf("bank 0 status = 0x%x [%s]\r\n", firmware_update_control_block_bank0->image_flag, get_status_string(firmware_update_control_block_bank0->image_flag));
			printf("bank 1 status = 0x%x [%s]\r\n", firmware_update_control_block_bank1->image_flag, get_status_string(firmware_update_control_block_bank1->image_flag));

			secure_boot_state = BOOT_LOADER_STATE_BANK1_CHECK;
			break;

		/* temporary area check */	// RZ/A2M OTA 2020.03.19 //
		case BOOT_LOADER_STATE_BANK1_CHECK:
			if(firmware_update_control_block_bank1->image_flag == LIFECYCLE_STATE_TESTING)
			{
				// RZ/A2M OTA 2020.03.19 // -->>
				image_size = firmware_update_control_block_bank1->image_size;
				if(image_size > BOOT_LOADER_USER_FIRMWARE_MAXSIZE)
				{
					image_size = BOOT_LOADER_USER_FIRMWARE_MAXSIZE;
				}
				// RZ/A2M OTA 2020.03.19 // <<--
				memcpy(load_firmware_control_block.flash_buffer, (void*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS, SF_SECTOR_SIZE);
    	    	printf("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
				printf("bank1(temporary area) on code flash integrity check...");

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
    	            printf("OK\r\n");
    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_VALID;
    	        }
    	        else
    	        {
    	            printf("NG\r\n");
    	        	firmware_update_control_block_tmp->image_flag = LIFECYCLE_STATE_INVALID;
    	        }
    	    	printf("update LIFECYCLE_STATE from [%s] to [%s]\r\n", get_status_string(firmware_update_control_block_bank1->image_flag), get_status_string(firmware_update_control_block_tmp->image_flag));
    	    	printf("bank1(temporary area) block0 erase (to update LIFECYCLE_STATE)...");

    	        /* 1 sector erase */	// RZ/A2M OTA 2020.03.19 //
    	        fl_ret = flash_erase_sector(NULL, BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS);
    	        if(fl_ret == -1)
    	        {
    	            printf("flash_erase_sector() returns error.\r\n");
    	            printf("system error.\r\n");
					secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
					secure_boot_error_code = BOOT_LOADER_FAIL;
    	            break;
    	        }
    	        else
    	        {
    	        	secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE;
    	        }
    		}
    		else
    		{
				if (firmware_update_control_block_bank0->image_flag == LIFECYCLE_STATE_VALID)
				{
					secure_boot_state = BOOT_LOADER_STATE_BANK0_UPDATE_CHECK;
	    		}
	    		else
	    		{
	    			secure_boot_state = BOOT_LOADER_STATE_BANK0_CHECK;
				}
			}
			break;

		case BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_ERASE_COMPLETE:
	        printf("bank1(temporary area) block0 write (to update LIFECYCLE_STATE)...");
			fl_ret = flash_program_page(NULL,BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,firmware_update_control_block_tmp,SF_SECTOR_SIZE);
			if(fl_ret == -1)
			{
				printf("flash_program_page() returns error.\r\n");
				printf("system error.\r\n");
				secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
				secure_boot_error_code = BOOT_LOADER_FAIL;
				break;
			}
			secure_boot_state = BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE;
			break;

		case BOOT_LOADER_STATE_BANK1_UPDATE_LIFECYCLE_WRITE_COMPLETE:
			// RZ/A2M OTA 2020.03.19 // -->>
			// 更新アプリケーション(bank1)のフラグがVALID以外の時はデータ異常のため、
			// RXではバンクスワップしてbank0(元のbank1)のフラグが異常、
			// 再度スワップして元のbank0に戻るようになっている。
			// RZ/A2Mではバンクスワップするとbank1のアプリケーションは削除されるため、
			// 元のbank0に戻すような処理はできない。
			// このため、bank1のフラグがVALID以外の場合はスワップせず、
			// bank1 のフラグを 0xff にして無効にする処置をし、そのまま bank0 で動作するようにする。
			if((firmware_update_control_block_bank1->image_flag) == LIFECYCLE_STATE_VALID)
			{
		        printf("swap bank...\r\n");
	//			R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
				R_SoftwareDelay(7500000);	//3s wait
				/* バンクスワップ：ソフトの入れ替え処理で実施 */
				bank_swap_with_software_reset();
			}
			else
			{
    	        printf("illegal status\r\n");
    	        printf("not swap bank...");
   	            flash_erase_sector(NULL, BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS);

//   	            R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
				R_SoftwareDelay(7500000);	//3s wait
				software_reset();
			}
			while(1);
			break;
			// RZ/A2M OTA 2020.03.19 //  <<--


		/* exec area check */	// RZ/A2M OTA 2020.03.19 //
		case BOOT_LOADER_STATE_BANK0_CHECK:
		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE:
		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT:
		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
		case BOOT_LOADER_STATE_BANK0_UPDATE_CHECK:
		case BOOT_LOADER_STATE_FINALIZE:
			switch(firmware_update_control_block_bank0->image_flag)
			{
				case LIFECYCLE_STATE_BLANK:
    	        	switch(secure_boot_state)
					{
    	        		case BOOT_LOADER_STATE_BANK0_CHECK:
							printf("start installing user program.\r\n");
							printf("========== install user program phase ==========\r\n");

			    	        secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE;
			    	        break;
    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_ERASE_COMPLETE:
    	        	        printf("send \"%s\" via UART.\r\n", INITIAL_FIRMWARE_FILE_NAME);
							secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT;
    	        	        break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_READ_WAIT:
							/* install code flash area */
							// RZ/A2M OTA 2020.03.19 //  -->>
	                        cnt = 0;
    	                    cntcnt = 0;
        	                for (k=0; k < downloaded_image_size; k++)
	                        {
	                            direct_read(scifa_handle, &hyper[k], 1);
	                            cntcnt ++;
	                            if (cntcnt >= 0x1000)
	                            {
	                                printf("downloaded:0x%08x\n\r", k);
	                                cntcnt = 0;
	                            }
		                        if( k == BOOT_LOADER_IMAGE_SIZE_BOT_ADR )
		                        {
		                        	FIRMWARE_UPDATE_CONTROL_BLOCK* p_header = (FIRMWARE_UPDATE_CONTROL_BLOCK*)&hyper;
		                        	if( p_header->image_size > BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH
		                        	 && p_header->image_size <= BOOT_LOADER_USER_FIRMWARE_MAXSIZE )
	                                {
										image_size = p_header->image_size;
	                                	downloaded_image_size = image_size + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH;
	                                	printf("DownLoad Image Size:0x%08x\n\r", downloaded_image_size);
	                                }
	                            }
	                        }

							if(downloaded_image_size == (BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH + BOOT_LOADER_USER_FIRMWARE_DESCRIPTOR_LENGTH))
							{
				    	        printf("Image Size error.\r\n");
								secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
								secure_boot_error_code = BOOT_LOADER_FAIL;
//								software_reset();
				    	        break;
							}

							direct_close(scifa_handle);
							// RZ/A2M OTA 2020.03.19 //  <<--
						
							uint8_t* pflash = (uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS;

							/* temporary area clear */	// RZ/A2M OTA 2020.03.19 //
							for (i = 0; i < downloaded_image_size; i+=SF_SECTOR_SIZE)
							{
								flash_erase_sector(NULL, BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + i);
							}

							/* Hyper RAM -> temporary area write*/	// RZ/A2M OTA 2020.03.19 //
							fl_ret = flash_program_page(NULL,BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS,hyper,downloaded_image_size);

							if(fl_ret == -1)
							{
								printf("flash_program_page() returns error.\r\n");
								printf("system error.\r\n");
								secure_boot_error_code = BOOT_LOADER_FAIL;
								break;
							}
							secure_boot_state = BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE;
							break;

    	        		case BOOT_LOADER_STATE_BANK0_INSTALL_CODE_FLASH_WRITE_COMPLETE:
							printf("\n");
							printf("completed installing firmware.\r\n");
			    	    	printf("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
							printf("bank1(temporary area) on code flash integrity check...");

							/* Firmware verification for the signature type. */
							if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
							{
								uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
							    /* Hash message */
							    struct tc_sha256_state_struct xCtx;
							    tc_sha256_init(&xCtx);
							    tc_sha256_update(&xCtx,
							    		(uint8_t*)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
//										APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);	// RZ/A2M OTA 2020.03.19 // 
										image_size);	// RZ/A2M OTA 2020.03.19 // 
							    tc_sha256_final(hash_sha256, &xCtx);
					   	        verification_result = memcmp(firmware_update_control_block_bank1->signature, hash_sha256, sizeof(hash_sha256));
					   	    }
					   	    else if (!strcmp((const char *)firmware_update_control_block_bank1->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
					   	    {
								verification_result = firmware_verification_sha256_ecdsa(
																	(const uint8_t *)BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
//																	APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,	// RZ/A2M OTA 2020.03.19 // 
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
								printf("OK\r\n");
								printf("completed installing const data.\r\n");
								printf("software reset...\r\n");
//								R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);	// RZ/A2M OTA 2020.03.19 //
								R_SoftwareDelay(7500000);	//3s wait // RZ/A2M OTA 2020.03.19 //
								software_reset();
							}
							else
							{
								printf("NG\r\n");
								printf("fatal error occurred.\r\n");
	    	        			secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
	    	        			secure_boot_error_code = BOOT_LOADER_FAIL;
							}
							break;
						}
    				break;
				case LIFECYCLE_STATE_TESTING:
    	            printf("illegal status\r\n");
    	            printf("not swap bank...");
					// RZ/A2M OTA 2020.03.19 //  -->>
					// 実行アプリケーション(bank0)のフラグがTESTINGの時はデータ異常のため、
					// RXではバンクスワップしてbank1のアプリケーションに戻している。
					// RZ/A2Mではbank1のアプリケーションはすでに削除されている状態であるため、
					// bank0 の状態を 0xff にして無効にする処置をする。
					// すると、両方 blank状態になるため、初期ファームウェアをインストール要求が行われる。
    	            flash_erase_sector(NULL, BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS);

//   	            R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
                    R_SoftwareDelay(7500000);	//3s wait
					software_reset();
//   	            bank_swap_with_software_reset();
					// RZ/A2M OTA 2020.03.19 //  <<--
    	            while(1);
    	            break;
				case LIFECYCLE_STATE_VALID:
					switch(secure_boot_state)
					{
						case BOOT_LOADER_STATE_BANK0_UPDATE_CHECK:
			    	    	printf("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank0->signature_type);
		    	            printf("bank0(execute area) on code flash integrity check...");
							// RZ/A2M OTA 2020.03.19 //  -->>
		    	            image_size = firmware_update_control_block_bank0->image_size;
							if(image_size > BOOT_LOADER_USER_FIRMWARE_MAXSIZE)
							{
								image_size = BOOT_LOADER_USER_FIRMWARE_MAXSIZE;
							}
							// RZ/A2M OTA 2020.03.19 //  <<--
							/* Firmware verification for the signature type. */
							if (!strcmp((const char *)firmware_update_control_block_bank0->signature_type, INTEGRITY_CHECK_SCHEME_HASH_SHA256_STANDALONE))
							{
							    /* Hash message */
								uint8_t hash_sha256[TC_SHA256_DIGEST_SIZE];
							    struct tc_sha256_state_struct xCtx;
							    tc_sha256_init(&xCtx);
							    tc_sha256_update(&xCtx,
							    		(uint8_t*)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
//										APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH);	// RZ/A2M OTA 2020.03.19 // 
										image_size);	// RZ/A2M OTA 2020.03.19 // 
							    tc_sha256_final(hash_sha256, &xCtx);
				    	        verification_result = memcmp(firmware_update_control_block_bank0->signature, hash_sha256, sizeof(hash_sha256));
				    	    }
				    	    else if (!strcmp((const char *)firmware_update_control_block_bank0->signature_type, INTEGRITY_CHECK_SCHEME_SIG_SHA256_ECDSA_STANDALONE))
				    	    {
								verification_result = firmware_verification_sha256_ecdsa(
																	(const uint8_t *)BOOT_LOADER_UPDATE_EXECUTE_AREA_LOW_ADDRESS + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,
//																	APP_SIZE - BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH,	// RZ/A2M OTA 2020.03.19 // 
																	image_size,	// RZ/A2M OTA 2020.03.19 // 
																	firmware_update_control_block_bank0->signature,
																	firmware_update_control_block_bank0->signature_size);
							}
							else
							{
								verification_result = -1;
							}

							if(0 == verification_result)
		    	            {
		    	                printf("OK\r\n");
		    	                secure_boot_state = BOOT_LOADER_STATE_FINALIZE;
		    	            }
		    	            else
		    	            {
		    					printf("NG.\r\n");
		    					printf("Code flash is completely broken.\r\n");
		    					printf("Please erase all code flash.\r\n");
		    					printf("And, write secure boot using debugger.\r\n");
								secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
		    					secure_boot_error_code = BOOT_LOADER_FAIL;
		    	            }
		    	            break;

		    	        case BOOT_LOADER_STATE_FINALIZE:
	    	                printf("jump to user program\r\n");
//	    	                R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);	// RZ/A2M OTA 2020.03.19 //
							R_SoftwareDelay(2500000);	//1s wait // RZ/A2M OTA 2020.03.19 //
	
	    	                secure_boot_error_code = BOOT_LOADER_SUCCESS;
		    	        	break;
		    	    }
   	            	break;

				default:
    	            printf("illegal flash rom status code 0x%x.\r\n", firmware_update_control_block_bank0->image_flag);
        	    	printf("integrity check scheme = %-.32s\r\n", firmware_update_control_block_bank1->signature_type);
    	            printf("bank1(temporary area) on code flash integrity check...");

    	            secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
		    		secure_boot_error_code = BOOT_LOADER_FAIL;
					break;
			}
	}
    return secure_boot_error_code;
}

static void software_reset(void)
{
	volatile uint16_t data;
	WDT.WTCNT.WORD = 0x5A00;
	data = WDT.WRCSR.WORD;
	WDT.WTCNT.WORD = 0x5A00;
	WDT.WRCSR.WORD = 0xA500;
	WDT.WTCSR.WORD = 0xA578;
	WDT.WRCSR.WORD = 0x5A40;
	while(1){}
}


/* バンクスワップ：ソフト処理で領域の入れ替えを行う */	// RZ/A2M OTA 2020.03.19 //
static void bank_swap_with_software_reset(void)
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
		secure_boot_state = BOOT_LOADER_STATE_FATAL_ERROR;
	}
	else
	{
	    /* temporary area clear */	// RZ/A2M OTA 2020.03.19 //
	    for (i = 0; i < (image_size + BOOT_LOADER_USER_FIRMWARE_HEADER_LENGTH); i+=SF_SECTOR_SIZE)
	    {
	    	flash_erase_sector(NULL, BOOT_LOADER_UPDATE_TEMPORARY_AREA_LOW_ADDRESS + i);
	    }
	}
    software_reset();
}



static const uint8_t *get_status_string(uint8_t status)
{
	static const uint8_t status_string[][32] = {{"LIFECYCLE_STATE_BLANK"},
	                                            {"LIFECYCLE_STATE_TESTING"},
	                                            {"LIFECYCLE_STATE_VALID"},
	                                            {"LIFECYCLE_STATE_INVALID"},
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
	else if(status == LIFECYCLE_STATE_VALID)
	{
		tmp = status_string[2];
	}
	else if(status == LIFECYCLE_STATE_INVALID)
	{
		tmp = status_string[3];
	}
	else
	{
		tmp = status_string[4];
	}
	return tmp;
}

static int32_t firmware_verification_sha256_ecdsa(const uint8_t * pucData, uint32_t ulSize, const uint8_t * pucSignature, uint32_t ulSignatureSize)
{
    int32_t xResult = -1;
    uint8_t pucHash[TC_SHA256_DIGEST_SIZE];
    uint8_t data_length;
    uint8_t public_key[64];
    uint8_t binary[256];
    uint8_t *head_pointer, *current_pointer, *tail_pointer;;

    /* Hash message */
    struct tc_sha256_state_struct xCtx;
    tc_sha256_init(&xCtx);
    tc_sha256_update(&xCtx, pucData, ulSize);
    tc_sha256_final(pucHash, &xCtx);

    /* extract public key from code_signer_public_key (pem format) */
    head_pointer = (uint8_t*)strstr((char *)code_signer_public_key, "-----BEGIN PUBLIC KEY-----");
    if(head_pointer)
    {
    	head_pointer += strlen("-----BEGIN PUBLIC KEY-----");
        tail_pointer = (uint8_t*)strstr((char *)code_signer_public_key, "-----END PUBLIC KEY-----");
    	base64_decode(head_pointer, binary, tail_pointer - head_pointer);
    	current_pointer = binary;
		data_length = *(current_pointer + 1);
    	while(1)
    	{
    		switch(*current_pointer)
    		{
    			case 0x30: /* found "SEQUENCE" */
    				current_pointer += 2;
    				break;
    			case 0x03: /* found BIT STRING (maybe public key) */
        			if(*(current_pointer + 1) == 0x42)
        			{
        				memcpy(public_key, current_pointer + 4, 64);
						/* Verify signature */
						if(uECC_verify(public_key, pucHash, TC_SHA256_DIGEST_SIZE, pucSignature, uECC_secp256r1()))
						{
							xResult = 0;
						}
        			}
    				current_pointer += *(current_pointer + 1) + 2;
					break;
    			default:
    				current_pointer += *(current_pointer + 1) + 2;
    				break;
    		}
			if((current_pointer - binary) > data_length)
			{
				/* parsing error */
				break;
			}
    	}
    }
    return xResult;
}

/**********************************************************************************************************************
 * Function Name: Sample_LED_Blink
 * Description  : This function is executed when the OSTM0 interrupt is received.
 *              : In this sample code, the processing to blink the LEDs on the CPU board every 500ms is executed.
 * Arguments    : uint32_t int_sense : Interrupt detection
 *              :                    :   INTC_LEVEL_SENSITIVE : Level sense
 *              :                    :   INTC_EDGE_TRIGGER    : Edge trigger
 * Return Value : none
 *********************************************************************************************************************/
void Sample_LED_Blink(uint32_t int_sense)
{
    /* int_sense not used */
    UNUSED_PARAM(int_sense);

    /* ==== LED blink ==== */
    gs_main_led_flg ^= 1;

    if (MAIN_PRV_LED_ON == gs_main_led_flg)
    {
        direct_control(gs_my_gpio_handle, CTL_GPIO_PIN_WRITE, &gs_p60_hi);
    }
    else
    {
        direct_control(gs_my_gpio_handle, CTL_GPIO_PIN_WRITE, &gs_p60_lo);
    }
}
/**********************************************************************************************************************
 * End of function Sample_LED_Blink
 *********************************************************************************************************************/

/* End of File */


