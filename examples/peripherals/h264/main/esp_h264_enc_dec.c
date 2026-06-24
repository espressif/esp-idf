/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_h264_alloc.h"
#include "esp_h264_dec_sw.h"
#if CONFIG_H264_ENCODER_HARDWARE
#include "esp_h264_enc_single_hw.h"
#else
#include "esp_h264_enc_single_sw.h"
#endif  /* CONFIG_H264_ENCODER_HARDWARE */
#include "video_pattern.h"
#include "esp_log.h"

static const char *TAG = "H264_ENC_DEC";

#define FRAME_MAX_NUM 10

// Helper function to allocate aligned memory with error checking
static void *allocate_frame_buffer(size_t size, uint32_t *actual_size, const char *buffer_name)
{
    void *buffer = esp_h264_aligned_calloc(16, 1, size, actual_size, ESP_H264_MEM_SPIRAM);
    if (!buffer) {
        ESP_LOGE(TAG, "Failed to allocate %s buffer memory (%zu bytes)", buffer_name, size);
    }
    return buffer;
}

// Helper function to initialize pattern info
static void init_pattern_info(pattern_info_t *pattern, uint32_t width, uint32_t height, uint32_t format_id)
{
    pattern->res.width = width;
    pattern->res.height = height;
    pattern->format_id = format_id;
    pattern->vertical = false;
    pattern->bar_count = 16;
    pattern->data_size = width * height * 3 / 2;
}

/*
    This function is used to encode and decode a single frame.
     src_frame --> encoder --> enc_frame(dec_input) --> decoder --> dest_frame(out_pattern)
*/

