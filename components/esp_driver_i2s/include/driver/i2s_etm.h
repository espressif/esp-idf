/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "driver/i2s_types.h"
#include "hal/i2s_types.h"

#include "esp_etm.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I2S_SUPPORTS_ETM
/**
 * @brief I2S ETM event configuration
 */
typedef struct {
    i2s_etm_event_type_t event_type;    /*!< I2S ETM event type */
    uint32_t threshold;                 /*!< The threshold word number that triggers `I2S_ETM_EVENT_REACH_THRESH` event,
                                             only take effect when the event type is `I2S_ETM_EVENT_REACH_THRESH`
                                             Unit is in word (4 bytes) */
} i2s_etm_event_config_t;

/**
 * @brief Register the ETM event for I2S channel
 *
 * @note The created ETM event object can be deleted later by calling `esp_etm_del_event`
 *
 * @param[in] handle I2S channel handle, allocated by `i2s_new_channel`
 * @param[in] config I2S ETM event configuration
 * @param[out] out_event Returned ETM event handle
 * @return
 *      - ESP_OK: Get ETM event successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM event failed because of invalid argument
 *      - ESP_ERR_NOT_SUPPORTED: Get ETM event failed because the I2S hardware doesn't support ETM event
 *      - ESP_FAIL: Get ETM event failed because of other error
 */
esp_err_t i2s_new_etm_event(i2s_chan_handle_t handle, const i2s_etm_event_config_t *config, esp_etm_event_handle_t *out_event);

/**
 * @brief I2S ETM task configuration
 */
typedef struct {
    i2s_etm_task_type_t task_type; /*!< I2S ETM task type */
} i2s_etm_task_config_t;

/**
 * @brief Register the ETM task for I2S channel
 *
 * @note The created ETM task object can be deleted later by calling `esp_etm_del_task`
 *
 * @param[in] handle I2S channel handle, allocated by `i2s_new_channel`
 * @param[in] config I2S ETM task configuration
 * @param[out] out_task Returned ETM task handle
 * @return
 *      - ESP_OK: Get ETM task successfully
 *      - ESP_ERR_INVALID_ARG: Get ETM task failed because of invalid argument
 *      - ESP_ERR_NOT_SUPPORTED: Get ETM task failed because the i2s hardware doesn't support ETM task
 *      - ESP_FAIL: Get ETM task failed because of other error
 */
esp_err_t i2s_new_etm_task(i2s_chan_handle_t handle, const i2s_etm_task_config_t *config, esp_etm_task_handle_t *out_task);

#endif  // SOC_I2S_SUPPORTS_ETM

#ifdef __cplusplus
}
#endif
