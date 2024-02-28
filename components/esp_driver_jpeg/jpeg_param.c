/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "driver/jpeg_types.h"
#include "jpeg_private.h"

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
const uint8_t zigzag_arr[64] = {
    0,   1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

/**
 * @brief DMA2D best hb value table for JPEG decompression.
 *
 * This two-dimensional array represents a Huffman decoding table for JPEG
 * decompression. It is used to decode the Huffman-coded symbols in the compressed
 * data stream during the decoding process.
 */
const uint32_t dec_hb_tbl[JPEG_DOWN_SAMPLING_MAX][JPEG_DEC_BEST_HB_MAX] = {
    {40, 40, 40, 32, 0},
    {64, 32, 32, 64, 0},
    {48, 32, 32, 48, 0},
    {96,  0,  0,  0, 96},
};
