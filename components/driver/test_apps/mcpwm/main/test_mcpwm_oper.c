/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_gen.h"
#include "driver/mcpwm_fault.h"
#include "driver/gpio.h"

TEST_CASE("mcpwm_operator_install_uninstall", "[mcpwm]")
{
    const int total_operators = SOC_MCPWM_OPERATORS_PER_GROUP * SOC_MCPWM_GROUPS;
    mcpwm_timer_handle_t timers[SOC_MCPWM_GROUPS];
    mcpwm_oper_handle_t operators[total_operators];

    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1 * 1000 * 1000,
        .period_ticks = 10 * 1000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_operator_config_t operator_config = {
    };
    printf("install one MCPWM timer for each group\r\n");
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        timer_config.group_id = i;
        TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timers[i]));
    }
    printf("install MCPWM operators for each group\r\n");
    int k = 0;
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        operator_config.group_id = i;
        for (int j = 0; j < SOC_MCPWM_OPERATORS_PER_GROUP; j++) {
            TEST_ESP_OK(mcpwm_new_operator(&operator_config, &operators[k++]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_operator(&operator_config, &operators[0]));
    }
    printf("connect MCPWM timer and operators\r\n");
    k = 0;
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        for (int j = 0; j < SOC_MCPWM_OPERATORS_PER_GROUP; j++) {
            TEST_ESP_OK(mcpwm_operator_connect_timer(operators[k++], timers[i]));
        }
    }
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, mcpwm_operator_connect_timer(operators[0], timers[1]));
    printf("uninstall operators and timers\r\n");
    for (int i = 0; i < total_operators; i++) {
        TEST_ESP_OK(mcpwm_del_operator(operators[i]));
    }
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        TEST_ESP_OK(mcpwm_del_timer(timers[i]));
    }
}

TEST_CASE("mcpwm_operator_carrier", "[mcpwm]")
{
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz, 1us per tick
        .period_ticks = 20000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));

    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    mcpwm_oper_handle_t oper = NULL;
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));

    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = 0,
    };
    mcpwm_gen_handle_t generator = NULL;
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator));

    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(generator,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_TOGGLE),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));

    printf("add carrier to PWM wave\r\n");
    mcpwm_carrier_config_t carrier_config = {
        .frequency_hz = 1000000, // 1MHz carrier
        .duty_cycle = 0.5,
        .first_pulse_duration_us = 10,
    };
    TEST_ESP_OK(mcpwm_operator_apply_carrier(oper, &carrier_config));

    TEST_ESP_OK(mcpwm_timer_enable(timer));

    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("remove carrier from PWM wave\r\n");
    carrier_config.frequency_hz = 0;
    TEST_ESP_OK(mcpwm_operator_apply_carrier(oper, &carrier_config));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(200));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(generator));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}

static bool test_cbc_brake_on_gpio_fault_callback(mcpwm_oper_handle_t oper, const mcpwm_brake_event_data_t *edata, void *user_data)
{
    esp_rom_printf("cbc brake\r\n");
    return false;
}

static bool test_ost_brake_on_gpio_fault_callback(mcpwm_oper_handle_t oper, const mcpwm_brake_event_data_t *edata, void *user_data)
{
    esp_rom_printf("ost brake\r\n");
    return false;
}

