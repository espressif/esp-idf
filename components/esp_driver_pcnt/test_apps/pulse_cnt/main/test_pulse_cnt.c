/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "test_pulse_cnt_board.h"

TEST_CASE("pcnt_unit_install_uninstall", "[pcnt]")
{
    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
        .intr_priority = 0,
    };
    pcnt_unit_handle_t units[SOC_PCNT_UNITS_PER_GROUP];
    int count_value = 0;

    printf("install pcnt units and check initial count\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP - 1; i++) {
        TEST_ESP_OK(pcnt_new_unit(&unit_config, &units[i]));
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        TEST_ASSERT_EQUAL(0, count_value);
    }

    // unit with a different interrupt priority
    unit_config.intr_priority = 3;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, pcnt_new_unit(&unit_config, &units[SOC_PCNT_UNITS_PER_GROUP - 1]));
    unit_config.intr_priority = 0;
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &units[SOC_PCNT_UNITS_PER_GROUP - 1]));

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
    pcnt_event_callbacks_t cbs = {
        .on_reach = NULL,
    };
    printf("enable pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_register_event_callbacks(units[i], &cbs, NULL));
        TEST_ESP_OK(pcnt_unit_enable(units[i]));
    }

    printf("start pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_start(units[i]));
    }

    printf("stop pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_stop(units[i]));
    }

    // can't uninstall unit before disable it
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_del_unit(units[0]));
    printf("disable pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_unit_disable(units[i]));
    }

    printf("uninstall pcnt units\r\n");
    for (int i = 0; i < SOC_PCNT_UNITS_PER_GROUP; i++) {
        TEST_ESP_OK(pcnt_del_unit(units[i]));
    }
}

TEST_CASE("pcnt_channel_install_uninstall", "[pcnt]")
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);

    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
    };
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = TEST_PCNT_GPIO_A, // only detect edge signal in this case
        .level_gpio_num = -1,
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
        TEST_ESP_OK(pcnt_unit_enable(units[i]));
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
        TEST_ESP_OK(pcnt_unit_disable(units[i]));
        // can't uninstall unit when channel is still alive
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_del_unit(units[i]));
        for (int j = 0; j < SOC_PCNT_CHANNELS_PER_UNIT; j++) {
            TEST_ESP_OK(pcnt_del_channel(chans[i][j]));
        }
        TEST_ESP_OK(pcnt_del_unit(units[i]));
    }
}

TEST_CASE("pcnt_multiple_units_pulse_count", "[pcnt]")
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_B);

    printf("install pcnt units\r\n");
    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
    };
    pcnt_unit_handle_t units[2];
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_new_unit(&unit_config, &units[i]));
    }

    printf("install pcnt channels\r\n");
    const int channel_gpios[] = {TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B};
    pcnt_chan_config_t chan_config = {
        .level_gpio_num = -1,
    };
    pcnt_channel_handle_t chans[2];
    for (int i = 0; i < 2; i++) {
        chan_config.edge_gpio_num = channel_gpios[i];
        TEST_ESP_OK(pcnt_new_channel(units[i], &chan_config, &chans[i]));
        TEST_ESP_OK(pcnt_channel_set_edge_action(chans[i], PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
        TEST_ESP_OK(pcnt_channel_set_level_action(chans[i], PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    }

    printf("enable and start unit\r\n");
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_unit_enable(units[i]));
        TEST_ESP_OK(pcnt_unit_start(units[i]));
    }

    // trigger 10 rising edge on GPIO
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_B, 10);

    int count_value = 0;
    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_unit_get_count(units[i], &count_value));
        TEST_ASSERT_EQUAL(10, count_value);
    }

    for (int i = 0; i < 2; i++) {
        TEST_ESP_OK(pcnt_unit_stop(units[i]));
        TEST_ESP_OK(pcnt_unit_disable(units[i]));
        TEST_ESP_OK(pcnt_del_channel(chans[i]));
        TEST_ESP_OK(pcnt_del_unit(units[i]));
    }
}

