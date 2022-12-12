/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
 * @brief MCPWM comparator configuration
 */
typedef struct {
    struct {
        uint32_t update_cmp_on_tez: 1;  /*!< Whether to update compare value when timer count equals to zero (tez) */
        uint32_t update_cmp_on_tep: 1;  /*!< Whether to update compare value when timer count equals to peak (tep) */
        uint32_t update_cmp_on_sync: 1; /*!< Whether to update compare value on sync event */
    } flags;                            /*!< Extra configuration flags for comparator */
} mcpwm_comparator_config_t;

/**
 * @brief Create MCPWM comparator
 *
 * @param[in] oper MCPWM operator, allocated by `mcpwm_new_operator()`, the new comparator will be allocated from this operator
 * @param[in] config MCPWM comparator configuration
 * @param[out] ret_cmpr Returned MCPWM comparator
 * @return
 *      - ESP_OK: Create MCPWM comparator successfully
 *      - ESP_ERR_INVALID_ARG: Create MCPWM comparator failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MCPWM comparator failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create MCPWM comparator failed because can't find free resource
 *      - ESP_FAIL: Create MCPWM comparator failed because of other error
 */
esp_err_t mcpwm_new_comparator(mcpwm_oper_handle_t oper, const mcpwm_comparator_config_t *config, mcpwm_cmpr_handle_t *ret_cmpr);

/**
 * @brief Delete MCPWM comparator
 *
 * @param[in] cmpr MCPWM comparator handle, allocated by `mcpwm_new_comparator()`
 * @return
 *      - ESP_OK: Delete MCPWM comparator successfully
 *      - ESP_ERR_INVALID_ARG: Delete MCPWM comparator failed because of invalid argument
 *      - ESP_FAIL: Delete MCPWM comparator failed because of other error
 */
esp_err_t mcpwm_del_comparator(mcpwm_cmpr_handle_t cmpr);

/**
 * @brief Group of supported MCPWM compare event callbacks
 * @note The callbacks are all running under ISR environment
 */
typedef struct {
    mcpwm_compare_event_cb_t on_reach; /*!< ISR callback function which would be invoked when counter reaches compare value */
} mcpwm_comparator_event_callbacks_t;

/**
 * @brief Set event callbacks for MCPWM comparator
 *
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 *
 * @param[in] cmpr MCPWM comparator handle, allocated by `mcpwm_new_comparator()`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t mcpwm_comparator_register_event_callbacks(mcpwm_cmpr_handle_t cmpr, const mcpwm_comparator_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Set MCPWM comparator's compare value
 *
 * @param[in] cmpr MCPWM comparator handle, allocated by `mcpwm_new_comparator()`
 * @param[in] cmp_ticks The new compare value
 * @return
 *      - ESP_OK: Set MCPWM compare value successfully
 *      - ESP_ERR_INVALID_ARG: Set MCPWM compare value failed because of invalid argument (e.g. the cmp_ticks is out of range)
 *      - ESP_ERR_INVALID_STATE: Set MCPWM compare value failed because the operator doesn't have a timer connected
 *      - ESP_FAIL: Set MCPWM compare value failed because of other error
 */
esp_err_t mcpwm_comparator_set_compare_value(mcpwm_cmpr_handle_t cmpr, uint32_t cmp_ticks);

#ifdef __cplusplus
}
#endif
