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

/**********************************************************************************************************************
 * File Name    : r_os_abstraction.c
 * Version      : see OS_LESS_RZ_HLD_VERSION_MAJOR.OS_LESS_RZ_HLD_VERSION_MINOR
 * Description  : OS-Less OS Abstraction Layer
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * History      : DD.MM.YYYY  Description
 *              : 02.05.2019  Fixed R_OS_SemaphoreWait issue with zero timeout
 *********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "r_typedefs.h"
#include "r_os_abstraction_typedef.h"
#include "r_devlink_wrapper.h"
#include "r_ostm_drv_api.h"

/* compiler specific API header */
#include "r_compiler_abstraction_api.h"

/* OS abstraction specific API header */
#include "r_os_abstraction_api.h"

#define R_OS_PRV_CONFIG_UNKNOWN           (0)

/* A minimum of 8 characters or "Unknown" will not fit! */
#define R_OS_PRV_TINY_STACK_SIZE          (R_OS_PRV_CONFIG_UNKNOWN)
#define R_OS_PRV_SMALL_STACK_SIZE         (R_OS_PRV_CONFIG_UNKNOWN)
#define R_OS_PRV_DEFAULT_STACK_SIZE       (R_OS_PRV_CONFIG_UNKNOWN)
#define R_OS_PRV_LARGE_STACK_SIZE         (R_OS_PRV_CONFIG_UNKNOWN)
#define R_OS_PRV_HUGE_STACK_SIZE          (R_OS_PRV_CONFIG_UNKNOWN)

#define R_OS_PRV_DEFAULT_HEAP             (R_OS_PRV_CONFIG_UNKNOWN)

#define R_OS_PRV_OSTM_INTERRUPT_PRIORITY  (5)

#define R_OSFREE_MUTEX_SET_PRV_           (1)
#define R_OSFREE_MUTEX_RESET_PRV_         (0)

/**********************************************************************************************************************
 * Function Macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Typedefs
 *********************************************************************************************************************/
typedef struct
{
    uint8_t  *p_queue_buffer;
    uint32_t max_elements;
    uint_t   element_size;
    uint_t   in_index;
    uint_t   out_index;
    bool_t   buffer_full;
} st_message_queue_t;

typedef struct
{
    uint32_t max_count;
    uint32_t count;
} st_semaphore_block_t;

typedef st_semaphore_block_t *p_st_semaphore_block_t;

static const char s_startup_task_name_str[] = "os-less";
static const st_drv_info_t s_os_version =
{
    { ((R_OS_ABSTRACTION_VERSION_MAJOR << 16) + R_OS_ABSTRACTION_VERSION_MINOR) },
    R_OS_ABSTRACTION_BUILD_NUM,
    (s_startup_task_name_str) };

static bool_t s_devlink_system_init = false; /* used to determine if devlink system has been initialised */
static char s_pc_file[200];
static uint32_t s_ul_line;
static int_t s_hwtimer_handle = ( -1);
static volatile uint32_t s_interrupt_counter = 0u;

static uint32_t get_timeout_tickcount (uint32_t sleep_ms);

/* extern to syscalls.c */
extern void initialise_monitor_handles (void);

/* Functions available to other modules */
void os_abstraction_isr (void);

/**********************************************************************************************************************
 * Function Name: R_OS_AssertCalled
 * Description  : Generic error Handler
 * Arguments    : p_file - Source code file where the error occurred.
 *                line - Source code line where the error occurred.
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_AssertCalled (const char *p_file, uint32_t line)
{
    volatile uint32_t ul = 0;

    /* unused variable */
    strcpy(s_pc_file, (const char *) p_file);

    /* unused variable */
    s_ul_line = line;

    /* Set ul to a non-zero value using the debugger to step out of this function */
    while (0 == ul)
    {
        R_COMPILER_Nop();
    }
}
/**********************************************************************************************************************
 End of function R_OS_AssertCalled
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_AbstractionLayerInit
 * Description  : Function to configure critical resources for the connected
 *                OS or scheduler.
 * Arguments    : void
 * Return Value : true if there were no errors when initialising the Layer.
 *                false if there errors when initialising the Layer.
 *********************************************************************************************************************/
bool_t R_OS_AbstractionLayerInit (void)
{
    bool_t ready = true;

    if (false == s_devlink_system_init)
    {
        s_devlink_system_init = true;

        /* Initialize the devlink layer */
        R_DEVLINK_Init();

        /* setup standard file descriptors */
        initialise_monitor_handles();
    }

    /* Don't re-open driver if R_OS_AbstractionLayerInit() has already been called */
    if (( -1) == s_hwtimer_handle)
    {
        s_hwtimer_handle = open(R_OS_ABSTRACTION_OSTM_RESOURCE, O_RDWR);
    }

    /* Check that resource can be opened
     * Report system is NOT READY if check fails
     * If the driver can not be opened check the following:
     *     [1] The OSTM driver is included in your project
     *     [2] One OSTM channel is configured with the name defined in R_OS_ABSTRACTION_OSTM_RESOURCE
     *     [3] The configured channel has interrupt enabled and call function is os_abstraction_isr
     */
    if (( -1) == s_hwtimer_handle)
    {
        /* Hardware Timer can not opened, do not start the application */
        ready = false;
    }
    else
    {
        int_t result = 0;

        /* Needs feature normalising user clock frequency to 1ms timer expected by abstraction layer */
        /* REF needs CTRL_OSTM_RECONFIGURE in OSTM module */

        /* Start timer */
        result = control(s_hwtimer_handle, CTRL_OSTM_START_TIMER, NULL);

        /* check to see that operation was successful
         * Report system is NOT READY if check fails
         *
         */
        if (( -1) == result)
        {
            /* Hardware Timer can not start, do not start the application */
            ready = false;
        }
    }

    return (ready);
}
/**********************************************************************************************************************
 End of function R_OS_AbstractionLayerInit
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_AbstractionLayerShutdown
 * Description  : Function to release critical resources for the connected
 *                OS or scheduler.
 * Arguments    : void
 * Return Value : true if there were no errors when closing the Layer.
 *                false if there errors when closing the Layer.
 *********************************************************************************************************************/