/**
 * @brief Using this context to save the triggered watch-points in sequence
 */
typedef struct {
    uint32_t index;
    int triggered_watch_values[8];
} test_pcnt_quadrature_context_t;

TEST_PCNT_CALLBACK_ATTR
static bool test_pcnt_quadrature_reach_watch_point(pcnt_unit_handle_t handle, const pcnt_watch_event_data_t *event_data, void *user_data)
{
    test_pcnt_quadrature_context_t *user_ctx = (test_pcnt_quadrature_context_t *)user_data;
    user_ctx->triggered_watch_values[user_ctx->index++] = event_data->watch_point_value;
    return false;
}

TEST_CASE("pcnt_quadrature_decode_event", "[pcnt]")
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_B);

    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
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

    printf("add watch points\r\n");
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 0));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 100));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, -100));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 50));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, -50));
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, pcnt_unit_add_watch_point(unit, 33));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_add_watch_point(unit, 50));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_add_watch_point(unit, 100));

#if !SOC_PCNT_SUPPORT_RUNTIME_THRES_UPDATE
    // the above added watch point won't take effect at once, unless we clear the internal counter manually
    TEST_ESP_OK(pcnt_unit_clear_count(unit));
#endif

    // start unit should fail if it's not enabled yet
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_start(unit));
    TEST_ESP_OK(pcnt_unit_enable(unit));
    TEST_ESP_OK(pcnt_unit_start(unit));

    printf("simulating quadrature signals\r\n");
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B, 30);
    // simply wait for done
    vTaskDelay(pdMS_TO_TICKS(100));

    int count_value;
    printf("checking count value\r\n");
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("counter stopped at %d\r\n", count_value);
    for (int i = 0 ; i < user_data.index; i++) {
        printf("%d:%d\r\n", i, user_data.triggered_watch_values[i]);
    }
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
    printf("counter stopped at %d\r\n", count_value);
    for (int i = 0 ; i < user_data.index; i++) {
        printf("%d:%d\r\n", i, user_data.triggered_watch_values[i]);
    }
    TEST_ASSERT_EQUAL(40, count_value); // -20+40*4-100
    TEST_ASSERT_EQUAL(4, user_data.index);
    TEST_ASSERT_EQUAL(0, user_data.triggered_watch_values[0]);
    TEST_ASSERT_EQUAL(50, user_data.triggered_watch_values[1]);
    TEST_ASSERT_EQUAL(100, user_data.triggered_watch_values[2]);
    TEST_ASSERT_EQUAL(0, user_data.triggered_watch_values[3]);

    printf("remove watch points and uninstall channels\r\n");
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
    TEST_ESP_OK(pcnt_unit_disable(unit));
    TEST_ESP_OK(pcnt_del_unit(unit));
}

typedef struct {
    pcnt_unit_zero_cross_mode_t mode;
} test_pcnt_zero_cross_context_t;

TEST_PCNT_CALLBACK_ATTR
static bool test_pcnt_on_zero_cross(pcnt_unit_handle_t handle, const pcnt_watch_event_data_t *event_data, void *user_data)
{
    test_pcnt_zero_cross_context_t *user_ctx = (test_pcnt_zero_cross_context_t *)user_data;
    user_ctx->mode = event_data->zero_cross_mode;
    return false;
}

TEST_CASE("pcnt_zero_cross_mode", "[pcnt]")
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);

    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
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

    printf("enable unit\r\n");
    TEST_ESP_OK(pcnt_unit_enable(unit));

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
    TEST_ESP_OK(pcnt_unit_disable(unit));
    TEST_ESP_OK(pcnt_unit_remove_watch_point(unit, 0));
    TEST_ESP_OK(pcnt_del_channel(channelA));
    TEST_ESP_OK(pcnt_del_channel(channelB));
    TEST_ESP_OK(pcnt_del_unit(unit));
}

