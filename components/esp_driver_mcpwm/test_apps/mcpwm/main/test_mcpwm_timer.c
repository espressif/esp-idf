/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "esp_timer.h"
#include "esp_rom_sys.h"
#include "hal/mcpwm_ll.h"
#include "driver/mcpwm_timer.h"
#include "esp_private/mcpwm.h"
#include "test_mcpwm_utils.h"

TEST_CASE("mcpwm_timer_start_stop", "[mcpwm]")
{
    mcpwm_timer_config_t config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz
        .period_ticks = 400,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
    };
    const int num_timers = MCPWM_LL_GET(TIMERS_PER_GROUP) * MCPWM_LL_GET(GROUP_NUM);

    printf("create mcpwm timer instances\r\n");
    mcpwm_timer_handle_t timers[num_timers];
    for (int i = 0; i < MCPWM_LL_GET(GROUP_NUM); i++) {
        for (int j = 0; j < MCPWM_LL_GET(TIMERS_PER_GROUP); j++) {
            config.group_id = i;
            TEST_ESP_OK(mcpwm_new_timer(&config, &timers[i * MCPWM_LL_GET(TIMERS_PER_GROUP) + j]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_timer(&config, &timers[0]));
    }

    // can't do start/stop control before enable
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_timer_start_stop(timers[0], MCPWM_TIMER_START_NO_STOP));

    printf("enable timers\r\n");
    for (int i = 0; i < num_timers; i++) {
        TEST_ESP_OK(mcpwm_timer_enable(timers[i]));
    }

    printf("start timer and then stop when empty\r\n");
    for (int i = 0; i < num_timers; i++) {
        TEST_ESP_OK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_START_STOP_EMPTY));
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    check_mcpwm_timer_phase(timers, num_timers, 0, MCPWM_TIMER_DIRECTION_UP);

    printf("start timer and then stop when full\r\n");
    for (int i = 0; i < num_timers; i++) {
        TEST_ESP_OK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_START_STOP_FULL));
    }
    vTaskDelay(pdMS_TO_TICKS(10));
    check_mcpwm_timer_phase(timers, num_timers, config.period_ticks / 2, MCPWM_TIMER_DIRECTION_DOWN);

    printf("start freely and stop manually when full\r\n");
    for (int i = 0; i < num_timers; i++) {
        TEST_ESP_OK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_START_NO_STOP));
        vTaskDelay(pdMS_TO_TICKS(10));
        // stop at next counter full
        TEST_ESP_OK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_STOP_FULL));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    check_mcpwm_timer_phase(timers, num_timers, config.period_ticks / 2, MCPWM_TIMER_DIRECTION_DOWN);

    printf("start freely and stop manually when empty\r\n");
    for (int i = 0; i < num_timers; i++) {
        TEST_ESP_OK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_START_NO_STOP));
        vTaskDelay(pdMS_TO_TICKS(10));
        // stop at next counter empty
        TEST_ESP_OK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_STOP_EMPTY));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    check_mcpwm_timer_phase(timers, num_timers, 0, MCPWM_TIMER_DIRECTION_UP);

    // can't delete timer before disable
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_del_timer(timers[0]));

    printf("disable timers\r\n");
    for (int i = 0; i < num_timers; i++) {
        TEST_ESP_OK(mcpwm_timer_disable(timers[i]));
    }

    printf("delete timers\r\n");
    for (int i = 0; i < num_timers; i++) {
        TEST_ESP_OK(mcpwm_del_timer(timers[i]));
    }
}

