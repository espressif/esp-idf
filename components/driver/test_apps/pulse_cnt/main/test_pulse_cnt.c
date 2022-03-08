/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"

#define TEST_PCNT_GPIO_A 0
#define TEST_PCNT_GPIO_B 2

#if CONFIG_PCNT_ISR_IRAM_SAFE
#define TEST_PCNT_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_PCNT_CALLBACK_ATTR
#endif // CONFIG_PCNT_ISR_IRAM_SAFE

// helper function to simulate several rising edges on gpio
static void test_gpio_simulate_rising_edge(int gpio_sig, size_t times)
{
    while (times--) {
        TEST_ESP_OK(gpio_set_level(gpio_sig, 0));
        TEST_ESP_OK(gpio_set_level(gpio_sig, 1));
    }
}

// helper function to simulate several groups of quadrature signals
static void test_gpio_simulate_quadrature_signals(int gpio_sig_a, int gpio_sig_b, size_t times)
{
    while (times--) {
        TEST_ESP_OK(gpio_set_level(gpio_sig_a, 1));
        TEST_ESP_OK(gpio_set_level(gpio_sig_b, 0));
        vTaskDelay(1);
        TEST_ESP_OK(gpio_set_level(gpio_sig_a, 0));
        TEST_ESP_OK(gpio_set_level(gpio_sig_b, 0));
        vTaskDelay(1);
        TEST_ESP_OK(gpio_set_level(gpio_sig_a, 0));
        TEST_ESP_OK(gpio_set_level(gpio_sig_b, 1));
        vTaskDelay(1);
        TEST_ESP_OK(gpio_set_level(gpio_sig_a, 1));
        TEST_ESP_OK(gpio_set_level(gpio_sig_b, 1));
        vTaskDelay(1);
    }
}

TEST_CASE("pcnt_unit_install_uninstall", "[pcnt]")
{
    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
    };
    pcnt_unit_handle_t units[SOC_PCNT_UNITS_PER_GROUP];
    int count_value = 0;

    printf("install pcnt units and check initial count\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_new_unit(&unit_config, &units[i]));
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        TEST_ASSERT_EQUAL(0, count_value);
    }
    // no more free pcnt units
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, pcnt_new_unit(&unit_config, &units[0]));

    printf("set glitch filter\r\n");
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_set_glitch_filter(units[i], &filter_config));
    }
    // invalid glitch configuration
    filter_config.max_glitch_ns = 500000;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, pcnt_unit_set_glitch_filter(units[0], &filter_config));

    printf("start pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_start(units[i]));
    }
    // can't uninstall unit before stop it
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_del_unit(units[0]));

    printf("stop pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_stop(units[i]));
    }

    printf("uninstall pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_del_unit(units[i]));
    }
}

TEST_CASE("pcnt_channel_install_uninstall", "[pcnt]")
{
    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
    };
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = TEST_PCNT_GPIO_A, // only detect edge signal in this case
        .level_gpio_num = -1,
        .flags.io_loop_back = true,
    };
    pcnt_unit_handle_t units[SOC_PCNT_UNITS_PER_GROUP];
    pcnt_channel_handle_t chans[SOC_PCNT_UNITS_PER_GROUP][SOC_PCNT_CHANNELS_PER_UNIT];

    printf("install pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_new_unit(&unit_config, &units[i]));
    }

    printf("install pcnt channels\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        for (int j = 0; j < SOC_PCNT_CHANNELS_PER_UNIT; j++) {
            TEST_ESP_OK(pcnt_new_channel(units[i], &chan_config, &chans[i][j]));
            TEST_ESP_OK(pcnt_channel_set_edge_action(chans[i][j], PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
            TEST_ESP_OK(pcnt_channel_set_level_action(chans[i][j], PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
        }
        TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, pcnt_new_channel(units[i], &chan_config, &chans[i][0]));
    }

    printf("start units\r\n");
    int count_value = 0;
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        // start unit
        TEST_ESP_OK(pcnt_unit_start(units[i]));
        // trigger 10 rising edge on GPIO0
        test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        // each channel increases to the same unit counter
        TEST_ASSERT_EQUAL(10 * SOC_PCNT_CHANNELS_PER_UNIT, count_value);
    }

    printf("clear counts\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_clear_count(units[i]));
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        TEST_ASSERT_EQUAL(0, count_value);
    }

    printf("stop unit\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        // stop unit
        TEST_ESP_OK(pcnt_unit_stop(units[i]));
    }

    // trigger 10 rising edge on GPIO0 shouldn't increase the counter
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        TEST_ASSERT_EQUAL(0, count_value);
    }

    printf("restart units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        // start unit
        TEST_ESP_OK(pcnt_unit_start(units[i]));
        // trigger 10 rising edge on GPIO
        test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        // each channel increases to the same unit counter
        TEST_ASSERT_EQUAL(10 * SOC_PCNT_CHANNELS_PER_UNIT, count_value);
    }

    printf("uninstall channels and units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        // stop unit
        TEST_ESP_OK(pcnt_unit_stop(units[i]));
        // can't uninstall unit when channel is still alive
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_del_unit(units[i]));
        for (int j = 0; j < SOC_PCNT_CHANNELS_PER_UNIT; j++) {
            TEST_ESP_OK(pcnt_del_channel(chans[i][j]));
        }
        TEST_ESP_OK(pcnt_del_unit(units[i]));
    }
}

