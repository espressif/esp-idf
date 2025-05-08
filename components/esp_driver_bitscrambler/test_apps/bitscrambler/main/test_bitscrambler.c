/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/bitscrambler.h"
#include "driver/bitscrambler_loopback.h"
#include "esp_heap_caps.h"

BITSCRAMBLER_PROGRAM(bitscrambler_program_trivial, "trivial");
BITSCRAMBLER_PROGRAM(bitscrambler_program_timeout, "timeout");
BITSCRAMBLER_PROGRAM(bitscrambler_program_eof_upstream, "eof_upstream");
BITSCRAMBLER_PROGRAM(bitscrambler_program_eof_downstream, "eof_downstream");
BITSCRAMBLER_PROGRAM(bitscrambler_program_lut32, "lut32");
BITSCRAMBLER_PROGRAM(bitscrambler_program_loop, "loop");

TEST_CASE("Basic BitScrambler I/O", "[bs]")
{
    int len = 0x4000;
    uint8_t *data_in = heap_caps_aligned_calloc(8, 1, len, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *data_out = heap_caps_aligned_calloc(8, 1, len, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(data_in);
    TEST_ASSERT_NOT_NULL(data_out);
    for (int i = 0; i < len; i++) {
        data_in[i] = (uint8_t)rand();
        data_out[i] = 0xFF;
    }
    bitscrambler_handle_t bs;
    TEST_ESP_OK(bitscrambler_loopback_create(&bs, SOC_BITSCRAMBLER_ATTACH_I2S0, len));
    TEST_ESP_OK(bitscrambler_load_program(bs, bitscrambler_program_trivial));
    size_t res_len;
    TEST_ESP_OK(bitscrambler_loopback_run(bs, data_in, len, data_out, len, &res_len));
    bitscrambler_free(bs);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(data_in, data_out, len);
    TEST_ASSERT_EQUAL(len, res_len);

    free(data_in);
    free(data_out);
}

TEST_CASE("Timeout on stuck program", "[bs]")
{
    int len = 4096 * 10;
    uint8_t *data_in = heap_caps_aligned_calloc(8, 1, len, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint8_t *data_out = heap_caps_aligned_calloc(8, 1, len, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    bitscrambler_handle_t bs;

    TEST_ESP_OK(bitscrambler_loopback_create(&bs, SOC_BITSCRAMBLER_ATTACH_I2S0, len));
    TEST_ESP_OK(bitscrambler_load_program(bs, bitscrambler_program_timeout));
    esp_err_t err = bitscrambler_loopback_run(bs, data_in, len, data_out, len, NULL);
    TEST_ASSERT(err == ESP_ERR_TIMEOUT);
    bitscrambler_free(bs);
    free(data_in);
    free(data_out);
}

TEST_CASE("BitScrambler with EOF counted on upstream", "[bs]")
{
    const size_t len = 32;
    uint8_t *data_in = heap_caps_aligned_calloc(8, 1, len, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint32_t *data_out = heap_caps_aligned_calloc(8, 1, len * 4, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(data_in);
    TEST_ASSERT_NOT_NULL(data_out);

    bitscrambler_handle_t bs;
    TEST_ESP_OK(bitscrambler_loopback_create(&bs, SOC_BITSCRAMBLER_ATTACH_I2S0, len * 4));
    TEST_ESP_OK(bitscrambler_load_program(bs, bitscrambler_program_eof_upstream));
    size_t res_len = 0;
    TEST_ESP_OK(bitscrambler_loopback_run(bs, data_in, len, data_out, len * 4, &res_len));
    bitscrambler_free(bs);

    printf("BitScrambler program complete. Input %zu, output %zu bytes:\n", len, res_len);
    for (size_t i = 0; i < res_len / 4; i++) {
        printf("%08lX ", data_out[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
        TEST_ASSERT_EQUAL(0xFFFF0000, data_out[i]);
    }
    TEST_ASSERT_EQUAL(len * 4, res_len);

    free(data_in);
    free(data_out);
}

TEST_CASE("BitScrambler with EOF counted on downstream", "[bs]")
{
    const size_t len = 32;
    uint8_t *data_in = heap_caps_aligned_calloc(8, 1, len, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint32_t *data_out = heap_caps_aligned_calloc(8, 1, len * 4, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(data_in);
    TEST_ASSERT_NOT_NULL(data_out);

    bitscrambler_handle_t bs;
    TEST_ESP_OK(bitscrambler_loopback_create(&bs, SOC_BITSCRAMBLER_ATTACH_I2S0, len * 4));
    TEST_ESP_OK(bitscrambler_load_program(bs, bitscrambler_program_eof_downstream));
    size_t res_len = 0;
    TEST_ESP_OK(bitscrambler_loopback_run(bs, data_in, len, data_out, len * 4, &res_len));
    bitscrambler_free(bs);

    printf("BitScrambler program complete. Input %zu, output %zu bytes:\n", len, res_len);
    for (size_t i = 0; i < res_len / 4; i++) {
        printf("%08lX ", data_out[i]);
        if (i % 4 == 3) {
            printf("\n");
        }
        TEST_ASSERT_EQUAL(0xFFFF0000, data_out[i]);
    }
    TEST_ASSERT_EQUAL(len * 4, res_len);

    free(data_in);
    free(data_out);
}

TEST_CASE("BitScrambler with LUT32", "[bs]")
{
    const size_t len = 32;
    uint8_t *data_in = heap_caps_aligned_calloc(8, 1, len, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    uint32_t *data_out = heap_caps_aligned_calloc(8, 1, len * 4, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(data_in);
    TEST_ASSERT_NOT_NULL(data_out);

    bitscrambler_handle_t bs;
    TEST_ESP_OK(bitscrambler_loopback_create(&bs, SOC_BITSCRAMBLER_ATTACH_I2S0, len * 4));
    TEST_ESP_OK(bitscrambler_load_program(bs, bitscrambler_program_lut32));
    size_t res_len = 0;
    TEST_ESP_OK(bitscrambler_loopback_run(bs, data_in, len, data_out, len * 4, &res_len));
    bitscrambler_free(bs);

    printf("BitScrambler program complete. Input %zu, output %zu bytes:\n", len, res_len);
    for (size_t i = 0; i < res_len / 4; i++) {
        printf("%08lX ", data_out[i]);
        switch (i % 4) {
        case 0:
            TEST_ASSERT_EQUAL(0xF0011111, data_out[i]);
            break;
        case 1:
            TEST_ASSERT_EQUAL(0xF0022222, data_out[i]);
            break;
        case 2:
            TEST_ASSERT_EQUAL(0xF0033333, data_out[i]);
            break;
        case 3:
            TEST_ASSERT_EQUAL(0xF0004444, data_out[i]);
            printf("\n");
            break;
        }
    }
    TEST_ASSERT_EQUAL(len * 4, res_len);

    free(data_in);
    free(data_out);
}

TEST_CASE("BitScrambler with loop instruction", "[bs]")
{
    uint8_t data_in[] = {0x00, 0xFF, 0x55};
    size_t len = sizeof(data_in) / sizeof(data_in[0]) * 64;
    uint32_t *data_out = heap_caps_aligned_calloc(8, 1, len, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(data_out);

    bitscrambler_handle_t bs;
    TEST_ESP_OK(bitscrambler_loopback_create(&bs, SOC_BITSCRAMBLER_ATTACH_I2S0, len));
    TEST_ESP_OK(bitscrambler_load_program(bs, bitscrambler_program_loop));
    size_t res_len = 0;
    TEST_ESP_OK(bitscrambler_loopback_run(bs, data_in, sizeof(data_in) / sizeof(data_in[0]), data_out, len, &res_len));
    bitscrambler_free(bs);

    printf("BitScrambler program complete. Input %zu, output %zu bytes:\n", sizeof(data_in) / sizeof(data_in[0]), res_len);
    TEST_ASSERT_EQUAL(sizeof(data_in) / sizeof(data_in[0]) * 32 + 4, res_len);
    printf("%08lX\r\n", data_out[0]); // reset code
    TEST_ASSERT_EQUAL(0x00F000F0, data_out[0]);
    // encoding result for 0x00
    for (size_t i = 1; i < 9; i++) {
        printf("%08lX ", data_out[i]);
        TEST_ASSERT_EQUAL(0x80030009, data_out[i]);
    }
    printf("\n");
    // encoding result for 0xFF
    for (size_t i = 9; i < 17; i++) {
        printf("%08lX ", data_out[i]);
        TEST_ASSERT_EQUAL(0x80090003, data_out[i]);
    }
    printf("\n");
    // encoding result for 0x55
    for (size_t i = 17; i < 25; i++) {
        printf("%08lX ", data_out[i]);
        if (i & 0x01) {
            TEST_ASSERT_EQUAL(0x80030009, data_out[i]);
        } else {
            TEST_ASSERT_EQUAL(0x80090003, data_out[i]);
        }
    }
    printf("\n");

    free(data_out);
}
