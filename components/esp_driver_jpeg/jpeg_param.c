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
const uint32_t dec_hb_tbl[JPEG_DOWN_SAMPLING_NUM][JPEG_DEC_BEST_HB_MAX] = {
    {40, 40, 40, 32, 0},
    {64, 32, 32, 64, 0},
    {48, 32, 32, 48, 0},
    {96,  0,  0,  0, 96},
};

/**
 * @brief DMA2D best hb value table for JPEG compression.
 *
 * This two-dimensional array represents a Huffman encoding table for JPEG
 * compression. It is used to decode the Huffman-coded symbols in the compressed
 * data stream during the encoding process.
 */
const uint32_t enc_hb_tbl[JPEG_ENC_BEST_HB_MAX][JPEG_DOWN_SAMPLING_NUM] = {
    {40, 32, 32, 0},
    {0,  64, 0,  0},
    {64, 64, 48, 0},
    {0,  0,  0,  128}
};

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.3 (for luminance DC coefficients), the 16 bytes which specify the list of code lengths for the table are
 *        X'00 01 05 01 01 01 01 01 01 00 00 00 00 00 00 00'
 */
const uint8_t luminance_dc_coefficients[JPEG_HUFFMAN_BITS_LEN_TABLE_LEN] = {
    0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0
};

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.3 (for luminance DC coefficients), the set of values following this list is
 *        X'00 01 02 03 04 05 06 07 08 09 0A 0B'
 */
const uint8_t luminance_dc_values[JPEG_HUFFMAN_DC_VALUE_TABLE_LEN] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.5 (for luminance AC coefficients), the 16 bytes which specify the list of code lengths for the table are
 *        X'00 02 01 03 03 02 04 03 05 05 04 04 00 00 01 7D'
 */
const uint8_t luminance_ac_coefficients[JPEG_HUFFMAN_BITS_LEN_TABLE_LEN] = {
    0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d
};

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.5 (for luminance AC values)
 */
const uint8_t luminance_ac_values[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa
};

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.4 (for chrominance DC coefficients), the 16 bytes which specify the list of code lengths for the table are
 *        X'00 03 01 01 01 01 01 01 01 01 01 00 00 00 00 00'
 */
const uint8_t chrominance_dc_coefficients[JPEG_HUFFMAN_BITS_LEN_TABLE_LEN] = {
    0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0
};

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.4 (for luminance DC coefficients), the set of values following this list is
 *        X'00 01 02 03 04 05 06 07 08 09 0A 0B'
 */
const uint8_t chrominance_dc_values[JPEG_HUFFMAN_DC_VALUE_TABLE_LEN] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.5 (for luminance AC coefficients), the 16 bytes which specify the list of code lengths for the table are
 *        X'00 02 01 02 04 04 03 04 07 05 04 04 00 01 02 77'
 */
const uint8_t chrominance_ac_coefficients[JPEG_HUFFMAN_BITS_LEN_TABLE_LEN] = {
    0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77
};

/**
 * @brief Setup the standard Huffman tables (JPEG standard sections K.3.3)
 *        For table K.5 (for chrominance AC values)
 */
const uint8_t chrominance_ac_values[JPEG_HUFFMAN_AC_VALUE_TABLE_LEN] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa
};

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
const uint32_t luminance_quantization_table[JPEG_QUANTIZATION_TABLE_LEN] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

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
const uint32_t chrominance_quantization_table[JPEG_QUANTIZATION_TABLE_LEN] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99
};
