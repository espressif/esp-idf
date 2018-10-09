/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


#ifndef INC_TASK_H
#define INC_TASK_H

#ifndef INC_FREERTOS_H
	#error "include FreeRTOS.h must appear in source files before include task.h"
#endif

#include <limits.h>

#include "list.h"
#include "portmacro.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/

#define tskKERNEL_VERSION_NUMBER "V8.2.0"
#define tskKERNEL_VERSION_MAJOR 8
#define tskKERNEL_VERSION_MINOR 2
#define tskKERNEL_VERSION_BUILD 0

/**
 * @brief Argument of xTaskCreatePinnedToCore indicating that task has no affinity
 */
#define tskNO_AFFINITY INT_MAX

/**
 * task. h
 *
 * Type by which tasks are referenced.  For example, a call to xTaskCreate
 * returns (via a pointer parameter) an TaskHandle_t variable that can then
 * be used as a parameter to vTaskDelete to delete the task.
 *
 * \ingroup Tasks
 */
typedef void * TaskHandle_t;

/**
 * Defines the prototype to which the application task hook function must
 * conform.
 */
typedef BaseType_t (*TaskHookFunction_t)( void * );

/** Task states returned by eTaskGetState. */
typedef enum
{
	eRunning = 0,	/*!< A task is querying the state of itself, so must be running. */
	eReady,			/*!< The task being queried is in a read or pending ready list. */
	eBlocked,		/*!< The task being queried is in the Blocked state. */
	eSuspended,		/*!< The task being queried is in the Suspended state, or is in the Blocked state with an infinite time out. */
	eDeleted		/*!< The task being queried has been deleted, but its TCB has not yet been freed. */
} eTaskState;

/** Actions that can be performed when vTaskNotify() is called. */
typedef enum
{
	eNoAction = 0,				/*!< Notify the task without updating its notify value. */
	eSetBits,					/*!< Set bits in the task's notification value. */
	eIncrement,					/*!< Increment the task's notification value. */
	eSetValueWithOverwrite,		/*!< Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
	eSetValueWithoutOverwrite	/*!< Set the task's notification value if the previous value has been read by the task. */
} eNotifyAction;

/** @cond */
/**
 * Used internally only.
 */
typedef struct xTIME_OUT
{
	BaseType_t xOverflowCount;
	TickType_t xTimeOnEntering;
} TimeOut_t;

/**
 * Defines the memory ranges allocated to the task when an MPU is used.
 */
typedef struct xMEMORY_REGION
{
	void *pvBaseAddress;
	uint32_t ulLengthInBytes;
	uint32_t ulParameters;
} MemoryRegion_t;

/**
 * Parameters required to create an MPU protected task.
 */
typedef struct xTASK_PARAMETERS
{
	TaskFunction_t pvTaskCode;
	const char * const pcName;	/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	uint32_t usStackDepth;
	void *pvParameters;
	UBaseType_t uxPriority;
	StackType_t *puxStackBuffer;
	MemoryRegion_t xRegions[ portNUM_CONFIGURABLE_REGIONS ];
} TaskParameters_t;
/** @endcond */

/**
 *  Used with the uxTaskGetSystemState() function to return the state of each task in the system.
*/
typedef struct xTASK_STATUS
{
	TaskHandle_t xHandle;			/*!< The handle of the task to which the rest of the information in the structure relates. */
	const char *pcTaskName;			/*!< A pointer to the task's name.  This value will be invalid if the task was deleted since the structure was populated! */ /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
	UBaseType_t xTaskNumber;		/*!< A number unique to the task. */
	eTaskState eCurrentState;		/*!< The state in which the task existed when the structure was populated. */
	UBaseType_t uxCurrentPriority;	/*!< The priority at which the task was running (may be inherited) when the structure was populated. */
	UBaseType_t uxBasePriority;		/*!< The priority to which the task will return if the task's current priority has been inherited to avoid unbounded priority inversion when obtaining a mutex.  Only valid if configUSE_MUTEXES is defined as 1 in FreeRTOSConfig.h. */
	uint32_t ulRunTimeCounter;		/*!< The total run time allocated to the task so far, as defined by the run time stats clock.  See http://www.freertos.org/rtos-run-time-stats.html.  Only valid when configGENERATE_RUN_TIME_STATS is defined as 1 in FreeRTOSConfig.h. */
	StackType_t *pxStackBase;		/*!< Points to the lowest address of the task's stack area. */
	uint32_t usStackHighWaterMark;	/*!< The minimum amount of stack space that has remained for the task since the task was created.  The closer this value is to zero the closer the task has come to overflowing its stack. */
#if configTASKLIST_INCLUDE_COREID
	BaseType_t xCoreID;				/*!< Core this task is pinned to. This field is present if CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID is set. */
#endif
} TaskStatus_t;

/**
 * Used with the uxTaskGetSnapshotAll() function to save memory snapshot of each task in the system.
 * We need this struct because TCB_t is defined (hidden) in tasks.c.
 */
typedef struct xTASK_SNAPSHOT
{
	void        *pxTCB;         /*!< Address of task control block. */
	StackType_t *pxTopOfStack;  /*!< Points to the location of the last item placed on the tasks stack. */
	StackType_t *pxEndOfStack;  /*!< Points to the end of the stack. pxTopOfStack < pxEndOfStack, stack grows hi2lo
									pxTopOfStack > pxEndOfStack, stack grows lo2hi*/
} TaskSnapshot_t;

/**
 * Possible return values for eTaskConfirmSleepModeStatus().
 */
typedef enum
{
	eAbortSleep = 0,		/*!< A task has been made ready or a context switch pended since portSUPPORESS_TICKS_AND_SLEEP() was called - abort entering a sleep mode. */
	eStandardSleep,			/*!< Enter a sleep mode that will not last any longer than the expected idle time. */
	eNoTasksWaitingTimeout	/*!< No tasks are waiting for a timeout so it is safe to enter a sleep mode that can only be exited by an external interrupt. */
} eSleepModeStatus;


/**
 * Defines the priority used by the idle task.  This must not be modified.
 *
 * \ingroup TaskUtils
 */
#define tskIDLE_PRIORITY			( ( UBaseType_t ) 0U )

/**
 * task. h
 *
 * Macro for forcing a context switch.
 *
 * \ingroup SchedulerControl
 */
#define taskYIELD()					portYIELD()

/**
 * task. h
 *
 * Macro to mark the start of a critical code region.  Preemptive context
 * switches cannot occur when in a critical region.
 *
 * @note This may alter the stack (depending on the portable implementation)
 * so must be used with care!
 *
 * \ingroup SchedulerControl
 */
#ifdef _ESP_FREERTOS_INTERNAL
#define taskENTER_CRITICAL(mux)		portENTER_CRITICAL(mux)
#else
#define taskENTER_CRITICAL(mux) _Pragma("GCC warning \"'taskENTER_CRITICAL(mux)' is deprecated in ESP-IDF, consider using 'portENTER_CRITICAL(mux)'\"") portENTER_CRITICAL(mux)
#endif
#define taskENTER_CRITICAL_ISR(mux)		portENTER_CRITICAL_ISR(mux)

/**
 * task. h
 *
 * Macro to mark the end of a critical code region.  Preemptive context
 * switches cannot occur when in a critical region.
 *
 * @note This may alter the stack (depending on the portable implementation)
 * so must be used with care!
 *
 * \ingroup SchedulerControl
 */
#ifdef _ESP_FREERTOS_INTERNAL
#define taskEXIT_CRITICAL(mux)			portEXIT_CRITICAL(mux)
#else
#define taskEXIT_CRITICAL(mux) _Pragma("GCC warning \"'taskEXIT_CRITICAL(mux)' is deprecated in ESP-IDF, consider using 'portEXIT_CRITICAL(mux)'\"") portEXIT_CRITICAL(mux)
#endif
#define taskEXIT_CRITICAL_ISR(mux)		portEXIT_CRITICAL_ISR(mux)

/**
 * task. h
 *
 * Macro to disable all maskable interrupts.
 *
 * \ingroup SchedulerControl
 */
#define taskDISABLE_INTERRUPTS()	portDISABLE_INTERRUPTS()

/**
 * task. h
 *
 * Macro to enable microcontroller interrupts.
 *
 * \ingroup SchedulerControl
 */
#define taskENABLE_INTERRUPTS()		portENABLE_INTERRUPTS()

/* Definitions returned by xTaskGetSchedulerState().  taskSCHEDULER_SUSPENDED is
0 to generate more optimal code when configASSERT() is defined as the constant
is used in assert() statements. */
#define taskSCHEDULER_SUSPENDED		( ( BaseType_t ) 0 )
#define taskSCHEDULER_NOT_STARTED	( ( BaseType_t ) 1 )
#define taskSCHEDULER_RUNNING		( ( BaseType_t ) 2 )


/*-----------------------------------------------------------
 * TASK CREATION API
 *----------------------------------------------------------*/