bool_t R_OS_AbstractionLayerShutdown (void)
{
    bool_t ready = true;

    if (( -1) != s_hwtimer_handle)
    {
        close(s_hwtimer_handle);

        /*reset the handle to allow abstraction layer to be re-started */
        s_hwtimer_handle = -1;
    }

    return (ready);
}
/**********************************************************************************************************************
 End of function R_OS_AbstractionLayerShutdown
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_KernelInit
 * Description  : Unsupported when using OS_LESS Abstraction Layer
 * Arguments    : void
 * Return Value : void
 *********************************************************************************************************************/
void R_OS_KernelInit (void)
{
    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);
}
/**********************************************************************************************************************
 End of function R_OS_KernelInit
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: R_OS_Running
 *          Used to determine if the OS has started
 * @retval OS running true or false
 *********************************************************************************************************************/
bool_t R_OS_Running(void)
{
    return (true);
}
/**********************************************************************************************************************
 End of function R_OS_IsOSRunning
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_KernelStart
 * Description  : Unsupported when using OS_LESS Abstraction Layer
 * Arguments    : void
 * Return Value : void
 *********************************************************************************************************************/
void R_OS_KernelStart (void)
{
    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);
}
/**********************************************************************************************************************
 End of function R_OS_KernelStart
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_KernelStop
 * Description  : Unsupported when using OS_LESS Abstraction Layer
 * Arguments    : void
 * Return Value : void
 *********************************************************************************************************************/
void R_OS_KernelStop (void)
{
    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);
}
/**********************************************************************************************************************
 End of function R_OS_KernelStop
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_InitMemManager
 * Description  : Unsupported when using OS_LESS Abstraction Layer
 * Arguments    : void
 * Return Value : void
 *********************************************************************************************************************/
void R_OS_InitMemManager (void)
{
    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);
}
/******************************************************************************
 End of function R_OS_InitMemManager
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskCreate
 * Description  : Unsupported when using OS_LESS Abstraction Layer
 * Arguments    : void
 * Return Value : void
 *********************************************************************************************************************/
os_task_t *R_OS_TaskCreate (const char_t *name, os_task_code_t task_code, void *params, size_t stack_size,
        int_t priority)
{
    /* handle unused function parameters */
    UNUSED_PARAM(name);
    UNUSED_PARAM(task_code);
    UNUSED_PARAM(params);
    UNUSED_PARAM(stack_size);
    UNUSED_PARAM(priority);

    /* set to NULL as good practice */
    os_task_t *p_new_task = (os_task_t *) NULL;

    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);

    return (p_new_task);
}
/**********************************************************************************************************************
 End of function R_OS_TaskCreate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskDelete
 * Description  : Delete task
 * Arguments    : p_task - pointer to task, NULL deletes the current task
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_TaskDelete (os_task_t **p_task)
{
    /* handle unused function parameters */
    UNUSED_PARAM(p_task);

    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);
}
/**********************************************************************************************************************
 End of function R_OS_TaskDelete
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskSuspend
 * Description  : Suspend the specified task
 * Arguments    : task - pointer to task, NULL deletes the current task
 * Return Value : none
 *********************************************************************************************************************/
bool_t R_OS_TaskSuspend (os_task_t *p_task)
{
    /* handle unused function parameters */
    UNUSED_PARAM(p_task);
    bool_t ret = false;

    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);

    return (ret);
}
/**********************************************************************************************************************
 End of function R_OS_TaskSuspend
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskResume
 * Description  : Resume the task
 * Arguments    : task - pointer to task, NULL deletes the current task
 * Return Value : none
 *********************************************************************************************************************/
bool_t R_OS_TaskResume (os_task_t *p_task)
{
    /* handle unused function parameters */
    UNUSED_PARAM(p_task);
    bool_t ret = false;

    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);

    return (ret);
}
/**********************************************************************************************************************
 End of function R_OS_TaskResume
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_timeout_tickcount
 * Description  : determines the system tickcount value that will be in
 sleep_ms time
 * Arguments    : sleep_ms - sleep period in milliseconds
 * Return Value : uint32_t system tickcount in sleep_ms milliseconds time
 *********************************************************************************************************************/
