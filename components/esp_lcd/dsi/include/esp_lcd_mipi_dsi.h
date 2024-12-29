/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_lcd_types.h"

typedef struct esp_lcd_dsi_bus_t *esp_lcd_dsi_bus_handle_t;   /*!< Type of MIPI DSI bus handle */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MIPI DSI bus configuration structure
 */
typedef struct {
    int bus_id;                              /*!< Select which DSI controller, index from 0 */
    uint8_t num_data_lanes;                  /*!< Number of data lanes, if set to 0, the driver will fallback to use maximum number of lanes */
    mipi_dsi_phy_clock_source_t phy_clk_src; /*!< MIPI DSI PHY clock source */
    uint32_t lane_bit_rate_mbps;             /*!< Lane bit rate in Mbps */
} esp_lcd_dsi_bus_config_t;

/**
 * @brief Create MIPI DSI bus handle
 *
 * @param[in] bus_config Bus configuration
 * @param[out] ret_bus Returned bus handle
 * @return
 *      - ESP_OK: Create MIPI DSI bus successfully
 *      - ESP_ERR_INVALID_ARG: Create MIPI DSI bus failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MIPI DSI bus failed because of out of memory
 *      - ESP_ERR_NOT_FOUND: Create MIPI DSI bus failed because no more free DSI hardware instance
 *      - ESP_FAIL: Create MIPI DSI bus failed because of other error
 */
esp_err_t esp_lcd_new_dsi_bus(const esp_lcd_dsi_bus_config_t *bus_config, esp_lcd_dsi_bus_handle_t *ret_bus);

/**
 * @brief Destroy MIPI DSI bus handle
 *
 * @param[in] bus MIPI DSI bus handle, returned from `esp_lcd_new_dsi_bus`
 * @return
 *      - ESP_OK: Destroy MIPI DSI bus successfully
 *      - ESP_ERR_INVALID_ARG: Destroy MIPI DSI bus failed because of invalid argument
 *      - ESP_FAIL: Destroy MIPI DSI bus failed because of other error
 */
esp_err_t esp_lcd_del_dsi_bus(esp_lcd_dsi_bus_handle_t bus);

/**
 * @brief Panel IO configuration structure, for MIPI DSI command interface
 */
typedef struct {
    uint8_t virtual_channel;  /*!< Virtual channel ID, index from 0 */
    int lcd_cmd_bits;         /*!< Bit-width of LCD command */
    int lcd_param_bits;       /*!< Bit-width of LCD parameter */
} esp_lcd_dbi_io_config_t;

/**
 * @brief Create LCD panel IO, for MIPI DSI DBI interface
 *
 * @note Although we call it "DBI", internally the driver is using a co-called "generic" interface for transmitting/receiving LCD commands and parameters.
 *
 * @param[in] bus MIPI DSI bus handle, returned from `esp_lcd_new_dsi_bus`
 * @param[in] io_config IO configuration
 * @param[out] ret_io Returned panel IO handle
 * @return
 *      - ESP_OK: Create MIPI DSI command IO successfully
 *      - ESP_ERR_INVALID_ARG: Create MIPI DSI command IO failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MIPI DSI command IO failed because of out of memory
 *      - ESP_FAIL: Create MIPI DSI command IO failed because of other error
 */
esp_err_t esp_lcd_new_panel_io_dbi(esp_lcd_dsi_bus_handle_t bus, const esp_lcd_dbi_io_config_t *io_config, esp_lcd_panel_io_handle_t *ret_io);

/**
 * @brief MIPI DSI DPI panel configuration structure
 */
typedef struct {
    uint8_t virtual_channel;                   /*!< Virtual channel ID, index from 0 */
    mipi_dsi_dpi_clock_source_t dpi_clk_src;   /*!< MIPI DSI DPI clock source */
    uint32_t dpi_clock_freq_mhz;               /*!< DPI clock frequency in MHz */
    lcd_color_rgb_pixel_format_t pixel_format; /*!< Pixel format that used by the MIPI LCD device */
    lcd_color_format_t in_color_format;        /*!< Format of the input data (color space and pixel format),
                                                    which is the format stored in the frame buffer */
    lcd_color_format_t out_color_format;       /*!< Format of the output data (color space and pixel format),
                                                    which is the format that the panel can accept */
    uint8_t num_fbs;                           /*!< Number of screen-sized frame buffers that allocated by the driver
                                                    By default (set to either 0 or 1) only one frame buffer will be created */
    esp_lcd_video_timing_t video_timing;       /*!< Video timing */
    /// Extra configuration flags for MIPI DSI DPI panel
    struct extra_dpi_panel_flags {
        uint32_t use_dma2d: 1; /*!< Use DMA2D to copy user buffer to the frame buffer when necessary */
        uint32_t disable_lp: 1;/*!< Disable low-power for DPI */
    } flags;                   /*!< Extra configuration flags */
} esp_lcd_dpi_panel_config_t;

