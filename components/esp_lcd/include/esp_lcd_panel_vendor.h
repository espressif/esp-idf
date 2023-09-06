/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for panel device
 */
typedef struct {
    int reset_gpio_num; /*!< GPIO used to reset the LCD panel, set to -1 if it's not used */
    union {
        lcd_rgb_element_order_t color_space;   /*!< @deprecated Set RGB color space, please use rgb_ele_order instead */
        lcd_rgb_element_order_t rgb_endian;    /*!< @deprecated Set RGB data endian, please use rgb_ele_order instead */
        lcd_rgb_element_order_t rgb_ele_order; /*!< Set RGB element order, RGB or BGR */
    };
    lcd_rgb_data_endian_t data_endian;         /*!< Set the data endian for color data larger than 1 byte */
    unsigned int bits_per_pixel;       /*!< Color depth, in bpp */
    struct {
        unsigned int reset_active_high: 1; /*!< Setting this if the panel reset is high level active */
    } flags;                               /*!< LCD panel config flags */
    void *vendor_config; /*!< vendor specific configuration, optional, left as NULL if not used */
} esp_lcd_panel_dev_config_t;

/**
 * @brief Create LCD panel for model ST7789
 *
 * @param[in] io LCD panel IO handle
 * @param[in] panel_dev_config general panel device configuration
 * @param[out] ret_panel Returned LCD panel handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_st7789(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);

/**
 * @brief Create LCD panel for model NT35510
 *
 * @param[in] io LCD panel IO handle
 * @param[in] panel_dev_config general panel device configuration
 * @param[out] ret_panel Returned LCD panel handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_nt35510(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);

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
 */
esp_err_t esp_lcd_new_panel_ssd1306(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif
