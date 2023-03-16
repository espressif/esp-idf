/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "unity.h"
#include "esp_timer.h"
#include "esp_private/mspi_timing_tuning.h"


static void sorted_array_insert(uint32_t *array, uint32_t *size, uint32_t item)
{
    uint32_t pos;
    for (pos = *size; pos > 0; pos--) {
        if (array[pos - 1] <= item) {
            break;
        }
        array[pos] = array[pos - 1];
    }
    array[pos] = item;
    (*size)++;
}

#define TEST_TIME_CNT           10
#define TEST_TIME_LIMIT_US      10
TEST_CASE("MSPI: Test mspi timing turning time cost", "[mspi]")
{
    uint64_t start, end;
    uint32_t cost, index_1 = 0, index_2 = 0;
    uint32_t slow_down_time[TEST_TIME_CNT], speed_up_time[TEST_TIME_CNT];

    printf("\nFunc call `mspi_timing_change_speed_mode_cache_safe` should spend time less than %d us\n", TEST_TIME_LIMIT_US);
    for (uint8_t i = 0; i < TEST_TIME_CNT; i++) {
        start = esp_timer_get_time();
        mspi_timing_change_speed_mode_cache_safe(true);
        end = esp_timer_get_time();
        cost = end - start;
        sorted_array_insert(slow_down_time, &index_1, cost);
        printf("mspi psram slow down cost %ld\t", cost);

        start = esp_timer_get_time();
        mspi_timing_change_speed_mode_cache_safe(false);
        end = esp_timer_get_time();
        cost = end - start;
        sorted_array_insert(speed_up_time, &index_2, cost);
        printf("mspi psram speed up cost %ld\n", cost);
    }

    TEST_ASSERT_LESS_THAN_UINT32(TEST_TIME_LIMIT_US, slow_down_time[TEST_TIME_CNT / 2]);
    TEST_ASSERT_LESS_THAN_UINT32(TEST_TIME_LIMIT_US, speed_up_time[TEST_TIME_CNT / 2]);
}
