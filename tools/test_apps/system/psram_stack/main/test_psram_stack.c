/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * System-level tests for CONFIG_FREERTOS_PLACE_TASK_STACKS_IN_EXT_RAM.
 *
 * Unity is run from a task created with xTaskCreate(), so every TEST_CASE
 * body executes directly on a PSRAM-backed stack.  No worker-task indirection
 * is needed except for the concurrency test.
 */

#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/idf_additions.h"
#include "unity.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_partition.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_flash_dispatcher.h"
#include "sdkconfig.h"

#define WORKER_STACK_SIZE   4096
#define WORKER_PRIORITY     5
#define UNITY_STACK_SIZE    8192
#define NUM_CONCURRENT      5
#define CONCURRENT_READS    10

static const char *TAG = "psram_stack_test";

/* Confirm the calling task's stack is in PSRAM before each test body runs. */
#define ASSERT_STACK_IN_PSRAM() \
    do { volatile uint8_t _probe = 0; \
         TEST_ASSERT_TRUE_MESSAGE(esp_ptr_external_ram((void *)&_probe), \
                                  "test task stack is not in PSRAM"); } while (0)


TEST_CASE("PSRAM stack: flash read via dispatcher succeeds", "[psram_stack]")
{
    ASSERT_STACK_IN_PSRAM();
    const esp_partition_t *part = esp_partition_find_first(
                                      ESP_PARTITION_TYPE_APP,
                                      ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(part);
    uint8_t buf[16];
    TEST_ASSERT_EQUAL(ESP_OK, esp_partition_read(part, 0, buf, sizeof(buf)));
}

TEST_CASE("PSRAM stack: NVS read/write via dispatcher succeeds", "[psram_stack]")
{
    ASSERT_STACK_IN_PSRAM();
    nvs_handle_t handle;
    TEST_ASSERT_EQUAL(ESP_OK, nvs_open("psram_test", NVS_READWRITE, &handle));
    TEST_ASSERT_EQUAL(ESP_OK, nvs_set_u32(handle, "key", 0xDEADBEEFUL));
    TEST_ASSERT_EQUAL(ESP_OK, nvs_commit(handle));
    uint32_t val = 0;
    TEST_ASSERT_EQUAL(ESP_OK, nvs_get_u32(handle, "key", &val));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEFUL, val);
    nvs_close(handle);
}

TEST_CASE("PSRAM stack: deep sleep rejected from PSRAM-stacked task", "[psram_stack]")
{
    ASSERT_STACK_IN_PSRAM();
    esp_sleep_enable_timer_wakeup(1000000ULL);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_ALLOWED, esp_deep_sleep_try_to_start());
}

#if CONFIG_PM_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_PM_LIGHT_SLEEP_CALLBACKS

typedef struct {
    TaskHandle_t parent;
    esp_err_t sleep_result;
    bool stack_in_internal_ram;
} light_sleep_result_t;

static void light_sleep_from_internal_stack(void *arg)
{
    light_sleep_result_t *result = arg;
    volatile uint8_t stack_probe = 0;

    result->stack_in_internal_ram = esp_ptr_in_dram((const void *)&stack_probe);
    esp_sleep_enable_timer_wakeup(100000ULL);
    result->sleep_result = esp_light_sleep_start();

    xTaskNotifyGive(result->parent);
    vTaskSuspend(NULL);
}

TEST_CASE("PSRAM stack: blocked task resumes correctly after light sleep", "[psram_stack][light_sleep]")
{
    ASSERT_STACK_IN_PSRAM();

    light_sleep_result_t result = {
        .parent = xTaskGetCurrentTaskHandle(),
        .sleep_result = ESP_FAIL,
    };
    TaskHandle_t sleep_task = NULL;
    TEST_ASSERT_EQUAL(
        pdPASS,
        xTaskCreateWithCaps(light_sleep_from_internal_stack, "light_sleep", WORKER_STACK_SIZE,
                            &result, WORKER_PRIORITY, &sleep_task, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    TEST_ASSERT_NOT_NULL(sleep_task);

    TEST_ASSERT_EQUAL_UINT32(1, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)));
    vTaskDeleteWithCaps(sleep_task);

    TEST_ASSERT_TRUE_MESSAGE(result.stack_in_internal_ram, "light sleep task stack is not in internal RAM");
    TEST_ASSERT_EQUAL(ESP_OK, result.sleep_result);
    ASSERT_STACK_IN_PSRAM();
}

#endif /* CONFIG_PM_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_PM_LIGHT_SLEEP_CALLBACKS */

/* ---------- Test 6: Concurrent flash reads from multiple PSRAM-stacked tasks ---------- */

typedef struct {
    esp_err_t    err;
    int          reads_done;
    TaskHandle_t parent;
} concurrent_result_t;

static void task_concurrent_flash(void *arg)
{
    concurrent_result_t *r = arg;
    const esp_partition_t *part = esp_partition_find_first(
                                      ESP_PARTITION_TYPE_APP,
                                      ESP_PARTITION_SUBTYPE_ANY, NULL);
    if (!part) {
        r->err = ESP_ERR_NOT_FOUND;
        xTaskNotifyGive(r->parent);
        vTaskDelete(NULL);
        return;
    }
    r->err = ESP_OK;
    for (int i = 0; i < CONCURRENT_READS; i++) {
        uint8_t buf[16];
        r->err = esp_partition_read(part, 0, buf, sizeof(buf));
        if (r->err != ESP_OK) {
            break;
        }
        r->reads_done++;
    }
    xTaskNotifyGive(r->parent);
    vTaskDelete(NULL);
}

TEST_CASE("PSRAM stack: concurrent flash reads from multiple PSRAM-stacked tasks", "[psram_stack]")
{
    ASSERT_STACK_IN_PSRAM();
    static concurrent_result_t results[NUM_CONCURRENT];
    TaskHandle_t self = xTaskGetCurrentTaskHandle();

    for (int i = 0; i < NUM_CONCURRENT; i++) {
        results[i] = (concurrent_result_t){ .parent = self };
        TEST_ASSERT_EQUAL(pdPASS, xTaskCreate(task_concurrent_flash, "cflash",
                                               WORKER_STACK_SIZE, &results[i], WORKER_PRIORITY, NULL));
    }
    for (int i = 0; i < NUM_CONCURRENT; i++) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    }
    for (int i = 0; i < NUM_CONCURRENT; i++) {
        ESP_LOGI(TAG, "worker[%d]: %d reads, err=%s",
                 i, results[i].reads_done, esp_err_to_name(results[i].err));
        TEST_ASSERT_EQUAL(ESP_OK, results[i].err);
        TEST_ASSERT_EQUAL(CONCURRENT_READS, results[i].reads_done);
    }
}


static void unity_task(void *arg)
{
    unity_run_menu();
    vTaskDelete(NULL);
}

void app_main(void)
{
    const esp_flash_dispatcher_config_t disp_cfg = {
        .task_stack_size = 4096,
        .task_priority   = configMAX_PRIORITIES - 1,
        .task_core_id    = tskNO_AFFINITY,
        .queue_size      = 8,
    };
    ESP_ERROR_CHECK(esp_flash_dispatcher_init(&disp_cfg));

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    /* Run Unity from a PSRAM-stacked task so all TEST_CASE bodies execute
     * with a PSRAM stack without needing per-test worker task indirection. */
    xTaskCreate(unity_task, "unity", UNITY_STACK_SIZE, NULL, WORKER_PRIORITY, NULL);
}
