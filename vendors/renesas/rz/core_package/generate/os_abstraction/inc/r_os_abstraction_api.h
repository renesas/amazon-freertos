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
 * File Name    : r_os_abstraction_api.h
 * Description  : see Detailed description (below)
 *********************************************************************************************************************/

/******************************************************************************************************************//**
 * @ingroup Interface_Library
 * @defgroup OS_ABSTRACT Operating System abstraction module
 * @brief Provides OS abstraction, use these primitives in the code base NOT direct calls to OS primitives.
 *
 * To make efficient code re-use the identical high layer driver shall be used in both OS and OS Less
 * applications.
 * This file aims to abstract the Operating system (OS) awareness when creating an OS Less driver.
 *
 * @section EBK_RZA2M_OS_ABSTRACT_API_LIMITATIONS Known Limitations
 * NONE
 *
 * @section EBK_RZA2M_OS_ABSTRACT_API_INSTANCES Known Implementations
 * @ref NONE_YET
 *
 * @section EBK_RZA2M_OS_ABSTRACT_API_RELATED Related modules
 * @ingroup DS_BOARD_SUPPORT
 * See also:
 * @ref DS_BOARD_SUPPORT,
 * @ref EBK_RZA2M_RSK_OSTM_DRIVER,
 * @ref EBK_RZA2M_RSK_LED
 * @{
 *********************************************************************************************************************/
#include "r_os_abstraction_typedef.h"
#include "r_os_abstraction_config.h"
#include "driver.h"

#ifndef SRC_RENESAS_APPLICATION_INC_R_OS_ABSTRACTION_API_H_
#define SRC_RENESAS_APPLICATION_INC_R_OS_ABSTRACTION_API_H_

/** Major Version Number of API. */
#define R_OS_ABSTRACTION_VERSION_MAJOR         (3)

/** Minor Version Number of API. */
#define R_OS_ABSTRACTION_VERSION_MINOR         (5)

/** Unique ID. */
#define R_OS_ABSTRACTION_UID                   (71)

/** Build Number of API.
 * Generated during customer release */
#define R_OS_ABSTRACTION_BUILD_NUM             (0)

/** Maximum timeout used in wait functions inside the OS abstraction module */
#define R_OS_ABSTRACTION_EV_WAIT_INFINITE      (0xFFFFFFFFUL)

/** Invalid handle used in functions inside the OS abstraction module */
#define R_OS_ABSTRACTION_INVALID_HANDLE        (-1)

/** Stack sizes, these indexes are mapped to actual sizes inside the OS abstraction module */
#define R_OS_ABSTRACTION_TINY_STACK_SIZE       (0)
#define R_OS_ABSTRACTION_SMALL_STACK_SIZE      (1)
#define R_OS_ABSTRACTION_DEFAULT_STACK_SIZE    (2)
#define R_OS_ABSTRACTION_LARGE_STACK_SIZE      (3)
#define R_OS_ABSTRACTION_HUGE_STACK_SIZE       (4)

#define R_OS_ABSTRACTION_MAX_TASK_NAME_SIZE    (24)
#define R_OS_ABSTRACTION_OSTM_RESOURCE          ("\\\\.\\ostm_reserved")

/** Milliseconds to system ticks */
#ifndef R_OS_MS_TO_SYSTICKS
    #define R_OS_MS_TO_SYSTICKS(n) (n)
#endif

/** System ticks to milliseconds */
#ifndef R_OS_SYSTICKS_TO_MS
    #define R_OS_SYSTICKS_TO_MS(n) (n)
#endif


/**********************************************************************************************************************
 *                                  OS Abstraction Manager API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_AbstractionLayerInit
 * @brief         Function to configure critical resources for the connected OS or scheduler.
 * @warning       .
 * @param [in]    .
 * @param [out]   .
 * @retval        true if there were no errors when initialising the OS Abstraction Layer.
 * @retval        false if there errors when initialising the OS Abstraction Layer.
 *********************************************************************************************************************/
bool_t R_OS_AbstractionLayerInit (void);

