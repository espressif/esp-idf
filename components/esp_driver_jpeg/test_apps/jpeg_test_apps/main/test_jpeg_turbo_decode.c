/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * The hardware encoder compresses a generated picture; libjpeg-turbo decodes
 * the bit stream back to bgr24, which is compared against the original picture.
 * An independent decoder reading the driver's bit stream catches mistakes the
 * driver's own decoder would mask.
 */

#include <stdlib.h>
#include <string.h>

#include "sdkconfig.h"

#include "driver/jpeg_encode.h"
#include "esp_heap_caps.h"
#include "unity.h"

#include "test_jpeg_turbo.h"

TEST_CASE("JPEG driver encode to libjpeg-turbo decode", "[jpeg][jpeg_turbo]")
{
    /* Reference stays in ordinary heap: the encoder is not given this buffer. */
    uint8_t *bgr_reference = heap_caps_malloc(TURBO_TEST_BGR_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(bgr_reference);
    turbo_fill_bgr_pattern(bgr_reference);

    /* Same bgr24 bytes the encoder reads for `JPEG_ENCODE_IN_FORMAT_RGB888`. */
    size_t raw_size = 0;
    uint8_t *raw_buffer = turbo_alloc_codec_buffer(true, true, TURBO_TEST_BGR_SIZE, &raw_size);
    memcpy(raw_buffer, bgr_reference, TURBO_TEST_BGR_SIZE);

    size_t jpeg_capacity = 0;
    uint8_t *jpeg = turbo_alloc_codec_buffer(true, false, TURBO_TEST_BGR_SIZE, &jpeg_capacity);

    jpeg_encoder_handle_t encoder = NULL;
    jpeg_encode_engine_cfg_t engine_cfg = {
        .timeout_ms = TURBO_TEST_TIMEOUT_MS,
    };
    /* YUV444 keeps every pixel's own chroma samples, which keeps chroma
     * subsampling out of the comparison: the only lossy step left is the DCT
     * quantization controlled by `image_quality`. */
    jpeg_encode_cfg_t encode_cfg = {
        .src_type = JPEG_ENCODE_IN_FORMAT_RGB888,
        .sub_sample = JPEG_DOWN_SAMPLING_YUV444,
        .image_quality = TURBO_TEST_QUALITY,
        .width = TURBO_TEST_WIDTH,
        .height = TURBO_TEST_HEIGHT,
    };
    TEST_ESP_OK(jpeg_new_encoder_engine(&engine_cfg, &encoder));
    uint32_t jpeg_size = 0;
    TEST_ESP_OK(jpeg_encoder_process(encoder, &encode_cfg, raw_buffer, raw_size,
                                     jpeg, jpeg_capacity, &jpeg_size));
    TEST_ESP_OK(jpeg_del_encoder_engine(encoder));
    TEST_ASSERT_GREATER_THAN_size_t(0, jpeg_size);

    uint8_t *bgr_decoded = heap_caps_malloc(TURBO_TEST_BGR_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(bgr_decoded);
    memset(bgr_decoded, 0, TURBO_TEST_BGR_SIZE);
    /* libjpeg-turbo reads the header and the picture. A matching bug in the
     * driver's own `jpeg_decoder_get_info` would not be visible here. */
    size_t decoded_size = turbo_decode_to_bgr(jpeg, jpeg_size, bgr_decoded, TURBO_TEST_BGR_SIZE);
    TEST_ASSERT_EQUAL_size_t(TURBO_TEST_BGR_SIZE, decoded_size);

    turbo_expect_same_picture("driver encode, libjpeg-turbo decode", bgr_reference, bgr_decoded, TURBO_TEST_BGR_SIZE);

    free(bgr_decoded);
    free(jpeg);
    free(raw_buffer);
    free(bgr_reference);
}
