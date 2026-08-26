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

/*
 *                                ISP Histogram Struct
 * |<----------------------------- INTERVAL_NUMS = 16 ------------------------------>|
 * |                     |                      |              |                     |
 * |       Segment 0     |      Segment 1       | ............ |      Segment 15     |
 * 0               threshold 0            threshold 1 ... threshold 14              255
 * |<------------------------------------------------------------------------------->|
 */

/**
 * @brief Hist controller config
 */
typedef struct {
    isp_window_t window;                                                            /*!< The sampling window of histogram, see `isp_window_t`*/
    isp_hist_sampling_mode_t hist_mode;                                             /*!< ISP histogram sampling mode */
    isp_hist_rgb_coefficient_t rgb_coefficient;                                       /*!< RGB coefficients, adjust the sensitivity to red, geen, and blue colors in the image,
                                                                                    only effect when hist_mode is ISP_HIST_SAMPLING_RGB, the sum of all coefficients decimal should be 256**/
    isp_hist_weight_t window_weight[ISP_HIST_BLOCK_X_NUM * ISP_HIST_BLOCK_Y_NUM];    /*!< Weights of histogram's each subwindows, the sum of all subwindows's weight decimal should be 256*/
    uint32_t segment_threshold[ISP_HIST_INTERVAL_NUMS];                             /*!< Threshold to segment the histogram into intervals, range 0~255 */
} esp_isp_hist_config_t;

/**
 * @brief New an ISP hist controller
 *
 * @param[in]  isp_proc   ISP Processor handle
 * @param[in]  hist_cfg    Pointer to hist config. Refer to ``esp_isp_hist_config_t``.
 * @param[out] ret_hdl    hist controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid
 *         - ESP_ERR_INVALID_STATE Invalid state
 *         - ESP_ERR_NOT_FOUND     No free interrupt found with the specified flags
 *         - ESP_ERR_NO_MEM        If out of memory
 */
esp_err_t esp_isp_new_hist_controller(isp_proc_handle_t isp_proc, const esp_isp_hist_config_t *hist_cfg, isp_hist_ctlr_t *ret_hdl);

/**
 * @brief Delete an ISP hist controller
 *
 * @param[in] hist_ctlr  hist controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_del_hist_controller(isp_hist_ctlr_t hist_ctlr);

/**
 * @brief Enable an ISP hist controller
 *
 * @param[in] hist_ctlr  hist controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_hist_controller_enable(isp_hist_ctlr_t hist_ctlr);

/**
 * @brief Disable an ISP hist controller
 *
 * @param[in] hist_ctlr  hist controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_hist_controller_disable(isp_hist_ctlr_t hist_ctlr);

/**
 * @brief Trigger hist reference statistics for one time and get the result
 *
 * @param[in]  hist_ctlr  hist controller handle
 * @param[in]  timeout_ms Timeout in millisecond
 *                            - timeout_ms < 0:   Won't return until finished
 *                            - timeout_ms = 0:   No timeout, trigger one time statistics and return immediately,
 *                                                in this case, the result won't be assigned in this function,
 *                                                but you can get the result in the callback `esp_isp_hist_cbs_t::on_statistics_done`
 *                            - timeout_ms > 0:   Wait for specified milliseconds, if not finished, then return timeout error
 * @param[out] out_res    hist reference statistics result
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_TIMEOUT       Wait for the result timeout
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_hist_controller_get_oneshot_statistics(isp_hist_ctlr_t hist_ctlr, int timeout_ms, isp_hist_result_t *out_res);

/**
 * @brief Start hist continuous statistics of the reference in the window
 * @note  This function is an asynchronous and non-block function,
 *        it will start the continuous statistics and return immediately.
 *        You have to register the hist callback and get the result from the callback event data.
 *
 * @param[in]  hist_ctlr  hist controller handle
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_hist_controller_start_continuous_statistics(isp_hist_ctlr_t hist_ctlr);

/**
 * @brief Stop hist continuous statistics of the reference in the window
 *
 * @param[in]  hist_ctlr  hist controller handle
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_hist_controller_stop_continuous_statistics(isp_hist_ctlr_t hist_ctlr);

/**
 * @brief Event data of callbacks
 */
typedef struct {
    isp_hist_result_t hist_result;    /*!< The histogram reference statistics result */
} esp_isp_hist_evt_data_t;

/**
 * @brief Prototype of ISP hist event callback
 *
 * @param[in] hist_ctlr    ISP hist controller handle
 * @param[in] edata     ISP hist event data
 * @param[in] user_data User registered context, registered when in `esp_isp_hist_register_event_callbacks()`
 *
 * @return Whether a high priority task is woken up by this function
 */
typedef bool (*esp_isp_hist_callback_t)(isp_hist_ctlr_t hist_ctlr, const esp_isp_hist_evt_data_t *edata, void *user_data);

/**
 * @brief Group of ISP hist callbacks
 *
 * @note These callbacks are all running in an ISR environment.
 * @note When CONFIG_ISP_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables should be in internal RAM as well.
 */
typedef struct {
    esp_isp_hist_callback_t on_statistics_done;    ///< Event callback, invoked when histogram statistic done.
} esp_isp_hist_cbs_t;

/**
 * @brief Register hist event callbacks
 *
 * @note User can deregister a previously registered callback by calling this function and setting the to-be-deregistered callback member in
 *       the `cbs` structure to NULL.
 * @note When CONFIG_ISP_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables (including `user_data`) should be in internal RAM as well.
 *
 * @param[in] hist_ctlr         hist controller handle
 * @param[in] cbs              Group of callback functions
 * @param[in] user_data        User data, which will be delivered to the callback functions directly
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t esp_isp_hist_register_event_callbacks(isp_hist_ctlr_t hist_ctlr, const esp_isp_hist_cbs_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
