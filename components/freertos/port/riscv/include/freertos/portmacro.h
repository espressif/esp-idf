/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>
#include "esp_timer.h"              /* required for FreeRTOS run time stats */

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#ifdef CONFIG_LEGACY_INCLUDE_COMMON_HEADERS
#include "soc/soc_memory_layout.h"
#endif
#include "soc/spinlock.h"
#include "soc/interrupt_core0_reg.h"
#include "esp_rom_sys.h"
#include "soc/cpu.h"
#include "esp_system.h"
#include "esp_newlib.h"

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
#define portCHAR		uint8_t
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		int32_t
#define portSHORT		int16_t
#define portSTACK_TYPE	uint8_t
#define portBASE_TYPE	int
// interrupt module will mask interrupt with priority less than threshold
#define RVHAL_EXCM_LEVEL    4

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
/*------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) (1000 / configTICK_RATE_HZ) )
#define portBYTE_ALIGNMENT			16
/*-----------------------------------------------------------*/
#include "portbenchmark.h"

static inline BaseType_t IRAM_ATTR xPortGetCoreID(void) {
    return cpu_hal_get_core_id();
}


static inline bool IRAM_ATTR xPortCanYield(void)
{
    uint32_t threshold = REG_READ(INTERRUPT_CORE0_CPU_INT_THRESH_REG);
    /* when enter critical code, freertos will mask threshold to RVHAL_EXCM_LEVEL
     * and exit critical code, will recover threshold value (1). so threshold <= 1
     * means not in critical code
     */
    return (threshold <= 1);
}

static inline void uxPortCompareSetExtram(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
#if defined(CONFIG_SPIRAM)
    compare_and_set_extram(addr, compare, set);
#endif
}

static inline void __attribute__((always_inline)) uxPortCompareSet(volatile uint32_t *addr, uint32_t compare, uint32_t *set) {
    compare_and_set_native(addr, compare, set);
}

#define portCRITICAL_NESTING_IN_TCB 0

/*
 * Send an interrupt to another core in order to make the task running
 * on it yield for a higher-priority task.
 */
void vPortYieldOtherCore( BaseType_t coreid);

/*
 Callback to set a watchpoint on the end of the stack. Called every context switch to change the stack
 watchpoint around.
 */
void vPortSetStackWatchpoint( void* pxStackStart );

/*
 * Returns true if the current core is in ISR context; low prio ISR, med prio ISR or timer tick ISR. High prio ISRs
 * aren't detected here, but they normally cannot call C code, so that should not be an issue anyway.
 */
BaseType_t xPortInIsrContext(void);

/*
 * This function will be called in High prio ISRs. Returns true if the current core was in ISR context
 * before calling into high prio ISR context.
 */
BaseType_t xPortInterruptedFromISRContext(void);

/* "mux" data structure (spinlock) */
typedef struct {
	/* owner field values:
	 * 0				- Uninitialized (invalid)
	 * portMUX_FREE_VAL - Mux is free, can be locked by either CPU
	 * CORE_ID_REGVAL_PRO / CORE_ID_REGVAL_APP - Mux is locked to the particular core
	 *
	 *
	 * Any value other than portMUX_FREE_VAL, CORE_ID_REGVAL_PRO, CORE_ID_REGVAL_APP indicates corruption
	 */
	uint32_t owner;
	/* count field:
	 * If mux is unlocked, count should be zero.
	 * If mux is locked, count is non-zero & represents the number of recursive locks on the mux.
	 */
	uint32_t count;
#ifdef CONFIG_FREERTOS_PORTMUX_DEBUG
	const char *lastLockedFn;
	int lastLockedLine;
#endif
} portMUX_TYPE;

#define portMUX_FREE_VAL		SPINLOCK_FREE

/* Special constants for vPortCPUAcquireMutexTimeout() */
#define portMUX_NO_TIMEOUT      SPINLOCK_WAIT_FOREVER  /* When passed for 'timeout_cycles', spin forever if necessary */
#define portMUX_TRY_LOCK        SPINLOCK_NO_WAIT     /* Try to acquire the spinlock a single time only */

// Keep this in sync with the portMUX_TYPE struct definition please.
#ifndef CONFIG_FREERTOS_PORTMUX_DEBUG
#define portMUX_INITIALIZER_UNLOCKED {					\
		.owner = portMUX_FREE_VAL,						\
		.count = 0,										\
	}
#else
#define portMUX_INITIALIZER_UNLOCKED {					\
		.owner = portMUX_FREE_VAL,						\
		.count = 0,										\
		.lastLockedFn = "(never locked)",				\
		.lastLockedLine = -1							\
	}
#endif

/* Scheduler utilities. */
extern void vPortYield( void );
extern void vPortYieldFromISR( void );

#define portYIELD()			    vPortYield()
#define portYIELD_FROM_ISR()    vPortYieldFromISR()

/* Yielding within an API call (when interrupts are off), means the yield should be delayed
   until interrupts are re-enabled.
   To do this, we use the "cross-core" interrupt as a trigger to yield on this core when interrupts are re-enabled.This
   is the same interrupt & code path which is used to trigger a yield between CPUs, although in this case the yield is
   happening on the same CPU.
*/
#define portYIELD_WITHIN_API() portYIELD()
/*-----------------------------------------------------------*/

/* Critical section management. */
extern int vPortSetInterruptMask(void);
extern void vPortClearInterruptMask( int );

void vPortCPUInitializeMutex(portMUX_TYPE *mux);
void vPortCPUAcquireMutex(portMUX_TYPE *mux);
bool vPortCPUAcquireMutexTimeout(portMUX_TYPE *mux, int timeout_cycles);
void vPortCPUReleaseMutex(portMUX_TYPE *mux);

extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portDISABLE_INTERRUPTS()				portSET_INTERRUPT_MASK_FROM_ISR()
#define portENABLE_INTERRUPTS()					portCLEAR_INTERRUPT_MASK_FROM_ISR(1)

#define portENTER_CRITICAL(mux)		{(void)mux;  vPortEnterCritical();}
#define portEXIT_CRITICAL(mux)		{(void)mux;  vPortExitCritical();}

#define portENTER_CRITICAL_ISR(mux)    portENTER_CRITICAL(mux)
#define portEXIT_CRITICAL_ISR(mux)     portEXIT_CRITICAL(mux)

#define portENTER_CRITICAL_SAFE(mux)  do {                                             \
                                         if (xPortInIsrContext()) {                    \
                                             portENTER_CRITICAL_ISR(mux);              \
                                         } else {                                      \
                                             portENTER_CRITICAL(mux);                  \
                                         }                                             \
                                      } while(0)

#define portEXIT_CRITICAL_SAFE(mux)  do {                                              \
                                         if (xPortInIsrContext()) {                    \
                                             portEXIT_CRITICAL_ISR(mux);               \
                                         } else {                                      \
                                             portEXIT_CRITICAL(mux);                   \
                                         }                                             \
                                      } while(0)

/*------------------------------------------------------------*/
#define portSET_INTERRUPT_MASK_FROM_ISR()       vPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedStatusValue )       vPortClearInterruptMask( uxSavedStatusValue )
#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired ) vPortYield()

// Cleaner solution allows nested interrupts disabling and restoring via local registers or stack.
// They can be called from interrupts too.
static inline unsigned portENTER_CRITICAL_NESTED(void) {
    unsigned state = portSET_INTERRUPT_MASK_FROM_ISR();
	return state;
}

#define portEXIT_CRITICAL_NESTED(state)   do { portCLEAR_INTERRUPT_MASK_FROM_ISR( state );} while(0);
/*-----------------------------------------------------------*/

//Because the ROM routines don't necessarily handle a stack in external RAM correctly, we force
//the stack memory to always be internal.
#define portTcbMemoryCaps (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)
#define portStackMemoryCaps (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)

#define pvPortMallocTcbMem(size) pvPortMalloc(size)
#define pvPortMallocStackMem(size)  pvPortMalloc(size)

/* Fine resolution time */
#define portGET_RUN_TIME_COUNTER_VALUE() 0
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()

#ifdef CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
/* Coarse resolution time (us) */
#define portALT_GET_RUN_TIME_COUNTER_VALUE(x)    do {x = (uint32_t)esp_timer_get_time();} while(0)
#endif

extern void esp_vApplicationIdleHook( void );
extern void esp_vApplicationTickHook( void );

#ifndef CONFIG_FREERTOS_LEGACY_HOOKS
#define vApplicationIdleHook    esp_vApplicationIdleHook
#define vApplicationTickHook    esp_vApplicationTickHook
#endif /* !CONFIG_FREERTOS_LEGACY_HOOKS */

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

void vApplicationSleep( TickType_t xExpectedIdleTime );
#define portSUPPRESS_TICKS_AND_SLEEP( idleTime ) vApplicationSleep( idleTime )

#define portNOP() __asm volatile 	( " nop " )

#define portVALID_TCB_MEM(ptr) esp_ptr_byte_accessible(ptr)
#define portVALID_STACK_MEM(ptr) esp_ptr_byte_accessible(ptr)

/* Get tick rate per second */
uint32_t xPortGetTickRateHz(void);

// configASSERT_2 if requested
#if configASSERT_2
#include <stdio.h>
void exit(int);
#define configASSERT( x )   if (!(x)) { porttracePrint(-1); printf("\nAssertion failed in %s:%d\n", __FILE__, __LINE__); exit(-1); }
#endif


#endif //__ASSEMBLER__

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
