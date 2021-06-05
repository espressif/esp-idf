/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct esp_lcd_panel_io_t esp_lcd_panel_io_t; /*!< Type of LCD panel IO */

/**
 * @brief LCD panel IO interface
 */
struct esp_lcd_panel_io_t {
    /**
     * @brief Transmit LCD command and corresponding parameters
     *
     * @note This is the panel-specific interface called by function `esp_lcd_panel_io_tx_param()`.
     *
     * @param[in] io LCD panel IO handle, which is created by other factory API like `esp_lcd_new_panel_io_spi()`
     * @param[in] lcd_cmd The specific LCD command
     * @param[in] lcd_cmd_bits Length of LCD command, in bits (e.g. 8 bits or 16 bits)
     * @param[in] param Buffer that holds the command specific parameters, set to NULL if no parameter is needed for the command
     * @param[in] param_size Size of `param` in memory, in bytes, set to zero if no parameter is needed for the command
     * @return
     *          - ESP_ERR_INVALID_ARG   if parameter is invalid
     *          - ESP_OK                on success
     */
    esp_err_t (*tx_param)(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *param, size_t param_size);

    /**
     * @brief Transmit LCD RGB data
     *
     * @note This is the panel-specific interface called by function `esp_lcd_panel_io_tx_color()`.
     *
     * @param[in] io LCD panel IO handle, which is created by other factory API like `esp_lcd_new_panel_io_spi()`
     * @param[in] lcd_cmd The specific LCD command
     * @param[in] lcd_cmd_bits Length of LCD command, in bits (e.g. 8 bits or 16 bits)
     * @param[in] color Buffer that holds the RGB color data
     * @param[in] color_size Size of `color` in memory, in bytes
     * @return
     *          - ESP_ERR_INVALID_ARG   if parameter is invalid
     *          - ESP_OK                on success
     */
    esp_err_t (*tx_color)(esp_lcd_panel_io_t *io, int lcd_cmd, int lcd_cmd_bits, const void *color, size_t color_size);

    /**
     * @brief Destory LCD panel IO handle (deinitialize all and free resource)
     *
     * @param[in] io LCD panel IO handle, which is created by other factory API like `esp_lcd_new_panel_io_spi()`
     * @return
     *          - ESP_ERR_INVALID_ARG   if parameter is invalid
     *          - ESP_OK                on success
     */
    esp_err_t (*del)(esp_lcd_panel_io_t *io);
};

#ifdef __cplusplus
}
#endif
