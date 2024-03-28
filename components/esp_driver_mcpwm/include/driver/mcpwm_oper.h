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
 * @brief MCPWM operator configuration
 */
typedef struct {
    int group_id;                              /*!< Specify from which group to allocate the MCPWM operator */
    int intr_priority;                         /*!< MCPWM operator interrupt priority,
                                                    if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
    struct {
        uint32_t update_gen_action_on_tez: 1;  /*!< Whether to update generator action when timer counts to zero */
        uint32_t update_gen_action_on_tep: 1;  /*!< Whether to update generator action when timer counts to peak */
        uint32_t update_gen_action_on_sync: 1; /*!< Whether to update generator action on sync event */
        uint32_t update_dead_time_on_tez: 1;   /*!< Whether to update dead time when timer counts to zero */
        uint32_t update_dead_time_on_tep: 1;   /*!< Whether to update dead time when timer counts to peak */
        uint32_t update_dead_time_on_sync: 1;  /*!< Whether to update dead time on sync event */
    } flags;                                   /*!< Extra configuration flags for operator */
} mcpwm_operator_config_t;

/**
 * @brief Create MCPWM operator
 *
 * @param[in] config MCPWM operator configuration
 * @param[out] ret_oper Returned MCPWM operator handle
 * @return
 *      - ESP_OK: Create MCPWM operator successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM operator failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM operator failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create MCPWM operator failed because can't find free resource
 *      - ESP_FAIL: Create MCPWM operator failed because of other error
 */
esp_err_t mcpwm_new_operator(const mcpwm_operator_config_t *config, mcpwm_oper_handle_t *ret_oper);

/**
 * @brief Delete MCPWM operator
 *
 * @param[in] oper MCPWM operator, allocated by `mcpwm_new_operator()`
 * @return
 *      - ESP_OK: Delete MCPWM operator successfully
 *      - ESP_ERR_INVALID_ARG: Delete MCPWM operator failed because of invalid argument
 *      - ESP_FAIL: Delete MCPWM operator failed because of other error
 */
esp_err_t mcpwm_del_operator(mcpwm_oper_handle_t oper);

/**
 * @brief Connect MCPWM operator and timer, so that the operator can be driven by the timer
 *
 * @param[in] oper MCPWM operator handle, allocated by `mcpwm_new_operator()`
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @return
 *      - ESP_OK: Connect MCPWM operator and timer successfully
 *      - ESP_ERR_INVALID_ARG: Connect MCPWM operator and timer failed because of invalid argument
 *      - ESP_FAIL: Connect MCPWM operator and timer failed because of other error
 */
esp_err_t mcpwm_operator_connect_timer(mcpwm_oper_handle_t oper, mcpwm_timer_handle_t timer);

/**
 * @brief MCPWM brake configuration structure
 */
typedef struct {
    mcpwm_fault_handle_t fault;             /*!< Which fault causes the operator to brake */
    mcpwm_operator_brake_mode_t brake_mode; /*!< Brake mode */
    struct {
        uint32_t cbc_recover_on_tez: 1; /*!< Recovery CBC brake state on tez event */
        uint32_t cbc_recover_on_tep: 1; /*!< Recovery CBC brake state on tep event */
    } flags;                            /*!< Extra flags for brake configuration */
} mcpwm_brake_config_t;

/**
 * @brief Set brake method for MCPWM operator
 *
 * @param[in] oper MCPWM operator, allocated by `mcpwm_new_operator()`
 * @param[in] config MCPWM brake configuration
 * @return
 *      - ESP_OK: Set trip for operator successfully
 *      - ESP_ERR_INVALID_ARG: Set trip for operator failed because of invalid argument
 *      - ESP_FAIL: Set trip for operator failed because of other error
 */
esp_err_t mcpwm_operator_set_brake_on_fault(mcpwm_oper_handle_t oper, const mcpwm_brake_config_t *config);

/**
 * @brief Try to make the operator recover from fault
 *
 * @note To recover from fault or escape from trip, you make sure the fault signal has disappeared already.
 *       Otherwise the recovery can't succeed.
 *
 * @param[in] oper MCPWM operator, allocated by `mcpwm_new_operator()`
 * @param[in] fault MCPWM fault handle
 * @return
 *      - ESP_OK: Recover from fault successfully
 *      - ESP_ERR_INVALID_ARG: Recover from fault failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Recover from fault failed because the fault source is still active
 *      - ESP_FAIL: Recover from fault failed because of other error
 */
esp_err_t mcpwm_operator_recover_from_fault(mcpwm_oper_handle_t oper, mcpwm_fault_handle_t fault);

/**
 * @brief Group of supported MCPWM operator event callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    mcpwm_brake_event_cb_t on_brake_cbc; /*!< callback function when mcpwm operator brakes in CBC */
    mcpwm_brake_event_cb_t on_brake_ost; /*!< callback function when mcpwm operator brakes in OST */
} mcpwm_operator_event_callbacks_t;

/**
 * @brief Set event callbacks for MCPWM operator
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 *
 * @param[in] oper MCPWM operator handle, allocated by `mcpwm_new_operator()`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t mcpwm_operator_register_event_callbacks(mcpwm_oper_handle_t oper, const mcpwm_operator_event_callbacks_t *cbs, void *user_data);

/**
 * @brief MCPWM carrier configuration structure
 */
typedef struct {
    mcpwm_carrier_clock_source_t clk_src;  /*!< MCPWM carrier clock source */
    uint32_t frequency_hz;            /*!< Carrier frequency in Hz */
    uint32_t first_pulse_duration_us; /*!< The duration of the first PWM pulse, in us */
    float duty_cycle;                 /*!< Carrier duty cycle */
    struct {
        uint32_t invert_before_modulate: 1; /*!< Invert the raw signal */
        uint32_t invert_after_modulate: 1;  /*!< Invert the modulated signal */
    } flags;                                /*!< Extra flags for carrier configuration */
} mcpwm_carrier_config_t;

/**
 * @brief Apply carrier feature for MCPWM operator
 *
 * @param[in] oper MCPWM operator, allocated by `mcpwm_new_operator()`
 * @param[in] config MCPWM carrier specific configuration
 * @return
 *      - ESP_OK: Set carrier for operator successfully
 *      - ESP_ERR_INVALID_ARG: Set carrier for operator failed because of invalid argument
 *      - ESP_FAIL: Set carrier for operator failed because of other error
 */
esp_err_t mcpwm_operator_apply_carrier(mcpwm_oper_handle_t oper, const mcpwm_carrier_config_t *config);

#ifdef __cplusplus
}
#endif
