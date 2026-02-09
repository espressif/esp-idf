/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_panel_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SSD1306 configuration structure
 *
 * To be used as esp_lcd_panel_dev_config_t.vendor_config.
 * See esp_lcd_new_panel_ssd1306().
 */
typedef struct {
    /**
     * @brief Display's height in pixels (64(default) or 32)
     */
    uint8_t height;
    /**
     * @brief Display's contrast (128(default) or 0~255)
     */
    uint8_t contrast;
} esp_lcd_panel_ssd1306_config_t;

/**
 * @brief Create LCD panel for model SSD1306
 *
 * @param[in] io LCD panel IO handle
 * @param[in] panel_dev_config general panel device configuration
 * @param[out] ret_panel Returned LCD panel handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 *
 * @note The default panel size is 128x64.
 * @note Use esp_lcd_panel_ssd1306_config_t to set the correct size.
 * Example usage:
 * @code {c}
 *
 * esp_lcd_panel_ssd1306_config_t ssd1306_config = {
 *     .height = 32
 * };
 * esp_lcd_panel_dev_config_t panel_config = {
 *     <...>
 *     .vendor_config = &ssd1306_config
 * };
 *
 * esp_lcd_panel_handle_t panel_handle = NULL;
 * esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle);
 * @endcode
 */
esp_err_t esp_lcd_new_panel_ssd1306(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif
