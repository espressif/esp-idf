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
#include "unity.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"
#include "esp_partition.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_flash_dispatcher.h"
#include "esp_private/esp_clk.h"
#include "sdkconfig.h"

#define WORKER_STACK_SIZE   4096
#define WORKER_PRIORITY     5
#define UNITY_STACK_SIZE    8192
#define NUM_CONCURRENT      5
#define CONCURRENT_READS    10
#define MHZ                 1000000

static const char *TAG = "psram_stack_test";
#if CONFIG_PM_ENABLE && CONFIG_FREERTOS_USE_TICKLESS_IDLE && CONFIG_PM_LIGHT_SLEEP_CALLBACKS
static volatile uint32_t s_light_sleep_exit_count;
static volatile int64_t s_last_light_sleep_us;
#endif

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

static esp_err_t IRAM_ATTR light_sleep_exit_cb(int64_t slept_us, void *arg)
{
    (void)arg;

    if (slept_us > 0) {
        s_last_light_sleep_us = slept_us;
        s_light_sleep_exit_count++;
    }

    return ESP_OK;
}

TEST_CASE("PSRAM stack: task resumes correctly after tickless-idle light sleep", "[psram_stack][light_sleep]")
{
    ASSERT_STACK_IN_PSRAM();

    s_light_sleep_exit_count = 0;
    s_last_light_sleep_us = 0;

    esp_pm_sleep_cbs_register_config_t sleep_cbs = {
        .exit_cb = light_sleep_exit_cb,
    };
    TEST_ESP_OK(esp_pm_light_sleep_unregister_cbs(&sleep_cbs));
    TEST_ESP_OK(esp_pm_light_sleep_register_cbs(&sleep_cbs));

    printf("Waiting for tickless-idle light sleep...\n");
    vTaskDelay(pdMS_TO_TICKS(100));

    const uint32_t exit_count = s_light_sleep_exit_count;
    const int64_t last_slept_us = s_last_light_sleep_us;
    TEST_ESP_OK(esp_pm_light_sleep_unregister_cbs(&sleep_cbs));

    ESP_LOGI(TAG, "Auto light sleep exit callbacks: %" PRIu32 ", last slept: %" PRId64 " us",
             exit_count, last_slept_us);
    TEST_ASSERT_GREATER_THAN_UINT32(0, exit_count);
    TEST_ASSERT_GREATER_THAN_UINT32(0, last_slept_us);

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
