/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/gptimer_types.h"
#include "driver/gptimer_etm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief General Purpose Timer configuration
 */
typedef struct {
    gptimer_clock_source_t clk_src;      /*!< GPTimer clock source */
    gptimer_count_direction_t direction; /*!< Count direction */
    uint32_t resolution_hz;              /*!< Counter resolution (working frequency) in Hz,
                                              hence, the step size of each count tick equals to (1 / resolution_hz) seconds */
    int intr_priority;                   /*!< GPTimer interrupt priority,
                                              if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
    struct {
        uint32_t intr_shared: 1;         /*!< Set true, the timer interrupt number can be shared with other peripherals */
        uint32_t backup_before_sleep: 1; /*!< If set, the driver will backup/restore the GPTimer registers before/after entering/exist sleep mode.
                                              By this approach, the system can power off GPTimer's power domain.
                                              This can save power, but at the expense of more RAM being consumed */
    } flags;                             /*!< GPTimer config flags*/
} gptimer_config_t;

/**
 * @brief Create a new General Purpose Timer, and return the handle
 *
 * @note The newly created timer is put in the "init" state.
 *
 * @param[in] config GPTimer configuration
 * @param[out] ret_timer Returned timer handle
 * @return
 *      - ESP_OK: Create GPTimer successfully
 *      - ESP_ERR_INVALID_ARG: Create GPTimer failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create GPTimer failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create GPTimer failed because all hardware timers are used up and no more free one
 *      - ESP_FAIL: Create GPTimer failed because of other error
 */
esp_err_t gptimer_new_timer(const gptimer_config_t *config, gptimer_handle_t *ret_timer);

/**
 * @brief Delete the GPTimer handle
 *
 * @note A timer must be in the "init" state before it can be deleted.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @return
 *      - ESP_OK: Delete GPTimer successfully
 *      - ESP_ERR_INVALID_ARG: Delete GPTimer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Delete GPTimer failed because the timer is not in init state
 *      - ESP_FAIL: Delete GPTimer failed because of other error
 */
esp_err_t gptimer_del_timer(gptimer_handle_t timer);

/**
 * @brief Set GPTimer raw count value
 *
 * @note When updating the raw count of an active timer, the timer will immediately start counting from the new value.
 * @note This function is allowed to run within ISR context
 * @note If `CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` is enabled, this function will be placed in the IRAM by linker,
 *       makes it possible to execute even when the Flash Cache is disabled.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[in] value Count value to be set
 * @return
 *      - ESP_OK: Set GPTimer raw count value successfully
 *      - ESP_ERR_INVALID_ARG: Set GPTimer raw count value failed because of invalid argument
 *      - ESP_FAIL: Set GPTimer raw count value failed because of other error
 */
esp_err_t gptimer_set_raw_count(gptimer_handle_t timer, uint64_t value);

/**
 * @brief Get GPTimer raw count value
 *
 * @note This function will trigger a software capture event and then return the captured count value.
 * @note With the raw count value and the resolution returned from `gptimer_get_resolution`, you can convert the count value into seconds.
 * @note This function is allowed to run within ISR context
 * @note If `CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` is enabled, this function will be placed in the IRAM by linker,
 *       makes it possible to execute even when the Flash Cache is disabled.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[out] value Returned GPTimer count value
 * @return
 *      - ESP_OK: Get GPTimer raw count value successfully
 *      - ESP_ERR_INVALID_ARG: Get GPTimer raw count value failed because of invalid argument
 *      - ESP_FAIL: Get GPTimer raw count value failed because of other error
 */
esp_err_t gptimer_get_raw_count(gptimer_handle_t timer, uint64_t *value);

/**
 * @brief Return the real resolution of the timer
 *
 * @note usually the timer resolution is same as what you configured in the `gptimer_config_t::resolution_hz`,
 *       but some unstable clock source (e.g. RC_FAST) will do a calibration, the real resolution can be different from the configured one.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[out] out_resolution Returned timer resolution, in Hz
 * @return
 *      - ESP_OK: Get GPTimer resolution successfully
 *      - ESP_ERR_INVALID_ARG: Get GPTimer resolution failed because of invalid argument
 *      - ESP_FAIL: Get GPTimer resolution failed because of other error
 */
esp_err_t gptimer_get_resolution(gptimer_handle_t timer, uint32_t *out_resolution);

/**
 * @brief Get GPTimer captured count value
 *
 * @note Different from `gptimer_get_raw_count`, this function won't trigger a software capture event. It just returns the last captured count value.
 *       It's especially useful when the capture has already been triggered by an external event and you want to read the captured value.
 * @note This function is allowed to run within ISR context
 * @note If `CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` is enabled, this function will be placed in the IRAM by linker,
 *       makes it possible to execute even when the Flash Cache is disabled.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[out] value Returned captured count value
 * @return
 *      - ESP_OK: Get GPTimer captured count value successfully
 *      - ESP_ERR_INVALID_ARG: Get GPTimer captured count value failed because of invalid argument
 *      - ESP_FAIL: Get GPTimer captured count value failed because of other error
 */
