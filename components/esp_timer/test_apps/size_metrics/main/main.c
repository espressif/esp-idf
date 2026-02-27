/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>

#include "esp_timer.h"
#include "sdkconfig.h"

#if CONFIG_SIZE_METRICS_FUNCTION_SET_MINIMAL
// Minimal function set - basic timer operations only
static void minimal_timer_callback(void* arg)
{
    // Minimal callback implementation
}

static void minimal_functions(void)
{
    esp_timer_handle_t timer;
    esp_timer_create_args_t timer_args = {
        .callback = minimal_timer_callback,
        .name = "minimal_timer"
    };
    esp_timer_create(&timer_args, &timer);
    esp_timer_start_once(timer, 1000);
    esp_timer_stop(timer);
    esp_timer_delete(timer);
}
#endif // CONFIG_SIZE_METRICS_FUNCTION_SET_MINIMAL

#if CONFIG_SIZE_METRICS_FUNCTION_SET_FULL
// Full function set - all timer features
static void full_timer_callback(void* arg)
{
    // Full callback implementation with more features
}

static void full_periodic_timer_callback(void* arg)
{
    // Periodic timer callback
}

static void full_functions(void)
{
    esp_timer_handle_t oneshot_timer;
    esp_timer_handle_t periodic_timer;

    // One-shot timer
    esp_timer_create_args_t oneshot_args = {
        .callback = full_timer_callback,
        .name = "oneshot_timer"
    };
    esp_timer_create(&oneshot_args, &oneshot_timer);
    esp_timer_start_once(oneshot_timer, 1000);

    // Periodic timer
    esp_timer_create_args_t periodic_args = {
        .callback = full_periodic_timer_callback,
        .name = "periodic_timer"
    };
    esp_timer_create(&periodic_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, 5000);

    // Timer operations
    esp_timer_stop(oneshot_timer);
    esp_timer_stop(periodic_timer);
    esp_timer_delete(oneshot_timer);
    esp_timer_delete(periodic_timer);
}
#endif // CONFIG_SIZE_METRICS_FUNCTION_SET_FULL

void app_main(void)
{
    printf("size_metrics app_main started\n");

#if CONFIG_SIZE_METRICS_FUNCTION_SET_MINIMAL
    minimal_functions();
#endif

#if CONFIG_SIZE_METRICS_FUNCTION_SET_FULL
    full_functions();
#endif
}
