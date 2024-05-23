/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_assert.h"
#include "hal/assert.h"
#include "hal/color_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of 2D-DMA descriptor
 */
typedef struct dma2d_descriptor_align8_s dma2d_descriptor_align8_t;
struct dma2d_descriptor_align8_s {
    struct {
        uint32_t vb_size : 14;        /*!< Vertical height of the block, unit: pixel
                                           When dma2d_en field is 0, this field is the lower 14-bit of the buffer memory space size, unit: byte */
        uint32_t hb_length : 14;      /*!< Horizontal width of the block, unit: pixel
                                           When dma2d_en field is 0, this field is the lower 14-bit of the buffer length, unit: byte
                                           For data receive, this field is filled by DMA after transfer finishes */
        uint32_t err_eof : 1;         /*!< Whether the received buffer contains error
                                           For data transfer, this bit is fixed to 0 */
        uint32_t dma2d_en : 1;        /*!< Whether to enable 2D functionality */
        uint32_t suc_eof : 1;         /*!< Whether the descriptor is the last one in the link */
        uint32_t owner : 1;           /*!< Who is allowed to access the buffer that this descriptor points to, select DMA2D_DESCRIPTOR_BUFFER_OWNER_CPU or DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA
                                           When owner is chosen to be DMA, after DMA finishes with the descriptor, it will clear this bit
                                           For data transfer, the bit won't be cleared unless DMA2D_OUT_AUTO_WRBACK is enabled */
    };                                /*!< Descriptor Word 0 */
    struct {
        uint32_t va_size : 14;        /*!< Vertical height of the picture, unit: pixel
                                           When dma2d_en field is 0, this field is the higher 14-bit of the buffer memory space size, unit: byte */
        uint32_t ha_length : 14;      /*!< Horizontal width of the picture, unit: pixel
                                           When dma2d_en field is 0, this field is the higher 14-bit of the buffer length, unit: byte
                                           For data receive, this field is filled by DMA after transfer finishes */
        uint32_t pbyte : 4;           /*!< Number of bytes per pixel (make use of dma2d_desc_pixel_format_to_pbyte_value to get the value)
                                           When dma2d_en field is 0, this field has no use */
    };                                /*!< Descriptor Word 1 */
    struct {
        uint32_t y : 14;              /*!< The y-coordinate value of the hb * vb block, unit: pixel
                                           When dma2d_en field is 0, this field has no use */
        uint32_t x : 14;              /*!< The x-coordinate value of the hb * vb block, unit: pixel
                                           When dma2d_en field is 0, this field has no use */
        uint32_t mode : 1;            /*!< Data block read/write mode, select DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE or DMA2D_DESCRIPTOR_BLOCK_RW_MODE_MULTIPLE
                                           When dma2d_en field is 0, this field must be 0 (i.e. DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE mode) */
        uint32_t reserved29 : 3;
    };                                /*!< Descriptor Word 2 */
    void *buffer;                     /*!< Pointer to the buffer (of ha * va picture)
                                           For RX buffer, there is 4-byte alignment addr and size requirement (spiram and axi bus restriction, otherwises, could overwrite other data or be overwritten) */
    dma2d_descriptor_align8_t *next;  /*!< Pointer to the next descriptor (set to NULL if the descriptor is the last one, e.g. suc_eof=1) */
} __attribute__((aligned(8)));

ESP_STATIC_ASSERT(sizeof(dma2d_descriptor_align8_t) == 24, "dma2d_descriptor_align8_t should occupy 24 bytes in memory");

// 2D-DMA descriptor requires 8-byte alignment
typedef dma2d_descriptor_align8_t dma2d_descriptor_t;

#define DMA2D_DESCRIPTOR_BUFFER_OWNER_CPU (0)                     /*!< 2D-DMA buffer is allowed to be accessed by CPU */
#define DMA2D_DESCRIPTOR_BUFFER_OWNER_DMA (1)                     /*!< 2D-DMA buffer is allowed to be accessed by 2D-DMA engine */

#define DMA2D_DESCRIPTOR_BLOCK_RW_MODE_SINGLE (0)                 /*!< 2D-DMA data block single R/W mode, only r/w one hb * vb block */
#define DMA2D_DESCRIPTOR_BLOCK_RW_MODE_MULTIPLE (1)               /*!< 2D-DMA data block multiple R/W mode, continues r/w hb * vb blocks until the ha * va picture is done */

