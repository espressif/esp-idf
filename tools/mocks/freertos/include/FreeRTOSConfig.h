/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define STACK_OVERHEAD_CHECKER                          256
#define STACK_OVERHEAD_OPTIMIZATION                     320
#define STACK_OVERHEAD_APPTRACE                         1280
#define STACK_OVERHEAD_WATCHPOINT                       60
#define configSTACK_OVERHEAD_TOTAL (                              \
                                    STACK_OVERHEAD_CHECKER +      \
                                    STACK_OVERHEAD_OPTIMIZATION + \
                                    STACK_OVERHEAD_APPTRACE +     \
                                    STACK_OVERHEAD_WATCHPOINT     \
                                                            )
#define configMINIMAL_STACK_SIZE                        ( 768 + configSTACK_OVERHEAD_TOTAL )
#define configMAX_PRIORITIES                            ( 25 )  //This has impact on speed of search for highest priority
#define configUSE_PREEMPTION                            1
#define configUSE_IDLE_HOOK                             CONFIG_FREERTOS_USE_IDLE_HOOK
#define configUSE_TICK_HOOK                             CONFIG_FREERTOS_USE_TICK_HOOK
#define configUSE_16_BIT_TICKS                          0
#define configUSE_TRACE_FACILITY 1
#define configSUPPORT_DYNAMIC_ALLOCATION                1
#define configUSE_MUTEXES                               1
#define configUSE_RECURSIVE_MUTEXES                     1
