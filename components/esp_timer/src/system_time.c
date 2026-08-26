/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Provides strong definition for system time functions relied upon
// by core components.
#include "sdkconfig.h"

#if CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER
#include "esp_timer.h"
#include "esp_timer_impl.h"
#include "esp_system.h"
#ifndef CONFIG_IDF_TARGET_LINUX
#include "esp_newlib.h"
#endif

#include "esp_private/startup_internal.h"
#include "esp_rtc_time.h"

// Correction for underlying timer to keep definition
// of system time consistent.
static int64_t s_correction_us = 0;

#if defined(CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER) && defined(CONFIG_ESP_TIME_FUNCS_USE_RTC_TIMER)
ESP_SHUTDOWN_HANDLER_REGISTER(esp_sync_timekeeping_timers_shutdown, 100)
{
    esp_sync_timekeeping_timers();
    return ESP_OK;
}
#endif

void esp_timer_impl_init_system_time(void)
{
#ifndef CONFIG_IDF_TARGET_LINUX
    s_correction_us = esp_rtc_get_time_us() - g_startup_time - esp_timer_impl_get_time();
#endif // !CONFIG_IDF_TARGET_LINUX
}

int64_t ESP_TIMER_IRAM_ATTR esp_system_get_time(void)
{
    return esp_timer_get_time() + s_correction_us;
}

uint32_t ESP_TIMER_IRAM_ATTR esp_system_get_time_resolution(void)
{
    return 1000;
}
#endif
