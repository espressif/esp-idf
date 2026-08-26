/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_private/startup_internal.h"
#include "esp_timer_impl.h"
#include "sdkconfig.h"

esp_err_t esp_timer_early_init(void)
{
    esp_timer_impl_early_init();
#if CONFIG_ESP_TIME_FUNCS_USE_ESP_TIMER
    esp_timer_impl_init_system_time();
#endif
    return ESP_OK;
}

/*
 * This function starts a timer, which is used by esp_timer
 * to count time from the very start of the application.
 *
 * Another initialization function, esp_timer_init_nonos (which initializes ISR and task),
 * is called only if other code calls the esp_timer API.
 */
ESP_SYSTEM_INIT_FN(esp_timer_init_nonos, CORE, BIT(0), 101)
{
    return esp_timer_early_init();
}

void esp_timer_init_include_func(void)
{
    // Hook to force the linker to include this file
}