#if CONFIG_H264_ENCODER_HARDWARE
esp_h264_err_t single_enc_dec_process(esp_h264_enc_cfg_hw_t enc_cfg, esp_h264_dec_cfg_sw_t dec_cfg)
#else
esp_h264_err_t single_enc_dec_process(esp_h264_enc_cfg_sw_t enc_cfg, esp_h264_dec_cfg_sw_t dec_cfg)
#endif  /* CONFIG_H264_ENCODER_HARDWARE */
{
    int frame_num = 0;
    // Frame buffers - Fixed types to match decoder expectations
    esp_h264_enc_in_frame_t src_frame = {0};    // Original input frame
    esp_h264_enc_out_frame_t enc_frame = {0};   // Encoded frame output
    esp_h264_dec_in_frame_t dec_input = {0};    // Decoder input frame (fixed type)
    esp_h264_dec_out_frame_t dest_frame = {0};  // Decoded frame output (fixed type)

    // Handles and variables
    esp_h264_err_t ret = ESP_H264_ERR_OK;
    esp_h264_enc_handle_t enc = NULL;
    esp_h264_dec_handle_t dec = NULL;

    // Pattern info structures
    pattern_info_t in_pattern = {};
    pattern_info_t out_pattern = {};

    size_t frame_size = enc_cfg.res.width * enc_cfg.res.height;
    size_t pixel_bits = 12; // 12 bits per pixel for YUV420
    if (enc_cfg.pic_type == ESP_H264_RAW_FMT_YUYV) {
        // Calculate frame size
        pixel_bits = 16; // 16 bits per pixel for YUYV
    }
    frame_size *= pixel_bits;

    // Initialize pattern configurations
    init_pattern_info(&in_pattern, enc_cfg.res.width, enc_cfg.res.height, enc_cfg.pic_type);
    init_pattern_info(&out_pattern, enc_cfg.res.width, enc_cfg.res.height, dec_cfg.pic_type);

    // Allocate frame buffers
    src_frame.raw_data.buffer = allocate_frame_buffer(frame_size, &src_frame.raw_data.len, "source frame");
    if (!src_frame.raw_data.buffer) {
        goto cleanup;
    }
    // Because of the different bitrate, the encoded frame buffer size is different.
    // It uses the same buffer size as the source frame to avoid not enough buffer error.
    enc_frame.raw_data.buffer = allocate_frame_buffer(frame_size, &enc_frame.raw_data.len, "encoded frame");
    if (!enc_frame.raw_data.buffer) {
        goto cleanup;
    }

    // Setup decoder input frame (correct structure for decoder)
    dec_input.raw_data.buffer = enc_frame.raw_data.buffer;

    // Assign pattern pixel buffers
    in_pattern.pixel = src_frame.raw_data.buffer;

    // Initialize H264 encoder
#if CONFIG_H264_ENCODER_HARDWARE
    ret = esp_h264_enc_hw_new(&enc_cfg, &enc);
#else
    ret = esp_h264_enc_sw_new(&enc_cfg, &enc);
#endif  /* CONFIG_H264_ENCODER_HARDWARE */
    if (ret != ESP_H264_ERR_OK) {
        ESP_LOGE(TAG, "Failed to create H264 encoder (error: %d)", ret);
        goto cleanup;
    }

    ret = esp_h264_enc_open(enc);
    if (ret != ESP_H264_ERR_OK) {
        ESP_LOGE(TAG, "Failed to open H264 encoder (error: %d)", ret);
        goto cleanup;
    }

    // Initialize H264 decoder
    ret = esp_h264_dec_sw_new(&dec_cfg, &dec);
    if (ret != ESP_H264_ERR_OK) {
        ESP_LOGE(TAG, "Failed to create H264 decoder (error: %d)", ret);
        goto cleanup;
    }

    ret = esp_h264_dec_open(dec);
    if (ret != ESP_H264_ERR_OK) {
        ESP_LOGE(TAG, "Failed to open H264 decoder (error: %d)", ret);
        goto cleanup;
    }

    ESP_LOGI(TAG, "H264 encode-decode loop started (%dx%d @ %dfps)",
             enc_cfg.res.width, enc_cfg.res.height, enc_cfg.fps);

    while (1) {
        // Generate input pattern
        gen_pattern_color_bar(&in_pattern);
        // Encode frame
        ret = esp_h264_enc_process(enc, &src_frame, &enc_frame);
        if (ret != ESP_H264_ERR_OK) {
            ESP_LOGE(TAG, "H264 encoding failed (error: %d)", ret);
            break;
        }
        //update decoder input
        dec_input.raw_data.len = enc_frame.length;
        // Decode frame
        ret = esp_h264_dec_process(dec, &dec_input, &dest_frame);
        if (ret != ESP_H264_ERR_OK) {
            ESP_LOGE(TAG, "H264 decoding failed (error: %d)", ret);
            break;
        }
        out_pattern.pixel = dest_frame.outbuf;
        // Display conversion result
        draw_convert_result(&in_pattern, &out_pattern);
        printf("\nFrame %d: source image             |            decoded image\n", frame_num);
        frame_num++;
        if (frame_num >= FRAME_MAX_NUM) {
            break;
        }
    }

cleanup:
    // Cleanup encoder
    esp_h264_enc_close(enc);
    esp_h264_enc_del(enc);
    // Cleanup decoder
    esp_h264_dec_close(dec);
    esp_h264_dec_del(dec);
    // Free memory buffers
    if (src_frame.raw_data.buffer) {
        esp_h264_free(src_frame.raw_data.buffer);
    }
    if (enc_frame.raw_data.buffer) {
        esp_h264_free(enc_frame.raw_data.buffer);
    }
    ESP_LOGI(TAG, "H264 process %s", (ret == ESP_H264_ERR_OK) ? "Completed successfully" : "Failed");
    return ret;
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

    // Always use software decoder since decoder choice was removed
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
    // Fixed format specifiers to use PRIu32 for uint32_t values
    ESP_LOGI(TAG, "Config: GOP=%d, Bitrate=%" PRIu32 " bps, QP=%d",
             CONFIG_H264_ENCODER_GOP_SIZE, CONFIG_H264_ENCODER_BITRATE, CONFIG_H264_ENCODER_QP_VALUE);

    // Start encode-decode process
    esp_h264_err_t ret = single_enc_dec_process(enc_cfg, dec_cfg);
    if (ret != ESP_H264_ERR_OK) {
        ESP_LOGE(TAG, "H264 example failed with error: %d", ret);
    }
}
