/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/jpeg_decode.h"
#include "mbedtls/base64.h"

#define EXAMPLE_BASE64_CHUNK_LEN   96
#define EXAMPLE_BYTES_PER_PIXEL    3

/* These linker symbols are generated automatically for the file added by
 * EMBED_FILES in CMakeLists.txt. They let the example treat the embedded
 * JPEG asset as a byte array stored in flash. */
extern const uint8_t example_jpeg_start[] asm("_binary_example_jpeg_start");
extern const uint8_t example_jpeg_end[]   asm("_binary_example_jpeg_end");

/* For JPEGs encoded with block-based chroma subsampling, the decoder output
 * buffer dimensions can be padded up to 16-pixel boundaries. This helper
 * rounds visible width or height up to that padded size. */
static uint32_t align_up_to_16(uint32_t value)
{
    return (value + 15U) & ~15U;
}

static void print_base64_payload(const unsigned char *encoded, size_t encoded_len)
{
    /* The payload is split into short lines so the UART log stays easy to
     * parse from pytest and less likely to be damaged by very long lines. */
    printf("JPEG_DECODE_BASE64_BEGIN\n");
    size_t chunk_idx = 0;
    for (size_t offset = 0; offset < encoded_len; offset += EXAMPLE_BASE64_CHUNK_LEN, ++chunk_idx) {
        size_t chunk_len = encoded_len - offset;
        if (chunk_len > EXAMPLE_BASE64_CHUNK_LEN) {
            chunk_len = EXAMPLE_BASE64_CHUNK_LEN;
        }
        printf("JPEG_DECODE_BASE64 %.*s\n", (int)chunk_len, (const char *)&encoded[offset]);
        /* Yield periodically to avoid watchdog triggers on long payloads. */
        if ((chunk_idx % 16U) == 15U) {
            vTaskDelay(1);
        }
    }
    printf("JPEG_DECODE_BASE64_END\n");
}

void app_main(void)
{
    const size_t embedded_size = example_jpeg_end - example_jpeg_start;
    jpeg_decoder_handle_t jpeg_handle = NULL;
    uint8_t *decoded_pixels = NULL;
    uint8_t *input_buf = NULL;
    unsigned char *encoded = NULL;

    printf("Loading embedded JPEG from flash...\n");
    printf("Embedded JPEG size: %zu bytes\n", embedded_size);

    /* Parse the JPEG header to learn the image dimensions. */
    jpeg_decode_picture_info_t header_info;
    ESP_ERROR_CHECK(jpeg_decoder_get_info(example_jpeg_start, embedded_size, &header_info));
    printf("JPEG header parsed: width=%" PRIu32 " height=%" PRIu32 "\n", header_info.width, header_info.height);

    /* The hardware decoder can pad the output image dimensions up to
     * 16-pixel boundaries, so the actual buffer may be larger than
     * width * height * 3. Allocate for the padded dimensions to avoid
     * out-of-bounds writes. */
    const uint32_t padded_width = align_up_to_16(header_info.width);
    const uint32_t padded_height = align_up_to_16(header_info.height);
    const uint32_t output_bytes = padded_width * padded_height * EXAMPLE_BYTES_PER_PIXEL;

    /* Ask the driver to allocate an output buffer for decoded pixels.
    * JPEG_DEC_ALLOC_OUTPUT_BUFFER tells the driver this memory will hold
    * the decode result (as opposed to an input bitstream buffer). */
    jpeg_decode_memory_alloc_cfg_t mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
    };
    size_t decoded_buffer_size = 0;
    decoded_pixels = (uint8_t *)jpeg_alloc_decoder_mem(output_bytes, &mem_cfg, &decoded_buffer_size);
    assert(decoded_pixels != NULL);

    /* Create a decoder engine instance. The timeout_ms value is the maximum
     * time the hardware is allowed to spend on a single decode call. */
    jpeg_decode_engine_cfg_t decode_eng_cfg = {
        .timeout_ms = 80,
    };
    ESP_ERROR_CHECK(jpeg_new_decoder_engine(&decode_eng_cfg, &jpeg_handle));

    /* RGB888 outputs 3 bytes per pixel. BGR order matches the default byte
     * layout expected by OpenCV and many display pipelines. */
    jpeg_decode_cfg_t decode_cfg = {
        .output_format = JPEG_DECODE_OUT_FORMAT_RGB888,
        .rgb_order = JPEG_DEC_RGB_ELEMENT_ORDER_BGR,
    };

    /* jpeg don't handle the encrypted data.*/
    const uint8_t *bit_stream = example_jpeg_start;
#if CONFIG_SECURE_FLASH_ENC_ENABLED
    size_t input_buffer_size = 0;
    jpeg_decode_memory_alloc_cfg_t in_mem_cfg = {
        .buffer_direction = JPEG_DEC_ALLOC_INPUT_BUFFER,
    };
    input_buf = (uint8_t *)jpeg_alloc_decoder_mem(embedded_size, &in_mem_cfg, &input_buffer_size);
    assert(input_buf != NULL);
    memcpy(input_buf, example_jpeg_start, embedded_size);
    bit_stream = input_buf;
#endif

    uint32_t decoded_size = 0;
    printf("Decoding JPEG -> RGB888...\n");
    ESP_ERROR_CHECK(jpeg_decoder_process(
                        jpeg_handle,
                        &decode_cfg,
                        bit_stream,
                        embedded_size,
                        decoded_pixels,
                        decoded_buffer_size,
                        &decoded_size
                    ));
    printf("Decoded RGB888 size: %" PRIu32 " bytes\n", decoded_size);

    /* Base64 turns the binary pixel data into printable ASCII so it can be
     * safely transported through the serial console and reconstructed by
     * pytest. The two-pass pattern (first call with NULL output to get the
     * required buffer size, then the real encode) is standard mbedtls usage. */
    size_t encoded_len = 0;
    int ret = mbedtls_base64_encode(NULL, 0, &encoded_len, decoded_pixels, decoded_size);
    ESP_ERROR_CHECK((ret == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) ? ESP_OK : ESP_FAIL);
    encoded = calloc(encoded_len + 1, 1);
    assert(encoded != NULL);
    ESP_ERROR_CHECK(mbedtls_base64_encode(encoded, encoded_len + 1, &encoded_len, decoded_pixels, decoded_size) == 0 ? ESP_OK : ESP_FAIL);

    /* JPEG_DECODE_INFO plus the chunked JPEG_DECODE_BASE64 lines form a tiny
     * text protocol that the pytest script understands and converts back
     * into a PPM golden file for comparison. */
    printf("JPEG_DECODE_INFO width=%" PRIu32 " height=%" PRIu32
           " padded_width=%" PRIu32 " padded_height=%" PRIu32
           " format=RGB888 encoding=base64 size=%" PRIu32 "\n",
           header_info.width, header_info.height, padded_width, padded_height, decoded_size);
    print_base64_payload(encoded, encoded_len);
    printf("JPEG decode demo done.\n");

    ESP_ERROR_CHECK(jpeg_del_decoder_engine(jpeg_handle));
    free(encoded);
    free(decoded_pixels);
    free(input_buf);
}
