/**
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_h264_alloc.h"
#include "esp_h264_dec_sw.h"
#if CONFIG_H264_ENCODER_HARDWARE
#include "esp_h264_enc_single_hw.h"
#else
#include "esp_h264_enc_single_sw.h"
#endif  /* CONFIG_H264_ENCODER_HARDWARE */
#include "video_pattern.h"
#include "esp_log.h"

static const char *TAG = "example";

#define FRAME_MAX_NUM 10

/**
 * @brief Allocate a 16-byte-aligned frame buffer in PSRAM.
 *
 * Keeping the source image and encoded bitstream in PSRAM leaves internal
 * memory available for the system and codec allocations. The returned buffer
 * is not zero-initialized: the pattern generator initializes the source frame,
 * and the encoder writes the valid byte range recorded in enc_frame.length.
 */
static void *allocate_frame_buffer(size_t size, uint32_t *actual_size)
{
    void *buffer = esp_h264_aligned_malloc(16, 1, size, actual_size, ESP_H264_MEM_SPIRAM);
    ESP_ERROR_CHECK(buffer ? ESP_OK : ESP_ERR_NO_MEM);
    return buffer;
}

/**
 * @brief Initialize metadata used to generate or display a color-bar frame.
 */
static void init_pattern_info(pattern_info_t *pattern, uint16_t width, uint16_t height,
                              esp_h264_raw_format_t format_id)
{
    pattern->res.width = width;
    pattern->res.height = height;
    pattern->format_id = format_id;
    pattern->vertical = false;
    pattern->bar_count = 16;
    pattern->data_size = (uint32_t)(width * height * ESP_H264_GET_BPP_BY_PIC_TYPE(format_id));
}

/**
 * @brief Decode all NAL units produced for one encoded video frame.
 *
 * esp_h264_dec_process() may consume only one NAL unit per call. It also
 * returns no image for non-picture NAL units such as SPS and PPS, so retain
 * the most recent non-empty decoded output while advancing by @c consume.
 */
static void decode_encoded_frame(esp_h264_dec_handle_t dec, const esp_h264_enc_out_frame_t *enc_frame,
                                 esp_h264_dec_out_frame_t *dest_frame)
{
    esp_h264_dec_in_frame_t dec_input = {
        .raw_data = {
            .buffer = enc_frame->raw_data.buffer,
            .len = enc_frame->length,
        },
    };
    esp_h264_dec_out_frame_t decoded_frame = {};
    bool picture_ready = false;

    while (dec_input.raw_data.len > 0) {
        uint32_t remaining_len = dec_input.raw_data.len;

        *dest_frame = (esp_h264_dec_out_frame_t) {};
        ESP_ERROR_CHECK((esp_err_t)esp_h264_dec_process(dec, &dec_input, dest_frame));
        ESP_ERROR_CHECK(dec_input.consume > 0 && dec_input.consume <= remaining_len ? ESP_OK : ESP_FAIL);

        dec_input.raw_data.buffer += dec_input.consume;
        dec_input.raw_data.len -= dec_input.consume;
        if (dest_frame->out_size > 0) {
            decoded_frame = *dest_frame;
            picture_ready = true;
        }
    }

    ESP_ERROR_CHECK(picture_ready ? ESP_OK : ESP_FAIL);
    *dest_frame = decoded_frame;
}

