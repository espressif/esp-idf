/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_debug_helpers.h"
#include "esp_openthread_debug.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if CONFIG_OPENTHREAD_TASK_BLOCK_MONITOR

#include "esp_private/eh_frame_parser.h"
#include "esp_attr.h"
#include "esp_private/esp_cpu_internal.h"
#include "esp_private/panic_internal.h"
#include "esp_private/freertos_debug.h"

static const char *TAG = "OT_DEBUG";
static TaskHandle_t s_ot_task_block_monitor_task = NULL;
static atomic_llong s_ot_task_block_monitor_latest_time = 0;
static atomic_bool s_ot_task_block_monitor_active = false;
static const int64_t s_ot_task_block_monitor_timeout = CONFIG_OPENTHREAD_TASK_BLOCK_MONITOR_TIMEOUT * 1000000;
static bool is_backtrace_printed = false;

#if CONFIG_IDF_TARGET_ARCH_RISCV
static esp_err_t IRAM_ATTR esp_task_backtrace_print(TaskHandle_t target_task)
{
    TaskSnapshot_t snapshot = {};
    void *frame = NULL;

    vTaskGetSnapshot(target_task, &snapshot);

    frame = snapshot.pxTopOfStack;
    char *name = pcTaskGetName(target_task);

    ESP_LOGI(TAG, "Target Task Backtrace: %s", name ? name : "No Name");

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
    esp_eh_frame_print_backtrace(frame);
#else
    esp_cpu_frame_t backtrace_frame = {};
    const int current_core = xPortGetCoreID();
    memset(&backtrace_frame, 0, sizeof(esp_cpu_frame_t));
    memcpy(&backtrace_frame, frame, sizeof(esp_cpu_frame_t));

    panic_prepare_frame_from_ctx(&backtrace_frame);
    panic_print_registers(&backtrace_frame, current_core);

    esp_rom_printf("\r\n");
    esp_rom_printf("Please enable CONFIG_ESP_SYSTEM_USE_EH_FRAME option to have a full backtrace.\r\n");
#endif

    return ESP_OK;
}
#endif

static void ot_debug_monitor_task(void *arg)
{
    (void)arg;

    while (true) {
        if (atomic_load(&s_ot_task_block_monitor_active)) {
            int64_t elapsed = esp_timer_get_time() - atomic_load(&s_ot_task_block_monitor_latest_time);
            if (elapsed > s_ot_task_block_monitor_timeout && !is_backtrace_printed) {
                is_backtrace_printed = true;
                ESP_LOGW(TAG,
                         "OpenThread mainloop blocked for more than %d seconds, printing all tasks backtrace",
                         CONFIG_OPENTHREAD_TASK_BLOCK_MONITOR_TIMEOUT);
#if CONFIG_IDF_TARGET_ARCH_XTENSA
                esp_backtrace_print_all_tasks(10);
#elif CONFIG_IDF_TARGET_ARCH_RISCV
                TaskHandle_t handle = xTaskGetHandle(CONFIG_OPENTHREAD_TASK_NAME);
                if (handle) {
                    esp_task_backtrace_print(handle);
                }
#endif
            } else if (elapsed <= s_ot_task_block_monitor_timeout) {
                is_backtrace_printed = false;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

esp_err_t esp_openthread_task_block_monitor_create(void)
{
    ESP_RETURN_ON_FALSE(s_ot_task_block_monitor_task == NULL, ESP_OK, TAG, "Task block monitor already created");

    BaseType_t ret = xTaskCreate(ot_debug_monitor_task, "ot_task_monitor", 3072, NULL, CONFIG_OPENTHREAD_TASK_PRIORITY, &s_ot_task_block_monitor_task);
    ESP_RETURN_ON_FALSE(ret == pdPASS, ESP_FAIL, TAG, "Failed to create OpenThread task block monitor");

    ESP_LOGI(TAG, "OpenThread task block monitor created");
    esp_openthread_task_block_monitor_set(true);
    return ESP_OK;
}

esp_err_t esp_openthread_task_block_monitor_delete(void)
{
    ESP_RETURN_ON_FALSE(s_ot_task_block_monitor_task != NULL, ESP_OK, TAG, "Task block monitor not created");

    esp_openthread_task_block_monitor_set(false);
    vTaskDelete(s_ot_task_block_monitor_task);
    s_ot_task_block_monitor_task = NULL;

    ESP_LOGI(TAG, "OpenThread task block monitor deleted");
    return ESP_OK;
}

void esp_openthread_task_block_monitor_set(bool enable)
{
    if (enable) {
        atomic_store(&s_ot_task_block_monitor_latest_time, esp_timer_get_time());
        atomic_store(&s_ot_task_block_monitor_active, true);
    } else {
        atomic_store(&s_ot_task_block_monitor_active, false);
    }
}

#endif // CONFIG_OPENTHREAD_TASK_BLOCK_MONITOR