/**
 * @brief Using this context to save the triggered watchpoints in sequence
 */
typedef struct {
    uint32_t index;
    int triggered_watch_values[8];
} test_pcnt_quadrature_context_t;

TEST_PCNT_CALLBACK_ATTR
static bool test_pcnt_quadrature_reach_watch_point(pcnt_unit_handle_t handle, pcnt_watch_event_data_t *event_data, void *user_data)
{
    test_pcnt_quadrature_context_t *user_ctx = (test_pcnt_quadrature_context_t *)user_data;
    user_ctx->triggered_watch_values[user_ctx->index++] = event_data->watch_point_value;
    return false;
}

TEST_CASE("pcnt_quadrature_decode_event", "[pcnt]")
{
    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100
    };

    printf("install pcnt unit\r\n");
    pcnt_unit_handle_t unit = NULL;
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &unit));
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    TEST_ESP_OK(pcnt_unit_set_glitch_filter(unit, &filter_config));

    printf("install two pcnt channels with different edge/level action\r\n");
    pcnt_chan_config_t channel_config = {
        .edge_gpio_num = TEST_PCNT_GPIO_A,
        .level_gpio_num = TEST_PCNT_GPIO_B,
        .flags.io_loop_back = true,
    };
    pcnt_channel_handle_t channelA = NULL;
    TEST_ESP_OK(pcnt_new_channel(unit, &channel_config, &channelA));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE));
    TEST_ESP_OK(pcnt_channel_set_level_action(channelA, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
    // switch edge gpio and level gpio, the assign to another channel in the same unit
    pcnt_channel_handle_t channelB = NULL;
    channel_config.edge_gpio_num = TEST_PCNT_GPIO_B;
    channel_config.level_gpio_num = TEST_PCNT_GPIO_A;
    TEST_ESP_OK(pcnt_new_channel(unit, &channel_config, &channelB));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelB, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE));
    TEST_ESP_OK(pcnt_channel_set_level_action(channelB, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    // ensure the simulation signal in a stable state
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_A, 1));
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_B, 1));

    pcnt_event_callbacks_t cbs = {
        .on_reach = test_pcnt_quadrature_reach_watch_point,
    };
    test_pcnt_quadrature_context_t user_data = {
        .index = 0,
        .triggered_watch_values = {}
    };
    TEST_ESP_OK(pcnt_unit_register_event_callbacks(unit, &cbs, &user_data));

    printf("add watchpoints\r\n");
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 0));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 100));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, -100));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 50));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, -50));
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, pcnt_unit_add_watch_point(unit, 33));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_add_watch_point(unit, 50));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_add_watch_point(unit, 100));

    // Clear internal counter, and make the watch points take effect
    TEST_ESP_OK(pcnt_unit_clear_count(unit));
    TEST_ESP_OK(pcnt_unit_start(unit));

    printf("simulating quadrature signals\r\n");
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B, 30);
    // simply wait for done
    vTaskDelay(pdMS_TO_TICKS(100));

    int count_value;
    printf("checking count value\r\n");
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("count_value=%d\r\n", count_value);
    TEST_ASSERT_EQUAL(-20, count_value); // 0-30*4+100
    TEST_ASSERT_EQUAL(3, user_data.index);
    TEST_ASSERT_EQUAL(-50, user_data.triggered_watch_values[0]);
    TEST_ASSERT_EQUAL(-100, user_data.triggered_watch_values[1]);
    TEST_ASSERT_EQUAL(0, user_data.triggered_watch_values[2]);

    printf("simulating quadrature signals in another direction\r\n");
    user_data.index = 0;
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_B, TEST_PCNT_GPIO_A, 40);
    // simply wait for done
    vTaskDelay(pdMS_TO_TICKS(100));
    printf("checking count value\r\n");
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("count_value=%d\r\n", count_value);
    TEST_ASSERT_EQUAL(40, count_value); // -20+40*4-100
    TEST_ASSERT_EQUAL(4, user_data.index);
    TEST_ASSERT_EQUAL(0, user_data.triggered_watch_values[0]);
    TEST_ASSERT_EQUAL(50, user_data.triggered_watch_values[1]);
    TEST_ASSERT_EQUAL(100, user_data.triggered_watch_values[2]);
    TEST_ASSERT_EQUAL(0, user_data.triggered_watch_values[3]);

    printf("remove watchpoints and uninstall channels\r\n");
    TEST_ESP_OK(pcnt_unit_remove_watch_point(unit, 0));
    TEST_ESP_OK(pcnt_unit_remove_watch_point(unit, 100));
    TEST_ESP_OK(pcnt_unit_remove_watch_point(unit, -100));
    TEST_ESP_OK(pcnt_unit_remove_watch_point(unit, 50));
    TEST_ESP_OK(pcnt_unit_remove_watch_point(unit, -50));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_remove_watch_point(unit, 50));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_remove_watch_point(unit, 33));
    TEST_ESP_OK(pcnt_del_channel(channelA));
    TEST_ESP_OK(pcnt_del_channel(channelB));
    TEST_ESP_OK(pcnt_unit_stop(unit));
    TEST_ESP_OK(pcnt_del_unit(unit));
}