/*
    This function is used to encode and decode a single frame.
     src_frame --> encoder --> enc_frame --> decoder --> dest_frame(out_pattern)
*/
#if CONFIG_H264_ENCODER_HARDWARE
static void single_enc_dec_process(esp_h264_enc_cfg_hw_t enc_cfg, esp_h264_dec_cfg_sw_t dec_cfg)
#else
static void single_enc_dec_process(esp_h264_enc_cfg_sw_t enc_cfg, esp_h264_dec_cfg_sw_t dec_cfg)
#endif  /* CONFIG_H264_ENCODER_HARDWARE */
{
    esp_h264_enc_in_frame_t src_frame = {0};
    esp_h264_enc_out_frame_t enc_frame = {0};
    esp_h264_dec_out_frame_t dest_frame = {0};
    esp_h264_enc_handle_t enc = NULL;
    esp_h264_dec_handle_t dec = NULL;
    pattern_info_t in_pattern = {};
    pattern_info_t out_pattern = {};

    size_t frame_size = (size_t)(enc_cfg.res.width * enc_cfg.res.height
                                 * ESP_H264_GET_BPP_BY_PIC_TYPE(enc_cfg.pic_type));

    // The encoder writes the source format; the software decoder always outputs I420.
    init_pattern_info(&in_pattern, enc_cfg.res.width, enc_cfg.res.height, enc_cfg.pic_type);
    init_pattern_info(&out_pattern, enc_cfg.res.width, enc_cfg.res.height, dec_cfg.pic_type);

    src_frame.raw_data.buffer = allocate_frame_buffer(frame_size, &src_frame.raw_data.len);
    // The encoder API recommends an output buffer at least as large as its input.
    enc_frame.raw_data.buffer = allocate_frame_buffer(frame_size, &enc_frame.raw_data.len);

    in_pattern.pixel = src_frame.raw_data.buffer;

#if CONFIG_H264_ENCODER_HARDWARE
    ESP_ERROR_CHECK((esp_err_t)esp_h264_enc_hw_new(&enc_cfg, &enc));
#else
    ESP_ERROR_CHECK((esp_err_t)esp_h264_enc_sw_new(&enc_cfg, &enc));
#endif  /* CONFIG_H264_ENCODER_HARDWARE */

    // Opening prepares the codec instance after it has been created.
    ESP_ERROR_CHECK((esp_err_t)esp_h264_enc_open(enc));

    ESP_ERROR_CHECK((esp_err_t)esp_h264_dec_sw_new(&dec_cfg, &dec));
    ESP_ERROR_CHECK((esp_err_t)esp_h264_dec_open(dec));

    ESP_LOGI(TAG, "H264 encode-decode loop started (%dx%d @ %dfps)",
             enc_cfg.res.width, enc_cfg.res.height, enc_cfg.fps);

    for (int frame_num = 0; frame_num < FRAME_MAX_NUM; frame_num++) {
        ESP_ERROR_CHECK(gen_pattern_color_bar(&in_pattern));

        ESP_ERROR_CHECK((esp_err_t)esp_h264_enc_process(enc, &src_frame, &enc_frame));
        decode_encoded_frame(dec, &enc_frame, &dest_frame);

        out_pattern.pixel = dest_frame.outbuf;
        printf("\nFrame %d: source image             |            decoded image\n", frame_num);
        draw_convert_result(&in_pattern, &out_pattern);
    }

    ESP_ERROR_CHECK((esp_err_t)esp_h264_dec_close(dec));
    ESP_ERROR_CHECK((esp_err_t)esp_h264_dec_del(dec));
    ESP_ERROR_CHECK((esp_err_t)esp_h264_enc_close(enc));
    ESP_ERROR_CHECK((esp_err_t)esp_h264_enc_del(enc));
    esp_h264_free(src_frame.raw_data.buffer);
    esp_h264_free(enc_frame.raw_data.buffer);
    ESP_LOGI(TAG, "H264 process completed successfully");
}

void app_main(void)
{

#if CONFIG_H264_ENCODER_HARDWARE
    esp_h264_enc_cfg_hw_t enc_cfg = {
        .gop = CONFIG_H264_ENCODER_GOP_SIZE,
        .fps = CONFIG_H264_ENCODER_FPS,
        .res = {.width = CONFIG_H264_ENCODER_WIDTH, .height = CONFIG_H264_ENCODER_HEIGHT},
        .rc = {
            .bitrate = CONFIG_H264_ENCODER_BITRATE,
            .qp_min = CONFIG_H264_ENCODER_QP_VALUE,
            .qp_max = CONFIG_H264_ENCODER_QP_VALUE
        },
        .pic_type = ESP_H264_RAW_FMT_O_UYY_E_VYY,
    };
#else
    esp_h264_enc_cfg_sw_t enc_cfg = {
        .gop = CONFIG_H264_ENCODER_GOP_SIZE,
        .fps = CONFIG_H264_ENCODER_FPS,
        .res = {.width = CONFIG_H264_ENCODER_WIDTH, .height = CONFIG_H264_ENCODER_HEIGHT},
        .rc = {
            .bitrate = CONFIG_H264_ENCODER_BITRATE,
            .qp_min = CONFIG_H264_ENCODER_QP_VALUE,
            .qp_max = CONFIG_H264_ENCODER_QP_VALUE
        },
        .pic_type = ESP_H264_RAW_FMT_I420,
    };
#endif  /* CONFIG_H264_ENCODER_HARDWARE */

    // This example uses the portable software decoder for both encoder modes.
    esp_h264_dec_cfg_sw_t dec_cfg = {
        .pic_type = ESP_H264_RAW_FMT_I420,
    };

    ESP_LOGI(TAG, "H264 Example starting: %dx%d @ %dfps",
             CONFIG_H264_ENCODER_WIDTH, CONFIG_H264_ENCODER_HEIGHT, CONFIG_H264_ENCODER_FPS);
    ESP_LOGI(TAG, "Encoder: %s, Decoder: Software",
#if CONFIG_H264_ENCODER_HARDWARE
             "Hardware"
#else
             "Software"
#endif
            );
    ESP_LOGI(TAG, "Config: GOP=%d, Bitrate=%" PRIu32 " bps, QP=%d",
             CONFIG_H264_ENCODER_GOP_SIZE, CONFIG_H264_ENCODER_BITRATE, CONFIG_H264_ENCODER_QP_VALUE);

    single_enc_dec_process(enc_cfg, dec_cfg);
}
