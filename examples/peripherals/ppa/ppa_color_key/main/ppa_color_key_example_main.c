/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "driver/ppa.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbedtls/base64.h"

#define EXAMPLE_IMAGE_WIDTH        320
#define EXAMPLE_IMAGE_HEIGHT       240
#define EXAMPLE_RGB565_PIXEL_SIZE  2
#define EXAMPLE_RGB888_PIXEL_SIZE  3
#define EXAMPLE_RGB565_IMAGE_SIZE  (EXAMPLE_IMAGE_WIDTH * EXAMPLE_IMAGE_HEIGHT * EXAMPLE_RGB565_PIXEL_SIZE)
#define EXAMPLE_RGB888_IMAGE_SIZE  (EXAMPLE_IMAGE_WIDTH * EXAMPLE_IMAGE_HEIGHT * EXAMPLE_RGB888_PIXEL_SIZE)
#define EXAMPLE_BASE64_CHUNK_LEN   96
#define EXAMPLE_RED_KEY_MIN_R      0xA0
#define EXAMPLE_RED_KEY_MAX_G      0x78
#define EXAMPLE_RED_KEY_MAX_B      0x78
#define EXAMPLE_GLOW_BLEND_ALPHA   176
#define EXAMPLE_GLOW_CENTER_X      160
#define EXAMPLE_GLOW_CENTER_Y      120
#define EXAMPLE_GLOW_RADIUS_X      160
#define EXAMPLE_GLOW_RADIUS_Y      60
#define EXAMPLE_GLOW_HIGHLIGHT_X   120
#define EXAMPLE_GLOW_PHASE_Y       90

/* The raw RGB565 asset is embedded by target_add_binary_data() in CMakeLists.txt.
 * RENAME_TO "image_rgb" gives the linker symbols below. */
extern const uint8_t image_rgb_start[] asm("_binary_image_rgb_start");
extern const uint8_t image_rgb_end[] asm("_binary_image_rgb_end");

static uint8_t clamp_u8(int value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 0xFF) {
        return 0xFF;
    }
    return (uint8_t)value;
}