/**********************************************************************************************************************
 * @fn            R_OS_AbstractionLayerShutdown
 * @brief         Function to release critical resources for the connected OS or scheduler.
 * @warning       .
 * @param [in]    .
 * @param [out]   .
 * @retval        true if there were no errors when closing the OS Abstraction Layer.
 * @retval        false if there errors when closing the OS Abstraction Layer.
 *********************************************************************************************************************/
bool_t R_OS_AbstractionLayerShutdown (void);


/**********************************************************************************************************************
 *                                  Kernel API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_KernelInit
 * @brief         Function to configure critical resources for the connected OS or scheduler,
 *                or configure an OS-Less sample.
 * @warning       .
 * @param [in]    .
 * @param [out]   .
 * @retval        NONE
 *********************************************************************************************************************/
void R_OS_KernelInit (void);

/**********************************************************************************************************************
 * @fn            R_OS_KernelStart
 * @brief         Function to enable the connected OS or scheduler, or configure an OS-Less sample.
 * @warning       .
 * @param [in]    .
 * @param [out]   .
 * @retval        NONE.
 *********************************************************************************************************************/
void R_OS_KernelStart (void);

/**********************************************************************************************************************
 * @fn            R_OS_KernelStop
 * @brief         Function to stop the connected OS or scheduler, or configure an OS-Less sample. Provided for
 *                completeness, may never be used. When powering down a system safely this function should
 *                be called.
 * @warning       .
 * @param [in]    .
 * @param [out]   .
 * @retval        NONE.
 *********************************************************************************************************************/
void R_OS_KernelStop (void);

/**********************************************************************************************************************
 * @fn            R_OS_InitMemManager
 * @brief         Function to configure memory resources for the connected OS or scheduler,
 *                or configured an OS-Less sample.
 * @warning       .
 * @param [in]    .
 * @param [out]   .
 * @retval        NONE.
 *********************************************************************************************************************/
void R_OS_InitMemManager (void);

/**********************************************************************************************************************
 * @fn            R_OS_Running
 * @brief         Used to determine if the OS has started
 * @retval        true if scheduler has started
 * @retval        false if scheduler has not started (at least once)
 *********************************************************************************************************************/
bool_t R_OS_Running (void);



/**********************************************************************************************************************
 *                                  Task API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_TaskCreate
 * @brief         Function to create a new task.
 * @warning       name string may be subject to length limitations. There is a security risk if the name
 *                is not bounded effectively in the implementation.
 * @param [in]    p_name        ASCII character representation for the name of the Task.
 * @param [in]    task_code     Function pointer to the implementation of the Task.
 * @param [in]    p_params      Structure to be used by the Task.
 * @param [in]    stack_size    Stack size for allocation to the Task.
 * @param [in]    priority      Task priority in system context.
 * @retval        os_task_t The task object.
 *********************************************************************************************************************/
os_task_t *R_OS_TaskCreate (const char_t *p_name, os_task_code_t task_code, void *p_params, 
                            size_t stack_size, int_t priority);

/**********************************************************************************************************************
 * @fn            R_OS_TaskDelete
 * @brief         Function to delete a task.
 * @warning       The target OS is responsible for verifying the Task is valid to delete.
 * @param [in]    p_task the task object.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_TaskDelete (os_task_t **p_task);

/**********************************************************************************************************************
 * @fn            R_OS_TaskSleep
 * @brief         Function to cause a task to suspend and pass control back to the
 *                OS / scheduler for a requested period.
 * @warning       The time stated is a minimum, higher priority tasks may prevent this Task from being restored
 *                immediately.
 * @param [in]    sleep_ms Time in ms (uint32 => max ~49 Days).
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_TaskSleep (uint32_t sleep_ms);

/**********************************************************************************************************************
 * @fn            R_OS_TaskYield
 * @brief         Function to cause a task to suspend and pass control back to the OS / scheduler.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_TaskYield (void);

/**********************************************************************************************************************
 * @fn            R_OS_TaskSuspend
 * @brief         Function to cause a task to suspend and pass control back to the OS / scheduler.
 * @param [in]    p_task the task object.
 * @retval        None.
 *********************************************************************************************************************/
