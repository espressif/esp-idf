/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_oper.h"
#include "driver/mcpwm_cmpr.h"
#include "driver/mcpwm_gen.h"
#include "driver/gpio.h"

TEST_CASE("mcpwm_generator_install_uninstall", "[mcpwm]")
{
    mcpwm_operator_config_t oper_config = {
        .group_id = 0,
    };
    mcpwm_oper_handle_t oper = NULL;
    printf("create a MCPWM operator\r\n");
    TEST_ESP_OK(mcpwm_new_operator(&oper_config, &oper));

    printf("create MCPWM generators from that operator\r\n");
    mcpwm_gen_handle_t gens[SOC_MCPWM_GENERATORS_PER_OPERATOR];
    mcpwm_generator_config_t gen_config = {
        .gen_gpio_num = 0,
    };
    for (int i = 0; i < SOC_MCPWM_GENERATORS_PER_OPERATOR; i++) {
        TEST_ESP_OK(mcpwm_new_generator(oper, &gen_config, &gens[i]));
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_generator(oper, &gen_config, &gens[0]));

    printf("delete generators and operator\r\n");
    // can't delete operator if the generator is till in working
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_del_operator(oper));
    for (int i = 0; i < SOC_MCPWM_GENERATORS_PER_OPERATOR; i++) {
        TEST_ESP_OK(mcpwm_del_generator(gens[i]));
    }
    TEST_ESP_OK(mcpwm_del_operator(oper));
}

TEST_CASE("mcpwm_generator_force_level_hold_on", "[mcpwm]")
{
    // The operator can even work without the timer
    printf("create operator and generator\r\n");
    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
    };
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));

    mcpwm_gen_handle_t generator = NULL;
    const int gen_gpio = 0;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = gen_gpio,
        .flags.io_loop_back = true, // loop back for test
    };
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator));

    printf("add force level to the generator, hold on");
    for (int i = 0; i < 10; i++) {
        TEST_ESP_OK(mcpwm_generator_set_force_level(generator, 0, true));
        vTaskDelay(pdMS_TO_TICKS(10));
        TEST_ASSERT_EQUAL(0, gpio_get_level(gen_gpio));
        TEST_ESP_OK(mcpwm_generator_set_force_level(generator, 1, true));
        vTaskDelay(pdMS_TO_TICKS(10));
        TEST_ASSERT_EQUAL(1, gpio_get_level(gen_gpio));
    }

    printf("remove the force level\r\n");
    TEST_ESP_OK(mcpwm_generator_set_force_level(generator, -1, true));

    printf("delete generator and operator\r\n");
    TEST_ESP_OK(mcpwm_del_generator(generator));
    TEST_ESP_OK(mcpwm_del_operator(oper));
}

TEST_CASE("mcpwm_generator_force_level_recovery", "[mcpwm]")
{
    printf("create mcpwm timer\r\n");
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .resolution_hz = 1000000,
        .period_ticks = 50000,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));
    TEST_ESP_OK(mcpwm_timer_enable(timer));

    printf("create operator\r\n");
    mcpwm_oper_handle_t oper = NULL;
    mcpwm_operator_config_t operator_config = {
        .group_id = 0,
        .flags.update_gen_action_on_tez = true,
    };
    TEST_ESP_OK(mcpwm_new_operator(&operator_config, &oper));
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));

    printf("create generator\r\n");
    mcpwm_gen_handle_t generator = NULL;
    const int gen_gpio = 0;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = gen_gpio,
        .flags.io_loop_back = true, // loop back for test
    };
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator));

    printf("add force level to the generator, and recovery by events");
    TEST_ESP_OK(mcpwm_generator_set_force_level(generator, 0, false));
    TEST_ASSERT_EQUAL(0, gpio_get_level(gen_gpio));
    TEST_ESP_OK(mcpwm_generator_set_force_level(generator, 1, false));
    TEST_ASSERT_EQUAL(1, gpio_get_level(gen_gpio));

    TEST_ESP_OK(mcpwm_generator_set_force_level(generator, 0, false));
    TEST_ASSERT_EQUAL(0, gpio_get_level(gen_gpio));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(generator,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    // generator should output high level on tez event, the previous force level should disappear
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(200));
    TEST_ASSERT_EQUAL(1, gpio_get_level(gen_gpio));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ESP_OK(mcpwm_generator_set_force_level(generator, 1, false));
    TEST_ASSERT_EQUAL(1, gpio_get_level(gen_gpio));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(generator,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    // generator should output low level on tez event, the previous force level should disappear
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(200));
    TEST_ASSERT_EQUAL(0, gpio_get_level(gen_gpio));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("delete generator, operator and timer\r\n");
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(generator));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}

