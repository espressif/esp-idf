/*
 * FreeRTOS Kernel V10.4.3
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

#ifndef __ASSEMBLER__

#include "sdkconfig.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <xtensa/config/core.h>
#include <xtensa/hal.h>             /* required for xthal_get_ccount. [refactor-todo] use cpu_hal instead */
#include <xtensa/xtruntime.h>       /* required for XTOS_SET_INTLEVEL. [refactor-todo] add common intr functions to esp_hw_support */
#include "xt_instr_macros.h"
#include "soc/spinlock.h"
#include "hal/cpu_hal.h"
#include "esp_private/crosscore_int.h"
#include "esp_attr.h"
#include "esp_timer.h"              /* required for esp_timer_get_time. [refactor-todo] make this common between archs */
#include "esp_newlib.h"             /* required for esp_reent_init() in tasks.c */
#include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#include "esp_system.h"             /* required by esp_get_...() functions in portable.h. [refactor-todo] Update portable.h */
#include "portbenchmark.h"

/* [refactor-todo] These includes are not directly used in this file. They are kept into to prevent a breaking change. Remove these. */
#include <limits.h>
#include <xtensa/config/system.h>
#include <xtensa/xtensa_api.h>
#include "soc/cpu.h"
#ifdef CONFIG_LEGACY_INCLUDE_COMMON_HEADERS
#include "soc/soc_memory_layout.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif



/* --------------------------------------------------- Port Types ------------------------------------------------------
 * - Port specific types.
 * - The settings in this file configure FreeRTOS correctly for the given hardware and compiler.
 * - These settings should not be altered.
 * - The port types must come before first as they are used further down the file
 * ------------------------------------------------------------------------------------------------------------------ */

#define portCHAR                    int8_t
#define portFLOAT                   float
#define portDOUBLE                  double
#define portLONG                    int32_t
#define portSHORT                   int16_t
#define portSTACK_TYPE              uint8_t
#define portBASE_TYPE               int

typedef portSTACK_TYPE              StackType_t;
typedef portBASE_TYPE               BaseType_t;
typedef unsigned portBASE_TYPE      UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )  void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )        void vFunction( void *pvParameters )



/* ----------------------------------------------- Port Configurations -------------------------------------------------
 * - Configurations values supplied by each port
 * - Required by FreeRTOS
 * ------------------------------------------------------------------------------------------------------------------ */

#define portCRITICAL_NESTING_IN_TCB     0
#define portSTACK_GROWTH                ( -1 )
#define portTICK_PERIOD_MS              ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT              4
#define portNOP()                       XT_NOP()



/* ---------------------------------------------- Forward Declarations -------------------------------------------------
 * - Forward declarations of all the port functions and macros need to implement the FreeRTOS porting interface
 * - These must come before definition/declaration of the FreeRTOS porting interface
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

/**
 * @brief Checks if the current core is in an ISR context
 *
 * - ISR context consist of Low/Mid priority ISR, or time tick ISR
 * - High priority ISRs aren't detected here, but they normally cannot call C code, so that should not be an issue anyway.
 *
 * @note [refactor-todo] Check if this should be inlined
 * @return
 *  - pdTRUE if in ISR
 *  - pdFALSE otherwise
 */
BaseType_t xPortInIsrContext(void);

/**
 * @brief Asserts if in ISR context
 *
 * - Asserts on xPortInIsrContext() internally
 *
 * @note [refactor-todo] Check if this API is still required
 * @note [refactor-todo] Check if this should be inlined
 */
void vPortAssertIfInISR(void);

/**
 * @brief Check if in ISR context from High priority ISRs
 *
 * - Called from High priority ISR
 * - Checks if the previous context (before high priority interrupt) was in ISR context (meaning low/med priority)
 *
 * @note [refactor-todo] Check if this should be inlined
 * @return
 *  - pdTRUE if in previous in ISR context
 *  - pdFALSE otherwise
 */
BaseType_t xPortInterruptedFromISRContext(void);

/**
 * @brief Disable interrupts in a nested manner (meant to be called from ISRs)
 *
 * @warning Only applies to current CPU.
 * @return UBaseType_t Previous interrupt level
 */
static inline UBaseType_t xPortSetInterruptMaskFromISR(void);

