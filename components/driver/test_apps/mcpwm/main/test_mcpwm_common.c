/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "unity.h"
#include "esp_private/mcpwm.h"
#include "test_mcpwm_utils.h"
#include "driver/mcpwm_prelude.h"

TEST_CASE("mcpwm_set_interrupt_priority", "[mcpwm]")
{
    printf("install timer\r\n");
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .period_ticks = 20 * 1000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .intr_priority = 3,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));
    printf("register event callbacks\r\n");
    mcpwm_timer_event_callbacks_t timer_cbs = {
        .on_stop = NULL,
        .on_full = NULL,
        .on_empty = NULL,
    };
    TEST_ESP_OK(mcpwm_timer_register_event_callbacks(timer, &timer_cbs, NULL));
    timer_config.intr_priority = 1;
    mcpwm_timer_handle_t timer2 = NULL;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_new_timer(&timer_config, &timer2));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, mcpwm_timer_register_event_callbacks(timer2, &timer_cbs, NULL));

    printf("install operator\r\n");
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
        .intr_priority = 0,
    };
    mcpwm_oper_handle_t oper = NULL;
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));
    printf("register event callbacks\r\n");
    mcpwm_operator_event_callbacks_t oper_cbs = {
        .on_brake_cbc = NULL,
        .on_brake_ost = NULL,
    };
    TEST_ESP_OK(mcpwm_operator_register_event_callbacks(oper, &oper_cbs, NULL));
    operator_config.intr_priority = 1;
    mcpwm_oper_handle_t oper2 = NULL;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_new_operator(&operator_config, &oper2));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, mcpwm_operator_register_event_callbacks(oper2, &oper_cbs, NULL));

    printf("install comparator\r\n");
    mcpwm_comparator_config_t comparator_config = {
        .intr_priority = 0,
    };
    mcpwm_cmpr_handle_t comparator = NULL;
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator));
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));
    printf("register event callback\r\n");
    mcpwm_comparator_event_callbacks_t comparator_cbs = {
        .on_reach = NULL,
    };
    TEST_ESP_OK(mcpwm_comparator_register_event_callbacks(comparator, &comparator_cbs, NULL));
    comparator_config.intr_priority = 1;
    mcpwm_cmpr_handle_t comparator2 = NULL;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_new_comparator(oper, &comparator_config, &comparator2));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, mcpwm_comparator_register_event_callbacks(comparator2, &comparator_cbs, NULL));

    printf("install gpio fault\r\n");
    const int fault_gpio = 0;
    mcpwm_fault_handle_t fault = NULL;
    mcpwm_gpio_fault_config_t gpio_fault_config = {
        .group_id = 0,
        .gpio_num = fault_gpio,
        .intr_priority = 0,
    };
    TEST_ESP_OK(mcpwm_new_gpio_fault(&gpio_fault_config, &fault));

    printf("register event callback\r\n");
    mcpwm_fault_event_callbacks_t fault_cbs = {
        .on_fault_enter = NULL,
        .on_fault_exit = NULL,
    };
    TEST_ESP_OK(mcpwm_fault_register_event_callbacks(fault, &fault_cbs, NULL));
    gpio_fault_config.intr_priority = 1;
    mcpwm_fault_handle_t fault2 = NULL;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_new_gpio_fault(&gpio_fault_config, &fault2));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, mcpwm_fault_register_event_callbacks(fault2, &fault_cbs, NULL));

    printf("install capture timer\r\n");
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));
    printf("install capture\r\n");
    mcpwm_cap_channel_handle_t cap_channel = NULL;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = -1,
        .prescale = 2,
        .intr_priority = 3,
    };
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &cap_channel));

    printf("register event callback\r\n");
    mcpwm_capture_event_callbacks_t cap_cbs = {
        .on_cap = NULL,
    };
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(cap_channel, &cap_cbs, NULL));
    cap_chan_config.intr_priority = 1;
    mcpwm_cap_channel_handle_t cap_channel2 = NULL;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &cap_channel2));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, mcpwm_capture_channel_register_event_callbacks(cap_channel2, &cap_cbs, NULL));

    printf("delete all mcpwm objects\r\n");
    TEST_ESP_OK(mcpwm_del_comparator(comparator));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_capture_channel(cap_channel));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
    TEST_ESP_OK(mcpwm_del_timer(timer));
    TEST_ESP_OK(mcpwm_del_fault(fault));
}

TEST_CASE("mcpwm_group_set_prescale_dynamically", "[mcpwm]")
{
    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 100 * 1000, // 100kHz
        .period_ticks = 400,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
        .group_id = 0,
    };

    mcpwm_timer_handle_t timer = NULL;
    printf("create mcpwm timer\r\n");
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    mcpwm_oper_handle_t oper = NULL;
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));

    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = 0,
    };
    mcpwm_gen_handle_t generator = NULL;
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator));

    printf("add carrier to PWM wave\r\n");
    mcpwm_carrier_config_t carrier_config = {
        .clk_src = MCPWM_CARRIER_CLK_SRC_DEFAULT,
        .frequency_hz = 100000, // 100KHz carrier need higher group prescale
        .duty_cycle = 0.5,
        .first_pulse_duration_us = 10,
    };

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_operator_apply_carrier(oper, &carrier_config));
    carrier_config.frequency_hz = 2000000; // 2MHz carrier
    carrier_config.first_pulse_duration_us = 5;
    TEST_ESP_OK(mcpwm_operator_apply_carrier(oper, &carrier_config));


    TEST_ESP_OK(mcpwm_del_generator(generator));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}