TEST_CASE("mcpwm_generator_action_on_timer_event", "[mcpwm]")
{
    const int generator_gpio = 0;
    printf("create timer and operator\r\n");
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));
    TEST_ESP_OK(mcpwm_timer_enable(timer));

    mcpwm_operator_config_t oper_config = {
        .group_id = 0,
    };
    mcpwm_oper_handle_t oper = NULL;
    TEST_ESP_OK(mcpwm_new_operator(&oper_config, &oper));

    printf("connect timer and operator\r\n");
    TEST_ESP_OK(mcpwm_operator_connect_timer(oper, timer));

    printf("create generator\r\n");
    mcpwm_generator_config_t gen_config = {
        .gen_gpio_num = generator_gpio,
        .flags.io_loop_back = 1, // so that we can read the GPIO value by GPIO driver
    };
    mcpwm_gen_handle_t gen = NULL;
    TEST_ESP_OK(mcpwm_new_generator(oper, &gen_config, &gen));

    printf("set generator to output high on timer full\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_KEEP),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    printf("start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("stop timer on full\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_FULL));
    TEST_ASSERT_EQUAL(1, gpio_get_level(generator_gpio));

    printf("set generator to output low on timer full\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_KEEP),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    printf("start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("stop timer on full\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_FULL));
    TEST_ASSERT_EQUAL(0, gpio_get_level(generator_gpio));

    printf("delete timer, operator, generator\r\n");
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(gen));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}

typedef void (*set_gen_actions_cb_t)(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb);

static void mcpwm_gen_action_test_template(uint32_t timer_resolution, uint32_t period, mcpwm_timer_count_mode_t count_mode,
        uint32_t cmpa, uint32_t cmpb, int gpioa, int gpiob, set_gen_actions_cb_t set_generator_actions)
{
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .count_mode = count_mode,
        .resolution_hz = timer_resolution,
        .period_ticks = period,
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

    mcpwm_cmpr_handle_t comparator_a = NULL;
    mcpwm_cmpr_handle_t comparator_b = NULL;
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator_a));
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator_b));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator_a, cmpa));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator_b, cmpb));

    mcpwm_gen_handle_t generator_a = NULL;
    mcpwm_gen_handle_t generator_b = NULL;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = gpioa,
    };
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator_a));
    generator_config.gen_gpio_num = gpiob;
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator_b));

    set_generator_actions(generator_a, generator_b, comparator_a, comparator_b);

    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    vTaskDelay(pdMS_TO_TICKS(10));

    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(generator_a));
    TEST_ESP_OK(mcpwm_del_generator(generator_b));
    TEST_ESP_OK(mcpwm_del_comparator(comparator_a));
    TEST_ESP_OK(mcpwm_del_comparator(comparator_b));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}