/**
 * @brief Reenable interrupts in a nested manner (meant to be called from ISRs)
 *
 * @warning Only applies to current CPU.
 * @param prev_level Previous interrupt level
 */
static inline void vPortClearInterruptMaskFromISR(UBaseType_t prev_level);

/* ---------------------- Spinlocks ------------------------
 * - Modifications made to critical sections to support SMP
 * - See "Critical Sections & Disabling Interrupts" in docs/api-guides/freertos-smp.rst for more details
 * - Remark: For the ESP32, portENTER_CRITICAL and portENTER_CRITICAL_ISR both alias vPortEnterCritical, meaning that
 *           either function can be called both from ISR as well as task context. This is not standard FreeRTOS
 *           behaviorr; please keep this in mind if you need any compatibility with other FreeRTOS implementations.
 * @note [refactor-todo] Check if these comments are still true
 * ------------------------------------------------------ */

typedef spinlock_t                          portMUX_TYPE;               /**< Spinlock type used by FreeRTOS critical sections */
#define portMUX_INITIALIZER_UNLOCKED        SPINLOCK_INITIALIZER        /**< Spinlock initializer */
#define portMUX_FREE_VAL                    SPINLOCK_FREE               /**< Spinlock is free. [refactor-todo] check if this is still required */
#define portMUX_NO_TIMEOUT                  SPINLOCK_WAIT_FOREVER       /**< When passed for 'timeout_cycles', spin forever if necessary. [refactor-todo] check if this is still required */
#define portMUX_TRY_LOCK                    SPINLOCK_NO_WAIT            /**< Try to acquire the spinlock a single time only. [refactor-todo] check if this is still required */
#define portMUX_INITIALIZE(mux)             spinlock_initialize(mux)    /*< Initialize a spinlock to its unlocked state */

// ------------------ Critical Sections --------------------

/**
 * @brief Enter a SMP critical section with a timeout
 *
 * This function enters an SMP critical section by disabling interrupts then
 * taking a spinlock with a specified timeout.
 *
 * This function can be called in a nested manner.
 *
 * @note This function is made non-inline on purpose to reduce code size
 * @param mux Spinlock
 * @param timeout Timeout to wait for spinlock in number of CPU cycles.
 *                Use portMUX_NO_TIMEOUT to wait indefinitely
 *                Use portMUX_TRY_LOCK to only getting the spinlock a single time
 * @retval pdPASS Critical section entered (spinlock taken)
 * @retval pdFAIL If timed out waiting for spinlock (will not occur if using portMUX_NO_TIMEOUT)
 */
BaseType_t xPortEnterCriticalTimeout(portMUX_TYPE *mux, BaseType_t timeout);

/**
 * @brief Enter a SMP critical section
 *
 * This function enters an SMP critical section by disabling interrupts then
 * taking a spinlock with an unlimited timeout.
 *
 * This function can be called in a nested manner
 *
 * @param[in] mux Spinlock
 */
static inline void __attribute__((always_inline)) vPortEnterCritical(portMUX_TYPE *mux);

/**
 * @brief Exit a SMP critical section
 *
 * This function can be called in a nested manner. On the outer most level of nesting, this function will:
 *
 * - Release the spinlock
 * - Restore the previous interrupt level before the critical section was entered
 *
 * If still nesting, this function simply decrements a critical nesting count
 *
 * @note This function is made non-inline on purpose to reduce code size
 * @param[in] mux Spinlock
 */
void vPortExitCritical(portMUX_TYPE *mux);

/**
 * @brief FreeRTOS Compliant version of xPortEnterCriticalTimeout()
 *
 * Compliant version of xPortEnterCriticalTimeout() will ensure that this is
 * called from a task context only. An abort is called otherwise.
 *
 * @note This function is made non-inline on purpose to reduce code size
 *
 * @param mux Spinlock
 * @param timeout Timeout
 * @return BaseType_t
 */
BaseType_t xPortEnterCriticalTimeoutCompliance(portMUX_TYPE *mux, BaseType_t timeout);

/**
 * @brief FreeRTOS compliant version of vPortEnterCritical()
 *
 * Compliant version of vPortEnterCritical() will ensure that this is
 * called from a task context only. An abort is called otherwise.
 *
 * @param[in] mux Spinlock
 */
