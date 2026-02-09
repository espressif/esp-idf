/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_etm.h"
#include "hal/ledc_types.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LEDC_SUPPORT_ETM

/**
 * @brief LEDC timer ETM event configuration
 */
typedef struct {
    ledc_timer_etm_event_type_t event_type;     /*!< LEDC timer ETM event type */
} ledc_timer_etm_event_config_t;

/**
 * @brief LEDC channel ETM event configuration
 */
typedef struct {
    ledc_channel_etm_event_type_t event_type;   /*!< LEDC channel ETM event type */
} ledc_channel_etm_event_config_t;

/**
 * @brief Get the ETM timer event for LEDC
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] speed_mode Select the LEDC channel group with specified speed mode
 * @param[in] timer_sel LEDC timer index, select from ledc_timer_t
 * @param[in] config LEDC timer ETM event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Get ETM event failed because of no memory
 */
esp_err_t ledc_timer_new_etm_event(ledc_mode_t speed_mode, ledc_timer_t timer_sel, const ledc_timer_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief Get the ETM channel event for LEDC
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] speed_mode Select the LEDC channel group with specified speed mode
 * @param[in] channel LEDC channel index, select from ledc_channel_t
 * @param[in] config LEDC channel ETM event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Get ETM event failed because of no memory
 */
esp_err_t ledc_channel_new_etm_event(ledc_mode_t speed_mode, ledc_channel_t channel, const ledc_channel_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief LEDC timer ETM task configuration
 */
typedef struct {
    ledc_timer_etm_task_type_t task_type;       /*!< LEDC timer ETM task type */
} ledc_timer_etm_task_config_t;

/**
 * @brief LEDC channel ETM task configuration
 */
typedef struct {
    ledc_channel_etm_task_type_t task_type;     /*!< LEDC channel ETM task type */
} ledc_channel_etm_task_config_t;

/**
 * @brief Get the ETM timer task for LEDC
 *
 * @note The created ETM task object can be deleted later by calling `esp_etm_del_task`
 *
 * @param[in] speed_mode Select the LEDC channel group with specified speed mode
 * @param[in] timer_sel LEDC timer index, select from ledc_timer_t
 * @param[in] config LEDC timer ETM task configuration
 * @param[out] out_task Returned ETM task handle
 * @return
 *      - ESP_OK: Get ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM task failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Get ETM task failed because of no memory
 */
esp_err_t ledc_timer_new_etm_task(ledc_mode_t speed_mode, ledc_timer_t timer_sel, const ledc_timer_etm_task_config_t *config, esp_etm_task_handle_t *out_task);

/**
 * @brief Get the ETM channel task for LEDC
 *
 * @note The created ETM task object can be deleted later by calling `esp_etm_del_task`
 *
 * @param[in] speed_mode Select the LEDC channel group with specified speed mode
 * @param[in] channel LEDC channel index, select from ledc_channel_t
 * @param[in] config LEDC channel ETM task configuration
 * @param[out] out_task Returned ETM task handle
 * @return
 *      - ESP_OK: Get ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM task failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Get ETM task failed because of no memory
 */
esp_err_t ledc_channel_new_etm_task(ledc_mode_t speed_mode, ledc_channel_t channel, const ledc_channel_etm_task_config_t *config, esp_etm_task_handle_t *out_task);

#endif // SOC_LEDC_SUPPORT_ETM
#ifdef __cplusplus
}
#endif
