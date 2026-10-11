/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "driver/mcpwm_prelude.h"
#include "board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* U/V/W high-side switch points applied by the inverter, in timer ticks. */
typedef struct {
    uint32_t u;
    uint32_t v;
    uint32_t w;
} inverter_uvw_switch_ticks_t;

/* Hardware configuration for a three-phase MCPWM inverter instance. */
typedef struct {
    int gate_en_gpio;
    mcpwm_timer_config_t timer_config;
    mcpwm_operator_config_t operator_config;
    mcpwm_comparator_config_t compare_config;
    int gen_gpios[3][2];
    mcpwm_dead_time_config_t dt_config;
    mcpwm_dead_time_config_t inv_dt_config;

    mcpwm_timer_event_callbacks_t timer_callbacks; /* on_full callback is required */
    void *timer_callback_user_ctx;
} inverter_config_t;

/* Default configuration for a six-output, center-aligned three-phase inverter.
 * Comparator updates are latched synchronously at the timer-empty (TEZ) event.
 */
#define BOARD_INVERTER_DEFAULT_CONFIG()                         \
    {                                                           \
        .gate_en_gpio = BOARD_INVERTER_GATE_EN_GPIO,            \
        .timer_config = {                                       \
            .group_id = BOARD_INVERTER_MCPWM_GROUP_ID,          \
            .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,             \
            .resolution_hz = BOARD_INVERTER_TIMER_RESOLUTION_HZ,\
            .count_mode = MCPWM_TIMER_COUNT_MODE_UP_DOWN,       \
            .period_ticks = BOARD_INVERTER_TIMER_RESOLUTION_HZ / BOARD_INVERTER_MODULATION_FREQ_HZ, \
        },                                                      \
        .operator_config = {                                    \
            .group_id = BOARD_INVERTER_MCPWM_GROUP_ID,          \
        },                                                      \
        .compare_config = {                                     \
            .flags.update_cmp_on_tez = true,                    \
        },                                                      \
        .gen_gpios = {                                          \
            {BOARD_INVERTER_PWM_UH_GPIO, BOARD_INVERTER_PWM_UL_GPIO}, \
            {BOARD_INVERTER_PWM_VH_GPIO, BOARD_INVERTER_PWM_VL_GPIO}, \
            {BOARD_INVERTER_PWM_WH_GPIO, BOARD_INVERTER_PWM_WL_GPIO}, \
        },                                                      \
        .dt_config = {                                          \
            .posedge_delay_ticks = BOARD_INVERTER_DEAD_TIME_TICKS, \
        },                                                      \
        .inv_dt_config = {                                      \
            .negedge_delay_ticks = BOARD_INVERTER_DEAD_TIME_TICKS, \
            .flags.invert_output = true,                        \
        },                                                      \
    }

typedef struct inverter_t *inverter_handle_t;

/**
 * @brief Create an inverter instance and initialize its MCPWM resources.
 */
esp_err_t board_inverter_new(const inverter_config_t *config, inverter_handle_t *ret_inverter);

/**
 * @brief Start or stop the inverter MCPWM timer.
 */
esp_err_t board_inverter_startstop(inverter_handle_t inverter, mcpwm_timer_start_stop_cmd_t command);

/**
 * @brief Set the U/V/W high-side turn-on switch points in MCPWM timer ticks.
 */
esp_err_t board_inverter_set_switchpoints(inverter_handle_t inverter, const inverter_uvw_switch_ticks_t *switch_ticks);

/**
 * @brief Enable or disable the gate driver.
 */
esp_err_t board_inverter_gate_enable(inverter_handle_t inverter, bool enable);

#ifdef __cplusplus
}
#endif
