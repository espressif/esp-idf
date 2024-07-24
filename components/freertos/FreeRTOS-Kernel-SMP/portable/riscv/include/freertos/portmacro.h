/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

/* Macros used instead ofsetoff() for better performance of interrupt handler */
#if CONFIG_FREERTOS_USE_LIST_DATA_INTEGRITY_CHECK_BYTES
/*
pxTopOfStack (4) +
xStateListItem (28) +
xEventListItem (28) +
uxPriority (4)
*/
#define PORT_OFFSET_PX_STACK 0x40
#else
/*
pxTopOfStack (4) +
xStateListItem (20) +
xEventListItem (20) +
uxPriority (4)
*/
#define PORT_OFFSET_PX_STACK 0x30
#endif /* #if CONFIG_FREERTOS_USE_LIST_DATA_INTEGRITY_CHECK_BYTES */

#define PORT_OFFSET_PX_END_OF_STACK ( \
    PORT_OFFSET_PX_STACK \
    + 4                                 /* StackType_t * pxStack */ \
    + CONFIG_FREERTOS_MAX_TASK_NAME_LEN /* pcTaskName[ configMAX_TASK_NAME_LEN ] */ \
)

#ifndef __ASSEMBLER__

#include <stdint.h>
#include "spinlock.h"
#include "soc/interrupt_reg.h"
#include "esp_macros.h"
#include "esp_cpu.h"
#include "esp_private/crosscore_int.h"
#include "esp_memory_utils.h"

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

#if ( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#define portMAX_DELAY ( TickType_t )  0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t )  0xffffffffUL
#endif

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )  void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void *pvParameters )

// interrupt module will mask interrupt with priority less than threshold
#define RVHAL_EXCM_LEVEL            4

/* ----------------------------------------------- Port Configurations -------------------------------------------------
 * - Configurations values supplied by each port
 * - Required by FreeRTOS
 * ------------------------------------------------------------------------------------------------------------------ */

#define portCRITICAL_NESTING_IN_TCB     1
#define portSTACK_GROWTH                ( -1 )
#define portTICK_PERIOD_MS              ( ( TickType_t ) ( 1000 / configTICK_RATE_HZ ) )
#define portBYTE_ALIGNMENT              16
#define portNOP() __asm volatile        (" nop ")

/* ---------------------------------------------- Forward Declarations -------------------------------------------------
 * - Forward declarations of all the port functions and macros needed to implement the FreeRTOS porting interface
 * - These must come before definition/declaration of the FreeRTOS porting interface
 * ------------------------------------------------------------------------------------------------------------------ */

/* ---------------------- Spinlocks ------------------------
 - Spinlocks added to match API with SMP FreeRTOS. Single core RISC-V does not need spin locks
 * ------------------------------------------------------ */

typedef spinlock_t                          portMUX_TYPE;               /**< Spinlock type used by FreeRTOS critical sections */

#define portMUX_INITIALIZER_UNLOCKED        SPINLOCK_INITIALIZER        /**< Spinlock initializer */
#define portMUX_FREE_VAL                    SPINLOCK_FREE               /**< Spinlock is free. [refactor-todo] check if this is still required */
#define portMUX_NO_TIMEOUT                  SPINLOCK_WAIT_FOREVER       /**< When passed for 'timeout_cycles', spin forever if necessary. [refactor-todo] check if this is still required */
#define portMUX_TRY_LOCK                    SPINLOCK_NO_WAIT            /**< Try to acquire the spinlock a single time only. [refactor-todo] check if this is still required */
#define portMUX_INITIALIZE(mux)             spinlock_initialize(mux)    /*< Initialize a spinlock to its unlocked state */

// --------------------- Interrupts ------------------------

BaseType_t xPortCheckIfInISR(void);

/**
 * @brief Assert if in ISR context
 *
 * - Asserts on xPortCheckIfInISR() internally
 */
void vPortAssertIfInISR(void);

// ------------------ Critical Sections --------------------

#if ( configNUMBER_OF_CORES > 1 )
/*
These are always called with interrupts already disabled. We simply need to get/release the spinlocks
*/
extern portMUX_TYPE port_xTaskLock;
extern portMUX_TYPE port_xISRLock;