TEST_CASE("mcpwm_operator_brake_on_gpio_fault", "[mcpwm]")
{
    printf("install timer\r\n");
    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = 1000000, // 1MHz, 1us per tick
        .period_ticks = 20000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));

    printf("install operator\r\n");
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    mcpwm_oper_handle_t oper = NULL;
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));

    printf("set brake event callbacks for operator\r\n");
    mcpwm_operator_event_callbacks_t cbs = {
        .on_brake_cbc = test_cbc_brake_on_gpio_fault_callback,
        .on_brake_ost = test_ost_brake_on_gpio_fault_callback,
    };
    TEST_ESP_OK(mcpwm_operator_register_event_callbacks(oper, &cbs, NULL));

    printf("install gpio fault\r\n");
    mcpwm_gpio_fault_config_t gpio_fault_config = {
        .group_id = 0,
        .flags.active_level = 1,
        .flags.io_loop_back = true,
        .flags.pull_down = true,
    };
    mcpwm_fault_handle_t gpio_cbc_fault = NULL;
    mcpwm_fault_handle_t gpio_ost_fault = NULL;
    const int cbc_fault_gpio = 4;
    const int ost_fault_gpio = 5;

    gpio_fault_config.gpio_num = cbc_fault_gpio;
    TEST_ESP_OK(mcpwm_new_gpio_fault(&gpio_fault_config, &gpio_cbc_fault));
    gpio_fault_config.gpio_num = ost_fault_gpio;
    TEST_ESP_OK(mcpwm_new_gpio_fault(&gpio_fault_config, &gpio_ost_fault));

    // put fault GPIO into a safe state
    gpio_set_level(cbc_fault_gpio, 0);
    gpio_set_level(ost_fault_gpio, 0);

    printf("set brake mode on fault\r\n");
    mcpwm_brake_config_t brake_config = {
        .fault = gpio_cbc_fault,
        .brake_mode = MCPWM_OPER_BRAKE_MODE_CBC,
        .flags.cbc_recover_on_tez = true,
    };
    TEST_ESP_OK(mcpwm_operator_set_brake_on_fault(oper, &brake_config));
    brake_config.fault = gpio_ost_fault;
    brake_config.brake_mode = MCPWM_OPER_BRAKE_MODE_OST;
    TEST_ESP_OK(mcpwm_operator_set_brake_on_fault(oper, &brake_config));

    printf("create generators\r\n");
    const int gen_a_gpio = 0;
    const int gen_b_gpio = 2;
    mcpwm_gen_handle_t gen_a = NULL;
    mcpwm_gen_handle_t gen_b = NULL;
    mcpwm_generator_config_t generator_config = {
        .flags.io_loop_back = true,
    };
    generator_config.gen_gpio_num = gen_a_gpio;
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &gen_a));
    generator_config.gen_gpio_num = gen_b_gpio;
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &gen_b));

    printf("set generator actions on timer event\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen_a,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen_b,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));

    printf("set generator actions on brake event\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_brake_event(gen_a,
                MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_BRAKE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_brake_event(gen_b,
                MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_OST, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_BRAKE_EVENT_ACTION_END()));

    printf("enable and start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_enable(timer));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    printf("trigger GPIO fault signal, brake in CBC mode\r\n");
    for (int i = 0; i < 10; i++) {
        gpio_set_level(cbc_fault_gpio, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        TEST_ASSERT_EQUAL(1, gpio_get_level(gen_a_gpio));
        // remove the fault signal
        gpio_set_level(cbc_fault_gpio, 0);
        // recovery
        TEST_ESP_OK(mcpwm_operator_recover_from_fault(oper, gpio_cbc_fault));
        vTaskDelay(pdMS_TO_TICKS(40));
        // should recovery automatically
        TEST_ASSERT_EQUAL(0, gpio_get_level(gen_a_gpio));
    }

    printf("trigger GPIO fault signal, brake in OST mode\r\n");
    for (int i = 0; i < 10; i++) {
        gpio_set_level(ost_fault_gpio, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
        TEST_ASSERT_EQUAL(1, gpio_get_level(gen_b_gpio));
        // can't recover because fault signal is still active
        TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_operator_recover_from_fault(oper, gpio_ost_fault));
        // remove the fault signal
        gpio_set_level(ost_fault_gpio, 0);
        vTaskDelay(pdMS_TO_TICKS(40));
        // for ost brake, the generator can't recover before we manually recover it
        TEST_ASSERT_EQUAL(1, gpio_get_level(gen_b_gpio));
        // now it's safe to recover the operator
        TEST_ESP_OK(mcpwm_operator_recover_from_fault(oper, gpio_ost_fault));
        vTaskDelay(pdMS_TO_TICKS(40));
        // should recovery now
        TEST_ASSERT_EQUAL(0, gpio_get_level(gen_b_gpio));
    }

    printf("delete all mcpwm objects\r\n");
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_fault(gpio_cbc_fault));
    TEST_ESP_OK(mcpwm_del_fault(gpio_ost_fault));
    TEST_ESP_OK(mcpwm_del_generator(gen_a));
    TEST_ESP_OK(mcpwm_del_generator(gen_b));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}

TEST_CASE("mcpwm_operator_brake_on_soft_fault", "[mcpwm]")
{
    printf("install timer\r\n");
    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = 1000000, // 1MHz, 1us per tick
        .period_ticks = 20000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));

    printf("install operator\r\n");
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    mcpwm_oper_handle_t oper = NULL;
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));

    printf("install soft fault\r\n");
    mcpwm_soft_fault_config_t soft_fault_config = {};
    mcpwm_fault_handle_t soft_fault = NULL;
    TEST_ESP_OK(mcpwm_new_soft_fault(&soft_fault_config, &soft_fault));

    printf("set brake mode on fault\r\n");
    mcpwm_brake_config_t brake_config = {
        .fault = soft_fault,
        .brake_mode = MCPWM_OPER_BRAKE_MODE_CBC,
        .flags.cbc_recover_on_tez = true,
    };
    TEST_ESP_OK(mcpwm_operator_set_brake_on_fault(oper, &brake_config));

    printf("create generators\r\n");
    const int gen_a_gpio = 0;
    const int gen_b_gpio = 2;
    mcpwm_gen_handle_t gen_a = NULL;
    mcpwm_gen_handle_t gen_b = NULL;
    mcpwm_generator_config_t generator_config = {
        .flags.io_loop_back = true,
    };
    generator_config.gen_gpio_num = gen_a_gpio;
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &gen_a));
    generator_config.gen_gpio_num = gen_b_gpio;
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &gen_b));

    printf("set generator actions on timer event\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen_a,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen_b,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));

    printf("set generator actions on brake event\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_brake_event(gen_a,
                MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_CBC, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_BRAKE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_brake_event(gen_b,
                MCPWM_GEN_BRAKE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_OPER_BRAKE_MODE_OST, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_BRAKE_EVENT_ACTION_END()));

    printf("enable and start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_enable(timer));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    printf("trigger soft fault signal, brake in CBC mode\r\n");
    for (int i = 0; i < 1; i++) {
        // stop the timer, so the operator can't recover from fault automatically
        TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
        vTaskDelay(pdMS_TO_TICKS(40));
        // check initial generator output
        TEST_ASSERT_EQUAL(0, gpio_get_level(gen_a_gpio));
        TEST_ESP_OK(mcpwm_soft_fault_activate(soft_fault));
        // check generate output on fault event
        TEST_ASSERT_EQUAL(1, gpio_get_level(gen_a_gpio));
        // start the timer, so that operator can recover at a specific event (e.g. tez)
        TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
        // recover on tez
        TEST_ESP_OK(mcpwm_operator_recover_from_fault(oper, soft_fault));
        vTaskDelay(pdMS_TO_TICKS(40));
        // the generator output should be recoverd automatically
        TEST_ASSERT_EQUAL(0, gpio_get_level(gen_a_gpio));
    }

    printf("change the brake mode to ost\r\n");
    brake_config.brake_mode = MCPWM_OPER_BRAKE_MODE_OST;
    TEST_ESP_OK(mcpwm_operator_set_brake_on_fault(oper, &brake_config));

    printf("trigger soft fault signal, brake in OST mode\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    for (int i = 0; i < 10; i++) {
        // check initial generator output
        TEST_ASSERT_EQUAL(0, gpio_get_level(gen_b_gpio));
        TEST_ESP_OK(mcpwm_soft_fault_activate(soft_fault));
        TEST_ASSERT_EQUAL(1, gpio_get_level(gen_b_gpio));
        vTaskDelay(pdMS_TO_TICKS(40));
        // don't recover without a manual recover
        TEST_ASSERT_EQUAL(1, gpio_get_level(gen_b_gpio));
        TEST_ESP_OK(mcpwm_operator_recover_from_fault(oper, soft_fault));
        vTaskDelay(pdMS_TO_TICKS(10));
        // should recovery now
        TEST_ASSERT_EQUAL(0, gpio_get_level(gen_b_gpio));
    }

    printf("delete all mcpwm objects\r\n");
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_fault(soft_fault));
    TEST_ESP_OK(mcpwm_del_generator(gen_a));
    TEST_ESP_OK(mcpwm_del_generator(gen_b));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}
