/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file psram_stack_example_main.c
 * @brief Demonstrates CONFIG_FREERTOS_PLACE_TASK_STACKS_IN_EXT_RAM.
 *
 * When CONFIG_FREERTOS_PLACE_TASK_STACKS_IN_EXT_RAM=y, xTaskCreate() places
 * task stacks in PSRAM automatically.  TCBs always stay in internal DRAM.
 *
 * Flash operations (e.g. esp_partition_read) are safe from these tasks
 * because the esp_flash_dispatcher component intercepts flash APIs and runs
 * the actual operation on a dedicated internal-RAM task.
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_partition.h"
#include "esp_log.h"
#include "esp_flash_dispatcher.h"

#define NUM_WORKER_TASKS    3
#define WORKER_STACK_SIZE   4096
#define WORKER_PRIORITY     5

static const char *TAG = "psram_stack";

typedef struct {
    int          task_idx;
    TaskHandle_t notify_handle;
} worker_args_t;

static void worker_task(void *pvArg)
{
    worker_args_t *args = (worker_args_t *)pvArg;

    /* Probe a local variable to confirm this stack is in PSRAM. */
    volatile uint8_t stack_probe = 0;
    void *stack_ptr = (void *)&stack_probe;

    ESP_LOGI(TAG, "worker[%d]: stack @ %p  -> PSRAM", args->task_idx, stack_ptr);
    assert(esp_ptr_external_ram(stack_ptr));


    /* Perform a flash read from a PSRAM-stacked task.
     * esp_flash_dispatcher intercepts both spi_flash_mmap (used by
     * esp_partition_find_first on its first call) and esp_flash_read
     * (used by esp_partition_read), executing them on the internal-RAM
     * dispatcher task where they can safely run even when cache gets
     * disabled during the flash operation. */
    const esp_partition_t *part = esp_partition_find_first(
                                      ESP_PARTITION_TYPE_APP,
                                      ESP_PARTITION_SUBTYPE_ANY, NULL);
    if (part) {
        uint8_t buf[16];
        esp_err_t err = esp_partition_read(part, 0, buf, sizeof(buf));
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "worker[%d]: flash read OK (first bytes: %02x %02x %02x %02x ...)",
                     args->task_idx, buf[0], buf[1], buf[2], buf[3]);
        } else {
            ESP_LOGE(TAG, "worker[%d]: flash read failed: %s", args->task_idx, esp_err_to_name(err));
        }
    }

    xTaskNotifyGive(args->notify_handle);
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initialising flash dispatcher (routes flash ops off PSRAM-stacked tasks)");
    const esp_flash_dispatcher_config_t flash_disp_cfg = {
        .task_stack_size = 2048,
        .task_priority   = configMAX_PRIORITIES - 1,
        .task_core_id    = tskNO_AFFINITY,
        .queue_size      = 5,
    };
    ESP_ERROR_CHECK(esp_flash_dispatcher_init(&flash_disp_cfg));

    static worker_args_t args[NUM_WORKER_TASKS];
    TaskHandle_t self = xTaskGetCurrentTaskHandle();

    for (int i = 0; i < NUM_WORKER_TASKS; i++) {
        args[i].task_idx      = i;
        args[i].notify_handle = self;
        xTaskCreate(worker_task, "worker", WORKER_STACK_SIZE, &args[i],
                    WORKER_PRIORITY, NULL);
    }

    /* Wait for all workers to finish. */
    for (int i = 0; i < NUM_WORKER_TASKS; i++) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    }

    ESP_LOGI(TAG, "Example complete");
}
