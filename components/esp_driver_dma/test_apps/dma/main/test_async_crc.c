/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/param.h>
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_async_crc.h"
#include "esp_crc.h"

#define TEST_ASYNC_CRC_BENCH_COUNTS 16

typedef struct {
    uint32_t init_value;
    uint32_t crc_bit_width;
    uint32_t poly_hex;
    bool reverse_data_mask;
    bool reverse_result;
    uint32_t final_xor;
    uint32_t expected_result;
} test_crc_case_t;

static void test_async_crc_various_poly(async_crc_handle_t driver)
{
    static const char test_input_string[] __attribute__((aligned(16))) = "GDMACRC Share::Connect::Innovate";
    // CRC online: https://www.lddgo.net/en/encrypt/crc
    static test_crc_case_t crc_test_cases[] = {
        {
            .crc_bit_width = 8,
            .init_value = 0x00,
            .poly_hex = 0x07,
            .expected_result = 0xB8,
        },
        {
            .crc_bit_width = 8,
            .init_value = 0x10,
            .poly_hex = 0x23,
            .reverse_data_mask = true,
            .reverse_result = true,
            .final_xor = 0x1F,
            .expected_result = 0xB7,
        },
        {
            .crc_bit_width = 16,
            .init_value = 0xFFFF,
            .poly_hex = 0x1021,
            .expected_result = 0xA9B2,
        },
        {
            .crc_bit_width = 16,
            .init_value = 0x1234,
            .poly_hex = 0x8005,
            .reverse_data_mask = true,
            .reverse_result = true,
            .final_xor = 0xABCD,
            .expected_result = 0x9C6B,
        }
    };
    uint32_t result = 0;
    for (int i = 0; i < sizeof(crc_test_cases) / sizeof(crc_test_cases[0]); i++) {
        async_crc_params_t params = {
            .width = crc_test_cases[i].crc_bit_width,
            .polynomial = crc_test_cases[i].poly_hex,
            .init_value = crc_test_cases[i].init_value,
            .final_xor_value = crc_test_cases[i].final_xor,
            .reverse_input = crc_test_cases[i].reverse_data_mask,
            .reverse_output = crc_test_cases[i].reverse_result,
        };

        TEST_ESP_OK(esp_crc_calc_blocking(driver, test_input_string, strlen(test_input_string), &params, -1, &result));
        printf("CRC Result: 0x%"PRIx32", Expected: 0x%"PRIx32"\r\n", result, crc_test_cases[i].expected_result);
        TEST_ASSERT_EQUAL(crc_test_cases[i].expected_result, result);
    }
}