TEST_CASE("pcnt_virtual_io", "[pcnt]")
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);

    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
    };
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = TEST_PCNT_GPIO_A, // only detect edge signal in this case
        .level_gpio_num = -1,              // level signal is connected to a virtual IO internally
        .flags.virt_level_io_level = 1,    // the level input is connected to high level, internally
    };
    pcnt_unit_handle_t unit = NULL;
    pcnt_channel_handle_t chan = NULL;

    printf("install pcnt unit\r\n");
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &unit));

    printf("install pcnt channel\r\n");
    TEST_ESP_OK(pcnt_new_channel(unit, &chan_config, &chan));
    TEST_ESP_OK(pcnt_channel_set_edge_action(chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_unit_enable(unit));

    int count_value = 0;
    printf("start units\r\n");
    // start unit
    TEST_ESP_OK(pcnt_unit_start(unit));

    // trigger 10 rising edge on GPIO
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    TEST_ASSERT_EQUAL(10, count_value);

    printf("update level action\r\n");
    // the counter will hold-on (i.e. freeze) if the level input is high level (which is obviously yes in this case)
    TEST_ESP_OK(pcnt_channel_set_level_action(chan, PCNT_CHANNEL_LEVEL_ACTION_HOLD, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_unit_clear_count(unit));

    // trigger 10 rising edge on GPIO
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    // the count value should still be zero, because the level signal is high level, and the high level action is to hold-on the count value
    TEST_ASSERT_EQUAL(0, count_value);

    TEST_ESP_OK(pcnt_unit_stop(unit));
    TEST_ESP_OK(pcnt_unit_disable(unit));
    TEST_ESP_OK(pcnt_del_channel(chan));
    TEST_ESP_OK(pcnt_del_unit(unit));
}

#if SOC_PCNT_SUPPORT_CLEAR_SIGNAL
TEST_CASE("pcnt_zero_input_signal", "[pcnt]")
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_Z);

    pcnt_unit_config_t unit_config = {
        .low_limit = -1000,
        .high_limit = 1000,
    };

    printf("install pcnt unit\r\n");
    pcnt_unit_handle_t unit = NULL;
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &unit));

    pcnt_clear_signal_config_t clear_signal_config = {
        .clear_signal_gpio_num = TEST_PCNT_GPIO_Z,
    };

    TEST_ESP_OK(pcnt_unit_set_clear_signal(unit, &clear_signal_config));

    printf("install pcnt channels\r\n");
    pcnt_chan_config_t chan_config = {
        .level_gpio_num = -1,
        .edge_gpio_num = TEST_PCNT_GPIO_A,
    };
    pcnt_channel_handle_t channel;

    TEST_ESP_OK(pcnt_new_channel(unit, &chan_config, &channel));
    TEST_ESP_OK(pcnt_channel_set_edge_action(channel, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(channel, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    printf("enable and start unit\r\n");

    TEST_ESP_OK(pcnt_unit_enable(unit));
    TEST_ESP_OK(pcnt_unit_start(unit));
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_Z, 0));

    // trigger 10 rising edge on GPIO
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);

    int count_value = 0;

    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("count_value=%d\r\n", count_value);
    TEST_ASSERT_EQUAL(10, count_value);

    printf("simulating zero input signal\r\n");
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_Z, 1));
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_Z, 0));

    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("count_value=%d\r\n", count_value);
    TEST_ASSERT_EQUAL(0, count_value); // 0 after zero signal

    printf("remove zero signal\r\n");
    TEST_ESP_OK(pcnt_unit_set_clear_signal(unit, NULL));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, pcnt_unit_set_clear_signal(unit, NULL));

    // trigger 10 rising edge on GPIO
    test_gpio_simulate_rising_edge(TEST_PCNT_GPIO_A, 10);

    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("count_value=%d\r\n", count_value);
    TEST_ASSERT_EQUAL(10, count_value);

    printf("simulating zero input signal\r\n");
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_Z, 1));
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_Z, 0));

    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("count_value=%d\r\n", count_value);
    TEST_ASSERT_EQUAL(10, count_value); // 10 with no zero signal

    TEST_ESP_OK(pcnt_del_channel(channel));
    TEST_ESP_OK(pcnt_unit_stop(unit));
    TEST_ESP_OK(pcnt_unit_disable(unit));
    TEST_ESP_OK(pcnt_del_unit(unit));
}
#endif // SOC_PCNT_SUPPORT_CLEAR_SIGNAL