static uint32_t get_timeout_tickcount (uint32_t sleep_ms)
{
    uint32_t normalised_delay = 0;
    uint32_t target_tickcount = 0;
    uint32_t assumed_timer_res = 1; /* ms*/

    /* Crude assumptions about configuration of ostm channel 'os_reserved'
     * [1] Timer is set to 1ms delay
     * [2] Any delay of less than 1ms is rounded up
     * [3] Any delay of greater than 1ms is rounded down to the nearest 1000ms
     */

    if (( -1) == s_hwtimer_handle)
    {
        /* Abort if R_OS_AbstractionLayerInit() was not previously called or timer 'os_reserved' is not open */
        R_OS_AssertCalled( __FILE__, __LINE__);
    }

    if (sleep_ms < assumed_timer_res)
    {
        normalised_delay = assumed_timer_res;
    }
    else
    {
        normalised_delay = sleep_ms / assumed_timer_res;
    }

    target_tickcount = s_interrupt_counter + normalised_delay;

    /* Special case, If calculated target_tickcount is 0xffffffff increment target_tickcount by 1
     * avoiding an issue where the delay of the specified duration could not be achieved */
    if (0xFFFFFFFF == target_tickcount)
    {
        target_tickcount++;
    }

    return (target_tickcount);
}
/**********************************************************************************************************************
 End of function get_timeout_tickcount
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskSleep
 * Description  : Suspend the task for the specified period of time
 * Arguments    : sleep_ms - sleep period in milliseconds
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_TaskSleep (uint32_t sleep_ms)
{
    uint32_t target_tickcount;

    /* get the system tick count appropriate for the sleep_ms argument */
    target_tickcount = get_timeout_tickcount(sleep_ms);

    /* Delay the task for the specified duration */
    do
    {
        R_COMPILER_Nop();
    } while (s_interrupt_counter < target_tickcount);
}
/**********************************************************************************************************************
 End of function R_OS_TaskSleep
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskYield
 * Description  : Force a context switch
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_TaskYield (void)
{
    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);
}
/**********************************************************************************************************************
 End of function R_OS_TaskYield
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TasksSuspendAll
 * Description  : Suspend all tasks, only attempted if the operating system is running
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_TasksSuspendAll (void)
{
    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);
}
/**********************************************************************************************************************
 End of function R_OS_TasksSuspendAll
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TasksResumeAll
 * Description  : Resume all tasks, only attempted if the operating system is running
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_TasksResumeAll (void)
{
    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);
}
/**********************************************************************************************************************
 End of function R_OS_TasksResumeAll
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskUsesFloatingPoint
 * Description  : Does nothing in OS Less environment
 * Arguments    : void
 * Return Value : void
 *********************************************************************************************************************/
void R_OS_TaskUsesFloatingPoint (void)
{
    /* Ignoring this in OS-LESS environment */
    R_COMPILER_Nop();
}
/**********************************************************************************************************************
 End of function R_OS_TaskUsesFloatingPoint
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TasksGetNumber
 * Description  : Not valid in OS-less environment
 * Arguments    : void
 * Return Value : uint32_t 0
 *********************************************************************************************************************/
uint32_t R_OS_TasksGetNumber (void)
{
    uint32_t ret = 0;

    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);

    return (ret);
}
/**********************************************************************************************************************
 End of function R_OS_TasksGetNumber
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_SysLock
 * Description  : Non-OS System lock
 * Arguments    : *p - object to lock. NULL argument will just disable interrupts.
 *                     otherwise a semaphore will be taken (& created if necessary)
 * Return Value : DRV_SUCCESS : locked
 *                DRV_ERROR   : An error occurred
 *********************************************************************************************************************/
int_t R_OS_SysLock (void)
{
    int32_t retval = DRV_SUCCESS;

    /* Disable the IRQ (interrupts are masked) */
    R_COMPILER_DisableInterrupts();

    return (retval);
}
/**********************************************************************************************************************
 End of function R_OS_SysLock
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_SysUnlock
 * Description  : Non OS System Unlock
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_SysUnlock (void)
{
    /* Enable the irq (interrupts are un-masked) */
    R_COMPILER_EnableInterrupts();
}
/**********************************************************************************************************************
 End of function R_OS_SysUnlock
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_SysWaitAccess
 * Description  : Not used in OS less environment.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_SysWaitAccess (void)
{
    /* Ignoring this in OS-LESS environment */
    R_COMPILER_Nop();
}
/**********************************************************************************************************************
 End of function R_OS_SysWaitAccess
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_SysReleaseAccess
 * Description  : Not used in OS less environment.
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_SysReleaseAccess (void)
{
    /* Ignoring this in OS-LESS environment */
    R_COMPILER_Nop();
}
/**********************************************************************************************************************
 End of function R_OS_SysReleaseAccess
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_Malloc
 * Description  : Not supported in non-OS environment
 * Arguments    : size, region
 * Return Value : NULL
 *********************************************************************************************************************/
void *R_OS_Malloc (size_t size, e_memory_region_t region)
{
    void *p_mem_to_alloc;

    /* handle unused function parameters */
    UNUSED_PARAM(region);

    /* Allocate a memory block */
    p_mem_to_alloc = malloc(size);

    /* Return a pointer to the newly allocated memory block */
    return (p_mem_to_alloc);
}
/**********************************************************************************************************************
 End of function R_OS_Malloc
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_Free
 * Description  : Free previously allocated memory
 * Arguments    : pp_memory_to_free
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_Free (void **pp_memory_to_free)
{
    /* Make sure the pointer, and the pointer that it points to are valid */
    if ((NULL != pp_memory_to_free) && (NULL != (*pp_memory_to_free)))
    {
        /* Free memory block */
        free(*pp_memory_to_free);

        /* clear pointer */
        *pp_memory_to_free = NULL;
    }
}
/**********************************************************************************************************************
 End of function R_OS_Free
 *********************************************************************************************************************/

/* Semaphore management */

