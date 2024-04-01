/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/isp_types.h"
#include "driver/isp.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AF controller config
 */
typedef struct {
#if SOC_ISP_AF_WINDOW_NUMS
    isp_af_window_t window[SOC_ISP_AF_WINDOW_NUMS];    ///< AF window settings
#endif
    int edge_thresh;                                   ///< Edge threshold, definition higher than this value will be counted as a valid pixel for calculating AF result
} esp_isp_af_config_t;

/**
 * @brief New an ISP AF controller
 *
 * @param[in]  isp_proc   ISP Processor handle
 * @param[in]  af_config  Pointer to AF config. Refer to ``esp_isp_af_config_t``.
 * @param[out] ret_hdl    AF controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid
 *         - ESP_ERR_INVALID_STATE Invalid state
 *         - ESP_ERR_NOT_FOUND     No free interrupt found with the specified flags
 *         - ESP_ERR_NO_MEM        If out of memory
 */
esp_err_t esp_isp_new_af_controller(isp_proc_handle_t isp_proc, const esp_isp_af_config_t *af_config, isp_af_ctrlr_t *ret_hdl);

/**
 * @brief Delete an ISP AF controller
 *
 * @param[in] af_ctrlr  AF controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_del_af_controller(isp_af_ctrlr_t af_ctrlr);

/**
 * @brief Enable an ISP AF controller
 *
 * @param[in] af_ctrlr  AF controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_enable(isp_af_ctrlr_t af_ctrlr);

/**
 * @brief Disable an ISP AF controller
 *
 * @param[in] af_ctrlr  AF controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_disable(isp_af_ctrlr_t af_ctrlr);

/**
 * @brief Get AF result
 *
 * @param[in]  af_ctrlr  AF controller handle
 * @param[out] out_res   AF result
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_get_oneshot_result(isp_af_ctrlr_t af_ctrlr, isp_af_result_t *out_res);

/*---------------------------------------------
                AF Env Monitor
----------------------------------------------*/
/**
 * @brief AF environment detector config
 */
typedef struct {
    int interval;    ///< Interval between environment detection, in frames
} esp_isp_af_env_config_t;

/**
 * @brief Set ISP AF environment detector
 *
 * @param[in] af_ctrlr    AF controller handle
 * @param[in] env_config  AF Env detector configuration
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_set_env_detector(isp_af_ctrlr_t af_ctrlr, const esp_isp_af_env_config_t *env_config);

/**
 * @brief Set ISP AF environment detector detecting threshold
 *
 * @param[in] af_ctrlr           AF controller handle
 * @param[in] definition_thresh  Threshold for definition
 * @param[in] luminance_thresh   Threshold for luminance
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_set_env_detector_threshold(isp_af_ctrlr_t af_ctrlr, int definition_thresh, int luminance_thresh);

/**
 * @brief Event data structure
 */
typedef struct {
    //empty for future proof
} esp_isp_af_env_detector_evt_data_t;

/**
 * @brief Prototype of ISP AF Env detector event callback
 *
 * @param[in] handle    ISP AF controller handle
 * @param[in] edata     ISP AF Env detector event data
 * @param[in] user_data User registered context, registered when in `esp_isp_af_env_detector_register_event_callbacks()`
 *
 * @return Whether a high priority task is woken up by this function
 */
typedef bool (*esp_isp_af_env_detector_callback_t)(isp_af_ctrlr_t af_ctrlr, const esp_isp_af_env_detector_evt_data_t *edata, void *user_data);

/**
 * @brief Group of ISP AF Env detector callbacks
 *
 * @note These callbacks are all running in an ISR environment.
 * @note When CONFIG_ISP_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables should be in internal RAM as well.
 */
typedef struct {
    esp_isp_af_env_detector_callback_t on_env_change;    ///< Event callback, invoked when environment change happens.
} esp_isp_af_env_detector_evt_cbs_t;

/**
 * @brief Register AF environment detector event callbacks
 *
 * @note User can deregister a previously registered callback by calling this function and setting the to-be-deregistered callback member in
 *       the `cbs` structure to NULL.
 * @note When CONFIG_ISP_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables (including `user_data`) should be in internal RAM as well.
 *
 * @param[in] af_ctrlr         AF controller handle
 * @param[in] cbs              Group of callback functions
 * @param[in] user_data        User data, which will be delivered to the callback functions directly
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t esp_isp_af_env_detector_register_event_callbacks(isp_af_ctrlr_t af_ctrlr, const esp_isp_af_env_detector_evt_cbs_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
