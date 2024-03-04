/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration for jpeg output format.
 */
typedef enum {
    JPEG_DECODE_OUT_FORMAT_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888),        /*!< output RGB888 format */
    JPEG_DECODE_OUT_FORMAT_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565),        /*!< output RGB565 format */
    JPEG_DECODE_OUT_FORMAT_GRAY = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8),          /*!< output the gray picture */
} jpeg_dec_output_format_t;

/**
 * @brief Enumeration for YUV and RGB conversion standard
 */
typedef enum {
    JPEG_YUV_RGB_CONV_STD_BT601 = COLOR_CONV_STD_RGB_YUV_BT601,     /*!< BT601 */
    JPEG_YUV_RGB_CONV_STD_BT709 = COLOR_CONV_STD_RGB_YUV_BT709,     /*!< BT709 */
} jpeg_yuv_rgb_conv_std_t;

/**
 * @brief Enumeration for jpeg big/small endian output.
 */
typedef enum {
    JPEG_DEC_RGB_ELEMENT_ORDER_BGR = COLOR_RGB_ELEMENT_ORDER_BGR,         /*!< Output the color component in small endian */
    JPEG_DEC_RGB_ELEMENT_ORDER_RGB = COLOR_RGB_ELEMENT_ORDER_RGB,         /*!< Output the color component in big endian */
} jpeg_dec_rgb_element_order;

/**
 * @brief Type of jpeg decoder handle
 */
typedef struct jpeg_decoder_t *jpeg_decoder_handle_t;

#ifdef __cplusplus
}
#endif