TEST_CASE("async_crc calculation with various CRC polynomials", "[async_crc]")
{
    async_crc_config_t config = {
        .backlog = 1,
        .dma_burst_size = 16,
    };
    // also test multiple instances
    async_crc_handle_t driver1 = NULL;
    async_crc_handle_t driver2 = NULL;
#if SOC_HAS(AHB_GDMA)
    printf("Testing async CRC calculation with various polynomials by AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_crc_install_gdma_ahb(&config, &driver1));
    TEST_ESP_OK(esp_async_crc_install_gdma_ahb(&config, &driver2));
    test_async_crc_various_poly(driver1);
    test_async_crc_various_poly(driver2);
    TEST_ESP_OK(esp_async_crc_uninstall(driver1));
    TEST_ESP_OK(esp_async_crc_uninstall(driver2));
#endif

#if SOC_HAS(AXI_GDMA)
    printf("Testing async CRC calculation with various polynomials by AXI GDMA\r\n");
    TEST_ESP_OK(esp_async_crc_install_gdma_axi(&config, &driver1));
    TEST_ESP_OK(esp_async_crc_install_gdma_axi(&config, &driver2));
    test_async_crc_various_poly(driver1);
    test_async_crc_various_poly(driver2);
    TEST_ESP_OK(esp_async_crc_uninstall(driver1));
    TEST_ESP_OK(esp_async_crc_uninstall(driver2));
#endif
}

typedef struct {
    uint32_t crc_result;
    SemaphoreHandle_t sem;
} crc_async_user_context_t;

static bool test_async_crc_result_cb(async_crc_handle_t crc_hdl, async_crc_event_data_t *edata, void *cb_args)
{
    crc_async_user_context_t* user = (crc_async_user_context_t*)cb_args;
    BaseType_t high_task_wakeup = pdFALSE;
    user->crc_result = edata->crc_result;
    xSemaphoreGiveFromISR(user->sem, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static void test_async_crc_calc_with_callback(async_crc_handle_t driver)
{
    static const char test_input_string[] __attribute__((aligned(16))) = "GDMACRC Share::Connect::Innovate";
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    crc_async_user_context_t user_ctx = {
        .sem = sem,
    };

    async_crc_params_t params = {
        .width = 16,
        .polynomial = 0x2025,
        .init_value = 0x00,
        .final_xor_value = 0x00,
    };

    TEST_ESP_OK(esp_async_crc_calc(driver, test_input_string, strlen(test_input_string), &params, test_async_crc_result_cb, &user_ctx));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(100)));
    printf("CRC Result: 0x%"PRIx32", Expected: 0x9D1B\r\n", user_ctx.crc_result);
    TEST_ASSERT_EQUAL(0x9D1B, user_ctx.crc_result);
    vSemaphoreDelete(sem);
}

TEST_CASE("async_crc calculation with callback", "[async_crc]")
{
    async_crc_config_t config = {
        .backlog = 1,
        .dma_burst_size = 16,
    };
    async_crc_handle_t driver = NULL;
#if SOC_HAS(AHB_GDMA)
    printf("Testing async CRC calculation with callback by AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_crc_install_gdma_ahb(&config, &driver));
    test_async_crc_calc_with_callback(driver);
    TEST_ESP_OK(esp_async_crc_uninstall(driver));
#endif

#if SOC_HAS(AXI_GDMA)
    printf("Testing async CRC calculation with callback by AXI GDMA\r\n");
    TEST_ESP_OK(esp_async_crc_install_gdma_axi(&config, &driver));
    test_async_crc_calc_with_callback(driver);
    TEST_ESP_OK(esp_async_crc_uninstall(driver));
#endif
}

static void test_async_crc_multiple_requests(async_crc_handle_t driver)
{
    static const char test_input_string[] __attribute__((aligned(16))) = "GDMACRC Share::Connect::Innovate";
    SemaphoreHandle_t sem = xSemaphoreCreateCounting(TEST_ASYNC_CRC_BENCH_COUNTS, 0);
    crc_async_user_context_t user_ctx = {
        .sem = sem,
    };
    async_crc_params_t params = {
        .width = 32,
        .polynomial = 0x04C11DB7,
        .init_value = 0xFFFFFFFF,
        .final_xor_value = 0x00,
    };

    for (int i = 0; i < TEST_ASYNC_CRC_BENCH_COUNTS; i++) {
        TEST_ESP_OK(esp_async_crc_calc(driver, test_input_string, strlen(test_input_string), &params, test_async_crc_result_cb, &user_ctx));
    }

    for (int i = 0; i < TEST_ASYNC_CRC_BENCH_COUNTS; i++) {
        TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(sem, pdMS_TO_TICKS(100)));
    }
    printf("CRC Result of request 0x%"PRIx32"\r\n", user_ctx.crc_result);
    TEST_ASSERT_EQUAL(0x692F6C7E, user_ctx.crc_result);
    vSemaphoreDelete(sem);
};

TEST_CASE("async_crc multiple requests", "[async_crc]")
{
    async_crc_config_t config = {
        .backlog = TEST_ASYNC_CRC_BENCH_COUNTS,
        .dma_burst_size = 32,
    };
    async_crc_handle_t driver = NULL;
#if SOC_HAS(AHB_GDMA)
    printf("Testing async CRC performance benchmark by AHB GDMA\r\n");
    TEST_ESP_OK(esp_async_crc_install_gdma_ahb(&config, &driver));
    test_async_crc_multiple_requests(driver);
    TEST_ESP_OK(esp_async_crc_uninstall(driver));
#endif
}
