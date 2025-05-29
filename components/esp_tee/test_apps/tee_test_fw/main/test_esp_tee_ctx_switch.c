/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_cpu.h"
#include "esp_tee.h"
#include "esp_tee_test.h"
#include "secure_service_num.h"

#include "unity.h"
#include "sdkconfig.h"

#define TEST_TASK_1_DONE_BIT (BIT0)
#define TEST_TASK_2_DONE_BIT (BIT1)

typedef struct {
    uint32_t id;
    uint32_t *val;
    EventBits_t done_bit;
} test_task_args_t;

static EventGroupHandle_t test_task_eg;

static void test_op(int sec_srv_num, uint32_t a, uint32_t b)
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");

    uint32_t lval = 0;
    switch (sec_srv_num) {
    case SS_ESP_TEE_TEST_SERVICE_ADD:
        lval = (a + b);
        break;
    case SS_ESP_TEE_TEST_SERVICE_SUB:
        lval = (a - b);
        break;
    case SS_ESP_TEE_TEST_SERVICE_MUL:
        lval = (a * b);
        break;
    case SS_ESP_TEE_TEST_SERVICE_DIV:
        lval = (a / b);
        break;
    default:
        lval = UINT32_MAX;
        break;
    }

    uint32_t val = esp_tee_service_call(3, sec_srv_num, a, b);
    TEST_ASSERT_EQUAL_UINT32(lval, val);

    mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");
}

TEST_CASE("Test single calls to sample app (basic services)", "[basic]")
{
    const uint32_t a = 200, b = 100;
    test_op(SS_ESP_TEE_TEST_SERVICE_ADD, a, b);
    test_op(SS_ESP_TEE_TEST_SERVICE_SUB, a, b);
    test_op(SS_ESP_TEE_TEST_SERVICE_MUL, a, b);
    test_op(SS_ESP_TEE_TEST_SERVICE_DIV, a, b);
}

TEST_CASE("Test multiple calls to sample app (basic services)", "[basic]")
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");

    for (int i = 0; i < 1024; i++) {
        uint32_t val = esp_tee_service_call(3, SS_ESP_TEE_TEST_SERVICE_ADD, i, i + 1);
        TEST_ASSERT_EQUAL_UINT32((2 * i + 1), val);
        mode = esp_cpu_get_curr_privilege_level();
        TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");
    }
}

TEST_CASE("Test custom secure service call", "[basic]")
{
    int res = -1;
    esp_tee_test_dummy_sec_srv(1, 2, 3, 4, 5, 6, 7, 8, &res);
    TEST_ASSERT_EQUAL_UINT32(36, res);
}

static void test_task(void *pvParameters)
{
    test_task_args_t *args = (test_task_args_t *)pvParameters;
    uint32_t *val = args->val;
    uint32_t id = args->id;

    uint32_t prev_val = *val;
    while (*val <= ESP_TEE_TEST_INTR_ITER) {
        uint32_t curr_val = *val;
        if (curr_val != prev_val) {
            esp_rom_printf("[mode: %d] test_task - %d | val - %d\n", esp_cpu_get_curr_privilege_level(), id, curr_val);
            prev_val = curr_val;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    xEventGroupSetBits(test_task_eg, args->done_bit);
    vTaskDelete(NULL);
}

TEST_CASE("Test task switching during secure service calls", "[basic]")
{
    esp_cpu_priv_mode_t mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");

    test_task_eg = xEventGroupCreate();
    TEST_ASSERT_NOT_NULL(test_task_eg);

    uint32_t a = 0, b = 0;

    test_task_args_t task_args_1 = {
        .id = 1,
        .val = &a,
        .done_bit = TEST_TASK_1_DONE_BIT
    };

    test_task_args_t task_args_2 = {
        .id = 2,
        .val = &b,
        .done_bit = TEST_TASK_2_DONE_BIT
    };

    xTaskCreate(test_task, "test_task_1", 4096, (void *)&task_args_1, CONFIG_UNITY_FREERTOS_PRIORITY + 3, NULL);
    xTaskCreate(test_task, "test_task_2", 4096, (void *)&task_args_2, CONFIG_UNITY_FREERTOS_PRIORITY + 3, NULL);

    uint32_t val = esp_tee_service_call(3, SS_ESP_TEE_TEST_PRIV_MODE_SWITCH, &a, &b);
    TEST_ASSERT_EQUAL_UINT32(ESP_TEE_TEST_INTR_ITER * 2, val);

    EventBits_t bits = xEventGroupWaitBits(test_task_eg, TEST_TASK_1_DONE_BIT | TEST_TASK_2_DONE_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
    TEST_ASSERT_MESSAGE((bits & TEST_TASK_1_DONE_BIT), "Task 1 did not complete");
    TEST_ASSERT_MESSAGE((bits & TEST_TASK_2_DONE_BIT), "Task 2 did not complete");
    vEventGroupDelete(test_task_eg);

    mode = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((mode == ESP_CPU_NS_MODE), "Incorrect privilege mode!");
}

TEST_CASE("Test TEE Heap: Malloc-write-free cycles", "[heap]")
{
    TEST_ASSERT_EQUAL(0, esp_tee_service_call(1, SS_ESP_TEE_TEST_HEAP_MALLOC_WRITE_FREE));
}