static inline void __attribute__((always_inline)) vPortEnterCriticalCompliance(portMUX_TYPE *mux);

/**
 * @brief FreeRTOS compliant version of vPortExitCritical()
 *
 * Compliant version of vPortExitCritical() will ensure that this is
 * called from a task context only. An abort is called otherwise.
 *
 * @note This function is made non-inline on purpose to reduce code size
 * @param[in] mux Spinlock
 */
void vPortExitCriticalCompliance(portMUX_TYPE *mux);

/**
 * @brief Safe version of enter critical timeout
 *
 * Safe version of enter critical will automatically select between
 * portTRY_ENTER_CRITICAL() and portTRY_ENTER_CRITICAL_ISR()
 *
 * @param mux Spinlock
 * @param timeout Timeout
 * @return BaseType_t
 */
static inline BaseType_t __attribute__((always_inline)) xPortEnterCriticalTimeoutSafe(portMUX_TYPE *mux, BaseType_t timeout);

/**
 * @brief Safe version of enter critical
 *
 * Safe version of enter critical will automatically select between
 * portENTER_CRITICAL() and portENTER_CRITICAL_ISR()
 *
 * @param[in] mux Spinlock
 */
static inline void __attribute__((always_inline)) vPortEnterCriticalSafe(portMUX_TYPE *mux);

/**
 * @brief Safe version of exit critical
 *
 * Safe version of enter critical will automatically select between
 * portEXIT_CRITICAL() and portEXIT_CRITICAL_ISR()
 *
 * @param[in] mux Spinlock
 */
static inline void __attribute__((always_inline)) vPortExitCriticalSafe(portMUX_TYPE *mux);

// ---------------------- Yielding -------------------------

/**
 * @brief Perform a solicited context switch
 *
 * - Defined in portasm.S
 *
 * @note [refactor-todo] The rest of ESP-IDF should call taskYield() instead
 */
void vPortYield( void );

/**
 * @brief
 *
 * @note [refactor-todo] Refactor this to avoid va_args
 * @param argc
 * @param ... Variable arguments to allow for IDF prototype without arguments, and vanilla version WITH argument
 */
void vPortEvaluateYieldFromISR(int argc, ...);

/**
 * @brief Yields the other core
 *
 * - Send an interrupt to another core in order to make the task running on it yield for a higher-priority task.
 * - Can be used to yield current core as well
 *
 * @note [refactor-todo] Put this into private macros as its only called from task.c and is not public API
 * @param coreid ID of core to yield
 */
void vPortYieldOtherCore(BaseType_t coreid);

/**
 * @brief Checks if the current core can yield
 *
 * - A core cannot yield if its in an ISR or in a critical section
 *
 * @note [refactor-todo] See if this can be separated from port macro
 * @return true Core can yield
 * @return false Core cannot yield
 */
static inline bool IRAM_ATTR xPortCanYield(void);

// ------------------- Hook Functions ----------------------

extern void esp_vApplicationIdleHook(void);     /* Required by tasks.c */
extern void esp_vApplicationTickHook(void);     /* Required by tasks.c */

/**
 * @brief Hook function called on entry to tickless idle
 *
 * - Implemented in pm_impl.c
 *
 * @param xExpectedIdleTime Expected idle time
 */
void vApplicationSleep(TickType_t xExpectedIdleTime);

// ----------------------- System --------------------------

/**
 * @brief Get the tick rate per second
 *
 * @note [refactor-todo] make this inline
 * @return uint32_t Tick rate in Hz
 */
uint32_t xPortGetTickRateHz(void);

/**
 * @brief Set a watchpoint to watch the last 32 bytes of the stack
 *
 * Callback to set a watchpoint on the end of the stack. Called every context switch to change the stack watchpoint
 * around.
 *
 * @param pxStackStart Pointer to the start of the stack
 */
void vPortSetStackWatchpoint( void *pxStackStart );

/**
 * @brief Get the current core's ID
 *
 * @note [refactor-todo] IDF should call a FreeRTOS like macro instead of port function directly
 * @return BaseType_t Core ID
 */
static inline BaseType_t IRAM_ATTR xPortGetCoreID(void);