TEST_CASE("pcnt overflow accumulation", "[pcnt]")
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_B);

    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
        .flags = {
            .accum_count = true,
#if SOC_PCNT_SUPPORT_STEP_NOTIFY
            .en_step_notify_down = true,
#endif
        },
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

    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, -100));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 100));

    // ensure the simulation signal in a stable state
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_A, 1));
    TEST_ESP_OK(gpio_set_level(TEST_PCNT_GPIO_B, 1));

    TEST_ESP_OK(pcnt_unit_enable(unit));
    TEST_ESP_OK(pcnt_unit_start(unit));

    printf("simulating quadrature signals and count down\r\n");
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B, 50); // 50*(-4) = -200

    int count_value;
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("counter stopped at %d\r\n", count_value);
    TEST_ASSERT_EQUAL(-200, count_value);

    printf("simulating quadrature signals and count up\r\n");
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_B, TEST_PCNT_GPIO_A, 31); // -200+31*4 = -76
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("counter stopped at %d\r\n", count_value);
    TEST_ASSERT_EQUAL(-76, count_value);

    printf("simulating quadrature signals and count down again\r\n");
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B, 20); // -76-20*4 = -156
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("counter stopped at %d\r\n", count_value);
    TEST_ASSERT_EQUAL(-156, count_value);

    TEST_ESP_OK(pcnt_del_channel(channelA));
    TEST_ESP_OK(pcnt_del_channel(channelB));
    TEST_ESP_OK(pcnt_unit_stop(unit));
    TEST_ESP_OK(pcnt_unit_disable(unit));
    TEST_ESP_OK(pcnt_del_unit(unit));
}

