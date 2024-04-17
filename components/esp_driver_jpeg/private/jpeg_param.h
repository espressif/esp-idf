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
extern const uint32_t dec_hb_tbl[JPEG_DOWN_SAMPLING_NUM][JPEG_DEC_BEST_HB_MAX];

/**
 * @brief DMA2D best hb value table for JPEG compression.
 *
 * This two-dimensional array represents a Huffman encoding table for JPEG
 * compression. It is used to decode the Huffman-coded symbols in the compressed
 * data stream during the encoding process.
 */
extern const uint32_t enc_hb_tbl[JPEG_ENC_BEST_HB_MAX][JPEG_DOWN_SAMPLING_NUM];

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.3 (for luminance DC coefficients), the 16 bytes which specify the list of code lengths for the table are
 *        X'00 01 05 01 01 01 01 01 01 00 00 00 00 00 00 00'
 */
extern const uint8_t luminance_dc_coefficients[JPEG_HUFFMAN_BITS_LEN_TABLE_LEN];

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.3 (for luminance DC coefficients), the set of values following this list is
 *        X'00 01 02 03 04 05 06 07 08 09 0A 0B'
 */
extern const uint8_t luminance_dc_values[JPEG_HUFFMAN_DC_VALUE_TABLE_LEN];

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.5 (for luminance AC coefficients), the 16 bytes which specify the list of code lengths for the table are
 *        X'00 02 01 03 03 02 04 03 05 05 04 04 00 00 01 7D'
 */
extern const uint8_t luminance_ac_coefficients[JPEG_HUFFMAN_BITS_LEN_TABLE_LEN];

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.5 (for luminance AC values)
 */
extern const uint8_t luminance_ac_values[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN];

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.4 (for chrominance DC coefficients), the 16 bytes which specify the list of code lengths for the table are
 *        X'00 03 01 01 01 01 01 01 01 01 01 00 00 00 00 00'
 */
extern const uint8_t chrominance_dc_coefficients[JPEG_HUFFMAN_BITS_LEN_TABLE_LEN];

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.4 (for luminance DC coefficients), the set of values following this list is
 *        X'00 01 02 03 04 05 06 07 08 09 0A 0B'
 */
extern const uint8_t chrominance_dc_values[JPEG_HUFFMAN_DC_VALUE_TABLE_LEN];

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.5 (for luminance AC coefficients), the 16 bytes which specify the list of code lengths for the table are
 *        X'00 02 01 02 04 04 03 04 07 05 04 04 00 01 02 77'
 */
extern const uint8_t chrominance_ac_coefficients[JPEG_HUFFMAN_BITS_LEN_TABLE_LEN];

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.5 (for chrominance AC values)
 */
extern const uint8_t chrominance_ac_values[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN];

/**
* @brief This array stores the luminance quantization values for each block in an image.(JPEG standard sections K.1)
* The luminance_quantization_table array is of type uint32_t and has a size of 64 elements.
*
* These values are part of the quantization table used in the JPEG image compression standard.
* They are employed during the process of quantizing the discrete cosine transform (DCT) coefficients
* of an image's luminance component, allowing for lossy compression.
*
* The quantization process involves dividing the DCT coefficients by these values to reduce the precision of the data,
* which results in higher compression ratios but also introduces some loss of image quality.
*
* Each value in the array corresponds to a specific position in the 8x8 luminance quantization matrix used in JPEG compression.
*
* These specific values are critical for achieving a balance between compression efficiency and visual quality
* in JPEG image compression, and they have become a fundamental component of the standard.
*/
extern const uint32_t luminance_quantization_table[JPEG_QUANTIZATION_TABLE_LEN];

/**
* @brief This array stores the chrominance quantization values for each block in an image.(JPEG standard sections K.2)
* The chrominance_quantization_table array is of type uint32_t and has a size of 64 elements.
*
* These values are part of the quantization table used in the JPEG image compression standard.
* They are employed during the process of quantizing the discrete cosine transform (DCT) coefficients
* of an image's chrominance component, allowing for lossy compression.
*
* The quantization process involves dividing the DCT coefficients by these values to reduce the precision of the data,
* which results in higher compression ratios but also introduces some loss of image quality.
*
* Each value in the array corresponds to a specific position in the 8x8 chrominance quantization matrix used in JPEG compression.
*
* These specific values are critical for achieving a balance between compression efficiency and visual quality
* in JPEG image compression, and they have become a fundamental component of the standard.
*/
extern const uint32_t chrominance_quantization_table[JPEG_QUANTIZATION_TABLE_LEN];

#ifdef __cplusplus
}
#endif
