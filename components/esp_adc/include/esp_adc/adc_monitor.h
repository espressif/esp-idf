/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "adc_continuous.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of adc digi monitor handle
 */
typedef struct adc_monitor_t *adc_monitor_handle_t;

/**
 * @brief ADC digital controller (DMA mode) monitor configuration.
 */
typedef struct {
    adc_unit_t adc_unit;            /*!<Set adc unit number for monitor. */
    adc_channel_t channel;          /*!<Set adc channel number for monitor. */
    int32_t h_threshold;            /*!<Set monitor high threshold of adc digital controller, -1 if not used. */
    int32_t l_threshold;            /*!<Set monitor low threshold of adc digital controller, -1 if not used. */
} adc_monitor_config_t;

/**
 * @brief Type of adc monitor event data
 */
typedef struct adc_monitor_evt_data {
    // reserved for extensibility
} adc_monitor_evt_data_t;

/**
 * @brief ADC digital controller (DMA mode) monitor callback type.
 */
typedef bool (*adc_monitor_evt_cb_t)(adc_monitor_handle_t monitor_handle, const adc_monitor_evt_data_t *event_data, void *user_data);

/**
 * @brief Struct type of many different adc_monitor evt callbacks.
 */
typedef struct {
    adc_monitor_evt_cb_t on_over_high_thresh;           /*!< adc_monitor high value interrupt callback */
    adc_monitor_evt_cb_t on_below_low_thresh;           /*!< adc_monitor low value interrupt callback */
} adc_monitor_evt_cbs_t;

/**
 * @brief Allocate an ADC continuous mode monitor (and configure it into an initial state)
 *
 * @param[in]  handle           ADC continuous mode driver handle
 * @param[in]  monitor_cfg      ADC monitor config struct
 * @param[out] ret_handle       Handle of allocated monitor
 *
 * @return
 *       - ESP_OK:                  On success
 *       - ESP_ERR_INVALID_ARG:     Invalid argument
 *       - ESP_ERR_INVALID_STATE：  Install monitor when ADC converter is running
 *       - ESP_ERR_NOT_FOUND：      No appropriate monitor Or no free monitor
 *       - ESP_ERR_NOT_SUPPORTED:   Threshold configuration not supported
 *       - ESP_ERR_NO_MEM:          Free memory not enough
 */
esp_err_t adc_new_continuous_monitor(adc_continuous_handle_t handle, const adc_monitor_config_t *monitor_cfg, adc_monitor_handle_t *ret_handle);

/**
 * @brief Register threshold interrupt callbacks for allocated monitor.
 *
 * @param[in]  monitor_handle       Monitor handle
 * @param[in]  cbs                  Pointer to a adc_monitor_evt_cbs_t struct
 * @param[in]  user_data            User data, which will be delivered to the callback functions directly
 *
 * @return
 *       - ESP_OK:                  On success
 *       - ESP_ERR_INVALID_STATE:   To register cbs when monitor is running, or cbs has been installed
 *       - ESP_ERR_NOT_SUPPORTED:   Register not supported callbacks to esp32s2
 *       - ESP_ERR_INVALID_ARG:     Invalid argument
 */
esp_err_t adc_continuous_monitor_register_event_callbacks(adc_monitor_handle_t monitor_handle, const adc_monitor_evt_cbs_t *cbs, void *user_data);

/**
 * @brief Enable an ADC continuous mode monitor.
 *
 * @param[in]  monitor_handle       Monitor handle
 *
 * @return
 *       - ESP_OK:                  On success
 *       - ESP_ERR_INVALID_STATE:   Monitor has enabled, no need to enable again
 *       - ESP_ERR_INVALID_ARG:     Invalid argument
 */
esp_err_t adc_continuous_monitor_enable(adc_monitor_handle_t monitor_handle);

/**
 * @brief Disable an ADC continuous mode monitor.
 *
 * @param[in]  monitor_handle       Monitor handle
 *
 * @return
 *       - ESP_OK:                  On success
 *       - ESP_ERR_INVALID_STATE:   Monitor not enabled, no need to disable
 *       - ESP_ERR_INVALID_ARG:     Invalid argument
 */
esp_err_t adc_continuous_monitor_disable(adc_monitor_handle_t monitor_handle);

/**
 * @brief Free an ADC continuous mode monitor.
 *
 * @param[in]  monitor_handle       Monitor handle
 *
 * @return
 *       - ESP_OK:                  On success
 *       - ESP_ERR_INVALID_STATE:   Monitor is in enabled state, should call `adc_continuous_monitor_disable` first
 *       - ESP_ERR_NOT_FOUND:       Monitor haven't been used
 *       - ESP_ERR_INVALID_ARG:     Invalid argument
 */
esp_err_t adc_del_continuous_monitor(adc_monitor_handle_t monitor_handle);

#ifdef __cplusplus
}
#endif
