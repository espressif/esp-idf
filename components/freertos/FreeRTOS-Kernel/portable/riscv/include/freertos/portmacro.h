/*
 * SPDX-FileCopyrightText: 2020 Amazon.com, Inc. or its affiliates
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2016-2022 Espressif Systems (Shanghai) CO LTD
 */
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
#include <stdio.h>
#include "spinlock.h"
#include "soc/interrupt_reg.h"
#include "esp_macros.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"
#include "esp_heap_caps.h"
#include "esp_system.h"             /* required by esp_get_...() functions in portable.h. [refactor-todo] Update portable.h */
#include "esp_newlib.h"

/* [refactor-todo] These includes are not directly used in this file. They are kept into to prevent a breaking change. Remove these. */
#include <limits.h>

/* [refactor-todo] introduce a port wrapper function to avoid including esp_timer.h into the public header */
#if CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
#include "esp_timer.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* --------------------------------------------------- Port Types ------------------------------------------------------
 * - Port specific types.
 * - The settings in this file configure FreeRTOS correctly for the given hardware and compiler.
 * - These settings should not be altered.
 * - The port types must come first as they are used further down in this file
 * ------------------------------------------------------------------------------------------------------------------ */

#define portCHAR                    uint8_t
#define portFLOAT                   float
#define portDOUBLE                  double
#define portLONG                    int32_t
#define portSHORT                   int16_t
#define portSTACK_TYPE              uint8_t
#define portBASE_TYPE               int

typedef portSTACK_TYPE              StackType_t;
typedef portBASE_TYPE               BaseType_t;
typedef unsigned portBASE_TYPE      UBaseType_t;

#if (configUSE_16_BIT_TICKS == 1)
typedef uint16_t TickType_t;
#define portMAX_DELAY (TickType_t)  0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY (TickType_t)  0xffffffffUL
#endif

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters) void vFunction(void *pvParameters)

// interrupt module will mask interrupt with priority less than threshold
#define RVHAL_EXCM_LEVEL            4


/* ----------------------------------------------- Port Configurations -------------------------------------------------
 * - Configurations values supplied by each port
 * - Required by FreeRTOS
 * ------------------------------------------------------------------------------------------------------------------ */

#define portCRITICAL_NESTING_IN_TCB     0
#define portSTACK_GROWTH                (-1)
#define portTICK_PERIOD_MS              ((TickType_t) (1000 / configTICK_RATE_HZ))
#define portBYTE_ALIGNMENT              16
#define portTICK_TYPE_IS_ATOMIC         1
#define portNOP() __asm volatile        (" nop ")



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

/* ---------------------- Spinlocks ------------------------
 - Spinlocks added to match API with SMP FreeRTOS. Single core RISC-V does not need spin locks
 - Because single core does not have a primitive spinlock data type, we have to implement one here
 * @note [refactor-todo] Refactor critical section API so that this is no longer required
 * ------------------------------------------------------ */

/**
 * @brief Spinlock object
 * Owner:
 *  - Set to 0 if uninitialized
 *  - Set to portMUX_FREE_VAL when free
 *  - Set to CORE_ID_REGVAL_PRO or CORE_ID_REGVAL_AP when locked
 *  - Any other value indicates corruption
 * Count:
 *  - 0 if unlocked
 *  - Recursive count if locked
 *
 * @note Not a true spinlock as single core RISC-V does not have atomic compare and set instruction
 * @note Keep portMUX_INITIALIZER_UNLOCKED in sync with this struct
 */
typedef struct {
    uint32_t owner;
    uint32_t count;
} portMUX_TYPE;
/**< Spinlock initializer */
#define portMUX_INITIALIZER_UNLOCKED {                      \
            .owner = portMUX_FREE_VAL,                      \
            .count = 0,                                     \
        }
#define portMUX_FREE_VAL                    SPINLOCK_FREE           /**< Spinlock is free. [refactor-todo] check if this is still required */
#define portMUX_NO_TIMEOUT                  SPINLOCK_WAIT_FOREVER   /**< When passed for 'timeout_cycles', spin forever if necessary. [refactor-todo] check if this is still required */
#define portMUX_TRY_LOCK                    SPINLOCK_NO_WAIT        /**< Try to acquire the spinlock a single time only. [refactor-todo] check if this is still required */
#define portMUX_INITIALIZE(mux)    ({ \
    (mux)->owner = portMUX_FREE_VAL; \
    (mux)->count = 0; \
})