#define DMA2D_DESCRIPTOR_PBYTE_0B5_PER_PIXEL (0)                  /*!< 2D-DMA descriptor pbyte value when 0.5 bytes/pixel */
#define DMA2D_DESCRIPTOR_PBYTE_1B0_PER_PIXEL (1)                  /*!< 2D-DMA descriptor pbyte value when 1 bytes/pixel */
#define DMA2D_DESCRIPTOR_PBYTE_1B5_PER_PIXEL (2)                  /*!< 2D-DMA descriptor pbyte value when 1.5 bytes/pixel */
#define DMA2D_DESCRIPTOR_PBYTE_2B0_PER_PIXEL (3)                  /*!< 2D-DMA descriptor pbyte value when 2 bytes/pixel */
#define DMA2D_DESCRIPTOR_PBYTE_3B0_PER_PIXEL (4)                  /*!< 2D-DMA descriptor pbyte value when 3 bytes/pixel */
#define DMA2D_DESCRIPTOR_PBYTE_4B0_PER_PIXEL (5)                  /*!< 2D-DMA descriptor pbyte value when 4 bytes/pixel */

// Helper function to convert pixel format to 2D-DMA descriptor pbyte value
static inline uint32_t dma2d_desc_pixel_format_to_pbyte_value(color_space_pixel_format_t pixel_format)
{
    switch (color_hal_pixel_format_get_bit_depth(pixel_format)) {
    case 4:
        return DMA2D_DESCRIPTOR_PBYTE_0B5_PER_PIXEL;
    case 8:
        return DMA2D_DESCRIPTOR_PBYTE_1B0_PER_PIXEL;
    case 12:
        return DMA2D_DESCRIPTOR_PBYTE_1B5_PER_PIXEL;
    case 16:
        return DMA2D_DESCRIPTOR_PBYTE_2B0_PER_PIXEL;
    case 24:
        return DMA2D_DESCRIPTOR_PBYTE_3B0_PER_PIXEL;
    case 32:
        return DMA2D_DESCRIPTOR_PBYTE_4B0_PER_PIXEL;
    default:
        // Unsupported bit depth
        abort();
    }
}

/**
 * @brief Enumeration of peripherals which have the 2D-DMA capability
 */
typedef enum {
    DMA2D_TRIG_PERIPH_M2M,              /*!< 2D-DMA trigger peripheral: M2M */
    DMA2D_TRIG_PERIPH_JPEG_ENCODER,     /*!< 2D-DMA trigger peripheral: JPEG Encoder */
    DMA2D_TRIG_PERIPH_JPEG_DECODER,     /*!< 2D-DMA trigger peripheral: JPEG Decoder */
    DMA2D_TRIG_PERIPH_PPA_SRM,          /*!< 2D-DMA trigger peripheral: PPA SRM engine */
    DMA2D_TRIG_PERIPH_PPA_BLEND,        /*!< 2D-DMA trigger peripheral: PPA Blending engine */
} dma2d_trigger_peripheral_t;

/**
 * @brief Enumeration of 2D-DMA channel direction
 */
typedef enum {
    DMA2D_CHANNEL_DIRECTION_TX,         /*!< 2D-DMA channel direction: TX */
    DMA2D_CHANNEL_DIRECTION_RX,         /*!< 2D-DMA channel direction: RX */
} dma2d_channel_direction_t;

/**
 * @brief Enumeration of 2D-DMA data burst length options
 *
 * Starting from 1, saving 0 for special purpose (upper layer could use 0 to be a default burst length)
 */
typedef enum {
    DMA2D_DATA_BURST_LENGTH_8 = 1,      /*!< 2D-DMA block size: 8 bytes */
    DMA2D_DATA_BURST_LENGTH_16,         /*!< 2D-DMA block size: 16 bytes */
    DMA2D_DATA_BURST_LENGTH_32,         /*!< 2D-DMA block size: 32 bytes */
    DMA2D_DATA_BURST_LENGTH_64,         /*!< 2D-DMA block size: 64 bytes */
    DMA2D_DATA_BURST_LENGTH_128,        /*!< 2D-DMA block size: 128 bytes */
    DMA2D_DATA_BURST_LENGTH_INVALID,    /*!< Invalid 2D-DMA block size */
} dma2d_data_burst_length_t;

/**
 * @brief Enumeration of 2D-DMA macro block size options
 * Only useful in DMA2D_DESCRIPTOR_BLOCK_RW_MODE_MULTIPLE mode (dma2d_en = 1, mode = 1)
 * Descriptor vb and hb fields has to be multiples of macro block sizes
 */
typedef enum {
    DMA2D_MACRO_BLOCK_SIZE_NONE,        /*!< 2D-DMA no macro block */
    DMA2D_MACRO_BLOCK_SIZE_8_8,         /*!< 2D-DMA 8 pixel x 8 pixel macro block */
    DMA2D_MACRO_BLOCK_SIZE_8_16,        /*!< 2D-DMA 8 pixel x 16 pixel macro block */
    DMA2D_MACRO_BLOCK_SIZE_16_16,       /*!< 2D-DMA 16 pixel x 16 pixel macro block */
    DMA2D_MACRO_BLOCK_SIZE_INVALID,     /*!< Invalid 2D-DMA macro block size */
} dma2d_macro_block_size_t;

