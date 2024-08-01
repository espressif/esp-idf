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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AF controller config
 */
typedef struct {
    isp_window_t window[ISP_AF_WINDOW_NUM];            ///< The sampling windows of AF
    int edge_thresh;                                   ///< Edge threshold, definition higher than this value will be counted as a valid pixel for calculating AF result
    int intr_priority;                                 ///< The interrupt priority, range 0~3, if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3)
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
esp_err_t esp_isp_new_af_controller(isp_proc_handle_t isp_proc, const esp_isp_af_config_t *af_config, isp_af_ctlr_t *ret_hdl);

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
esp_err_t esp_isp_del_af_controller(isp_af_ctlr_t af_ctrlr);

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
esp_err_t esp_isp_af_controller_enable(isp_af_ctlr_t af_ctrlr);

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
esp_err_t esp_isp_af_controller_disable(isp_af_ctlr_t af_ctrlr);

/**
 * @brief Trigger AF luminance and definition statistics for one time and get the result
 *
 * @param[in]  af_ctrlr   AF controller handle
 * @param[in]  timeout_ms Timeout in millisecond
 *                            - timeout_ms < 0:   Won't return until finished
 *                            - timeout_ms = 0:   No timeout, trigger one time statistics and return immediately,
 *                                                in this case, the result won't be assigned in this function,
 *                                                but you can get the result in the callback `esp_isp_af_env_detector_evt_cbs_t::on_env_statistics_done`
 *                            - timeout_ms > 0:   Wait for specified milliseconds, if not finished, then return timeout error
 * @param[out] out_res    AF luminance and definition statistics result, can be NULL if `timeout_ms = 0`
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_TIMEOUT       If the waiting time exceeds the specified timeout.
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_get_oneshot_statistics(isp_af_ctlr_t af_ctrlr, int timeout_ms, isp_af_result_t *out_res);

/** @cond */
#define esp_isp_af_controller_get_oneshot_result(af_ctrlr, out_res)     \
        esp_isp_af_controller_get_oneshot_statistics(af_ctrlr, -1, out_res)    // Alias
/** @endcond */

/**
 * @brief Start AF continuous statistics of the luminance and definition in the windows
 * @note  This function is an asynchronous and non-block function,
 *        it will start the continuous statistics and return immediately.
 *        You have to register the AF callback and get the result from the callback event data.
 * @note  When continuous mode start, AF environment detector will be invalid
 *
 * @param[in]  af_ctrlr  AF controller handle
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_start_continuous_statistics(isp_af_ctlr_t af_ctrlr);

/**
 * @brief Stop AF continuous statistics of the luminance and definition in the windows
 *
 * @param[in]  af_ctrlr  AF controller handle
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_stop_continuous_statistics(isp_af_ctlr_t af_ctrlr);

/*---------------------------------------------
                AF Env Detector
----------------------------------------------*/
/**
 * @brief AF environment detector config
 */
typedef struct {
    int interval;    /*!< Interval between environment detection, in frames.
                      *   i.e., AF controller will trigger the statistic periodically to detect the environment change.
                      */
} esp_isp_af_env_config_t;

/**
 * @brief Set ISP AF environment detector
 *
 * @param[in] af_ctrlr    AF controller handle
 * @param[in] env_config  AF Env detector configuration
 *
 * @note    When continuous mode start, AF environment detector will be invalid
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_af_controller_set_env_detector(isp_af_ctlr_t af_ctrlr, const esp_isp_af_env_config_t *env_config);

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
esp_err_t esp_isp_af_controller_set_env_detector_threshold(isp_af_ctlr_t af_ctrlr, int definition_thresh, int luminance_thresh);

/**
 * @brief Event data structure
 */
typedef struct {
    isp_af_result_t af_result;      /*!< The AF statistics result */
} esp_isp_af_env_detector_evt_data_t;

/**
 * @brief Prototype of ISP AF Env detector event callback
 *
 * @param[in] af_ctrlr  ISP AF controller handle
 * @param[in] edata     ISP AF Env detector event data
 * @param[in] user_data User registered context, registered when in `esp_isp_af_env_detector_register_event_callbacks()`
 *
 * @return Whether a high priority task is woken up by this function
 */
typedef bool (*esp_isp_af_env_detector_callback_t)(isp_af_ctlr_t af_ctrlr, const esp_isp_af_env_detector_evt_data_t *edata, void *user_data);

/**
 * @brief Group of ISP AF Env detector callbacks
 *
 * @note These callbacks are all running in an ISR environment.
 * @note When CONFIG_ISP_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables should be in internal RAM as well.
 */
typedef struct {
    esp_isp_af_env_detector_callback_t on_env_statistics_done;  ///< Event callback, invoked when environment sample done.
    esp_isp_af_env_detector_callback_t on_env_change;       ///< Event callback, invoked when environment change happens.
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
esp_err_t esp_isp_af_env_detector_register_event_callbacks(isp_af_ctlr_t af_ctrlr, const esp_isp_af_env_detector_evt_cbs_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
