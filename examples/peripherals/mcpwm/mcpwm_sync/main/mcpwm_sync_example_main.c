/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"

const static char *TAG = "example";

#define EXAMPLE_TIMER_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define EXAMPLE_TIMER_PERIOD        1000     // 1000 ticks, 1ms
#define EXAMPLE_GEN_GPIO0           0
#define EXAMPLE_GEN_GPIO1           2
#define EXAMPLE_GEN_GPIO2           4
#define EXAMPLE_SYNC_GPIO           5

#if CONFIG_EXAMPLE_SYNC_FROM_GPIO
static void example_setup_sync_strategy(mcpwm_timer_handle_t timers[])
{
    //    +----GPIO----+
    //    |     |      |
    //    |     |      |
    //    v     v      v
    // timer0 timer1 timer2
    ESP_LOGI(TAG, "Create GPIO sync source");
    mcpwm_sync_handle_t gpio_sync_source = NULL;
    mcpwm_gpio_sync_src_config_t gpio_sync_config = {
        .group_id = 0,  // GPIO fault should be in the same group of the above timers
        .gpio_num = EXAMPLE_SYNC_GPIO,
        .flags.pull_down = true,
        .flags.active_neg = false,  // by default, a posedge pulse can trigger a sync event
        .flags.io_loop_back = true, // then we can trigger a sync event using `gpio_set_level` on the same GPIO
    };
    ESP_ERROR_CHECK(mcpwm_new_gpio_sync_src(&gpio_sync_config, &gpio_sync_source));

    ESP_LOGI(TAG, "Set timers to sync on the GPIO");
    mcpwm_timer_sync_phase_config_t sync_phase_config = {
        .count_value = 0,
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .sync_src = gpio_sync_source,
    };
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_timer_set_phase_on_sync(timers[i], &sync_phase_config));
    }

    ESP_LOGI(TAG, "Trigger a pulse on the GPIO as a sync event");
    gpio_set_level(EXAMPLE_SYNC_GPIO, 0);
    gpio_set_level(EXAMPLE_SYNC_GPIO, 1);
}
#endif // CONFIG_EXAMPLE_SYNC_FROM_GPIO

#if CONFIG_EXAMPLE_SYNC_FROM_TEZ
static void example_setup_sync_strategy(mcpwm_timer_handle_t timers[])
{
    //          +->timer1
    // (TEZ)    |
    // timer0---+
    //          |
    //          +->timer2
    ESP_LOGI(TAG, "Create TEZ sync source from timer0");
    mcpwm_sync_handle_t timer_sync_source = NULL;
    mcpwm_timer_sync_src_config_t timer_sync_config = {
        .timer_event = MCPWM_TIMER_EVENT_EMPTY, // generate sync event on timer empty
    };
    ESP_ERROR_CHECK(mcpwm_new_timer_sync_src(timers[0], &timer_sync_config, &timer_sync_source));

    ESP_LOGI(TAG, "Set other timers sync to the first timer");
    mcpwm_timer_sync_phase_config_t sync_phase_config = {
        .count_value = 0,
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .sync_src = timer_sync_source,
    };
    for (int i = 1; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_timer_set_phase_on_sync(timers[i], &sync_phase_config));
    }

    ESP_LOGI(TAG, "Wait some time for the timer TEZ event");
    vTaskDelay(pdMS_TO_TICKS(10));
}
#endif // CONFIG_EXAMPLE_SYNC_FROM_TEZ

