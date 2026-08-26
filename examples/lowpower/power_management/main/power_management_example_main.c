/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_pm.h"
#include "sdkconfig.h"

#define DEMO_PERIOD_MS        1000
#define NESTED_HOLD_MS        200
#define NESTED_ACTIVE_MS      (3 * NESTED_HOLD_MS)
#define NESTED_IDLE_MS        (DEMO_PERIOD_MS - NESTED_ACTIVE_MS)
#define PM_DUMP_INTERVAL_MS   10000

static void task_demo_nested_locks(void *arg)
{
    (void)arg;
    esp_pm_lock_handle_t lock_no_ls;
    esp_pm_lock_handle_t lock_apb;
    esp_pm_lock_handle_t lock_cpu;

    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "demo_no_ls", &lock_no_ls));
    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "demo_apb", &lock_apb));
    ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "demo_cpu", &lock_cpu));

    while (1) {
        ESP_ERROR_CHECK(esp_pm_lock_acquire(lock_no_ls));
        // CPU working at min_freq_mhz, but not enter Light-sleep mode to
        // maintain the ability to respond to interruptions
        vTaskDelay(pdMS_TO_TICKS(NESTED_HOLD_MS));
        ESP_ERROR_CHECK(esp_pm_lock_acquire(lock_apb));
        // CPU working at the frequency of the maximum APB frequency supported
        //  by the SOC, to keep peripherals working at a high frequency.
        vTaskDelay(pdMS_TO_TICKS(NESTED_HOLD_MS));
        ESP_ERROR_CHECK(esp_pm_lock_acquire(lock_cpu));
        // CPU working at max_freq_mhz, to achieve best performance.
        vTaskDelay(pdMS_TO_TICKS(NESTED_HOLD_MS));
        ESP_ERROR_CHECK(esp_pm_lock_release(lock_cpu));
        ESP_ERROR_CHECK(esp_pm_lock_release(lock_apb));
        ESP_ERROR_CHECK(esp_pm_lock_release(lock_no_ls));
        // If light_sleep_enable is true, the system will enter Light-sleep mode.
        // Otherwise, the CPU stays at min_freq_mhz in IDLE.
        vTaskDelay(pdMS_TO_TICKS(NESTED_IDLE_MS));
    }
}

/* Print PM config and lock/mode stats every 10s */
static void task_pm_dump(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(PM_DUMP_INTERVAL_MS));
    while (1) {
        esp_pm_config_t cfg;
        if (esp_pm_get_configuration(&cfg) == ESP_OK) {
            printf("\n--- PM: max_freq=%d min_freq=%d MHz, light_sleep=%d ---\n",
                   cfg.max_freq_mhz, cfg.min_freq_mhz, cfg.light_sleep_enable ? 1 : 0);
        }
        esp_pm_dump_locks(stdout);
        vTaskDelay(pdMS_TO_TICKS(PM_DUMP_INTERVAL_MS));
    }
}

void app_main(void)
{
    printf("\n=== Power Management Example (nested locks, single task) ===\n");
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_EXAMPLE_PM_MAX_FREQ,
        .min_freq_mhz = CONFIG_EXAMPLE_PM_MIN_FREQ,
        .light_sleep_enable = CONFIG_EXAMPLE_PM_LIGHT_SLEEP_ENABLE,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    printf("PM configured: max=%d min=%d MHz, light_sleep_enable=%d\n",
           pm_config.max_freq_mhz, pm_config.min_freq_mhz, pm_config.light_sleep_enable ? 1 : 0);

    xTaskCreate(task_demo_nested_locks, "demo_pm", 2048, NULL, 3, NULL);
    xTaskCreate(task_pm_dump,           "pm_dump", 2048, NULL, 0, NULL);
}
