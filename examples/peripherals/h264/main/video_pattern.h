/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file video_pattern.h
 * @brief  Video pattern generation and processing utilities for H264 encoding/decoding examples
 *
 *         This header provides structures and functions for generating test video patterns
 *         and displaying conversion results for H264 codec testing.
 */
#pragma once

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_h264_types.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief  Video resolution definition
 */
typedef struct {
    uint16_t width;   /*!< Width of the video in pixels */
    uint16_t height;  /*!< Height of the video in pixels */
} video_resolution_t;

/**
 * @brief  Pattern information structure for video pattern generation
 */
typedef struct {
    esp_h264_raw_format_t format_id;  /*!< Video format identifier */
    video_resolution_t    res;        /*!< Video resolution */
    uint8_t              *pixel;      /*!< Pixel data buffer */
    uint32_t              data_size;  /*!< Size of pixel data in bytes */
    bool                  vertical;   /*!< Vertical orientation flag */
    uint8_t               bar_count;  /*!< Number of color bars */
} pattern_info_t;

/**
 * @brief  Generate color bar pattern for video testing
 * @param  info  Pointer to pattern information structure
 * @return
 *       - ESP_OK    on success
 *       - ESP_FAIL  on error
 */
esp_err_t gen_pattern_color_bar(pattern_info_t *info);

/**
 * @brief  Draw and display conversion result comparison
 * @param  a  Pointer to source pattern information
 * @param  b  Pointer to destination pattern information
 */
void draw_convert_result(pattern_info_t *a, pattern_info_t *b);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
