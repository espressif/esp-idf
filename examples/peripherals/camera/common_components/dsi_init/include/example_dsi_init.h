/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DSI allocation configuration
 */
typedef struct {
    uint8_t num_fbs;  /*!< Number of frame buffers (1-2). */
} example_dsi_alloc_config_t;

/**
 * @brief Default DSI allocation configuration
 */
#define EXAMPLE_DSI_ALLOC_CONFIG_DEFAULT() { \
    .num_fbs = 1, \
}

/**
 * @brief DSI init function with configurable frame buffers
 *
 * @param[in]  config           DSI allocation configuration. If NULL, uses default (single buffer)
 * @param[out] mipi_dsi_bus     MIPI DSI bus handle
 * @param[out] mipi_dbi_io      MIPI DBI io handle
 * @param[out] mipi_dpi_panel   MIPI DPI panel handle
 * @param[out] fb0              Pointer to receive the address of frame buffer 0 (first frame buffer)
 * @param[out] fb1              Pointer to receive the address of frame buffer 1 (second frame buffer). If NULL, single buffer mode is used.
 *
 * @note The number of frame buffers allocated is determined by config->num_fbs. If config is NULL, single buffer mode is used.
 */
void example_dsi_resource_alloc(const example_dsi_alloc_config_t *config,
                                esp_lcd_dsi_bus_handle_t *mipi_dsi_bus,
                                esp_lcd_panel_io_handle_t *mipi_dbi_io,
                                esp_lcd_panel_handle_t *mipi_dpi_panel,
                                void **fb0,
                                void **fb1);

/**
 * @brief DPI panel reset function
 *
 * @param[in] mipi_dpi_panel MIPI DPI panel handle
 */
void example_dpi_panel_reset(esp_lcd_panel_handle_t mipi_dpi_panel);

/**
 * @brief DPI panel init function
 *
 * @param[in] mipi_dpi_panel MIPI DPI panel handle
 */
void example_dpi_panel_init(esp_lcd_panel_handle_t mipi_dpi_panel);

/**
 * @brief Destroy DSI related resources
 *
 * @param[in] mipi_dsi_bus         MIPI DSI bus handle
 * @param[in] mipi_dbi_io          MIPI DBI io handle
 * @param[in] mipi_dpi_panel       MIPI DPI panel handle
 */
void example_dsi_resource_destroy(esp_lcd_dsi_bus_handle_t mipi_dsi_bus, esp_lcd_panel_io_handle_t mipi_dbi_io, esp_lcd_panel_handle_t mipi_dpi_panel);

#ifdef __cplusplus
}
#endif