// ------------------ Critical Sections --------------------

/**
 * @brief Enter a critical section
 *
 * - Simply disable interrupts
 * - Can be nested
 */
void vPortEnterCritical(void);

/**
 * @brief Exit a critical section
 *
 * - Reenables interrupts
 * - Can be nested
 */
void vPortExitCritical(void);

// ---------------------- Yielding -------------------------

/**
 * @brief Set interrupt mask and return current interrupt enable register
 *
 * @note [refactor-todo] Check if this function should be renamed (due to int return type)
 * @return int Current interrupt enable register before set
 */
int vPortSetInterruptMask(void);

/**
 * @brief Clear current interrupt mask and set given mask
 *
 * @param mask Interrupt mask
 */
void vPortClearInterruptMask(int mask);

/**
 * @brief Perform a context switch from a task
 *
 * @note [refactor-todo] The rest of ESP-IDF should call taskYield() instead
 */
void vPortYield(void);

/**
 * @brief Perform a context switch from an ISR
 */
void vPortYieldFromISR(void);

/**
 * @brief Yields the other core
 *
 * @note Added to be compatible with SMP API
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
 * @note [refactor-todo] Check if this function should be renamed (due to bool return type)
 * @return true Core can yield
 * @return false Core cannot yield
 */
FORCE_INLINE_ATTR bool xPortCanYield(void);

// ------------------- Hook Functions ----------------------

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
 * @note [refactor-todo] Check if this function should be renamed (due to uint return type)
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
void vPortSetStackWatchpoint(void *pxStackStart);

/**
 * @brief Get the current core's ID
 *
 * @note Added to be compatible with SMP API
 * @note [refactor-todo] IDF should call a FreeRTOS like macro instead of port function directly
 * @return BaseType_t Core ID
 */
FORCE_INLINE_ATTR BaseType_t xPortGetCoreID(void)
{
    return (BaseType_t) esp_cpu_get_core_id();
}



/* ------------------------------------------- FreeRTOS Porting Interface ----------------------------------------------
 * - Contains all the mappings of the macros required by FreeRTOS
 * - Most come after forward declare as porting macros map to declared functions
 * - Maps to forward declared functions
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

#define portDISABLE_INTERRUPTS()            portSET_INTERRUPT_MASK_FROM_ISR()
#define portENABLE_INTERRUPTS()             portCLEAR_INTERRUPT_MASK_FROM_ISR(1)
#define portSET_INTERRUPT_MASK_FROM_ISR()                       vPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedStatusValue)   vPortClearInterruptMask(uxSavedStatusValue)

// ------------------ Critical Sections --------------------

#define portENTER_CRITICAL(mux)                 {(void)mux;  vPortEnterCritical();}
#define portEXIT_CRITICAL(mux)                  {(void)mux;  vPortExitCritical();}
#define portTRY_ENTER_CRITICAL(mux, timeout)    ({  \
    (void)mux; (void)timeout;                       \
    vPortEnterCritical();                           \
    BaseType_t ret = pdPASS;                        \
    ret;                                            \
})
//In single-core RISC-V, we can use the same critical section API
#define portENTER_CRITICAL_ISR(mux)                 portENTER_CRITICAL(mux)
#define portEXIT_CRITICAL_ISR(mux)                  portEXIT_CRITICAL(mux)
#define portTRY_ENTER_CRITICAL_ISR(mux, timeout)    portTRY_ENTER_CRITICAL(mux, timeout)

/* [refactor-todo] on RISC-V, both ISR and non-ISR cases result in the same call. We can redefine this macro */
#define portENTER_CRITICAL_SAFE(mux)    ({  \
    if (xPortInIsrContext()) {              \
        portENTER_CRITICAL_ISR(mux);        \
    } else {                                \
        portENTER_CRITICAL(mux);            \
    }                                       \
})
#define portEXIT_CRITICAL_SAFE(mux)     ({  \
    if (xPortInIsrContext()) {              \
        portEXIT_CRITICAL_ISR(mux);         \
    } else {                                \
        portEXIT_CRITICAL(mux);             \
    }                                       \
})
#define portTRY_ENTER_CRITICAL_SAFE(mux, timeout)   portENTER_CRITICAL_SAFE(mux, timeout)

// ---------------------- Yielding -------------------------

