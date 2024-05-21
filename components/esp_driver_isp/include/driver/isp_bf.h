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
 * @brief ISP BF configurations
 */
typedef struct {
    isp_bf_edge_padding_mode_t padding_mode;                                 ///< BF edge padding mode
    uint8_t padding_data;                                                    ///< BF edge padding pixel data
    uint8_t bf_template[ISP_BF_TEMPLATE_X_NUMS][ISP_BF_TEMPLATE_Y_NUMS];     ///< BF template data
    uint8_t denoising_level;                                                 ///< BF denoising level, from 2 to 20, the bigger the better denoising performance, but the worse detailed
    uint8_t padding_line_tail_valid_start_pixel;                             ///< BF edge padding line tail valid start pixel, padding data will only be valid between the valid start pixel and the valid end pixel. Set both the start and end pixel to 0 to make all padding pixel valid
    uint8_t padding_line_tail_valid_end_pixel;                               ///< BF edge padding line tail valid end pixel, padding data will only be valid between the valid start pixel and the valid end pixel. Set both the start and end pixel to 0 to make all padding pixel valid
} esp_isp_bf_config_t;

/**
 * @brief ISP BF configuration
 *
 * @note After calling this API, BF doesn't take into effect until `esp_isp_bf_enable` is called
 *
 * @param[in] proc    Processor handle
 * @param[in] config  BF configurations, set NULL to de-configure the ISP BF
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_STATE  Not allowed to be called under current state
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 */
esp_err_t esp_isp_bf_configure(isp_proc_handle_t proc, const esp_isp_bf_config_t *config);

/**
 * @brief Enable ISP BF function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_bf_enable(isp_proc_handle_t proc);

/**
 * @brief Disable ISP BF function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_bf_disable(isp_proc_handle_t proc);

#ifdef __cplusplus
}
#endif
