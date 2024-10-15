/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "driver/mcpwm_timer.h"
#include "driver/mcpwm_sync.h"
#include "driver/gpio.h"
#include "esp_private/mcpwm.h"
#include "test_mcpwm_utils.h"

TEST_CASE("mcpwm_sync_source_install_uninstall", "[mcpwm]")
{
    printf("install timer sync_src\r\n");
    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz
        .period_ticks = 200,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    const int total_timers = SOC_MCPWM_TIMERS_PER_GROUP * SOC_MCPWM_GROUPS;
    mcpwm_timer_handle_t timers[total_timers];
    int k = 0;
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        timer_config.group_id = i;
        for (int j = 0; j < SOC_MCPWM_TIMERS_PER_GROUP; j++) {
            TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timers[k++]));
        }
    }
    mcpwm_timer_sync_src_config_t timer_sync_src_config = {
        .timer_event = MCPWM_TIMER_EVENT_EMPTY,
    };
    mcpwm_sync_handle_t timer_syncs[total_timers];
    for (int i = 0; i < total_timers; i++) {
        TEST_ESP_OK(mcpwm_new_timer_sync_src(timers[i], &timer_sync_src_config, &timer_syncs[i]));
    }
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_new_timer_sync_src(timers[0], &timer_sync_src_config, &timer_syncs[0]));

    printf("install gpio sync_src\r\n");
    mcpwm_gpio_sync_src_config_t gpio_sync_config = {
        .gpio_num = TEST_SYNC_GPIO,
    };
    const int total_gpio_sync_srcs = SOC_MCPWM_GROUPS * SOC_MCPWM_GPIO_SYNCHROS_PER_GROUP;
    mcpwm_sync_handle_t gpio_sync_srcs[total_gpio_sync_srcs];
    k = 0;
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        gpio_sync_config.group_id = i;
        for (int j = 0; j < SOC_MCPWM_GPIO_SYNCHROS_PER_GROUP; j++) {
            TEST_ESP_OK(mcpwm_new_gpio_sync_src(&gpio_sync_config, &gpio_sync_srcs[k++]));
        }
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_gpio_sync_src(&gpio_sync_config, &gpio_sync_srcs[0]));

    printf("delete synchors\r\n");
    for (int i = 0; i < total_gpio_sync_srcs; i++) {
        TEST_ESP_OK(mcpwm_del_sync_src(gpio_sync_srcs[i]));
    }
    for (int i = 0; i < total_timers; i++) {
        TEST_ESP_OK(mcpwm_del_sync_src(timer_syncs[i]));
        TEST_ESP_OK(mcpwm_del_timer(timers[i]));
    }
}

TEST_CASE("mcpwm_soft_sync_timer_phase_lock", "[mcpwm]")
{
    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = 1000000, // 1MHz
        .period_ticks = 200,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
    };
    mcpwm_timer_handle_t timer = NULL;
    TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timer));
    TEST_ESP_OK(mcpwm_timer_enable(timer));
    TEST_ESP_OK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_STOP_FULL));
    vTaskDelay(pdMS_TO_TICKS(10));
    check_mcpwm_timer_phase(&timer, 1, timer_config.period_ticks / 2, MCPWM_TIMER_DIRECTION_DOWN);

    printf("install soft sync source\r\n");
    mcpwm_sync_handle_t soft_sync = NULL;
    mcpwm_soft_sync_config_t soft_sync_config = {};
    TEST_ESP_OK(mcpwm_new_soft_sync_src(&soft_sync_config, &soft_sync));

    mcpwm_timer_sync_phase_config_t sync_phase_config = {
        .count_value = 77,
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .sync_src = soft_sync,
    };
    TEST_ESP_OK(mcpwm_timer_set_phase_on_sync(timer, &sync_phase_config));
    TEST_ESP_OK(mcpwm_soft_sync_activate(soft_sync));
    check_mcpwm_timer_phase(&timer, 1, 77, MCPWM_TIMER_DIRECTION_UP);

    TEST_ESP_OK(mcpwm_timer_disable(timer));
    TEST_ESP_OK(mcpwm_del_timer(timer));
    TEST_ESP_OK(mcpwm_del_sync_src(soft_sync));
}

