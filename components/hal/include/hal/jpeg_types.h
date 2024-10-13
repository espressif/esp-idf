/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "hal/color_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JPEG_HUFFMAN_BITS_LEN_TABLE_LEN        (16)
#define JPEG_HUFFMAN_AC_VALUE_TABLE_LEN        (256)
#define JPEG_HUFFMAN_DC_VALUE_TABLE_LEN        (16)
#define JPEG_QUANTIZATION_TABLE_LEN            (64)
#define JPEG_COMPONENT_NUMBER_MAX              (4)

#define DHT_TC_NUM (2)  /// Table type
#define DHT_TH_NUM (2)  /// Huffman table destination identifier
#define JPEG_DOWN_SAMPLING_NUM (4) // The number of down sampling methods

/**
 * @brief Enum for JPEG codec working mode.
 */
typedef enum {
    JPEG_CODEC_ENCODER,    ///< Encode mode
    JPEG_CODEC_DECODER,    ///< Decode mode
} jpeg_codec_mode_t;

/**
 * @brief Structure for recording factor of component.
 */
typedef struct {
    uint32_t horizontal;   ///< horizontal factor
    uint32_t vertical;     ///< vertical factor
} jpeg_component_factor_t;

/**
 * @brief Enum for JPEG sampling mode.
 */
typedef enum {
    JPEG_SAMPLE_MODE_YUV444 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV444),  ///< sample in YUV444
    JPEG_SAMPLE_MODE_YUV422 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422),  ///< sample in YUV422
    JPEG_SAMPLE_MODE_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420),  ///< sample in YUV420
} jpeg_sample_mode_t;

/**
 * @brief Structure for huffman information
 */
typedef union {
    struct {
        uint8_t id   : 4;   ///< Huffman table id
        uint8_t type : 4;   ///< Huffman table type
    };
    uint8_t info;           ///< Information of huffman table
} jpeg_huffman_table_info_t;

/**
 * @brief Enumeration for jpeg decoder sample methods.
*/
typedef enum {
    JPEG_DOWN_SAMPLING_YUV444 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV444),        /*!< Sample by YUV444 */
    JPEG_DOWN_SAMPLING_YUV422 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422),        /*!< Sample by YUV422 */
    JPEG_DOWN_SAMPLING_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420),        /*!< Sample by YUV420 */
    JPEG_DOWN_SAMPLING_GRAY   = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8),        /*!< Sample the gray picture */
} jpeg_down_sampling_type_t;

/**
 * @brief JPEG encoder source formats.
 */
typedef enum {
    JPEG_ENC_SRC_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888),   /*!< JPEG encoder source RGB888 */
    JPEG_ENC_SRC_YUV422 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422),   /*!< JPEG encoder source YUV422 */
    JPEG_ENC_SRC_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565),   /*!< JPEG encoder source RGB565 */
    JPEG_ENC_SRC_GRAY = COLOR_TYPE_ID(COLOR_SPACE_GRAY, COLOR_PIXEL_GRAY8),     /*!< JPEG encoder source GRAY */
} jpeg_enc_src_type_t;

#ifdef __cplusplus
}
#endif