/**
 * Create a new task with a specified affinity.
 *
 * This function is similar to xTaskCreate, but allows setting task affinity
 * in SMP system.
 *
 * @param pvTaskCode Pointer to the task entry function.  Tasks
 * must be implemented to never return (i.e. continuous loop).
 *
 * @param pcName A descriptive name for the task.  This is mainly used to
 * facilitate debugging.  Max length defined by configMAX_TASK_NAME_LEN - default
 * is 16.
 *
 * @param usStackDepth The size of the task stack specified as the number of
 * bytes. Note that this differs from vanilla FreeRTOS.
 *
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 *
 * @param uxPriority The priority at which the task should run.  Systems that
 * include MPU support can optionally create tasks in a privileged (system)
 * mode by setting bit portPRIVILEGE_BIT of the priority parameter.  For
 * example, to create a privileged task at priority 2 the uxPriority parameter
 * should be set to ( 2 | portPRIVILEGE_BIT ).
 *
 * @param pvCreatedTask Used to pass back a handle by which the created task
 * can be referenced.
 *
 * @param xCoreID If the value is tskNO_AFFINITY, the created task is not
 * pinned to any CPU, and the scheduler can run it on any core available.
 * Other values indicate the index number of the CPU which the task should
 * be pinned to. Specifying values larger than (portNUM_PROCESSORS - 1) will
 * cause the function to fail.
 *
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 *
 * \ingroup Tasks
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	BaseType_t xTaskCreatePinnedToCore(	TaskFunction_t pvTaskCode,
										const char * const pcName,
										const uint32_t usStackDepth,
										void * const pvParameters,
										UBaseType_t uxPriority,
										TaskHandle_t * const pvCreatedTask,
										const BaseType_t xCoreID);

#endif

/**
 * Create a new task and add it to the list of tasks that are ready to run.
 *
 * Internally, within the FreeRTOS implementation, tasks use two blocks of
 * memory.  The first block is used to hold the task's data structures.  The
 * second block is used by the task as its stack.  If a task is created using
 * xTaskCreate() then both blocks of memory are automatically dynamically
 * allocated inside the xTaskCreate() function.  (see
 * http://www.freertos.org/a00111.html).  If a task is created using
 * xTaskCreateStatic() then the application writer must provide the required
 * memory.  xTaskCreateStatic() therefore allows a task to be created without
 * using any dynamic memory allocation.
 *
 * See xTaskCreateStatic() for a version that does not use any dynamic memory
 * allocation.
 *
 * xTaskCreate() can only be used to create a task that has unrestricted
 * access to the entire microcontroller memory map.  Systems that include MPU
 * support can alternatively create an MPU constrained task using
 * xTaskCreateRestricted().
 *
 * @param pvTaskCode Pointer to the task entry function.  Tasks
 * must be implemented to never return (i.e. continuous loop).
 *
 * @param pcName A descriptive name for the task.  This is mainly used to
 * facilitate debugging.  Max length defined by configMAX_TASK_NAME_LEN - default
 * is 16.
 *
 * @param usStackDepth The size of the task stack specified as the number of
 * bytes. Note that this differs from vanilla FreeRTOS.
 *
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 *
 * @param uxPriority The priority at which the task should run.  Systems that
 * include MPU support can optionally create tasks in a privileged (system)
 * mode by setting bit portPRIVILEGE_BIT of the priority parameter.  For
 * example, to create a privileged task at priority 2 the uxPriority parameter
 * should be set to ( 2 | portPRIVILEGE_BIT ).
 *
 * @param pvCreatedTask Used to pass back a handle by which the created task
 * can be referenced.
 *
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 *
 * @note If program uses thread local variables (ones specified with "__thread" keyword)
 * then storage for them will be allocated on the task's stack.
 *
 * Example usage:
 * @code{c}
 *  // Task to be created.
 *  void vTaskCode( void * pvParameters )
 *  {
 *   for( ;; )
 *   {
 *       // Task code goes here.
 *   }
 *  }
 *
 *  // Function that creates a task.
 *  void vOtherFunction( void )
 *  {
 *  static uint8_t ucParameterToPass;
 *  TaskHandle_t xHandle = NULL;
 *
 *   // Create the task, storing the handle.  Note that the passed parameter ucParameterToPass
 *   // must exist for the lifetime of the task, so in this case is declared static.  If it was just an
 *   // an automatic stack variable it might no longer exist, or at least have been corrupted, by the time
 *   // the new task attempts to access it.
 *   xTaskCreate( vTaskCode, "NAME", STACK_SIZE, &ucParameterToPass, tskIDLE_PRIORITY, &xHandle );
 *      configASSERT( xHandle );
 *
 *   // Use the handle to delete the task.
 *      if( xHandle != NULL )
 *      {
 *       vTaskDelete( xHandle );
 *      }
 *  }
 * @endcode
 * \ingroup Tasks
 */

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

	static inline IRAM_ATTR BaseType_t xTaskCreate(
			TaskFunction_t pvTaskCode,
			const char * const pcName,
			const uint32_t usStackDepth,
			void * const pvParameters,
			UBaseType_t uxPriority,
			TaskHandle_t * const pvCreatedTask)
	{
		return xTaskCreatePinnedToCore( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pvCreatedTask, tskNO_AFFINITY );
	}

#endif




/**
 * Create a new task with a specified affinity.
 *
 * This function is similar to xTaskCreateStatic, but allows specifying
 * task affinity in an SMP system.
 *
 * @param pvTaskCode Pointer to the task entry function.  Tasks
 * must be implemented to never return (i.e. continuous loop).
 *
 * @param pcName A descriptive name for the task.  This is mainly used to
 * facilitate debugging.  The maximum length of the string is defined by
 * configMAX_TASK_NAME_LEN in FreeRTOSConfig.h.
 *
 * @param ulStackDepth The size of the task stack specified as the number of
 * bytes. Note that this differs from vanilla FreeRTOS.
 *
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 *
 * @param uxPriority The priority at which the task will run.
 *
 * @param pxStackBuffer Must point to a StackType_t array that has at least
 * ulStackDepth indexes - the array will then be used as the task's stack,
 * removing the need for the stack to be allocated dynamically.
 *
 * @param pxTaskBuffer Must point to a variable of type StaticTask_t, which will
 * then be used to hold the task's data structures, removing the need for the
 * memory to be allocated dynamically.
 *
 * @param xCoreID If the value is tskNO_AFFINITY, the created task is not
 * pinned to any CPU, and the scheduler can run it on any core available.
 * Other values indicate the index number of the CPU which the task should
 * be pinned to. Specifying values larger than (portNUM_PROCESSORS - 1) will
 * cause the function to fail.
 *
 * @return If neither pxStackBuffer or pxTaskBuffer are NULL, then the task will
 * be created and pdPASS is returned.  If either pxStackBuffer or pxTaskBuffer
 * are NULL then the task will not be created and
 * errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY is returned.
 *
 * \ingroup Tasks
 */
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	TaskHandle_t xTaskCreateStaticPinnedToCore(	TaskFunction_t pvTaskCode,
												const char * const pcName,
												const uint32_t ulStackDepth,
												void * const pvParameters,
												UBaseType_t uxPriority,
												StackType_t * const pxStackBuffer,
												StaticTask_t * const pxTaskBuffer,
												const BaseType_t xCoreID );
#endif /* configSUPPORT_STATIC_ALLOCATION */

/**
 * Create a new task and add it to the list of tasks that are ready to run.
 *
 * Internally, within the FreeRTOS implementation, tasks use two blocks of
 * memory.  The first block is used to hold the task's data structures.  The
 * second block is used by the task as its stack.  If a task is created using
 * xTaskCreate() then both blocks of memory are automatically dynamically
 * allocated inside the xTaskCreate() function.  (see
 * http://www.freertos.org/a00111.html).  If a task is created using
 * xTaskCreateStatic() then the application writer must provide the required
 * memory.  xTaskCreateStatic() therefore allows a task to be created without
 * using any dynamic memory allocation.
 *
 * @param pvTaskCode Pointer to the task entry function.  Tasks
 * must be implemented to never return (i.e. continuous loop).
 *
 * @param pcName A descriptive name for the task.  This is mainly used to
 * facilitate debugging.  The maximum length of the string is defined by
 * configMAX_TASK_NAME_LEN in FreeRTOSConfig.h.
 *
 * @param ulStackDepth The size of the task stack specified as the number of
 * bytes. Note that this differs from vanilla FreeRTOS.
 *
 * @param pvParameters Pointer that will be used as the parameter for the task
 * being created.
 *
 * @param uxPriority The priority at which the task will run.
 *
 * @param pxStackBuffer Must point to a StackType_t array that has at least
 * ulStackDepth indexes - the array will then be used as the task's stack,
 * removing the need for the stack to be allocated dynamically.
 *
 * @param pxTaskBuffer Must point to a variable of type StaticTask_t, which will
 * then be used to hold the task's data structures, removing the need for the
 * memory to be allocated dynamically.
 *
 * @return If neither pxStackBuffer or pxTaskBuffer are NULL, then the task will
 * be created and pdPASS is returned.  If either pxStackBuffer or pxTaskBuffer
 * are NULL then the task will not be created and
 * errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY is returned.
 *
 * @note If program uses thread local variables (ones specified with "__thread" keyword)
 * then storage for them will be allocated on the task's stack.
 *
 * Example usage:
 * @code{c}
 *
 *     // Dimensions the buffer that the task being created will use as its stack.
 *     // NOTE:  This is the number of bytes the stack will hold, not the number of
 *     // words as found in vanilla FreeRTOS.
 *     #define STACK_SIZE 200
 *
 *     // Structure that will hold the TCB of the task being created.
 *     StaticTask_t xTaskBuffer;
 *
 *     // Buffer that the task being created will use as its stack.  Note this is
 *     // an array of StackType_t variables.  The size of StackType_t is dependent on
 *     // the RTOS port.
 *     StackType_t xStack[ STACK_SIZE ];
 *
 *     // Function that implements the task being created.
 *     void vTaskCode( void * pvParameters )
 *     {
 *         // The parameter value is expected to be 1 as 1 is passed in the
 *         // pvParameters value in the call to xTaskCreateStatic().
 *         configASSERT( ( uint32_t ) pvParameters == 1UL );
 *
 *         for( ;; )
 *         {
 *             // Task code goes here.
 *         }
 *     }
 *
 *     // Function that creates a task.
 *     void vOtherFunction( void )
 *     {
 *         TaskHandle_t xHandle = NULL;
 *
 *         // Create the task without using any dynamic memory allocation.
 *         xHandle = xTaskCreateStatic(
 *                       vTaskCode,       // Function that implements the task.
 *                       "NAME",          // Text name for the task.
 *                       STACK_SIZE,      // Stack size in bytes, not words.
 *                       ( void * ) 1,    // Parameter passed into the task.
 *                       tskIDLE_PRIORITY,// Priority at which the task is created.
 *                       xStack,          // Array to use as the task's stack.
 *                       &xTaskBuffer );  // Variable to hold the task's data structure.
 *
 *         // puxStackBuffer and pxTaskBuffer were not NULL, so the task will have
 *         // been created, and xHandle will be the task's handle.  Use the handle
 *         // to suspend the task.
 *         vTaskSuspend( xHandle );
 *     }
 * @endcode
 * \ingroup Tasks
 */

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	static inline IRAM_ATTR TaskHandle_t xTaskCreateStatic(
			TaskFunction_t pvTaskCode,
			const char * const pcName,
			const uint32_t ulStackDepth,
			void * const pvParameters,
			UBaseType_t uxPriority,
			StackType_t * const pxStackBuffer,
			StaticTask_t * const pxTaskBuffer)
	{
		return xTaskCreateStaticPinnedToCore( pvTaskCode, pcName, ulStackDepth, pvParameters, uxPriority, pxStackBuffer, pxTaskBuffer, tskNO_AFFINITY );
	}
#endif /* configSUPPORT_STATIC_ALLOCATION */