void vPortTakeLock( portMUX_TYPE *lock );
void vPortReleaseLock( portMUX_TYPE *lock );
#endif /* configNUMBER_OF_CORES > 1 */

// ---------------------- Yielding -------------------------

void vPortYield( void );
static inline void __attribute__((always_inline)) vPortYieldCore( BaseType_t xCoreID );

/**
 * @brief Set interrupt mask and return current interrupt enable register
 *
 * @return UBaseType_t Current interrupt enable register before set
 */
UBaseType_t ulPortSetInterruptMask(void);

/**
 * @brief Clear current interrupt mask and set given mask
 *
 * @param mask Interrupt mask
 */
void vPortClearInterruptMask(UBaseType_t mask);

/**
 * @brief Perform a context switch from an ISR
 */
void vPortYieldFromISR(void);

#define portYIELD_FROM_ISR_CHECK(x)     ({ \
    if ( (x) == pdTRUE ) { \
        vPortYieldFromISR(); \
    } \
})
#define portYIELD_FROM_ISR_NO_CHECK()               vPortYieldFromISR()

// ----------------------- System --------------------------

static inline BaseType_t __attribute__((always_inline)) xPortGetCoreID( void );

// --------------------- TCB Cleanup -----------------------

/**
 * @brief TCB cleanup hook
 *
 * The portCLEAN_UP_TCB() macro is called in prvDeleteTCB() right before a
 * deleted task's memory is freed. We map that macro to this internal function
 * so that IDF FreeRTOS ports can inject some task pre-deletion operations.
 *
 * @note We can't use vPortCleanUpTCB() due to API compatibility issues. See
 * CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP. Todo: IDF-8097
 */
void vPortTCBPreDeleteHook( void *pxTCB );

/* ------------------------------------------- FreeRTOS Porting Interface ----------------------------------------------
 * - Contains all the mappings of the macros required by FreeRTOS
 * - Most come after forward declare as porting macros map to declared functions
 * - Maps to forward declared functions
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- Interrupts ------------------------

#define portDISABLE_INTERRUPTS()                    ulPortSetInterruptMask()
#define portENABLE_INTERRUPTS()                     vPortClearInterruptMask(1)
#define portRESTORE_INTERRUPTS(x)                   vPortClearInterruptMask(x)

/**
 * @brief Assert if in ISR context
 *
 * TODO: Enable once ISR safe version of vTaskEnter/ExitCritical() is implemented
 * for single-core SMP FreeRTOS Kernel. (IDF-10540)
 */
// #define portASSERT_IF_IN_ISR() vPortAssertIfInISR()


// ------------------ Critical Sections --------------------

#if ( configNUMBER_OF_CORES > 1 )
#define portGET_TASK_LOCK()                         vPortTakeLock(&port_xTaskLock)
#define portRELEASE_TASK_LOCK()                     vPortReleaseLock(&port_xTaskLock)
#define portGET_ISR_LOCK()                          vPortTakeLock(&port_xISRLock)
#define portRELEASE_ISR_LOCK()                      vPortReleaseLock(&port_xISRLock)
#endif /* configNUMBER_OF_CORES > 1 */

//Critical sections used by FreeRTOS SMP
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
#define portENTER_CRITICAL_SMP()                    vTaskEnterCritical();
#define portEXIT_CRITICAL_SMP()                     vTaskExitCritical();

#if defined(__cplusplus) && (__cplusplus >  201703L)
#define portENTER_CRITICAL(...)                     CHOOSE_MACRO_VA_ARG(portENTER_CRITICAL_IDF, portENTER_CRITICAL_SMP __VA_OPT__(,) __VA_ARGS__)(__VA_ARGS__)
#define portEXIT_CRITICAL(...)                      CHOOSE_MACRO_VA_ARG(portEXIT_CRITICAL_IDF, portEXIT_CRITICAL_SMP __VA_OPT__(,) __VA_ARGS__)(__VA_ARGS__)
#else
#define portENTER_CRITICAL(...)                     CHOOSE_MACRO_VA_ARG(portENTER_CRITICAL_IDF, portENTER_CRITICAL_SMP, ##__VA_ARGS__)(__VA_ARGS__)
#define portEXIT_CRITICAL(...)                      CHOOSE_MACRO_VA_ARG(portEXIT_CRITICAL_IDF, portEXIT_CRITICAL_SMP, ##__VA_ARGS__)(__VA_ARGS__)
#endif

