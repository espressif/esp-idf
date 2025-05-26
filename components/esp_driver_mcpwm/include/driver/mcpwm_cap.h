/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
 * @brief MCPWM capture timer configuration structure
 */
typedef struct {
    int group_id;                         /*!< Specify from which group to allocate the capture timer */
    mcpwm_capture_clock_source_t clk_src; /*!< MCPWM capture timer clock source */
    uint32_t resolution_hz;               /*!< Resolution of capture timer */
    struct {
        uint32_t allow_pd: 1;       /*!< Set to allow power down. When this flag set, the driver will backup/restore the MCPWM registers before/after entering/exist sleep mode.
                                      By this approach, the system can power off MCPWM's power domain.
                                      This can save power, but at the expense of more RAM being consumed.*/
    } flags;                        /*!< Extra configuration flags for timer */
} mcpwm_capture_timer_config_t;

/**
 * @brief Create MCPWM capture timer
 *
 * @param[in] config MCPWM capture timer configuration
 * @param[out] ret_cap_timer Returned MCPWM capture timer handle
 * @return
 *      - ESP_OK: Create MCPWM capture timer successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM capture timer failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM capture timer failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create MCPWM capture timer failed because can't find free resource
 *      - ESP_FAIL: Create MCPWM capture timer failed because of other error
 */
esp_err_t mcpwm_new_capture_timer(const mcpwm_capture_timer_config_t *config, mcpwm_cap_timer_handle_t *ret_cap_timer);

/**
 * @brief Delete MCPWM capture timer
 *
 * @param[in] cap_timer MCPWM capture timer, allocated by `mcpwm_new_capture_timer()`
 * @return
 *      - ESP_OK: Delete MCPWM capture timer successfully
 *      - ESP_ERR_INVALID_ARG: Delete MCPWM capture timer failed because of invalid argument
 *      - ESP_FAIL: Delete MCPWM capture timer failed because of other error
 */
esp_err_t mcpwm_del_capture_timer(mcpwm_cap_timer_handle_t cap_timer);

/**
 * @brief Enable MCPWM capture timer
 *
 * @param[in] cap_timer MCPWM capture timer handle, allocated by `mcpwm_new_capture_timer()`
 * @return
 *      - ESP_OK: Enable MCPWM capture timer successfully
 *      - ESP_ERR_INVALID_ARG: Enable MCPWM capture timer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable MCPWM capture timer failed because timer is enabled already
 *      - ESP_FAIL: Enable MCPWM capture timer failed because of other error
 */
esp_err_t mcpwm_capture_timer_enable(mcpwm_cap_timer_handle_t cap_timer);

/**
 * @brief Disable MCPWM capture timer
 *
 * @param[in] cap_timer MCPWM capture timer handle, allocated by `mcpwm_new_capture_timer()`
 * @return
 *      - ESP_OK: Disable MCPWM capture timer successfully
 *      - ESP_ERR_INVALID_ARG: Disable MCPWM capture timer failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable MCPWM capture timer failed because timer is disabled already
 *      - ESP_FAIL: Disable MCPWM capture timer failed because of other error
 */
esp_err_t mcpwm_capture_timer_disable(mcpwm_cap_timer_handle_t cap_timer);

/**
 * @brief Start MCPWM capture timer
 *
 * @param[in] cap_timer MCPWM capture timer, allocated by `mcpwm_new_capture_timer()`
 * @return
 *      - ESP_OK: Start MCPWM capture timer successfully
 *      - ESP_ERR_INVALID_ARG: Start MCPWM capture timer failed because of invalid argument
 *      - ESP_FAIL: Start MCPWM capture timer failed because of other error
 */
esp_err_t mcpwm_capture_timer_start(mcpwm_cap_timer_handle_t cap_timer);

/**
 * @brief Stop MCPWM capture timer
 *
 * @param[in] cap_timer MCPWM capture timer, allocated by `mcpwm_new_capture_timer()`
 * @return
 *      - ESP_OK: Stop MCPWM capture timer successfully
 *      - ESP_ERR_INVALID_ARG: Stop MCPWM capture timer failed because of invalid argument
 *      - ESP_FAIL: Stop MCPWM capture timer failed because of other error
 */
esp_err_t mcpwm_capture_timer_stop(mcpwm_cap_timer_handle_t cap_timer);

/**
 * @brief Get MCPWM capture timer resolution, in Hz
 *
 * @param[in] cap_timer MCPWM capture timer, allocated by `mcpwm_new_capture_timer()`
 * @param[out] out_resolution Returned capture timer resolution, in Hz
 * @return
 *      - ESP_OK: Get capture timer resolution successfully
 *      - ESP_ERR_INVALID_ARG: Get capture timer resolution failed because of invalid argument
 *      - ESP_FAIL: Get capture timer resolution failed because of other error
 */
esp_err_t mcpwm_capture_timer_get_resolution(mcpwm_cap_timer_handle_t cap_timer, uint32_t *out_resolution);

/**
 * @brief MCPWM Capture timer sync phase configuration
 */
typedef struct {
    mcpwm_sync_handle_t sync_src;      /*!< The sync event source */
    uint32_t count_value;              /*!< The count value that should lock to upon sync event */
    mcpwm_timer_direction_t direction; /*!< The count direction that should lock to upon sync event */
} mcpwm_capture_timer_sync_phase_config_t;

