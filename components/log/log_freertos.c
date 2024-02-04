/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_cpu.h" // for esp_cpu_get_cycle_count()
#include "esp_compiler.h"
#include "esp_log.h"
#include "esp_log_private.h"

// Maximum time to wait for the mutex in a logging statement.
//
// We don't expect this to happen in most cases, as contention is low. The most likely case is if a
// log function is called from an ISR (technically caller should use the ISR-friendly logging macros but
// possible they use the normal one instead and disable the log type by tag).
#define MAX_MUTEX_WAIT_MS 10
#define MAX_MUTEX_WAIT_TICKS ((MAX_MUTEX_WAIT_MS + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS)

static SemaphoreHandle_t s_log_mutex = NULL;

void esp_log_impl_lock(void)
{
    if (unlikely(!s_log_mutex)) {
        s_log_mutex = xSemaphoreCreateMutex();
    }
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
        return;
    }
    xSemaphoreTake(s_log_mutex, portMAX_DELAY);
}

bool esp_log_impl_lock_timeout(void)
{
    if (unlikely(!s_log_mutex)) {
        s_log_mutex = xSemaphoreCreateMutex();
    }
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
        return true;
    }
    return xSemaphoreTake(s_log_mutex, MAX_MUTEX_WAIT_TICKS) == pdTRUE;
}

void esp_log_impl_unlock(void)
{
    if (unlikely(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)) {
        return;
    }
    xSemaphoreGive(s_log_mutex);
}

char *esp_log_system_timestamp(void)
{
    static char buffer[18] = {0};
    static _lock_t bufferLock = 0;

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        uint32_t timestamp = esp_log_early_timestamp();
        for (uint8_t i = 0; i < sizeof(buffer); i++) {
            if ((timestamp > 0) || (i == 0)) {
                for (uint8_t j = sizeof(buffer) - 1; j > 0; j--) {
                    buffer[j] = buffer[j - 1];
                }
                buffer[0] = (char)(timestamp % 10) + '0';
                timestamp /= 10;
            } else {
                buffer[i] = 0;
                break;
            }
        }
        return buffer;
    } else {
        struct timeval tv;
        struct tm timeinfo;

        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &timeinfo);

        _lock_acquire(&bufferLock);
        snprintf(buffer, sizeof(buffer),
                 "%02d:%02d:%02d.%03ld",
                 timeinfo.tm_hour,
                 timeinfo.tm_min,
                 timeinfo.tm_sec,
                 tv.tv_usec / 1000);
        _lock_release(&bufferLock);

        return buffer;
    }
}

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
