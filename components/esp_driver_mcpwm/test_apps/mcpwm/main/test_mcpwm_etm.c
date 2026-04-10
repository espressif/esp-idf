/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_attr.h"
#include "hal/mcpwm_ll.h"
#include "driver/gptimer_etm.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "driver/mcpwm_prelude.h"

TEST_CASE("mcpwm_comparator_etm_event", "[mcpwm][etm]")
{
    // MCPWM cmpra -------------------------------------> ETM channel A ---> GPTimer start
    // MCPWM cmprb / evt_cmpra (if support evt_cmpr) ---> ETM channel B ---> GPTimer stop
    // Use the last MCPWM group (e.g. group_id=1 when MCPWM_LL_GROUP_NUM==2) to test multi-group chips
    const int mcpwm_group_id = MCPWM_LL_GET(GROUP_NUM) - 1;

    mcpwm_timer_config_t timer_config = {
        .group_id = mcpwm_group_id,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 10000,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_operator_config_t operator_config = {
        .group_id = mcpwm_group_id,
    };
    mcpwm_oper_handle_t oper = NULL;
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));

    TEST_ESP_OK(mcpwm_timer_enable(timer));

    // install comparator
    int cmpa = 2000, cmpb = 8000;
    mcpwm_cmpr_handle_t comparator_a = NULL;
    mcpwm_cmpr_handle_t comparator_b = NULL;
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator_a));
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator_b));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator_a, cmpa));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator_b, cmpb));

    // install generator
    const uint32_t gen_gpio = 10;
    mcpwm_gen_handle_t generator = NULL;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = gen_gpio,
    };
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator));
    TEST_ESP_OK(mcpwm_generator_set_action_on_compare_event(generator,
                                                            MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator_a, MCPWM_GEN_ACTION_HIGH)));
    TEST_ESP_OK(mcpwm_generator_set_action_on_compare_event(generator,
                                                            MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator_b, MCPWM_GEN_ACTION_LOW)));

    // allocate etm channels
    printf("allocate etm channels\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a = NULL;
    esp_etm_channel_handle_t etm_channel_b = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_b));

    printf("allocate mcpwm comparator event\r\n");
    esp_etm_event_handle_t mcpwm_etm_event1 = NULL;
    esp_etm_event_handle_t mcpwm_etm_event2 = NULL;
    mcpwm_cmpr_etm_event_config_t mcpwm_cmpr_event_config = {
        .event_type = MCPWM_CMPR_ETM_EVENT_EQUAL,
    };
    TEST_ESP_OK(mcpwm_comparator_new_etm_event(comparator_a, &mcpwm_cmpr_event_config, &mcpwm_etm_event1));

#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    // install event comparator
    cmpb = 5000;
    mcpwm_cmpr_handle_t event_comparator = NULL;
    mcpwm_event_comparator_config_t event_comparator_config = {};
    TEST_ESP_OK(mcpwm_new_event_comparator(oper, &event_comparator_config, &event_comparator));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(event_comparator, cmpb));
    TEST_ESP_OK(mcpwm_comparator_new_etm_event(event_comparator, &mcpwm_cmpr_event_config, &mcpwm_etm_event2));
#else
    TEST_ESP_OK(mcpwm_comparator_new_etm_event(comparator_b, &mcpwm_cmpr_event_config, &mcpwm_etm_event2));
#endif // SOC_MCPWM_SUPPORT_EVENT_COMPARATOR

    printf("create a gptimer\r\n");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t gptimer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };
    TEST_ESP_OK(gptimer_new_timer(&gptimer_config, &gptimer));

    printf("get gptimer etm task handle\r\n");
    esp_etm_task_handle_t gptimer_task_start, gptimer_task_stop;
    gptimer_etm_task_config_t gptimer_etm_task_conf = {
        .task_type = GPTIMER_ETM_TASK_START_COUNT,
    };
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task_start));
    gptimer_etm_task_conf.task_type = GPTIMER_ETM_TASK_STOP_COUNT;
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task_stop));

    printf("enable timer\r\n");
    TEST_ESP_OK(gptimer_enable(gptimer));

    printf("connect event and task to the channel\r\n");
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, mcpwm_etm_event1, gptimer_task_start));
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_b, mcpwm_etm_event2, gptimer_task_stop));

    printf("enable etm channel\r\n");
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_b));

    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    esp_rom_delay_us(100 * 1000);
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));

    uint64_t cur_count_val = 0;
    TEST_ESP_OK(gptimer_get_raw_count(gptimer, &cur_count_val));
    printf("cur_count_val: %llu\r\n", cur_count_val);
    TEST_ASSERT_UINT_WITHIN(100, (cmpb - cmpa) * 10, cur_count_val);

    // delete gptimer
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));

    // delete etm primitives
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_b));
    TEST_ESP_OK(esp_etm_del_task(gptimer_task_start));
    TEST_ESP_OK(esp_etm_del_task(gptimer_task_stop));
    TEST_ESP_OK(esp_etm_del_event(mcpwm_etm_event1));
    TEST_ESP_OK(esp_etm_del_event(mcpwm_etm_event2));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_b));
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(generator));
    TEST_ESP_OK(mcpwm_del_comparator(comparator_a));
    TEST_ESP_OK(mcpwm_del_comparator(comparator_b));
#if SOC_MCPWM_SUPPORT_EVENT_COMPARATOR
    TEST_ESP_OK(mcpwm_del_comparator(event_comparator));
#endif
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}

TEST_CASE("mcpwm_timer_etm_event", "[mcpwm][etm]")
{
    // MCPWM timer TEP (timer equals peak) ---> ETM channel A ---> GPTimer start
    // MCPWM timer TEZ (timer equals zero) ---> ETM channel B ---> GPTimer stop
    // Using UP_DOWN count mode, the timer counts: 0 -> peak -> 0 -> peak -> ...
    // GPTimer starts at TEP and stops at TEZ, so it measures the time from peak to 0
    // Use the last MCPWM group (e.g. group_id=1 when MCPWM_LL_GROUP_NUM==2) to test multi-group chips
    const int mcpwm_group_id = MCPWM_LL_GET(GROUP_NUM) - 1;

    printf("create mcpwm timer\r\n");
    const uint32_t timer_resolution = 1 * 1000 * 1000; // 1MHz, 1 tick = 1us
    const uint32_t period_ticks = 10000; // For UP_DOWN mode, peak = period_ticks / 2 = 5000
    mcpwm_timer_config_t timer_config = {
        .group_id = mcpwm_group_id,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = timer_resolution,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
        .period_ticks = period_ticks,
    };
    mcpwm_timer_handle_t mcpwm_timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &mcpwm_timer));
    TEST_ESP_OK(mcpwm_timer_enable(mcpwm_timer));

    printf("create mcpwm timer etm event\r\n");
    esp_etm_event_handle_t mcpwm_timer_event_tez = NULL;
    esp_etm_event_handle_t mcpwm_timer_event_tep = NULL;
    mcpwm_timer_etm_event_config_t timer_etm_event_config = {
        .event_type = MCPWM_TIMER_ETM_EVENT_TEZ,
    };
    TEST_ESP_OK(mcpwm_timer_new_etm_event(mcpwm_timer, &timer_etm_event_config, &mcpwm_timer_event_tez));
    timer_etm_event_config.event_type = MCPWM_TIMER_ETM_EVENT_TEP;
    TEST_ESP_OK(mcpwm_timer_new_etm_event(mcpwm_timer, &timer_etm_event_config, &mcpwm_timer_event_tep));

    printf("create gptimer\r\n");
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t gptimer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = timer_resolution, // Same resolution as MCPWM timer
    };
    TEST_ESP_OK(gptimer_new_timer(&gptimer_config, &gptimer));

    printf("create gptimer etm task\r\n");
    esp_etm_task_handle_t gptimer_task_start = NULL;
    esp_etm_task_handle_t gptimer_task_stop = NULL;
    gptimer_etm_task_config_t gptimer_etm_task_conf = {
        .task_type = GPTIMER_ETM_TASK_START_COUNT,
    };
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task_start));
    gptimer_etm_task_conf.task_type = GPTIMER_ETM_TASK_STOP_COUNT;
    TEST_ESP_OK(gptimer_new_etm_task(gptimer, &gptimer_etm_task_conf, &gptimer_task_stop));
    TEST_ESP_OK(gptimer_enable(gptimer));

    printf("create and connect etm channels\r\n");
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel_a = NULL;
    esp_etm_channel_handle_t etm_channel_b = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_a));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel_b));
    // TEP -> start gptimer, TEZ -> stop gptimer
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_a, mcpwm_timer_event_tep, gptimer_task_start));
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel_b, mcpwm_timer_event_tez, gptimer_task_stop));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel_b));

    printf("start mcpwm timer and let it run for multiple periods\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(mcpwm_timer, MCPWM_TIMER_START_NO_STOP));
    esp_rom_delay_us(100 * 1000); // Let it run for 100ms
    TEST_ESP_OK(mcpwm_timer_start_stop(mcpwm_timer, MCPWM_TIMER_STOP_EMPTY));

    printf("verify gptimer count value\r\n");
    uint64_t gptimer_count = 0;
    TEST_ESP_OK(gptimer_get_raw_count(gptimer, &gptimer_count));
    printf("gptimer_count: %"PRIu64"\r\n", gptimer_count);
    // In UP_DOWN mode with period_ticks=10000:
    // - One full cycle (0->peak->0) takes 10000 ticks
    // - Peak value is period_ticks/2 = 5000
    // - GPTimer accumulates: 10 cycles * 5000 ticks = 50000 ticks
    const uint32_t expected_count = (100 * 1000 / period_ticks) * (period_ticks / 2);
    TEST_ASSERT_UINT_WITHIN(500, expected_count, gptimer_count);

    printf("cleanup\r\n");
    TEST_ESP_OK(gptimer_disable(gptimer));
    TEST_ESP_OK(gptimer_del_timer(gptimer));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_a));
    TEST_ESP_OK(esp_etm_channel_disable(etm_channel_b));
    TEST_ESP_OK(esp_etm_del_task(gptimer_task_start));
    TEST_ESP_OK(esp_etm_del_task(gptimer_task_stop));
    TEST_ESP_OK(esp_etm_del_event(mcpwm_timer_event_tez));
    TEST_ESP_OK(esp_etm_del_event(mcpwm_timer_event_tep));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_a));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel_b));
    TEST_ESP_OK(mcpwm_timer_disable(mcpwm_timer));
    TEST_ESP_OK(mcpwm_del_timer(mcpwm_timer));
}