/**
 * @brief Set sync phase for MCPWM capture timer
 *
 * @param[in] cap_timer MCPWM capture timer, allocated by `mcpwm_new_capture_timer()`
 * @param[in] config MCPWM capture timer sync phase configuration
 * @return
 *      - ESP_OK: Set sync phase for MCPWM capture timer successfully
 *      - ESP_ERR_INVALID_ARG: Set sync phase for MCPWM capture timer failed because of invalid argument
 *      - ESP_FAIL: Set sync phase for MCPWM capture timer failed because of other error
 */
esp_err_t mcpwm_capture_timer_set_phase_on_sync(mcpwm_cap_timer_handle_t cap_timer, const mcpwm_capture_timer_sync_phase_config_t *config);

/**
 * @brief MCPWM capture channel configuration structure
 */
typedef struct {
    int gpio_num;                    /*!< GPIO used capturing input signal */
    int intr_priority;               /*!< MCPWM capture interrupt priority,
                                          if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
    uint32_t prescale;               /*!< Prescale of input signal, effective frequency = cap_input_clk/prescale */

    /// Extra configuration flags for capture channel
    struct extra_capture_channel_flags {
        uint32_t pos_edge: 1;          /*!< Whether to capture on positive edge */
        uint32_t neg_edge: 1;          /*!< Whether to capture on negative edge */
        uint32_t invert_cap_signal: 1; /*!< Invert the input capture signal */
    } flags;                           /*!< Extra configuration flags for capture channel */
} mcpwm_capture_channel_config_t;

/**
 * @brief Create MCPWM capture channel
 *
 * @note The created capture channel won't be enabled until calling `mcpwm_capture_channel_enable`
 *
 * @param[in] cap_timer MCPWM capture timer, allocated by `mcpwm_new_capture_timer()`, will be connected to the new capture channel
 * @param[in] config MCPWM capture channel configuration
 * @param[out] ret_cap_channel Returned MCPWM capture channel
 * @return
 *      - ESP_OK: Create MCPWM capture channel successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM capture channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM capture channel failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create MCPWM capture channel failed because can't find free resource
 *      - ESP_FAIL: Create MCPWM capture channel failed because of other error
 */
esp_err_t mcpwm_new_capture_channel(mcpwm_cap_timer_handle_t cap_timer, const mcpwm_capture_channel_config_t *config, mcpwm_cap_channel_handle_t *ret_cap_channel);

/**
 * @brief Delete MCPWM capture channel
 *
 * @param[in] cap_channel MCPWM capture channel handle, allocated by `mcpwm_new_capture_channel()`
 * @return
 *      - ESP_OK: Delete MCPWM capture channel successfully
 *      - ESP_ERR_INVALID_ARG: Delete MCPWM capture channel failed because of invalid argument
 *      - ESP_FAIL: Delete MCPWM capture channel failed because of other error
 */
esp_err_t mcpwm_del_capture_channel(mcpwm_cap_channel_handle_t cap_channel);

/**
 * @brief Enable MCPWM capture channel
 *
 * @note This function will transit the channel state from init to enable.
 * @note This function will enable the interrupt service, if it's lazy installed in `mcpwm_capture_channel_register_event_callbacks()`.
 *
 * @param[in] cap_channel MCPWM capture channel handle, allocated by `mcpwm_new_capture_channel()`
 * @return
 *      - ESP_OK: Enable MCPWM capture channel successfully
 *      - ESP_ERR_INVALID_ARG: Enable MCPWM capture channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable MCPWM capture channel failed because the channel is already enabled
 *      - ESP_FAIL: Enable MCPWM capture channel failed because of other error
 */
esp_err_t mcpwm_capture_channel_enable(mcpwm_cap_channel_handle_t cap_channel);

/**
 * @brief Disable MCPWM capture channel
 *
 * @param[in] cap_channel MCPWM capture channel handle, allocated by `mcpwm_new_capture_channel()`
 * @return
 *      - ESP_OK: Disable MCPWM capture channel successfully
 *      - ESP_ERR_INVALID_ARG: Disable MCPWM capture channel failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable MCPWM capture channel failed because the channel is not enabled yet
 *      - ESP_FAIL: Disable MCPWM capture channel failed because of other error
 */
esp_err_t mcpwm_capture_channel_disable(mcpwm_cap_channel_handle_t cap_channel);

/**
 * @brief Group of supported MCPWM capture event callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    mcpwm_capture_event_cb_t on_cap; /*!< Callback function that would be invoked when capture event occurred */
} mcpwm_capture_event_callbacks_t;

/**
 * @brief Set event callbacks for MCPWM capture channel
 *
 * @note The first call to this function needs to be before the call to `mcpwm_capture_channel_enable`
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 *
 * @param[in] cap_channel MCPWM capture channel handle, allocated by `mcpwm_new_capture_channel()`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set event callbacks failed because the channel is not in init state
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t mcpwm_capture_channel_register_event_callbacks(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Trigger a catch by software
 *
 * @param[in] cap_channel MCPWM capture channel handle, allocated by `mcpwm_new_capture_channel()`
 * @return
 *      - ESP_OK: Trigger software catch successfully
 *      - ESP_ERR_INVALID_ARG: Trigger software catch failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Trigger software catch failed because the channel is not enabled yet
 *      - ESP_FAIL: Trigger software catch failed because of other error
 */
esp_err_t mcpwm_capture_channel_trigger_soft_catch(mcpwm_cap_channel_handle_t cap_channel);

#ifdef __cplusplus
}
#endif