#define portYIELD() vPortYield()
#define portYIELD_FROM_ISR_NO_ARG() vPortYieldFromISR()
#define portYIELD_FROM_ISR_ARG(xHigherPriorityTaskWoken) ({ \
    if (xHigherPriorityTaskWoken == pdTRUE) { \
        vPortYieldFromISR(); \
    } \
})
/**
 * @note    The macro below could be used when passing a single argument, or without any argument,
 *          it was developed to support both usages of portYIELD inside of an ISR. Any other usage form
 *          might result in undesired behavior
 */
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define portYIELD_FROM_ISR(...) CHOOSE_MACRO_VA_ARG(portYIELD_FROM_ISR_ARG, portYIELD_FROM_ISR_NO_ARG __VA_OPT__(,) __VA_ARGS__)(__VA_ARGS__)
#else
#define portYIELD_FROM_ISR(...) CHOOSE_MACRO_VA_ARG(portYIELD_FROM_ISR_ARG, portYIELD_FROM_ISR_NO_ARG, ##__VA_ARGS__)(__VA_ARGS__)
#endif


#define portEND_SWITCHING_ISR(xSwitchRequired) if(xSwitchRequired) vPortYield()
/* Yielding within an API call (when interrupts are off), means the yield should be delayed
   until interrupts are re-enabled.
   To do this, we use the "cross-core" interrupt as a trigger to yield on this core when interrupts are re-enabled.This
   is the same interrupt & code path which is used to trigger a yield between CPUs, although in this case the yield is
   happening on the same CPU.
*/
#define portYIELD_WITHIN_API() portYIELD()

// ------------------- Hook Functions ----------------------

#define portSUPPRESS_TICKS_AND_SLEEP(idleTime) vApplicationSleep(idleTime)

// ------------------- Run Time Stats ----------------------

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE() 0
#ifdef CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
/* Coarse resolution time (us) */
#define portALT_GET_RUN_TIME_COUNTER_VALUE(x)    do {x = (uint32_t)esp_timer_get_time();} while(0)
#endif

// --------------------- TCB Cleanup -----------------------

#if CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP
/* If enabled, users must provide an implementation of vPortCleanUpTCB() */
extern void vPortCleanUpTCB ( void *pxTCB );
#define portCLEAN_UP_TCB( pxTCB )                   vPortCleanUpTCB( pxTCB )
#endif /* CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP */

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
 * - For implementation of non-inlined functions, see port.c, port_common.c, or other assembly files
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

// ---------------------- Yielding -------------------------

FORCE_INLINE_ATTR bool xPortCanYield(void)
{
    uint32_t threshold = REG_READ(INTERRUPT_CORE0_CPU_INT_THRESH_REG);
    /* when enter critical code, FreeRTOS will mask threshold to RVHAL_EXCM_LEVEL
     * and exit critical code, will recover threshold value (1). so threshold <= 1
     * means not in critical code
     */
    return (threshold <= 1);
}



/* ------------------------------------------------------ Misc ---------------------------------------------------------
 * - Miscellaneous porting macros
 * - These are not part of the FreeRTOS porting interface, but are used by other FreeRTOS dependent components
 * ------------------------------------------------------------------------------------------------------------------ */

// -------------------- Heap Related -----------------------

/**
 * @brief Checks if a given piece of memory can be used to store a task's TCB
 *
 * - Defined in heap_idf.c
 *
 * @param ptr Pointer to memory
 * @return true Memory can be used to store a TCB
 * @return false Otherwise
 */
bool xPortCheckValidTCBMem(const void *ptr);

/**
 * @brief Checks if a given piece of memory can be used to store a task's stack
 *
 * - Defined in heap_idf.c
 *
 * @param ptr Pointer to memory
 * @return true Memory can be used to store a task stack
 * @return false Otherwise
 */
bool xPortcheckValidStackMem(const void *ptr);

#define portVALID_TCB_MEM(ptr)      xPortCheckValidTCBMem(ptr)
#define portVALID_STACK_MEM(ptr)    xPortcheckValidStackMem(ptr)

// --------------------- App-Trace -------------------------

#if CONFIG_APPTRACE_SV_ENABLE
extern int xPortSwitchFlag;
#define os_task_switch_is_pended(_cpu_) (xPortSwitchFlag)
#else
#define os_task_switch_is_pended(_cpu_) (false)
#endif

#ifdef __cplusplus
}
#endif

#endif //__ASSEMBLER__

#endif /* PORTMACRO_H */
