/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "hal/cam_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lcd_cam_dev_t cam_dev_t; // CAM SOC layer handle

/**
 * @brief CAM hardware interface object data
 */
typedef struct cam_hal_context {
    cam_dev_t *hw;                          /*!< Beginning address of the CAM peripheral registers. */
} cam_hal_context_t;

/**
 * @brief CAM HAL driver configuration
 */
typedef struct cam_hal_config {
    int port;                               /*!< CAM port */
    bool byte_swap_en;                      /*!< CAM enable byte swap */
} cam_hal_config_t;

/**
 * @brief Initialize CAM hardware
 *
 * @param hal    CAM object data pointer
 * @param config CAM configuration
 *
 * @return None
 */
void cam_hal_init(cam_hal_context_t *hal, const cam_hal_config_t *config);

/**
 * @brief De-initialize CAM hardware
 *
 * @param hal CAM object data pointer
 *
 * @return None
 */
void cam_hal_deinit(cam_hal_context_t *hal);

/**
 * @brief Start CAM to receive frame data
 *
 * @param hal CAM object data pointer
 *
 * @return None
 */
void cam_hal_start_streaming(cam_hal_context_t *hal);

/**
 * @brief Stop CAM receiving frame data
 *
 * @param hal CAM object data pointer
 *
 * @return None
 */
void cam_hal_stop_streaming(cam_hal_context_t *hal);

/**
 * @brief Configure color format conversion
 *
 * This function handles all types of color format conversions:
 * - YUV to RGB conversion
 * - RGB to YUV conversion
 * - YUV to YUV conversion
 *
 * Color range support:
 * - Full range: 0-255 for both RGB and YUV
 * - Limited range: RGB 16-240, YUV Y:16-240, U-V:16-235
 *
 * @param hal    CAM HAL context pointer
 * @param config Color conversion configuration. If NULL, default config is used.
 */
void cam_hal_color_format_convert(cam_hal_context_t *hal,
                                 const cam_ctlr_format_conv_config_t *config);

#ifdef __cplusplus
}
#endif
