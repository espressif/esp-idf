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
#include "soc/spinlock.h"
#include "soc/interrupt_core0_reg.h"
#include "soc/cpu.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"              /* required for FreeRTOS run time stats */
#include "esp_heap_caps.h"
#include "esp_system.h"             /* required by esp_get_...() functions in portable.h. [refactor-todo] Update portable.h */
#include "esp_newlib.h"
#include "portbenchmark.h"

/* [refactor-todo] These includes are not directly used in this file. They are kept into to prevent a breaking change. Remove these. */
#include <limits.h>
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

/**
 * @brief Wrapper for atomic compare-and-set instruction
 *
 * @note Isn't a real atomic CAS.
 * @note [refactor-todo] check if we still need this
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
 * @note Isn't a real atomic CAS.
 * @note [refactor-todo] check if we still need this
 * @note [refactor-todo] Check if this function should be renamed (due to void return type)
 *
 * @param[inout] addr Pointer to target address
 * @param[in] compare Compare value
 * @param[inout] set Pointer to set value
 */
static inline void uxPortCompareSetExtram(volatile uint32_t *addr, uint32_t compare, uint32_t *set);

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
static inline bool IRAM_ATTR xPortCanYield(void);

// ------------------- Hook Functions ----------------------

extern void esp_vApplicationIdleHook(void);
extern void esp_vApplicationTickHook(void);

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
static inline BaseType_t IRAM_ATTR xPortGetCoreID(void)
{
    return (uint32_t) cpu_hal_get_core_id();
}



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
#define pvPortMallocTcbMem(size)        pvPortMalloc(size)
#define pvPortMallocStackMem(size)      pvPortMalloc(size)

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
#define portYIELD_FROM_ISR() vPortYieldFromISR()
#define portEND_SWITCHING_ISR(xSwitchRequired) if(xSwitchRequired) vPortYield()
/* Yielding within an API call (when interrupts are off), means the yield should be delayed
   until interrupts are re-enabled.
   To do this, we use the "cross-core" interrupt as a trigger to yield on this core when interrupts are re-enabled.This
   is the same interrupt & code path which is used to trigger a yield between CPUs, although in this case the yield is
   happening on the same CPU.
*/
#define portYIELD_WITHIN_API() portYIELD()

// ------------------- Hook Functions ----------------------

#ifndef CONFIG_FREERTOS_LEGACY_HOOKS
#define vApplicationIdleHook    esp_vApplicationIdleHook
#define vApplicationTickHook    esp_vApplicationTickHook
#endif /* !CONFIG_FREERTOS_LEGACY_HOOKS */
#define portSUPPRESS_TICKS_AND_SLEEP(idleTime) vApplicationSleep(idleTime)

// ------------------- Run Time Stats ----------------------

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE() 0
#ifdef CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
/* Coarse resolution time (us) */
#define portALT_GET_RUN_TIME_COUNTER_VALUE(x)    do {x = (uint32_t)esp_timer_get_time();} while(0)
#endif



/* --------------------------------------------- Inline Implementations ------------------------------------------------
 * - Implementation of inline functions of the forward declares
 * - Should come after forward declare and FreeRTOS Porting interface, as implementation may use both.
 * - For implementation of non-inlined functions, see port.c, port_common.c, or other assembly files
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------



// ---------------------- Spinlocks ------------------------

static inline void __attribute__((always_inline)) uxPortCompareSet(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
    compare_and_set_native(addr, compare, set);
}

static inline void uxPortCompareSetExtram(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
#if defined(CONFIG_SPIRAM)
    compare_and_set_extram(addr, compare, set);
#endif
}

// ---------------------- Yielding -------------------------

static inline bool IRAM_ATTR xPortCanYield(void)
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
 * - These are not port of the FreeRTOS porting interface, but are used by other FreeRTOS dependent components
 * ------------------------------------------------------------------------------------------------------------------ */

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

#endif //__ASSEMBLER__

#endif /* PORTMACRO_H */