/**
 * @brief Create LCD panel for MIPI DSI DPI interface
 *
 * @param[in] bus MIPI DSI bus handle, returned from `esp_lcd_new_dsi_bus`
 * @param[in] panel_config DSI data panel configuration
 * @param[out] ret_panel Returned LCD panel handle
 * @return
 *      - ESP_OK: Create MIPI DSI data panel successfully
 *      - ESP_ERR_INVALID_ARG: Create MIPI DSI data panel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create MIPI DSI data panel failed because of out of memory
 *      - ESP_ERR_NOT_SUPPORTED: Create MIPI DSI data panel failed because of unsupported feature
 *      - ESP_FAIL: Create MIPI DSI data panel failed because of other error
 */
esp_err_t esp_lcd_new_panel_dpi(esp_lcd_dsi_bus_handle_t bus, const esp_lcd_dpi_panel_config_t *panel_config, esp_lcd_panel_handle_t *ret_panel);

/**
 * @brief Get the address of the frame buffer(s) that allocated by the driver
 *
 * @param[in] dpi_panel MIPI DPI panel handle, returned from esp_lcd_new_panel_dpi()
 * @param[in] fb_num Number of frame buffer(s) to get. This value must be the same as the number of the followed parameters.
 * @param[out] fb0 Address of the frame buffer 0 (first frame buffer)
 * @param[out] ... List of other frame buffers if any
 * @return
 *      - ESP_ERR_INVALID_ARG: Get frame buffer address failed because of invalid argument
 *      - ESP_OK: Get frame buffer address successfully
 */
esp_err_t esp_lcd_dpi_panel_get_frame_buffer(esp_lcd_panel_handle_t dpi_panel, uint32_t fb_num, void **fb0, ...);

/**
 * @brief Set pre-defined pattern to the screen for testing or debugging purpose
 *
 * @param[in] dpi_panel MIPI DPI panel handle, returned from esp_lcd_new_panel_dpi()
 * @param[in] pattern Pattern type
 * @return
 *      - ESP_OK: Set pattern successfully
 *      - ESP_ERR_INVALID_ARG: Set pattern failed because of invalid argument
 *      - ESP_FAIL: Set pattern failed because of other error
 */
esp_err_t esp_lcd_dpi_panel_set_pattern(esp_lcd_panel_handle_t dpi_panel, mipi_dsi_pattern_type_t pattern);

/**
 * @brief Set color conversion configuration for DPI panel
 *
 * @param[in] dpi_panel MIPI DPI panel handle, returned from esp_lcd_new_panel_dpi()
 * @param[in] config Color conversion configuration
 * @return
 *      - ESP_OK: Set color conversion configuration successfully
 *      - ESP_ERR_INVALID_ARG: Set color conversion configuration failed because of invalid argument
 *      - ESP_FAIL: Set color conversion configuration failed because of other error
 */
esp_err_t esp_lcd_dpi_panel_set_color_conversion(esp_lcd_panel_handle_t dpi_panel, const esp_lcd_color_conv_config_t *config);

/**
 * @brief Type of LCD DPI panel event data
 */
typedef struct {
} esp_lcd_dpi_panel_event_data_t;

/**
 * @brief A general function callback prototype for DPI panel driver
 *
 * @param[in] panel LCD panel handle, which is created by factory API like esp_lcd_new_panel_dpi()
 * @param[in] edata DPI panel event data, fed by driver
 * @param[in] user_ctx User data
 * @return Whether a high priority task has been waken up by this function
 */
typedef bool (*esp_lcd_dpi_panel_general_cb_t)(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx);

/**
 * @brief Declare the prototype of the function that will be invoked
 *        when driver finishes coping user's color buffer to frame buffer
 */
typedef esp_lcd_dpi_panel_general_cb_t esp_lcd_dpi_panel_color_trans_done_cb_t;

/**
 * @brief Declare the prototype of the function that will be invoked
 *        when driver finishes refreshing the frame buffer to the screen
 */
typedef esp_lcd_dpi_panel_general_cb_t esp_lcd_dpi_panel_refresh_done_cb_t;

/**
 * @brief Type of LCD DPI panel callbacks
 */
typedef struct {
    esp_lcd_dpi_panel_color_trans_done_cb_t on_color_trans_done; /*!< Invoked when user's color buffer copied to the internal frame buffer.
                                                                      This is an indicator that the draw buffer can be recycled safely.
                                                                      But doesn't mean the draw buffer finishes the refreshing to the screen. */
    esp_lcd_dpi_panel_refresh_done_cb_t on_refresh_done;         /*!< Invoked when the internal frame buffer finishes refreshing to the screen */
} esp_lcd_dpi_panel_event_callbacks_t;

/**
 * @brief Register LCD DPI panel callbacks
 *
 * @param[in] dpi_panel LCD DPI panel handle, which is returned from esp_lcd_new_panel_dpi()
 * @param[in] cbs structure with all LCD panel callbacks
 * @param[in] user_ctx User private data, passed directly to callback's user_ctx
 * @return
 *      - ESP_ERR_INVALID_ARG: Register callbacks failed because of invalid argument
 *      - ESP_OK: Register callbacks successfully
 */
esp_err_t esp_lcd_dpi_panel_register_event_callbacks(esp_lcd_panel_handle_t dpi_panel, const esp_lcd_dpi_panel_event_callbacks_t *cbs, void *user_ctx);

#ifdef __cplusplus
}
#endif