typedef struct {
    pcnt_unit_zero_cross_mode_t mode;
} test_pcnt_zero_cross_context_t;

TEST_PCNT_CALLBACK_ATTR
static bool test_pcnt_on_zero_cross(pcnt_unit_handle_t handle, pcnt_watch_event_data_t *event_data, void *user_data)
{
    test_pcnt_zero_cross_context_t *user_ctx = (test_pcnt_zero_cross_context_t *)user_data;
    user_ctx->mode = event_data->zero_cross_mode;
    return false;
}

TEST_CASE("pcnt_zero_cross_mode", "[pcnt]")
{
    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100
    };

    printf("install pcnt unit\r\n");
    pcnt_unit_handle_t unit = NULL;
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &unit));

    printf("add watchpoint to detect zero cross\r\n");
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 0));

    printf("register callback for zero cross event\r\n");
    pcnt_event_callbacks_t cbs = {
        .on_reach = test_pcnt_on_zero_cross,
    };
    test_pcnt_zero_cross_context_t user_data = {};
    TEST_ESP_OK(pcnt_unit_register_event_callbacks(unit, &cbs, &user_data));

    printf("install pcnt channels\r\n");
    pcnt_chan_config_t channel_config = {
        .edge_gpio_num = TEST_PCNT_GPIO_A,
        .level_gpio_num = -1,
        .flags.io_loop_back = true,
    };
    pcnt_channel_handle_t channelA = NULL;
    pcnt_channel_handle_t channelB = NULL;
    TEST_ESP_OK(pcnt_new_channel(unit, &channel_config, &channelA));
    TEST_ESP_OK(pcnt_new_channel(unit, &channel_config, &channelB));

    printf("Initialize pcnt actions for channels\r\n");
    // only channel will increase the counter, 0->1
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(channelA, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelB, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(channelB, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    printf("start unit\r\n");
    TEST_ESP_OK(pcnt_unit_start(unit));

    int count_value = 0;
    printf("counter goes 0->1\r\n");
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 1);
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    TEST_ASSERT_EQUAL(1, count_value);

    printf("counter goes 1->-1\r\n");
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelB, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 1);
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    TEST_ASSERT_EQUAL(-1, count_value);
    TEST_ASSERT_EQUAL(PCNT_UNIT_ZERO_CROSS_POS_NEG, user_data.mode);

    printf("counter goes -1->1\r\n");
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelB, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 1);
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    TEST_ASSERT_EQUAL(1, count_value);
    TEST_ASSERT_EQUAL(PCNT_UNIT_ZERO_CROSS_NEG_POS, user_data.mode);

    printf("counter goes 1->0->-1\r\n");
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelB, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 2);
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    TEST_ASSERT_EQUAL(-1, count_value);
    TEST_ASSERT_EQUAL(PCNT_UNIT_ZERO_CROSS_POS_ZERO, user_data.mode);

    printf("counter goes -1->0->1\r\n");
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelA, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channelB, PCNT_CHANNEL_EDGE_ACTION_HOLD, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 2);
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    TEST_ASSERT_EQUAL(1, count_value);
    TEST_ASSERT_EQUAL(PCNT_UNIT_ZERO_CROSS_NEG_ZERO, user_data.mode);

    TEST_ESP_OK(pcnt_unit_stop(unit));
    TEST_ESP_OK(pcnt_unit_remove_watch_point(unit, 0));
    TEST_ESP_OK(pcnt_del_channel(channelA));
    TEST_ESP_OK(pcnt_del_channel(channelB));
    TEST_ESP_OK(pcnt_del_unit(unit));
}

void test_app_include_pulse_cnt(void)
{
}
