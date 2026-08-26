/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_etm.h"
#include "hal/lp_core_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LP-Core ETM event configuration
 */
typedef struct {
    lp_core_etm_event_type_t event_type;     /*!< LP-Core ETM event type */
} lp_core_etm_event_config_t;

/**
 * @brief LP-Core ETM task configuration
 */
typedef struct {
    lp_core_etm_task_type_t task_type;    /*!< LP-Core ETM task type */
} lp_core_etm_task_config_t;

/**
 * @brief Create a ETM event for LP-Core
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] config LP-Core ETM event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_FAIL: Get ETM event failed because of other error
 */
esp_err_t lp_core_new_etm_event(const lp_core_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief Create a ETM task for LP-Core
 *
 * @note The created ETM task object can be deleted later by calling `esp_etm_del_task`
 *
 * @param[in] config LP-Core ETM task configuration
 * @param[out] out_task Returned ETM task handle
 * @return
 *      - ESP_OK: Get ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM task failed because of invalid argument
 *      - ESP_FAIL: Get ETM task failed because of other error
 */
esp_err_t lp_core_new_etm_task(const lp_core_etm_task_config_t *config, esp_etm_task_handle_t *out_task);

#ifdef __cplusplus
}
#endif
