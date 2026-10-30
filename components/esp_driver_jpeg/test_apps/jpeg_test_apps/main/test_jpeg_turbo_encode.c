/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * libjpeg-turbo encodes the generated picture; the hardware decoder turns the
 * bit stream back to bgr24, which is compared against the original picture.
 */

#include <stdlib.h>
#include <string.h>

#include "sdkconfig.h"

#include "driver/jpeg_decode.h"
#include "esp_heap_caps.h"
#include "unity.h"

#include "test_jpeg_turbo.h"

TEST_CASE("libjpeg-turbo encode to JPEG driver decode", "[jpeg][jpeg_turbo]")
{
    uint8_t *bgr_reference = heap_caps_calloc(1, TURBO_TEST_BGR_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(bgr_reference);
    turbo_fill_bgr_pattern(bgr_reference);

    unsigned char *jpeg = NULL;
    size_t compressed_size = turbo_encode_bgr(bgr_reference, &jpeg);
    TEST_ASSERT_GREATER_THAN_size_t(0, compressed_size);

    /* The header has to describe the picture as encoded. */
    jpeg_decode_picture_info_t picture_info = {0};
    TEST_ESP_OK(jpeg_decoder_get_info(jpeg, compressed_size, &picture_info));
    TEST_ASSERT_EQUAL_UINT(TURBO_TEST_WIDTH, picture_info.width);
    TEST_ASSERT_EQUAL_UINT(TURBO_TEST_HEIGHT, picture_info.height);
    TEST_ASSERT_EQUAL(JPEG_DOWN_SAMPLING_YUV444, picture_info.sample_method);

    /* Both the bit stream and the decoded picture have to live in buffers the
     * 2D-DMA may access, so the software codec's output is copied into one. */
    size_t bit_stream_size = 0;
    uint8_t *bit_stream = turbo_alloc_codec_buffer(false, true, compressed_size, &bit_stream_size);
    memcpy(bit_stream, jpeg, compressed_size);

    size_t output_capacity = 0;
    uint8_t *bgr_decoded = turbo_alloc_codec_buffer(false, false, TURBO_TEST_BGR_SIZE, &output_capacity);

    jpeg_decoder_handle_t decoder = NULL;
    jpeg_decode_engine_cfg_t engine_cfg = {
        .timeout_ms = TURBO_TEST_TIMEOUT_MS,
    };
    /* `JPEG_DECODE_OUT_FORMAT_RGB888` is bgr24. `rgb_order` is RGB when left
     * zero, so BGR has to be selected to keep the hardware's native layout. */
    jpeg_decode_cfg_t decode_cfg = {
        .output_format = JPEG_DECODE_OUT_FORMAT_RGB888,
        .rgb_order = JPEG_DEC_RGB_ELEMENT_ORDER_BGR,
    };
    TEST_ESP_OK(jpeg_new_decoder_engine(&engine_cfg, &decoder));
    uint32_t decoded_size = 0;
    TEST_ESP_OK(jpeg_decoder_process(decoder, &decode_cfg, bit_stream, compressed_size,
                                     bgr_decoded, output_capacity, &decoded_size));
    TEST_ESP_OK(jpeg_del_decoder_engine(decoder));
    TEST_ASSERT_EQUAL_size_t(TURBO_TEST_BGR_SIZE, decoded_size);

    turbo_expect_same_picture("libjpeg-turbo encode, driver decode", bgr_reference, bgr_decoded, TURBO_TEST_BGR_SIZE);

    free(bgr_decoded);
    free(bit_stream);
    free(jpeg);
    free(bgr_reference);
}
