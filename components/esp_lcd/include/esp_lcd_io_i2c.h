/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "driver/i2c_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t esp_lcd_i2c_bus_handle_t;                    /*!< Type of LCD I2C bus handle */

/**
 * @brief Panel IO configuration structure, for I2C interface
 *
 */
typedef struct {
    uint32_t dev_addr; /*!< I2C device address */
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; /*!< Callback invoked when color data transfer has finished */
    void *user_ctx; /*!< User private data, passed directly to on_color_trans_done's user_ctx */
    size_t control_phase_bytes; /*!< I2C LCD panel will encode control information (e.g. D/C selection) into control phase, in several bytes */
    unsigned int dc_bit_offset; /*!< Offset of the D/C selection bit in control phase */
    int lcd_cmd_bits;           /*!< Bit-width of LCD command */
    int lcd_param_bits;         /*!< Bit-width of LCD parameter */
    struct {
        unsigned int dc_low_on_data: 1;  /*!< If this flag is enabled, DC line = 0 means transfer data, DC line = 1 means transfer command; vice versa */
        unsigned int disable_control_phase: 1; /*!< If this flag is enabled, the control phase isn't used */
    } flags; /*!< Extra flags to fine-tune the I2C device */
    uint32_t scl_speed_hz; /*!< I2C LCD SCL frequency (hz) */
} esp_lcd_panel_io_i2c_config_t;

/**
 * @brief Create LCD panel IO handle, for I2C interface in legacy implementation
 *
 * @param[in] bus I2C bus handle, (in uint32_t)
 * @param[in] io_config IO configuration, for I2C interface
 * @param[out] ret_io Returned IO handle
 *
 * @note Please don't call this function in your project directly. Please call `esp_lcd_new_panel_to_i2c` instead.
 *
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_io_i2c_v1(uint32_t bus, const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);

/**
 * @brief Create LCD panel IO handle, for I2C interface in new implementation
 *
 * @param[in] bus I2C bus handle, (in i2c_master_dev_handle_t)
 * @param[in] io_config IO configuration, for I2C interface
 * @param[out] ret_io Returned IO handle
 *
 * @note Please don't call this function in your project directly. Please call `esp_lcd_new_panel_to_i2c` instead.
 *
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
esp_err_t esp_lcd_new_panel_io_i2c_v2(i2c_master_bus_handle_t bus, const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
/**
 * @brief Create LCD panel IO handle
 *
 * @param[in] bus I2C bus ID, indicates which I2C port to use
 * @param[in] io_config IO configuration, for I2C interface
 * @param[out] ret_io Returned IO handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
static inline esp_err_t esp_lcd_new_panel_io_i2c(uint32_t bus, const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
    return esp_lcd_new_panel_io_i2c_v1(bus, io_config, ret_io);
}

/**
 * @brief Create LCD panel IO handle
 *
 * @param[in] bus I2C bus handle, returned from `i2c_new_master_bus`
 * @param[in] io_config IO configuration, for I2C interface
 * @param[out] ret_io Returned IO handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
static inline esp_err_t esp_lcd_new_panel_io_i2c(i2c_master_bus_handle_t bus, const esp_lcd_panel_io_i2c_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io)
{
    return esp_lcd_new_panel_io_i2c_v2(bus, io_config, ret_io);
}
#else
/**
 * @brief Create LCD panel IO handle
 *
 * @param[in] bus I2C bus handle
 * @param[in] io_config IO configuration, for I2C interface
 * @param[out] ret_io Returned IO handle
 * @return
 *          - ESP_ERR_INVALID_ARG   if parameter is invalid
 *          - ESP_ERR_NO_MEM        if out of memory
 *          - ESP_OK                on success
 */
#define esp_lcd_new_panel_io_i2c(bus, io_config, ret_io) _Generic((bus),  \
            i2c_master_bus_handle_t : esp_lcd_new_panel_io_i2c_v2, \
            default : esp_lcd_new_panel_io_i2c_v1) (bus, io_config, ret_io)

#endif
