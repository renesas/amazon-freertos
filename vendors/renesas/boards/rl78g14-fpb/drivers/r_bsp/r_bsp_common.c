/*
 * r_bsp_common.c
 *
 *  Created on: 2020/03/24
 *      Author: hmU11282
 */

#include "FreeRTOS.h"
#include "task.h"

#include "rl_platform.h"
#include "r_bsp_config.h"

uint32_t R_BSP_CpuInterruptLevelRead (void) {
	return 0;
}

bool R_BSP_CpuInterruptLevelWrite (uint32_t level) {
	return true;
}

bool R_BSP_SoftwareDelay(uint32_t delay, bsp_delay_units_t units) {

	if (BSP_DELAY_MILLISECS == units) {
		vTaskDelay( delay );
	}
	if (BSP_DELAY_SECS == units) {
		vTaskDelay( delay * 1000 );
	}

	return true;
}
