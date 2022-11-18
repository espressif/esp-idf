/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_utility.h"
#include "sdkconfig.h"

__attribute__((unused)) static const char* TAG = "efuse_key_test";

#ifdef CONFIG_EFUSE_VIRTUAL

TEST_CASE("Test keys and purposes, rd, wr, wr_key_purposes are in the initial state", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();

    for (esp_efuse_block_t num_key = EFUSE_BLK_KEY0; num_key < EFUSE_BLK_KEY_MAX; ++num_key) {
        printf("EFUSE_BLK_KEY%d, RD, WR, PURPOSE_USER, PURPOSE_USER WR ... \n", num_key - EFUSE_BLK_KEY0);
        uint8_t key[32] = { 0xEE };
        TEST_ESP_OK(esp_efuse_read_field_blob(ESP_EFUSE_KEY0, &key, sizeof(key) * 8));
        TEST_ASSERT_EACH_EQUAL_HEX8(0, key, sizeof(key));
        TEST_ASSERT_FALSE(esp_efuse_get_key_dis_read(num_key));
        TEST_ASSERT_FALSE(esp_efuse_get_key_dis_write(num_key));
        TEST_ASSERT_EQUAL(ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS, esp_efuse_get_key_purpose(num_key));

        esp_efuse_block_t key_block = EFUSE_BLK_MAX;
        TEST_ASSERT_TRUE(esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_USER, NULL));
        TEST_ASSERT_TRUE(esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_USER, &key_block));
        TEST_ASSERT_EQUAL(EFUSE_BLK_KEY0, key_block);

        printf("EFUSE_BLK_KEY%d, RD, WR, PURPOSE_USER, PURPOSE_USER WR ... OK\n", num_key - EFUSE_BLK_KEY0);
    }
}
#endif // CONFIG_EFUSE_VIRTUAL

// If using efuse is real, then turn off writing tests.
#if CONFIG_EFUSE_VIRTUAL || CONFIG_EFUSE_FPGA_TEST

static esp_err_t s_check_key(esp_efuse_block_t num_key, void* wr_key, esp_efuse_purpose_t purpose)
{
    size_t offset_in_bits = 0;
    uint8_t key_size = 32;
    if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS || purpose == ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2) {
        key_size = 16;
    }

    if (purpose == ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2) {
        offset_in_bits = 16 * 8;
    }

    uint8_t rd_key[32] = { 0xEE };
    TEST_ESP_OK(esp_efuse_read_block(EFUSE_BLK_KEY0, &rd_key, offset_in_bits, key_size * 8));

#ifndef CONFIG_EFUSE_FPGA_TEST
    TEST_ASSERT_EQUAL_HEX8_ARRAY(wr_key, rd_key, key_size);
#endif // not CONFIG_EFUSE_FPGA_TEST

    TEST_ASSERT_TRUE(esp_efuse_get_key_dis_write(num_key));
    if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY || purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS) {
        TEST_ASSERT_TRUE(esp_efuse_get_key_dis_read(num_key));
#if CONFIG_EFUSE_FPGA_TEST && !CONFIG_EFUSE_VIRTUAL
        TEST_ASSERT_EACH_EQUAL_HEX8(0, rd_key, key_size);
#endif // CONFIG_EFUSE_FPGA_TEST && ! CONFIG_EFUSE_VIRTUAL
    } else {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(wr_key, rd_key, key_size);
    }

    return ESP_OK;
}

void test_write_key(esp_efuse_block_t num_key, esp_efuse_purpose_t purpose) {
    int id = num_key - EFUSE_BLK_KEY0;
    printf("EFUSE_BLK_KEY%d, purpose=%d ... \n", id, purpose);

    uint8_t wr_key[32];
    for (int i = 0; i < sizeof(wr_key); i++) {
        wr_key[i] = id + 1 + i;
    }

    uint8_t key_size = sizeof(wr_key);
    if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS || purpose == ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2) {
        key_size = 16;
    }

    TEST_ASSERT_TRUE(esp_efuse_key_block_unused(num_key));

    TEST_ESP_OK(esp_efuse_write_key(num_key, purpose, &wr_key, key_size));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_efuse_write_key(num_key, purpose, &wr_key, key_size));

    TEST_ESP_OK(s_check_key(num_key, wr_key, purpose));

    TEST_ASSERT_FALSE(esp_efuse_key_block_unused(num_key));

    printf("EFUSE_BLK_KEY%d, purpose=%d ... OK\n", id, purpose);
}

#ifndef CONFIG_EFUSE_FPGA_TEST
TEST_CASE("Test esp_efuse_write_key for virt mode", "[efuse]")
{
    uint8_t rd_key[32] = { 0xEE };
    esp_efuse_utility_reset();
    esp_efuse_utility_update_virt_blocks();

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_key(EFUSE_BLK_KEY0, ESP_EFUSE_KEY_PURPOSE_USER, &rd_key, 33));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_key(EFUSE_BLK3, ESP_EFUSE_KEY_PURPOSE_USER, NULL, sizeof(rd_key)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_key(EFUSE_BLK0, ESP_EFUSE_KEY_PURPOSE_USER, &rd_key, sizeof(rd_key)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_key(EFUSE_BLK0, ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS, &rd_key, sizeof(rd_key)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_key(EFUSE_BLK0, ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2, &rd_key, sizeof(rd_key)));

    for (esp_efuse_purpose_t purpose = ESP_EFUSE_KEY_PURPOSE_USER; purpose < ESP_EFUSE_KEY_PURPOSE_MAX; ++purpose) {
        esp_efuse_utility_reset();
        esp_efuse_utility_update_virt_blocks();
        esp_efuse_utility_debug_dump_blocks();

        if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY) {
            TEST_ESP_OK(esp_efuse_write_field_bit(ESP_EFUSE_XTS_KEY_LENGTH_256));
        }

        TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
        test_write_key(EFUSE_BLK_KEY0, purpose);
        TEST_ASSERT_FALSE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));

        esp_efuse_utility_debug_dump_blocks();
    }
}
#endif // not CONFIG_EFUSE_FPGA_TEST

