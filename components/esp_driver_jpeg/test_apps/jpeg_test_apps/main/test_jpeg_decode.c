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
#include "driver/jpeg_decode.h"
#include "esp_log.h"
#include "test_jpeg_performance.h"
#include "esp_system.h"
#include "ccomp_timer.h"

extern const uint8_t image_esp1080_jpg_start[] asm("_binary_esp1080_jpg_start");
extern const uint8_t image_esp1080_jpg_end[]   asm("_binary_esp1080_jpg_end");

TEST_CASE("JPEG decode driver memory leaking check", "[jpeg]")
{
    jpeg_decoder_handle_t jpgd_handle;

    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .timeout_ms = 40,
    };

    int size = esp_get_free_heap_size();
    for (uint32_t i = 0; i <= 10; i++) {
        TEST_ESP_OK(jpeg_new_decoder_engine(&decode_eng_cfg, &jpgd_handle));
        vTaskDelay(10 / portTICK_PERIOD_MS);
        TEST_ESP_OK(jpeg_del_decoder_engine(jpgd_handle));
    }

    TEST_ASSERT_INT_WITHIN(400, size, esp_get_free_heap_size());
}

TEST_CASE("JPEG decode performance test for 1080*1920 YUV->RGB picture", "[jpeg]")
{
    jpeg_decoder_handle_t jpgd_handle;

    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .intr_priority = 0,
        .timeout_ms = 40,
    };

    jpeg_decode_cfg_t decode_cfg = {
        .output_format = JPEG_DECODE_OUT_FORMAT_RGB565,
    };

    jpeg_decode_memory_alloc_cfg_t rx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
    };

    jpeg_decode_memory_alloc_cfg_t tx_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_INPUT_BUFFER,
    };

    size_t rx_buffer_size;
    uint8_t *rx_buf_1080p = (uint8_t*)jpeg_alloc_decoder_mem(1080 * 1920 * 3, &rx_mem_cfg, &rx_buffer_size);
    uint32_t out_size_1080p = 0;

    size_t bit_stream_length = (size_t)image_esp1080_jpg_end - (size_t)image_esp1080_jpg_start;

    size_t tx_buffer_size;
    uint8_t *tx_buf_1080p = (uint8_t*)jpeg_alloc_decoder_mem(bit_stream_length, &tx_mem_cfg, &tx_buffer_size);
    // Copy bit stream to psram
    memcpy(tx_buf_1080p, image_esp1080_jpg_start, bit_stream_length);
    TEST_ESP_OK(jpeg_new_decoder_engine(&decode_eng_cfg, &jpgd_handle));

    ccomp_timer_start();

    // Decode picture for 50 times, and get the average
    uint8_t cnt = 50;
    for (int i = 0; i < cnt; i++) {
        TEST_ESP_OK(jpeg_decoder_process(jpgd_handle, &decode_cfg, tx_buf_1080p, bit_stream_length, rx_buf_1080p, rx_buffer_size, &out_size_1080p));
    }
    int64_t decode_time = ccomp_timer_stop();

    TEST_PERFORMANCE_GREATER_THAN(JPEG_DECODE_1080P_2_RGB565_PERFORMANCE, "1080p from *jpg -> rgb565 speed is %lld fps", 1 * 1000 * 1000 / (decode_time / cnt));

    free(rx_buf_1080p);
    free(tx_buf_1080p);
    TEST_ESP_OK(jpeg_del_decoder_engine(jpgd_handle));
}