/** @cond */
/**
 * xTaskCreateRestricted() should only be used in systems that include an MPU
 * implementation.
 *
 * Create a new task and add it to the list of tasks that are ready to run.
 * The function parameters define the memory regions and associated access
 * permissions allocated to the task.
 *
 * @param pxTaskDefinition Pointer to a structure that contains a member
 * for each of the normal xTaskCreate() parameters (see the xTaskCreate() API
 * documentation) plus an optional stack buffer and the memory region
 * definitions.
 *
 * @param pxCreatedTask Used to pass back a handle by which the created task
 * can be referenced.
 *
 * @return pdPASS if the task was successfully created and added to a ready
 * list, otherwise an error code defined in the file projdefs.h
 *
 * Example usage:
 * @code{c}
 * // Create an TaskParameters_t structure that defines the task to be created.
 * static const TaskParameters_t xCheckTaskParameters =
 * {
 * 	vATask,		// pvTaskCode - the function that implements the task.
 * 	"ATask",	// pcName - just a text name for the task to assist debugging.
 * 	100,		// usStackDepth	- the stack size DEFINED IN BYTES.
 * 	NULL,		// pvParameters - passed into the task function as the function parameters.
 * 	( 1UL | portPRIVILEGE_BIT ),// uxPriority - task priority, set the portPRIVILEGE_BIT if the task should run in a privileged state.
 * 	cStackBuffer,// puxStackBuffer - the buffer to be used as the task stack.
 *
 * 	// xRegions - Allocate up to three separate memory regions for access by
 * 	// the task, with appropriate access permissions.  Different processors have
 * 	// different memory alignment requirements - refer to the FreeRTOS documentation
 * 	// for full information.
 * 	{
 * 		// Base address					Length	Parameters
 *         { cReadWriteArray,				32,		portMPU_REGION_READ_WRITE },
 *         { cReadOnlyArray,				32,		portMPU_REGION_READ_ONLY },
 *         { cPrivilegedOnlyAccessArray,	128,	portMPU_REGION_PRIVILEGED_READ_WRITE }
 * 	}
 * };
 *
 * int main( void )
 * {
 * TaskHandle_t xHandle;
 *
 * 	// Create a task from the const structure defined above.  The task handle
 * 	// is requested (the second parameter is not NULL) but in this case just for
 * 	// demonstration purposes as its not actually used.
 * 	xTaskCreateRestricted( &xRegTest1Parameters, &xHandle );
 *
 * 	// Start the scheduler.
 * 	vTaskStartScheduler();
 *
 * 	// Will only get here if there was insufficient memory to create the idle
 * 	// and/or timer task.
 * 	for( ;; );
 * }
 * @endcode
 * \ingroup Tasks
 */
#if( portUSING_MPU_WRAPPERS == 1 )
	BaseType_t xTaskCreateRestricted( const TaskParameters_t * const pxTaskDefinition, TaskHandle_t *pxCreatedTask ) PRIVILEGED_FUNCTION;
#endif


/**
 * Memory regions are assigned to a restricted task when the task is created by
 * a call to xTaskCreateRestricted().  These regions can be redefined using
 * vTaskAllocateMPURegions().
 *
 * @param xTask The handle of the task being updated.
 *
 * @param xRegions A pointer to an MemoryRegion_t structure that contains the
 * new memory region definitions.
 *
 * Example usage:
 *
 * @code{c}
 * // Define an array of MemoryRegion_t structures that configures an MPU region
 * // allowing read/write access for 1024 bytes starting at the beginning of the
 * // ucOneKByte array.  The other two of the maximum 3 definable regions are
 * // unused so set to zero.
 * static const MemoryRegion_t xAltRegions[ portNUM_CONFIGURABLE_REGIONS ] =
 * {
 * 	// Base address		Length		Parameters
 * 	{ ucOneKByte,		1024,		portMPU_REGION_READ_WRITE },
 * 	{ 0,				0,			0 },
 * 	{ 0,				0,			0 }
 * };
 *
 * void vATask( void *pvParameters )
 * {
 * 	// This task was created such that it has access to certain regions of
 * 	// memory as defined by the MPU configuration.  At some point it is
 * 	// desired that these MPU regions are replaced with that defined in the
 * 	// xAltRegions const struct above.  Use a call to vTaskAllocateMPURegions()
 * 	// for this purpose.  NULL is used as the task handle to indicate that this
 * 	// function should modify the MPU regions of the calling task.
 * 	vTaskAllocateMPURegions( NULL, xAltRegions );
 *
 * 	// Now the task can continue its function, but from this point on can only
 * 	// access its stack and the ucOneKByte array (unless any other statically
 * 	// defined or shared regions have been declared elsewhere).
 * }
 * @endcode
 * \ingroup Tasks
 */
void vTaskAllocateMPURegions( TaskHandle_t xTask, const MemoryRegion_t * const pxRegions ) PRIVILEGED_FUNCTION;

/** @endcond */

/**
 * Remove a task from the RTOS real time kernel's management.
 *
 * The task being deleted will be removed from all ready, blocked, suspended
 * and event lists.
 *
 * INCLUDE_vTaskDelete must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * @note The idle task is responsible for freeing the kernel allocated
 * memory from tasks that have been deleted.  It is therefore important that
 * the idle task is not starved of microcontroller processing time if your
 * application makes any calls to vTaskDelete ().  Memory allocated by the
 * task code is not automatically freed, and should be freed before the task
 * is deleted.
 *
 * See the demo application file death.c for sample code that utilises
 * vTaskDelete ().
 *
 * @param xTaskToDelete The handle of the task to be deleted.  Passing NULL will
 * cause the calling task to be deleted.
 *
 * Example usage:
 * @code{c}
 *  void vOtherFunction( void )
 *  {
 *  TaskHandle_t xHandle;
 *
 * 	 // Create the task, storing the handle.
 * 	 xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );
 *
 * 	 // Use the handle to delete the task.
 * 	 vTaskDelete( xHandle );
 *  }
 * @endcode
 * \ingroup Tasks
 */
void vTaskDelete( TaskHandle_t xTaskToDelete ) PRIVILEGED_FUNCTION;

/*-----------------------------------------------------------
 * TASK CONTROL API
 *----------------------------------------------------------*/

/**
 * Delay a task for a given number of ticks.
 *
 * The actual time that the task remains blocked depends on the tick rate.
 * The constant portTICK_PERIOD_MS can be used to calculate real time from
 * the tick rate - with the resolution of one tick period.
 *
 * INCLUDE_vTaskDelay must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * vTaskDelay() specifies a time at which the task wishes to unblock relative to
 * the time at which vTaskDelay() is called.  For example, specifying a block
 * period of 100 ticks will cause the task to unblock 100 ticks after
 * vTaskDelay() is called.  vTaskDelay() does not therefore provide a good method
 * of controlling the frequency of a periodic task as the path taken through the
 * code, as well as other task and interrupt activity, will effect the frequency
 * at which vTaskDelay() gets called and therefore the time at which the task
 * next executes.  See vTaskDelayUntil() for an alternative API function designed
 * to facilitate fixed frequency execution.  It does this by specifying an
 * absolute time (rather than a relative time) at which the calling task should
 * unblock.
 *
 * @param xTicksToDelay The amount of time, in tick periods, that
 * the calling task should block.
 *
 * Example usage:
 * @code{c}
 *  void vTaskFunction( void * pvParameters )
 *  {
 *  // Block for 500ms.
 *  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
 *
 * 	 for( ;; )
 * 	 {
 * 		 // Simply toggle the LED every 500ms, blocking between each toggle.
 * 		 vToggleLED();
 * 		 vTaskDelay( xDelay );
 * 	 }
 *  }
 * @endcode
 * \ingroup TaskCtrl
 */
void vTaskDelay( const TickType_t xTicksToDelay ) PRIVILEGED_FUNCTION;

/**
 * Delay a task until a specified time.
 *
 * INCLUDE_vTaskDelayUntil must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * This function can be used by periodic tasks to ensure a constant execution frequency.
 *
 * This function differs from vTaskDelay () in one important aspect:  vTaskDelay () will
 * cause a task to block for the specified number of ticks from the time vTaskDelay () is
 * called.  It is therefore difficult to use vTaskDelay () by itself to generate a fixed
 * execution frequency as the time between a task starting to execute and that task
 * calling vTaskDelay () may not be fixed [the task may take a different path though the
 * code between calls, or may get interrupted or preempted a different number of times
 * each time it executes].
 *
 * Whereas vTaskDelay () specifies a wake time relative to the time at which the function
 * is called, vTaskDelayUntil () specifies the absolute (exact) time at which it wishes to
 * unblock.
 *
 * The constant portTICK_PERIOD_MS can be used to calculate real time from the tick
 * rate - with the resolution of one tick period.
 *
 * @param pxPreviousWakeTime Pointer to a variable that holds the time at which the
 * task was last unblocked.  The variable must be initialised with the current time
 * prior to its first use (see the example below).  Following this the variable is
 * automatically updated within vTaskDelayUntil ().
 *
 * @param xTimeIncrement The cycle time period.  The task will be unblocked at
 * time *pxPreviousWakeTime + xTimeIncrement.  Calling vTaskDelayUntil with the
 * same xTimeIncrement parameter value will cause the task to execute with
 * a fixed interface period.
 *
 * Example usage:
 * @code{c}
 *  // Perform an action every 10 ticks.
 *  void vTaskFunction( void * pvParameters )
 *  {
 *  TickType_t xLastWakeTime;
 *  const TickType_t xFrequency = 10;
 *
 * 	 // Initialise the xLastWakeTime variable with the current time.
 * 	 xLastWakeTime = xTaskGetTickCount ();
 * 	 for( ;; )
 * 	 {
 * 		 // Wait for the next cycle.
 * 		 vTaskDelayUntil( &xLastWakeTime, xFrequency );
 *
 * 		 // Perform action here.
 * 	 }
 *  }
 * @endcode
 * \ingroup TaskCtrl
 */
void vTaskDelayUntil( TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement ) PRIVILEGED_FUNCTION;

/**
 * Obtain the priority of any task.
 *
 * INCLUDE_uxTaskPriorityGet must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * @param xTask Handle of the task to be queried.  Passing a NULL
 * handle results in the priority of the calling task being returned.
 *
 * @return The priority of xTask.
 *
 * Example usage:
 * @code{c}
 *  void vAFunction( void )
 *  {
 *  TaskHandle_t xHandle;
 *
 *   // Create a task, storing the handle.
 *   xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );
 *
 *   // ...
 *
 *   // Use the handle to obtain the priority of the created task.
 *   // It was created with tskIDLE_PRIORITY, but may have changed
 *   // it itself.
 *   if( uxTaskPriorityGet( xHandle ) != tskIDLE_PRIORITY )
 *   {
 *       // The task has changed it's priority.
 *   }
 *
 *   // ...
 *
 *   // Is our priority higher than the created task?
 *   if( uxTaskPriorityGet( xHandle ) < uxTaskPriorityGet( NULL ) )
 *   {
 *       // Our priority (obtained using NULL handle) is higher.
 *   }
 * }
 * @endcode
 * \ingroup TaskCtrl
 */
UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/**
 * A version of uxTaskPriorityGet() that can be used from an ISR.
 *
 * @param xTask Handle of the task to be queried.  Passing a NULL
 * handle results in the priority of the calling task being returned.
 *
 * @return The priority of xTask.
 *
 */
UBaseType_t uxTaskPriorityGetFromISR( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/**
 * Obtain the state of any task.
 *
 * States are encoded by the eTaskState enumerated type.
 *
 * INCLUDE_eTaskGetState must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * @param xTask Handle of the task to be queried.
 *
 * @return The state of xTask at the time the function was called.  Note the
 * state of the task might change between the function being called, and the
 * functions return value being tested by the calling task.
 */
eTaskState eTaskGetState( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/**
 * Set the priority of any task.
 *
 * INCLUDE_vTaskPrioritySet must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * A context switch will occur before the function returns if the priority
 * being set is higher than the currently executing task.
 *
 * @param xTask Handle to the task for which the priority is being set.
 * Passing a NULL handle results in the priority of the calling task being set.
 *
 * @param uxNewPriority The priority to which the task will be set.
 *
 * Example usage:
 * @code{c}
 *  void vAFunction( void )
 *  {
 *  TaskHandle_t xHandle;
 *
 *   // Create a task, storing the handle.
 *   xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );
 *
 *   // ...
 *
 *   // Use the handle to raise the priority of the created task.
 *   vTaskPrioritySet( xHandle, tskIDLE_PRIORITY + 1 );
 *
 *   // ...
 *
 *   // Use a NULL handle to raise our priority to the same value.
 *   vTaskPrioritySet( NULL, tskIDLE_PRIORITY + 1 );
 *  }
 * @endcode
 * \ingroup TaskCtrl
 */
void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority ) PRIVILEGED_FUNCTION;

/**
 * Suspend a task.
 *
 * INCLUDE_vTaskSuspend must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * When suspended, a task will never get any microcontroller processing time,
 * no matter what its priority.
 *
 * Calls to vTaskSuspend are not accumulative -
 * i.e. calling vTaskSuspend () twice on the same task still only requires one
 * call to vTaskResume () to ready the suspended task.
 *
 * @param xTaskToSuspend Handle to the task being suspended.  Passing a NULL
 * handle will cause the calling task to be suspended.
 *
 * Example usage:
 * @code{c}
 *  void vAFunction( void )
 *  {
 *  TaskHandle_t xHandle;
 *
 *   // Create a task, storing the handle.
 *   xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );
 *
 *   // ...
 *
 *   // Use the handle to suspend the created task.
 *   vTaskSuspend( xHandle );
 *
 *   // ...
 *
 *   // The created task will not run during this period, unless
 *   // another task calls vTaskResume( xHandle ).
 *
 *   //...
 *
 *
 *   // Suspend ourselves.
 *   vTaskSuspend( NULL );
 *
 *   // We cannot get here unless another task calls vTaskResume
 *   // with our handle as the parameter.
 *  }
 * @endcode
 * \ingroup TaskCtrl
 */
void vTaskSuspend( TaskHandle_t xTaskToSuspend ) PRIVILEGED_FUNCTION;

/**
 * Resumes a suspended task.
 *
 * INCLUDE_vTaskSuspend must be defined as 1 for this function to be available.
 * See the configuration section for more information.
 *
 * A task that has been suspended by one or more calls to vTaskSuspend ()
 * will be made available for running again by a single call to
 * vTaskResume ().
 *
 * @param xTaskToResume Handle to the task being readied.
 *
 * Example usage:
 * @code{c}
 *  void vAFunction( void )
 *  {
 *  TaskHandle_t xHandle;
 *
 *   // Create a task, storing the handle.
 *   xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle );
 *
 *   // ...
 *
 *   // Use the handle to suspend the created task.
 *   vTaskSuspend( xHandle );
 *
 *   // ...
 *
 *   // The created task will not run during this period, unless
 *   // another task calls vTaskResume( xHandle ).
 *
 *   //...
 *
 *
 *   // Resume the suspended task ourselves.
 *   vTaskResume( xHandle );
 *
 *   // The created task will once again get microcontroller processing
 *   // time in accordance with its priority within the system.
 *  }
 * @endcode
 * \ingroup TaskCtrl
 */
void vTaskResume( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;

/**
 * An implementation of vTaskResume() that can be called from within an ISR.
 *
 * INCLUDE_xTaskResumeFromISR must be defined as 1 for this function to be
 * available.  See the configuration section for more information.
 *
 * A task that has been suspended by one or more calls to vTaskSuspend ()
 * will be made available for running again by a single call to
 * xTaskResumeFromISR ().
 *
 * xTaskResumeFromISR() should not be used to synchronise a task with an
 * interrupt if there is a chance that the interrupt could arrive prior to the
 * task being suspended - as this can lead to interrupts being missed. Use of a
 * semaphore as a synchronisation mechanism would avoid this eventuality.
 *
 * @param xTaskToResume Handle to the task being readied.
 *
 * @return pdTRUE if resuming the task should result in a context switch,
 * otherwise pdFALSE. This is used by the ISR to determine if a context switch
 * may be required following the ISR.
 *
 * \ingroup TaskCtrl
 */
BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume ) PRIVILEGED_FUNCTION;

/*-----------------------------------------------------------
 * SCHEDULER CONTROL
 *----------------------------------------------------------*/
/** @cond */
/**
 * Starts the real time kernel tick processing.
 *
 * After calling the kernel has control over which tasks are executed and when.
 *
 * See the demo application file main.c for an example of creating
 * tasks and starting the kernel.
 *
 * Example usage:
 * @code{c}
 *  void vAFunction( void )
 *  {
 *   // Create at least one task before starting the kernel.
 *   xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
 *
 *   // Start the real time kernel with preemption.
 *   vTaskStartScheduler ();
 *
 *   // Will not get here unless a task calls vTaskEndScheduler ()
 *  }
 * @endcode
 *
 * \ingroup SchedulerControl
 */
void vTaskStartScheduler( void ) PRIVILEGED_FUNCTION;

/**
 * Stops the real time kernel tick.
 *
 * @note At the time of writing only the x86 real mode port, which runs on a PC
 * in place of DOS, implements this function.
 *
 * All created tasks will be automatically deleted and multitasking
 * (either preemptive or cooperative) will stop.
 * Execution then resumes from the point where vTaskStartScheduler ()
 * was called, as if vTaskStartScheduler () had just returned.
 *
 * See the demo application file main. c in the demo/PC directory for an
 * example that uses vTaskEndScheduler ().
 *
 * vTaskEndScheduler () requires an exit function to be defined within the
 * portable layer (see vPortEndScheduler () in port. c for the PC port).  This
 * performs hardware specific operations such as stopping the kernel tick.
 *
 * vTaskEndScheduler () will cause all of the resources allocated by the
 * kernel to be freed - but will not free resources allocated by application
 * tasks.
 *
 * Example usage:
 * @code{c}
 *  void vTaskCode( void * pvParameters )
 *  {
 *   for( ;; )
 *   {
 *       // Task code goes here.
 *
 *       // At some point we want to end the real time kernel processing
 *       // so call ...
 *       vTaskEndScheduler ();
 *   }
 *  }
 *
 *  void vAFunction( void )
 *  {
 *   // Create at least one task before starting the kernel.
 *   xTaskCreate( vTaskCode, "NAME", STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
 *
 *   // Start the real time kernel with preemption.
 *   vTaskStartScheduler ();
 *
 *   // Will only get here when the vTaskCode () task has called
 *   // vTaskEndScheduler ().  When we get here we are back to single task
 *   // execution.
 *  }
 * @endcode
 * \ingroup SchedulerControl
 */
void vTaskEndScheduler( void ) PRIVILEGED_FUNCTION;

/** @endcond */

/**
 * Suspends the scheduler without disabling interrupts.
 *
 * Context switches will not occur while the scheduler is suspended.
 *
 * After calling vTaskSuspendAll () the calling task will continue to execute
 * without risk of being swapped out until a call to xTaskResumeAll () has been
 * made.
 *
 * API functions that have the potential to cause a context switch (for example,
 * vTaskDelayUntil(), xQueueSend(), etc.) must not be called while the scheduler
 * is suspended.
 *
 * Example usage:
 * @code{c}
 *  void vTask1( void * pvParameters )
 *  {
 *   for( ;; )
 *   {
 *       // Task code goes here.
 *
 *       // ...
 *
 *       // At some point the task wants to perform a long operation during
 *       // which it does not want to get swapped out.  It cannot use
 *       // taskENTER_CRITICAL ()/taskEXIT_CRITICAL () as the length of the
 *       // operation may cause interrupts to be missed - including the
 *       // ticks.
 *
 *       // Prevent the real time kernel swapping out the task.
 *       vTaskSuspendAll ();
 *
 *       // Perform the operation here.  There is no need to use critical
 *       // sections as we have all the microcontroller processing time.
 *       // During this time interrupts will still operate and the kernel
 *       // tick count will be maintained.
 *
 *       // ...
 *
 *       // The operation is complete.  Restart the kernel.
 *       xTaskResumeAll ();
 *   }
 *  }
 * @endcode
 * \ingroup SchedulerControl
 */
void vTaskSuspendAll( void ) PRIVILEGED_FUNCTION;

/**
 * Resumes scheduler activity after it was suspended by a call to
 * vTaskSuspendAll().
 *
 * xTaskResumeAll() only resumes the scheduler.  It does not unsuspend tasks
 * that were previously suspended by a call to vTaskSuspend().
 *
 * @return If resuming the scheduler caused a context switch then pdTRUE is
 *		  returned, otherwise pdFALSE is returned.
 *
 * Example usage:
 * @code{c}
 *  void vTask1( void * pvParameters )
 *  {
 *   for( ;; )
 *   {
 *       // Task code goes here.
 *
 *       // ...
 *
 *       // At some point the task wants to perform a long operation during
 *       // which it does not want to get swapped out.  It cannot use
 *       // taskENTER_CRITICAL ()/taskEXIT_CRITICAL () as the length of the
 *       // operation may cause interrupts to be missed - including the
 *       // ticks.
 *
 *       // Prevent the real time kernel swapping out the task.
 *       vTaskSuspendAll ();
 *
 *       // Perform the operation here.  There is no need to use critical
 *       // sections as we have all the microcontroller processing time.
 *       // During this time interrupts will still operate and the real
 *       // time kernel tick count will be maintained.
 *
 *       // ...
 *
 *       // The operation is complete.  Restart the kernel.  We want to force
 *       // a context switch - but there is no point if resuming the scheduler
 *       // caused a context switch already.
 *       if( !xTaskResumeAll () )
 *       {
 *            taskYIELD ();
 *       }
 *   }
 *  }
 * @endcode
 * \ingroup SchedulerControl
 */
BaseType_t xTaskResumeAll( void ) PRIVILEGED_FUNCTION;

/*-----------------------------------------------------------
 * TASK UTILITIES
 *----------------------------------------------------------*/

/**
 * Get tick count
 *
 * @return The count of ticks since vTaskStartScheduler was called.
 *
 * \ingroup TaskUtils
 */
TickType_t xTaskGetTickCount( void ) PRIVILEGED_FUNCTION;

/**
 * Get tick count from ISR
 *
 * @return The count of ticks since vTaskStartScheduler was called.
 *
 * This is a version of xTaskGetTickCount() that is safe to be called from an
 * ISR - provided that TickType_t is the natural word size of the
 * microcontroller being used or interrupt nesting is either not supported or
 * not being used.
 *
 * \ingroup TaskUtils
 */
TickType_t xTaskGetTickCountFromISR( void ) PRIVILEGED_FUNCTION;

/**
 * Get current number of tasks
 *
 * @return The number of tasks that the real time kernel is currently managing.
 * This includes all ready, blocked and suspended tasks.  A task that
 * has been deleted but not yet freed by the idle task will also be
 * included in the count.
 *
 * \ingroup TaskUtils
 */
UBaseType_t uxTaskGetNumberOfTasks( void ) PRIVILEGED_FUNCTION;

/**
 * Get task name
 *
 * @return The text (human readable) name of the task referenced by the handle
 * xTaskToQuery.  A task can query its own name by either passing in its own
 * handle, or by setting xTaskToQuery to NULL.  INCLUDE_pcTaskGetTaskName must be
 * set to 1 in FreeRTOSConfig.h for pcTaskGetTaskName() to be available.
 *
 * \ingroup TaskUtils
 */
char *pcTaskGetTaskName( TaskHandle_t xTaskToQuery ) PRIVILEGED_FUNCTION; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

/**
 * Returns the high water mark of the stack associated with xTask.
 *
 * INCLUDE_uxTaskGetStackHighWaterMark must be set to 1 in FreeRTOSConfig.h for
 * this function to be available.
 *
 * High water mark is the minimum free stack space there has been (in bytes
 * rather than words as found in vanilla FreeRTOS) since the task started.
 * The smaller the returned number the closer the task has come to overflowing its stack.
 *
 * @param xTask Handle of the task associated with the stack to be checked.
 * Set xTask to NULL to check the stack of the calling task.
 *
 * @return The smallest amount of free stack space there has been (in bytes
 * rather than words as found in vanilla FreeRTOS) since the task referenced by
 * xTask was created.
 */
UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/**
 * Returns the start of the stack associated with xTask.
 *
 * INCLUDE_pxTaskGetStackStart must be set to 1 in FreeRTOSConfig.h for
 * this function to be available.
 *
 * Returns the highest stack memory address on architectures where the stack grows down
 * from high memory, and the lowest memory address on architectures where the
 * stack grows up from low memory.
 *
 * @param xTask Handle of the task associated with the stack returned.
 * Set xTask to NULL to return the stack of the calling task.
 *
 * @return A pointer to the start of the stack.
 */
uint8_t* pxTaskGetStackStart( TaskHandle_t xTask) PRIVILEGED_FUNCTION;

/* When using trace macros it is sometimes necessary to include task.h before
FreeRTOS.h.  When this is done TaskHookFunction_t will not yet have been defined,
so the following two prototypes will cause a compilation error.  This can be
fixed by simply guarding against the inclusion of these two prototypes unless
they are explicitly required by the configUSE_APPLICATION_TASK_TAG configuration
constant. */
#ifdef configUSE_APPLICATION_TASK_TAG
	#if configUSE_APPLICATION_TASK_TAG == 1
		/**
		 * Sets pxHookFunction to be the task hook function used by the task xTask.
		 * @param xTask Handle of the task to set the hook function for
		 *              Passing xTask as NULL has the effect of setting the calling
		 *              tasks hook function.
		 * @param pxHookFunction  Pointer to the hook function.
		 */
		void vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction ) PRIVILEGED_FUNCTION;

		/**
		 * Get the hook function assigned to given task.
		 * @param xTask Handle of the task to get the hook function for
		 *              Passing xTask as NULL has the effect of getting the calling
		 *              tasks hook function.
		 * @return The pxHookFunction value assigned to the task xTask.
		 */
		TaskHookFunction_t xTaskGetApplicationTaskTag( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;
	#endif /* configUSE_APPLICATION_TASK_TAG ==1 */
#endif /* ifdef configUSE_APPLICATION_TASK_TAG */
#if( configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0 )

	/**
	 * Set local storage pointer specific to the given task.
	 *
	 * Each task contains an array of pointers that is dimensioned by the
	 * configNUM_THREAD_LOCAL_STORAGE_POINTERS setting in FreeRTOSConfig.h.
	 * The kernel does not use the pointers itself, so the application writer
	 * can use the pointers for any purpose they wish.
	 *
	 * @param xTaskToSet  Task to set thread local storage pointer for
	 * @param xIndex The index of the pointer to set, from 0 to
	 *               configNUM_THREAD_LOCAL_STORAGE_POINTERS - 1.
	 * @param pvValue  Pointer value to set.
	 */
	void vTaskSetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue ) PRIVILEGED_FUNCTION;


	/**
	 * Get local storage pointer specific to the given task.
	 *
	 * Each task contains an array of pointers that is dimensioned by the
	 * configNUM_THREAD_LOCAL_STORAGE_POINTERS setting in FreeRTOSConfig.h.
	 * The kernel does not use the pointers itself, so the application writer
	 * can use the pointers for any purpose they wish.
	 *
	 * @param xTaskToQuery  Task to get thread local storage pointer for
	 * @param xIndex The index of the pointer to get, from 0 to
	 *               configNUM_THREAD_LOCAL_STORAGE_POINTERS - 1.
	 * @return  Pointer value
	 */
	void *pvTaskGetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex ) PRIVILEGED_FUNCTION;

	#if ( configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS )

		/**
		 * Prototype of local storage pointer deletion callback.
		 */
		typedef void (*TlsDeleteCallbackFunction_t)( int, void * );

		/**
		 * Set local storage pointer and deletion callback.
		 *
		 * Each task contains an array of pointers that is dimensioned by the
		 * configNUM_THREAD_LOCAL_STORAGE_POINTERS setting in FreeRTOSConfig.h.
		 * The kernel does not use the pointers itself, so the application writer
		 * can use the pointers for any purpose they wish.
		 *
		 * Local storage pointers set for a task can reference dynamically
		 * allocated resources. This function is similar to
		 * vTaskSetThreadLocalStoragePointer, but provides a way to release
		 * these resources when the task gets deleted. For each pointer,
		 * a callback function can be set. This function will be called
		 * when task is deleted, with the local storage pointer index
		 * and value as arguments.
		 *
		 * @param xTaskToSet  Task to set thread local storage pointer for
		 * @param xIndex The index of the pointer to set, from 0 to
		 *               configNUM_THREAD_LOCAL_STORAGE_POINTERS - 1.
		 * @param pvValue  Pointer value to set.
		 * @param pvDelCallback  Function to call to dispose of the local
		 *                       storage pointer when the task is deleted.
		 */
		void vTaskSetThreadLocalStoragePointerAndDelCallback( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue, TlsDeleteCallbackFunction_t pvDelCallback);
	#endif

