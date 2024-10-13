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
 * @brief Color Correction Matrix configurations
 *
 */
typedef struct {
    float matrix[ISP_CCM_DIMENSION][ISP_CCM_DIMENSION];     /*!< The color correction matrix in float, range (-4.0, 4.0) */
    bool saturation;                                        /*!< Whether to use saturation when the float data in the matrix is out of the range,
                                                             *   For example, if one of the matrix data is 5.0,
                                                             *   When saturation is true, and final value will be limited to 4.0, and won't rise error
                                                             *   When saturation is false, `esp_isp_ccm_configure` will rise ESP_ERR_INVALID_ARG error
                                                             */
} esp_isp_ccm_config_t;

/**
 * @brief ISP Color Correction Matrix (CCM) configuration
 *
 * @note This function is allowed to be called before or after `esp_isp_ccm_enable`,
 *       but it only takes effect until `esp_isp_ccm_enable` is called
 *
 * @param[in] proc    Processor handle
 * @param[in] ccm_cfg CCM configurations
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 */
esp_err_t esp_isp_ccm_configure(isp_proc_handle_t proc, const esp_isp_ccm_config_t *ccm_cfg);

/**
 * @brief Enable ISP CCM function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 */
esp_err_t esp_isp_ccm_enable(isp_proc_handle_t proc);

/**
 * @brief Disable ISP CCM function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 */
esp_err_t esp_isp_ccm_disable(isp_proc_handle_t proc);

#ifdef __cplusplus
}
#endif
