/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

/**
 * @brief ISP Sharpen configurations
 */
typedef struct {
    isp_sharpen_h_freq_coeff h_freq_coeff;                                                  ///< High freq pixel sharpeness coeff
    isp_sharpen_m_freq_coeff m_freq_coeff;                                                  ///< Medium freq pixel sharpeness coeff
    uint8_t h_thresh;                                                                       ///< High threshold, pixel value higher than this threshold will be multiplied by `h_freq_coeff`
    uint8_t l_thresh;                                                                       ///< Low threshold, pixel value higher than this threshold but lower than `h_thresh` will be multiplied by `m_freq_coeff`. Pixel value lower than this threshold will be set to 0
    isp_sharpen_edge_padding_mode_t padding_mode;                                           ///< Sharpen edge padding mode
    uint8_t padding_data;                                                                   ///< Sharpen edge padding pixel data
    uint8_t sharpen_template[ISP_SHARPEN_TEMPLATE_X_NUMS][ISP_SHARPEN_TEMPLATE_Y_NUMS];     ///< Sharpen template data
    uint8_t padding_line_tail_valid_start_pixel;                                            ///< Sharpen edge padding line tail valid start pixel, padding data will only be valid between the valid start pixel and the valid end pixel. Set both the start and end pixel to 0 to make all padding pixel valid
    uint8_t padding_line_tail_valid_end_pixel;                                              ///< Sharpen edge padding line tail valid end pixel, padding data will only be valid between the valid start pixel and the valid end pixel. Set both the start and end pixel to 0 to make all padding pixel valid
} esp_isp_sharpen_config_t;

/**
 * @brief ISP Sharpen configuration
 *
 * @note After calling this API, sharpen doesn't take into effect until `esp_isp_sharpen_enable` is called
 *
 * @param[in] proc    Processor handle
 * @param[in] config  Sharpen configurations, set NULL to de-configure the ISP Sharpen
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 */
esp_err_t esp_isp_sharpen_configure(isp_proc_handle_t proc, const esp_isp_sharpen_config_t *config);

/**
 * @brief Enable ISP Sharpen function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_sharpen_enable(isp_proc_handle_t proc);

/**
 * @brief Disable ISP Sharpen function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_sharpen_disable(isp_proc_handle_t proc);

#ifdef __cplusplus
}
#endif