/**********************************************************************************************************************
 * Function Name: R_OS_SemaphoreCreate
 * Description  : Create a semaphore
 * Arguments    : semaphore_ptr - Pointer to a associated semaphore
 *              : count         - The maximum count for the semaphore object. This value must be greater than zero
 * Return Value : The function returns TRUE if the semaphore object was successfully created
 *                Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_SemaphoreCreate (p_semaphore_t p_semaphore, uint32_t count)
{
    bool_t ret = false;

    /* temporary semaphore pointer */
    p_st_semaphore_block_t p_new_semaphore = NULL;

    /* check that semaphore pointer argument is valid */
    if (NULL != p_semaphore)
    {
        /* semaphore is uint32_t */
        p_new_semaphore = R_OS_Malloc(sizeof(st_semaphore_block_t), R_MEMORY_REGION_DEFAULT);

        /* Check semaphore memory is allocated */
        if (NULL != p_new_semaphore)
        {
            p_new_semaphore->max_count = count;
            p_new_semaphore->count = count;

            /* cast to end variable type of semaphore pointer */
            *p_semaphore = ((uint32_t ) p_new_semaphore);
            ret = true;
        }
    }

    return (ret);
}
/**********************************************************************************************************************
 End of function R_OS_SemaphoreCreate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_SemaphoreDelete
 * Description  : Delete a semaphore, freeing any associated resources
 * Arguments    : semaphore_ptr - Pointer to a associated semaphore
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_SemaphoreDelete (p_semaphore_t p_semaphore)
{
    /* Test for a null semaphore */
    if (NULL != (void *)( *p_semaphore))
    {
        /* Access to Semaphore element requires locking. */
        R_OS_SysLock();

        /* get handle to semaphore */
        p_st_semaphore_block_t p_semaphore_handle = (p_st_semaphore_block_t ) *p_semaphore;

        /* NULL semaphore pointer */
        *p_semaphore = (uint32_t)NULL;

        /* Free semaphore memory */
        R_OS_Free((void **) &p_semaphore_handle);

        /* Release lock */
        R_OS_SysUnlock();
    }
}
/**********************************************************************************************************************
 End of function R_OS_SemaphoreDelete
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_SemaphoreWait
 * Description  : Blocks operation until one of the following occurs:
 *              :   A timeout occurs
 *              :   The associated semaphore has been set
 *Important     : Calling R_OS_SemaphoreWait() inside an ISR can lock the system
 *                This is because the R_OS_TaskSleep function needs isr running to operate
 * Arguments    : p_semaphore - Pointer to an associated semaphore
 *              : timeout       - Maximum time to wait for associated event to occur
 * Return Value : The function returns TRUE if the semaphore object was successfully set. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_SemaphoreWait (p_semaphore_t p_semaphore, systime_t timeout)
{
    bool_t ret = false;

    /* cast to uint32_t */
    uint32_t ticks_to_wait = (uint32_t) timeout;

    /* NULL check */
    if (NULL != p_semaphore)
    {
        /* NULL check on semaphore */
        if (0 != ( *p_semaphore))
        {
            bool_t waiting_for_semaphore = true;

            /* Casted to semaphore block ptr type */
            volatile p_st_semaphore_block_t p_semaphore_handle = (p_st_semaphore_block_t ) *p_semaphore;

            do
            {
                /* Access to Semaphore element requires locking. */
                R_OS_SysLock();

                if (0 != p_semaphore_handle->count)
                {
                    /* semaphore available - get it */
                    p_semaphore_handle->count -= 1;

                    /* Release lock */
                    R_OS_SysUnlock();

                    ret = true;
                    waiting_for_semaphore = false;
                }
                else
                {
                    /* Release lock */
                    R_OS_SysUnlock();

                    /* semaphore not yet available */
                    if (0 == ticks_to_wait)
                    {
                        /* ready for exit */
                        waiting_for_semaphore = false;
                    }
                    else
                    {
                        /* wait as timer not yet expired */
                        R_OS_TaskSleep(1);
                    }

                    /* update timer if appropriate */
                    if (R_OS_ABSTRACTION_EV_WAIT_INFINITE != timeout)
                    {
                        ticks_to_wait--;
                    }
                }
            } while (waiting_for_semaphore);
        }
    }

    return (ret);
}
/**********************************************************************************************************************
 End of function R_OS_SemaphoreWait
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_SemaphoreRelease
 * Description  : Release a semaphore, freeing freeing it to be used by another task
 * Arguments    : semaphore_ptr - Pointer to a associated semaphore
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_SemaphoreRelease (p_semaphore_t p_semaphore)
{
    if (0 != ( *p_semaphore))
    {
        /* Access to Semaphore element requires locking. */
        R_OS_SysLock();

        /* get handle to semaphore */
        p_st_semaphore_block_t p_semaphore_handle = (p_st_semaphore_block_t ) *p_semaphore;

        if (p_semaphore_handle->max_count > p_semaphore_handle->count)
        {
            p_semaphore_handle->count += 1;
        }

        /* Release lock */
        R_OS_SysUnlock();
    }
}
/**********************************************************************************************************************
 End of function R_OS_SemaphoreRelease
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MutexCreate
 * Description  : Create a mutex
 * Arguments    : None
 * Return Value : mutex - ptr to object
 *********************************************************************************************************************/
void *R_OS_MutexCreate (void)
{
    /* pointer to mutex is NULL as a default */
    p_mutex_t p_mutex = (p_mutex_t) NULL;

    /* actual element of mutex is int_t */
    p_mutex = R_OS_Malloc(sizeof(int_t), R_MEMORY_REGION_DEFAULT);

    /* Initialise new mutex if malloc was successful*/
    if (NULL != p_mutex)
    {
        /* initialise mutex in reset state */
        *(int_t *) p_mutex = R_OSFREE_MUTEX_RESET_PRV_;
    }

    /* return pointer to mutex or NULL depending upon result */
    return ((void *) p_mutex);
}
/**********************************************************************************************************************
 End of function R_OS_MutexCreate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MutexDelete
 * Description  : Delete mutex
 * Arguments    : pp_mutex - ptr to mutex pointer
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_MutexDelete (pp_mutex_t pp_mutex)
{
    pp_mutex_t pp_temp = pp_mutex;

    /* NULL check */
    if ((NULL == pp_mutex) || (NULL == ( *pp_mutex)))
    {
        return;
    }

    /* Access to Mutex element requires locking. */
    R_OS_SysLock();

    /* free up memory and NULL the pointer */
    R_OS_Free((void **) pp_temp);

    /* Release lock */
    R_OS_SysUnlock();
}
/**********************************************************************************************************************
 End of function R_OS_MutexDelete
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MutexAcquire
 * Description  : Acquire the mutex, waiting indefinitely if need be
 * Arguments    : mutex - pointer to object
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_MutexAcquire (p_mutex_t p_mutex)
{
    /* The functionality of R_OS_MutexAcquire is identical to calling
     * R_OS_MutexWait with an infinite timeout                               */
    R_OS_MutexWait( &p_mutex, R_OS_ABSTRACTION_EV_WAIT_INFINITE);
}
/**********************************************************************************************************************
 End of function R_OS_MutexAcquire
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MutexRelease
 * Description  : This primitive function is specific for os_less applications
 * Arguments    : p_mutex - ptr to object
 * Return Value : none
 *********************************************************************************************************************/
