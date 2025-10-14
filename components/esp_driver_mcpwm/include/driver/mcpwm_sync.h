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
 * @brief MCPWM timer sync source configuration
 */
typedef struct {
    mcpwm_timer_event_t timer_event; /*!< Timer event, upon which MCPWM timer will generate the sync signal */
    struct {
        uint32_t propagate_input_sync: 1; /*!< The input sync signal would be routed to its sync output */
    } flags;                          /*!< Extra configuration flags for timer sync source */
} mcpwm_timer_sync_src_config_t;

/**
 * @brief Create MCPWM timer sync source
 *
 * @param[in] timer MCPWM timer handle, allocated by `mcpwm_new_timer()`
 * @param[in] config MCPWM timer sync source configuration
 * @param[out] ret_sync Returned MCPWM sync handle
 * @return
 *      - ESP_OK: Create MCPWM timer sync source successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM timer sync source failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM timer sync source failed because out of memory
 *      - ESP_ERR_INVALID_STATE: Create MCPWM timer sync source failed because the timer has created a sync source before
 *      - ESP_FAIL: Create MCPWM timer sync source failed because of other error
 */
esp_err_t mcpwm_new_timer_sync_src(mcpwm_timer_handle_t timer, const mcpwm_timer_sync_src_config_t *config, mcpwm_sync_handle_t *ret_sync);

/**
 * @brief MCPWM GPIO sync source configuration
 */
typedef struct {
    int group_id; /*!< MCPWM group ID */
    int gpio_num; /*!< GPIO used by sync source */
    struct {
        uint32_t active_neg: 1;   /*!< Whether the sync signal is active on negedge, by default, the sync signal's posedge is treated as active */
    } flags;                      /*!< Extra configuration flags for GPIO sync source */
} mcpwm_gpio_sync_src_config_t;

/**
 * @brief Create MCPWM GPIO sync source
 *
 * @param[in] config MCPWM GPIO sync source configuration
 * @param[out] ret_sync Returned MCPWM GPIO sync handle
 * @return
 *      - ESP_OK: Create MCPWM GPIO sync source successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM GPIO sync source failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM GPIO sync source failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create MCPWM GPIO sync source failed because can't find free resource
 *      - ESP_FAIL: Create MCPWM GPIO sync source failed because of other error
 */
esp_err_t mcpwm_new_gpio_sync_src(const mcpwm_gpio_sync_src_config_t *config, mcpwm_sync_handle_t *ret_sync);

/**
 * @brief MCPWM software sync configuration structure
 */
typedef struct {
} mcpwm_soft_sync_config_t;

/**
 * @brief Create MCPWM software sync source
 *
 * @param[in] config MCPWM software sync source configuration
 * @param[out] ret_sync Returned software sync handle
 * @return
 *      - ESP_OK: Create MCPWM software sync successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM software sync failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM software sync failed because out of memory
 *      - ESP_FAIL: Create MCPWM software sync failed because of other error
 */
esp_err_t mcpwm_new_soft_sync_src(const mcpwm_soft_sync_config_t *config, mcpwm_sync_handle_t *ret_sync);

/**
 * @brief Delete MCPWM sync source
 *
 * @param[in] sync MCPWM sync handle, allocated by `mcpwm_new_timer_sync_src()` or `mcpwm_new_gpio_sync_src()` or `mcpwm_new_soft_sync_src()`
 * @return
 *      - ESP_OK: Delete MCPWM sync source successfully
 *      - ESP_ERR_INVALID_ARG: Delete MCPWM sync source failed because of invalid argument
 *      - ESP_FAIL: Delete MCPWM sync source failed because of other error
 */
esp_err_t mcpwm_del_sync_src(mcpwm_sync_handle_t sync);

/**
 * @brief Activate the software sync, trigger the sync event for once
 *
 * @param[in] sync MCPWM soft sync handle, allocated by `mcpwm_new_soft_sync_src()`
 * @return
 *      - ESP_OK: Trigger MCPWM software sync event successfully
 *      - ESP_ERR_INVALID_ARG: Trigger MCPWM software sync event failed because of invalid argument
 *      - ESP_FAIL: Trigger MCPWM software sync event failed because of other error
 */
esp_err_t mcpwm_soft_sync_activate(mcpwm_sync_handle_t sync);

#ifdef __cplusplus
}
#endif
