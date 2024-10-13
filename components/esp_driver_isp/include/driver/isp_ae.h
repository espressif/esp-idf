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
 * @brief AE controller config
 */
typedef struct {
    isp_ae_sample_point_t   sample_point;           ///< The input data source, ISP_AE_SAMPLE_POINT_AFTER_DEMOSAIC: AE input data after demosaic, ISP_AE_SAMPLE_POINT_AFTER_GAMMA: AE input data after gamma
    isp_window_t            window;                 ///< The sampling windows of AE
    int                     intr_priority;          ///< The interrupt priority, range 0~3, if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3)
} esp_isp_ae_config_t;

/**
 * @brief New an ISP AE controller
 *
 * @param[in]  isp_proc   ISP Processor handle
 * @param[in]  ae_config  Pointer to AE config. Refer to ``esp_isp_ae_config_t``.
 * @param[out] ret_hdl    AE controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid
 *         - ESP_ERR_INVALID_STATE Invalid state
 *         - ESP_ERR_NOT_FOUND     No free interrupt found with the specified flags
 *         - ESP_ERR_NO_MEM        If out of memory
 */
esp_err_t esp_isp_new_ae_controller(isp_proc_handle_t isp_proc, const esp_isp_ae_config_t *ae_config, isp_ae_ctlr_t *ret_hdl);

/**
 * @brief Delete an ISP AE controller
 *
 * @param[in] ae_ctlr  AE controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_del_ae_controller(isp_ae_ctlr_t ae_ctlr);

/**
 * @brief Enable an ISP AE controller
 *
 * @param[in] ae_ctlr  AE controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_ae_controller_enable(isp_ae_ctlr_t ae_ctlr);

/**
 * @brief Disable an ISP AE controller
 *
 * @param[in] ae_ctlr  AE controller handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_ae_controller_disable(isp_ae_ctlr_t ae_ctlr);

/**
 * @brief Trigger AE luminance statistics for one time and get the result
 *
 * @param[in]  ae_ctlr   AE controller handle
 * @param[in]  timeout_ms Timeout in millisecond
 *                            - timeout_ms < 0:   Won't return until finished
 *                            - timeout_ms = 0:   No timeout, trigger one time statistics and return immediately,
 *                                                in this case, the result won't be assigned in this function,
 *                                                but you can get the result in the callback `esp_isp_ae_env_detector_evt_cbs_t::on_env_statistics_done`
 *                            - timeout_ms > 0:   Wait for specified milliseconds, if not finished, then return timeout error
 * @param[out] out_res    AE luminance statistics result, can be NULL if `timeout_ms = 0`
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_TIMEOUT       If the waiting time exceeds the specified timeout.
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_ae_controller_get_oneshot_statistics(isp_ae_ctlr_t ae_ctlr, int timeout_ms, isp_ae_result_t *out_res);

/**
 * @brief Start AE continuous statistics of the luminance in the windows
 * @note  This function is an asynchronous and non-block function,
 *        it will start the continuous statistics and return immediately.
 *        You have to register the AE callback and get the result from the callback event data.
 * @note  When using oneshot statistics, the AE Environment Detector will be temporarily disabled
 *        and will automatically recover once the oneshot is complete.
 * @param[in]  ae_ctlr  AE controller handle
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_ae_controller_start_continuous_statistics(isp_ae_ctlr_t ae_ctlr);

/**
 * @brief Stop AE continuous statistics of the luminance in the windows
 *
 * @param[in]  ae_ctlr  AE controller handle
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_ae_controller_stop_continuous_statistics(isp_ae_ctlr_t ae_ctlr);

/*---------------------------------------------
                AE env detector
----------------------------------------------*/
/**
 * @brief AE environment detector config
 */
typedef struct {
    int interval;       /*!< Interval between environment detection, in frames.
                         *   i.e., AE controller will trigger the statistic periodically to detect the environment change.
                         */
} esp_isp_ae_env_config_t;

/**
 * @brief AE environment detector config
 */
typedef struct {
    int low_thresh;         /*!< Low threshold for AE environment detector luminance */
    int high_thresh;        /*!< High threshold for AE environment detector luminance */
} esp_isp_ae_env_thresh_t;

/**
 * @brief Set ISP AE environment detector
 *
 * @param[in] ae_ctlr    AE controller handle
 * @param[in] env_config  AE Env detector configuration
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_ae_controller_set_env_detector(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_config_t *env_config);

/**
 * @brief Set ISP AE environment detector detecting threshold
 *
 * @param[in] ae_ctlr           AE controller handle
 * @param[in] env_thresh         Luminance thresholds for AE env detector
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_ae_controller_set_env_detector_threshold(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_thresh_t *env_thresh);

/**
 * @brief Event data structure
 */
typedef struct {
    isp_ae_result_t ae_result;      /*!< The AE statistics result */
} esp_isp_ae_env_detector_evt_data_t;

/**
 * @brief Prototype of ISP AE Env detector event callback
 *
 * @param[in] ae_ctlr  ISP AE controller handle
 * @param[in] edata     ISP AE Env detector event data
 * @param[in] user_data User registered context, registered when in `esp_isp_ae_env_detector_register_event_callbacks()`
 *
 * @return Whether a high priority task is woken up by this function
 */
typedef bool (*esp_isp_ae_env_detector_callback_t)(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_detector_evt_data_t *edata, void *user_data);

/**
 * @brief Group of ISP AE env_detector
 * @note These callbacks are all running in an ISR environment.
 * @note When CONFIG_ISP_ISR_IRAM_SAEE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables should be in internal RAM as well.
 */
typedef struct {
    esp_isp_ae_env_detector_callback_t on_env_statistics_done;      ///< Event callback, invoked when environment sample done.
    esp_isp_ae_env_detector_callback_t on_env_change;       ///< Event callback, invoked when environment change happens.
} esp_isp_ae_env_detector_evt_cbs_t;

/**
 * @brief Register AE Env detector event callbacks
 *
 * @note User can deregister a previously registered callback by calling this function and setting the to-be-deregistered callback member in
 *       the `cbs` structure to NULL.
 * @note When CONFIG_ISP_ISR_IRAM_SAEE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       Involved variables (including `user_data`) should be in internal RAM as well.
 *
 * @param[in] ae_ctlr         AE controller handle
 * @param[in] cbs              Group of callback functions
 * @param[in] user_data        User data, which will be delivered to the callback functions directly
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t esp_isp_ae_env_detector_register_event_callbacks(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_detector_evt_cbs_t *cbs, void *user_data);

#ifdef __cplusplus
}
#endif
