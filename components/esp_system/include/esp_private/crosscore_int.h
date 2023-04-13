/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ESP_CROSSCORE_INT_H
#define __ESP_CROSSCORE_INT_H

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the crosscore interrupt system for this CPU.
 * This needs to be called once on every CPU that is used
 * by FreeRTOS.
 *
 * If multicore FreeRTOS support is enabled, this will be
 * called automatically by the startup code and should not
 * be called manually.
 */
void esp_crosscore_int_init(void);


/**
 * Send an interrupt to a CPU indicating it should yield its
 * currently running task in favour of a higher-priority task
 * that presumably just woke up.
 *
 * This is used internally by FreeRTOS in multicore mode
 * and should not be called by the user.
 *
 * @param core_id Core that should do the yielding
 */
void esp_crosscore_int_send_yield(int core_id);


/**
 * Send an interrupt to a CPU indicating it should update its
 * CCOMPARE1 value due to a frequency switch.
 *
 * This is used internally when dynamic frequency switching is
 * enabled, and should not be called from application code.
 *
 * @param core_id Core that should update its CCOMPARE1 value
 */
void esp_crosscore_int_send_freq_switch(int core_id);

void esp_crosscore_int_send_gdb_call(int core_id);

#if !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32C2 && !CONFIG_IDF_TARGET_ESP32C6 && !CONFIG_IDF_TARGET_ESP32H2
/**
 * Send an interrupt to a CPU indicating it should print its current backtrace
 *
 * This is used internally by the Task Watchdog to dump the backtrace of the
 * opposite core and should not be called from application code.
 *
 * @param core_id Core that should print its backtrace
 */
void esp_crosscore_int_send_print_backtrace(int core_id);

#if CONFIG_ESP_TASK_WDT_EN
/**
 * Send an interrupt to a CPU indicating it call `task_wdt_timeout_abort_xtensa`.
 * This will make the CPU abort, using the interrupted task frame.
 *
 * This is used internally by the Task Watchdog when it should abort after a task,
 * running on the other core than the one running the TWDT ISR, failed to reset
 * its timer.
 *
 * @param core_id Core that should abort
 */
void esp_crosscore_int_send_twdt_abort(int core_id);

#endif // CONFIG_ESP_TASK_WDT_EN
#endif // !CONFIG_IDF_TARGET_ESP32C3 && !CONFIG_IDF_TARGET_ESP32C2 && !CONFIG_IDF_TARGET_ESP32C6

#ifdef __cplusplus
}
#endif

#endif
