/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_etm.h"
#include "driver/mcpwm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MCPWM event comparator ETM event configuration
 */
typedef struct {
    mcpwm_comparator_etm_event_type_t event_type; /*!< MCPWM comparator ETM event type */
} mcpwm_cmpr_etm_event_config_t;

/**
 * @brief MCPWM timer ETM event configuration
 */
typedef struct {
    mcpwm_timer_etm_event_type_t event_type; /*!< MCPWM timer ETM event type */
} mcpwm_timer_etm_event_config_t;

/**
 * @brief Get the ETM event for MCPWM comparator
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] cmpr MCPWM comparator, allocated by `mcpwm_new_comparator()` or `mcpwm_new_event_comparator()`
 * @param[in] config MCPWM ETM comparator event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_FAIL: Get ETM event failed because of other error
 */
esp_err_t mcpwm_comparator_new_etm_event(mcpwm_cmpr_handle_t cmpr, const mcpwm_cmpr_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief Get the ETM event for MCPWM timer
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] timer MCPWM timer, allocated by `mcpwm_new_timer()`
 * @param[in] config MCPWM ETM timer event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_FAIL: Get ETM event failed because of other error
 */
esp_err_t mcpwm_timer_new_etm_event(mcpwm_timer_handle_t timer, const mcpwm_timer_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

#ifdef __cplusplus
}
#endif
