/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "hal/mipi_csi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MIPI CSI SOC layer representation of the host controller
 */
typedef struct csi_host_dev_t *mipi_csi_host_soc_handle_t;

/**
 * @brief MIPI CSI SOC layer representation of the bridge controller
 */
typedef struct csi_brg_dev_t *mipi_csi_bridge_soc_handle_t;

/**
 * @brief MIPI CSI HAL driver context
 */
typedef struct {
    mipi_csi_host_soc_handle_t host_dev;
    mipi_csi_bridge_soc_handle_t bridge_dev;
} mipi_csi_hal_context_t;

/**
 * @brief MIPI CSI HAL driver configuration
 */
typedef struct {
    uint8_t lanes_num;                          ///< Lane num
    uint32_t frame_width;                       ///< Frame width
    uint32_t frame_height;                      ///< Frame height
    uint32_t in_bpp;                            ///< In bits per pixel
    uint32_t out_bpp;                           ///< Out bits per pixel
    bool byte_swap_en;                          ///< Enable byte swap
    int lane_bit_rate_mbps;                     ///< Lane bit rate in Mbps
} mipi_csi_hal_config_t;

/**
 * @brief MIPI CSI HAL layer initialization
 *
 * @param hal         Pointer to the HAL driver context
 * @param config      Pointer to the HAL configuration
 */
void mipi_csi_hal_init(mipi_csi_hal_context_t *hal, const mipi_csi_hal_config_t *config);

#ifdef __cplusplus
}
#endif