/**
 * @brief Enumeration of 2D-DMA pixel bytes scamble order in color space conversion
 *
 * Assuming the original pixel byte order (from MSB to LSB) is 2-1-0 .
 */
typedef enum {
    DMA2D_SCRAMBLE_ORDER_BYTE2_1_0,     /*!< 2D-DMA pixel data scrambled as BYTE2-1-0 (no scramble) */
    DMA2D_SCRAMBLE_ORDER_BYTE2_0_1,     /*!< 2D-DMA pixel data scrambled as BYTE2-0-1 */
    DMA2D_SCRAMBLE_ORDER_BYTE1_0_2,     /*!< 2D-DMA pixel data scrambled as BYTE1-0-2 */
    DMA2D_SCRAMBLE_ORDER_BYTE1_2_0,     /*!< 2D-DMA pixel data scrambled as BYTE1-2-0 */
    DMA2D_SCRAMBLE_ORDER_BYTE0_2_1,     /*!< 2D-DMA pixel data scrambled as BYTE0-2-1 */
    DMA2D_SCRAMBLE_ORDER_BYTE0_1_2,     /*!< 2D-DMA pixel data scrambled as BYTE0-1-2 */
    DMA2D_SCRAMBLE_ORDER_INVALID,       /*!< Invalid 2D-DMA pixel data scramble order */
} dma2d_scramble_order_t;

//*********************BT601***********************************//
//  Y = 16  + 0.257 * R + 0.504 * g + 0.098 * b                //
// Cb = 128 - 0.148 * R - 0.291 * g + 0.439 * b                //
// Cr = 128 + 0.439 * R - 0.368 * g - 0.071 * b                //
//  R = 1.164 *(Y - 16) + 1.596 *(Cr - 128)                    //
//  G = 1.164 *(Y - 16) - 0.392 *(Cb - 128) - 0.812 *(Cr - 128)//
//  B = 1.164 *(Y - 16) + 2.016 *(Cb - 128)                    //
//*********************BT601***********************************//

//*********************BT709***********************************//
//  Y = 16  + 0.183 * R + 0.614 * g + 0.062 * b                //
// Cb = 128 - 0.101 * R - 0.339 * g + 0.439 * b                //
// Cr = 128 + 0.439 * R - 0.399 * g - 0.040 * b                //
//  R = 1.164 *(Y - 16) + 1.792 *(Cr - 128)                    //
//  G = 1.164 *(Y - 16) - 0.213 *(Cb - 128) - 0.534 *(Cr - 128)//
//  B = 1.164 *(Y - 16) + 2.114 *(Cb - 128)                    //
//*********************BT709***********************************//

//  R/G/B [0 ... 255]
//  Y     [16 ... 235]
//  Cb/Cr [16 ... 240]

// 256 * Q = A[9:0] * x + B[10:0] * y + C[9:0] * z + D[17:0]

#define DMA2D_COLOR_SPACE_CONV_PARAM_RGB2YUV_BT601 \
{                                                  \
    {  66,   129,   25,     4096},                 \
    { -38,   -74,  112,    32768},                 \
    { 112,   -94,  -18,    32768},                 \
}

#define DMA2D_COLOR_SPACE_CONV_PARAM_RGB2YUV_BT709 \
{                                                  \
    {  47,   157,   16,     4096},                 \
    { -26,   -86,  112,    32768},                 \
    { 112,  -102,  -10,    32768},                 \
}

#define DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT601 \
{                                                  \
    { 298,     0,   409,  -56906},                 \
    { 298,  -100,  -208,   34707},                 \
    { 298,   516,     0,  -70836},                 \
}

#define DMA2D_COLOR_SPACE_CONV_PARAM_YUV2RGB_BT709 \
{                                                  \
    { 298,     0,   459,  -63367},                 \
    { 298,   -55,  -136,   19681},                 \
    { 298,   541,     0,  -73918},                 \
}

/**
 * @brief Enumeration of 2D-DMA TX color space conversion (CSC) option
 */
