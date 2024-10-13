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
 * @brief ISP Demosaic configurations
 */
typedef struct {
    isp_demosaic_grad_ratio_t grad_ratio;             /**< Demosaic gradient ratio,
                                                           - gradient_x * grad_ratio < gradient_y, use interpolation results in X direction
                                                           - gradient_y * grad_ratio < gradient_x, use interpolation results in Y direction
                                                           - else use the average results between X and Y
                                                      */
    isp_demosaic_edge_padding_mode_t padding_mode;    ///< Demosaic edge padding mode
    uint8_t padding_data;                             ///< Demosaic edge padding pixel data
    uint8_t padding_line_tail_valid_start_pixel;      ///< Demosaic edge padding line tail valid start pixel, padding data will only be valid between the valid start pixel and the valid end pixel. Set both the start and end pixel to 0 to make all padding pixel valid
    uint8_t padding_line_tail_valid_end_pixel;        ///< Demosaic edge padding line tail valid end pixel, padding data will only be valid between the valid start pixel and the valid end pixel. Set both the start and end pixel to 0 to make all padding pixel valid
} esp_isp_demosaic_config_t;

/**
 * @brief ISP Demosaic configuration
 *
 * @note After calling this API, Demosaic doesn't take into effect until `esp_isp_demosaic_enable` is called
 *
 * @param[in] proc    Processor handle
 * @param[in] config  Demosaic configurations, set NULL to de-configure the ISP Demosaic
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 */
esp_err_t esp_isp_demosaic_configure(isp_proc_handle_t proc, const esp_isp_demosaic_config_t *config);

/**
 * @brief Enable ISP Demosaic function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_demosaic_enable(isp_proc_handle_t proc);

/**
 * @brief Disable ISP Demosaic function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_demosaic_disable(isp_proc_handle_t proc);

#ifdef __cplusplus
}
#endif
