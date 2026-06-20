/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "mbedtls/base64.h"
#include "esp_check.h"
#include "driver/jpeg_encode.h"

#define EXAMPLE_WIDTH              1280
#define EXAMPLE_HEIGHT             720
#define EXAMPLE_RGB_FRAME_SIZE     (EXAMPLE_WIDTH * EXAMPLE_HEIGHT * 3)
#define EXAMPLE_JPEG_BUFFER_SIZE   (EXAMPLE_RGB_FRAME_SIZE / 10) /* Estimate output size with an approximately 10:1 JPEG compression ratio for this demo image. */
#define EXAMPLE_BASE64_CHUNK_LEN   96
#define EXAMPLE_JPEG_QUALITY       80

extern const uint8_t esp720p_rgb_start[] asm("_binary_esp720p_rgb_start");
extern const uint8_t esp720p_rgb_end[]   asm("_binary_esp720p_rgb_end");

static void print_base64_payload(const unsigned char *encoded, size_t encoded_len)
{
    /* Split the printable payload into short lines so it is easy to read in
     * the serial monitor and robust for pytest to parse back into a JPEG. */
    printf("JPEG_BASE64_BEGIN\n");
    for (size_t offset = 0; offset < encoded_len; offset += EXAMPLE_BASE64_CHUNK_LEN) {
        size_t chunk_len = encoded_len - offset;
        if (chunk_len > EXAMPLE_BASE64_CHUNK_LEN) {
            chunk_len = EXAMPLE_BASE64_CHUNK_LEN;
        }
        printf("JPEG_BASE64 %.*s\n", (int)chunk_len, (const char *)&encoded[offset]);
    }
    printf("JPEG_BASE64_END\n");
}

void app_main(void)
{
    /* EMBED_FILES turns the raw asset into linker symbols, so the example can
     * read the picture directly from flash without mounting a filesystem. */
    const size_t embedded_size = esp720p_rgb_end - esp720p_rgb_start;
    uint32_t jpeg_size = 0;
    jpeg_encoder_handle_t jpeg_handle = NULL;

    printf("Loading embedded BGR24 image from flash...\n");
    printf("Embedded raw image size: %zu bytes\n", embedded_size);
    assert(embedded_size == EXAMPLE_RGB_FRAME_SIZE);

    /* Despite the enum name, the current driver maps
     * JPEG_ENCODE_IN_FORMAT_RGB888 to a BGR24-style byte layout.
     * Keep the embedded raw asset in bgr24 order or red/blue will swap. */
    jpeg_encode_cfg_t enc_config = {
        .src_type = JPEG_ENCODE_IN_FORMAT_RGB888,
        .sub_sample = JPEG_DOWN_SAMPLING_YUV422,
        .image_quality = EXAMPLE_JPEG_QUALITY,
        .width = EXAMPLE_WIDTH,
        .height = EXAMPLE_HEIGHT,
    };

    size_t result_buffer_size = 0;
    /* The output JPEG is compressed, so the example does not need to reserve
     * a full raw-frame worth of space for the bitstream. This 10:1 estimate
     * is intentionally conservative for the bundled demo image and quality
     * setting; real applications should size this buffer for their own worst
     * case and handle "buffer too small" errors if needed. */
    jpeg_encode_memory_alloc_cfg_t mem_cfg = {
        .buffer_direction = JPEG_ENC_ALLOC_OUTPUT_BUFFER,
    };
    uint8_t *jpeg_buf = (uint8_t *)jpeg_alloc_encoder_mem(EXAMPLE_JPEG_BUFFER_SIZE, &mem_cfg, &result_buffer_size);
    assert(jpeg_buf != NULL);

    /* Create the encoder instance once, then feed it one full-frame request. */
    jpeg_encode_engine_cfg_t encode_eng_cfg = {
        .timeout_ms = 200,
    };
    ESP_ERROR_CHECK(jpeg_new_encoder_engine(&encode_eng_cfg, &jpeg_handle));

    printf("JPEG encoder will read the embedded raw buffer directly from flash.\n");
    printf("Encoding BGR24(raw) -> JPEG...\n");
    ESP_ERROR_CHECK(jpeg_encoder_process(jpeg_handle, &enc_config, esp720p_rgb_start, EXAMPLE_RGB_FRAME_SIZE,
                                         jpeg_buf, result_buffer_size, &jpeg_size));
    printf("Encoded JPEG size: %" PRIu32 " bytes\n", jpeg_size);

    size_t encoded_len = 0;
    /* First call asks mbedTLS how big the base64 buffer must be, then the
     * second call performs the actual binary-to-text conversion. */
    int ret = mbedtls_base64_encode(NULL, 0, &encoded_len, jpeg_buf, jpeg_size);
    ESP_ERROR_CHECK((ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) ? ESP_OK : ESP_FAIL);
    unsigned char *encoded = calloc(encoded_len + 1, 1);
    assert(encoded != NULL);
    ESP_ERROR_CHECK(mbedtls_base64_encode(encoded, encoded_len + 1, &encoded_len, jpeg_buf, jpeg_size) == 0 ? ESP_OK : ESP_FAIL);

    /* JPEG_META plus the chunked JPEG_BASE64 lines form a tiny text protocol
     * that pytest understands and can reconstruct into a host-side .jpeg. */
    printf("JPEG_META width=%u height=%u format=JPEG encoding=base64 size=%" PRIu32 "\n",
           EXAMPLE_WIDTH, EXAMPLE_HEIGHT, jpeg_size);
    print_base64_payload(encoded, encoded_len);
    printf("JPEG encode demo done.\n");

    ESP_ERROR_CHECK(jpeg_del_encoder_engine(jpeg_handle));
    free(encoded);
    free(jpeg_buf);
}
