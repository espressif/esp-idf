/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Picture geometry. The hardware codec works on MCU blocks, which are 8x8 pixels
 * for YUV444. Dimensions that are a multiple of 8 keep every MCU fully covered by
 * visible pixels, so neither direction has to pad a partially used edge block.
 */
#define TURBO_TEST_WIDTH            64
#define TURBO_TEST_HEIGHT           64
#define TURBO_TEST_COMPONENTS       3
#define TURBO_TEST_BGR_SIZE         (TURBO_TEST_WIDTH * TURBO_TEST_HEIGHT * TURBO_TEST_COMPONENTS)
#define TURBO_TEST_QUALITY          90
#define TURBO_TEST_TIMEOUT_MS       500

/*
 * Comparison budget, shared by every check in these files.
 *
 *                                            mean   max
 *   correct picture, one library            ~1.4    35
 *   two implementations, other rounding     ~5.1    24
 *   red and blue channels swapped          ~49.6   224
 *   chroma dropped, luma only              ~41.2   150
 *   pixels shifted by one                   ~4.4   253
 *   empty output                          ~113.2   253
 *   4:2:0 bit stream treated as 4:4:4       ~2.3   116
 *
 * So the mean catches a swapped channel or a lost conversion, and the maximum
 * catches a misalignment, a truncated buffer, or a wrong sampling mode. What
 * matters is the gap between the two groups; both limits sit inside it, which
 * leaves room for the hardware codec's own conversion.
 */
#define TURBO_TEST_MAX_MEAN_ERROR   16
#define TURBO_TEST_MAX_PIXEL_ERROR  80

/*
 * Fills `bgr24` with the reference picture, one byte per component in blue, green,
 * red order. That is the hardware codec's RGB888 layout (`ESP_COLOR_FOURCC_BGR24`)
 * and what libjpeg-turbo writes for `JCS_EXT_BGR`. It is smooth in both directions
 * so the quantization error spreads evenly instead of concentrating on isolated
 * pixels; constant areas would hide a wrong color conversion, stepping areas
 * would make single pixel errors dominate the mean.
 */
void turbo_fill_bgr_pattern(uint8_t *bgr24);

/* Asserts `candidate` reproduces `reference` within the rounding budget. */
void turbo_expect_same_picture(const char *stage, const uint8_t *reference, const uint8_t *candidate, size_t size);

/*
 * Decodes a JPEG bit stream into a tightly packed interleaved bgr24 buffer with
 * libjpeg-turbo. Asking for `JCS_EXT_BGR` gives one byte per component in blue,
 * green, red order, which is the layout the reference picture and the hardware
 * codec use.
 */
size_t turbo_decode_to_bgr(const uint8_t *jpeg_data, size_t jpeg_size, uint8_t *bgr24_dst, size_t dst_size);

/*
 * Compresses the interleaved bgr24 picture into a baseline JPEG with libjpeg-turbo.
 *
 * `*jpeg_out` receives the bit stream, which libjpeg-turbo allocates itself through
 * `malloc()`; the caller owns it and has to `free()` it. The return value is the
 * produced length.
 */
size_t turbo_encode_bgr(const uint8_t *bgr24_src, unsigned char **jpeg_out);

/*
 * Allocates a codec buffer through the driver's helper, which is the only way to
 * get memory that satisfies the cache line and 2D-DMA alignment the hardware
 * codec requires. `free()` releases it.
 */
uint8_t *turbo_alloc_codec_buffer(bool for_encoder, bool input_buffer, size_t wanted_size, size_t *allocated_size);

#ifdef __cplusplus
}
#endif
