/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "driver/isp_types.h"
#include "driver/isp_dpc_dynamic.h"
#include "driver/isp_dpc_static.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------
                    DPC (Dead Pixel Correction)
---------------------------------------------------------------*/
/**
 * @brief ISP DPC common configuration
 *
 * This configuration controls common DPC behavior only. Configure static LUT
 * coordinates with esp_isp_dpc_static_configure() and dynamic algorithm
 * parameters with esp_isp_dpc_dynamic_configure().
 */
typedef struct {
    struct {
        uint32_t update_once_configured : 1; ///< If set, apply configuration to hardware immediately; otherwise defer to frame boundary
    } flags;                                ///< Driver behavior flags
} esp_isp_dpc_config_t;

/**
 * @brief Configure common ISP DPC settings
 *
 * @note Call this while DPC is disabled, after configuring any required static
 *       and dynamic correction paths, and before esp_isp_dpc_enable(). This API
 *       configures the common clock and shadow-register update behavior; it does
 *       not configure or enable static or dynamic correction.
 *
 * @param[in] isp_proc Processor handle
 * @param[in] config DPC common configuration
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_STATE  DPC is enabled
 *        - ESP_ERR_INVALID_ARG    Invalid argument
 */
esp_err_t esp_isp_dpc_configure(isp_proc_handle_t isp_proc, const esp_isp_dpc_config_t *config);

/**
 * @brief Enable ISP DPC function
 *
 * @note Call `esp_isp_dpc_static_configure` and/or `esp_isp_dpc_dynamic_configure`,
 *       then `esp_isp_dpc_configure`, first. Static and dynamic correction can be
 *       configured together.
 *       Static calibration is a separate flow started by
 *       `esp_isp_dpc_static_calibration_start_once`; that API enables DPC internally.
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_dpc_enable(isp_proc_handle_t isp_proc);

/**
 * @brief Disable ISP DPC function
 *
 * @param[in] isp_proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_dpc_disable(isp_proc_handle_t isp_proc);

#ifdef __cplusplus
}
#endif