TEST_CASE("mcpwm_timer_various_clk_src", "[mcpwm]")
{
    mcpwm_timer_clock_source_t clk_srcs[] = SOC_MCPWM_TIMER_CLKS;
    const int num_timers = MCPWM_LL_GET(TIMERS_PER_GROUP) * MCPWM_LL_GET(GROUP_NUM);
    const uint32_t resolution_test_duration_us = 5000;

    for (size_t clk_src_idx = 0; clk_src_idx < sizeof(clk_srcs) / sizeof(clk_srcs[0]); clk_src_idx++) {
        mcpwm_timer_clock_source_t clk_src = clk_srcs[clk_src_idx];
        const uint32_t tolerance_percent = (soc_module_clk_t)clk_src == SOC_MOD_CLK_RC_FAST ? 10 : 1;
        mcpwm_timer_config_t config = {
            .clk_src = clk_src,
            .resolution_hz = 1000000, // 1MHz
            .period_ticks = 12000,
            .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        };

        printf("create MCPWM timers with clock source: %d\r\n", clk_src);
        mcpwm_timer_handle_t timers[num_timers];
        for (int group_id = 0; group_id < MCPWM_LL_GET(GROUP_NUM); group_id++) {
            config.group_id = group_id;
            for (int timer_id = 0; timer_id < MCPWM_LL_GET(TIMERS_PER_GROUP); timer_id++) {
                int timer_index = group_id * MCPWM_LL_GET(TIMERS_PER_GROUP) + timer_id;
                TEST_ESP_OK(mcpwm_new_timer(&config, &timers[timer_index]));
            }
        }

        printf("enable timers\r\n");
        for (int i = 0; i < num_timers; i++) {
            TEST_ESP_OK(mcpwm_timer_enable(timers[i]));
        }

        printf("check timer resolution\r\n");
        for (int i = 0; i < num_timers; i++) {
            TEST_ESP_OK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_START_NO_STOP));

            uint32_t start_count = 0;
            uint32_t end_count = 0;
            mcpwm_timer_direction_t direction;
            TEST_ESP_OK(mcpwm_timer_get_phase(timers[i], &start_count, &direction));
            int64_t start_time_us = esp_timer_get_time();

            esp_rom_delay_us(resolution_test_duration_us);

            TEST_ESP_OK(mcpwm_timer_get_phase(timers[i], &end_count, &direction));
            int64_t elapsed_time_us = esp_timer_get_time() - start_time_us;
            uint32_t expected_ticks = (uint64_t)config.resolution_hz * elapsed_time_us / 1000000;
            uint32_t actual_ticks;
            if (end_count >= start_count) {
                actual_ticks = end_count - start_count;
            } else {
                actual_ticks = config.period_ticks - start_count + end_count;
            }
            uint32_t tolerance_ticks = expected_ticks * tolerance_percent / 100;

            TEST_ASSERT_EQUAL(MCPWM_TIMER_DIRECTION_UP, direction);
            TEST_ASSERT_UINT32_WITHIN(tolerance_ticks, expected_ticks, actual_ticks);

            // make sure the timer has stopped
            TEST_ESP_OK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_STOP_EMPTY));
            vTaskDelay(pdMS_TO_TICKS(20));
            check_mcpwm_timer_phase(&timers[i], 1, 0, MCPWM_TIMER_DIRECTION_UP);
        }

        printf("disable and delete timers\r\n");
        for (int i = 0; i < num_timers; i++) {
            TEST_ESP_OK(mcpwm_timer_disable(timers[i]));
            TEST_ESP_OK(mcpwm_del_timer(timers[i]));
        }
    }
}

#define TEST_MCPWM_TIMER_EVENT_BIT_FULL  (1 << 0)
#define TEST_MCPWM_TIMER_EVENT_BIT_EMPTY (1 << 1)
#define TEST_MCPWM_TIMER_EVENT_BIT_STOP  (1 << 2)

typedef struct {
    EventGroupHandle_t event_group;
    uint32_t expected_full_counts;
    uint32_t expected_empty_counts;
    uint32_t accumulate_full_counts;
    uint32_t accumulate_empty_counts;
} test_mcpwm_timer_user_data_t;

