/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <assert.h>
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
#define EXAMPLE_IMAGE_SIZE         (EXAMPLE_IMAGE_WIDTH * EXAMPLE_IMAGE_HEIGHT * EXAMPLE_RGB565_PIXEL_SIZE)
#define EXAMPLE_BASE64_CHUNK_LEN   96
#define EXAMPLE_BORDER_WIDTH       8

/* The raw RGB565 asset is embedded by target_add_binary_data() in CMakeLists.txt.
 * RENAME_TO "image_rgb" gives the linker symbols below. */
extern const uint8_t image_rgb_start[] asm("_binary_image_rgb_start");
extern const uint8_t image_rgb_end[] asm("_binary_image_rgb_end");

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

static void transform_image(ppa_client_handle_t ppa_srm_handle, const void *src_buf, void *dst_buf)
{
    /* SRM means scale-rotate-mirror. This operation keeps the image at 1x scale
     * and rotates the full frame by 180 degrees. */
    ppa_srm_oper_config_t srm_config = {
        .in = {
            .buffer = src_buf,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_w = EXAMPLE_IMAGE_WIDTH,
            .block_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        },
        .out = {
            .buffer = dst_buf,
            .buffer_size = EXAMPLE_IMAGE_SIZE,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = 0,
            .block_offset_y = 0,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        },
        .rotation_angle = PPA_SRM_ROTATION_ANGLE_180,
        .scale_x = 1,
        .scale_y = 1,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config));
}

static void fill_rect(ppa_client_handle_t ppa_fill_handle, void *buffer, uint32_t x, uint32_t y,
                      uint32_t width, uint32_t height, color_pixel_argb8888_data_t color)
{
    /* The fill engine writes one solid-color rectangle into an existing output
     * image. All coordinates are in pixels. */
    ppa_fill_oper_config_t fill_config = {
        .out = {
            .buffer = buffer,
            .buffer_size = EXAMPLE_IMAGE_SIZE,
            .pic_w = EXAMPLE_IMAGE_WIDTH,
            .pic_h = EXAMPLE_IMAGE_HEIGHT,
            .block_offset_x = x,
            .block_offset_y = y,
            .fill_cm = PPA_FILL_COLOR_MODE_RGB565,
        },
        .fill_block_w = width,
        .fill_block_h = height,
        .fill_argb_color = color,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    ESP_ERROR_CHECK(ppa_do_fill(ppa_fill_handle, &fill_config));
}

static void draw_border(ppa_client_handle_t ppa_fill_handle, void *buffer)
{
    const color_pixel_argb8888_data_t amber = {
        .a = 0xff,
        .r = 0xff,
        .g = 0xd0,
        .b = 0x00,
    };
    const color_pixel_argb8888_data_t blue = {
        .a = 0xff,
        .r = 0x00,
        .g = 0x78,
        .b = 0xff,
    };

    fill_rect(ppa_fill_handle, buffer, 0, 0, EXAMPLE_IMAGE_WIDTH, EXAMPLE_BORDER_WIDTH, amber);
    fill_rect(ppa_fill_handle, buffer, 0, EXAMPLE_IMAGE_HEIGHT - EXAMPLE_BORDER_WIDTH,
              EXAMPLE_IMAGE_WIDTH, EXAMPLE_BORDER_WIDTH, amber);
    fill_rect(ppa_fill_handle, buffer, 0, EXAMPLE_BORDER_WIDTH,
              EXAMPLE_BORDER_WIDTH, EXAMPLE_IMAGE_HEIGHT - 2 * EXAMPLE_BORDER_WIDTH, amber);
    fill_rect(ppa_fill_handle, buffer, EXAMPLE_IMAGE_WIDTH - EXAMPLE_BORDER_WIDTH, EXAMPLE_BORDER_WIDTH,
              EXAMPLE_BORDER_WIDTH, EXAMPLE_IMAGE_HEIGHT - 2 * EXAMPLE_BORDER_WIDTH, amber);

    fill_rect(ppa_fill_handle, buffer, 40, 40, 48, 16, blue);
    fill_rect(ppa_fill_handle, buffer, 40, 40, 16, 48, blue);
    fill_rect(ppa_fill_handle, buffer, EXAMPLE_IMAGE_WIDTH - 88, EXAMPLE_IMAGE_HEIGHT - 56, 48, 16, blue);
    fill_rect(ppa_fill_handle, buffer, EXAMPLE_IMAGE_WIDTH - 56, EXAMPLE_IMAGE_HEIGHT - 88, 16, 48, blue);
}

static void encode_and_print_image(const uint8_t *image, size_t image_size)
{
    /* Binary image data is not safe to print directly on the serial console.
     * Base64 turns it into ASCII that pytest can capture and decode. */
    size_t encoded_len = 0;
    int ret = mbedtls_base64_encode(NULL, 0, &encoded_len, image, image_size);
    ESP_ERROR_CHECK((ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) ? ESP_OK : ESP_FAIL);

    unsigned char *encoded = calloc(encoded_len + 1, 1);
    assert(encoded);
    ESP_ERROR_CHECK(mbedtls_base64_encode(encoded, encoded_len + 1, &encoded_len, image, image_size) == 0 ? ESP_OK : ESP_FAIL);

    printf("IMAGE_META width=%u height=%u format=RGB565 encoding=base64\n", EXAMPLE_IMAGE_WIDTH, EXAMPLE_IMAGE_HEIGHT);
    print_base64_payload(encoded, encoded_len);
    free(encoded);
}

void app_main(void)
{
    const size_t embedded_size = image_rgb_end - image_rgb_start;

    printf("Loading embedded RGB565 image from flash...\n");
    printf("Embedded raw image size: %zu bytes\n", embedded_size);
    assert(embedded_size == EXAMPLE_IMAGE_SIZE);

    uint8_t *srm_buf = alloc_ppa_buffer(EXAMPLE_IMAGE_SIZE);

    ppa_client_handle_t ppa_srm_handle = NULL;
    ppa_client_config_t ppa_srm_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_srm_config, &ppa_srm_handle));

    ppa_client_handle_t ppa_fill_handle = NULL;
    ppa_client_config_t ppa_fill_config = {
        .oper_type = PPA_OPERATION_FILL,
        .max_pending_trans_num = 1,
    };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_fill_config, &ppa_fill_handle));

    /* The two steps below are deliberately simple and independent:
     * 1. SRM transforms the source image into srm_buf.
     * 2. Fill draws visible markers directly into srm_buf. */
    printf("Transforming image with PPA SRM...\n");
    /* PPA is backed by DMA2D, which can read the source image directly from
     * mapped flash. Only the destination buffers need DMA-capable RAM here. */
    transform_image(ppa_srm_handle, image_rgb_start, srm_buf);

    printf("Drawing border with PPA fill...\n");
    draw_border(ppa_fill_handle, srm_buf);

    encode_and_print_image(srm_buf, EXAMPLE_IMAGE_SIZE);
    printf("PPA image processing demo done.\n");

    ESP_ERROR_CHECK(ppa_unregister_client(ppa_srm_handle));
    ESP_ERROR_CHECK(ppa_unregister_client(ppa_fill_handle));
    free(srm_buf);
}