#define portSET_INTERRUPT_MASK_FROM_ISR() ({ \
    unsigned int cur_level; \
    cur_level = REG_READ(INTERRUPT_CURRENT_CORE_INT_THRESH_REG); \
    vTaskEnterCritical(); \
    cur_level; \
})
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) ({ \
    vTaskExitCritical(); \
    portRESTORE_INTERRUPTS(x); \
})

// ---------------------- Yielding -------------------------

#define portYIELD()                                 vPortYield()
#if defined(__cplusplus) && (__cplusplus >  201703L)
#define portYIELD_FROM_ISR(...)                     CHOOSE_MACRO_VA_ARG(portYIELD_FROM_ISR_CHECK, portYIELD_FROM_ISR_NO_CHECK __VA_OPT__(,) __VA_ARGS__)(__VA_ARGS__)
#else
#define portYIELD_FROM_ISR(...)                     CHOOSE_MACRO_VA_ARG(portYIELD_FROM_ISR_CHECK, portYIELD_FROM_ISR_NO_CHECK, ##__VA_ARGS__)(__VA_ARGS__)
#endif
#define portYIELD_CORE(x)                           vPortYieldCore(x)

// ----------------------- System --------------------------

#define portGET_CORE_ID()                           xPortGetCoreID()
#define portCHECK_IF_IN_ISR()                       xPortCheckIfInISR()

// ------------------- Run Time Stats ----------------------

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#ifdef CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
#define portGET_RUN_TIME_COUNTER_VALUE()        ((configRUN_TIME_COUNTER_TYPE) esp_timer_get_time())
#else
#define portGET_RUN_TIME_COUNTER_VALUE()        0
#endif // CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER

// --------------------- TCB Cleanup -----------------------

#define portCLEAN_UP_TCB( pxTCB )                   vPortTCBPreDeleteHook( pxTCB )


/* --------------------------------------------- Inline Implementations ------------------------------------------------
 * - Implementation of inline functions of the forward declares
 * - Should come after forward declare and FreeRTOS Porting interface, as implementation may use both.
 * - For implementation of non-inlined functions, see port.c, port_common.c, or other assembly files
 * ------------------------------------------------------------------------------------------------------------------ */

// ---------------------- Yielding -------------------------

static inline void __attribute__((always_inline)) vPortYieldCore( BaseType_t xCoreID )
{
    esp_crosscore_int_send_yield( xCoreID );
}

// ----------------------- System --------------------------

static inline BaseType_t __attribute__((always_inline)) xPortGetCoreID( void )
{
    return (BaseType_t) esp_cpu_get_core_id();
}

/* ------------------------------------------------ IDF Compatibility --------------------------------------------------
 * - These macros and functions need to be defined for IDF to compile
 * ------------------------------------------------------------------------------------------------------------------ */

static inline BaseType_t xPortInIsrContext(void)
{
    //Just call the FreeRTOS port interface version
    return xPortCheckIfInISR();
}

// Added for backward compatibility with IDF
#define xPortInterruptedFromISRContext()    xPortInIsrContext()

// ------------------ Critical Sections --------------------

/*
IDF style critical sections which are orthogonal to FreeRTOS critical sections. However, on single core, the IDF style
critical sections simply disable interrupts, thus we discard the lock and timeout arguments.

Note: The IDF Style critical sections are named differently to their counterparts present in the xtensa port as few IDF
examples such as esp_zigbee_gateway, when compiled for RISC-V targets, have a reference to vPortEnterCritical()
and vPortExitCritical() from precompiled libraries (.a) thereby failing linking.
*/
void vPortEnterCritical(void);
void vPortExitCritical(void);

