/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_attr.h"
#include "esp_clk_tree.h"
#include "driver/gpio.h"
#include "driver/mcpwm_prelude.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/esp_pmu.h"
#include "test_mcpwm_utils.h"

/**
 * @brief Test the MCPWM timer can still work after light sleep
 *
 * @param allow_pd Whether to allow power down the peripheral in light sleep
 */
static void test_mcpwm_timer_sleep_retention(bool allow_pd)
{
    const int generator_gpio = TEST_PWMA_GPIO;
    printf("init a gpio to read generator output\r\n");
    gpio_config_t gpio_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT(generator_gpio),
    };
    TEST_ESP_OK(gpio_config(&gpio_conf));

    printf("create timer and operator\r\n");
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = 1000,
        .flags.allow_pd = allow_pd,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));

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
        .flags.io_loop_back = true,
    };
    mcpwm_gen_handle_t gen = NULL;
    TEST_ESP_OK(mcpwm_new_generator(oper, &gen_config, &gen));

    printf("set generator to output high on timer full\r\n");
    TEST_ESP_OK(mcpwm_generator_set_actions_on_timer_event(gen,
                                                           MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_FULL, MCPWM_GEN_ACTION_HIGH),
                                                           MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_KEEP),
                                                           MCPWM_GEN_TIMER_EVENT_ACTION_END()));

    // go to sleep
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 1 seconds\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if MCPWM driver can still work...\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

    printf("enable timer\r\n");
    TEST_ESP_OK(mcpwm_timer_enable(timer));
    printf("start timer\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("stop timer on full\r\n");
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_FULL));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL(1, gpio_get_level(generator_gpio));

    printf("delete timer, operator, generator\r\n");
    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_generator(gen));
    TEST_ESP_OK(mcpwm_del_operator(oper));
    TEST_ESP_OK(mcpwm_del_timer(timer));
    TEST_ESP_OK(gpio_reset_pin(generator_gpio));
}

TEST_CASE("mcpwm_timer_sleep_retention", "[mcpwm]")
{
    test_mcpwm_timer_sleep_retention(false);
#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
    test_mcpwm_timer_sleep_retention(true);
#endif
}

TEST_MCPWM_CALLBACK_ATTR
static bool test_capture_callback(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    uint32_t *cap_value = (uint32_t *)user_data;
    if (edata->cap_edge == MCPWM_CAP_EDGE_NEG) {
        cap_value[1] = edata->cap_value;
    } else {
        cap_value[0] = edata->cap_value;
    }
    return false;
}

/**
 * @brief Test the MCPWM capture timer can still work after light sleep
 *
 * @param allow_pd Whether to allow power down the peripheral in light sleep
 */
static void test_mcpwm_capture_timer_sleep_retention(bool allow_pd)
{
    const int cap_gpio = TEST_CAP_GPIO;
    printf("init a gpio to simulate the external capture signal\r\n");
    gpio_config_t gpio_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT(cap_gpio),
    };
    TEST_ESP_OK(gpio_config(&gpio_conf));
    // reset the gpio
    TEST_ESP_OK(gpio_set_level(cap_gpio, 0));
    // install mcpwm capture timer
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    uint32_t clk_src_freq_hz;
    esp_clk_tree_src_get_freq_hz(MCPWM_CAPTURE_CLK_SRC_DEFAULT, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &clk_src_freq_hz);
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = clk_src_freq_hz / 2,
        .flags.allow_pd = allow_pd,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));
    // install mcpwm capture channel
    mcpwm_cap_channel_handle_t cap_channel;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = cap_gpio,
        .prescale = 1,
        .flags.pos_edge = true,
        .flags.neg_edge = true,
    };
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &cap_channel));

    // install callback for capture channel
    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = test_capture_callback,
    };
    uint32_t cap_value[2] = {0};
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(cap_channel, &cbs, cap_value));

    // go to sleep
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 1 seconds\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());

    printf("Waked up! Let's see if MCPWM driver can still work...\r\n");
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif

    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);

    printf("enable capture\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_enable(cap_channel));
    TEST_ESP_OK(mcpwm_capture_timer_enable(cap_timer));

    printf("simulate GPIO capture signal\r\n");
    TEST_ESP_OK(mcpwm_capture_timer_start(cap_timer));
    TEST_ESP_OK(gpio_set_level(cap_gpio, 1));
    esp_rom_delay_us(10 * 1000);
    TEST_ESP_OK(gpio_set_level(cap_gpio, 0));

    uint32_t clk_src_res;
    TEST_ESP_OK(mcpwm_capture_timer_get_resolution(cap_timer, &clk_src_res));
    clk_src_res /= 1000; // convert to kHz
    uint32_t capture_value = (cap_value[1] - cap_value[0]) * 1000 / clk_src_res;
    TEST_ASSERT_UINT_WITHIN(1000, 10000, capture_value);

    printf("delete timer, operator, generator\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_disable(cap_channel));
    TEST_ESP_OK(mcpwm_del_capture_channel(cap_channel));
    TEST_ESP_OK(mcpwm_capture_timer_disable(cap_timer));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
    TEST_ESP_OK(gpio_reset_pin(cap_gpio));
}

TEST_CASE("mcpwm_capture_timer_sleep_retention", "[mcpwm]")
{
    test_mcpwm_capture_timer_sleep_retention(false);
#if SOC_MCPWM_SUPPORT_SLEEP_RETENTION
    test_mcpwm_capture_timer_sleep_retention(true);
#endif
}
