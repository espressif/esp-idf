/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/isp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    WBG (White Balance Gain)
---------------------------------------------------------------*/
/**
 * @brief ISP BLC configurations
 */
typedef struct {
    struct {
        uint32_t update_once_configured : 1;    ///< If set, apply configuration to hardware immediately; otherwise defer to frame boundary
    } flags;                                    ///< Driver behaviour flags
} esp_isp_wbg_config_t;

/**
 * @brief ISP WBG configuration
 *
 * @note After calling this API, WBG doesn't take into effect until `esp_isp_wbg_enable` is called
 *
 * @param[in] isp_proc    Processor handle
 * @param[in] config      WBG configurations
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_STATE  Not allowed to be called under current state
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 *        - ESP_ERR_NOT_SUPPORTED  Not supported
 */
esp_err_t esp_isp_wbg_configure(isp_proc_handle_t isp_proc, const esp_isp_wbg_config_t *config);

/**
 * @brief Enable ISP WBG function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_wbg_enable(isp_proc_handle_t isp_proc);

/**
 * @brief Set AWB white balance gain
 *
 * @param[in] isp_proc  Processor handle
 * @param[in] gain      WBG white balance gain
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   Null pointer
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_wbg_set_wb_gain(isp_proc_handle_t isp_proc, isp_wbg_gain_t gain);

/**
 * @brief Disable ISP WBG function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_wbg_disable(isp_proc_handle_t isp_proc);

#ifdef __cplusplus
}
#endif
