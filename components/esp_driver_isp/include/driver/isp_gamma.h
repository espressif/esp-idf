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
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ISP gamma Correction configuration
 *
 * @note This function is allowed to be called before or after esp_isp_gamma_enable(),
 *       but it only takes effect until esp_isp_gamma_enable() is called
 *
 * @param[in] proc      Processor handle
 * @param[in] component One of the R/G/B components, color_component_t
 * @param[in] pts       Group of points that describe the desired gamma correction curve;<br>
 *                      Passing in NULL to reset to default parameters (no correction)
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 */
esp_err_t esp_isp_gamma_configure(isp_proc_handle_t proc, color_component_t component, const isp_gamma_curve_points_t *pts);

/**
 * @brief Enable ISP gamma function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 */
esp_err_t esp_isp_gamma_enable(isp_proc_handle_t proc);

/**
 * @brief Disable ISP gamma function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 */
esp_err_t esp_isp_gamma_disable(isp_proc_handle_t proc);

/**
 * @brief Helper function to fill the isp_gamma_curve_points_t structure, giving the mathematical function of the desired gamma correction curve
 *
 * @note The raw values are sampled with equal spacing
 *
 * @param[in] gamma_correction_operator The desired gamma correction curve y = f(x).<br>
 *                                      x is the raw value, in [0, 256]; y is the gamma-corrected value, in [0, 256];<br>
 *                                      y can be equal to 256 only if x = 256. For any other x value, y should be always less than 256.
 * @param[out] pts Pointer to the to-be-filled isp_gamma_curve_points_t structure
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 */
esp_err_t esp_isp_gamma_fill_curve_points(uint32_t (* gamma_correction_operator)(uint32_t), isp_gamma_curve_points_t *pts);

#ifdef __cplusplus
}
#endif
