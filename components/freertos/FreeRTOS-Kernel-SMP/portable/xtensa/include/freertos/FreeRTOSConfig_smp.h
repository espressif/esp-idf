/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FREERTOS_CONFIG_SMP_H
#define FREERTOS_CONFIG_SMP_H

#include "sdkconfig.h"

/*
This file get's pulled into assembly sources. Therefore, some includes need to be wrapped in #ifndef __ASSEMBLER__
*/

#ifndef __ASSEMBLER__
#include <assert.h>         //For configASSERT()
#endif /* def __ASSEMBLER__ */

/* Required for configuration-dependent settings. */
#include "xtensa_config.h"

/* -------------------------------------------- Xtensa Additional Config  ----------------------------------------------
 * - Provide Xtensa definitions usually given by -D option when building with xt-make (see readme_xtensa.txt)
 * - xtensa_rtos.h and xtensa_timer.h will default some of these values
 *      - XT_SIMULATOR         configXT_SIMULATOR
 *      - XT_BOARD             configXT_BOARD
 *      - XT_CLOCK_FREQ        Should not be defined as we are using XT_BOARD mode
 *      - XT_TICK_PER_SEC      Defaults to configTICK_RATE_HZ
 *      - XT_TIMER_INDEX       Defaults to configXT_TIMER_INDEX
 *      - XT_INTEXC_HOOKS      Defaults to configXT_INTEXC_HOOKS
 *      - XT_USE_OVLY          We don't define this (unused)
 *      - XT_USE_SWPRI         We don't define this (unused)
 * ------------------------------------------------------------------------------------------------------------------ */

#define configXT_SIMULATOR                                  0
#define configXT_BOARD                                      1   /* Board mode */
#if CONFIG_FREERTOS_CORETIMER_0
#define configXT_TIMER_INDEX                                0
#elif CONFIG_FREERTOS_CORETIMER_1
#define configXT_TIMER_INDEX                                1
#endif
#define configXT_INTEXC_HOOKS                               0

#define configBENCHMARK                                     0

/* ------------------------------------------------ ESP-IDF Additions --------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

/* The Xtensa port uses a separate interrupt stack. Adjust the stack size
 * to suit the needs of your specific application.
 * Size needs to be aligned to the stack increment, since the location of
 * the stack for the 2nd CPU will be calculated using configISR_STACK_SIZE.
 */
#define configSTACK_ALIGNMENT                               16
#ifndef configISR_STACK_SIZE
#define configISR_STACK_SIZE                                ((CONFIG_FREERTOS_ISR_STACKSIZE + configSTACK_ALIGNMENT - 1) & (~(configSTACK_ALIGNMENT - 1)))
#endif

/* ----------------------------------------------------- Helpers -------------------------------------------------------
 * - Macros that the FreeRTOS configuration macros depend on
 * ------------------------------------------------------------------------------------------------------------------ */

/* Higher stack checker modes cause overhead on each function call */
#if CONFIG_STACK_CHECK_ALL || CONFIG_STACK_CHECK_STRONG
#define STACK_OVERHEAD_CHECKER                          256
#else
#define STACK_OVERHEAD_CHECKER                          0
#endif

/* with optimizations disabled, scheduler uses additional stack */
#if CONFIG_COMPILER_OPTIMIZATION_NONE
#define STACK_OVERHEAD_OPTIMIZATION                     320
#else
#define STACK_OVERHEAD_OPTIMIZATION                     0
#endif

/* apptrace mdule increases minimum stack usage */
#if CONFIG_APPTRACE_ENABLE
#define STACK_OVERHEAD_APPTRACE                         1280
#else
#define STACK_OVERHEAD_APPTRACE                         0
#endif

/* Stack watchpoint decreases minimum usable stack size by up to 60 bytes.
   See FreeRTOS FREERTOS_WATCHPOINT_END_OF_STACK option in Kconfig. */
#if CONFIG_FREERTOS_WATCHPOINT_END_OF_STACK
#define STACK_OVERHEAD_WATCHPOINT                       60
#else
#define STACK_OVERHEAD_WATCHPOINT                       0
#endif

#define configSTACK_OVERHEAD_TOTAL (                              \
                                    STACK_OVERHEAD_CHECKER +      \
                                    STACK_OVERHEAD_OPTIMIZATION + \
                                    STACK_OVERHEAD_APPTRACE +     \
                                    STACK_OVERHEAD_WATCHPOINT     \
                                                            )

/* ----------------------------------------------------- Helpers -------------------------------------------------------
 * - Macros that the FreeRTOS configuration macros depend on
 * ------------------------------------------------------------------------------------------------------------------ */


/* ------------------------------------------------- FreeRTOS Config ---------------------------------------------------
 * - All Vanilla FreeRTOS configuration goes into this section
 * - Keep this section in-sync with the corresponding version of single-core upstream version of FreeRTOS
 * - Don't put any SMP or ESP-IDF exclusive FreeRTOS configurations here. Those go into the next section
 * - Not all FreeRTOS configuration are listed. Some configurations have default values set in FreeRTOS.h thus don't
 *   need to be explicitly defined.
 * ------------------------------------------------------------------------------------------------------------------ */

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

// ------------------ Scheduler Related --------------------

#define configUSE_PREEMPTION                            1
#define configUSE_TASK_PREEMPTION_DISABLE               1
#define configUSE_TICKLESS_IDLE                         0
#define configCPU_CLOCK_HZ                              (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000)
#define configTICK_RATE_HZ                              CONFIG_FREERTOS_HZ
#define configMAX_PRIORITIES                            ( 25 )  //This has impact on speed of search for highest priority
#define configMINIMAL_STACK_SIZE                        ( 768 + configSTACK_OVERHEAD_TOTAL )
#define configUSE_TIME_SLICING                          1
#define configUSE_16_BIT_TICKS                          0
#define configIDLE_SHOULD_YIELD                         0   //Todo: Check this
#define configKERNEL_INTERRUPT_PRIORITY                 1   //Todo: This currently isn't used anywhere
#define configMAX_API_CALL_INTERRUPT_PRIORITY           XCHAL_EXCM_LEVEL
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         0   //SMP currently does not support optimized selection

// -------------------- FreeRTOS SMP -----------------------

#ifdef CONFIG_FREERTOS_UNICORE
#define configNUM_CORES                                 1
#else
#define configNUM_CORES                                 2
#endif
#define configUSE_CORE_AFFINITY                         1
#define configRUN_MULTIPLE_PRIORITIES                   1
#define configUSE_MINIMAL_IDLE_HOOK                     1

// ------------- Synchronization Primitives ----------------

#define configUSE_MUTEXES                               1
#define configUSE_RECURSIVE_MUTEXES                     1
#define configUSE_COUNTING_SEMAPHORES                   1
#define configUSE_QUEUE_SETS                            1
#define configQUEUE_REGISTRY_SIZE                       CONFIG_FREERTOS_QUEUE_REGISTRY_SIZE
#define configUSE_TASK_NOTIFICATIONS                    1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES           1

// ----------------------- System --------------------------

#define configMAX_TASK_NAME_LEN                         CONFIG_FREERTOS_MAX_TASK_NAME_LEN

#define configNUM_THREAD_LOCAL_STORAGE_POINTERS         CONFIG_FREERTOS_THREAD_LOCAL_STORAGE_POINTERS
#define configSTACK_DEPTH_TYPE                          uint32_t
#define configUSE_NEWLIB_REENTRANT                      1
#define configENABLE_BACKWARD_COMPATIBILITY             0
#define configASSERT(a)                                 assert(a)
#define configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H       1

// ----------------------- Memory  -------------------------

#define configSUPPORT_STATIC_ALLOCATION                 1
#define configSUPPORT_DYNAMIC_ALLOCATION                1
//We define the heap to span all of the non-statically-allocated shared RAM. ToDo: Make sure there
//is some space left for the app and main cpu when running outside of a thread.
#define configTOTAL_HEAP_SIZE                           (&_heap_end - &_heap_start)//( ( size_t ) (64 * 1024) )
#define configAPPLICATION_ALLOCATED_HEAP                1
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP       0   //Todo: Check this

// ------------------------ Hooks --------------------------

#define configUSE_IDLE_HOOK                             1
#define configUSE_TICK_HOOK                             1
#if CONFIG_FREERTOS_CHECK_STACKOVERFLOW_NONE
#define configCHECK_FOR_STACK_OVERFLOW                  0
#elif CONFIG_FREERTOS_CHECK_STACKOVERFLOW_PTRVAL
#define configCHECK_FOR_STACK_OVERFLOW                  1
#elif CONFIG_FREERTOS_CHECK_STACKOVERFLOW_CANARY
#define configCHECK_FOR_STACK_OVERFLOW                  2
#endif
#define configRECORD_STACK_HIGH_ADDRESS                 1

// ------------------- Run-time Stats ----------------------

#ifdef CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS
#define configGENERATE_RUN_TIME_STATS                   1   /* Used by vTaskGetRunTimeStats() */
#endif
#ifdef CONFIG_FREERTOS_USE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY                        1   /* Used by uxTaskGetSystemState(), and other trace facility functions */
#endif
#ifdef CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS
#define configUSE_STATS_FORMATTING_FUNCTIONS            1   /* Used by vTaskList() */
#endif

