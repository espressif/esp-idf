/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "unity.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "test_utils.h"

typedef struct {
    int delay_us;
    int method;
    int result;
    SemaphoreHandle_t done;
} delay_test_arg_t;

static void test_delay_task(void *p)
{
    delay_test_arg_t *arg = (delay_test_arg_t *)p;
    vTaskDelay(1);
    uint64_t start = ref_clock_get();
    switch (arg->method) {
    case 0:
        esp_rom_delay_us(arg->delay_us);
        break;
    case 1:
        vTaskDelay(arg->delay_us / portTICK_PERIOD_MS / 1000);
        break;
    default:
        TEST_FAIL();
    }
    uint64_t stop = ref_clock_get();

    arg->result = (int)(stop - start);
    xSemaphoreGive(arg->done);
    vTaskDelete(NULL);
}

TEST_CASE("esp_rom_delay_us produces correct delay on CPUs", "[delay]")
{
    int delay_ms = 50;
    const delay_test_arg_t args = {
        .delay_us = delay_ms * 1000,
        .method = 0,
        .done = xSemaphoreCreateBinary()
    };
    ref_clock_init();
    xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void *)&args, 3, NULL, 0);
    TEST_ASSERT(xSemaphoreTake(args.done, delay_ms * 2 / portTICK_PERIOD_MS));
    TEST_ASSERT_INT32_WITHIN(1000, args.delay_us, args.result);

#if portNUM_PROCESSORS == 2
    xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void *)&args, 3, NULL, 1);
    TEST_ASSERT(xSemaphoreTake(args.done, delay_ms * 2 / portTICK_PERIOD_MS));
    TEST_ASSERT_INT32_WITHIN(1000, args.delay_us, args.result);
#endif

    ref_clock_deinit();
    vSemaphoreDelete(args.done);
}

TEST_CASE("vTaskDelay produces correct delay on CPUs", "[delay]")
{
    int delay_ms = 50;
    const delay_test_arg_t args = {
        .delay_us = delay_ms * 1000,
        .method = 1,
        .done = xSemaphoreCreateBinary()
    };
    ref_clock_init();
    xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void *)&args, 3, NULL, 0);
    TEST_ASSERT(xSemaphoreTake(args.done, delay_ms * 2 / portTICK_PERIOD_MS));
    TEST_ASSERT_INT32_WITHIN(1000, args.delay_us, args.result);

#if portNUM_PROCESSORS == 2
    xTaskCreatePinnedToCore(test_delay_task, "", 2048, (void *)&args, 3, NULL, 1);
    TEST_ASSERT(xSemaphoreTake(args.done, delay_ms * 2 / portTICK_PERIOD_MS));
    TEST_ASSERT_INT32_WITHIN(1000, args.delay_us, args.result);
#endif

    ref_clock_deinit();
    vSemaphoreDelete(args.done);
}
