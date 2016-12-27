/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__

#include <stdint.h>

#include <xtensa/tie/xt_core.h>
#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>	/* required for XSHAL_CLIB */
#include <xtensa/xtruntime.h>

//#include "xtensa_context.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */

#define portCHAR		int8_t
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		int32_t
#define portSHORT		int16_t
#define portSTACK_TYPE	uint8_t
#define portBASE_TYPE	int

typedef portSTACK_TYPE			StackType_t;
typedef portBASE_TYPE			BaseType_t;
typedef unsigned portBASE_TYPE	UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif
/*-----------------------------------------------------------*/

// portbenchmark
#include "portbenchmark.h"

#include "sdkconfig.h"

#define portFIRST_TASK_HOOK CONFIG_FREERTOS_BREAK_ON_SCHEDULER_START_JTAG


typedef struct {
	volatile uint32_t mux;
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
	const char *lastLockedFn;
	int lastLockedLine;
#endif
} portMUX_TYPE;

 /*
  * Kernel mux values can be:
  * 0 - Uninitialized
  * (0-portNUM_PROCESSORS)|(recCnt<<8)|0xB33F0000 - taken by core (val), recurse count is (recCnt)
  * 0xB33FFFFF - free
  *
  * The magic number in the top 16 bits is there so we can detect uninitialized and corrupted muxes.
  */

#define portMUX_MAGIC_VAL		0xB33F0000
#define portMUX_FREE_VAL		0xB33FFFFF
#define portMUX_MAGIC_MASK		0xFFFF0000
#define portMUX_MAGIC_SHIFT		16
#define portMUX_CNT_MASK		0x0000FF00
#define portMUX_CNT_SHIFT		8
#define portMUX_VAL_MASK		0x000000FF
#define portMUX_VAL_SHIFT		0

//Keep this in sync with the portMUX_TYPE struct definition please.
#ifndef CONFIG_FREERTOS_PORTMUX_DEBUG
#define portMUX_INITIALIZER_UNLOCKED { 					\
		.mux = portMUX_MAGIC_VAL|portMUX_FREE_VAL 		\
	}
#else
#define portMUX_INITIALIZER_UNLOCKED { 					\
		.mux = portMUX_MAGIC_VAL|portMUX_FREE_VAL, 		\
		.lastLockedFn = "(never locked)", 				\
		.lastLockedLine = -1							\
	}
#endif

/* Critical section management. NW-TODO: replace XTOS_SET_INTLEVEL with more efficient version, if any? */
// These cannot be nested. They should be used with a lot of care and cannot be called from interrupt level.
#define portDISABLE_INTERRUPTS()      do { XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); portbenchmarkINTERRUPT_DISABLE(); } while (0)
#define portENABLE_INTERRUPTS()       do { portbenchmarkINTERRUPT_RESTORE(0); XTOS_SET_INTLEVEL(0); } while (0)

#define portASSERT_IF_IN_ISR()        vPortAssertIfInISR()
void vPortAssertIfInISR();

#define portCRITICAL_NESTING_IN_TCB 1 

/*
Modifications to portENTER_CRITICAL:

The original portENTER_CRITICAL only disabled the ISRs. This is enough for single-CPU operation: by 
disabling the interrupts, there is no task switch so no other tasks can meddle in the data, and because
interrupts are disabled, ISRs can't corrupt data structures either.

For multiprocessing, things get a bit more hairy. First of all, disabling the interrupts doesn't stop
the tasks or ISRs on the other processors meddling with our CPU. For tasks, this is solved by adding
a spinlock to the portENTER_CRITICAL macro. A task running on the other CPU accessing the same data will
spinlock in the portENTER_CRITICAL code until the first CPU is done.

For ISRs, we now also need muxes: while portENTER_CRITICAL disabling interrupts will stop ISRs on the same
CPU from meddling with the data, it does not stop interrupts on the other cores from interfering with the
data. For this, we also use a spinlock in the routines called by the ISR, but these spinlocks
do not disable the interrupts (because they already are).

This all assumes that interrupts are either entirely disabled or enabled. Interrupr priority levels
will break this scheme.
*/
void vPortCPUInitializeMutex(portMUX_TYPE *mux);
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
void vPortCPUAcquireMutex(portMUX_TYPE *mux, const char *function, int line);
portBASE_TYPE vPortCPUReleaseMutex(portMUX_TYPE *mux, const char *function, int line);
void vTaskEnterCritical( portMUX_TYPE *mux, const char *function, int line );
void vTaskExitCritical( portMUX_TYPE *mux, const char *function, int line );
#define portENTER_CRITICAL(mux)        vTaskEnterCritical(mux, __FUNCTION__, __LINE__)
#define portEXIT_CRITICAL(mux)         vTaskExitCritical(mux, __FUNCTION__, __LINE__)
#define portENTER_CRITICAL_ISR(mux)    vPortCPUAcquireMutex(mux, __FUNCTION__, __LINE__)
#define portEXIT_CRITICAL_ISR(mux)    vPortCPUReleaseMutex(mux, __FUNCTION__, __LINE__)
#else
void vTaskExitCritical( portMUX_TYPE *mux );
void vTaskEnterCritical( portMUX_TYPE *mux );
void vPortCPUAcquireMutex(portMUX_TYPE *mux);
portBASE_TYPE vPortCPUReleaseMutex(portMUX_TYPE *mux);
#define portENTER_CRITICAL(mux)        vTaskEnterCritical(mux)
#define portEXIT_CRITICAL(mux)         vTaskExitCritical(mux)
#define portENTER_CRITICAL_ISR(mux)    vPortCPUAcquireMutex(mux)
#define portEXIT_CRITICAL_ISR(mux)    vPortCPUReleaseMutex(mux)
#endif