static void single_edge_active_high(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(genb,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(genb,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void single_edge_active_low(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(genb,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(genb,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void pulse_placement(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(genb,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_TOGGLE),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
}

static void dual_edge_active_low_asym(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpb, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(genb,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
}

static void dual_edge_active_low_sym(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(genb,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpb, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void dual_edge_complementary(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(genb,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, cmpb, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

TEST_CASE("mcpwm_generator_action_on_compare_event", "[mcpwm]")
{
    printf("[Asymmetric, SingleEdge, ActiveHigh]\r\n");
    // PWMA: high = [1->350], low = [351->499,0]
    // PWMB: high = [1->200], low = [201->499,0]
    mcpwm_gen_action_test_template(1000000, 500, MCPWM_TIMER_COUNT_MODE_UP, 350, 200, 0, 2, single_edge_active_high);

    printf("[Asymmetric, SingleEdge, ActiveLow]\r\n");
    // PWMA: low = [0->300], high = [301->499]
    // PWMB: low = [0->150], high = [151->499]
    mcpwm_gen_action_test_template(1000000, 500, MCPWM_TIMER_COUNT_MODE_UP, 300, 150, 0, 2, single_edge_active_low);

    printf("[Asymmetric, PulsePlacement]\r\n");
    // PWMA: low = [0->200], high = [201->400], low = [401->599]
    // PWMB: high = [0->599], low = [0->599]
    mcpwm_gen_action_test_template(1000000, 600, MCPWM_TIMER_COUNT_MODE_UP, 200, 400, 0, 2, pulse_placement);

    printf("[Asymmetric, DualEdge, ActiveLow]\r\n");
    // PWMA: low = [0->250], high = [251->599, 600->450], low = [451->1]
    // PWMB: low = [0->599], low = [600->1]
    mcpwm_gen_action_test_template(1000000, 1200, MCPWM_TIMER_COUNT_MODE_UP_DOWN, 250, 450, 0, 2, dual_edge_active_low_asym);

    printf("[Symmetric, DualEdge, ActiveLow]\r\n");
    // PWMA: low = [0->400], high = [401->599, 600->400], low = [399->1]
    // PWMB: low = [0->500], high = [501->599, 600->500], low = [499->1]
    mcpwm_gen_action_test_template(1000000, 1200, MCPWM_TIMER_COUNT_MODE_UP_DOWN, 400, 500, 0, 2, dual_edge_active_low_sym);

    printf("[Symmetric, DualEdge, Complementary]\r\n");
    // PWMA: low = [0->350], high = [351->599, 600->350], low = [349->1]
    // PWMB: low = [0->400], high = [401->599, 600->400], low = [399->1]
    mcpwm_gen_action_test_template(1000000, 1200, MCPWM_TIMER_COUNT_MODE_UP_DOWN, 350, 400, 0, 2, dual_edge_complementary);
}

typedef void (*set_dead_time_cb_t)(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb);

static void mcpwm_deadtime_test_template(uint32_t timer_resolution, uint32_t period, uint32_t cmpa, uint32_t cmpb, int gpioa, int gpiob,
        set_gen_actions_cb_t set_generator_actions, set_dead_time_cb_t set_dead_time)
{
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = timer_resolution,
        .period_ticks = period,
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

    TEST_ESP_OK(mcpwm_timer_enable(timer));

    mcpwm_cmpr_handle_t comparator_a = NULL;
    mcpwm_cmpr_handle_t comparator_b = NULL;
    mcpwm_comparator_config_t comparator_config = {
        .flags.update_cmp_on_tez = true,
    };
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator_a));
    TEST_ESP_OK(mcpwm_new_comparator(oper, &comparator_config, &comparator_b));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator_a, cmpa));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator_b, cmpb));

    mcpwm_gen_handle_t generator_a = NULL;
    mcpwm_gen_handle_t generator_b = NULL;
    mcpwm_generator_config_t generator_config = {
        .gen_gpio_num = gpioa,
    };
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator_a));
    generator_config.gen_gpio_num = gpiob;
    TEST_ESP_OK(mcpwm_new_generator(oper, &generator_config, &generator_b));

    set_generator_actions(generator_a, generator_b, comparator_a, comparator_b);
    set_dead_time(generator_a, generator_b);

    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    vTaskDelay(pdMS_TO_TICKS(10));

    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(generator_a));
    TEST_ESP_OK(mcpwm_del_generator(generator_b));
    TEST_ESP_OK(mcpwm_del_comparator(comparator_a));
    TEST_ESP_OK(mcpwm_del_comparator(comparator_b));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}

static void ahc_set_generator_actions(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void ahc_set_dead_time(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb)
{
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 50,
        .negedge_delay_ticks = 0
    };
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = 100;
    dead_time_config.flags.invert_output = true;
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));
}

static void alc_set_generator_actions(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void alc_set_dead_time(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb)
{
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 50,
        .negedge_delay_ticks = 0,
        .flags.invert_output = true
    };
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = 100;
    dead_time_config.flags.invert_output = false;
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));
}

static void ah_set_generator_actions(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void ah_set_dead_time(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb)
{
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 50,
        .negedge_delay_ticks = 0,
    };
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = 100;
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));
}

static void al_set_generator_actions(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void al_set_dead_time(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb)
{
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 50,
        .negedge_delay_ticks = 0,
        .flags.invert_output = true
    };
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    dead_time_config.posedge_delay_ticks = 0;
    dead_time_config.negedge_delay_ticks = 100;
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, genb, &dead_time_config));
}

