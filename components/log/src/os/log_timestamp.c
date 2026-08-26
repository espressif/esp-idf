/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_cpu.h" // for esp_cpu_get_cycle_count()
#include "esp_compiler.h"
#include "esp_log_timestamp.h"
#include "sdkconfig.h"

uint32_t esp_log_timestamp(void)
{
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
        return esp_log_early_timestamp();
    }
    static uint32_t base = 0;
    if (base == 0 && xPortGetCoreID() == 0) {
        base = esp_log_early_timestamp();
    }
    TickType_t tick_count = xPortInIsrContext() ? xTaskGetTickCountFromISR() : xTaskGetTickCount();
    return base + tick_count * (1000 / configTICK_RATE_HZ);
}

/* FIXME: define an API for getting the timestamp in soc/hal IDF-2351 */
uint32_t esp_log_early_timestamp(void)
{
#if CONFIG_IDF_TARGET_ESP32
    /* ESP32 ROM stores separate clock rate values for each CPU, but we want the PRO CPU value always */
    extern uint32_t g_ticks_per_us_pro;
    return esp_cpu_get_cycle_count() / (g_ticks_per_us_pro * 1000);
#else
    return esp_cpu_get_cycle_count() / (esp_rom_get_cpu_ticks_per_us() * 1000);
#endif
}
