/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Shared helpers for the libjpeg-turbo interoperability tests. See
 * `test_jpeg_turbo.h` for how they are used by `test_jpeg_turbo_encode.c` and
 * `test_jpeg_turbo_decode.c`.
 */

#include <inttypes.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

#include "sdkconfig.h"

#include "driver/jpeg_decode.h"
#include "driver/jpeg_encode.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "jerror.h"
#include "jpeglib.h"
#include "unity.h"

#include "test_jpeg_turbo.h"

static const char *TAG = "jpeg_turbo_test";

/*
 * libjpeg-turbo reports fatal errors through a callback that must not return, so
 * a `longjmp()` recovery point is layered on top of the stock error manager. The
 * manager is part of the JPEG object and outlives the enclosing function, so it
 * has to be a local that stays alive until `jpeg_destroy_*()` runs: every helper
 * below calls `setjmp()` before touching library state and jumps back here on
 * error.
 */
typedef struct {
    struct jpeg_error_mgr pub;
    jmp_buf jump_buffer;
} turbo_error_mgr_t;

static void turbo_error_exit(j_common_ptr cinfo)
{
    turbo_error_mgr_t *error_mgr = (turbo_error_mgr_t *)cinfo->err;
    char message[JMSG_LENGTH_MAX] = {0};

    (*cinfo->err->format_message)(cinfo, message);
    ESP_LOGE(TAG, "libjpeg-turbo reported: %s", message);
    longjmp(error_mgr->jump_buffer, 1);
}

void turbo_fill_bgr_pattern(uint8_t *bgr24)
{
    for (uint32_t y = 0; y < TURBO_TEST_HEIGHT; y++) {
        for (uint32_t x = 0; x < TURBO_TEST_WIDTH; x++) {
            uint8_t *pixel = bgr24 + ((size_t)y * TURBO_TEST_WIDTH + x) * TURBO_TEST_COMPONENTS;
            pixel[0] = (uint8_t)(240 - (x * 3) - (y * 3));   /* blue ramps diagonally */
            pixel[1] = (uint8_t)(16 + (y * 3));              /* green ramps along y */
            pixel[2] = (uint8_t)(16 + (x * 3));              /* red ramps along x */
        }
    }
}

/* Mean absolute per byte error between two equally sized buffers. */
static uint32_t turbo_mean_error(const uint8_t *reference, const uint8_t *candidate, size_t size)
{
    uint64_t total = 0;

    for (size_t i = 0; i < size; i++) {
        total += (reference[i] > candidate[i]) ? (reference[i] - candidate[i]) : (candidate[i] - reference[i]);
    }

    return (uint32_t)(total / size);
}

/* Largest absolute per byte error between two equally sized buffers. */
static uint32_t turbo_max_error(const uint8_t *reference, const uint8_t *candidate, size_t size)
{
    uint32_t worst = 0;

    for (size_t i = 0; i < size; i++) {
        uint32_t diff = (reference[i] > candidate[i]) ? (reference[i] - candidate[i]) : (candidate[i] - reference[i]);
        if (diff > worst) {
            worst = diff;
        }
    }

    return worst;
}

void turbo_expect_same_picture(const char *stage, const uint8_t *reference, const uint8_t *candidate, size_t size)
{
    uint32_t mean_error = turbo_mean_error(reference, candidate, size);
    uint32_t max_error = turbo_max_error(reference, candidate, size);

    ESP_LOGI(TAG, "%s: mean error %" PRIu32 ", max error %" PRIu32, stage, mean_error, max_error);
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(TURBO_TEST_MAX_MEAN_ERROR, mean_error);
    TEST_ASSERT_LESS_OR_EQUAL_UINT32(TURBO_TEST_MAX_PIXEL_ERROR, max_error);
}

size_t turbo_decode_to_bgr(const uint8_t *jpeg_data, size_t jpeg_size, uint8_t *bgr24_dst, size_t dst_size)
{
    turbo_error_mgr_t error_mgr;
    struct jpeg_decompress_struct cinfo;
    JSAMPARRAY row_buffer = NULL;
    size_t row_bytes = 0;
    size_t expected_size = 0;

    memset(&error_mgr, 0, sizeof(error_mgr));
    memset(&cinfo, 0, sizeof(cinfo));

    cinfo.err = jpeg_std_error(&error_mgr.pub);
    error_mgr.pub.error_exit = turbo_error_exit;

    if (setjmp(error_mgr.jump_buffer) != 0) {
        jpeg_destroy_decompress(&cinfo);
        TEST_FAIL_MESSAGE("libjpeg-turbo failed to decode the bit stream");
    }

    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, jpeg_data, (unsigned long)jpeg_size);
    TEST_ASSERT_EQUAL_INT(JPEG_HEADER_OK, jpeg_read_header(&cinfo, TRUE));
    /* The software decoder is the independent check that the hardware encoder
     * wrote 4:4:4. Asking the driver's own header parser would not catch a
     * matching bug in both sides. */
    TEST_ASSERT_EQUAL_INT(TURBO_TEST_COMPONENTS, cinfo.num_components);
    for (int component = 0; component < cinfo.num_components; component++) {
        TEST_ASSERT_EQUAL_INT(1, cinfo.comp_info[component].h_samp_factor);
        TEST_ASSERT_EQUAL_INT(1, cinfo.comp_info[component].v_samp_factor);
    }
    cinfo.out_color_space = JCS_EXT_BGR;
    TEST_ASSERT_TRUE(jpeg_start_decompress(&cinfo));

    /* If either direction ever disagreed about the picture, comparing
     * differently sized buffers would read out of bounds, so check first. */
    TEST_ASSERT_EQUAL_UINT(TURBO_TEST_WIDTH, cinfo.output_width);
    TEST_ASSERT_EQUAL_UINT(TURBO_TEST_HEIGHT, cinfo.output_height);
    TEST_ASSERT_EQUAL_INT(TURBO_TEST_COMPONENTS, cinfo.output_components);

    row_bytes = (size_t)cinfo.output_width * TURBO_TEST_COMPONENTS;
    expected_size = row_bytes * cinfo.output_height;
    TEST_ASSERT_GREATER_OR_EQUAL_size_t(expected_size, dst_size);

    row_buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, (JDIMENSION)row_bytes, 1);
    TEST_ASSERT_NOT_NULL(row_buffer);

    while (cinfo.output_scanline < cinfo.output_height) {
        uint32_t row = cinfo.output_scanline;
        TEST_ASSERT_EQUAL(1, jpeg_read_scanlines(&cinfo, row_buffer, 1));
        memcpy(bgr24_dst + (size_t)row * row_bytes, row_buffer[0], row_bytes);
    }

    TEST_ASSERT_TRUE(jpeg_finish_decompress(&cinfo));
    jpeg_destroy_decompress(&cinfo);

    return expected_size;
}