#endif

/**
 * Calls the hook function associated with xTask. Passing xTask as NULL has
 * the effect of calling the Running tasks (the calling task) hook function.
 *
 * @param xTask  Handle of the task to call the hook for.
 * @param pvParameter  Parameter passed to the hook function for the task to interpret as it
 * wants.  The return value is the value returned by the task hook function
 * registered by the user.
 */
BaseType_t xTaskCallApplicationTaskHook( TaskHandle_t xTask, void *pvParameter ) PRIVILEGED_FUNCTION;

/**
 * Get the handle of idle task for the current CPU.
 *
 * xTaskGetIdleTaskHandle() is only available if
 * INCLUDE_xTaskGetIdleTaskHandle is set to 1 in FreeRTOSConfig.h.
 *
 * @return The handle of the idle task.  It is not valid to call
 * xTaskGetIdleTaskHandle() before the scheduler has been started.
 */
TaskHandle_t xTaskGetIdleTaskHandle( void );

/**
 * Get the handle of idle task for the given CPU.
 *
 * xTaskGetIdleTaskHandleForCPU() is only available if
 * INCLUDE_xTaskGetIdleTaskHandle is set to 1 in FreeRTOSConfig.h.
 *
 * @param cpuid The CPU to get the handle for
 *
 * @return Idle task handle of a given cpu. It is not valid to call
 * xTaskGetIdleTaskHandleForCPU() before the scheduler has been started.
 */
TaskHandle_t xTaskGetIdleTaskHandleForCPU( UBaseType_t cpuid );