void R_OS_MutexRelease (p_mutex_t p_mutex)
{
    /* NULL pointer check*/
    if (NULL == p_mutex)
    {
        return;
    }

    /* Access to Mutex element requires locking. */
    R_OS_SysLock();

    /* the mutex element is an int_t, so cast pointer */
    *(int_t *) p_mutex = R_OSFREE_MUTEX_RESET_PRV_;

    /* Release lock */
    R_OS_SysUnlock();
}
/**********************************************************************************************************************
 End of function R_OS_MutexRelease
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MutexWait
 * Description  : Wait for a mutex. The mutex is created if it's NULL
 * Arguments    : pp_mutex - object to lock
 *                time_out - wait time, maximum use R_OS_ABSTRACTION_EV_WAIT_INFINITE
 * Return Value : true if the mutex was acquired, false if not
 *********************************************************************************************************************/
bool_t R_OS_MutexWait (pp_mutex_t pp_mutex, uint32_t timeout_ms)
{
    bool_t ret = false;
    uint32_t ticks_to_wait = timeout_ms;

    /* NULL check */
    if ((NULL == pp_mutex) || (NULL == ( *pp_mutex)))
    {
        return (false);
    }

    bool_t waiting_for_mutex = true;

    do
    {
        /* Access to Mutex element requires locking. */
        R_OS_SysLock();

        /* the mutex element is an int_t, so cast pointer */
        if (R_OSFREE_MUTEX_RESET_PRV_ == ( *(int_t *) ( *pp_mutex)))
        {
            /* mutex available - get it */
            *(int_t *) ( *pp_mutex) = R_OSFREE_MUTEX_SET_PRV_;

            /* Release lock */
            R_OS_SysUnlock();

            ret = true;
            waiting_for_mutex = false;
        }
        else
        {
            /* Release lock */
            R_OS_SysUnlock();

            /* mutex not yet available */
            if (0 == ticks_to_wait)
            {
                /* ready for exit */
                waiting_for_mutex = false;
            }
            else
            {
                /* wait as timer not yet expired */
                R_OS_TaskSleep(1);
            }

            /* update timer if appropriate */
            if (R_OS_ABSTRACTION_EV_WAIT_INFINITE != timeout_ms)
            {
                ticks_to_wait--;
            }
        }
    } while (waiting_for_mutex);

    /* return status of operation */
    return (ret);
}
/**********************************************************************************************************************
 End of function R_OS_MutexWait
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_EnterCritical
 * Description  : Enter critical section
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
void R_OS_EnterCritical (void)
{
    R_COMPILER_DisableInterrupts();
}
/**********************************************************************************************************************
 End of function R_OS_EnterCritical
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_ExitCritical
 * Description  : Exit critical section
 * Arguments    : None
 * Return Value : None
 *********************************************************************************************************************/
void R_OS_ExitCritical (void)
{
    R_COMPILER_EnableInterrupts();
}
/**********************************************************************************************************************
 End of function R_OS_ExitCritical
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MessageQueueCreate
 * Description  : Create a Message Queue
 * Arguments    : queue_size Maximum number of elements in queue
 *                pp_queue_handle pointer to queue handle pointer
 * Return Value : The function returns TRUE if the message queue was successfully created.
 *                Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueueCreate (p_os_msg_queue_handle_t *pp_queue_handle, uint32_t queue_size)
{
    st_message_queue_t *p_message_queue;
    uint_t element_size;
    uint32_t p_queue_buffer_size;
    bool_t status = true;

    /* allocate memory for message queue control structure */
    p_message_queue = (st_message_queue_t *) R_OS_Malloc(sizeof(st_message_queue_t), R_MEMORY_REGION_DEFAULT);

    /* validate queue */
    if ((NULL == p_message_queue) || (0u == queue_size))
    {
        status = false;
    }
    else
    {
        /* The message queue element is (void *) */
        element_size = sizeof(void *);

        p_queue_buffer_size = element_size * queue_size;

        /* allocate memory for message queue itself */
        p_message_queue->p_queue_buffer = R_OS_Malloc(p_queue_buffer_size, R_MEMORY_REGION_DEFAULT);

        /* handle memory allocation failure */
        if (NULL == p_message_queue->p_queue_buffer)
        {
            /* free the memory allocated to the queue control structure */
            R_OS_Free((void *) p_message_queue);
            status = false;
        }
        else
        {
            /* initialise the queue to empty */
            p_message_queue->element_size = element_size;
            p_message_queue->max_elements = queue_size;
            p_message_queue->in_index = 0;
            p_message_queue->out_index = 0;
            p_message_queue->buffer_full = 0;
        }
    }

    if (false == status)
    {
        /* Create failed. NULL the queue handle */
        p_message_queue = NULL;
    }

    /* set the queue handle pointer argument appropriately */
    *pp_queue_handle = p_message_queue;

    /* return operation status */
    return (status);
}
/**********************************************************************************************************************
 End of function R_OS_MessageQueueCreate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MessageQueuePut
 * Description  : Put a message onto a queue
 * Arguments    : p_queue_handle pointer to queue handle
 *                p_message pointer to message
 * Return Value : The function returns TRUE if the message queue object was successfully added to the queue.
 *                Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueuePut (p_os_msg_queue_handle_t p_queue_handle, p_os_msg_t p_message)
{
    st_message_queue_t *p_message_queue;
    uint_t next_in_index;
    uint_t offset;

    /* check that we have a valid handle or message queue pointer */
    if ((NULL == p_queue_handle) || (NULL == ((st_message_queue_t *) p_queue_handle)->p_queue_buffer))
    {
        return false;
    }

    R_OS_SysLock();

    /* assign pointer to message queue pointer for clarity */
    p_message_queue = (st_message_queue_t *) p_queue_handle;

    /* check for space in the queue and the queue buffer is valid */
    if (p_message_queue->buffer_full)
    {
        R_OS_SysUnlock();
        return false; /* the queue is full */
    }

    /* add the message to the queue */
    offset = p_message_queue->element_size * p_message_queue->in_index;

    /* void pointer cast */
    memcpy(p_message_queue->p_queue_buffer + offset, (void *) &p_message, p_message_queue->element_size);

    /* update the in index */
    next_in_index = p_message_queue->in_index + 1;

    if (next_in_index >= p_message_queue->max_elements)
    {
        next_in_index = 0;
    }

    p_message_queue->in_index = next_in_index;

    /* mark the queue as full if the in index has caught up with the out index */
    if (p_message_queue->in_index == p_message_queue->out_index)
    {
        p_message_queue->buffer_full = true;
    }

    R_OS_SysUnlock();

    return true;
}
/**********************************************************************************************************************
 End of function R_OS_MessageQueuePut
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MessageQueueGet
 * Description  : Get a message from a queue
 * Arguments    : p_queue_handle pointer to queue handle
 *                pp_message pointer to message pointer. Pointer will point to NULL if no message and times out
 *                timeout in system ticks
 *                blocking true = block thread/task until message received.False = not blocking
 * Return Value : The function returns TRUE if the message queue object was successfully retrieved from the queue.
 *                Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueueGet (p_os_msg_queue_handle_t p_queue_handle, p_os_msg_t *pp_message, uint32_t timeout,
        bool_t blocking)
{
    st_message_queue_t *p_message_queue;
    uint_t offset;
    bool_t message_received;

    /* check that we have a valid handle or message queue pointer */
    if ((NULL == p_queue_handle) || (NULL == ((st_message_queue_t *) p_queue_handle)->p_queue_buffer))
    {
        return false;
    }

    /* (blocking == true) means wait forever for a message, so set the timeout to infinite */
    if (blocking)
    {
        timeout = R_OS_ABSTRACTION_EV_WAIT_INFINITE;
    }

    R_OS_SysLock();

    /* assign pointer to message queue pointer for clarity */
    p_message_queue = (st_message_queue_t *) p_queue_handle;

    /* check if there is a message available */
    message_received = ((p_message_queue->in_index != p_message_queue->out_index) || p_message_queue->buffer_full);

    R_OS_SysUnlock();

    /* if there is no message and no timeout is specified then return with no message */
    if ((0 == timeout) && ( !message_received))
    {
        /* zero the message */
        memset((void *) pp_message, 0, p_message_queue->element_size);
        return false;
    }

    /* wait for a message, remembering that we're OS less, so this can only come in on a interrupt */
    while (( !message_received) && (timeout > 0))
    {
        /* interrupts are only disabled while we check the queue for a message */
        R_OS_SysLock();

        /* test for a message in the queue */
        message_received = ((p_message_queue->in_index != p_message_queue->out_index) || p_message_queue->buffer_full);

        R_OS_SysUnlock();

        /* if there is no message then delay for 1ms */
        /* interrupts are enabled here, so a message could arrive on an interrupt routine */
        if ( !message_received)
        {
            R_OS_TaskSleep(1);

            if (R_OS_ABSTRACTION_EV_WAIT_INFINITE != timeout)
            {
                timeout--;
            }
        }
    }

    if (message_received)
    {
        R_OS_SysLock();

        /* retrieve the message from the queue */
        offset = p_message_queue->element_size * p_message_queue->out_index;

        /* void pointer cast */
        memcpy((void *) pp_message, p_message_queue->p_queue_buffer + offset, p_message_queue->element_size);

        /* update the out index */
        p_message_queue->out_index++;

        if (p_message_queue->out_index >= p_message_queue->max_elements)
        {
            p_message_queue->out_index = 0;
        }

        /* we've taken a message out, so the queue is no longer full */
        p_message_queue->buffer_full = false;

        R_OS_SysUnlock();
    }
    else
    {
        /* zero the message */
        memset((void *) pp_message, 0, p_message_queue->element_size);
    }

    return message_received;
}
/**********************************************************************************************************************
 End of function R_OS_MessageQueueGet
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MessageQueueClear
 * Description  : Clear a message queue
 * Arguments    : p_queue_handle pointer to queue handle
 * Return Value : The function returns TRUE if the message queue object was successfully cleared.
 *                Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueueClear (p_os_msg_queue_handle_t p_queue_handle)
{
    st_message_queue_t *p_message_queue;

    /* check that we have a valid handle or message queue pointer */
    if ((NULL == p_queue_handle) || (NULL == ((st_message_queue_t *) p_queue_handle)->p_queue_buffer))
    {
        return false;
    }

    /* assign pointer to message queue pointer for clarity */
    p_message_queue = (st_message_queue_t *) p_queue_handle;

    R_OS_SysLock();

    /* clear the queue */
    p_message_queue->in_index = 0;
    p_message_queue->out_index = 0;
    p_message_queue->buffer_full = false;

    R_OS_SysUnlock();

    return true;
}
/**********************************************************************************************************************
 End of function R_OS_MessageQueueClear
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_MessageQueueDelete
 * Description  : Delete a message queue. The message queue pointer argument will be set to NULL
 * Arguments    : pp_queue_handle pointer to queue handle pointer
 * Return Value : The function returns true if the message queue object was successfully deleted.
 *                Otherwise, false is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueueDelete (p_os_msg_queue_handle_t *pp_queue_handle)
{
    /* validate the queue handle pointer and queue pointer */
    if ((NULL == pp_queue_handle) || (NULL == ( *pp_queue_handle)))
    {
        return false;
    }

    R_OS_SysLock();

    /* free the queue buffer memory */
    if (NULL != ((st_message_queue_t *) *pp_queue_handle)->p_queue_buffer)
    {
        /* cast to the appropriate types to avoid warnings */
        R_OS_Free((void *) &(((st_message_queue_t *) *pp_queue_handle)->p_queue_buffer));
    }

    /* NULL the message queue buffer pointer before freeing control structure out of paranoia */
    ((st_message_queue_t *) pp_queue_handle)->p_queue_buffer = NULL;

    /* free the message queue control structure */
    R_OS_Free((void *) pp_queue_handle);

    /* set the queue pointer to NULL */
    *pp_queue_handle = NULL;

    R_OS_SysUnlock();

    return true;
}
/**********************************************************************************************************************
 End of function R_OS_MessageQueueDelete
 *********************************************************************************************************************/