// Cleaner and preferred solution allows nested interrupts disabling and restoring via local registers or stack.
// They can be called from interrupts too.
//NOT SMP-COMPATIBLE! Use only if all you want is to disable the interrupts locally!
static inline unsigned portENTER_CRITICAL_NESTED() { unsigned state = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); portbenchmarkINTERRUPT_DISABLE(); return state; }
#define portEXIT_CRITICAL_NESTED(state)   do { portbenchmarkINTERRUPT_RESTORE(state); XTOS_RESTORE_JUST_INTLEVEL(state); } while (0)

// These FreeRTOS versions are similar to the nested versions above
#define portSET_INTERRUPT_MASK_FROM_ISR()            portENTER_CRITICAL_NESTED()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(state)     portEXIT_CRITICAL_NESTED(state)


/*
 * Wrapper for the Xtensa compare-and-set instruction. This subroutine will atomically compare
 * *mux to compare, and if it's the same, will set *mux to set. It will return the old value
 * of *addr in *set.
 *
 * Warning: From the ISA docs: in some (unspecified) cases, the s32c1i instruction may return the
 * *bitwise inverse* of the old mem if the mem wasn't written. This doesn't seem to happen on the
 * ESP32, though. (Would show up directly if it did because the magic wouldn't match.)
 */
static inline void uxPortCompareSet(volatile uint32_t *addr, uint32_t compare, uint32_t *set) {
    __asm__ __volatile__(
        "WSR 	    %2,SCOMPARE1 \n"
        "ISYNC      \n"
        "S32C1I     %0, %1, 0	 \n"
        :"=r"(*set)
        :"r"(addr), "r"(compare), "0"(*set)
        );
}


/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			4
#define portNOP()					XT_NOP()
/*-----------------------------------------------------------*/

/* Fine resolution time */
#define portGET_RUN_TIME_COUNTER_VALUE()  xthal_get_ccount()

/* Kernel utilities. */
void vPortYield( void );
void _frxt_setup_switch( void );
#define portYIELD()					vPortYield()
#define portYIELD_FROM_ISR()		_frxt_setup_switch()
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

// When coprocessors are defined, we to maintain a pointer to coprocessors area.
// We currently use a hack: redefine field xMPU_SETTINGS in TCB block as a structure that can hold:
// MPU wrappers, coprocessor area pointer, trace code structure, and more if needed.
// The field is normally used for memory protection. FreeRTOS should create another general purpose field.
typedef struct {
	#if XCHAL_CP_NUM > 0
	volatile StackType_t* coproc_area; // Pointer to coprocessor save area; MUST BE FIRST
	#endif

	#if portUSING_MPU_WRAPPERS
	// Define here mpu_settings, which is port dependent
	int mpu_setting; // Just a dummy example here; MPU not ported to Xtensa yet
	#endif

	#if configUSE_TRACE_FACILITY_2
	struct {
		// Cf. porttraceStamp()
		int taskstamp;        /* Stamp from inside task to see where we are */
		int taskstampcount;   /* A counter usually incremented when we restart the task's loop */
	} porttrace;
	#endif
} xMPU_SETTINGS;

// Main hack to use MPU_wrappers even when no MPU is defined (warning: mpu_setting should not be accessed; otherwise move this above xMPU_SETTINGS)
#if (XCHAL_CP_NUM > 0 || configUSE_TRACE_FACILITY_2) && !portUSING_MPU_WRAPPERS   // If MPU wrappers not used, we still need to allocate coproc area
	#undef portUSING_MPU_WRAPPERS
	#define portUSING_MPU_WRAPPERS 1   // Enable it to allocate coproc area
	#define MPU_WRAPPERS_H             // Override mpu_wrapper.h to disable unwanted code
	#define PRIVILEGED_FUNCTION
	#define PRIVILEGED_DATA
#endif

// porttrace
#if configUSE_TRACE_FACILITY_2
#include "porttrace.h"
#endif

// configASSERT_2 if requested
#if configASSERT_2
#include <stdio.h>
void exit(int);
#define configASSERT( x )   if (!(x)) { porttracePrint(-1); printf("\nAssertion failed in %s:%d\n", __FILE__, __LINE__); exit(-1); }
#endif

#endif // __ASSEMBLER__

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

