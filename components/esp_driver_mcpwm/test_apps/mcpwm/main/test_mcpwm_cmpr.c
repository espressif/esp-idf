/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_cmpr.h"

TEST_CASE("mcpwm_comparator_install_uninstall", "[mcpwm]")
{
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparators[SOC_MCPWM_COMPARATORS_PER_OPERATOR];

    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .period_ticks = 10 * 1000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    printf("install timer and operator");
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));

    printf("install comparator\r\n");
    mcpwm_comparator_config_t comparator_config = {};
    for (int i = 0; i < SOC_MCPWM_COMPARATORS_PER_OPERATOR; i++) {
        TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparators[i]));
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_comparator(oper, &comparator_config, &comparators[0]));

    printf("connect MCPWM timer and operators\r\n");
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));

    printf("uninstall timer, operator and comparators\r\n");
    // can't delete operator if the comparators are still in working
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_del_operator(oper));
    for (int i = 0; i < SOC_MCPWM_COMPARATORS_PER_OPERATOR; i++) {
        TEST_ESP_OK(mcpwm_del_comparator(comparators[i]));
    }
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}

static bool IRAM_ATTR test_compare_on_reach(mcpwm_cmpr_handle_t cmpr, const mcpwm_compare_event_data_t *ev_data, void *user_data)
{
    uint32_t *counts = (uint32_t *)user_data;
    (*counts)++;
    return false;
}

TEST_CASE("mcpwm_comparator_event_callback", "[mcpwm]")
{
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator;

    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .period_ticks = 10 * 1000, // 10ms
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    mcpwm_comparator_config_t comparator_config = {};
    printf("install timer, operator and comparator\r\n");
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

    // set compare value before connecting timer and operator will fail
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_comparator_set_compare_value(comparator, 5000));
    printf("connect MCPWM timer and operators\r\n");
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));
    // compare ticks can't exceed the timer's period ticks
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, mcpwm_comparator_set_compare_value(comparator, 20 * 1000));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator, 5 * 1000));

    printf("register compare event callback\r\n");
    uint32_t compare_counts = 0;
    mcpwm_comparator_event_callbacks_t cbs = {
        .on_reach = test_compare_on_reach,
    };
    TEST_ESP_OK(mcpwm_comparator_register_event_callbacks(comparator, &cbs, &compare_counts));

    printf("start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_enable(timer));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    esp_rom_delay_us(1000 * 1000);
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    printf("compare_counts=%"PRIu32"\r\n", compare_counts);
    // the timer period is 10ms, the expected compare_counts = 1s/10ms = 100
    TEST_ASSERT_INT_WITHIN(1, 100, compare_counts);

    printf("uninstall timer, operator and comparator\r\n");
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_comparator(comparator));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}
