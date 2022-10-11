/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_clk.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"
#include "test_mcpwm_utils.h"

static bool IRAM_ATTR test_capture_callback_iram_safe(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    uint32_t *cap_value = (uint32_t *)user_data;
    if (edata->cap_edge == MCPWM_CAP_EDGE_NEG) {
        cap_value[1] = edata->cap_value;
    } else {
        cap_value[0] = edata->cap_value;
    }
    return false;
}

static void IRAM_ATTR test_simulate_input_post_cache_disable(void *args)
{
    int gpio_sig = (int)args;
    gpio_set_level(gpio_sig, 1);
    esp_rom_delay_us(1000);
    gpio_set_level(gpio_sig, 0);
}

TEST_CASE("mcpwm_capture_iram_safe", "[mcpwm]")
{
    printf("install mcpwm capture timer\r\n");
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_APB,
        .group_id = 0,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));

    const int cap_gpio = 0;
    // put the GPIO into a preset state
    gpio_set_level(cap_gpio, 0);

    printf("install mcpwm capture channel\r\n");
    mcpwm_cap_channel_handle_t pps_channel;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = cap_gpio,
        .prescale = 1,
        .flags.pos_edge = true,
        .flags.neg_edge = true,
        .flags.io_loop_back = true, // so we can use GPIO functions to simulate the external capture signal
        .flags.pull_up = true,
    };
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &pps_channel));

    printf("install callback for capture channel\r\n");
    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = test_capture_callback_iram_safe,
    };
    uint32_t cap_value[2] = {0};
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(pps_channel, &cbs, cap_value));

    printf("enable capture channel\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_enable(pps_channel));

    printf("enable and start capture timer\r\n");
    TEST_ESP_OK(mcpwm_capture_timer_enable(cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_start(cap_timer));

    printf("disable cache, simulate GPIO capture signal\r\n");
    unity_utils_run_cache_disable_stub(test_simulate_input_post_cache_disable, (void *)cap_gpio);

    printf("capture value: Pos=%"PRIu32", Neg=%"PRIu32"\r\n", cap_value[0], cap_value[1]);
    // Capture timer is clocked from APB by default
    uint32_t clk_src_res = esp_clk_apb_freq();
    TEST_ASSERT_UINT_WITHIN(2000, clk_src_res / 1000, cap_value[1] - cap_value[0]);

    printf("uninstall capture channel and timer\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_disable(pps_channel));
    TEST_ESP_OK(mcpwm_del_capture_channel(pps_channel));
    TEST_ESP_OK(mcpwm_capture_timer_disable(cap_timer));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
}

static bool IRAM_ATTR test_compare_on_reach(mcpwm_cmpr_handle_t cmpr, const mcpwm_compare_event_data_t *ev_data, void *user_data)
{
    uint32_t cmp_val = ev_data->compare_ticks;
    cmp_val += 10;
    // compare ticks can't exceed the timer's period ticks
    if (cmp_val >= 50) {
        cmp_val = 0;
    }
    mcpwm_comparator_set_compare_value(cmpr, cmp_val);
    return false;
}

static void IRAM_ATTR test_delay_post_cache_disable(void *args)
{
    esp_rom_delay_us(1000);
}

TEST_CASE("mcpwm_comparator_iram_safe", "[mcpwm]")
{
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t oper;
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t gen;

    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .period_ticks = 50, // 50us <-> 20KHz
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tep = true,
        .flags.update_cmp_on_tez = true,
    };
    printf("install timer, operator and comparator\r\n");
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

    printf("connect MCPWM timer and operators\r\n");
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator, 10));

    printf("install MCPWM generator\r\n");
    mcpwm_generator_config_t gen_config = {
        .gen_gpio_num = 0,
    };
    TEST_ESP_OK(mcpwm_new_generator(oper, &gen_config, &gen));

    printf("set generator actions on timer and compare events\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gen,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));

    printf("register compare event callback\r\n");
    mcpwm_comparator_event_callbacks_t cbs = {
        .on_reach = test_compare_on_reach,
    };
    TEST_ESP_OK(mcpwm_comparator_register_event_callbacks(comparator, &cbs, NULL));

    printf("start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_enable(timer));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    printf("disable flash cache and check the compare events are still in working\r\n");
    for (int i = 0; i < 50; i++) {
        unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
    }

    printf("uninstall timer, operator and comparator\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(gen));
    TEST_ESP_OK(mcpwm_del_comparator(comparator));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}
