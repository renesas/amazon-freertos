
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>
#include <stdbool.h>

#include "r_bsp_config.h"
#include "r_cg_userdefine.h"

uint32_t R_BSP_CpuInterruptLevelRead (void);

bool R_BSP_CpuInterruptLevelWrite (uint32_t level);

bool R_BSP_SoftwareDelay(uint32_t delay, bsp_delay_units_t units);

#endif /* PLATFORM_H */

