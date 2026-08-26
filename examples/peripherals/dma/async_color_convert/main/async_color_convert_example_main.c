/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "mbedtls/base64.h"
#include "esp_async_color_convert.h"
#include "esp_check.h"
#include "esp_heap_caps.h"

#define EXAMPLE_WIDTH             96
#define EXAMPLE_HEIGHT            64
#define EXAMPLE_BASE64_CHUNK_LEN  96

/* These linker symbols are generated automatically for the file added by
 * EMBED_FILES in CMakeLists.txt. They let the example treat the embedded
 * raw .yuv asset as a byte array stored in flash. */
extern const uint8_t sample_96x64_uyvy_yuv_start[] asm("_binary_sample_96x64_uyvy_yuv_start");
extern const uint8_t sample_96x64_uyvy_yuv_end[]   asm("_binary_sample_96x64_uyvy_yuv_end");

static void print_base64_payload(const unsigned char *encoded, size_t encoded_len)
{
    /* The payload is split into short lines so the UART log stays easy to
     * parse from pytest and less likely to be damaged by very long lines. */
    printf("IMAGE_BASE64_BEGIN\n");
    for (size_t offset = 0; offset < encoded_len; offset += EXAMPLE_BASE64_CHUNK_LEN) {
        size_t chunk_len = encoded_len - offset;
        if (chunk_len > EXAMPLE_BASE64_CHUNK_LEN) {
            chunk_len = EXAMPLE_BASE64_CHUNK_LEN;
        }
        printf("IMAGE_BASE64 %.*s\n", (int)chunk_len, (const char *)&encoded[offset]);
    }
    printf("IMAGE_BASE64_END\n");
}

void app_main(void)
{
    /* UYVY422 stores 2 bytes per pixel on average, while BGR/RGB888 uses
     * 3 bytes per pixel. The example keeps the image size small so the
     * buffers and UART payload stay beginner-friendly. */
    const size_t pixel_num = EXAMPLE_WIDTH * EXAMPLE_HEIGHT;
    const size_t yuv422_size = pixel_num * 2;
    const size_t rgb888_size = pixel_num * 3;
    const size_t embedded_size = sample_96x64_uyvy_yuv_end - sample_96x64_uyvy_yuv_start;

    printf("Loading embedded UYVY image from flash...\n");
    printf("Embedded image size: %zu bytes\n", embedded_size);
    assert(embedded_size == yuv422_size);

    /* The destination buffer still needs DMA-capable internal RAM because
     * DMA2D writes the converted pixels into this memory region. */
    uint8_t *dst_bgr = heap_caps_aligned_calloc(64, 1, rgb888_size,
                                                MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    assert(dst_bgr);

    async_color_convert_config_t config = {
        .backlog = 1, // because we use the blocking API, so only need 1 in-flight request at most
        .dma_burst_size = 16,
    };
    async_color_convert_handle_t conv_hdl = NULL;

    /* Install the async color convert driver with the DMA2D backend.
     * The returned handle is used by later conversion requests. */
    ESP_ERROR_CHECK(esp_async_color_convert_install_dma2d(&config, &conv_hdl));

    /* This request describes one full-frame conversion:
     * - source buffer: UYVY422 image
     * - destination buffer: BGR24 image
     * - stride/height: layout of each image in memory
     * - copy_width/copy_height: region to convert */
    async_color_convert_request_t req_yuv_to_bgr = {
        /* DMA2D can read the source image directly from mapped flash, so the
         * example does not need an extra CPU copy into internal RAM first. */
        .src_buffer = sample_96x64_uyvy_yuv_start,
        .src_stride = EXAMPLE_WIDTH,
        .src_height = EXAMPLE_HEIGHT,
        .src_x = 0,
        .src_y = 0,
        .dst_buffer = dst_bgr,
        .dst_stride = EXAMPLE_WIDTH,
        .dst_height = EXAMPLE_HEIGHT,
        .dst_x = 0,
        .dst_y = 0,
        .copy_width = EXAMPLE_WIDTH,
        .copy_height = EXAMPLE_HEIGHT,
        .src_color_format = ESP_COLOR_FOURCC_UYVY,
        .dst_color_format = ESP_COLOR_FOURCC_BGR24,
        .color_conv_std = COLOR_CONV_STD_RGB_YUV_BT601,
    };

    printf("Converting UYVY422 -> RGB888...\n");
    /* This example uses the blocking API for simplicity: the call returns only
     * after the hardware conversion is finished and dst_bgr contains the result. */
    ESP_ERROR_CHECK(esp_color_convert_blocking(conv_hdl, &req_yuv_to_bgr, -1));
    printf("Converted image size: %zu bytes\n", rgb888_size);

    /* Base64 turns the binary BGR image into printable ASCII so it can be
     * safely transported through the serial console and reconstructed by pytest. */
    size_t encoded_len = 0;
    int ret = mbedtls_base64_encode(NULL, 0, &encoded_len, dst_bgr, rgb888_size);
    ESP_ERROR_CHECK((ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) ? ESP_OK : ESP_FAIL);
    unsigned char *encoded = calloc(encoded_len + 1, 1);
    assert(encoded);
    ESP_ERROR_CHECK(mbedtls_base64_encode(encoded, encoded_len + 1, &encoded_len, dst_bgr, rgb888_size) == 0 ? ESP_OK : ESP_FAIL);

    /* IMAGE_META plus the chunked IMAGE_BASE64 lines form a tiny text protocol
     * that the pytest script understands and converts back into a PPM file. */
    printf("IMAGE_META width=%u height=%u format=BGR24 encoding=base64\n", EXAMPLE_WIDTH, EXAMPLE_HEIGHT);
    print_base64_payload(encoded, encoded_len);
    printf("Async color convert visual demo done.\n");

    ESP_ERROR_CHECK(esp_async_color_convert_uninstall(conv_hdl));
    free(encoded);
    free(dst_bgr);
}