//IDF task critical sections
#define portTRY_ENTER_CRITICAL(lock, timeout)       ({(void) lock; (void) timeout; vPortEnterCritical(); pdPASS;})
#define portENTER_CRITICAL_IDF(lock)                ({(void) lock; vPortEnterCritical();})
#define portEXIT_CRITICAL_IDF(lock)                 ({(void) lock; vPortExitCritical();})
//IDF ISR critical sections
#define portTRY_ENTER_CRITICAL_ISR(lock, timeout)   ({(void) lock; (void) timeout; vPortEnterCritical(); pdPASS;})
#define portENTER_CRITICAL_ISR(lock)                ({(void) lock; vPortEnterCritical();})
#define portEXIT_CRITICAL_ISR(lock)                 ({(void) lock; vPortExitCritical();})
//IDF safe critical sections (they're the same)
#define portENTER_CRITICAL_SAFE(lock)               ({(void) lock; vPortEnterCritical();})
#define portEXIT_CRITICAL_SAFE(lock)                ({(void) lock; vPortExitCritical();})

// ---------------------- Yielding -------------------------

static inline bool IRAM_ATTR xPortCanYield(void)
{
    uint32_t threshold = REG_READ(INTERRUPT_CURRENT_CORE_INT_THRESH_REG);
#if SOC_INT_CLIC_SUPPORTED
    threshold = threshold >> (CLIC_CPU_INT_THRESH_S + (8 - NLBITS));

    /* When CLIC is supported, the lowest interrupt threshold level is 0.
     * Therefore, an interrupt threshold level above 0 would mean that we
     * are either in a critical section or in an ISR.
     */
    return (threshold == 0);
#endif /* SOC_INT_CLIC_SUPPORTED */
    /* when enter critical code, FreeRTOS will mask threshold to RVHAL_EXCM_LEVEL
     * and exit critical code, will recover threshold value (1). so threshold <= 1
     * means not in critical code
     */
    return (threshold <= 1);
}

// Defined even for configNUMBER_OF_CORES > 1 for IDF compatibility
#define portYIELD_WITHIN_API()                      esp_crosscore_int_send_yield(xPortGetCoreID())

// ----------------------- System --------------------------

void vPortSetStackWatchpoint(void *pxStackStart);

// -------------------- Heap Related -----------------------

/**
 * @brief Checks if a given piece of memory can be used to store a FreeRTOS list
 *
 * - Defined in heap_idf.c
 *
 * @param ptr Pointer to memory
 * @return true Memory can be used to store a List
 * @return false Otherwise
 */
bool xPortCheckValidListMem(const void *ptr);

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

#define portVALID_LIST_MEM(ptr)     xPortCheckValidListMem(ptr)
#define portVALID_TCB_MEM(ptr)      xPortCheckValidTCBMem(ptr)
#define portVALID_STACK_MEM(ptr)    xPortcheckValidStackMem(ptr)

/* ------------------------------------------------------ Misc ---------------------------------------------------------
 * - Miscellaneous porting macros
 * - These are not part of the FreeRTOS porting interface, but are used by other FreeRTOS dependent components
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------------- App-Trace -------------------------

#if CONFIG_APPTRACE_SV_ENABLE
extern volatile BaseType_t xPortSwitchFlag;
#define os_task_switch_is_pended(_cpu_) (xPortSwitchFlag)
#else
#define os_task_switch_is_pended(_cpu_) (false)
#endif

// --------------- Compatibility Includes ------------------
/*
ESP-IDF currently does not have a "Include what you use" policy. A lot of files implicitly pull in API through
portmacro.h. Therefore, we need to keep these headers around for now to allow the rest of IDF to compile.

[refactor-todo] Clean up ESP-IDF inclusion dependencies and add a inclusion check.
*/

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "esp_attr.h"
#include "esp_newlib.h"
#include "esp_heap_caps.h"
#include "esp_rom_sys.h"
#include "esp_system.h"             /* required by esp_get_...() functions in portable.h. [refactor-todo] Update portable.h */

/* [refactor-todo] These includes are not directly used in this file. They are kept into to prevent a breaking change. Remove these. */
#include <limits.h>

/* [refactor-todo] introduce a port wrapper function to avoid including esp_timer.h into the public header */
#if CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
#include "esp_timer.h"
#endif

#ifdef __cplusplus
}
#endif

#endif // __ASSEMBLER__