/* Event Management */
/**********************************************************************************************************************
 * Function Name: R_OS_EventCreate
 * Description  : Create an event
 * Arguments    : pp_event - pointer to an associated event
 * Return Value : The function returns TRUE if the event object was
 *                successfully created. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_EventCreate (pp_event_t pp_event)
{
    bool_t ret_val = false;

    /* actual element of mutex is int_t */
    *pp_event = (p_event_t) R_OS_Malloc(sizeof(e_event_state_t), R_MEMORY_REGION_DEFAULT);

    /* Initialise new mutex if malloc was successful*/
    if (NULL != ( *pp_event))
    {
        /* initialise mutex in reset state */
        R_OS_EventReset(pp_event);

        ret_val = true;
    }

    /* return status */
    return (ret_val);
}
/**********************************************************************************************************************
 End of function R_OS_EventCreate
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_EventDelete
 * Description  : Delete an event, freeing any associated resources
 * Arguments    : event_ptr - pointer to a associated event
 * Return Value : None
 *********************************************************************************************************************/
void R_OS_EventDelete (pp_event_t pp_event)
{
    /* NULL check */
    if ((NULL == pp_event) || (NULL == ( *pp_event)))
    {
        return;
    }

    /* Delete Event
     * Deleting Event requires locking around the modification to the Event
     * if we have a nested interrupt capability, so that the previous
     * interrupt state can be restored. */
    R_OS_SysLock();

    /* free up memory and NULL the pointer */
    R_OS_Free((void **) pp_event);

    R_OS_SysUnlock();
}
/**********************************************************************************************************************
 End of function R_OS_EventDelete
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_EventSet
 * Description  : Sets the state on the associated event
 * Arguments    : event_ptr - pointer to a associated event
 * Return Value : None
 *********************************************************************************************************************/
void R_OS_EventSet (pp_event_t pp_event)
{
    /* NULL check */
    if ((NULL == pp_event) || (NULL == ( *pp_event)))
    {
        return;
    }

    /* Access to Event element requires locking. */
    R_OS_SysLock();

    /* Force the specified event to the signalled state */
    /* pp_event is a pointer to a pointer to an event */
    **(e_event_state_t **) pp_event = EV_SET;

    /* release lock */
    R_OS_SysUnlock();
}
/**********************************************************************************************************************
 End of function R_OS_EventSet
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_EventReset
 * Description  : Clears the state on the associated event
 * Arguments    : event_ptr - pointer to a associated event
 * Return Value : None
 *********************************************************************************************************************/
void R_OS_EventReset (pp_event_t pp_event)
{
    /* NULL check */
    if ((NULL == pp_event) || (NULL == ( *pp_event)))
    {
        return;
    }

    /* Access to Event element requires locking. */
    R_OS_SysLock();

    /* Force the specified event to the signalled state */
    /* pp_event is a pointer to a pointer to an event */
    **(e_event_state_t **) pp_event = EV_RESET;

    /* release lock */
    R_OS_SysUnlock();
}
/**********************************************************************************************************************
 End of function R_OS_EventReset
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Function Name: R_OS_EventGet
 Description:   Function to return the current state of an event
 Arguments:     IN event_ptr - pointer to the event
 Return value:  The state of the event
 *********************************************************************************************************************/
