/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"

static const char * TAG = "log_test";

static int calc_time_of_logging(int iterations)
{
    int diff;
    int64_t start, end;
    int attempts = 2;
    while (attempts--) {
        start = esp_timer_get_time();
        for (int i = 0; i < iterations; i++) {
            ESP_LOGI(TAG, "some test data, %d, %d, %d", i, iterations - i, 12);
        }
        end = esp_timer_get_time();
    }
    diff = (int)(end - start);
    printf("%d iterations took %d usec (%.02f usec per invocation)\n",
           iterations, diff, (float)diff / iterations);
    return diff;
}

TEST_CASE("test master logging level performance", "[log]")
{
    const int ITERATIONS = 1000;
    ESP_LOGI(TAG, "Start");

#ifdef CONFIG_LOG_MASTER_LEVEL
    esp_log_set_level_master(ESP_LOG_NONE);
    TEST_ASSERT_INT_WITHIN(100, 150, calc_time_of_logging(ITERATIONS));
#else
    esp_log_level_set("*", ESP_LOG_NONE);
    TEST_ASSERT_INT_WITHIN(5, 11, calc_time_of_logging(ITERATIONS) / ITERATIONS);
#endif

    esp_log_level_set("*", ESP_LOG_NONE);
#ifdef CONFIG_LOG_MASTER_LEVEL
    esp_log_set_level_master(ESP_LOG_DEBUG);
#endif
    TEST_ASSERT_INT_WITHIN(5, 11, calc_time_of_logging(ITERATIONS) / ITERATIONS);

    esp_log_level_set("*", ESP_LOG_INFO);
    ESP_LOGI(TAG, "End");
}
