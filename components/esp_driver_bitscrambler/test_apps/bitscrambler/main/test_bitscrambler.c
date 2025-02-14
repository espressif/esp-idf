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