IRAM_ATTR static bool test_on_stop(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)
{
    test_mcpwm_timer_user_data_t *udata = (test_mcpwm_timer_user_data_t *)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    esp_rom_printf("timer stopped at %" PRIu32 "\r\n", edata->count_value);
    TEST_ASSERT_EQUAL(0, edata->count_value);
    xEventGroupSetBitsFromISR(udata->event_group, TEST_MCPWM_TIMER_EVENT_BIT_STOP, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

IRAM_ATTR static bool test_on_full(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)
{
    test_mcpwm_timer_user_data_t *udata = (test_mcpwm_timer_user_data_t *)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    udata->accumulate_full_counts++;
    if (udata->accumulate_full_counts >= udata->expected_full_counts) {
        udata->accumulate_full_counts = 0;
        xEventGroupSetBitsFromISR(udata->event_group, TEST_MCPWM_TIMER_EVENT_BIT_FULL, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

IRAM_ATTR static bool test_on_empty(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)
{
    test_mcpwm_timer_user_data_t *udata = (test_mcpwm_timer_user_data_t *)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    udata->accumulate_empty_counts++;
    if (udata->accumulate_empty_counts >= udata->expected_empty_counts) {
        udata->accumulate_empty_counts = 0;
        xEventGroupSetBitsFromISR(udata->event_group, TEST_MCPWM_TIMER_EVENT_BIT_EMPTY, &high_task_wakeup);
    }
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("mcpwm_timer_event_callbacks", "[mcpwm]")
{
    EventGroupHandle_t event_group = xEventGroupCreate();
    EventBits_t bits = 0;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1us per tick
        .period_ticks = 20 * 1000,        // 20ms, 50Hz
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_timer_handle_t timer = NULL;
    printf("create mcpwm timer\r\n");
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));

    printf("register event callbacks\r\n");
    mcpwm_timer_event_callbacks_t cbs = {
        .on_stop = test_on_stop,
        .on_full = test_on_full,
        .on_empty = test_on_empty,
    };
    test_mcpwm_timer_user_data_t udata = {
        .event_group = event_group,
        .expected_empty_counts = 50,
        .expected_full_counts = 50,
    };
    TEST_ESP_OK(mcpwm_timer_register_event_callbacks(timer, &cbs, &udata));

    printf("enable timer\r\n");
    TEST_ESP_OK(mcpwm_timer_enable(timer));

    printf("start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    printf("wait for full and empty events\r\n");
    bits = xEventGroupWaitBits(event_group, TEST_MCPWM_TIMER_EVENT_BIT_FULL | TEST_MCPWM_TIMER_EVENT_BIT_EMPTY, pdTRUE, pdTRUE, pdMS_TO_TICKS(1300));
    TEST_ASSERT_EQUAL(TEST_MCPWM_TIMER_EVENT_BIT_FULL | TEST_MCPWM_TIMER_EVENT_BIT_EMPTY, bits);

    printf("stop timer and wait for event\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    bits = xEventGroupWaitBits(event_group, TEST_MCPWM_TIMER_EVENT_BIT_STOP, pdTRUE, pdTRUE, pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(TEST_MCPWM_TIMER_EVENT_BIT_STOP, bits);

    printf("update timer period\r\n");
    TEST_ESP_OK(mcpwm_timer_set_period(timer, 50 * 1000)); // period: 50ms, 20Hz
    udata.accumulate_empty_counts = 0;
    udata.accumulate_full_counts = 0;

    printf("start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    printf("wait for full and empty events\r\n");
    bits = xEventGroupWaitBits(event_group, TEST_MCPWM_TIMER_EVENT_BIT_FULL | TEST_MCPWM_TIMER_EVENT_BIT_EMPTY, pdTRUE, pdTRUE, pdMS_TO_TICKS(1500));
    // because the timer period changed, the previous wait time is not sufficient, so timeout
    TEST_ASSERT_EQUAL(0, bits);

    bits = xEventGroupWaitBits(event_group, TEST_MCPWM_TIMER_EVENT_BIT_FULL | TEST_MCPWM_TIMER_EVENT_BIT_EMPTY, pdTRUE, pdTRUE, pdMS_TO_TICKS(1500));
    TEST_ASSERT_EQUAL(TEST_MCPWM_TIMER_EVENT_BIT_FULL | TEST_MCPWM_TIMER_EVENT_BIT_EMPTY, bits);

    printf("stop timer\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    printf("disable timer\r\n");
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    printf("delete timer\r\n");
    TEST_ESP_OK(mcpwm_del_timer(timer));
    vEventGroupDelete(event_group);
}
