/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FREERTOS_CONFIG_XTENSA_H
#define FREERTOS_CONFIG_XTENSA_H

//Xtensa Archiecture specific configuration. This file is included in the common FreeRTOSConfig.h.

#include "sdkconfig.h"

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

/* ------------------------------------------------- FreeRTOS Config ---------------------------------------------------
 * - All Vanilla FreeRTOS configuration goes into this section
 * ------------------------------------------------------------------------------------------------------------------ */

// ------------------ Scheduler Related --------------------

#ifdef CONFIG_FREERTOS_OPTIMIZED_SCHEDULER
#define configUSE_PORT_OPTIMISED_TASK_SELECTION             1
#else
#define configUSE_PORT_OPTIMISED_TASK_SELECTION             0
#endif
#define configMAX_API_CALL_INTERRUPT_PRIORITY               XCHAL_EXCM_LEVEL

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

#endif // FREERTOS_CONFIG_XTENSA_H
