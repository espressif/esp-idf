/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_attr.h"
#include "driver/gptimer_etm.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "driver/mcpwm_prelude.h"

TEST_CASE("mcpwm_comparator_etm_event", "[mcpwm][etm]")
{
    // MCPWM cmpra -------------------------------------> ETM channel A ---> GPTimer start
    // MCPWM cmprb / evt_cmpra (if support evt_cmpr) ---> ETM channel B ---> GPTimer stop

    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 10000,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
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