esp_err_t gptimer_get_captured_count(gptimer_handle_t timer, uint64_t *value);

/**
 * @brief Group of supported GPTimer callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_GPTIMER_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 */
typedef struct {
    gptimer_alarm_cb_t on_alarm; /*!< Timer alarm callback */
} gptimer_event_callbacks_t;

/**
 * @brief Set callbacks for GPTimer
 *
 * @note User registered callbacks are expected to be runnable within ISR context
 * @note The first call to this function needs to be before the call to `gptimer_enable`
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set event callbacks failed because the timer is not in init state
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t gptimer_register_event_callbacks(gptimer_handle_t timer, const gptimer_event_callbacks_t *cbs, void *user_data);

/**
 * @brief General Purpose Timer alarm configuration
 */
typedef struct {
    uint64_t alarm_count;  /*!< Alarm target count value */
    uint64_t reload_count; /*!< Alarm reload count value, effect only when `auto_reload_on_alarm` is set to true */
    struct {
        uint32_t auto_reload_on_alarm: 1; /*!< Reload the count value by hardware, immediately at the alarm event */
    } flags;                              /*!< Alarm config flags*/
} gptimer_alarm_config_t;

/**
 * @brief Set alarm event actions for GPTimer.
 *
 * @note This function is allowed to run within ISR context, so you can update new alarm action immediately in any ISR callback.
 * @note If `CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` is enabled, this function will be placed in the IRAM by linker,
 *       makes it possible to execute even when the Flash Cache is disabled.
 *       In this case, please also ensure the `gptimer_alarm_config_t` instance is placed in the static data section
 *       instead of in the read-only data section. e.g.: `static gptimer_alarm_config_t alarm_config = { ... };`
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[in] config Alarm configuration, especially, set config to NULL means disabling the alarm function
 * @return
 *      - ESP_OK: Set alarm action for GPTimer successfully
 *      - ESP_ERR_INVALID_ARG: Set alarm action for GPTimer failed because of invalid argument
 *      - ESP_FAIL: Set alarm action for GPTimer failed because of other error
 */
esp_err_t gptimer_set_alarm_action(gptimer_handle_t timer, const gptimer_alarm_config_t *config);

/**
 * @brief Enable GPTimer
 *
 * @note This function will transit the timer state from "init" to "enable".
 * @note This function will enable the interrupt service, if it's lazy installed in `gptimer_register_event_callbacks`.
 * @note This function will acquire a PM lock, if a specific source clock (e.g. APB) is selected in the `gptimer_config_t`, while `CONFIG_PM_ENABLE` is enabled.
 * @note Enable a timer doesn't mean to start it. See also `gptimer_start` for how to make the timer start counting.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @return
 *      - ESP_OK: Enable GPTimer successfully
 *      - ESP_ERR_INVALID_ARG: Enable GPTimer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable GPTimer failed because the timer is already enabled
 *      - ESP_FAIL: Enable GPTimer failed because of other error
 */
esp_err_t gptimer_enable(gptimer_handle_t timer);

/**
 * @brief Disable GPTimer
 *
 * @note This function will transit the timer state from "enable" to "init".
 * @note This function will disable the interrupt service if it's installed.
 * @note This function will release the PM lock if it's acquired in the `gptimer_enable`.
 * @note Disable a timer doesn't mean to stop it. See also `gptimer_stop` for how to make the timer stop counting.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @return
 *      - ESP_OK: Disable GPTimer successfully
 *      - ESP_ERR_INVALID_ARG: Disable GPTimer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable GPTimer failed because the timer is not enabled yet
 *      - ESP_FAIL: Disable GPTimer failed because of other error
 */
esp_err_t gptimer_disable(gptimer_handle_t timer);

/**
 * @brief Start GPTimer (internal counter starts counting)
 *
 * @note This function will transit the timer state from "enable" to "run".
 * @note This function is allowed to run within ISR context
 * @note If `CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` is enabled, this function will be placed in the IRAM by linker,
 *       makes it possible to execute even when the Flash Cache is disabled.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @return
 *      - ESP_OK: Start GPTimer successfully
 *      - ESP_ERR_INVALID_ARG: Start GPTimer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Start GPTimer failed because the timer is not enabled or is already in running
 *      - ESP_FAIL: Start GPTimer failed because of other error
 */
esp_err_t gptimer_start(gptimer_handle_t timer);

/**
 * @brief Stop GPTimer (internal counter stops counting)
 *
 * @note This function will transit the timer state from "run" to "enable".
 * @note This function is allowed to run within ISR context
 * @note If `CONFIG_GPTIMER_CTRL_FUNC_IN_IRAM` is enabled, this function will be placed in the IRAM by linker,
 *       makes it possible to execute even when the Flash Cache is disabled.
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @return
 *      - ESP_OK: Stop GPTimer successfully
 *      - ESP_ERR_INVALID_ARG: Stop GPTimer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Stop GPTimer failed because the timer is not in running.
 *      - ESP_FAIL: Stop GPTimer failed because of other error
 */
esp_err_t gptimer_stop(gptimer_handle_t timer);

#ifdef __cplusplus
}
#endif
