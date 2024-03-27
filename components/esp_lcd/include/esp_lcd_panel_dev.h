/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
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
    uint32_t bits_per_pixel;                   /*!< Color depth, in bpp */
    struct {
        uint32_t reset_active_high: 1; /*!< Setting this if the panel reset is high level active */
    } flags;                           /*!< LCD panel config flags */
    void *vendor_config; /*!< vendor specific configuration, optional, left as NULL if not used */
} esp_lcd_panel_dev_config_t;

#ifdef __cplusplus
}
#endif
