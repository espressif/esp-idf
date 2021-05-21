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

#include <sdkconfig.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>	/* required for XSHAL_CLIB */
#include <xtensa/xtruntime.h>
#include "esp_private/crosscore_int.h"
#include "esp_timer.h"              /* required for FreeRTOS run time stats */
#include "esp_system.h"
#include "esp_newlib.h"
#include "soc/spinlock.h"
#include <esp_heap_caps.h>
#include "esp_rom_sys.h"
#include "sdkconfig.h"
#include "freertos/xtensa_api.h"
#include "esp_system.h"
#include "soc/cpu.h"
#include <limits.h>

#ifdef CONFIG_LEGACY_INCLUDE_COMMON_HEADERS
#include "soc/soc_memory_layout.h"
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

#include "esp_system.h"
#include "hal/cpu_hal.h"
#include "xt_instr_macros.h"

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
#include "esp_attr.h"
#include "portmacro_priv.h"

// Cleaner solution allows nested interrupts disabling and restoring via local registers or stack.
// They can be called from interrupts too.
// WARNING: Only applies to current CPU. See notes above.
static inline unsigned portENTER_CRITICAL_NESTED(void) {
	unsigned state = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);
	portbenchmarkINTERRUPT_DISABLE();
	return state;
}
#define portEXIT_CRITICAL_NESTED(state)   do { portbenchmarkINTERRUPT_RESTORE(state); XTOS_RESTORE_JUST_INTLEVEL(state); } while (0)

/*
Modifications to portENTER_CRITICAL.

For an introduction, see "Critical Sections & Disabling Interrupts" in docs/api-guides/freertos-smp.rst

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

This all assumes that interrupts are either entirely disabled or enabled. Interrupt priority levels
will break this scheme.

Remark: For the ESP32, portENTER_CRITICAL and portENTER_CRITICAL_ISR both alias vPortEnterCritical, meaning
that either function can be called both from ISR as well as task context. This is not standard FreeRTOS
behaviour; please keep this in mind if you need any compatibility with other FreeRTOS implementations.
*/
/* "mux" data structure (spinlock) */
typedef spinlock_t portMUX_TYPE;

#define portMUX_FREE_VAL		SPINLOCK_FREE
#define portMUX_NO_TIMEOUT      SPINLOCK_WAIT_FOREVER  /* When passed for 'timeout_cycles', spin forever if necessary */
#define portMUX_TRY_LOCK        SPINLOCK_NO_WAIT       /* Try to acquire the spinlock a single time only */
#define portMUX_INITIALIZER_UNLOCKED  SPINLOCK_INITIALIZER

#define portCRITICAL_NESTING_IN_TCB 0

static inline void __attribute__((always_inline)) vPortCPUInitializeMutex(portMUX_TYPE *mux)
{
    spinlock_initialize(mux);
}

static inline void __attribute__((always_inline)) vPortCPUAcquireMutex(portMUX_TYPE *mux)
{
    spinlock_acquire(mux, portMUX_NO_TIMEOUT);
}

static inline bool __attribute__((always_inline)) vPortCPUAcquireMutexTimeout(portMUX_TYPE *mux, int timeout)
{
    return (spinlock_acquire(mux, timeout));
}

static inline void __attribute__((always_inline)) vPortCPUReleaseMutex(portMUX_TYPE *mux)
{
    spinlock_release(mux);
}

void vPortEnterCritical(portMUX_TYPE *mux);
void vPortExitCritical(portMUX_TYPE *mux);

#define portASSERT_IF_IN_ISR()  vPortAssertIfInISR()
void vPortAssertIfInISR(void);

/*
 * Returns true if the current core is in ISR context; low prio ISR, med prio ISR or timer tick ISR. High prio ISRs
 * aren't detected here, but they normally cannot call C code, so that should not be an issue anyway.
 */
BaseType_t xPortInIsrContext(void);

static inline void __attribute__((always_inline)) vPortEnterCriticalCompliance(portMUX_TYPE *mux)
{
    if(!xPortInIsrContext()) {
        vPortEnterCritical(mux);
    } else {
        esp_rom_printf("%s:%d (%s)- port*_CRITICAL called from ISR context!\n",
                       __FILE__, __LINE__, __FUNCTION__);
        abort();
    }
}

static inline void __attribute__((always_inline)) vPortExitCriticalCompliance(portMUX_TYPE *mux)
{
    if(!xPortInIsrContext()) {
        vPortExitCritical(mux);
    } else {
        esp_rom_printf("%s:%d (%s)- port*_CRITICAL called from ISR context!\n",
                       __FILE__, __LINE__, __FUNCTION__);
        abort();
    }
}

#ifdef CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE
/* Calling port*_CRITICAL from ISR context would cause an assert failure.
 * If the parent function is called from both ISR and Non-ISR context then call port*_CRITICAL_SAFE
 */
#define portENTER_CRITICAL(mux)        vPortEnterCriticalCompliance(mux)
#define portEXIT_CRITICAL(mux)         vPortExitCriticalCompliance(mux)
#else
#define portENTER_CRITICAL(mux)        vPortEnterCritical(mux)
#define portEXIT_CRITICAL(mux)         vPortExitCritical(mux)
#endif

#define portENTER_CRITICAL_ISR(mux)    vPortEnterCritical(mux)
#define portEXIT_CRITICAL_ISR(mux)     vPortExitCritical(mux)

static inline void __attribute__((always_inline)) vPortEnterCriticalSafe(portMUX_TYPE *mux)
{
    if (xPortInIsrContext()) {
        portENTER_CRITICAL_ISR(mux);
    } else {
        portENTER_CRITICAL(mux);
    }
}

static inline void __attribute__((always_inline)) vPortExitCriticalSafe(portMUX_TYPE *mux)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(mux);
    } else {
        portEXIT_CRITICAL(mux);
    }
}

#define portENTER_CRITICAL_SAFE(mux)  vPortEnterCriticalSafe(mux)
#define portEXIT_CRITICAL_SAFE(mux)  vPortExitCriticalSafe(mux)

/*
 * Wrapper for the Xtensa compare-and-set instruction. This subroutine will atomically compare
 * *addr to 'compare'. If *addr == compare, *addr is set to *set. *set is updated with the previous
 * value of *addr (either 'compare' or some other value.)
 *
 * Warning: From the ISA docs: in some (unspecified) cases, the s32c1i instruction may return the
 * *bitwise inverse* of the old mem if the mem wasn't written. This doesn't seem to happen on the
 * ESP32 (portMUX assertions would fail).
 */
static inline void __attribute__((always_inline)) uxPortCompareSet(volatile uint32_t *addr, uint32_t compare, uint32_t *set) {
    compare_and_set_native(addr, compare, set);
}

// Critical section management. NW-TODO: replace XTOS_SET_INTLEVEL with more efficient version, if any?
// These cannot be nested. They should be used with a lot of care and cannot be called from interrupt level.
//
// Only applies to one CPU. See notes above & below for reasons not to use these.
#define portDISABLE_INTERRUPTS()      do { XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); portbenchmarkINTERRUPT_DISABLE(); } while (0)
#define portENABLE_INTERRUPTS()       do { portbenchmarkINTERRUPT_RESTORE(0); XTOS_SET_INTLEVEL(0); } while (0)


// These FreeRTOS versions are similar to the nested versions above
#define portSET_INTERRUPT_MASK_FROM_ISR()            portENTER_CRITICAL_NESTED()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(state)     portEXIT_CRITICAL_NESTED(state)

//Because the ROM routines don't necessarily handle a stack in external RAM correctly, we force
//the stack memory to always be internal.
#define portTcbMemoryCaps (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)
#define portStackMemoryCaps (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)
#define pvPortMallocTcbMem(size) heap_caps_malloc(size, portTcbMemoryCaps)
#define pvPortMallocStackMem(size)  heap_caps_malloc(size, portStackMemoryCaps)

//xTaskCreateStatic uses these functions to check incoming memory.
#define portVALID_TCB_MEM(ptr) (esp_ptr_internal(ptr) && esp_ptr_byte_accessible(ptr))
#ifdef CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
#define portVALID_STACK_MEM(ptr) esp_ptr_byte_accessible(ptr)
#else
#define portVALID_STACK_MEM(ptr) (esp_ptr_internal(ptr) && esp_ptr_byte_accessible(ptr))
#endif