static void reda_only_set_generator_actions(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(genb,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(genb,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void reda_only_set_dead_time(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb)
{
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 50,
        .negedge_delay_ticks = 0,
    };
    // apply deadtime to generator_a
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    // bypass deadtime module for generator_b
    dead_time_config.posedge_delay_ticks = 0;
    TEST_ESP_OK(mcpwm_generator_set_dead_time(genb, genb, &dead_time_config));
}

static void fedb_only_set_generator_actions(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(genb,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(genb,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void fedb_only_set_dead_time(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb)
{
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 0,
        .negedge_delay_ticks = 0,
    };
    // generator_a bypass the deadtime module (no delay)
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    // apply dead time to generator_b
    dead_time_config.negedge_delay_ticks = 50;
    TEST_ESP_OK(mcpwm_generator_set_dead_time(genb, genb, &dead_time_config));

}

static void redfedb_only_set_generator_actions(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb, mcpwm_cmpr_handle_t cmpa, mcpwm_cmpr_handle_t cmpb)
{
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gena,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gena,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpa, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(genb,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(genb,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpb, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));
}

static void redfedb_only_set_dead_time(mcpwm_gen_handle_t gena, mcpwm_gen_handle_t genb)
{
    mcpwm_dead_time_config_t dead_time_config = {
        .posedge_delay_ticks = 0,
        .negedge_delay_ticks = 0,
    };
    // generator_a bypass the deadtime module (no delay)
    TEST_ESP_OK(mcpwm_generator_set_dead_time(gena, gena, &dead_time_config));
    // apply dead time on both edge for generator_b
    dead_time_config.negedge_delay_ticks = 50;
    dead_time_config.posedge_delay_ticks = 50;
    TEST_ESP_OK(mcpwm_generator_set_dead_time(genb, genb, &dead_time_config));
}

TEST_CASE("mcpwm_generator_deadtime_classical_configuration", "[mcpwm]")
{
    printf("Active High Complementary\r\n");
    mcpwm_deadtime_test_template(1000000, 600, 200, 400, 0, 2, ahc_set_generator_actions, ahc_set_dead_time);

    printf("Active Low Complementary\r\n");
    mcpwm_deadtime_test_template(1000000, 600, 200, 400, 0, 2, alc_set_generator_actions, alc_set_dead_time);

    printf("Active High\r\n");
    mcpwm_deadtime_test_template(1000000, 600, 200, 400, 0, 2, ah_set_generator_actions, ah_set_dead_time);

    printf("Active Low\r\n");
    mcpwm_deadtime_test_template(1000000, 600, 200, 400, 0, 2, al_set_generator_actions, al_set_dead_time);

    printf("RED on A, Bypass B\r\n");
    mcpwm_deadtime_test_template(1000000, 500, 350, 350, 0, 2, reda_only_set_generator_actions, reda_only_set_dead_time);

    printf("Bypass A, FED on B\r\n");
    mcpwm_deadtime_test_template(1000000, 500, 350, 350, 0, 2, fedb_only_set_generator_actions, fedb_only_set_dead_time);

    printf("Bypass A, RED + FED on B\r\n");
    mcpwm_deadtime_test_template(1000000, 500, 350, 350, 0, 2, redfedb_only_set_generator_actions, redfedb_only_set_dead_time);
}

TEST_CASE("mcpwm_duty_empty_full", "[mcpwm]")
{
    const int gen_gpio_num = 0;
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
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator, 0));

    printf("install MCPWM generator\r\n");
    mcpwm_generator_config_t gen_config = {
        .gen_gpio_num = gen_gpio_num,
        .flags.io_loop_back = true, // we want to read the output level as well
    };
    TEST_ESP_OK(mcpwm_new_generator(oper, &gen_config, &gen));

    printf("set generator actions on timer and compare events\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen,
                MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
                MCPWM_GEN_TIMER_EVENT_ACTION_END()));
    TEST_ESP_OK(mcpwm_generator_set_actions_on_compare_event(gen,
                MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW),
                MCPWM_GEN_COMPARE_EVENT_ACTION_END()));

    printf("start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_enable(timer));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));

    // check if the output is a const low level
    for (int i = 0; i < 100; i++) {
        TEST_ASSERT_EQUAL(0, gpio_get_level(gen_gpio_num));
        esp_rom_delay_us(1);
    }

    // set the compare equals to the period
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator, 50));
    vTaskDelay(pdMS_TO_TICKS(10));
    // so the output should be a const high level
    for (int i = 0; i < 100; i++) {
        TEST_ASSERT_EQUAL(1, gpio_get_level(gen_gpio_num));
        esp_rom_delay_us(1);
    }

    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator, 49));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(mcpwm_comparator_set_compare_value(comparator, 1));
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("uninstall timer, operator and comparator\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(gen));
    TEST_ESP_OK(mcpwm_del_comparator(comparator));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
}
