/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbedtls/base64.h"
#include "esp_check.h"
#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "driver/isp_dma.h"
#include "driver/isp_core.h"
#include "driver/isp_color.h"

#define EXAMPLE_WIDTH            240
#define EXAMPLE_HEIGHT           280
#define EXAMPLE_BASE64_CHUNK_LEN 384
#define EXAMPLE_BASE64_DELAY_MS  10
#define EXAMPLE_DMA_ALIGN        64
#define EXAMPLE_FRAME_COUNT      1

extern const uint8_t sensor_raw_start[] asm("_binary_sensor_raw_start");
extern const uint8_t sensor_raw_end[] asm("_binary_sensor_raw_end");

static void *s_alloc_dma_buffer(size_t size)
{
    return heap_caps_aligned_calloc(EXAMPLE_DMA_ALIGN, 1, size,
                                    MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
}

static void s_configure_neutral_color(isp_proc_handle_t isp_proc)
{
    esp_isp_color_config_t color_cfg = {
        .color_contrast = { .integer = 1, .decimal = 0 },
        .color_saturation = { .integer = 1, .decimal = 0 },
        .color_hue = 0,
        .color_brightness = 0,
    };

    ESP_ERROR_CHECK(esp_isp_color_configure(isp_proc, &color_cfg));
    ESP_ERROR_CHECK(esp_isp_color_enable(isp_proc));
}

static void s_print_base64_payload(const unsigned char *encoded, size_t encoded_len)
{
    printf("IMAGE_BASE64_BEGIN\n");
    fflush(stdout);
    for (size_t offset = 0; offset < encoded_len; offset += EXAMPLE_BASE64_CHUNK_LEN) {
        size_t chunk_len = encoded_len - offset;
        if (chunk_len > EXAMPLE_BASE64_CHUNK_LEN) {
            chunk_len = EXAMPLE_BASE64_CHUNK_LEN;
        }
        printf("IMAGE_BASE64 %.*s\n", (int)chunk_len, (const char *)&encoded[offset]);
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(EXAMPLE_BASE64_DELAY_MS));
    }
    printf("IMAGE_BASE64_END\n");
    fflush(stdout);
}

void app_main(void)
{
    const uint32_t h_res = EXAMPLE_WIDTH;
    const uint32_t v_res = EXAMPLE_HEIGHT;
    const size_t in_size = (size_t)h_res * v_res;      // RAW8: 1 byte/pixel
    const size_t out_size = (size_t)h_res * v_res * 3; // RGB888: 3 bytes/pixel

    isp_proc_handle_t isp_proc = NULL;
    esp_isp_processor_cfg_t isp_cfg = {
        .clk_hz = 240 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_DWGDMA,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB888,
        .bayer_order = COLOR_RAW_ELEMENT_ORDER_BGGR,
        .has_line_start_packet = false,
        .has_line_end_packet = false,
        .h_res = h_res,
        .v_res = v_res,
        .dma_burst_size = 8,
    };
    ESP_ERROR_CHECK(esp_isp_new_processor(&isp_cfg, &isp_proc));
    ESP_ERROR_CHECK(esp_isp_enable(isp_proc));
    s_configure_neutral_color(isp_proc);

    uint8_t *isp_in_buf = s_alloc_dma_buffer(in_size);
    uint8_t *isp_out_buf = s_alloc_dma_buffer(out_size);
    assert(isp_in_buf && isp_out_buf);

    size_t embedded_raw_size = sensor_raw_end - sensor_raw_start;
    assert(embedded_raw_size == in_size);
    memcpy(isp_in_buf, sensor_raw_start, embedded_raw_size);

    size_t encoded_len = 0;
    int ret = mbedtls_base64_encode(NULL, 0, &encoded_len, isp_out_buf, out_size);
    ESP_ERROR_CHECK((ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) ? ESP_OK : ESP_FAIL);
    unsigned char *encoded = calloc(encoded_len + 1, 1);
    assert(encoded);

    printf("Feeding %d frames through ISP DMA input...\n", EXAMPLE_FRAME_COUNT);
    for (int frame = 0; frame < EXAMPLE_FRAME_COUNT; frame++) {
        ESP_ERROR_CHECK(esp_cache_msync(isp_in_buf, in_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M));
        ESP_ERROR_CHECK(esp_isp_dma_process_frame(isp_proc, isp_out_buf, isp_in_buf, 1000));
        ESP_ERROR_CHECK(esp_cache_msync(isp_out_buf, out_size, ESP_CACHE_MSYNC_FLAG_DIR_M2C));

        size_t out_len = 0;
        ESP_ERROR_CHECK(mbedtls_base64_encode(encoded, encoded_len + 1, &out_len, isp_out_buf, out_size) == 0 ? ESP_OK : ESP_FAIL);

        printf("IMAGE_META frame=%d width=%u height=%u format=BGR24 encoding=base64\n",
               frame, (unsigned)h_res, (unsigned)v_res);
        s_print_base64_payload(encoded, out_len);
        printf("Frame %d done\n", frame);
    }
    printf("ISP DMA visual demo done.\n");

    free(encoded);
    ESP_ERROR_CHECK(esp_isp_color_disable(isp_proc));
    ESP_ERROR_CHECK(esp_isp_disable(isp_proc));
    ESP_ERROR_CHECK(esp_isp_del_processor(isp_proc));
    heap_caps_free(isp_in_buf);
    heap_caps_free(isp_out_buf);
}