bool_t   R_OS_TaskSuspend (os_task_t *p_task);

/**********************************************************************************************************************
 * @fn            R_OS_TaskResume
 * @brief         Function to cause a task to suspend and pass control back to the OS / scheduler.
 * @param [in]    p_task the task object.
 * @retval        None.
 *********************************************************************************************************************/
bool_t   R_OS_TaskResume (os_task_t *p_task);

/**********************************************************************************************************************
 * @fn            R_OS_TasksSuspendAll
 * @brief         Suspend all tasks, only attempted if the operating system is running.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_TasksSuspendAll (void);

/**********************************************************************************************************************
 * @fn            R_OS_TasksResumeAll
 * @brief         Resume all tasks, only attempted if the operating system is running.
 * @retval        None
 *********************************************************************************************************************/
void   R_OS_TasksResumeAll (void);

/**********************************************************************************************************************
 * @fn            R_OS_TasksGetNumber
 * @brief         Function to obtain total number of active tasks defined in the system,
 *                only attempted if the operating system is running.
 * @retval        number of tasks.
 *********************************************************************************************************************/
uint32_t   R_OS_TasksGetNumber (void);

/**********************************************************************************************************************
 * @fn            R_OS_TaskUsesFloatingPoint
 * @brief         Function to indicate to the OS that the current task uses floating point numbers.
 * @retval        NONE.
 *********************************************************************************************************************/
void R_OS_TaskUsesFloatingPoint (void);

/**********************************************************************************************************************
 * @fn            R_OS_TaskGetPriority
 * @brief         Gets current task priority
 * @warning       Function shall only be called when the scheduler is running
 * @param [in]    task_id desired Task
 * @retval        The function returns the task priority of the specified uiTaskID
 * @retval        -1 if the uiTaskID can not be found
 *********************************************************************************************************************/
int32_t R_OS_TaskGetPriority (uint32_t task_id);

/**********************************************************************************************************************
 * @fn            R_OS_TaskSetPriority
 * @brief         Sets current task priority
 * @warning       Function shall only be called when the scheduler is running
 * @param [in]    task_id desired task
 * @param [out]   priority desired priority
 * @retval        true if priority is set
 * @retval        false if priority can not be set
 *********************************************************************************************************************/
bool_t R_OS_TaskSetPriority (uint32_t task_id, uint32_t priority);

/**********************************************************************************************************************
 * @fn            R_OS_TaskGetCurrentHandle
 * @brief         Gets current task
 * @warning       Function shall only be called when the scheduler is running
 * @param [in]    none
 * @retval        The function returns the current running task
 *********************************************************************************************************************/
os_task_t * R_OS_TaskGetCurrentHandle (void);

/**********************************************************************************************************************
 * @fn            R_OS_TaskGetCurrentName
 * @brief         Gets text name of current task
 * @warning       Function shall only be called when the scheduler is running
 * @param [in]    none
 * @retval        The function returns a pointer to the text name of the current task
 *********************************************************************************************************************/
const char * R_OS_TaskGetCurrentName (void);

/**********************************************************************************************************************
 * @fn            R_OS_TaskGetState
 * @brief         Gets status information on selected task in human readable form.
 * @warning       Function shall only be called when the scheduler is running
 * @param [in]    p_task: task name in human readable form.
 * @retval        The function returns a character string that can be displayed on a console.
 *********************************************************************************************************************/
const char * R_OS_TaskGetState (const char* p_task);

/**********************************************************************************************************************
 *                                  System API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_SysLock
 * @brief         Function to lock a critical section.
 * @warning       This function must prevent the OS or scheduler from swapping context. This is often implemented by
 *                preventing system interrupts from occurring, and so pending any OS timer interruptions. Timing is
 *                critical, code protected by this function must be able to complete in the minimum time possible and
 *                never block.
 * @retval        1  Critical Section entered
 * @retval        0  Object locked
 * @retval        -1  Error, neither action possible
 *********************************************************************************************************************/
int_t  R_OS_SysLock (void);

