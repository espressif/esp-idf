/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"
#include "esp_pm.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"

extern const uint8_t image_esp1080_jpg_start[] asm("_binary_esp1080_jpg_start");
extern const uint8_t image_esp1080_jpg_end[]   asm("_binary_esp1080_jpg_end");

#if SOC_LIGHT_SLEEP_SUPPORTED && CONFIG_PM_ENABLE
static void test_jpeg_sleep_retention(bool allow_pd)
{
    jpeg_decoder_handle_t jpgd_handle;

    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .intr_priority = 0,
        .timeout_ms = 40,
        .flags = {
            .allow_pd = allow_pd,
        },
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

    TEST_ESP_OK(jpeg_decoder_process(jpgd_handle, &decode_cfg, tx_buf_1080p, bit_stream_length, rx_buf_1080p, rx_buffer_size, &out_size_1080p));

    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);

#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif

    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(2 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
    printf("check if the sleep happened as expected\r\n");
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);

    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);

    TEST_ESP_OK(jpeg_decoder_process(jpgd_handle, &decode_cfg, tx_buf_1080p, bit_stream_length, rx_buf_1080p, rx_buffer_size, &out_size_1080p));

    free(rx_buf_1080p);
    free(tx_buf_1080p);
    esp_sleep_set_sleep_context(NULL);
    TEST_ESP_OK(jpeg_del_decoder_engine(jpgd_handle));
}

TEST_CASE("jpeg sleep retention test", "[jpeg]")
{
    test_jpeg_sleep_retention(false);
    test_jpeg_sleep_retention(true);
}
#endif
