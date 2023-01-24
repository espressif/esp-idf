/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef FREERTOS_CONFIG_XTENSA_H
#define FREERTOS_CONFIG_XTENSA_H

#include "sdkconfig.h"

/* enable use of optimized task selection by the scheduler */
#ifdef CONFIG_FREERTOS_OPTIMIZED_SCHEDULER
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         1
#endif

#define XT_USE_THREAD_SAFE_CLIB                         0
#undef XT_USE_SWPRI

#if CONFIG_FREERTOS_CORETIMER_0
#define XT_TIMER_INDEX                                  0
#elif CONFIG_FREERTOS_CORETIMER_1
#define XT_TIMER_INDEX                                  1
#endif

#ifndef __ASSEMBLER__
/**
 * This function is defined to provide a deprecation warning whenever
 * XT_CLOCK_FREQ macro is used.
 * Update the code to use esp_clk_cpu_freq function instead.
 * @return current CPU clock frequency, in Hz
 */
int xt_clock_freq(void) __attribute__((deprecated));

#define XT_CLOCK_FREQ   (xt_clock_freq())

#endif // __ASSEMBLER__

/* Required for configuration-dependent settings */
#include <freertos/xtensa_config.h>

/* configASSERT behaviour */
#ifndef __ASSEMBLER__
#include <assert.h>
#include "esp_rom_sys.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"  // will be removed in idf v5.0
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/ets_sys.h"
#endif
#endif // __ASSEMBLER__

// If CONFIG_FREERTOS_ASSERT_DISABLE is set then configASSERT is defined empty later in FreeRTOS.h and the macro
// configASSERT_DEFINED remains unset (meaning some warnings are avoided)

#if defined(CONFIG_FREERTOS_ASSERT_FAIL_PRINT_CONTINUE)
#define configASSERT(a) if (unlikely(!(a))) {                           \
        esp_rom_printf("%s:%d (%s)- assert failed!\n", __FILE__, __LINE__,  \
                   __FUNCTION__);                                           \
    }
#elif defined(CONFIG_FREERTOS_ASSERT_FAIL_ABORT)
#define configASSERT(a) assert(a)
#endif

#if CONFIG_FREERTOS_ASSERT_ON_UNTESTED_FUNCTION
#define UNTESTED_FUNCTION() { esp_rom_printf("Untested FreeRTOS function %s\r\n", __FUNCTION__); configASSERT(false); } while(0)
#else
#define UNTESTED_FUNCTION()
#endif

#define configXT_BOARD                                  1   /* Board mode */
#define configXT_SIMULATOR                              0

/* The maximum interrupt priority from which FreeRTOS.org API functions can
   be called.  Only API functions that end in ...FromISR() can be used within
   interrupts. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY	XCHAL_EXCM_LEVEL

/* Stack alignment, architecture specifc. Must be a power of two. */
#define configSTACK_ALIGNMENT                           16


/* The Xtensa port uses a separate interrupt stack. Adjust the stack size
 * to suit the needs of your specific application.
 * Size needs to be aligned to the stack increment, since the location of
 * the stack for the 2nd CPU will be calculated using configISR_STACK_SIZE.
 */
#ifndef configISR_STACK_SIZE
#define configISR_STACK_SIZE                            ((CONFIG_FREERTOS_ISR_STACKSIZE + configSTACK_ALIGNMENT - 1) & (~(configSTACK_ALIGNMENT - 1)))
#endif

#ifndef __ASSEMBLER__
#if CONFIG_APPTRACE_SV_ENABLE
extern uint32_t port_switch_flag[];
#define os_task_switch_is_pended(_cpu_) (port_switch_flag[_cpu_])
#else
#define os_task_switch_is_pended(_cpu_) (false)
#endif
#endif

#endif // FREERTOS_CONFIG_XTENSA_H