/**********************************************************************************************************************
 * @fn            R_OS_SysUnlock
 * @brief         Function to unlock a critical section.
 * @warning       This function releases the OS or scheduler to normal operation. Timing is critical, code
 *                preceding this function must be able to complete in the minimum time possible and never block.
 * @retval        None.
 *********************************************************************************************************************/
void R_OS_SysUnlock (void);

/**********************************************************************************************************************
 * @fn            R_OS_SysWaitAccess
 * @brief         Function to acquire system mutex.
 * @warning       The OS Abstraction layer contains a system mutex.
 *                This function allows a user to obtain the mutex for system critical usage.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_SysWaitAccess (void);

/**********************************************************************************************************************
 * @fn            R_OS_SysReleaseAccess
 * @brief         Function to release system mutex.
 * @warning       The OS Abstraction layer contains a system mutex. This function allows a user to release
 *                the mutex from system critical usage.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_SysReleaseAccess (void);

/**********************************************************************************************************************
 * @fn            R_OS_GetTickCount
 * @brief         Gets ticks currently counted for task which calls it.
 * @warning       Function can only be called when the scheduler is running
 * @retval        The function returns the number of ticks counted.
 *********************************************************************************************************************/
uint32_t R_OS_GetTickCount (void);

/**********************************************************************************************************************
 * @fn            R_OS_AssertCalled
 * @brief         Generic error handler, enters forever loop but allows debugger to step out.
 * @param [in]    p_file: file in which the error occurred.
 * @param [out]   line: line where the error occurred.
 * @retval        NONE.
 *********************************************************************************************************************/
void R_OS_AssertCalled (const char * p_file, uint32_t line);

/**********************************************************************************************************************
 *                                  Memory Management API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_Malloc
 * @brief         Allocates block of memory the length of "size"
 * @param [in]    size: Size of memory to allocate.
 * @param [in]    region: Region of memory to allocate from.
 * @retval        pointer to allocated memory or NULL if failed
 *********************************************************************************************************************/
void * R_OS_Malloc (size_t size, e_memory_region_t region);

/**********************************************************************************************************************
 * @fn            R_OS_Free
 * @brief         Function to free allocated memory.
 * @param [in]    p_memory_to_free: Block of memory to free.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_Free (void ** pp_memory_to_free);

/**********************************************************************************************************************
 *                                  Semaphore API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_SemaphoreCreate
 * @brief         Create a semaphore.
 * @param [in]    p_semaphore pointer to an associated semaphore.
 * @param[in]     count the maximum count for the semaphore object. This value must be greater than zero
 * @retval        TRUE The semaphore object was successfully created.
 * @retval        FALSE Semaphore not created.
 *********************************************************************************************************************/
bool_t R_OS_SemaphoreCreate (p_semaphore_t p_semaphore, uint32_t count);

/**********************************************************************************************************************
 * @fn            R_OS_SemaphoreDelete
 * @brief         Delete a semaphore, freeing any associated resources.
 * @param [in]    p_semaphore: Pointer to an associated semaphore.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_SemaphoreDelete (p_semaphore_t  p_semaphore);

/**********************************************************************************************************************
 * @fn            R_OS_SemaphoreWait
 * @brief         Blocks operation until either a timeout occurs or the associated semaphore has been set.
 * @warning       .
 * @param [in]    p_semaphore Pointer to an associated semaphore.
 * @param [in]    timeout Maximum time to wait for associated event to occur.
 * @retval        TRUE The semaphore object was successfully set.
 * @retval        FALSE Semaphore not set.
 *********************************************************************************************************************/
bool_t R_OS_SemaphoreWait (p_semaphore_t p_semaphore, systime_t timeout);

/**********************************************************************************************************************
 * @fn            R_OS_SemaphoreRelease
 * @brief         Release a semaphore, freeing it to be used by another task.
 * @param [in]    p_semaphore: Pointer to an associated semaphore.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_SemaphoreRelease (p_semaphore_t p_semaphore);

/**********************************************************************************************************************
 *                                  Mutex API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_MutexCreate
 * @brief         Creates a mutex and returns a pointer to it.
 * @retval        p_mutex: Pointer to mutex created.
 * @retval        NULL If mutex creation fails.
 *********************************************************************************************************************/