static uint8_t *alloc_ppa_buffer(size_t size)
{
    /* PPA uses DMA underneath, so input and output buffers must be DMA-capable
     * and cache-line aligned. PSRAM keeps the example friendly to boards with
     * limited internal RAM. */
    uint8_t *buffer = heap_caps_aligned_calloc(64, 1, size, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(buffer);
    return buffer;
}

static void print_base64_payload(const unsigned char *encoded, size_t encoded_len)
{
    printf("IMAGE_BASE64_BEGIN\n");
    size_t chunk_count = 0;
    for (size_t offset = 0; offset < encoded_len; offset += EXAMPLE_BASE64_CHUNK_LEN) {
        size_t chunk_len = encoded_len - offset;
        if (chunk_len > EXAMPLE_BASE64_CHUNK_LEN) {
            chunk_len = EXAMPLE_BASE64_CHUNK_LEN;
        }
        printf("IMAGE_BASE64 %.*s\n", (int)chunk_len, (const char *)&encoded[offset]);
        chunk_count++;
        if ((chunk_count % 16) == 0) {
            /* The complete payload is large. Yield periodically so the serial
             * monitor used by pytest can consume every line reliably. */
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    printf("IMAGE_BASE64_END\n");
}

static void prepare_glow_foreground(uint8_t *fg_buf)
{
    /* The foreground is generated in software so the example can blend a
     * modern glow effect into the whole scene without needing a second image
     * asset. The text is centered in this demo image, so the glow shape can
     * use fixed geometry instead of dynamically scanning the picture. */
    uint8_t *pixels = fg_buf;

    for (int y = 0; y < EXAMPLE_IMAGE_HEIGHT; y++) {
        for (int x = 0; x < EXAMPLE_IMAGE_WIDTH; x++) {
            const int dx = x - EXAMPLE_GLOW_CENTER_X;
            const int dy = y - EXAMPLE_GLOW_CENTER_Y;
            const int ellipse_x = (dx * dx * 255) / (EXAMPLE_GLOW_RADIUS_X * EXAMPLE_GLOW_RADIUS_X);
            const int ellipse_y = (dy * dy * 255) / (EXAMPLE_GLOW_RADIUS_Y * EXAMPLE_GLOW_RADIUS_Y);
            int halo = 255 - ellipse_x - ellipse_y;
            if (halo < 0) {
                halo = 0;
            }
            if (halo > 255) {
                halo = 255;
            }

            const int shimmer_dx = abs(x - EXAMPLE_GLOW_HIGHLIGHT_X);
            int shimmer = 255 - (shimmer_dx * 255) / EXAMPLE_GLOW_RADIUS_X;
            if (shimmer < 0) {
                shimmer = 0;
            }

            int intensity = halo + shimmer / 3;
            if (intensity > 255) {
                intensity = 255;
            }

            int red = 96 + (255 - intensity) * 36 / 255 + shimmer / 18;
            int green = 138 + intensity * 110 / 255;
            int blue = 208 + intensity * 47 / 255;
            if ((((y - EXAMPLE_GLOW_PHASE_Y) / 5) & 1) == 0) {
                green += 4;
                blue += 8;
            }

            /* PPA_BLEND_COLOR_MODE_RGB888 maps to BGR24 in memory. */
            size_t pixel_index = (y * EXAMPLE_IMAGE_WIDTH + x) * EXAMPLE_RGB888_PIXEL_SIZE;
            pixels[pixel_index] = clamp_u8(blue);
            pixels[pixel_index + 1] = clamp_u8(green);
            pixels[pixel_index + 2] = clamp_u8(red);
        }
    }
}

static void replace_keyed_text_with_glow(ppa_client_handle_t ppa_blend_handle, const void *bg_buf,
                                         const void *fg_buf, void *out_buf)
{
    /* Background color-keying searches the red ESP32 text. With
     * ck_reverse_bg2fg enabled, keyed background pixels are replaced by the
     * generated glow foreground while non-key pixels stay unchanged. */
    ppa_blend_oper_config_t blend_config = {
        .in_bg = {
            .buffer = bg_buf,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_w = EXAMPLE_IMAGE_WIDTH,
            .block_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB565,
        },
        .in_fg = {
            .buffer = fg_buf,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_w = EXAMPLE_IMAGE_WIDTH,
            .block_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB888,
        },
        .out = {
            .buffer = out_buf,
            .buffer_size = EXAMPLE_RGB565_IMAGE_SIZE,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB565,
        },
        .bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE,
        .fg_alpha_update_mode = PPA_ALPHA_FIX_VALUE,
        .fg_alpha_fix_val = 0,
        .bg_ck_en = true,
        .bg_ck_rgb_low_thres = {
            .r = EXAMPLE_RED_KEY_MIN_R,
            .g = 0x00,
            .b = 0x00,
        },
        .bg_ck_rgb_high_thres = {
            .r = 0xff,
            .g = EXAMPLE_RED_KEY_MAX_G,
            .b = EXAMPLE_RED_KEY_MAX_B,
        },
        .ck_reverse_bg2fg = true,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ESP_ERROR_CHECK(ppa_do_blend(ppa_blend_handle, &blend_config));
}

static void blend_glow_around_keyed_text(ppa_client_handle_t ppa_blend_handle, const void *bg_buf,
                                         const void *fg_buf, void *out_buf)
{
    /* Background color-keying searches the red ESP32 text. With
     * ck_reverse_bg2fg disabled, keyed background pixels stay unchanged while
     * all non-key pixels alpha-blend with the generated glow foreground. */
    ppa_blend_oper_config_t blend_config = {
        .in_bg = {
            .buffer = bg_buf,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_w = EXAMPLE_IMAGE_WIDTH,
            .block_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB565,
        },
        .in_fg = {
            .buffer = fg_buf,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_w = EXAMPLE_IMAGE_WIDTH,
            .block_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB888,
        },
        .out = {
            .buffer = out_buf,
            .buffer_size = EXAMPLE_RGB565_IMAGE_SIZE,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .blend_cm = PPA_BLEND_COLOR_MODE_RGB565,
        },
        .bg_alpha_update_mode = PPA_ALPHA_NO_CHANGE,
        .fg_alpha_update_mode = PPA_ALPHA_FIX_VALUE,
        .fg_alpha_fix_val = EXAMPLE_GLOW_BLEND_ALPHA,
        .bg_ck_en = true,
        .bg_ck_rgb_low_thres = {
            .r = EXAMPLE_RED_KEY_MIN_R,
            .g = 0x00,
            .b = 0x00,
        },
        .bg_ck_rgb_high_thres = {
            .r = 0xff,
            .g = EXAMPLE_RED_KEY_MAX_G,
            .b = EXAMPLE_RED_KEY_MAX_B,
        },
        .ck_reverse_bg2fg = false,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ESP_ERROR_CHECK(ppa_do_blend(ppa_blend_handle, &blend_config));
}

static void encode_and_print_image(const char *effect_name, const uint8_t *image, size_t image_size)
{
    /* Binary image data is not safe to print directly on the serial console.
     * Base64 turns it into ASCII that pytest can capture and decode. */
    size_t encoded_len = 0;
    int ret = mbedtls_base64_encode(NULL, 0, &encoded_len, image, image_size);
    ESP_ERROR_CHECK((ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) ? ESP_OK : ESP_FAIL);

    unsigned char *encoded = calloc(encoded_len + 1, 1);
    assert(encoded);
    ESP_ERROR_CHECK(mbedtls_base64_encode(encoded, encoded_len + 1, &encoded_len, image, image_size) == 0 ? ESP_OK : ESP_FAIL);

    printf("IMAGE_META effect=%s width=%u height=%u format=RGB565 encoding=base64\n",
           effect_name, EXAMPLE_IMAGE_WIDTH, EXAMPLE_IMAGE_HEIGHT);
    print_base64_payload(encoded, encoded_len);
    free(encoded);
}

void app_main(void)
{
    const size_t embedded_size = image_rgb_end - image_rgb_start;

    printf("Loading embedded RGB565 image from flash...\n");
    printf("Embedded raw image size: %zu bytes\n", embedded_size);
    assert(embedded_size == EXAMPLE_RGB565_IMAGE_SIZE);

    uint8_t *result_buf = alloc_ppa_buffer(EXAMPLE_RGB565_IMAGE_SIZE);
    uint8_t *foreground_buf = alloc_ppa_buffer(EXAMPLE_RGB888_IMAGE_SIZE);
    printf("Generating shared glow foreground...\n");
    prepare_glow_foreground(foreground_buf);

    ppa_client_handle_t ppa_blend_handle = NULL;
    ppa_client_config_t ppa_blend_config = {
        .oper_type = PPA_OPERATION_BLEND,
        .max_pending_trans_num = 1,
    };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_blend_config, &ppa_blend_handle));

    printf("Replacing keyed red text with glow foreground...\n");
    replace_keyed_text_with_glow(ppa_blend_handle, image_rgb_start, foreground_buf, result_buf);
    encode_and_print_image("replace_keyed_text_with_glow", result_buf, EXAMPLE_RGB565_IMAGE_SIZE);

    printf("Blending glow around keyed red text...\n");
    blend_glow_around_keyed_text(ppa_blend_handle, image_rgb_start, foreground_buf, result_buf);
    encode_and_print_image("blend_glow_around_keyed_text", result_buf, EXAMPLE_RGB565_IMAGE_SIZE);
    printf("PPA color key demo done.\n");

    ESP_ERROR_CHECK(ppa_unregister_client(ppa_blend_handle));
    free(result_buf);
    free(foreground_buf);
}
