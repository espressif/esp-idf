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
    JPEG_DECODE_OUT_FORMAT_YUV444 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV444),        /*!< output yuv444 format */
    JPEG_DECODE_OUT_FORMAT_YUV422 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422),        /*!< output yuv422 format */
    JPEG_DECODE_OUT_FORMAT_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420),        /*!< output yuv420 format */
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
} jpeg_dec_rgb_element_order_t;

/**
 * @brief Enumeration for jpeg decoder alloc buffer direction.
 */
typedef enum {
    JPEG_DEC_ALLOC_INPUT_BUFFER = 0,                   /*!< Alloc the picture input buffer, (compressed format in decoder) */
    JPEG_DEC_ALLOC_OUTPUT_BUFFER = 1,                  /*!< Alloc the picture output buffer, (decompressed format in decoder) */
} jpeg_dec_buffer_alloc_direction_t;

/**
 * @brief Enumeration for jpeg input format.
 */
typedef enum {
    JPEG_ENCODE_IN_FORMAT_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888),       /*!< input RGB888 format */
    JPEG_ENCODE_IN_FORMAT_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565),       /*!< input RGB565 format */
    JPEG_ENCODE_IN_FORMAT_GRAY = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8),         /*!< input GRAY format */
    JPEG_ENCODE_IN_FORMAT_YUV422 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422),       /*!< input YUV422 format */
} jpeg_enc_input_format_t;

/**
 * @brief Enumeration for jpeg encoder alloc buffer direction.
 */
typedef enum {
    JPEG_ENC_ALLOC_INPUT_BUFFER = 0,                   /*!< Alloc the picture input buffer, (decompressed format in encoder) */
    JPEG_ENC_ALLOC_OUTPUT_BUFFER = 1,                  /*!< Alloc the picture output buffer, (compressed format in encoder) */
} jpeg_enc_buffer_alloc_direction_t;

/**
 * @brief Type of jpeg decoder handle
 */
typedef struct jpeg_decoder_t *jpeg_decoder_handle_t;

/**
 * @brief Type of jpeg encoder handle
 */
typedef struct jpeg_encoder_t *jpeg_encoder_handle_t;

#ifdef __cplusplus
}
#endif