void *  R_OS_MutexCreate (void);

/**********************************************************************************************************************
 * @fn            R_OS_MutexDelete
 * @brief         Deletes a Mutex
 * @param [in]    pp_mutex: Address of mutex object to delete, set to NULL when deleted.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_MutexDelete (pp_mutex_t pp_mutex);

/**********************************************************************************************************************
 * @fn            R_OS_MutexAcquire
 * @brief         Acquires possession of a Mutex, will context switch until free, with no timeout.
 * @param [in]    p_mutex: Mutex object to acquire.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_MutexAcquire (p_mutex_t p_mutex);

/**********************************************************************************************************************
 * @fn            R_OS_MutexRelease
 * @brief         Releases possesion of a mutex
 * @param [in]    p_mutex: Mutex object to release.
 * @retval        None.
 *********************************************************************************************************************/
void   R_OS_MutexRelease (p_mutex_t p_mutex);

/**********************************************************************************************************************
 * @fn            R_OS_MutexWait
 * @brief         Attempts to claim mutex for 'timeout' length, will fail if not possible.
 *                If mutex passed is NULL, this function will create new mutex.
 * @param [in]    pp_mutex: Address of mutex object to acquire.
 * @param[in]     time_out: Length of Time to wait for mutex.
 * @retval        TRUE Mutex is acquired
 * @retval        FALSE Wait Timed out, mutex not acquired.
 *********************************************************************************************************************/
bool_t R_OS_MutexWait (pp_mutex_t pp_mutex, uint32_t time_out);

/**********************************************************************************************************************
 * @fn            R_OS_EnterCritical
 * @brief         Enter critical area of code - prevent context switches.
 * @warning       This function must prevent the OS or scheduler from swapping context. This is often implemented by
 *                preventing system interrupts from occurring, and so pending any OS timer interruptions. Timing is
 *                critical, code protected by this function must be able to complete in the minimum time possible and
 *                never block.
 * @param [in]    None.
 * @retval        None.
 *********************************************************************************************************************/
void R_OS_EnterCritical (void);

/**********************************************************************************************************************
 * @fn            R_OS_ExitCritical
 * @brief         Exit critical area.
 * @warning       This function releases the OS or scheduler to normal operation. Timing is critical, code
 *                preceding this function must be able to complete in the minimum time possible and never block.
 * @param [in]    None.
 * @retval        None.
 *********************************************************************************************************************/
void R_OS_ExitCritical (void);

/**********************************************************************************************************************
 *                                  Message Queue API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_MessageQueueCreate
 * @brief         Create a Message Queue of length "queue_sz".
 * @param [in]    queue_sz: Maximum number of elements in queue.
 * @param[in]     pp_queue_handle: pointer to queue handle pointer.
 * @retval        The function returns TRUE if the queue was successfully created. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueueCreate (p_os_msg_queue_handle_t * pp_queue_handle, uint32_t queue_sz);

/**********************************************************************************************************************
 * @fn            R_OS_MessageQueuePut
 * @brief         Put a message onto a queue.
 * @warning       Can be called from both inside and outside of an Interrupt Service Routine.
 * @param [in]    p_queue_handle pointer to queue handle.
 * @param[in]     p_message pointer to message.
 * @retval        The function returns TRUE if the message was successfully queued. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueuePut (p_os_msg_queue_handle_t p_queue_handle, p_os_msg_t p_message);

/**********************************************************************************************************************
 * @fn            R_OS_MessageQueueGet
 * @brief         Retrieve a message from a queue.
 * @warning       Can only be called outside of an Interrupt Service Routine.
 * @param [in]    p_queue pointer to queue handle.
 * @param [out]   pp_msg pointer to message pointer. Pointer will point to NULL if no message and times out.
 * @param [in]    timeout in system ticks.
 * @param [in]    blocking true = block thread/task until message received. False = not blocking
 * @retval        The function returns TRUE if the message was successfully retrieved. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueueGet (p_os_msg_queue_handle_t p_queue, p_os_msg_t * pp_msg, uint32_t timeout, bool_t blocking);

/**********************************************************************************************************************
 * @fn            R_OS_MessageQueueClear
 * @brief         Clear a message queue, resetting it to an empty state.
 * @param [in]    p_queue_handle pointer to queue handle.
 * @retval        The function returns TRUE if the queue was successfully cleared. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueueClear (p_os_msg_queue_handle_t p_queue_handle);

/**********************************************************************************************************************
 * @fn            R_OS_MessageQueueDelete
 * @brief         Delete a message queue. The message queue pointer argument will be set to NULL.
 * @param [in]    pp_queue_handle pointer to queue handle pointer.
 * @retval        The function returns TRUE if the queue was successfully deleted. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_MessageQueueDelete (p_os_msg_queue_handle_t * pp_queue_handle);

/**********************************************************************************************************************
 *                                      Event API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_EventCreate
 * @brief         Create an event object for inter-task communication.
 * @param [in]    pp_event Pointer to an associated event.
 * @retval        The function returns TRUE if the event object was successfully created. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_EventCreate (pp_event_t pp_event);

/**********************************************************************************************************************
 * @fn            R_OS_EventDelete
 * @brief         Delete an event, freeing any associated resources.
 * @param [in]    pp_event Pointer to an associated event.
 * @retval        none
 *********************************************************************************************************************/
