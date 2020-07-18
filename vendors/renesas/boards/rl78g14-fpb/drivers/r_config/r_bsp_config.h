
#ifndef R_BSP_CONFIG_H
#define R_BSP_CONFIG_H

#include "r_cg_macrodriver.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* By default FIT modules will check input parameters to be valid. This is helpful during development but some users
   will want to disable this for production code. The reason for this would be to save execution time and code space.
   This macro is a global setting for enabling or disabling parameter checking. Each FIT module will also have its
   own local macro for this same purpose. By default the local macros will take the global value from here though
   they can be overridden. Therefore, the local setting has priority over this global setting. Disabling parameter
   checking should only used when inputs are known to be good and the increase in speed or decrease in code space is
   needed.
   0 = Global setting for parameter checking is disabled.
   1 = Global setting for parameter checking is enabled (Default).
*/
#define BSP_CFG_PARAM_CHECKING_ENABLE               ( 1 )

#define R_BSP_NOP                                   NOP

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
/* Available delay units. */
typedef enum
{
//    BSP_DELAY_MICROSECS = 1000000L, // Requested delay amount is in microseconds
    BSP_DELAY_MILLISECS = 1000,     // Requested delay amount is in milliseconds
    BSP_DELAY_SECS = 1              // Requested delay amount is in seconds
} bsp_delay_units_t;

#endif /* R_BSP_CONFIG_H */

