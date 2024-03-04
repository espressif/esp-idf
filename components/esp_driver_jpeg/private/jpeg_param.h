/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "driver/jpeg_types.h"
#include "../jpeg_private.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Zigzag scan pattern array.
 *
 * This array represents a zigzag scan pattern for reordering the coefficients
 * in a block of data. It is commonly used in JPEG compression and decompression
 * algorithms to optimize encoding and decoding processes.
 *
 * The array consists of 64 elements, where each element represents the position
 * of a coefficient in the zigzag pattern. The coefficients are traversed in the
 * order specified by this array to achieve efficient compression and decompression.
 *
 * @note The values in this array are zero-indexed.
 */
extern const uint8_t zigzag_arr[64];

/**
 * @brief DMA2D best hb value table for JPEG decompression.
 *
 * This two-dimensional array represents a Huffman decoding table for JPEG
 * decompression. It is used to decode the Huffman-coded symbols in the compressed
 * data stream during the decoding process.
 */
extern const uint32_t dec_hb_tbl[JPEG_DOWN_SAMPLING_MAX][JPEG_DEC_BEST_HB_MAX];

#ifdef __cplusplus
}
#endif