#if CONFIG_EXAMPLE_SYNC_FROM_SOFT
static void example_setup_sync_strategy(mcpwm_timer_handle_t timers[])
{
    //       soft
    //        |
    //        v
    //    +-timer0--+
    //    |         |
    //    v         v
    // timer1    timer2
    ESP_LOGI(TAG, "Create software sync source");
    mcpwm_sync_handle_t soft_sync_source = NULL;
    mcpwm_soft_sync_config_t soft_sync_config = {};
    ESP_ERROR_CHECK(mcpwm_new_soft_sync_src(&soft_sync_config, &soft_sync_source));

    ESP_LOGI(TAG, "Create timer sync source to propagate the sync event");
    mcpwm_sync_handle_t timer_sync_source;
    mcpwm_timer_sync_src_config_t timer_sync_config = {
        .flags.propagate_input_sync = true,
    };
    ESP_ERROR_CHECK(mcpwm_new_timer_sync_src(timers[0], &timer_sync_config, &timer_sync_source));

    ESP_LOGI(TAG, "Set sync phase for timers");
    mcpwm_timer_sync_phase_config_t sync_phase_config = {
        .count_value = 0,
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .sync_src = soft_sync_source,
    };
    ESP_ERROR_CHECK(mcpwm_timer_set_phase_on_sync(timers[0], &sync_phase_config));
    sync_phase_config.sync_src = timer_sync_source;
    for (int i = 1; i < 3; ++i) {
        ESP_ERROR_CHECK(mcpwm_timer_set_phase_on_sync(timers[i], &sync_phase_config));
    }

    ESP_LOGI(TAG, "Trigger the software sync event");
    ESP_ERROR_CHECK(mcpwm_soft_sync_activate(soft_sync_source));
}
#endif // CONFIG_EXAMPLE_SYNC_FROM_SOFT

void app_main(void)
{
    ESP_LOGI(TAG, "Create timers");
    mcpwm_timer_handle_t timers[3];
    mcpwm_timer_config_t timer_config = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = EXAMPLE_TIMER_RESOLUTION_HZ,
        .period_ticks = EXAMPLE_TIMER_PERIOD,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timers[i]));
    }

    ESP_LOGI(TAG, "Create operators");
    mcpwm_oper_handle_t operators[3];
    mcpwm_operator_config_t operator_config = {
        .group_id = 0, // operator should be in the same group of the above timers
    };
    for (int i = 0; i < 3; ++i) {
        ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operators[i]));
    }

    ESP_LOGI(TAG, "Connect timers and operators with each other");
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operators[i], timers[i]));
    }

    ESP_LOGI(TAG, "Create comparators");
    mcpwm_cmpr_handle_t comparators[3];
    mcpwm_comparator_config_t compare_config = {
        .flags.update_cmp_on_tez = true,
    };
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_new_comparator(operators[i], &compare_config, &comparators[i]));
        // init compare for each comparator
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparators[i], 200));
    }

    ESP_LOGI(TAG, "Create generators");
    mcpwm_gen_handle_t generators[3];
    const int gen_gpios[3] = {EXAMPLE_GEN_GPIO0, EXAMPLE_GEN_GPIO1, EXAMPLE_GEN_GPIO2};
    mcpwm_generator_config_t gen_config = {};
    for (int i = 0; i < 3; i++) {
        gen_config.gen_gpio_num = gen_gpios[i];
        ESP_ERROR_CHECK(mcpwm_new_generator(operators[i], &gen_config, &generators[i]));
    }

    ESP_LOGI(TAG, "Set generator actions on timer and compare event");
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generators[i],
                                                                  // when the timer value is zero, and is counting up, set output to high
                                                                  MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generators[i],
                                                                    // when compare event happens, and timer is counting up, set output to low
                                                                    MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparators[i], MCPWM_GEN_ACTION_LOW)));
    }

    ESP_LOGI(TAG, "Start timers one by one, so they are not synced");
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_timer_enable(timers[i]));
        ESP_ERROR_CHECK(mcpwm_timer_start_stop(timers[i], MCPWM_TIMER_START_NO_STOP));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelay(pdMS_TO_TICKS(100));

    // Manually added this "IDLE" phase, which can help us distinguish the wave forms before and after sync
    ESP_LOGI(TAG, "Force the output level to low, timer still running");
    for (int i = 0; i < 3; i++) {
        ESP_ERROR_CHECK(mcpwm_generator_set_force_level(generators[i], 0, true));
    }

    ESP_LOGI(TAG, "Setup sync strategy");
    example_setup_sync_strategy(timers);

    ESP_LOGI(TAG, "Now the output PWMs should in sync");
    for (int i = 0; i < 3; ++i) {
        // remove the force level on the generator, so that we can see the PWM again
        ESP_ERROR_CHECK(mcpwm_generator_set_force_level(generators[i], -1, true));
    }
    vTaskDelay(pdMS_TO_TICKS(100));
}
