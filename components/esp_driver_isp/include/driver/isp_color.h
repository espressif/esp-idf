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
 * @brief ISP color configurations
 */
typedef struct {
    isp_color_contrast_t color_contrast;          /*!< The color contrast value, defines the contrast level of the image,
                                                   *   which controls the difference in luminance between the lightest and darkest parts of the image
                                                   *   Range 0 ~ 1, decimal value should be 0~127, default 1
                                                   */
    isp_color_saturation_t color_saturation;      /*!< The color saturation value, controls the intensity of colors in the image,
                                                   *   affecting how vivid or muted the colors appear.
                                                   *   Range 0 ~ 1, decimal value should be 0~127, default 1
                                                   */
    uint32_t color_hue;                           /*!< The color hue value, based on the color wheel.
                                                   *   0 degrees represents red, 120 degrees represents green, and 240 degrees represents blue. 360 degrees overlaps with 0 degrees
                                                   *   Range 0 ~ 360, default 0.
                                                   */
    int color_brightness;                         /*!< The color brightness value.
                                                   *   Range -128 ~ 127, default 0.
                                                   *   Negative range (-128 to -1): Decreases brightness, the smaller the value, the darker the image.
                                                   *   Zero (0): Maintains the original brightness, without adjusting the image's brightness.
                                                   *   Positive range (1 to 127): Increases brightness, the larger the value, the brighter the image.
                                                   */
} esp_isp_color_config_t;

/**
 * @brief ISP Color configuration
 *
 * @note After calling this API, Color doesn't take into effect until `esp_isp_color_enable` is called
 * @note API is ISR available
 *
 * @param[in] proc    Processor handle
 * @param[in] config  Color configurations, set NULL to de-configure the ISP Color
 *
 * @return
 *        - ESP_OK                 On success
 *        - ESP_ERR_INVALID_ARG    If the combination of arguments is invalid
 */
esp_err_t esp_isp_color_configure(isp_proc_handle_t proc, const esp_isp_color_config_t *config);

/**
 * @brief Enable ISP color function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_color_enable(isp_proc_handle_t proc);

/**
 * @brief Disable ISP color function
 *
 * @param[in] proc  Processor handle
 *
 * @return
 *         - ESP_OK                On success
 *         - ESP_ERR_INVALID_ARG   If the combination of arguments is invalid.
 *         - ESP_ERR_INVALID_STATE Driver state is invalid.
 */
esp_err_t esp_isp_color_disable(isp_proc_handle_t proc);

#ifdef __cplusplus
}
#endif