size_t turbo_encode_bgr(const uint8_t *bgr24_src, unsigned char **jpeg_out)
{
    turbo_error_mgr_t error_mgr;
    struct jpeg_compress_struct cinfo;
    unsigned long jpeg_size = 0;

    memset(&error_mgr, 0, sizeof(error_mgr));
    memset(&cinfo, 0, sizeof(cinfo));

    cinfo.err = jpeg_std_error(&error_mgr.pub);
    error_mgr.pub.error_exit = turbo_error_exit;

    if (setjmp(error_mgr.jump_buffer) != 0) {
        jpeg_destroy_compress(&cinfo);
        TEST_FAIL_MESSAGE("libjpeg-turbo failed to encode the picture");
    }

    jpeg_create_compress(&cinfo);
    jpeg_mem_dest(&cinfo, jpeg_out, &jpeg_size);
    cinfo.image_width = TURBO_TEST_WIDTH;
    cinfo.image_height = TURBO_TEST_HEIGHT;
    cinfo.input_components = TURBO_TEST_COMPONENTS;
    /* BGR input still ends up as a YCbCr JFIF bit stream; only the conversion
     * into it is done by the library here instead of by the hardware codec.
     * `JCS_EXT_BGR` matches the bgr24 layout `JPEG_ENCODE_IN_FORMAT_RGB888` reads. */
    cinfo.in_color_space = JCS_EXT_BGR;
    jpeg_set_defaults(&cinfo);
    /* `jpeg_set_defaults()` leaves 4:2:0 chroma subsampling selected. Switch to
     * 4:4:4 so the sampling factors match the ones the hardware encoder writes
     * for `JPEG_DOWN_SAMPLING_YUV444`, otherwise the chroma resolution asserted
     * by the decoder test would not be the one compared against. */
    for (int component = 0; component < TURBO_TEST_COMPONENTS; component++) {
        cinfo.comp_info[component].h_samp_factor = 1;
        cinfo.comp_info[component].v_samp_factor = 1;
    }
    /* Baseline only: the hardware decoder does not read arithmetic coded streams. */
    jpeg_set_quality(&cinfo, TURBO_TEST_QUALITY, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = (JSAMPROW)(bgr24_src + (size_t)cinfo.next_scanline * TURBO_TEST_WIDTH * TURBO_TEST_COMPONENTS);
        TEST_ASSERT_EQUAL(1, jpeg_write_scanlines(&cinfo, row_pointer, 1));
    }

    jpeg_finish_compress(&cinfo);
    /* `jpeg_destroy_compress()` frees the destination manager, but not the
     * buffer it handed out -- that stays owned by the caller. */
    jpeg_destroy_compress(&cinfo);

    return (size_t)jpeg_size;
}

uint8_t *turbo_alloc_codec_buffer(bool for_encoder, bool input_buffer,
                                  size_t wanted_size, size_t *allocated_size)
{
    uint8_t *buffer = NULL;

    if (for_encoder) {
        jpeg_encode_memory_alloc_cfg_t alloc_cfg = {
            .buffer_direction = input_buffer ? JPEG_ENC_ALLOC_INPUT_BUFFER : JPEG_ENC_ALLOC_OUTPUT_BUFFER,
        };
        buffer = (uint8_t *)jpeg_alloc_encoder_mem(wanted_size, &alloc_cfg, allocated_size);
    } else {
        jpeg_decode_memory_alloc_cfg_t alloc_cfg = {
            .buffer_direction = input_buffer ? JPEG_DEC_ALLOC_INPUT_BUFFER : JPEG_DEC_ALLOC_OUTPUT_BUFFER,
        };
        buffer = (uint8_t *)jpeg_alloc_decoder_mem(wanted_size, &alloc_cfg, allocated_size);
    }

    TEST_ASSERT_NOT_NULL(buffer);

    return buffer;
}
