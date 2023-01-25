/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FREERTOS_CONFIG_RISCV_H
#define FREERTOS_CONFIG_RISCV_H

// This file is included in the common FreeRTOSConfig.h.

#include "sdkconfig.h"

#define configUSE_PORT_OPTIMISED_TASK_SELECTION         0

#ifndef __ASSEMBLER__
#if CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/ets_sys.h"
#endif
#endif // __ASSEMBLER__

/* The maximum interrupt priority from which FreeRTOS.org API functions can
   be called.  Only API functions that end in ...FromISR() can be used within
   interrupts. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            0

#ifndef configISR_STACK_SIZE
#define configISR_STACK_SIZE                            (CONFIG_FREERTOS_ISR_STACKSIZE)
#endif

#ifndef __ASSEMBLER__
#if CONFIG_APPTRACE_SV_ENABLE
extern int xPortSwitchFlag;
#define os_task_switch_is_pended(_cpu_) (xPortSwitchFlag)
#else
#define os_task_switch_is_pended(_cpu_) (false)
#endif
#endif

#endif // FREERTOS_CONFIG_RISCV_H