/**
 * @brief Wrapper for atomic compare-and-set instruction
 *
 * This subroutine will atomically compare *addr to 'compare'. If *addr == compare, *addr is set to *set. *set is
 * updated with the previous value of *addr (either 'compare' or some other value.)
 *
 * @warning From the ISA docs: in some (unspecified) cases, the s32c1i instruction may return the "bitwise inverse" of
 *          the old mem if the mem wasn't written. This doesn't seem to happen on the ESP32 (portMUX assertions would
 *          fail).
 *
 * @note [refactor-todo] Check if this can be deprecated
 * @note [refactor-todo] Check if this function should be renamed (due to void return type)
 *
 * @param[inout] addr Pointer to target address
 * @param[in] compare Compare value
 * @param[inout] set Pointer to set value
 */
static inline void __attribute__((always_inline)) uxPortCompareSet(volatile uint32_t *addr, uint32_t compare, uint32_t *set);

/**
 * @brief Wrapper for atomic compare-and-set instruction in external RAM
 *
 * Atomic compare-and-set but the target address is placed in external RAM
 *
 * @note [refactor-todo] Check if this can be deprecated
 *
 * @param[inout] addr Pointer to target address
 * @param[in] compare Compare value
 * @param[inout] set Pointer to set value
 */
static inline void __attribute__((always_inline)) uxPortCompareSetExtram(volatile uint32_t *addr, uint32_t compare, uint32_t *set);



/* ------------------------------------------- FreeRTOS Porting Interface ----------------------------------------------
 * - Contains all the mappings of the macros required by FreeRTOS
 * - Most come after forward declare as porting macros map to declared functions
 * - Maps to forward declared functions
 * ------------------------------------------------------------------------------------------------------------------ */

// ----------------------- Memory --------------------------

/**
 * @brief Task memory allocation macros
 *
 * @note Because the ROM routines don't necessarily handle a stack in external RAM correctly, we force the stack
 * memory to always be internal.
 * @note [refactor-todo] Update portable.h to match v10.4.3 to use new malloc prototypes
 */
#define portTcbMemoryCaps               (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)
#define portStackMemoryCaps             (MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)
#define pvPortMallocTcbMem(size)        heap_caps_malloc(size, portTcbMemoryCaps)
#define pvPortMallocStackMem(size)      heap_caps_malloc(size, portStackMemoryCaps)

// --------------------- Interrupts ------------------------

/**
 * - Only applies to current core
 * - These cannot be nested. They should be used with a lot of care and cannot be called from interrupt level.
 *
 * @note [refactor-todo] replace XTOS_SET_INTLEVEL with more efficient version, if any?
 */
#define portDISABLE_INTERRUPTS()            do { XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); portbenchmarkINTERRUPT_DISABLE(); } while (0)
#define portENABLE_INTERRUPTS()             do { portbenchmarkINTERRUPT_RESTORE(0); XTOS_SET_INTLEVEL(0); } while (0)

/**
 * ISR versions to enable/disable interrupts
 */
#define portSET_INTERRUPT_MASK_FROM_ISR()                   xPortSetInterruptMaskFromISR()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(prev_level)       vPortClearInterruptMaskFromISR(prev_level)

#define portASSERT_IF_IN_ISR() vPortAssertIfInISR()

// ------------------ Critical Sections --------------------

/**
 * @brief FreeRTOS critical section macros
 *
 * - Added a spinlock argument for SMP
 * - Can be nested
 * - Compliance versions will assert if regular critical section API is used in ISR context
 * - Safe versions can be called from either contexts
 */
#ifdef CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE
#define portTRY_ENTER_CRITICAL(mux, timeout)        xPortEnterCriticalTimeoutCompliance(mux, timeout)
#define portENTER_CRITICAL(mux)                     vPortEnterCriticalCompliance(mux)
#define portEXIT_CRITICAL(mux)                      vPortExitCriticalCompliance(mux)
#else
#define portTRY_ENTER_CRITICAL(mux, timeout)        xPortEnterCriticalTimeout(mux, timeout)
#define portENTER_CRITICAL(mux)                     vPortEnterCritical(mux)
#define portEXIT_CRITICAL(mux)                      vPortExitCritical(mux)
#endif /* CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE */

#define portTRY_ENTER_CRITICAL_ISR(mux, timeout)    xPortEnterCriticalTimeout(mux, timeout)
#define portENTER_CRITICAL_ISR(mux)                 vPortEnterCritical(mux)
#define portEXIT_CRITICAL_ISR(mux)                  vPortExitCritical(mux)

#define portTRY_ENTER_CRITICAL_SAFE(mux, timeout)   xPortEnterCriticalTimeoutSafe(mux)
#define portENTER_CRITICAL_SAFE(mux)                vPortEnterCriticalSafe(mux)
#define portEXIT_CRITICAL_SAFE(mux)                 vPortExitCriticalSafe(mux)

// ---------------------- Yielding -------------------------

#define portYIELD() vPortYield()

/**
 * @note    The macro below could be used when passing a single argument, or without any argument,
 *          it was developed to support both usages of portYIELD inside of an ISR. Any other usage form
 *          might result in undesired behavior
 *
 * @note [refactor-todo] Refactor this to avoid va_args
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

// ------------------- Hook Functions ----------------------

#ifndef CONFIG_FREERTOS_LEGACY_HOOKS
#define vApplicationIdleHook esp_vApplicationIdleHook
#define vApplicationTickHook esp_vApplicationTickHook
#endif /* !CONFIG_FREERTOS_LEGACY_HOOKS */

#define portSUPPRESS_TICKS_AND_SLEEP(idleTime) vApplicationSleep(idleTime)

// ------------------- Run Time Stats ----------------------

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()

/**
 * - Fine resolution uses ccount
 * - ALT is coarse and uses esp_timer
 * @note [refactor-todo] Make fine and alt timers mutually exclusive
 */
#define portGET_RUN_TIME_COUNTER_VALUE() xthal_get_ccount()
#ifdef CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
#define portALT_GET_RUN_TIME_COUNTER_VALUE(x) do {x = (uint32_t)esp_timer_get_time();} while(0)
#endif

// -------------- Optimized Task Selection -----------------

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
/* Check the configuration. */
#if( configMAX_PRIORITIES > 32 )
#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 different priorities as tasks that share a priority will time slice.
#endif

/* Store/clear the ready priorities in a bit map. */
#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )
#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31 - __builtin_clz( ( uxReadyPriorities ) ) )
#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */



/* --------------------------------------------- Inline Implementations ------------------------------------------------
 * - Implementation of inline functions of the forward declares
 * - Should come after forward declare and FreeRTOS Porting interface, as implementation may use both.
 * - For implementation of non-inlined functions, see port.c
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

static inline UBaseType_t xPortSetInterruptMaskFromISR(void)
{
    UBaseType_t prev_int_level = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL);
    portbenchmarkINTERRUPT_DISABLE();
    return prev_int_level;
}

static inline void vPortClearInterruptMaskFromISR(UBaseType_t prev_level)
{
    portbenchmarkINTERRUPT_RESTORE(prev_level);
    XTOS_RESTORE_JUST_INTLEVEL(prev_level);
}

// ------------------ Critical Sections --------------------

static inline void __attribute__((always_inline)) vPortEnterCritical(portMUX_TYPE *mux)
{
    xPortEnterCriticalTimeout(mux, portMUX_NO_TIMEOUT);
}

static inline void __attribute__((always_inline)) vPortEnterCriticalCompliance(portMUX_TYPE *mux)
{
    xPortEnterCriticalTimeoutCompliance(mux, portMUX_NO_TIMEOUT);
}

static inline BaseType_t __attribute__((always_inline)) xPortEnterCriticalTimeoutSafe(portMUX_TYPE *mux, BaseType_t timeout)
{
    BaseType_t ret;
    if (xPortInIsrContext()) {
        ret = portTRY_ENTER_CRITICAL_ISR(mux, timeout);
    } else {
        ret = portTRY_ENTER_CRITICAL(mux, timeout);
    }
    return ret;
}

static inline void __attribute__((always_inline)) vPortEnterCriticalSafe(portMUX_TYPE *mux)
{
    xPortEnterCriticalTimeoutSafe(mux, portMUX_NO_TIMEOUT);
}

static inline void __attribute__((always_inline)) vPortExitCriticalSafe(portMUX_TYPE *mux)
{
    if (xPortInIsrContext()) {
        portEXIT_CRITICAL_ISR(mux);
    } else {
        portEXIT_CRITICAL(mux);
    }
}

// ---------------------- Yielding -------------------------

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

// ----------------------- System --------------------------

static inline BaseType_t IRAM_ATTR xPortGetCoreID(void)
{
    return (uint32_t) cpu_hal_get_core_id();
}

static inline void __attribute__((always_inline)) uxPortCompareSet(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
    compare_and_set_native(addr, compare, set);
}

static inline void __attribute__((always_inline)) uxPortCompareSetExtram(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
#ifdef CONFIG_SPIRAM
    compare_and_set_extram(addr, compare, set);
#endif
}



/* ------------------------------------------------------ Misc ---------------------------------------------------------
 * - Miscellaneous porting macros
 * - These are not port of the FreeRTOS porting interface, but are used by other FreeRTOS dependent components
 * - [refactor-todo] Remove dependency on MPU wrappers by modifying TCB
 * ------------------------------------------------------------------------------------------------------------------ */