TEST_CASE("mcpwm_gpio_sync_timer_phase_lock", "[mcpwm]")
{
    //  GPIO
    //   |
    //   v
    // timer0-->timer1-->timer2
    const int gpio_num = TEST_SYNC_GPIO;
    gpio_config_t sync_gpio_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = BIT(gpio_num),
    };
    TEST_ESP_OK(gpio_config(&sync_gpio_conf));

    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = 1000000, // 1MHz, 1us per tick
        .period_ticks = 500,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    mcpwm_timer_sync_src_config_t sync_config = {
        .flags.propagate_input_sync = 1, // reuse the input sync source as the output sync trigger
    };
    mcpwm_timer_handle_t timers[SOC_MCPWM_TIMERS_PER_GROUP];
    mcpwm_sync_handle_t sync_srcs[SOC_MCPWM_TIMERS_PER_GROUP];
    for (int i = 0; i < SOC_MCPWM_TIMERS_PER_GROUP; i++) {
        TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timers[i]));
        TEST_ESP_OK(mcpwm_new_timer_sync_src(timers[i], &sync_config, &sync_srcs[i]));
    }
    mcpwm_timer_sync_phase_config_t sync_phase_config = {
        .count_value = 100,
        .direction = MCPWM_TIMER_DIRECTION_UP,
    };
    mcpwm_sync_handle_t gpio_sync_src;
    mcpwm_gpio_sync_src_config_t gpio_sync_config = {
        .group_id = 0,
        .gpio_num = gpio_num,
        .flags.pull_down = true, // internally pull down
    };
    TEST_ESP_OK(mcpwm_new_gpio_sync_src(&gpio_sync_config, &gpio_sync_src));
    // put the GPIO into initial state
    gpio_set_level(gpio_num, 0);
    for (int i = 1; i < SOC_MCPWM_TIMERS_PER_GROUP; i++) {
        sync_phase_config.sync_src = sync_srcs[i - 1];
        TEST_ESP_OK(mcpwm_timer_set_phase_on_sync(timers[i], &sync_phase_config));
    }
    sync_phase_config.sync_src = gpio_sync_src;
    TEST_ESP_OK(mcpwm_timer_set_phase_on_sync(timers[0], &sync_phase_config));

    // simulate an GPIO sync signal
    gpio_set_level(gpio_num, 1);
    gpio_set_level(gpio_num, 0);
    check_mcpwm_timer_phase(timers, SOC_MCPWM_CAPTURE_TIMERS_PER_GROUP, 100, MCPWM_TIMER_DIRECTION_UP);

    TEST_ESP_OK(mcpwm_del_sync_src(gpio_sync_src));
    for (int i = 0; i < SOC_MCPWM_TIMERS_PER_GROUP; i++) {
        TEST_ESP_OK(mcpwm_del_sync_src(sync_srcs[i]));
        TEST_ESP_OK(mcpwm_del_timer(timers[i]));
    }
    TEST_ESP_OK(gpio_reset_pin(gpio_num));
}

TEST_CASE("mcpwm_timer_sync_timer_phase_lock", "[mcpwm]")
{
    //          +->timer1
    //          |
    // timer0---+
    //          |
    //          +->timer2
    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = 1000000, // 1MHz, 1us per tick
        .period_ticks = 500,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,
    };
    mcpwm_timer_handle_t timers[SOC_MCPWM_TIMERS_PER_GROUP];
    for (int i = 0; i < SOC_MCPWM_TIMERS_PER_GROUP; i++) {
        TEST_ESP_OK(mcpwm_new_timer(&timer_config, &timers[i]));
    }

    mcpwm_timer_sync_src_config_t sync_config = {
        .timer_event = MCPWM_TIMER_EVENT_FULL,
    };
    mcpwm_sync_handle_t sync_src;
    TEST_ESP_OK(mcpwm_new_timer_sync_src(timers[0], &sync_config, &sync_src));

    mcpwm_timer_sync_phase_config_t sync_phase_config = {
        .count_value = 50,
        .direction = MCPWM_TIMER_DIRECTION_DOWN,
        .sync_src = sync_src,
    };
    for (int i = 1; i < SOC_MCPWM_TIMERS_PER_GROUP; i++) {
        TEST_ESP_OK(mcpwm_timer_set_phase_on_sync(timers[i], &sync_phase_config));
    }

    TEST_ESP_OK(mcpwm_timer_enable(timers[0]));
    TEST_ESP_OK(mcpwm_timer_start_stop(timers[0], MCPWM_TIMER_START_STOP_FULL));
    vTaskDelay(pdMS_TO_TICKS(10));

    check_mcpwm_timer_phase(&timers[1], 2, 50, MCPWM_TIMER_DIRECTION_DOWN);

    TEST_ESP_OK(mcpwm_timer_disable(timers[0]));
    TEST_ESP_OK(mcpwm_del_sync_src(sync_src));
    for (int i = 0; i < SOC_MCPWM_TIMERS_PER_GROUP; i++) {
        TEST_ESP_OK(mcpwm_del_timer(timers[i]));
    }
}
