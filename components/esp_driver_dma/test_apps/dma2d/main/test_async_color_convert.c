/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "soc/soc_caps.h"
#include "esp_heap_caps.h"
#include "hal/color_types.h"
#include "hal/color_hal.h"
#include "esp_async_color_convert.h"

typedef struct {
    SemaphoreHandle_t sem;
    int cb_called;
} async_color_convert_user_ctx_t;

static void fill_pattern(uint8_t *buf, size_t len, uint8_t seed)
{
    for (size_t i = 0; i < len; i++) {
        buf[i] = (uint8_t)(seed + i * 13);
    }
}

static bool test_async_color_convert_cb(async_color_convert_handle_t conv_hdl,
                                        async_color_convert_event_data_t *edata,
                                        void *cb_args)
{
    (void)conv_hdl;
    (void)edata;
    async_color_convert_user_ctx_t *user_ctx = (async_color_convert_user_ctx_t *)cb_args;
    user_ctx->cb_called++;
    BaseType_t high_task_wakeup = pdFALSE;
    xSemaphoreGiveFromISR(user_ctx->sem, &high_task_wakeup);
    return (high_task_wakeup == pdTRUE);
}

TEST_CASE("async color convert basic callback", "[async_color_convert]")
{
    const uint32_t width = 32;
    const uint32_t height = 24;
    const uint32_t pixel_num = width * height;

    uint16_t *src565 = heap_caps_aligned_calloc(64, pixel_num, sizeof(uint16_t),
                                                MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    uint8_t *dst888 = heap_caps_aligned_calloc(64, pixel_num, 3,
                                               MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src565);
    TEST_ASSERT_NOT_NULL(dst888);

    for (uint32_t i = 0; i < pixel_num; i++) {
        src565[i] = (uint16_t)((i * 13) ^ 0x5AA5);
    }

    async_color_convert_config_t config = {
        .backlog = 2,
        .intr_priority = 0,
        .dma_burst_size = 16,
    };
    async_color_convert_handle_t conv_hdl = NULL;
    TEST_ESP_OK(esp_async_color_convert_install_dma2d(&config, &conv_hdl));

    async_color_convert_request_t req = {
        .src_buffer = src565,
        .src_stride = width,
        .src_height = height,
        .src_x = 0,
        .src_y = 0,
        .dst_buffer = dst888,
        .dst_stride = width,
        .dst_height = height,
        .dst_x = 0,
        .dst_y = 0,
        .copy_width = width,
        .copy_height = height,
        .src_color_format = ESP_COLOR_FOURCC_RGB16,
        .dst_color_format = ESP_COLOR_FOURCC_BGR24,
    };

    async_color_convert_user_ctx_t user_ctx = {
        .sem = xSemaphoreCreateBinary(),
        .cb_called = 0,
    };
    TEST_ASSERT_NOT_NULL(user_ctx.sem);

    TEST_ESP_OK(esp_async_color_convert(conv_hdl, &req, test_async_color_convert_cb, &user_ctx));
    TEST_ASSERT_EQUAL(pdTRUE, xSemaphoreTake(user_ctx.sem, pdMS_TO_TICKS(200)));
    TEST_ASSERT_EQUAL(1, user_ctx.cb_called);

    vSemaphoreDelete(user_ctx.sem);
    TEST_ESP_OK(esp_async_color_convert_uninstall(conv_hdl));

    free(src565);
    free(dst888);
}

TEST_CASE("async color convert roundtrip: RGB565<->RGB888", "[async_color_convert]")
{
    const uint32_t width = 32;
    const uint32_t height = 20;
    const uint32_t pixel_num = width * height;

    uint16_t *src565 = heap_caps_aligned_calloc(64, pixel_num, sizeof(uint16_t),
                                                MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    uint8_t *mid888 = heap_caps_aligned_calloc(64, pixel_num, 3,
                                               MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    uint16_t *dst565 = heap_caps_aligned_calloc(64, pixel_num, sizeof(uint16_t),
                                                MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src565);
    TEST_ASSERT_NOT_NULL(mid888);
    TEST_ASSERT_NOT_NULL(dst565);

    for (uint32_t i = 0; i < pixel_num; i++) {
        src565[i] = (uint16_t)((i * 37) ^ 0xA55A);
    }

    async_color_convert_config_t config = {
        .backlog = 4,
        .intr_priority = 0,
        .dma_burst_size = 32,
    };
    async_color_convert_handle_t conv_hdl = NULL;
    TEST_ESP_OK(esp_async_color_convert_install_dma2d(&config, &conv_hdl));

    async_color_convert_request_t req_565_to_888 = {
        .src_buffer = src565,
        .src_stride = width,
        .src_height = height,
        .src_x = 0,
        .src_y = 0,
        .dst_buffer = mid888,
        .dst_stride = width,
        .dst_height = height,
        .dst_x = 0,
        .dst_y = 0,
        .copy_width = width,
        .copy_height = height,
        .src_color_format = ESP_COLOR_FOURCC_RGB16,
        .dst_color_format = ESP_COLOR_FOURCC_BGR24,
    };

    async_color_convert_request_t req_888_to_565 = {
        .src_buffer = mid888,
        .src_stride = width,
        .src_height = height,
        .src_x = 0,
        .src_y = 0,
        .dst_buffer = dst565,
        .dst_stride = width,
        .dst_height = height,
        .dst_x = 0,
        .dst_y = 0,
        .copy_width = width,
        .copy_height = height,
        .src_color_format = ESP_COLOR_FOURCC_BGR24,
        .dst_color_format = ESP_COLOR_FOURCC_RGB16,
    };

    TEST_ESP_OK(esp_color_convert_blocking(conv_hdl, &req_565_to_888, -1));
    TEST_ESP_OK(esp_color_convert_blocking(conv_hdl, &req_888_to_565, -1));

    // The final dst565 should be the same as the original src565 after round-trip conversion
    TEST_ASSERT_EQUAL_MEMORY(src565, dst565, pixel_num * sizeof(uint16_t));

    TEST_ESP_OK(esp_async_color_convert_uninstall(conv_hdl));

    free(src565);
    free(mid888);
    free(dst565);
}

TEST_CASE("async color convert bypasses color convert for 2D copy", "[async_color_convert]")
{
    const uint32_t src_stride = 48;
    const uint32_t src_height = 28;
    const uint32_t dst_stride = 64;
    const uint32_t dst_height = 30;
    const uint32_t copy_width = 32;
    const uint32_t copy_height = 18;
    const uint32_t src_x = 5;
    const uint32_t src_y = 4;
    const uint32_t dst_x = 7;
    const uint32_t dst_y = 6;
    const esp_color_fourcc_t fourcc = ESP_COLOR_FOURCC_RGB16;
    const size_t bytes_per_pixel = color_hal_pixel_format_fourcc_get_bit_depth(fourcc) / 8;
    const size_t src_size = (size_t)src_stride * src_height * bytes_per_pixel;
    const size_t dst_size = (size_t)dst_stride * dst_height * bytes_per_pixel;
    const size_t row_size = (size_t)copy_width * bytes_per_pixel;

    uint8_t *src = heap_caps_aligned_calloc(64, 1, src_size,
                                            MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    uint8_t *dst = heap_caps_aligned_calloc(64, 1, dst_size,
                                            MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    uint8_t *expected = heap_caps_aligned_calloc(64, 1, dst_size,
                                                 MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src);
    TEST_ASSERT_NOT_NULL(dst);
    TEST_ASSERT_NOT_NULL(expected);

    fill_pattern(src, src_size, 0x3C);
    memset(dst, 0xA5, dst_size);
    memset(expected, 0xA5, dst_size);

    for (uint32_t row = 0; row < copy_height; row++) {
        const size_t src_offset = ((size_t)(src_y + row) * src_stride + src_x) * bytes_per_pixel;
        const size_t dst_offset = ((size_t)(dst_y + row) * dst_stride + dst_x) * bytes_per_pixel;
        memcpy(expected + dst_offset, src + src_offset, row_size);
    }

    async_color_convert_config_t config = {
        .backlog = 2,
        .intr_priority = 0,
        .dma_burst_size = 16,
    };
    async_color_convert_handle_t conv_hdl = NULL;
    TEST_ESP_OK(esp_async_color_convert_install_dma2d(&config, &conv_hdl));

    async_color_convert_request_t req = {
        .src_buffer = src,
        .src_stride = src_stride,
        .src_height = src_height,
        .src_x = src_x,
        .src_y = src_y,
        .dst_buffer = dst,
        .dst_stride = dst_stride,
        .dst_height = dst_height,
        .dst_x = dst_x,
        .dst_y = dst_y,
        .copy_width = copy_width,
        .copy_height = copy_height,
        .src_color_format = fourcc,
        .dst_color_format = fourcc,
    };

    TEST_ESP_OK(esp_color_convert_blocking(conv_hdl, &req, -1));
    TEST_ASSERT_EQUAL_MEMORY(expected, dst, dst_size);

    TEST_ESP_OK(esp_async_color_convert_uninstall(conv_hdl));

    free(src);
    free(dst);
    free(expected);
}

static uint8_t clamp_to_u8(int value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 255) {
        return 255;
    }
    return (uint8_t)value;
}

static void uyvy_to_bgr24_reference_pixel(uint8_t y, uint8_t u, uint8_t v,
                                          color_conv_std_rgb_yuv_t color_conv_std,
                                          uint8_t *out_bgr)
{
    static const int bt601[3][4] = {
        { 298,    0,  409, -56906 },
        { 298, -100, -208,  34707 },
        { 298,  516,    0, -70836 },
    };
    static const int bt709[3][4] = {
        { 298,    0,  459, -63367 },
        { 298,  -55, -136,  19681 },
        { 298,  541,    0, -73918 },
    };

    const int (*coeff)[4] = (color_conv_std == COLOR_CONV_STD_RGB_YUV_BT709) ? bt709 : bt601;
    int r = (coeff[0][0] * y + coeff[0][1] * u + coeff[0][2] * v + coeff[0][3] + 128) >> 8;
    int g = (coeff[1][0] * y + coeff[1][1] * u + coeff[1][2] * v + coeff[1][3] + 128) >> 8;
    int b = (coeff[2][0] * y + coeff[2][1] * u + coeff[2][2] * v + coeff[2][3] + 128) >> 8;
    out_bgr[0] = clamp_to_u8(b);
    out_bgr[1] = clamp_to_u8(g);
    out_bgr[2] = clamp_to_u8(r);
}

static void uyvy_to_bgr24_reference_image(const uint8_t *src_uyvy, uint8_t *dst_bgr24,
                                          uint32_t src_stride, uint32_t dst_stride,
                                          uint32_t copy_width, uint32_t copy_height,
                                          color_conv_std_rgb_yuv_t color_conv_std)
{
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, copy_width % 2, "UYVY width must be even");

    for (uint32_t y = 0; y < copy_height; y++) {
        for (uint32_t x = 0; x < copy_width; x += 2) {
            size_t src_idx = ((size_t)y * src_stride + x) * 2;
            size_t dst_idx0 = ((size_t)y * dst_stride + x) * 3;
            size_t dst_idx1 = ((size_t)y * dst_stride + x + 1) * 3;
            uint8_t u = src_uyvy[src_idx + 0];
            uint8_t y0 = src_uyvy[src_idx + 1];
            uint8_t v = src_uyvy[src_idx + 2];
            uint8_t y1 = src_uyvy[src_idx + 3];

            uyvy_to_bgr24_reference_pixel(y0, u, v, color_conv_std, &dst_bgr24[dst_idx0]);
            uyvy_to_bgr24_reference_pixel(y1, u, v, color_conv_std, &dst_bgr24[dst_idx1]);
        }
    }
}

TEST_CASE("async color convert UYVY->RGB888 matches reference", "[async_color_convert]")
{
    const uint32_t src_stride = 32;
    const uint32_t dst_stride = 64;
    const uint32_t height = 2;
    const uint32_t copy_width = 6;
    const uint32_t copy_height = 2;
    const size_t src_size = src_stride * height * 2;
    const size_t dst_size = dst_stride * height * 3;
    static const uint8_t sample_uyvy[] = {
        128,  16, 128, 235,  90,  81, 240, 145, 240, 200,  16,  54,
        54,  32, 200, 210, 180, 100,  90, 180,  16, 235, 240,  16,
    };
    TEST_ASSERT_EQUAL(sizeof(sample_uyvy), copy_width * copy_height * 2);

    uint8_t *src = heap_caps_aligned_calloc(64, 1, src_size,
                                            MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    uint8_t *dst = heap_caps_aligned_calloc(64, 1, dst_size,
                                            MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    uint8_t *expected = heap_caps_aligned_calloc(64, 1, dst_size,
                                                 MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(src);
    TEST_ASSERT_NOT_NULL(dst);
    TEST_ASSERT_NOT_NULL(expected);
    for (uint32_t row = 0; row < copy_height; row++) {
        memcpy(src + row * src_stride * 2, sample_uyvy + row * copy_width * 2, copy_width * 2);
    }

    async_color_convert_config_t config = {
        .backlog = 2,
        .intr_priority = 0,
        .dma_burst_size = 16,
    };
    async_color_convert_handle_t conv_hdl = NULL;
    TEST_ESP_OK(esp_async_color_convert_install_dma2d(&config, &conv_hdl));

    const color_conv_std_rgb_yuv_t conv_stds[] = {
        COLOR_CONV_STD_RGB_YUV_BT601,
        COLOR_CONV_STD_RGB_YUV_BT709,
    };
    for (size_t i = 0; i < sizeof(conv_stds) / sizeof(conv_stds[0]); i++) {
        memset(dst, 0xA5, dst_size);
        memset(expected, 0xA5, dst_size);
        uyvy_to_bgr24_reference_image(src, expected, src_stride, dst_stride, copy_width, copy_height, conv_stds[i]);

        async_color_convert_request_t req = {
            .src_buffer = src,
            .src_stride = src_stride,
            .src_height = height,
            .src_x = 0,
            .src_y = 0,
            .dst_buffer = dst,
            .dst_stride = dst_stride,
            .dst_height = height,
            .dst_x = 0,
            .dst_y = 0,
            .copy_width = copy_width,
            .copy_height = copy_height,
            .src_color_format = ESP_COLOR_FOURCC_UYVY,
            .dst_color_format = ESP_COLOR_FOURCC_BGR24,
            .color_conv_std = conv_stds[i],
        };

        TEST_ESP_OK(esp_color_convert_blocking(conv_hdl, &req, -1));
        TEST_ASSERT_EQUAL_MEMORY(expected, dst, dst_size);
    }

    TEST_ESP_OK(esp_async_color_convert_uninstall(conv_hdl));
    free(src);
    free(dst);
    free(expected);
}
