/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdbool.h>

#include "esp_check.h"
#include "esp_log.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gpio.h"

#include "board_inverter.h"

static const char *TAG = "board_inverter";

struct inverter_t {
    gpio_num_t gate_en_gpio;
    mcpwm_timer_handle_t timer;
    mcpwm_oper_handle_t operators[3];
    mcpwm_cmpr_handle_t comparators[3];
    mcpwm_gen_handle_t generators[3][2];
};

esp_err_t board_inverter_new(const inverter_config_t *config, inverter_handle_t *ret_inverter)
{
    ESP_RETURN_ON_FALSE(config && ret_inverter, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(config->timer_callbacks.on_full, ESP_ERR_INVALID_ARG, TAG, "on_full timer callback is required");

    struct inverter_t *inverter = calloc(1, sizeof(struct inverter_t));
    ESP_RETURN_ON_FALSE(inverter, ESP_ERR_NO_MEM, TAG, "no memory");

    /* Initialize the gate-driver enable GPIO in the disabled state. */
    inverter->gate_en_gpio = config->gate_en_gpio;
    ESP_ERROR_CHECK(gpio_set_level(inverter->gate_en_gpio, false));
    gpio_config_t gate_config = {
        .pin_bit_mask = 1ULL << inverter->gate_en_gpio,
                             .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&gate_config));

    /* Create one timer shared by all three phases. */
    ESP_ERROR_CHECK(mcpwm_new_timer(&config->timer_config, &inverter->timer));

    /* Register the required timer-full callback. */
    ESP_ERROR_CHECK(mcpwm_timer_register_event_callbacks(inverter->timer, &config->timer_callbacks, config->timer_callback_user_ctx));

    /* Create one operator per phase and connect all operators to the shared timer. */
    for (int phase = 0; phase < 3; phase++) {
        ESP_ERROR_CHECK(mcpwm_new_operator(&config->operator_config, &inverter->operators[phase]));
        ESP_ERROR_CHECK(mcpwm_operator_connect_timer(inverter->operators[phase], inverter->timer));
    }

    /* Create one comparator per phase and initialize all phases to the same switch point, producing zero line-to-line voltage. */
    for (int phase = 0; phase < 3; phase++) {
        ESP_ERROR_CHECK(mcpwm_new_comparator(inverter->operators[phase], &config->compare_config, &inverter->comparators[phase]));
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(inverter->comparators[phase], config->timer_config.period_ticks / 2));
    }

    /* Create high-side and low-side PWM generators for each phase. */
    for (int phase = 0; phase < 3; phase++) {
        for (int output = 0; output < 2; output++) {
            mcpwm_generator_config_t generator_config = {
                .gen_gpio_num = config->gen_gpios[phase][output],
            };
            ESP_ERROR_CHECK(mcpwm_new_generator(inverter->operators[phase], &generator_config, &inverter->generators[phase][output]));
        }
    }

    /* Generate center-aligned high-side PWM using compare events in both count directions. */
    for (int phase = 0; phase < 3; phase++) {
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
                            inverter->generators[phase][0], MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, inverter->comparators[phase], MCPWM_GEN_ACTION_HIGH)));
        ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(
                            inverter->generators[phase][0], MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_DOWN, inverter->comparators[phase], MCPWM_GEN_ACTION_LOW)));
    }

    /* Derive complementary high-side and low-side outputs from each phase generator and insert dead time. */
    for (int phase = 0; phase < 3; phase++) {
        ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(inverter->generators[phase][0], inverter->generators[phase][0], &config->dt_config));
        ESP_ERROR_CHECK(mcpwm_generator_set_dead_time(inverter->generators[phase][0], inverter->generators[phase][1], &config->inv_dt_config));
    }

    /* Finally, enable the timer. */
    ESP_ERROR_CHECK(mcpwm_timer_enable(inverter->timer));

    *ret_inverter = inverter;
    return ESP_OK;
}

esp_err_t board_inverter_startstop(inverter_handle_t inverter, mcpwm_timer_start_stop_cmd_t command)
{
    ESP_RETURN_ON_FALSE(inverter, ESP_ERR_INVALID_ARG, TAG, "invalid inverter");

    ESP_RETURN_ON_ERROR(mcpwm_timer_start_stop(inverter->timer, command), TAG, "start or stop MCPWM timer failed");

    return ESP_OK;
}

/* In up-down count mode, each phase switch point maps directly to its MCPWM comparator value. */
esp_err_t board_inverter_set_switchpoints(inverter_handle_t inverter, const inverter_uvw_switch_ticks_t *switch_ticks)
{
    ESP_RETURN_ON_FALSE(inverter && switch_ticks, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(inverter->comparators[0], switch_ticks->u), TAG, "set U-phase switch point failed");
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(inverter->comparators[1], switch_ticks->v), TAG, "set V-phase switch point failed");
    ESP_RETURN_ON_ERROR(mcpwm_comparator_set_compare_value(inverter->comparators[2], switch_ticks->w), TAG, "set W-phase switch point failed");

    return ESP_OK;
}

esp_err_t board_inverter_gate_enable(inverter_handle_t inverter, bool enable)
{
    ESP_RETURN_ON_FALSE(inverter, ESP_ERR_INVALID_ARG, TAG, "invalid inverter");

    ESP_RETURN_ON_ERROR(gpio_set_level(inverter->gate_en_gpio, enable), TAG, "set gate driver state failed");

    return ESP_OK;
}
