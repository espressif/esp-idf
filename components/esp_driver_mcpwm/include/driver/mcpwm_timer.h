/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Group of supported MCPWM timer event callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    mcpwm_timer_event_cb_t on_full;  /*!< callback function when MCPWM timer counts to peak value */
    mcpwm_timer_event_cb_t on_empty; /*!< callback function when MCPWM timer counts to zero */
    mcpwm_timer_event_cb_t on_stop;  /*!< callback function when MCPWM timer stops */
} mcpwm_timer_event_callbacks_t;

/**
 * @brief MCPWM timer configuration
 */
typedef struct {
    int group_id;                        /*!< Specify from which group to allocate the MCPWM timer */
    mcpwm_timer_clock_source_t clk_src;  /*!< MCPWM timer clock source */
    uint32_t resolution_hz;              /*!< Counter resolution in Hz
                                              The step size of each count tick equals to (1 / resolution_hz) seconds */
    mcpwm_timer_count_mode_t count_mode; /*!< Count mode */
    uint32_t period_ticks;               /*!< Number of count ticks within a period */
    int intr_priority;                   /*!< MCPWM timer interrupt priority,
                                              if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
    struct {
        uint32_t update_period_on_empty: 1; /*!< Whether to update period when timer counts to zero */
        uint32_t update_period_on_sync: 1;  /*!< Whether to update period on sync event */
        uint32_t allow_pd: 1;               /*!< Set to allow power down. When this flag set, the driver will backup/restore the MCPWM registers before/after entering/exist sleep mode.
                                              By this approach, the system can power off MCPWM's power domain.
                                              This can save power, but at the expense of more RAM being consumed. */
    } flags;                                /*!< Extra configuration flags for timer */
} mcpwm_timer_config_t;

/**
 * @brief Create MCPWM timer
 *
 * @param[in] config MCPWM timer configuration
 * @param[out] ret_timer Returned MCPWM timer handle
 * @return
 *      - ESP_OK: Create MCPWM timer successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM timer failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM timer failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create MCPWM timer failed because all hardware timers are used up and no more free one
 *      - ESP_FAIL: Create MCPWM timer failed because of other error
 */
esp_err_t mcpwm_new_timer(const mcpwm_timer_config_t *config, mcpwm_timer_handle_t *ret_timer);

/**
 * @brief Delete MCPWM timer
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @return
 *      - ESP_OK: Delete MCPWM timer successfully
 *      - ESP_ERR_INVALID_ARG: Delete MCPWM timer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Delete MCPWM timer failed because timer is not in init state
 *      - ESP_FAIL: Delete MCPWM timer failed because of other error
 */
esp_err_t mcpwm_del_timer(mcpwm_timer_handle_t timer);

/**
 * @brief Set a new period for MCPWM timer
 *
 * @note If `mcpwm_timer_config_t::update_period_on_empty` and `mcpwm_timer_config_t::update_period_on_sync` are not set,
 *       the new period will take effect immediately.
 *       Otherwise, the new period will take effect when timer counts to zero or on sync event.
 * @note You may need to use `mcpwm_comparator_set_compare_value` to set a new compare value for MCPWM comparator
 *       in order to keep the same PWM duty cycle.
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer`
 * @param[in] period_ticks New period in count ticks
 * @return
 *      - ESP_OK: Set new period for MCPWM timer successfully
 *      - ESP_ERR_INVALID_ARG: Set new period for MCPWM timer failed because of invalid argument
 *      - ESP_FAIL: Set new period for MCPWM timer failed because of other error
 */
esp_err_t mcpwm_timer_set_period(mcpwm_timer_handle_t timer, uint32_t period_ticks);

/**
 * @brief Enable MCPWM timer
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @return
 *      - ESP_OK: Enable MCPWM timer successfully
 *      - ESP_ERR_INVALID_ARG: Enable MCPWM timer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable MCPWM timer failed because timer is enabled already
 *      - ESP_FAIL: Enable MCPWM timer failed because of other error
 */
esp_err_t mcpwm_timer_enable(mcpwm_timer_handle_t timer);

/**
 * @brief Disable MCPWM timer
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @return
 *      - ESP_OK: Disable MCPWM timer successfully
 *      - ESP_ERR_INVALID_ARG: Disable MCPWM timer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable MCPWM timer failed because timer is disabled already
 *      - ESP_FAIL: Disable MCPWM timer failed because of other error
 */
esp_err_t mcpwm_timer_disable(mcpwm_timer_handle_t timer);

/**
 * @brief Send specific start/stop commands to MCPWM timer
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @param[in] command Supported command list for MCPWM timer
 * @return
 *      - ESP_OK: Start or stop MCPWM timer successfully
 *      - ESP_ERR_INVALID_ARG: Start or stop MCPWM timer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Start or stop MCPWM timer failed because timer is not enabled
 *      - ESP_FAIL: Start or stop MCPWM timer failed because of other error
 */
esp_err_t mcpwm_timer_start_stop(mcpwm_timer_handle_t timer, mcpwm_timer_start_stop_cmd_t command);

/**
 * @brief Set event callbacks for MCPWM timer
 *
 * @note The first call to this function needs to be before the call to `mcpwm_timer_enable`
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set event callbacks failed because timer is not in init state
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t mcpwm_timer_register_event_callbacks(mcpwm_timer_handle_t timer, const mcpwm_timer_event_callbacks_t *cbs, void *user_data);

/**
 * @brief MCPWM Timer sync phase configuration
 */
typedef struct {
    mcpwm_sync_handle_t sync_src;      /*!< The sync event source. Set to NULL will disable the timer being synced by others */
    uint32_t count_value;              /*!< The count value that should lock to upon sync event */
    mcpwm_timer_direction_t direction; /*!< The count direction that should lock to upon sync event */
} mcpwm_timer_sync_phase_config_t;

/**
 * @brief Set sync phase for MCPWM timer
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @param[in] config MCPWM timer sync phase configuration
 * @return
 *      - ESP_OK: Set sync phase for MCPWM timer successfully
 *      - ESP_ERR_INVALID_ARG: Set sync phase for MCPWM timer failed because of invalid argument
 *      - ESP_FAIL: Set sync phase for MCPWM timer failed because of other error
 */
esp_err_t mcpwm_timer_set_phase_on_sync(mcpwm_timer_handle_t timer, const mcpwm_timer_sync_phase_config_t *config);

#ifdef __cplusplus
}
#endif
