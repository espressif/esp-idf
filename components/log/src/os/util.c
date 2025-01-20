/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_private/log_util.h"
#include "esp_compiler.h"
#include "sdkconfig.h"

static esp_log_cache_enabled_t esp_log_cache_enabled = NULL;

void esp_log_util_set_cache_enabled_cb(esp_log_cache_enabled_t func)
{
    esp_log_cache_enabled = func;
}

bool esp_log_util_is_constrained(void)
{
    return (xPortInIsrContext()
#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
            || ((esp_log_cache_enabled) ? !esp_log_cache_enabled() : false)
#endif
            || unlikely(xTaskGetSchedulerState() != taskSCHEDULER_RUNNING));
}