// -------------------- Co-routines  -----------------------

#define configUSE_CO_ROUTINES                           0
#define configMAX_CO_ROUTINE_PRIORITIES                 2

// ------------------- Software Timer ----------------------

#define configUSE_TIMERS                                1
#define configTIMER_TASK_PRIORITY                       CONFIG_FREERTOS_TIMER_TASK_PRIORITY
#define configTIMER_QUEUE_LENGTH                        CONFIG_FREERTOS_TIMER_QUEUE_LENGTH
#define configTIMER_TASK_STACK_DEPTH                    CONFIG_FREERTOS_TIMER_TASK_STACK_DEPTH

// -------------------- API Includes -----------------------

#define configENABLE_BACKWARD_COMPATIBILITY             0

#define INCLUDE_vTaskPrioritySet                        1
#define INCLUDE_uxTaskPriorityGet                       1
#define INCLUDE_vTaskDelete                             1
#define INCLUDE_vTaskSuspend                            1
#define INCLUDE_xTaskDelayUntil                         1
#define INCLUDE_vTaskDelay                              1
#define INCLUDE_xTaskGetIdleTaskHandle                  1
#define INCLUDE_xTaskAbortDelay                         1
#define INCLUDE_xSemaphoreGetMutexHolder                1
#define INCLUDE_xTaskGetHandle                          1
#define INCLUDE_uxTaskGetStackHighWaterMark             1
#define INCLUDE_uxTaskGetStackHighWaterMark2            0
#define INCLUDE_eTaskGetState                           1
#define INCLUDE_xTaskResumeFromISR                      1
#define INCLUDE_xTimerPendFunctionCall                  1
#define INCLUDE_xTaskGetSchedulerState                  1
#define INCLUDE_xTaskGetCurrentTaskHandle               1

// -------------------- Trace Macros -----------------------

/*
For trace macros.
Note: Include trace macros here and not above as trace macros are dependent on some of the FreeRTOS configs
*/
#ifndef __ASSEMBLER__
#if CONFIG_SYSVIEW_ENABLE
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#undef INLINE // to avoid redefinition
#endif //CONFIG_SYSVIEW_ENABLE
#endif /* def __ASSEMBLER__ */

/*
Default values for trace macros added by ESP-IDF and are not part of Vanilla FreeRTOS
*/
#ifndef traceISR_EXIT
    #define traceISR_EXIT()
#endif
#ifndef traceISR_ENTER
    #define traceISR_ENTER(_n_)
#endif

/* ------------------------------------------------ IDF Compatibility --------------------------------------------------
 * - We need these in order for ESP-IDF to compile
 * ------------------------------------------------------------------------------------------------------------------ */

#define portNUM_PROCESSORS                              configNUM_CORES
#ifdef CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID
#define configTASKLIST_INCLUDE_COREID                   1
#endif

#define configTHREAD_LOCAL_STORAGE_DELETE_CALLBACKS     1

#ifndef __ASSEMBLER__
#if CONFIG_APPTRACE_SV_ENABLE
extern uint32_t port_switch_flag[];
#define os_task_switch_is_pended(_cpu_) (port_switch_flag[_cpu_])
#else
#define os_task_switch_is_pended(_cpu_) (false)
#endif
#endif

// ---------------------- Features -------------------------

#ifdef CONFIG_FREERTOS_ENABLE_TASK_SNAPSHOT
#define configENABLE_TASK_SNAPSHOT                      1
#else
#define configENABLE_TASK_SNAPSHOT                      0
#endif

/* These currently aren't required, but could be useful additions in the future */
#if 0
#ifndef configIDLE_TASK_STACK_SIZE
#define configIDLE_TASK_STACK_SIZE                      CONFIG_FREERTOS_IDLE_TASK_STACKSIZE
#endif
#if CONFIG_FREERTOS_CHECK_MUTEX_GIVEN_BY_OWNER
#define configCHECK_MUTEX_GIVEN_BY_OWNER                1
#else
#define configCHECK_MUTEX_GIVEN_BY_OWNER                0
#endif

#ifndef __ASSEMBLER__
#if CONFIG_FREERTOS_ENABLE_STATIC_TASK_CLEAN_UP
extern void vPortCleanUpTCB ( void *pxTCB );
#define portCLEAN_UP_TCB( pxTCB )                       vPortCleanUpTCB( pxTCB )
#endif
#endif
#endif //0

// -------------------- Compatibility ----------------------

// backward compatibility for 4.4
#define xTaskRemoveFromUnorderedEventList vTaskRemoveFromUnorderedEventList

#endif /* FREERTOS_CONFIG_SMP_H */
