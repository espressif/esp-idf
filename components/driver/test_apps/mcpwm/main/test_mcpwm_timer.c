/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "soc/soc_caps.h"
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
    const int num_timers = SOC_MCPWM_TIMERS_PER_GROUP * SOC_MCPWM_GROUPS;

    printf("create mcpwm timer instances\r\n");
    mcpwm_timer_handle_t timers[num_timers];
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            config.group_id = i;
            TEST_ESP_OK(mcpwm_new_timer(&config, &timers[i * SOC_MCPWM_TIMERS_PER_GROUP + j]));
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

static bool test_on_stop(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)
{
    test_mcpwm_timer_user_data_t *udata = (test_mcpwm_timer_user_data_t *)user_data;
    BaseType_t high_task_wakeup = pdFALSE;
    esp_rom_printf("timer stopped at %u\r\n", edata->count_value);
    TEST_ASSERT_EQUAL(0, edata->count_value);
    xEventGroupSetBitsFromISR(udata->event_group, TEST_MCPWM_TIMER_EVENT_BIT_STOP, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

static bool test_on_full(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)
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

static bool test_on_empty(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data)
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
    bits = xEventGroupWaitBits(event_group, TEST_MCPWM_TIMER_EVENT_BIT_FULL | TEST_MCPWM_TIMER_EVENT_BIT_EMPTY, pdTRUE, pdTRUE, pdMS_TO_TICKS(1050));
    TEST_ASSERT_EQUAL(TEST_MCPWM_TIMER_EVENT_BIT_FULL | TEST_MCPWM_TIMER_EVENT_BIT_EMPTY, bits);

    printf("stop timer and wait for event\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    bits = xEventGroupWaitBits(event_group, TEST_MCPWM_TIMER_EVENT_BIT_STOP, pdTRUE, pdTRUE, pdMS_TO_TICKS(50));
    TEST_ASSERT_EQUAL(TEST_MCPWM_TIMER_EVENT_BIT_STOP, bits);

    printf("disable timer\r\n");
    TEST_ESP_OK(mcpwm_timer_disable(timer));

    printf("delete timer\r\n");
    TEST_ESP_OK(mcpwm_del_timer(timer));
    vEventGroupDelete(event_group);
}
