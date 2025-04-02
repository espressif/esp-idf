/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "esp_tee.h"
#include "secure_service_num.h"
#include "esp_tee_test.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

static void test_op(int sec_srv_num, uint32_t a, uint32_t b)
{
    esp_cpu_priv_mode_t world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Current world is not NS");

    uint32_t lval = 0;
    switch (sec_srv_num) {
    case SS_ESP_TEE_SERVICE_ADD:
        lval = (a + b);
        break;
    case SS_ESP_TEE_SERVICE_SUB:
        lval = (a - b);
        break;
    case SS_ESP_TEE_SERVICE_MUL:
        lval = (a * b);
        break;
    case SS_ESP_TEE_SERVICE_DIV:
        lval = (a / b);
        break;
    default:
        lval = UINT32_MAX;
        break;
    }

    uint32_t val = esp_tee_service_call(3, sec_srv_num, a, b);
    TEST_ASSERT_EQUAL_UINT32(lval, val);

    world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Sample app world switch failed");
}

TEST_CASE("Test single calls to sample app (basic services)", "[basic]")
{
    const uint32_t a = 200, b = 100;
    test_op(SS_ESP_TEE_SERVICE_ADD, a, b);
    test_op(SS_ESP_TEE_SERVICE_SUB, a, b);
    test_op(SS_ESP_TEE_SERVICE_MUL, a, b);
    test_op(SS_ESP_TEE_SERVICE_DIV, a, b);
}

TEST_CASE("Test multiple calls to sample app (basic services)", "[basic]")
{
    esp_cpu_priv_mode_t world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Current world is not NS");

    for (int i = 0; i < 1024; i++) {
        uint32_t val = esp_tee_service_call(3, SS_ESP_TEE_SERVICE_ADD, i, i + 1);
        TEST_ASSERT_EQUAL_UINT32((2 * i + 1), val);
        world = esp_cpu_get_curr_privilege_level();
        TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Sample app world switch failed");
    }
}

TEST_CASE("Custom secure service call", "[basic]")
{
    int res = -1;
    dummy_secure_service(1, 2, 3, 4, 5, 6, 7, 8, &res);
    TEST_ASSERT_EQUAL_UINT32(36, res);
}

void test_task(void *pvParameters)
{
    for (int i = 0; i < 8; i++) {
        esp_rom_printf("[mode: %d] test_task - %d\n", esp_cpu_get_curr_privilege_level(), i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

TEST_CASE("Task switching during secure service calls", "[basic]")
{
    esp_cpu_priv_mode_t world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Current world is not NS");

    xTaskCreate(test_task, "test_task", 4096, NULL, CONFIG_UNITY_FREERTOS_PRIORITY + 3, NULL);

    const uint32_t a = 100, b = 200, iter = 8;
    TEST_ASSERT_EQUAL_UINT32(a + b * iter, esp_tee_service_call(4, SS_ADD_IN_LOOP, a, b, iter));

    world = esp_cpu_get_curr_privilege_level();
    TEST_ASSERT_MESSAGE((world == ESP_CPU_NS_MODE), "Current world is not NS");
}

TEST_CASE("Test TEE Heap: Malloc-write-free cycles", "[heap]")
{
    TEST_ASSERT_EQUAL(0, esp_tee_service_call(1, SS_ESP_TEE_TEST_HEAP_MALLOC_WRITE_FREE));
}