static inline void uxPortCompareSetExtram(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
#ifdef CONFIG_SPIRAM
    compare_and_set_extram(addr, compare, set);
#endif
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
//ccount or esp_timer are initialized elsewhere
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()

#ifdef CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
/* Coarse resolution time (us) */
#define portALT_GET_RUN_TIME_COUNTER_VALUE(x)     do {x = (uint32_t)esp_timer_get_time();} while(0)
#endif

void vPortYield( void );
void vPortEvaluateYieldFromISR(int argc, ...);
void _frxt_setup_switch( void );

/**
 * Macro to count number of arguments of a __VA_ARGS__ used to support portYIELD_FROM_ISR with,
 * or without arguments. The macro counts only 0 or 1 arguments.
 *
 * In the future, we want to switch to C++20. We also want to become compatible with clang.
 * Hence, we provide two versions of the following macros which are using variadic arguments.
 * The first one is using the GNU extension ##__VA_ARGS__. The second one is using the C++20 feature __VA_OPT__(,).
 * This allows users to compile their code with standard C++20 enabled instead of the GNU extension.
 * Below C++20, we haven't found any good alternative to using ##__VA_ARGS__.
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define portGET_ARGUMENT_COUNT(...) portGET_ARGUMENT_COUNT_INNER(0 __VA_OPT__(,) __VA_ARGS__,1,0)
#else
#define portGET_ARGUMENT_COUNT(...) portGET_ARGUMENT_COUNT_INNER(0, ##__VA_ARGS__,1,0)
#endif
#define portGET_ARGUMENT_COUNT_INNER(zero, one, count, ...) count

_Static_assert(portGET_ARGUMENT_COUNT() == 0, "portGET_ARGUMENT_COUNT() result does not match for 0 arguments");
_Static_assert(portGET_ARGUMENT_COUNT(1) == 1, "portGET_ARGUMENT_COUNT() result does not match for 1 argument");

#define portYIELD()	vPortYield()

/**
 * @note    The macro below could be used when passing a single argument, or without any argument,
 *          it was developed to support both usages of portYIELD inside of an ISR. Any other usage form
 *          might result in undesired behaviour
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define portYIELD_FROM_ISR(...) vPortEvaluateYieldFromISR(portGET_ARGUMENT_COUNT(__VA_ARGS__) __VA_OPT__(,) __VA_ARGS__)
#else
#define portYIELD_FROM_ISR(...) vPortEvaluateYieldFromISR(portGET_ARGUMENT_COUNT(__VA_ARGS__), ##__VA_ARGS__)
#endif

/* Yielding within an API call (when interrupts are off), means the yield should be delayed
   until interrupts are re-enabled.

   To do this, we use the "cross-core" interrupt as a trigger to yield on this core when interrupts are re-enabled.This
   is the same interrupt & code path which is used to trigger a yield between CPUs, although in this case the yield is
   happening on the same CPU.
*/
#define portYIELD_WITHIN_API() esp_crosscore_int_send_yield(xPortGetCoreID())

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

extern void esp_vApplicationIdleHook( void );
extern void esp_vApplicationTickHook( void );

#ifndef CONFIG_FREERTOS_LEGACY_HOOKS
#define vApplicationIdleHook    esp_vApplicationIdleHook
#define vApplicationTickHook    esp_vApplicationTickHook
#endif /* !CONFIG_FREERTOS_LEGACY_HOOKS */

void vApplicationSleep( TickType_t xExpectedIdleTime );

#define portSUPPRESS_TICKS_AND_SLEEP( idleTime ) vApplicationSleep( idleTime )

void _xt_coproc_release(volatile void * coproc_sa_base);

/* Architecture specific optimisations. */
#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

/* Check the configuration. */
#if( configMAX_PRIORITIES > 32 )
    #error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 different priorities as tasks that share a priority will time slice.
#endif

/* Store/clear the ready priorities in a bit map. */
#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

/*-----------------------------------------------------------*/

#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31 - __builtin_clz( ( uxReadyPriorities ) ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/*
 * Send an interrupt to another core in order to make the task running
 * on it yield for a higher-priority task.
 */

void vPortYieldOtherCore( BaseType_t coreid) ;

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

/*
 * The structures and methods of manipulating the MPU are contained within the
 * port layer.
 *
 * Fills the xMPUSettings structure with the memory region information
 * contained in xRegions.
 */
#if( portUSING_MPU_WRAPPERS == 1 )
    struct xMEMORY_REGION;
    void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions, StackType_t *pxBottomOfStack, uint32_t usStackDepth ) PRIVILEGED_FUNCTION;
    void vPortReleaseTaskMPUSettings( xMPU_SETTINGS *xMPUSettings );
#endif

/* Multi-core: get current core ID */
static inline BaseType_t IRAM_ATTR xPortGetCoreID(void) {
    return cpu_hal_get_core_id();
}

/* Get tick rate per second */
uint32_t xPortGetTickRateHz(void);

static inline bool IRAM_ATTR xPortCanYield(void)
{
    uint32_t ps_reg = 0;

    //Get the current value of PS (processor status) register
    RSR(PS, ps_reg);

    /*
     * intlevel = (ps_reg & 0xf);
     * excm  = (ps_reg >> 4) & 0x1;
     * CINTLEVEL is max(excm * EXCMLEVEL, INTLEVEL), where EXCMLEVEL is 3.
     * However, just return true, only intlevel is zero.
     */

    return ((ps_reg & PS_INTLEVEL_MASK) == 0);
}

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
