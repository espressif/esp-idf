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
 * @brief AWB controller config
 */
typedef struct {
    isp_awb_sample_point_t sample_point;                /*!< AWB sample point of the ISP pipeline.
                                                         *   ISP_AWB_SAMPLE_POINT_BEFORE_CCM: sample before Color Correction Matrix(CCM).
                                                         *   ISP_AWB_SAMPLE_POINT_AFTER_CCM: sample after Color Correction Matrix(CCM).
                                                         *   If your camera support to set the manual gain to the RGB channels,
                                                         *   then you can choose to sample before CCM, and set the gain to the camera registers.
                                                         *   If your camera doesn't support the manual gain or don't want to change the camera configuration,
                                                         *   then you can choose to sample after CCM, and set the calculated gain to the CCM
                                                         */
    isp_window_t window;                                /*!< Statistic window of AWB.
                                                         *   Suggest to set it at the middle of the image and a little smaller than the whole image.
                                                         *   It will be more reliable because the edges of image are easily to be overexposure,
                                                         *   the overexposure pixels are almost at maximum luminance,
                                                         *   which are not good references to calculate the gain for white balance.
                                                         */
    struct {
        isp_u32_range_t luminance;                     /*!< Luminance range of the white patch. Range [0, 255 * 3]
                                                         *   Not suggest to set the max value to 255 * 3,
                                                         *   because these pixels are too bright, very possible to be overexposure.
                                                         *   So the pixels that too bright should not be the reference of the white balance.
                                                         *   And the minimum value better to be 0 to allow the white balance work under low luminance environment.
                                                         */
        isp_float_range_t red_green_ratio;              /*!< Red to green ratio of the white patch. Range [0, 4.0).
                                                         *   The ratio could be as wider as possible,
                                                         *   so that all the distorted pixels will be counted for the reference of white balance.
                                                         */
        isp_float_range_t blue_green_ratio;             /*!< Blue to green ratio of the white patch. Range [0, 4.0)
                                                         *   The ratio could be as wider as possible,
                                                         *   so that all the distorted pixels will be counted for the reference of white balance.
                                                         */
    } white_patch;                                        /*!< white patch configuration */
    int intr_priority;                                  /*!< The interrupt priority, range 0~3, if set to 0, the driver will try to allocate an interrupt with
                                                         *   a relative low priority (1,2,3)
                                                         */
} esp_isp_awb_config_t;

/**
 * @brief New an ISP AWB controller
 *
 * @param[in]  isp_proc   ISP Processor handle
 * @param[in]  awb_cfg    Pointer to AWB config. Refer to ``esp_isp_awb_config_t``.
 * @param[out] ret_hdl    AWB controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid
 *         - ESP_ERR_INVALID_STATE Invalid state
 *         - ESP_ERR_NOT_FOUND     No free interrupt found with the specified flags
 *         - ESP_ERR_NO_MEM        If out of memory
 */
esp_err_t esp_isp_new_awb_controller(isp_proc_handle_t isp_proc, const esp_isp_awb_config_t *awb_cfg, isp_awb_ctlr_t *ret_hdl);

/**
 * @brief Delete an ISP AWB controller
 *
 * @param[in] awb_ctlr  AWB controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_del_awb_controller(isp_awb_ctlr_t awb_ctlr);

/**
 * @brief Reconfigure the ISP AWB controller
 * @note  This function is allowed to be called no matter the awb controller is enabled or not.
 *
 * @param[in] awb_ctlr  AWB controller handle
 * @param[in]  awb_cfg    Pointer to AWB config. Refer to ``esp_isp_awb_config_t``
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid
 */
esp_err_t esp_isp_awb_controller_reconfig(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_config_t *awb_cfg);

/**
 * @brief Enable an ISP AWB controller
 *
 * @param[in] awb_ctlr  AWB controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_awb_controller_enable(isp_awb_ctlr_t awb_ctlr);

/**
 * @brief Disable an ISP AWB controller
 *
 * @param[in] awb_ctlr  AWB controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_awb_controller_disable(isp_awb_ctlr_t awb_ctlr);

/**
 * @brief Trigger AWB white patch statistics for one time and get the result
 *
 * @param[in]  awb_ctlr  AWB controller handle
 * @param[in]  timeout_ms Timeout in millisecond
 *                            - timeout_ms < 0:   Won't return until finished
 *                            - timeout_ms = 0:   No timeout, trigger one time statistics and return immediately,
 *                                                in this case, the result won't be assigned in this function,
 *                                                but you can get the result in the callback `esp_isp_awb_cbs_t::on_statistics_done`
 *                            - timeout_ms > 0:   Wait for specified milliseconds, if not finished, then return timeout error
 * @param[out] out_res    AWB white patch statistics result
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_TIMEOUT       Wait for the result timeout
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_awb_controller_get_oneshot_statistics(isp_awb_ctlr_t awb_ctlr, int timeout_ms, isp_awb_stat_result_t *out_res);

/**
 * @brief Start AWB continuous statistics of the white patch in the window
 * @note  This function is an asynchronous and non-block function,
 *        it will start the continuous statistics and return immediately.
 *        You have to register the AWB callback and get the result from the callback event data.
 *
 * @param[in]  awb_ctlr  AWB controller handle
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_awb_controller_start_continuous_statistics(isp_awb_ctlr_t awb_ctlr);

/**
 * @brief Stop AWB continuous statistics of the white patch in the window
 *
 * @param[in]  awb_ctlr  AWB controller handle
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_awb_controller_stop_continuous_statistics(isp_awb_ctlr_t awb_ctlr);

/**
 * @brief Event data of callbacks
 */
typedef struct {
    isp_awb_stat_result_t awb_result;    /*!< The AWB white patch statistics result */
} esp_isp_awb_evt_data_t;

/**
 * @brief Prototype of ISP AWB event callback
 *
 * @param[in] awb_ctlr    ISP AWB controller handle
 * @param[in] edata     ISP AWB event data
 * @param[in] user_data User registered context, registered when in `esp_isp_awb_env_detector_register_event_callbacks()`
 *
 * @return Whether a high priority task is woken up by this function
 */
typedef bool (*esp_isp_awb_callback_t)(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_evt_data_t *edata, void *user_data);

/**
 * @brief Group of ISP AWB callbacks
 *
 * @note These callbacks are all running in an ISR environment.
 * @note When CONFIG_ISP_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables should be in internal RAM as well.
 */
typedef struct {
    esp_isp_awb_callback_t on_statistics_done;    ///< Event callback, invoked when white patches statistic done.
} esp_isp_awb_cbs_t;

/**
 * @brief Register AWB event callbacks
 *
 * @note User can deregister a previously registered callback by calling this function and setting the to-be-deregistered callback member in
 *       the `cbs` structure to NULL.
 * @note When CONFIG_ISP_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables (including `user_data`) should be in internal RAM as well.
 *
 * @param[in] awb_ctlr         AWB controller handle
 * @param[in] cbs              Group of callback functions
 * @param[in] user_data        User data, which will be delivered to the callback functions directly
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t esp_isp_awb_register_event_callbacks(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_cbs_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