/**
 * Get the state of tasks in the system.
 *
 * configUSE_TRACE_FACILITY must be defined as 1 in FreeRTOSConfig.h for
 * uxTaskGetSystemState() to be available.
 *
 * uxTaskGetSystemState() populates an TaskStatus_t structure for each task in
 * the system.  TaskStatus_t structures contain, among other things, members
 * for the task handle, task name, task priority, task state, and total amount
 * of run time consumed by the task.  See the TaskStatus_t structure
 * definition in this file for the full member list.
 *
 * @note  This function is intended for debugging use only as its use results in
 * the scheduler remaining suspended for an extended period.
 *
 * @param pxTaskStatusArray A pointer to an array of TaskStatus_t structures.
 * The array must contain at least one TaskStatus_t structure for each task
 * that is under the control of the RTOS.  The number of tasks under the control
 * of the RTOS can be determined using the uxTaskGetNumberOfTasks() API function.
 *
 * @param uxArraySize The size of the array pointed to by the pxTaskStatusArray
 * parameter.  The size is specified as the number of indexes in the array, or
 * the number of TaskStatus_t structures contained in the array, not by the
 * number of bytes in the array.
 *
 * @param pulTotalRunTime If configGENERATE_RUN_TIME_STATS is set to 1 in
 * FreeRTOSConfig.h then *pulTotalRunTime is set by uxTaskGetSystemState() to the
 * total run time (as defined by the run time stats clock, see
 * http://www.freertos.org/rtos-run-time-stats.html) since the target booted.
 * pulTotalRunTime can be set to NULL to omit the total run time information.
 *
 * @return The number of TaskStatus_t structures that were populated by
 * uxTaskGetSystemState().  This should equal the number returned by the
 * uxTaskGetNumberOfTasks() API function, but will be zero if the value passed
 * in the uxArraySize parameter was too small.
 *
 * Example usage:
 * @code{c}
 * // This example demonstrates how a human readable table of run time stats
 * // information is generated from raw data provided by uxTaskGetSystemState().
 * // The human readable table is written to pcWriteBuffer
 * void vTaskGetRunTimeStats( char *pcWriteBuffer )
 * {
 * TaskStatus_t *pxTaskStatusArray;
 * volatile UBaseType_t uxArraySize, x;
 * uint32_t ulTotalRunTime, ulStatsAsPercentage;
 *
 *  // Make sure the write buffer does not contain a string.
 *  *pcWriteBuffer = 0x00;
 *
 *  // Take a snapshot of the number of tasks in case it changes while this
 *  // function is executing.
 *  uxArraySize = uxTaskGetNumberOfTasks();
 *
 *  // Allocate a TaskStatus_t structure for each task.  An array could be
 *  // allocated statically at compile time.
 *  pxTaskStatusArray = pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );
 *
 *  if( pxTaskStatusArray != NULL )
 *  {
 *      // Generate raw status information about each task.
 *      uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );
 *
 *      // For percentage calculations.
 *      ulTotalRunTime /= 100UL;
 *
 *      // Avoid divide by zero errors.
 *      if( ulTotalRunTime > 0 )
 *      {
 *          // For each populated position in the pxTaskStatusArray array,
 *          // format the raw data as human readable ASCII data
 *          for( x = 0; x < uxArraySize; x++ )
 *          {
 *              // What percentage of the total run time has the task used?
 *              // This will always be rounded down to the nearest integer.
 *              // ulTotalRunTimeDiv100 has already been divided by 100.
 *              ulStatsAsPercentage = pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;
 *
 *              if( ulStatsAsPercentage > 0UL )
 *              {
 *                  sprintf( pcWriteBuffer, "%s\t\t%lu\t\t%lu%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter, ulStatsAsPercentage );
 *              }
 *              else
 *              {
 *                  // If the percentage is zero here then the task has
 *                  // consumed less than 1% of the total run time.
 *                  sprintf( pcWriteBuffer, "%s\t\t%lu\t\t<1%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter );
 *              }
 *
 *              pcWriteBuffer += strlen( ( char * ) pcWriteBuffer );
 *          }
 *      }
 *
 *      // The array is no longer needed, free the memory it consumes.
 *      vPortFree( pxTaskStatusArray );
 *  }
 * }
 * @endcode
 */
UBaseType_t uxTaskGetSystemState( TaskStatus_t * const pxTaskStatusArray, const UBaseType_t uxArraySize, uint32_t * const pulTotalRunTime );

/**
 * List all the current tasks.
 *
 * configUSE_TRACE_FACILITY and configUSE_STATS_FORMATTING_FUNCTIONS must
 * both be defined as 1 for this function to be available.  See the
 * configuration section of the FreeRTOS.org website for more information.
 *
 * @note This function will disable interrupts for its duration.  It is
 * not intended for normal application runtime use but as a debug aid.
 *
 * Lists all the current tasks, along with their current state and stack
 * usage high water mark.
 *
 * Tasks are reported as blocked ('B'), ready ('R'), deleted ('D') or
 * suspended ('S').
 *
 * @note This function is provided for convenience only, and is used by many of the
 * demo applications.  Do not consider it to be part of the scheduler.
 *
 * vTaskList() calls uxTaskGetSystemState(), then formats part of the
 * uxTaskGetSystemState() output into a human readable table that displays task
 * names, states and stack usage.
 *
 * vTaskList() has a dependency on the sprintf() C library function that might
 * bloat the code size, use a lot of stack, and provide different results on
 * different platforms.  An alternative, tiny, third party, and limited
 * functionality implementation of sprintf() is provided in many of the
 * FreeRTOS/Demo sub-directories in a file called printf-stdarg.c (note
 * printf-stdarg.c does not provide a full snprintf() implementation!).
 *
 * It is recommended that production systems call uxTaskGetSystemState()
 * directly to get access to raw stats data, rather than indirectly through a
 * call to vTaskList().
 *
 * @param pcWriteBuffer A buffer into which the above mentioned details
 * will be written, in ASCII form.  This buffer is assumed to be large
 * enough to contain the generated report.  Approximately 40 bytes per
 * task should be sufficient.
 *
 * \ingroup TaskUtils
 */
void vTaskList( char * pcWriteBuffer ) PRIVILEGED_FUNCTION; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

/**
 * Get the state of running tasks as a string
 *
 * configGENERATE_RUN_TIME_STATS and configUSE_STATS_FORMATTING_FUNCTIONS
 * must both be defined as 1 for this function to be available.  The application
 * must also then provide definitions for
 * portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() and portGET_RUN_TIME_COUNTER_VALUE()
 * to configure a peripheral timer/counter and return the timers current count
 * value respectively.  The counter should be at least 10 times the frequency of
 * the tick count.
 *
 * @note This function will disable interrupts for its duration.  It is
 * not intended for normal application runtime use but as a debug aid.
 *
 * Setting configGENERATE_RUN_TIME_STATS to 1 will result in a total
 * accumulated execution time being stored for each task.  The resolution
 * of the accumulated time value depends on the frequency of the timer
 * configured by the portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() macro.
 * Calling vTaskGetRunTimeStats() writes the total execution time of each
 * task into a buffer, both as an absolute count value and as a percentage
 * of the total system execution time.
 *
 * @note This function is provided for convenience only, and is used by many of the
 * demo applications.  Do not consider it to be part of the scheduler.
 *
 * vTaskGetRunTimeStats() calls uxTaskGetSystemState(), then formats part of the
 * uxTaskGetSystemState() output into a human readable table that displays the
 * amount of time each task has spent in the Running state in both absolute and
 * percentage terms.
 *
 * vTaskGetRunTimeStats() has a dependency on the sprintf() C library function
 * that might bloat the code size, use a lot of stack, and provide different
 * results on different platforms.  An alternative, tiny, third party, and
 * limited functionality implementation of sprintf() is provided in many of the
 * FreeRTOS/Demo sub-directories in a file called printf-stdarg.c (note
 * printf-stdarg.c does not provide a full snprintf() implementation!).
 *
 * It is recommended that production systems call uxTaskGetSystemState() directly
 * to get access to raw stats data, rather than indirectly through a call to
 * vTaskGetRunTimeStats().
 *
 * @param pcWriteBuffer A buffer into which the execution times will be
 * written, in ASCII form.  This buffer is assumed to be large enough to
 * contain the generated report.  Approximately 40 bytes per task should
 * be sufficient.
 *
 * \ingroup TaskUtils
 */
void vTaskGetRunTimeStats( char *pcWriteBuffer ) PRIVILEGED_FUNCTION; /*lint !e971 Unqualified char types are allowed for strings and single characters only. */

/**
 * Send task notification.
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this
 * function to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * A notification sent to a task will remain pending until it is cleared by the
 * task calling xTaskNotifyWait() or ulTaskNotifyTake().  If the task was
 * already in the Blocked state to wait for a notification when the notification
 * arrives then the task will automatically be removed from the Blocked state
 * (unblocked) and the notification cleared.
 *
 * A task can use xTaskNotifyWait() to [optionally] block to wait for a
 * notification to be pending, or ulTaskNotifyTake() to [optionally] block
 * to wait for its notification value to have a non-zero value.  The task does
 * not consume any CPU time while it is in the Blocked state.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for details.
 *
 * @param xTaskToNotify The handle of the task being notified.  The handle to a
 * task can be returned from the xTaskCreate() API function used to create the
 * task, and the handle of the currently running task can be obtained by calling
 * xTaskGetCurrentTaskHandle().
 *
 * @param ulValue Data that can be sent with the notification.  How the data is
 * used depends on the value of the eAction parameter.
 *
 * @param eAction Specifies how the notification updates the task's notification
 * value, if at all.  Valid values for eAction are as follows:
 *	- eSetBits:
 *	  The task's notification value is bitwise ORed with ulValue.  xTaskNofify()
 * 	  always returns pdPASS in this case.
 *
 *	- eIncrement:
 *	  The task's notification value is incremented.  ulValue is not used and
 *	  xTaskNotify() always returns pdPASS in this case.
 *
 *	- eSetValueWithOverwrite:
 *	  The task's notification value is set to the value of ulValue, even if the
 *	  task being notified had not yet processed the previous notification (the
 *	  task already had a notification pending).  xTaskNotify() always returns
 *	  pdPASS in this case.
 *
 *	- eSetValueWithoutOverwrite:
 *	  If the task being notified did not already have a notification pending then
 *	  the task's notification value is set to ulValue and xTaskNotify() will
 *	  return pdPASS.  If the task being notified already had a notification
 *	  pending then no action is performed and pdFAIL is returned.
 *
 *	- eNoAction:
 *	  The task receives a notification without its notification value being
 *	  updated.  ulValue is not used and xTaskNotify() always returns pdPASS in
 *	  this case.
 *
 * @return Dependent on the value of eAction.  See the description of the
 * eAction parameter.
 *
 * \ingroup TaskNotifications
 */
BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction );

/**
 * Send task notification from an ISR.
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this
 * function to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * A version of xTaskNotify() that can be used from an interrupt service routine
 * (ISR).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * A notification sent to a task will remain pending until it is cleared by the
 * task calling xTaskNotifyWait() or ulTaskNotifyTake().  If the task was
 * already in the Blocked state to wait for a notification when the notification
 * arrives then the task will automatically be removed from the Blocked state
 * (unblocked) and the notification cleared.
 *
 * A task can use xTaskNotifyWait() to [optionally] block to wait for a
 * notification to be pending, or ulTaskNotifyTake() to [optionally] block
 * to wait for its notification value to have a non-zero value.  The task does
 * not consume any CPU time while it is in the Blocked state.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for details.
 *
 * @param xTaskToNotify The handle of the task being notified.  The handle to a
 * task can be returned from the xTaskCreate() API function used to create the
 * task, and the handle of the currently running task can be obtained by calling
 * xTaskGetCurrentTaskHandle().
 *
 * @param ulValue Data that can be sent with the notification.  How the data is
 * used depends on the value of the eAction parameter.
 *
 * @param eAction Specifies how the notification updates the task's notification
 * value, if at all.  Valid values for eAction are as follows:
 *	- eSetBits:
 *	  The task's notification value is bitwise ORed with ulValue.  xTaskNofify()
 * 	  always returns pdPASS in this case.
 *
 *	- eIncrement:
 *	  The task's notification value is incremented.  ulValue is not used and
 *	  xTaskNotify() always returns pdPASS in this case.
 *
 *	- eSetValueWithOverwrite:
 *	  The task's notification value is set to the value of ulValue, even if the
 *	  task being notified had not yet processed the previous notification (the
 *	  task already had a notification pending).  xTaskNotify() always returns
 *	  pdPASS in this case.
 *
 *	- eSetValueWithoutOverwrite:
 *	  If the task being notified did not already have a notification pending then
 *	  the task's notification value is set to ulValue and xTaskNotify() will
 *	  return pdPASS.  If the task being notified already had a notification
 *	  pending then no action is performed and pdFAIL is returned.
 *
 *	- eNoAction:
 *	  The task receives a notification without its notification value being
 *	  updated.  ulValue is not used and xTaskNotify() always returns pdPASS in
 *	  this case.
 *
 * @param pxHigherPriorityTaskWoken  xTaskNotifyFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending the notification caused the
 * task to which the notification was sent to leave the Blocked state, and the
 * unblocked task has a priority higher than the currently running task.  If
 * xTaskNotifyFromISR() sets this value to pdTRUE then a context switch should
 * be requested before the interrupt is exited.  How a context switch is
 * requested from an ISR is dependent on the port - see the documentation page
 * for the port in use.
 *
 * @return Dependent on the value of eAction.  See the description of the
 * eAction parameter.
 *
 * \ingroup TaskNotifications
 */
BaseType_t xTaskNotifyFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken );

/**
 * Wait for task notification
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this
 * function to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * A notification sent to a task will remain pending until it is cleared by the
 * task calling xTaskNotifyWait() or ulTaskNotifyTake().  If the task was
 * already in the Blocked state to wait for a notification when the notification
 * arrives then the task will automatically be removed from the Blocked state
 * (unblocked) and the notification cleared.
 *
 * A task can use xTaskNotifyWait() to [optionally] block to wait for a
 * notification to be pending, or ulTaskNotifyTake() to [optionally] block
 * to wait for its notification value to have a non-zero value.  The task does
 * not consume any CPU time while it is in the Blocked state.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for details.
 *
 * @param ulBitsToClearOnEntry Bits that are set in ulBitsToClearOnEntry value
 * will be cleared in the calling task's notification value before the task
 * checks to see if any notifications are pending, and optionally blocks if no
 * notifications are pending.  Setting ulBitsToClearOnEntry to ULONG_MAX (if
 * limits.h is included) or 0xffffffffUL (if limits.h is not included) will have
 * the effect of resetting the task's notification value to 0.  Setting
 * ulBitsToClearOnEntry to 0 will leave the task's notification value unchanged.
 *
 * @param ulBitsToClearOnExit If a notification is pending or received before
 * the calling task exits the xTaskNotifyWait() function then the task's
 * notification value (see the xTaskNotify() API function) is passed out using
 * the pulNotificationValue parameter.  Then any bits that are set in
 * ulBitsToClearOnExit will be cleared in the task's notification value (note
 * *pulNotificationValue is set before any bits are cleared).  Setting
 * ulBitsToClearOnExit to ULONG_MAX (if limits.h is included) or 0xffffffffUL
 * (if limits.h is not included) will have the effect of resetting the task's
 * notification value to 0 before the function exits.  Setting
 * ulBitsToClearOnExit to 0 will leave the task's notification value unchanged
 * when the function exits (in which case the value passed out in
 * pulNotificationValue will match the task's notification value).
 *
 * @param pulNotificationValue Used to pass the task's notification value out
 * of the function.  Note the value passed out will not be effected by the
 * clearing of any bits caused by ulBitsToClearOnExit being non-zero.
 *
 * @param xTicksToWait The maximum amount of time that the task should wait in
 * the Blocked state for a notification to be received, should a notification
 * not already be pending when xTaskNotifyWait() was called.  The task
 * will not consume any processing time while it is in the Blocked state.  This
 * is specified in kernel ticks, the macro pdMS_TO_TICSK( value_in_ms ) can be
 * used to convert a time specified in milliseconds to a time specified in
 * ticks.
 *
 * @return If a notification was received (including notifications that were
 * already pending when xTaskNotifyWait was called) then pdPASS is
 * returned.  Otherwise pdFAIL is returned.
 *
 * \ingroup TaskNotifications
 */
BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait );

/**
 * Simplified macro for sending task notification.
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this macro
 * to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * xTaskNotifyGive() is a helper macro intended for use when task notifications
 * are used as light weight and faster binary or counting semaphore equivalents.
 * Actual FreeRTOS semaphores are given using the xSemaphoreGive() API function,
 * the equivalent action that instead uses a task notification is
 * xTaskNotifyGive().
 *
 * When task notifications are being used as a binary or counting semaphore
 * equivalent then the task being notified should wait for the notification
 * using the ulTaskNotificationTake() API function rather than the
 * xTaskNotifyWait() API function.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for more details.
 *
 * @param xTaskToNotify The handle of the task being notified.  The handle to a
 * task can be returned from the xTaskCreate() API function used to create the
 * task, and the handle of the currently running task can be obtained by calling
 * xTaskGetCurrentTaskHandle().
 *
 * @return xTaskNotifyGive() is a macro that calls xTaskNotify() with the
 * eAction parameter set to eIncrement - so pdPASS is always returned.
 *
 * \ingroup TaskNotifications
 */
#define xTaskNotifyGive( xTaskToNotify ) xTaskNotify( ( xTaskToNotify ), 0, eIncrement );

/**
 * Simplified macro for sending task notification from ISR.
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this macro
 * to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * A version of xTaskNotifyGive() that can be called from an interrupt service
 * routine (ISR).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * vTaskNotifyGiveFromISR() is intended for use when task notifications are
 * used as light weight and faster binary or counting semaphore equivalents.
 * Actual FreeRTOS semaphores are given from an ISR using the
 * xSemaphoreGiveFromISR() API function, the equivalent action that instead uses
 * a task notification is vTaskNotifyGiveFromISR().
 *
 * When task notifications are being used as a binary or counting semaphore
 * equivalent then the task being notified should wait for the notification
 * using the ulTaskNotificationTake() API function rather than the
 * xTaskNotifyWait() API function.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for more details.
 *
 * @param xTaskToNotify The handle of the task being notified.  The handle to a
 * task can be returned from the xTaskCreate() API function used to create the
 * task, and the handle of the currently running task can be obtained by calling
 * xTaskGetCurrentTaskHandle().
 *
 * @param pxHigherPriorityTaskWoken  vTaskNotifyGiveFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending the notification caused the
 * task to which the notification was sent to leave the Blocked state, and the
 * unblocked task has a priority higher than the currently running task.  If
 * vTaskNotifyGiveFromISR() sets this value to pdTRUE then a context switch
 * should be requested before the interrupt is exited.  How a context switch is
 * requested from an ISR is dependent on the port - see the documentation page
 * for the port in use.
 *
 * \ingroup TaskNotifications
 */
void vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken );

/**
 * Simplified macro for receiving task notification.
 *
 * configUSE_TASK_NOTIFICATIONS must be undefined or defined as 1 for this
 * function to be available.
 *
 * When configUSE_TASK_NOTIFICATIONS is set to one each task has its own private
 * "notification value", which is a 32-bit unsigned integer (uint32_t).
 *
 * Events can be sent to a task using an intermediary object.  Examples of such
 * objects are queues, semaphores, mutexes and event groups.  Task notifications
 * are a method of sending an event directly to a task without the need for such
 * an intermediary object.
 *
 * A notification sent to a task can optionally perform an action, such as
 * update, overwrite or increment the task's notification value.  In that way
 * task notifications can be used to send data to a task, or be used as light
 * weight and fast binary or counting semaphores.
 *
 * ulTaskNotifyTake() is intended for use when a task notification is used as a
 * faster and lighter weight binary or counting semaphore alternative.  Actual
 * FreeRTOS semaphores are taken using the xSemaphoreTake() API function, the
 * equivalent action that instead uses a task notification is
 * ulTaskNotifyTake().
 *
 * When a task is using its notification value as a binary or counting semaphore
 * other tasks should send notifications to it using the xTaskNotifyGive()
 * macro, or xTaskNotify() function with the eAction parameter set to
 * eIncrement.
 *
 * ulTaskNotifyTake() can either clear the task's notification value to
 * zero on exit, in which case the notification value acts like a binary
 * semaphore, or decrement the task's notification value on exit, in which case
 * the notification value acts like a counting semaphore.
 *
 * A task can use ulTaskNotifyTake() to [optionally] block to wait for a
 * the task's notification value to be non-zero.  The task does not consume any
 * CPU time while it is in the Blocked state.
 *
 * Where as xTaskNotifyWait() will return when a notification is pending,
 * ulTaskNotifyTake() will return when the task's notification value is
 * not zero.
 *
 * See http://www.FreeRTOS.org/RTOS-task-notifications.html for details.
 *
 * @param xClearCountOnExit if xClearCountOnExit is pdFALSE then the task's
 * notification value is decremented when the function exits.  In this way the
 * notification value acts like a counting semaphore.  If xClearCountOnExit is
 * not pdFALSE then the task's notification value is cleared to zero when the
 * function exits.  In this way the notification value acts like a binary
 * semaphore.
 *
 * @param xTicksToWait The maximum amount of time that the task should wait in
 * the Blocked state for the task's notification value to be greater than zero,
 * should the count not already be greater than zero when
 * ulTaskNotifyTake() was called.  The task will not consume any processing
 * time while it is in the Blocked state.  This is specified in kernel ticks,
 * the macro pdMS_TO_TICSK( value_in_ms ) can be used to convert a time
 * specified in milliseconds to a time specified in ticks.
 *
 * @return The task's notification count before it is either cleared to zero or
 * decremented (see the xClearCountOnExit parameter).
 *
 * \ingroup TaskNotifications
 */
uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait );

/*-----------------------------------------------------------
 * SCHEDULER INTERNALS AVAILABLE FOR PORTING PURPOSES
 *----------------------------------------------------------*/
/** @cond */
/*
 * THIS FUNCTION MUST NOT BE USED FROM APPLICATION CODE.  IT IS ONLY
 * INTENDED FOR USE WHEN IMPLEMENTING A PORT OF THE SCHEDULER AND IS
 * AN INTERFACE WHICH IS FOR THE EXCLUSIVE USE OF THE SCHEDULER.
 *
 * Called from the real time kernel tick (either preemptive or cooperative),
 * this increments the tick count and checks if any tasks that are blocked
 * for a finite period required removing from a blocked list and placing on
 * a ready list.  If a non-zero value is returned then a context switch is
 * required because either:
 *   + A task was removed from a blocked list because its timeout had expired,
 *     or
 *   + Time slicing is in use and there is a task of equal priority to the
 *     currently running task.
 */
BaseType_t xTaskIncrementTick( void ) PRIVILEGED_FUNCTION;

/*
 * THIS FUNCTION MUST NOT BE USED FROM APPLICATION CODE.  IT IS AN
 * INTERFACE WHICH IS FOR THE EXCLUSIVE USE OF THE SCHEDULER.
 *
 * THIS FUNCTION MUST BE CALLED WITH INTERRUPTS DISABLED.
 *
 * Removes the calling task from the ready list and places it both
 * on the list of tasks waiting for a particular event, and the
 * list of delayed tasks.  The task will be removed from both lists
 * and replaced on the ready list should either the event occur (and
 * there be no higher priority tasks waiting on the same event) or
 * the delay period expires.
 *
 * The 'unordered' version replaces the event list item value with the
 * xItemValue value, and inserts the list item at the end of the list.
 *
 * The 'ordered' version uses the existing event list item value (which is the
 * owning tasks priority) to insert the list item into the event list is task
 * priority order.
 *
 * @param pxEventList The list containing tasks that are blocked waiting
 * for the event to occur.
 *
 * @param xItemValue The item value to use for the event list item when the
 * event list is not ordered by task priority.
 *
 * @param xTicksToWait The maximum amount of time that the task should wait
 * for the event to occur.  This is specified in kernel ticks,the constant
 * portTICK_PERIOD_MS can be used to convert kernel ticks into a real time
 * period.
 */
void vTaskPlaceOnEventList( List_t * const pxEventList, const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;
void vTaskPlaceOnUnorderedEventList( List_t * pxEventList, const TickType_t xItemValue, const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

/*
 * THIS FUNCTION MUST NOT BE USED FROM APPLICATION CODE.  IT IS AN
 * INTERFACE WHICH IS FOR THE EXCLUSIVE USE OF THE SCHEDULER.
 *
 * THIS FUNCTION MUST BE CALLED WITH INTERRUPTS DISABLED.
 *
 * This function performs nearly the same function as vTaskPlaceOnEventList().
 * The difference being that this function does not permit tasks to block
 * indefinitely, whereas vTaskPlaceOnEventList() does.
 *
 */
void vTaskPlaceOnEventListRestricted( List_t * const pxEventList, const TickType_t xTicksToWait ) PRIVILEGED_FUNCTION;

/*
 * THIS FUNCTION MUST NOT BE USED FROM APPLICATION CODE.  IT IS AN
 * INTERFACE WHICH IS FOR THE EXCLUSIVE USE OF THE SCHEDULER.
 *
 * THIS FUNCTION MUST BE CALLED WITH INTERRUPTS DISABLED.
 *
 * Removes a task from both the specified event list and the list of blocked
 * tasks, and places it on a ready queue.
 *
 * xTaskRemoveFromEventList()/xTaskRemoveFromUnorderedEventList() will be called
 * if either an event occurs to unblock a task, or the block timeout period
 * expires.
 *
 * xTaskRemoveFromEventList() is used when the event list is in task priority
 * order.  It removes the list item from the head of the event list as that will
 * have the highest priority owning task of all the tasks on the event list.
 * xTaskRemoveFromUnorderedEventList() is used when the event list is not
 * ordered and the event list items hold something other than the owning tasks
 * priority.  In this case the event list item value is updated to the value
 * passed in the xItemValue parameter.
 *
 * @return pdTRUE if the task being removed has a higher priority than the task
 * making the call, otherwise pdFALSE.
 */
BaseType_t xTaskRemoveFromEventList( const List_t * const pxEventList ) PRIVILEGED_FUNCTION;
BaseType_t xTaskRemoveFromUnorderedEventList( ListItem_t * pxEventListItem, const TickType_t xItemValue ) PRIVILEGED_FUNCTION;

/*
 * THIS FUNCTION MUST NOT BE USED FROM APPLICATION CODE.  IT IS ONLY
 * INTENDED FOR USE WHEN IMPLEMENTING A PORT OF THE SCHEDULER AND IS
 * AN INTERFACE WHICH IS FOR THE EXCLUSIVE USE OF THE SCHEDULER.
 *
 * Sets the pointer to the current TCB to the TCB of the highest priority task
 * that is ready to run.
 */
void vTaskSwitchContext( void ) PRIVILEGED_FUNCTION;

/*
 * THESE FUNCTIONS MUST NOT BE USED FROM APPLICATION CODE.  THEY ARE USED BY
 * THE EVENT BITS MODULE.
 */
TickType_t uxTaskResetEventItemValue( void ) PRIVILEGED_FUNCTION;

/*
 * Return the handle of the calling task.
 */
TaskHandle_t xTaskGetCurrentTaskHandle( void ) PRIVILEGED_FUNCTION;



/*
 * Return the handle of the task running on a certain CPU. Because of
 * the nature of SMP processing, there is no guarantee that this
 * value will still be valid on return and should only be used for
 * debugging purposes.
 */
TaskHandle_t xTaskGetCurrentTaskHandleForCPU( BaseType_t cpuid );


/*
 * Capture the current time status for future reference.
 */
void vTaskSetTimeOutState( TimeOut_t * const pxTimeOut ) PRIVILEGED_FUNCTION;

/*
 * Compare the time status now with that previously captured to see if the
 * timeout has expired.
 */
BaseType_t xTaskCheckForTimeOut( TimeOut_t * const pxTimeOut, TickType_t * const pxTicksToWait ) PRIVILEGED_FUNCTION;

/*
 * Shortcut used by the queue implementation to prevent unnecessary call to
 * taskYIELD();
 */
void vTaskMissedYield( void ) PRIVILEGED_FUNCTION;

/*
 * Returns the scheduler state as taskSCHEDULER_RUNNING,
 * taskSCHEDULER_NOT_STARTED or taskSCHEDULER_SUSPENDED.
 */
BaseType_t xTaskGetSchedulerState( void ) PRIVILEGED_FUNCTION;

/*
 * Raises the priority of the mutex holder to that of the calling task should
 * the mutex holder have a priority less than the calling task.
 */
void vTaskPriorityInherit( TaskHandle_t const pxMutexHolder ) PRIVILEGED_FUNCTION;

/*
 * Set the priority of a task back to its proper priority in the case that it
 * inherited a higher priority while it was holding a semaphore.
 */
BaseType_t xTaskPriorityDisinherit( TaskHandle_t const pxMutexHolder ) PRIVILEGED_FUNCTION;

/*
 * Get the uxTCBNumber assigned to the task referenced by the xTask parameter.
 */
UBaseType_t uxTaskGetTaskNumber( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;


/*
 * Get the current core affinity of a task
 */
BaseType_t xTaskGetAffinity( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/*
 * Set the uxTaskNumber of the task referenced by the xTask parameter to
 * uxHandle.
 */
void vTaskSetTaskNumber( TaskHandle_t xTask, const UBaseType_t uxHandle ) PRIVILEGED_FUNCTION;

/*
 * Only available when configUSE_TICKLESS_IDLE is set to 1.
 * If tickless mode is being used, or a low power mode is implemented, then
 * the tick interrupt will not execute during idle periods.  When this is the
 * case, the tick count value maintained by the scheduler needs to be kept up
 * to date with the actual execution time by being skipped forward by a time
 * equal to the idle period.
 */
void vTaskStepTick( const TickType_t xTicksToJump ) PRIVILEGED_FUNCTION;

/*
 * Only avilable when configUSE_TICKLESS_IDLE is set to 1.
 * Provided for use within portSUPPRESS_TICKS_AND_SLEEP() to allow the port
 * specific sleep function to determine if it is ok to proceed with the sleep,
 * and if it is ok to proceed, if it is ok to sleep indefinitely.
 *
 * This function is necessary because portSUPPRESS_TICKS_AND_SLEEP() is only
 * called with the scheduler suspended, not from within a critical section.  It
 * is therefore possible for an interrupt to request a context switch between
 * portSUPPRESS_TICKS_AND_SLEEP() and the low power mode actually being
 * entered.  eTaskConfirmSleepModeStatus() should be called from a short
 * critical section between the timer being stopped and the sleep mode being
 * entered to ensure it is ok to proceed into the sleep mode.
 */
eSleepModeStatus eTaskConfirmSleepModeStatus( void ) PRIVILEGED_FUNCTION;

/*
 * For internal use only.  Increment the mutex held count when a mutex is
 * taken and return the handle of the task that has taken the mutex.
 */
void *pvTaskIncrementMutexHeldCount( void );

/*
 * This function fills array with TaskSnapshot_t structures for every task in the system.
 * Used by core dump facility to get snapshots of all tasks in the system.
 * Only available when configENABLE_TASK_SNAPSHOT is set to 1.
 * @param pxTaskSnapshotArray Pointer to array of TaskSnapshot_t structures to store tasks snapshot data.
 * @param uxArraySize Size of tasks snapshots array.
 * @param pxTcbSz Pointer to store size of TCB.
 * @return Number of elements stored in array.
 */
UBaseType_t uxTaskGetSnapshotAll( TaskSnapshot_t * const pxTaskSnapshotArray, const UBaseType_t uxArraySize, UBaseType_t * const pxTcbSz );

/** @endcond */

#ifdef __cplusplus
}
#endif
#endif /* INC_TASK_H */