#if SOC_PCNT_SUPPORT_STEP_NOTIFY
TEST_CASE("pcnt_step_notify_event", "[pcnt]")
{
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_A);
    test_gpio_init_for_simulation(TEST_PCNT_GPIO_B);

    pcnt_unit_config_t unit_config = {
        .low_limit = -100,
        .high_limit = 100,
        .flags = {
            .en_step_notify_down = true,
        },
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
        .triggered_watch_values = {0},
    };
    TEST_ESP_OK(pcnt_unit_register_event_callbacks(unit, &cbs, &user_data));

    printf("add watch step and point\r\n");
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, pcnt_unit_add_watch_step(unit, 0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, pcnt_unit_add_watch_step(unit, 20));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, pcnt_unit_add_watch_step(unit, -120));
    TEST_ESP_OK(pcnt_unit_add_watch_step(unit, -25));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_add_watch_step(unit, -100));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, -100));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 0));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, -50));
    TEST_ESP_OK(pcnt_unit_add_watch_point(unit, 11));

    TEST_ESP_OK(pcnt_unit_enable(unit));
    TEST_ESP_OK(pcnt_unit_start(unit));

    printf("simulating quadrature signals and count down\r\n");
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B, 25); // 25*(-4) = -100 -> 0

    int count_value;
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("counter stopped at %d\r\n", count_value);

    for (int i = 0 ; i < user_data.index; i++) {
        printf("%d:%d\r\n", i, user_data.triggered_watch_values[i]);
    }
    TEST_ASSERT_EQUAL(0, count_value);
    TEST_ASSERT_EQUAL(5, user_data.index);
    TEST_ASSERT_EQUAL(-25, user_data.triggered_watch_values[0]); // step point (-25*1)
    TEST_ASSERT_EQUAL(-50, user_data.triggered_watch_values[1]); // step point && watch point
    TEST_ASSERT_EQUAL(-75, user_data.triggered_watch_values[2]); // step point (-25*3)
    TEST_ASSERT_EQUAL(-100, user_data.triggered_watch_values[3]);// step point && watch point
    TEST_ASSERT_EQUAL(0, user_data.triggered_watch_values[4]);   // watch point (overflow zero cross)

    printf("simulating quadrature signals and count up\r\n");
    user_data.index = 0;
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_B, TEST_PCNT_GPIO_A, 3); // 0+3*4 = 12
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("counter stopped at %d\r\n", count_value);
    for (int i = 0 ; i < user_data.index; i++) {
        printf("%d:%d\r\n", i, user_data.triggered_watch_values[i]);
    }
    TEST_ASSERT_EQUAL(12, count_value);
    TEST_ASSERT_EQUAL(1, user_data.index);
    TEST_ASSERT_EQUAL(11, user_data.triggered_watch_values[0]); // watch point

    printf("simulating quadrature signals and count down again\r\n");
    user_data.index = 0;
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B, 13); // 12-13*4 = -40
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("counter stopped at %d\r\n", count_value);
    for (int i = 0 ; i < user_data.index; i++) {
        printf("%d:%d\r\n", i, user_data.triggered_watch_values[i]);
    }
    TEST_ASSERT_EQUAL(-40, count_value);
    TEST_ASSERT_EQUAL(3, user_data.index);
    TEST_ASSERT_EQUAL(11, user_data.triggered_watch_values[0]); // watch point
    TEST_ASSERT_EQUAL(0, user_data.triggered_watch_values[1]);  // watch point (zero cross)
    TEST_ASSERT_EQUAL(-25, user_data.triggered_watch_values[2]);// step point (-25*1)

    // before change step interval, the next step point should be -25-25=-50
    printf("change step interval\r\n");
    TEST_ESP_OK(pcnt_unit_remove_watch_step(unit));
    TEST_ESP_OK(pcnt_unit_add_watch_step(unit, -20));
    // but after change, the next step point is -25-20=-45 (-25 is the last active step point)

    printf("simulating quadrature signals and count down\r\n");
    user_data.index = 0;
    test_gpio_simulate_quadrature_signals(TEST_PCNT_GPIO_A, TEST_PCNT_GPIO_B, 20); // -40+20*(-4) = -120 -> -20
    TEST_ESP_OK(pcnt_unit_get_count(unit, &count_value));
    printf("counter stopped at %d\r\n", count_value);

    for (int i = 0 ; i < user_data.index; i++) {
        printf("%d:%d\r\n", i, user_data.triggered_watch_values[i]);
    }
    TEST_ASSERT_EQUAL(-20, count_value);
    TEST_ASSERT_EQUAL(7, user_data.index);
    TEST_ASSERT_EQUAL(-45, user_data.triggered_watch_values[0]); // watch step
    TEST_ASSERT_EQUAL(-50, user_data.triggered_watch_values[1]); // watch point
    TEST_ASSERT_EQUAL(-65, user_data.triggered_watch_values[2]); // step point (-45-20)
    TEST_ASSERT_EQUAL(-85, user_data.triggered_watch_values[3]); // step point (-65-20)
    TEST_ASSERT_EQUAL(-100, user_data.triggered_watch_values[4]);// step && watch point
    TEST_ASSERT_EQUAL(0, user_data.triggered_watch_values[5]);   // watch point (overflow)
    TEST_ASSERT_EQUAL(-20, user_data.triggered_watch_values[6]); // step point (0-20)

    printf("remove step_notify and uninstall channels\r\n");
    TEST_ESP_OK(pcnt_unit_remove_watch_step(unit));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, pcnt_unit_remove_watch_step(unit));
    TEST_ESP_OK(pcnt_del_channel(channelA));
    TEST_ESP_OK(pcnt_del_channel(channelB));
    TEST_ESP_OK(pcnt_unit_stop(unit));
    TEST_ESP_OK(pcnt_unit_disable(unit));
    TEST_ESP_OK(pcnt_del_unit(unit));
}
#endif // SOC_PCNT_SUPPORT_STEP_NOTIFY
