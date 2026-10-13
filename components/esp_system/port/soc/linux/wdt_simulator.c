/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Watchdog timer simulation for the Linux/POSIX target.
 *
 * On real hardware the interrupt watchdog is backed by a dedicated timer
 * peripheral.  On the Linux simulator it is emulated with a helper pthread
 * that periodically checks whether the FreeRTOS tick hook has fed the
 * watchdog since the last period.  If not, the watchdog "fires" and
 * esp_restart is called, mirroring the behaviour on target.
 *
 * The task watchdog uses the real esp_timer-backed implementation
 * (task_wdt.c + task_wdt_impl_esp_timer.c) and is NOT simulated here.
 */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_freertos_hooks.h"
#include "esp_private/esp_int_wdt.h"
#include "esp_private/esp_sys_event_app_init.h"
#include "esp_private/freertos_linux_backtrace.h"

#if CONFIG_ESP_INT_WDT
static pthread_mutex_t s_int_wdt_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t s_int_wdt_thread;
static bool s_int_wdt_fed = false;

static void *int_wdt_runner(void *arg)
{
    (void)arg;
    while (1) {
        /* sleep for the required amount of time */
        const int sleep_time_us = CONFIG_ESP_INT_WDT_TIMEOUT_MS * 1000;
        usleep(sleep_time_us);

        /* check that the tick hook toggled the variable */
        pthread_mutex_lock(&s_int_wdt_mutex);

        if (!s_int_wdt_fed) {
            printf("\n----- interrupt watchdog reset triggered ----\n");
            freertos_linux_print_backtrace();
            esp_restart();
        }

        /* reset the variable value */
        s_int_wdt_fed = false;

        pthread_mutex_unlock(&s_int_wdt_mutex);
    }
    return NULL;
}

static void interrupt_wdt_hook(void)
{
    pthread_mutex_lock(&s_int_wdt_mutex);

    /* toggle the variable */
    s_int_wdt_fed = true;

    pthread_mutex_unlock(&s_int_wdt_mutex);
}
#endif // CONFIG_ESP_INT_WDT

void esp_int_wdt_init(void)
{
#if CONFIG_ESP_INT_WDT
    /* register the interrupt_wdt_hook to be called by the implementation
     * of esp_vApplicationTickHook in esp_system */
    if (esp_register_freertos_tick_hook_for_cpu(interrupt_wdt_hook, 0) != ESP_OK) {
        abort();
    }

    if (pthread_create(&s_int_wdt_thread, NULL, int_wdt_runner, NULL) != 0) {
        abort();
    }
    pthread_detach(s_int_wdt_thread);
#endif // CONFIG_ESP_INT_WDT
}

void esp_int_wdt_cpu_init(void)
{
    /* Per-CPU enable is a no-op on the single-core Linux simulator. */
}

ESP_PRE_SCHEDULER_HANDLER_REGISTER(init_int_wdt, 100)
{
    (void)user_arg;
    (void)ctx;
    esp_int_wdt_init();
    return ESP_OK;
}

ESP_PRE_SCHEDULER_HANDLER_REGISTER_PER_CPU(init_int_wdt_cpu, 110)
{
    (void)user_arg;
    (void)ctx;
    esp_int_wdt_cpu_init();
    return ESP_OK;
}