// -------------------- Co-Processor -----------------------

// When coprocessors are defined, we maintain a pointer to coprocessors area.
// We currently use a hack: redefine field xMPU_SETTINGS in TCB block as a structure that can hold:
// MPU wrappers, coprocessor area pointer, trace code structure, and more if needed.
// The field is normally used for memory protection. FreeRTOS should create another general purpose field.
typedef struct {
#if XCHAL_CP_NUM > 0
    volatile StackType_t *coproc_area; // Pointer to coprocessor save area; MUST BE FIRST
#endif

#if portUSING_MPU_WRAPPERS
    // Define here mpu_settings, which is port dependent
    int mpu_setting; // Just a dummy example here; MPU not ported to Xtensa yet
#endif
} xMPU_SETTINGS;

// Main hack to use MPU_wrappers even when no MPU is defined (warning: mpu_setting should not be accessed; otherwise move this above xMPU_SETTINGS)
#if (XCHAL_CP_NUM > 0) && !portUSING_MPU_WRAPPERS   // If MPU wrappers not used, we still need to allocate coproc area
#undef portUSING_MPU_WRAPPERS
#define portUSING_MPU_WRAPPERS 1   // Enable it to allocate coproc area
#define MPU_WRAPPERS_H             // Override mpu_wrapper.h to disable unwanted code
#define PRIVILEGED_FUNCTION
#define PRIVILEGED_DATA
#endif

void _xt_coproc_release(volatile void *coproc_sa_base);

/*
 * The structures and methods of manipulating the MPU are contained within the
 * port layer.
 *
 * Fills the xMPUSettings structure with the memory region information
 * contained in xRegions.
 */
#if( portUSING_MPU_WRAPPERS == 1 )
struct xMEMORY_REGION;
void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION *const xRegions, StackType_t *pxBottomOfStack, uint32_t usStackDepth ) PRIVILEGED_FUNCTION;
void vPortReleaseTaskMPUSettings( xMPU_SETTINGS *xMPUSettings );
#endif

// -------------------- VA_ARGS Yield ----------------------

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

// -------------------- Heap Related -----------------------

/**
 * @brief Checks if a given piece of memory can be used to store a task's TCB
 *
 * - Defined in port_common.c
 *
 * @param ptr Pointer to memory
 * @return true Memory can be used to store a TCB
 * @return false Otherwise
 */
bool xPortCheckValidTCBMem(const void *ptr);

/**
 * @brief Checks if a given piece of memory can be used to store a task's stack
 *
 * - Defined in port_common.c
 *
 * @param ptr Pointer to memory
 * @return true Memory can be used to store a task stack
 * @return false Otherwise
 */
bool xPortcheckValidStackMem(const void *ptr);

#define portVALID_TCB_MEM(ptr) xPortCheckValidTCBMem(ptr)
#define portVALID_STACK_MEM(ptr) xPortcheckValidStackMem(ptr)



/* ---------------------------------------------------- Deprecate ------------------------------------------------------
 * - Pull in header containing deprecated macros here
 * ------------------------------------------------------------------------------------------------------------------ */

#include "portmacro_deprecated.h"

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__

#endif /* PORTMACRO_H */