e_event_state_t R_OS_EventGet (pp_event_t pp_event)
{
    e_event_state_t event_state = EV_INVALID;

    /* NULL check */
    if ((NULL == pp_event) || (NULL == ( *pp_event)))
    {
        return (EV_INVALID);
    }

    /* Access to Event element requires locking. */
    R_OS_SysLock();

    /* get state of event - p_event is a pointer to a pointer to an event */
    event_state = **(e_event_state_t **) pp_event;

    /* release lock */
    R_OS_SysUnlock();

    return (event_state);
}
/**********************************************************************************************************************
 End of function R_OS_EventGet
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_EventWait
 * Description  : Wait for event. In an OS Less environment. This function is
 *                blocking so the event must be set from within an ISR or at
 *                least something in a different context to where is function
 *                is called from, or it will wait forever.
 *                This function uses the OSTM driver to provide timing.
 * Arguments    : event_ptr - pointer to a associated event
 *                timeout   - maximum time to wait for associated event to occur
 * Return Value : false - timeout occurred.
 *                true - event occurred.
 *********************************************************************************************************************/
bool_t R_OS_EventWait (pp_event_t pp_event, systime_t timeout_ms)
{
    volatile bool_t ret = false;
    uint32_t ticks_to_wait = timeout_ms;
    volatile bool_t waiting_for_event = true;
    e_event_state_t event_state = EV_INVALID;

    /* NULL check */
    if ((NULL == pp_event) || (NULL == ( *pp_event)))
    {
        return (false);
    }

    do
    {
        /* Access to Event element requires locking. */
        R_OS_SysLock();

        /* get state of event */
        event_state = **(e_event_state_t **) pp_event;

        /* Release Locking */
        R_OS_SysUnlock();

        if (EV_SET == event_state)
        {
            /* event is set */
            ret = true;
            waiting_for_event = false;
        }
        else
        {
            /* event not yet available */
            if (0 == ticks_to_wait)
            {
                /* ready for exit */
                waiting_for_event = false;
            }
            else
            {
                /* wait as timer not yet expired */
                R_OS_TaskSleep(1);
            }

            /* update timer if appropriate */
            if (R_OS_ABSTRACTION_EV_WAIT_INFINITE != timeout_ms)
            {
                ticks_to_wait--;
            }
        }
    } while (waiting_for_event);

    /* return status of operation */
    return (ret);
}
/**********************************************************************************************************************
 End of function R_OS_EventWait
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_EventSetFromIsr
 * Description  : Sets the state on the associated event
 *                Warning : Function shall only be caller from within an ISR routine
 * Arguments    : event_ptr  - Pointer to a associated event
 * Return Value : The function returns TRUE if the event object was successfully set. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_EventSetFromIsr (pp_event_t pp_event)
{
    R_OS_EventSet(pp_event);

    return (true);
}
/**********************************************************************************************************************
 End of function R_OS_EventSetFromIsr
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Function Name: R_OS_TaskGetPriority
 Description:   Not relevant in os-less environment
 Arguments:     uiTaskID
 Return value:  0
 *********************************************************************************************************************/
int32_t R_OS_TaskGetPriority (uint32_t uiTaskID)
{
    /* unused variable */
    UNUSED_PARAM(uiTaskID);

    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);

    /* Return false */
    return (0);
}
/**********************************************************************************************************************
 End of function  R_OS_TaskGetPriority
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Function Name: R_OS_TaskSetPriority
 Description:   Not relevant in os-less environment
 Arguments:     uiTaskID
 uiPriority
 Return value:  false
 *********************************************************************************************************************/
bool_t R_OS_TaskSetPriority (uint32_t uiTaskID, uint32_t uiPriority)
{
    /* unused variable */
    UNUSED_PARAM(uiTaskID);

    /* unused variable */
    UNUSED_PARAM(uiPriority);

    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);

    /* Return false */
    return (false);
}
/**********************************************************************************************************************
 End of function  R_OS_TaskSetPriority
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskGetCurrentHandle
 * Description  : Not relevant in os-less environment
 * Arguments    : none
 * Return Value : (os_task_t *)NULL
 *********************************************************************************************************************/
os_task_t *R_OS_TaskGetCurrentHandle (void)
{
    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);

    /* Return NULL */
    return ((os_task_t *) NULL);
}
/**********************************************************************************************************************
 End of function R_OS_TaskGetCurrentHandle
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_TaskGetState
 * Description  : not relevant in os-less environment
 * Arguments    : task
 * Return Value : NULL
 *********************************************************************************************************************/
const char *R_OS_TaskGetState (const char *task)
{
    /* unused variable */
    UNUSED_PARAM(task);

    /* Unsupported when using OS_LESS Abstraction Layer */
    R_OS_AssertCalled( __FILE__, __LINE__);

    /* Return NULL */
    return ((os_task_t *) NULL);
}
/**********************************************************************************************************************
 End of function R_OS_TaskGetState
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_GetVersion
 * Description  : Get the version
 * Arguments    : pinfo - detailed driver information structure
 * Return Value : 0 can not fail
 *********************************************************************************************************************/
int32_t R_OS_GetVersion (st_os_abstraction_info_t *p_info)
{
    p_info->version.sub.major = s_os_version.version.sub.major;
    p_info->version.sub.minor = s_os_version.version.sub.minor;
    p_info->build = s_os_version.build;
    p_info->p_szdriver_name = s_os_version.p_szdriver_name;
    return (0);
}
/**********************************************************************************************************************
 End of function R_OS_GetVersion
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: R_OS_GetTickCount
 * Description  : Get the number of task ticks.
 * Arguments    : None
 * Return Value : Number of Ticks counted for a task
 *********************************************************************************************************************/
uint32_t R_OS_GetTickCount (void)
{
    return (s_interrupt_counter);
}
/**********************************************************************************************************************
 End of function R_OS_GetTickCount
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: os_abstraction_isr
 *          Used to perform timing operations (for function like R_OS_TaskSleep()
 * @retval test status pass true or false
 *********************************************************************************************************************/
void os_abstraction_isr (void)
{
    s_interrupt_counter++;
}
/**********************************************************************************************************************
 End of function os_abstraction_isr
 *********************************************************************************************************************/

