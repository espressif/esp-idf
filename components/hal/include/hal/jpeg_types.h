/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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

#define JEPG_HUFFMAN_BITS_LEN_TABLE_LEN        (16)
#define JPEG_HUFFMAN_AC_VALUE_TABLE_LEN        (256)
#define JPEG_HUFFMAN_DC_VALUE_TABLE_LEN        (16)
#define JPEG_QUANTIZATION_TABLE_LEN            (64)

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
 * @brief Enum for JEPG sampling mode.
 */
typedef enum {
    JPEG_SAMPLE_MODE_YUV444 = COLOR_PIXEL_YUV444,  ///< sample in YUV444
    JPEG_SAMPLE_MODE_YUV422 = COLOR_PIXEL_YUV422,  ///< sample in YUV422
    JPEG_SAMPLE_MODE_YUV420 = COLOR_PIXEL_YUV420,  ///< sample in YUV420
} jpeg_sample_mode_t;


#ifdef __cplusplus
}
#endif