TEST_CASE("Test 1 esp_efuse_write_key for FPGA", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_TRUE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS, NULL));

    test_write_key(EFUSE_BLK_KEY0, ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS);

    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_FALSE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
#ifdef CONFIG_EFUSE_FPGA_TEST
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
#else
    TEST_ASSERT_FALSE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
#endif

    printf("reset efuses on the FPGA board for the next test\n");
}

TEST_CASE("Test 2 esp_efuse_write_key for FPGA", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_TRUE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS, NULL));

    test_write_key(EFUSE_BLK_KEY0, ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2);

    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_FALSE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
    TEST_ASSERT_FALSE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));

    printf("reset efuses on the FPGA board for the next test\n");
}

TEST_CASE("Test 3 esp_efuse_write_key for FPGA", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_TRUE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));

    TEST_ESP_OK(esp_efuse_write_field_bit(ESP_EFUSE_XTS_KEY_LENGTH_256));
    TEST_ASSERT_TRUE(esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY, NULL));

    test_write_key(EFUSE_BLK_KEY0, ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY);

    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_FALSE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
#ifdef CONFIG_EFUSE_FPGA_TEST
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
#else
    TEST_ASSERT_FALSE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
#endif

    printf("reset efuses on the FPGA board for the next test\n");
}

TEST_CASE("Test esp_efuse_write_keys", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_TRUE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS, NULL));
    esp_efuse_block_t key_block = EFUSE_BLK_MAX;

    enum { BLOCKS_NEEDED1 = 2 };
    esp_efuse_purpose_t purpose1[BLOCKS_NEEDED1] = {
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2,
    };
    uint8_t keys1[BLOCKS_NEEDED1][32] = {{0xEE}};

    for (int num_key = 0; num_key < BLOCKS_NEEDED1; ++num_key) {
        for (int i = 0; i < 32; ++i) {
            keys1[num_key][i] = purpose1[num_key] + i + 1;
        }
    }

    TEST_ESP_OK(esp_efuse_write_keys(purpose1, keys1, BLOCKS_NEEDED1));

    TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose1[0], &key_block));
    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY0, key_block);

    TEST_ESP_OK(s_check_key(EFUSE_BLK_KEY0, keys1[0], ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS));
    TEST_ESP_OK(s_check_key(EFUSE_BLK_KEY0, keys1[1], ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_V2));

    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_FALSE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
    TEST_ASSERT_FALSE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));

    enum { BLOCKS_NEEDED2 = 1 };
    esp_efuse_purpose_t purpose2[BLOCKS_NEEDED2] = {
            ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY,
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_efuse_write_keys(purpose2, keys1, BLOCKS_NEEDED2));

    printf("reset efuses on the FPGA board for the next test\n");
}

TEST_CASE("Test esp_efuse_write_keys for returned errors", "[efuse]")
{
    esp_efuse_utility_reset();
    esp_efuse_utility_update_virt_blocks();
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_TRUE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));

    enum { BLOCKS_NEEDED = 2 };
    esp_efuse_purpose_t purpose[BLOCKS_NEEDED] = {
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY_DERIVED_FROM_128_EFUSE_BITS,
        ESP_EFUSE_KEY_PURPOSE_MAX,                 // it leads  ESP_ERR_INVALID_ARG in esp_efuse_write_keys
    };
    uint8_t keys[BLOCKS_NEEDED][32];

    for (int num_key = 0; num_key < BLOCKS_NEEDED; ++num_key) {
        for (int i = 0; i < 32; ++i) {
            keys[num_key][i] = purpose[num_key] + i + 1;
        }
    }

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_keys(NULL, keys, BLOCKS_NEEDED));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_keys(purpose, NULL, BLOCKS_NEEDED));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_keys(purpose, keys, BLOCKS_NEEDED)); // ESP_EFUSE_KEY_PURPOSE_MAX is not a valid purpose.
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_keys(purpose, keys, 3));
    TEST_ASSERT_TRUE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
    TEST_ESP_OK(esp_efuse_write_keys(purpose, keys, 1));

    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_FALSE(esp_efuse_key_block_unused(EFUSE_BLK_KEY0));
#ifdef CONFIG_EFUSE_FPGA_TEST
    TEST_ASSERT_TRUE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
#else
    TEST_ASSERT_FALSE(esp_efuse_block_is_empty(EFUSE_BLK_KEY0));
#endif
}

#endif // CONFIG_EFUSE_VIRTUAL || CONFIG_EFUSE_FPGA_TEST
