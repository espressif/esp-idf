/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_etm.h"
#include "driver/gptimer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPTimer ETM event configuration
 */
typedef struct {
    gptimer_etm_event_type_t event_type; /*!< GPTimer ETM event type */
} gptimer_etm_event_config_t;

/**
 * @brief Get the ETM event for GPTimer
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[in] config GPTimer ETM event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_FAIL: Get ETM event failed because of other error
 */
esp_err_t gptimer_new_etm_event(gptimer_handle_t timer, const gptimer_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief GPTimer ETM task configuration
 */
typedef struct {
    gptimer_etm_task_type_t task_type; /*!< GPTimer ETM task type */
} gptimer_etm_task_config_t;

/**
 * @brief Get the ETM task for GPTimer
 *
 * @note The created ETM task object can be deleted later by calling `esp_etm_del_task`
 *
 * @param[in] timer Timer handle created by `gptimer_new_timer`
 * @param[in] config GPTimer ETM task configuration
 * @param[out] out_task Returned ETM task handle
 * @return
 *      - ESP_OK: Get ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM task failed because of invalid argument
 *      - ESP_FAIL: Get ETM task failed because of other error
 */
esp_err_t gptimer_new_etm_task(gptimer_handle_t timer, const gptimer_etm_task_config_t *config, esp_etm_task_handle_t *out_task);

#ifdef __cplusplus
}
#endif
