/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/periph_ctrl.h"
#include "driver/jpeg_encode.h"
#include "driver/jpeg_decode.h"
#include "esp_log.h"
#include "test_jpeg_performance.h"
#include "esp_system.h"
#include "ccomp_timer.h"

extern const uint8_t image_esp480_rgb_start[] asm("_binary_esp480_rgb_start");
extern const uint8_t image_esp480_rgb_end[]   asm("_binary_esp480_rgb_end");

TEST_CASE("JPEG encode driver memory leaking check", "[jpeg]")
{
    jpeg_encoder_handle_t encoder_handle;

    jpeg_encode_engine_cfg_t encode_eng_cfg = {
        .timeout_ms = 40,
    };

    int size = esp_get_free_heap_size();
    for (uint32_t i = 0; i <= 10; i++) {
        TEST_ESP_OK(jpeg_new_encoder_engine(&encode_eng_cfg, &encoder_handle));
        TEST_ESP_OK(jpeg_del_encoder_engine(encoder_handle));
    }

    TEST_ASSERT_INT_WITHIN(400, size, esp_get_free_heap_size());
}

TEST_CASE("JPEG encode performance test for 480*640 RGB->YUV picture", "[jpeg]")
{
    jpeg_encoder_handle_t jpeg_handle = NULL;

    jpeg_encode_engine_cfg_t encode_eng_cfg = {
        .intr_priority = 0,
        .timeout_ms = 40,
    };

    jpeg_encode_cfg_t enc_config = {
        .src_type = JPEG_ENCODE_IN_FORMAT_RGB888,
        .sub_sample = JPEG_DOWN_SAMPLING_YUV422,
        .image_quality = 80,
        .width = 640,
        .height = 480,
    };

    jpeg_encode_memory_alloc_cfg_t rx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
    };

    jpeg_encode_memory_alloc_cfg_t tx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_INPUT_BUFFER,
    };

    size_t rx_buffer_size = 0;
    uint8_t *jpg_buf_480p = (uint8_t*)jpeg_alloc_encoder_mem(480 * 640 * 3, &rx_mem_cfg, &rx_buffer_size);

    uint32_t jpg_size_480p = 0;

    size_t rgb_file_size = (size_t)image_esp480_rgb_end - (size_t)image_esp480_rgb_start;

    size_t tx_buffer_size = 0;
    uint8_t *raw_buf_480p = (uint8_t*)jpeg_alloc_encoder_mem(rgb_file_size, &tx_mem_cfg, &tx_buffer_size);
    // Copy bit stream to psram
    memcpy(raw_buf_480p, image_esp480_rgb_start, rgb_file_size);
    TEST_ESP_OK(jpeg_new_encoder_engine(&encode_eng_cfg, &jpeg_handle));

    ccomp_timer_start();

    // Decode picture for 50 times, and get the average
    uint8_t cnt = 50;
    for (int i = 0; i < cnt; i++) {
        TEST_ESP_OK(jpeg_encoder_process(jpeg_handle, &enc_config, raw_buf_480p, rgb_file_size, jpg_buf_480p, rx_buffer_size, &jpg_size_480p));
    }
    int64_t encode_time = ccomp_timer_stop();

    TEST_PERFORMANCE_GREATER_THAN(JPEG_ENCODE_RGB888_2_480P_PERFORMANCE, "480p from rgb888 -> *.jpg speed is %lld fps", 1 * 1000 * 1000 / (encode_time / cnt));

    free(jpg_buf_480p);
    free(raw_buf_480p);
    TEST_ESP_OK(jpeg_del_encoder_engine(jpeg_handle));
}

TEST_CASE("jpeg initialize twice test", "[jpeg]")
{
    jpeg_encoder_handle_t encoder_handle = NULL;

    jpeg_encode_engine_cfg_t encode_eng_cfg = {
        .intr_priority = 0,
        .timeout_ms = 40,
    };

    TEST_ESP_OK(jpeg_new_encoder_engine(&encode_eng_cfg, &encoder_handle));
    assert(encoder_handle != NULL);

    jpeg_decoder_handle_t decoder_handle = NULL;

    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .intr_priority = 0,
        .timeout_ms = 40,
    };

    TEST_ESP_OK(jpeg_new_decoder_engine(&decode_eng_cfg, &decoder_handle));
    assert(decoder_handle != NULL);

    TEST_ESP_OK(jpeg_del_encoder_engine(encoder_handle));
    TEST_ESP_OK(jpeg_del_decoder_engine(decoder_handle));
}
