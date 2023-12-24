/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/lcd_types.h"
#include "hal/mipi_dsi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MIPI DSI SOC layer representation of the host controller
 */
typedef struct dsi_host_dev_t *mipi_dsi_host_soc_handle_t;

/**
 * @brief MIPI DSI SOC layer representation of the bridge controller
 */
typedef struct dsi_brg_dev_t *mipi_dsi_bridge_soc_handle_t;

/**
 * @brief MIPI DSI HAL driver context
 */
typedef struct {
    mipi_dsi_host_soc_handle_t host;     /*!< Pointer to the host controller registers */
    mipi_dsi_bridge_soc_handle_t bridge; /*!< Pointer to the bridge controller registers */
    float dpi2lane_clk_ratio;            /*!< Ratio between DPI clock and lane byte clock */
} mipi_dsi_hal_context_t;

/**
 * @brief MIPI DSI HAL driver configuration
 */
typedef struct {
    uint32_t lane_byte_clk_hz; /*!< Lane byte clock frequency */
    uint32_t dpi_clk_hz;       /*!< DPI clock frequency */
} mipi_dsi_hal_config_t;

/**
 * @brief Initialize MIPI DSI Hal driver context
 *
 * @note Caller should malloc the memory for the hal context
 *
 * @param hal Pointer to the HAL driver context
 * @param config Pointer to the HAL driver configuration
 */
void mipi_dsi_hal_init(mipi_dsi_hal_context_t *hal, const mipi_dsi_hal_config_t *config);

/**
 * @brief Write a value to a PHY register via internal bus (so-called test interface)
 *
 * @param hal Pointer to the HAL driver context
 * @param reg_addr Address of the PHY register
 * @param reg_val Value to be written to the PHY register
 */
void mipi_dsi_hal_phy_write_register(mipi_dsi_hal_context_t *hal, uint8_t reg_addr, uint8_t reg_val);

/**
 * @brief Send a DCS command with parameters via the generic interface
 *
 * @note The command itself and the parameters are sent in one packet. For simplicity, we use the same
 *       DCS_LONG_WRITE type to send all different commands.
 *
 * @param hal Pointer to the HAL driver context
 * @param vc Virtual channel number
 * @param command DCS command
 * @param command_bytes Number of bytes of the command
 * @param param Pointer to the parameters
 * @param param_size Number of bytes of the parameters
 */
void mipi_dsi_hal_host_gen_write_dcs_command(mipi_dsi_hal_context_t *hal, uint8_t vc,
        uint32_t command, uint32_t command_bytes, const void *param, uint16_t param_size);

/**
 * @brief Send a DCS command and return the associated parameters via the generic interface
 *
 * @param hal Pointer to the HAL driver context
 * @param vc Virtual channel number
 * @param command DCS command
 * @param command_bytes Number of bytes of the command
 * @param ret_param Pointer to the buffer to store the returned parameters
 * @param param_buf_size Size of the buffer to store the returned parameters
 */
void mipi_dsi_hal_host_gen_read_dcs_command(mipi_dsi_hal_context_t *hal, uint8_t vc, uint32_t command, uint32_t command_bytes, void *ret_param, uint16_t param_buf_size);

/**
 * @brief Send a long packet via the generic interface
 *
 * @param hal Pointer to the HAL driver context
 * @param vc Virtual channel number
 * @param dt Data type
 * @param buffer Pointer to the buffer
 * @param buffer_size Number of bytes to be sent
 */
void mipi_dsi_hal_host_gen_write_long_packet(mipi_dsi_hal_context_t *hal, uint8_t vc, mipi_dsi_data_type_t dt, const void *buffer, uint16_t buffer_size);

/**
 * @brief Send a short packet via the generic interface
 *
 * @param hal Pointer to the HAL driver context
 * @param vc Virtual channel number
 * @param dt Data type
 * @param header_data Data to be sent, filled into the DSI packet header
 */
void mipi_dsi_hal_host_gen_write_short_packet(mipi_dsi_hal_context_t *hal, uint8_t vc, mipi_dsi_data_type_t dt, uint16_t header_data);

/**
 * @brief Send a short packet via the generic interface and return the associated data
 *
 * @param hal Pointer to the HAL driver context
 * @param vc Virtual channel number
 * @param dt Data type
 * @param header_data Data to be filled into the DSI packet header
 * @param ret_buffer Pointer to the buffer to store the returned data
 * @param buffer_size Size of the buffer to store the returned data
 */
void mipi_dsi_hal_host_gen_read_short_packet(mipi_dsi_hal_context_t *hal, uint8_t vc, mipi_dsi_data_type_t dt, uint16_t header_data, void *ret_buffer, uint16_t buffer_size);

/**
 * @brief Set DPI color coding
 *
 * @param hal Pointer to the HAL driver context
 * @param color_coding Color coding
 * @param sub_config Sub configuration
 */
void mipi_dsi_hal_host_dpi_set_color_coding(mipi_dsi_hal_context_t *hal, lcd_color_rgb_pixel_format_t color_coding, uint32_t sub_config);

/**
 * @brief Set horizontal timing parameters for DPI
 *
 * @param hal Pointer to the HAL driver context
 * @param hsw Horizontal Synchronization Width
 * @param hbp Horizontal Back Porch
 * @param active_width Active Width
 * @param hfp Horizontal Front Porch
 */
void mipi_dsi_hal_host_dpi_set_horizontal_timing(mipi_dsi_hal_context_t *hal, uint32_t hsw, uint32_t hbp, uint32_t active_width, uint32_t hfp);

/**
 * @brief Set vertical timing parameters for DPI
 *
 * @param hal Pointer to the HAL driver context
 * @param vsw Vertical Synchronization Width
 * @param vbp Vertical Back Porch
 * @param active_height Active Height
 * @param vfp Vertical Front Porch
 */
void mipi_dsi_hal_host_dpi_set_vertical_timing(mipi_dsi_hal_context_t *hal, uint32_t vsw, uint32_t vbp, uint32_t active_height, uint32_t vfp);

#ifdef __cplusplus

}
#endif