typedef enum {
    DMA2D_CSC_TX_NONE,                          /*!< 2D-DMA TX perform no CSC */
    DMA2D_CSC_TX_SCRAMBLE,                      /*!< 2D-DMA TX perform only data scramble */
    DMA2D_CSC_TX_RGB888_TO_RGB565,              /*!< 2D-DMA TX perform RGB888 to RGB565 conversion */
    DMA2D_CSC_TX_RGB565_TO_RGB888,              /*!< 2D-DMA TX perform RGB565 to RGB888 conversion */
    DMA2D_CSC_TX_RGB888_TO_YUV444_601,          /*!< 2D-DMA TX perform RGB888 to YUV444 conversion (follow BT601 standard) */
    DMA2D_CSC_TX_RGB888_TO_YUV444_709,          /*!< 2D-DMA TX perform RGB888 to YUV444 conversion (follow BT709 standard) */
    DMA2D_CSC_TX_RGB888_TO_YUV422_601,          /*!< 2D-DMA TX perform RGB888 to YUV422-MIPI conversion (follow BT601 standard) */
    DMA2D_CSC_TX_RGB888_TO_YUV422_709,          /*!< 2D-DMA TX perform RGB888 to YUV422-MIPI conversion (follow BT709 standard) */
    DMA2D_CSC_TX_YUV444_TO_RGB888_601,          /*!< 2D-DMA TX perform YUV444 to RGB888 conversion (follow BT601 standard) */
    DMA2D_CSC_TX_YUV444_TO_RGB888_709,          /*!< 2D-DMA TX perform YUV444 to RGB888 conversion (follow BT709 standard) */
    DMA2D_CSC_TX_YUV422_TO_RGB888_601,          /*!< 2D-DMA TX perform YUV422-MIPI to RGB888 conversion (follow BT601 standard) */
    DMA2D_CSC_TX_YUV422_TO_RGB888_709,          /*!< 2D-DMA TX perform YUV422-MIPI to RGB888 conversion (follow BT709 standard) */
    DMA2D_CSC_TX_INVALID,                       /*!< Invalid 2D-DMA TX color space conversion */
} dma2d_csc_tx_option_t;

/**
 * @brief Enumeration of 2D-DMA RX color space conversion (CSC) option
 * RX side only JPEG requires CSC
 */
typedef enum {
    DMA2D_CSC_RX_NONE,                          /*!< 2D-DMA RX perform no CSC */
    DMA2D_CSC_RX_SCRAMBLE,                      /*!< 2D-DMA RX perform only data scramble */
    DMA2D_CSC_RX_YUV422_TO_YUV444,              /*!< 2D-DMA RX perform YUV422 to YUV444 conversion */
    DMA2D_CSC_RX_YUV420_TO_YUV444,              /*!< 2D-DMA RX perform YUV420 to YUV444 conversion */
    DMA2D_CSC_RX_YUV420_TO_RGB888_601,          /*!< 2D-DMA RX perform YUV420 to RGB888 conversion (follow BT601 standard) */
    DMA2D_CSC_RX_YUV420_TO_RGB565_601,          /*!< 2D-DMA RX perform YUV420 to RGB565 conversion (follow BT601 standard) */
    DMA2D_CSC_RX_YUV420_TO_RGB888_709,          /*!< 2D-DMA RX perform YUV420 to RGB888 conversion (follow BT709 standard) */
    DMA2D_CSC_RX_YUV420_TO_RGB565_709,          /*!< 2D-DMA RX perform YUV420 to RGB565 conversion (follow BT709 standard) */
    DMA2D_CSC_RX_YUV422_TO_RGB888_601,          /*!< 2D-DMA RX perform YUV422 to RGB888 conversion (follow BT601 standard) */
    DMA2D_CSC_RX_YUV422_TO_RGB565_601,          /*!< 2D-DMA RX perform YUV422 to RGB565 conversion (follow BT601 standard) */
    DMA2D_CSC_RX_YUV422_TO_RGB888_709,          /*!< 2D-DMA RX perform YUV422 to RGB888 conversion (follow BT709 standard) */
    DMA2D_CSC_RX_YUV422_TO_RGB565_709,          /*!< 2D-DMA RX perform YUV422 to RGB565 conversion (follow BT709 standard) */
    DMA2D_CSC_RX_YUV444_TO_RGB888_601,          /*!< 2D-DMA RX perform YUV444 to RGB888 conversion (follow BT601 standard) */
    DMA2D_CSC_RX_YUV444_TO_RGB565_601,          /*!< 2D-DMA RX perform YUV444 to RGB565 conversion (follow BT601 standard) */
    DMA2D_CSC_RX_YUV444_TO_RGB888_709,          /*!< 2D-DMA RX perform YUV444 to RGB888 conversion (follow BT709 standard) */
    DMA2D_CSC_RX_YUV444_TO_RGB565_709,          /*!< 2D-DMA RX perform YUV444 to RGB565 conversion (follow BT709 standard) */
    DMA2D_CSC_RX_INVALID,                       /*!< Invalid 2D-DMA RX color space conversion */
} dma2d_csc_rx_option_t;

#ifdef __cplusplus
}
#endif