void   R_OS_EventDelete (pp_event_t pp_event);

/**********************************************************************************************************************
 * @fn            R_OS_EventSet
 * @brief         Sets the state on the associated event, setting event to EV_SET.
 * @warning       use outside of an interrupt service routine.
 * @param [in]    pp_event Pointer to an associated event.
 * @retval        none.
 *********************************************************************************************************************/
void   R_OS_EventSet (pp_event_t pp_event);

/**********************************************************************************************************************
 * @fn            R_OS_EventReset
 * @brief         Clears the state on the associated event, setting event to EV_RESET.
 * @param [in]    pp_event Pointer to an associated event.
 * @retval        none.
 *********************************************************************************************************************/
void   R_OS_EventReset (pp_event_t pp_event);

/**********************************************************************************************************************
 * @fn            R_OS_EventGet
 * @brief         Returns the state on the associated event.
 * @param [in]    pp_event Pointer to an associated event.
 * @retval        EV_RESET Event Reset.
 * @retval        EV_SET   Event Set.
 * @retval        EV_INVALID Invalid Event.
 *********************************************************************************************************************/
e_event_state_t R_OS_EventGet (pp_event_t pp_event);

/**********************************************************************************************************************
 * @fn            R_OS_EventWait
 * @brief         Blocks operation until a timeout occurs or the associated event has been set.
 * @param [in]    pp_event Pointer to an associated event.
 * @param [in]    timeout Maximum time to wait for associated event to occur.
 * @retval        The function returns TRUE if the event object was set, Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_EventWait (pp_event_t pp_event, systime_t timeout);

/**********************************************************************************************************************
 * @fn            R_OS_EventSetFromIsr
 * @brief         Sets the state on the associated event from inside an interrupt service routine.
 *                Setting event to EV_SET
 * @warning       Function shall only be called from within an ISR routine
 * @param [in]    pp_event Pointer to an associated event
 * @retval        The function returns TRUE if the event object was successfully set. Otherwise, FALSE is returned
 *********************************************************************************************************************/
bool_t R_OS_EventSetFromIsr (pp_event_t pp_event);

/**********************************************************************************************************************
 *                                      Version Control API
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * @fn            R_OS_GetVersion
 * @brief         Obtains the version information from this module
 * @param [out]   p_info Structure containing module version information.
 * @retval        The function returns 0
 *********************************************************************************************************************/
int32_t R_OS_GetVersion (st_os_abstraction_info_t * p_info);

#endif /* SRC_RENESAS_APPLICATION_INC_R_OS_ABSTRACTION_API_H_ */

/******************************************************************************************************************//**
 * @} (end addtogroup OS_ABSTRACT)
 *********************************************************************************************************************/
