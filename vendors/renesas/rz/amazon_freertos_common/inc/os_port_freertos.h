

#ifndef _OS_PORT_FREERTOS_H
#define _OS_PORT_FREERTOS_H

//Dependencies
#include "freertos.h"
#include "compiler_settings.h"
#include "task.h"
#include "semphr.h"

//Milliseconds to system ticks
#ifndef OS_MS_TO_SYSTICKS
   #define OS_MS_TO_SYSTICKS(n) (n)
#endif

//System ticks to milliseconds
#ifndef OS_SYSTICKS_TO_MS
   #define OS_SYSTICKS_TO_MS(n) (n)
#endif

//Enter interrupt service routine
#if defined(portENTER_SWITCHING_ISR)
   #define osEnterIsr() portENTER_SWITCHING_ISR()
#else
   #define osEnterIsr()
#endif

//Leave interrupt service routine
#if defined(portEXIT_SWITCHING_ISR)
   #define osExitIsr(flag) portEXIT_SWITCHING_ISR()
#elif defined(portEND_SWITCHING_ISR)
   #define osExitIsr(flag) portEND_SWITCHING_ISR(flag)
#elif defined(portYIELD_FROM_ISR)
   #define osExitIsr(flag) portYIELD_FROM_ISR(flag)
#else
   #define osExitIsr(flag)
#endif


/**
 * @brief Task object
 **/

//typedef void OsTask;
#define OsTask void


/**
 * @brief Event object
 **/

typedef struct
{
   xSemaphoreHandle handle;
} OsEvent;


/**
 * @brief Semaphore object
 **/

typedef struct
{
   xSemaphoreHandle handle;
} OsSemaphore;


/**
 * @brief Mutex object
 **/

typedef struct
{
   xSemaphoreHandle handle;
} OsMutex;


/**
 * @brief Task routine
 **/

typedef void (*OsTaskCode)(void *params);


//Kernel management
void osInitKernel(void);
void osStartKernel(void);

//Task management
OsTask *osCreateTask(const char_t *name, OsTaskCode taskCode,
   void *params, size_t stackSize, int_t priority);

void osDeleteTask(OsTask *task);
void osDelayTask(systime_t delay);
void osSwitchTask(void);
void osSuspendAllTasks(void);
void osResumeAllTasks(void);

//Event management
bool_t osCreateEvent(OsEvent *event);
void osDeleteEvent(OsEvent *event);
void osSetEvent(OsEvent *event);
void osResetEvent(OsEvent *event);
bool_t osWaitForEvent(OsEvent *event, systime_t timeout);
bool_t osSetEventFromIsr(OsEvent *event);

//Semaphore management
bool_t osCreateSemaphore(OsSemaphore *semaphore, uint_t count);
void osDeleteSemaphore(OsSemaphore *semaphore);
bool_t osWaitForSemaphore(OsSemaphore *semaphore, systime_t timeout);
void osReleaseSemaphore(OsSemaphore *semaphore);

//Mutex management
bool_t osCreateMutex(OsMutex *mutex);
void osDeleteMutex(OsMutex *mutex);
void osAcquireMutex(OsMutex *mutex);
void osReleaseMutex(OsMutex *mutex);

//System time
systime_t osGetSystemTime(void);

//Memory management
void *osAllocMem(size_t size);
void osFreeMem(void *p);

//Atomic operations
uint16_t osAtomicInc16(uint16_t *n);
uint32_t osAtomicInc32(uint32_t *n);

#endif
