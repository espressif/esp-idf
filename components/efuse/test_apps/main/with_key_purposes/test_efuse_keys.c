/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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
#ifdef CONFIG_EFUSE_FPGA_TEST
    esp_efuse_utility_update_virt_blocks();
#else
    esp_efuse_utility_erase_virt_blocks();
#endif
    esp_efuse_utility_debug_dump_blocks();

    for (esp_efuse_block_t num_key = EFUSE_BLK_KEY0; num_key < EFUSE_BLK_KEY_MAX; ++num_key) {
        printf("EFUSE_BLK_KEY%d, RD, WR, PURPOSE_USER, PURPOSE_USER WR ... \n", num_key - EFUSE_BLK_KEY0);
        uint8_t key[32] = { 0xEE };
        TEST_ESP_OK(esp_efuse_read_field_blob(esp_efuse_get_key(num_key), &key, sizeof(key) * 8));
        TEST_ASSERT_EACH_EQUAL_HEX8(0, key, sizeof(key));
        TEST_ASSERT_FALSE(esp_efuse_get_key_dis_read(num_key));
        TEST_ASSERT_FALSE(esp_efuse_get_key_dis_write(num_key));
        TEST_ASSERT_EQUAL(ESP_EFUSE_KEY_PURPOSE_USER, esp_efuse_get_key_purpose(num_key));
        TEST_ASSERT_FALSE(esp_efuse_get_keypurpose_dis_write(num_key));

        esp_efuse_block_t key_block = EFUSE_BLK_MAX;
        TEST_ASSERT_TRUE(esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_USER, NULL));
        TEST_ASSERT_TRUE(esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_USER, &key_block));
        TEST_ASSERT_EQUAL(EFUSE_BLK_KEY0, key_block);
        TEST_ASSERT_EQUAL(EFUSE_BLK_KEY0, esp_efuse_find_unused_key_block());

        printf("EFUSE_BLK_KEY%d, RD, WR, PURPOSE_USER, PURPOSE_USER WR ... OK\n", num_key - EFUSE_BLK_KEY0);
    }
}

#if SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK
TEST_CASE("Test efuse API blocks burning XTS and ECDSA keys into BLOCK9", "[efuse]")
{
    uint8_t key[32] = {0};
    esp_efuse_purpose_t purpose = ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY;
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_efuse_write_key(EFUSE_BLK9, purpose, &key, sizeof(key)));
#if SOC_FLASH_ENCRYPTION_XTS_AES_256
    purpose = ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1;
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_efuse_write_key(EFUSE_BLK9, purpose, &key, sizeof(key)));
    purpose = ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2;
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_efuse_write_key(EFUSE_BLK9, purpose, &key, sizeof(key)));
#endif
#if SOC_EFUSE_ECDSA_KEY
    purpose = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY;
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, esp_efuse_write_key(EFUSE_BLK9, purpose, &key, sizeof(key)));
#endif
}
#endif // SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK

#endif // CONFIG_EFUSE_VIRTUAL

// If using efuse is real, then turn off writing tests.
#if CONFIG_EFUSE_VIRTUAL || CONFIG_EFUSE_FPGA_TEST

static esp_err_t s_check_key(esp_efuse_block_t num_key, void* wr_key)
{
    uint8_t rd_key[32] = { 0xEE };
    esp_efuse_purpose_t purpose = esp_efuse_get_key_purpose(num_key);
    TEST_ESP_OK(esp_efuse_read_field_blob(esp_efuse_get_key(num_key), &rd_key, sizeof(rd_key) * 8));
#ifndef CONFIG_EFUSE_FPGA_TEST
    TEST_ASSERT_EQUAL_HEX8_ARRAY(wr_key, rd_key, sizeof(wr_key));
#endif // not CONFIG_EFUSE_FPGA_TEST

    TEST_ASSERT_TRUE(esp_efuse_get_key_dis_write(num_key));
    if (purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY ||
#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_256
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1 ||
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2 ||
#endif
#if SOC_EFUSE_ECDSA_KEY
            purpose == ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY ||
#endif
#if SOC_EFUSE_ECDSA_KEY_P192
            purpose == ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P192 ||
#endif
#if SOC_EFUSE_ECDSA_KEY_P384
            purpose == ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P384_L ||
            purpose == ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P384_H ||
#endif
#if SOC_PSRAM_ENCRYPTION_XTS_AES_128
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_PSRAM_KEY ||
#endif
#if SOC_PSRAM_ENCRYPTION_XTS_AES_256
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_PSRAM_KEY_1 ||
            purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_PSRAM_KEY_2 ||
#endif
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG ||
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE ||
#if SOC_KEY_MANAGER_SUPPORTED
            purpose == ESP_EFUSE_KEY_PURPOSE_KM_INIT_KEY ||
#endif
            purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        TEST_ASSERT_TRUE(esp_efuse_get_key_dis_read(num_key));
#if CONFIG_EFUSE_FPGA_TEST && !CONFIG_EFUSE_VIRTUAL
        TEST_ASSERT_EACH_EQUAL_HEX8(0, rd_key, sizeof(rd_key));
#endif // CONFIG_EFUSE_FPGA_TEST && ! CONFIG_EFUSE_VIRTUAL
    } else {
        TEST_ASSERT_FALSE(esp_efuse_get_key_dis_read(num_key));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(wr_key, rd_key, sizeof(wr_key));
    }

    TEST_ASSERT_EQUAL(purpose, esp_efuse_get_key_purpose(num_key));
    esp_efuse_purpose_t purpose2 = 0;
    const esp_efuse_desc_t** key_purpose = esp_efuse_get_purpose_field(num_key);
    TEST_ESP_OK(esp_efuse_read_field_blob(key_purpose, &purpose2, key_purpose[0]->bit_count));
    TEST_ASSERT_EQUAL(purpose, purpose2);
    TEST_ASSERT_TRUE(esp_efuse_get_keypurpose_dis_write(num_key));
    return ESP_OK;
}

void test_write_key(esp_efuse_block_t num_key, esp_efuse_purpose_t purpose) {
    int id = num_key - EFUSE_BLK_KEY0;
    printf("EFUSE_BLK_KEY%d, purpose=%d ... \n", id, purpose);

    uint8_t wr_key[32];
    for (int i = 0; i < sizeof(wr_key); i++) {
        wr_key[i] = id + 1 + i;
    }

    TEST_ASSERT_TRUE(esp_efuse_key_block_unused(num_key));

    TEST_ESP_OK(esp_efuse_write_key(num_key, purpose, &wr_key, sizeof(wr_key)));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_efuse_write_key(num_key, purpose, &wr_key, sizeof(wr_key)));

    TEST_ESP_OK(s_check_key(num_key, wr_key));

    TEST_ASSERT_FALSE(esp_efuse_key_block_unused(num_key));

    printf("EFUSE_BLK_KEY%d, purpose=%d ... OK\n", id, purpose);
}

#ifndef CONFIG_EFUSE_FPGA_TEST
TEST_CASE("Test esp_efuse_write_key for virt mode", "[efuse]")
{
    uint8_t rd_key[32] = { 0xEE };
    int tmp_purpose = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_key(EFUSE_BLK3, tmp_purpose,  &rd_key, sizeof(rd_key)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_key(EFUSE_BLK_KEY0, tmp_purpose, &rd_key, 33));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_key(EFUSE_BLK10, tmp_purpose, &rd_key, sizeof(rd_key)));

    for (esp_efuse_purpose_t g_purpose = ESP_EFUSE_KEY_PURPOSE_USER; g_purpose < ESP_EFUSE_KEY_PURPOSE_MAX; ++g_purpose) {
        if (g_purpose == ESP_EFUSE_KEY_PURPOSE_USER) {
            continue;
        }
        esp_efuse_utility_reset();
#ifdef CONFIG_EFUSE_FPGA_TEST
        esp_efuse_utility_update_virt_blocks();
#else
        esp_efuse_utility_erase_virt_blocks();
#endif
        esp_efuse_utility_debug_dump_blocks();

        TEST_ASSERT_FALSE(esp_efuse_find_purpose(g_purpose, NULL));

        for (esp_efuse_block_t num_key = (EFUSE_BLK_KEY_MAX - 1); num_key >= EFUSE_BLK_KEY0; --num_key) {
            esp_efuse_purpose_t purpose = g_purpose;
#if SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK
            if (num_key == EFUSE_BLK9 && (
#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_256
                purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1 ||
                purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2 ||
#endif //#ifdef SOC_EFUSE_SUPPORT_XTS_AES_256_KEYS
#if SOC_EFUSE_ECDSA_KEY
                purpose == ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY ||
#endif
                purpose == ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY)) {
                printf("BLOCK9 can not have the %d purpose, use RESERVED instead\n", purpose);
                purpose = ESP_EFUSE_KEY_PURPOSE_RESERVED;
            }
#endif // SOC_EFUSE_BLOCK9_KEY_PURPOSE_QUIRK
            int id = num_key - EFUSE_BLK_KEY0;
            TEST_ASSERT_EQUAL(id + 1, esp_efuse_count_unused_key_blocks());
            test_write_key(num_key, purpose);
            TEST_ASSERT_EQUAL(id, esp_efuse_count_unused_key_blocks());

            esp_efuse_block_t key_block = EFUSE_BLK_KEY_MAX;
            TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose, &key_block));
            TEST_ASSERT_EQUAL(num_key, key_block);
        }
        esp_efuse_utility_debug_dump_blocks();
    }
}
#endif // not CONFIG_EFUSE_FPGA_TEST

TEST_CASE("Test 1 esp_efuse_write_key for FPGA", "[efuse]")
{
    esp_efuse_utility_reset();
#ifdef CONFIG_EFUSE_FPGA_TEST
    esp_efuse_utility_update_virt_blocks();
#else
    esp_efuse_utility_erase_virt_blocks();
#endif
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_EQUAL_MESSAGE(EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0, esp_efuse_count_unused_key_blocks(), "Efuses should be in initial state");

    esp_efuse_purpose_t purpose [] = {
        ESP_EFUSE_KEY_PURPOSE_USER,
#if SOC_EFUSE_ECDSA_KEY
        ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY,
#else
        ESP_EFUSE_KEY_PURPOSE_RESERVED,
#endif
#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_256
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1,
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2,
#else
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY,
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY,
#endif
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY,
        ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL,
    };

    int max_keys = EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0;
    for (esp_efuse_block_t num_key = EFUSE_BLK_KEY0; num_key < EFUSE_BLK_KEY_MAX; ++num_key) {
        int id = num_key - EFUSE_BLK_KEY0;
        TEST_ASSERT_EQUAL(EFUSE_BLK_KEY_MAX - num_key, esp_efuse_count_unused_key_blocks());
        test_write_key(num_key, purpose[id]);
        TEST_ASSERT_EQUAL(EFUSE_BLK_KEY_MAX - num_key - 1, esp_efuse_count_unused_key_blocks());
        TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose[id], NULL));
        TEST_ASSERT_EQUAL(--max_keys, esp_efuse_count_unused_key_blocks());
    }
    esp_efuse_utility_debug_dump_blocks();
    printf("reset efuses on the FPGA board for the next test\n");
}

TEST_CASE("Test 2 esp_efuse_write_key for FPGA", "[efuse]")
{
    esp_efuse_utility_reset();
#ifdef CONFIG_EFUSE_FPGA_TEST
    esp_efuse_utility_update_virt_blocks();
#else
    esp_efuse_utility_erase_virt_blocks();
#endif
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_EQUAL_MESSAGE(EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0, esp_efuse_count_unused_key_blocks(), "Efuses should be in initial state");

    esp_efuse_purpose_t purpose [] = {
        ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG,
        ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE,
        ESP_EFUSE_KEY_PURPOSE_HMAC_UP,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2,
    };

    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY0, esp_efuse_find_unused_key_block());
    int max_keys = EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0;
    for (esp_efuse_block_t num_key = EFUSE_BLK_KEY0; num_key < EFUSE_BLK_KEY_MAX; ++num_key) {
        int id = num_key - EFUSE_BLK_KEY0;
        TEST_ASSERT_EQUAL(EFUSE_BLK_KEY_MAX - num_key, esp_efuse_count_unused_key_blocks());
        test_write_key(num_key, purpose[id]);
        TEST_ASSERT_EQUAL(EFUSE_BLK_KEY_MAX - num_key - 1, esp_efuse_count_unused_key_blocks());
        TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose[id], NULL));
        TEST_ASSERT_EQUAL(--max_keys, esp_efuse_count_unused_key_blocks());
        if (esp_efuse_count_unused_key_blocks()) {
            TEST_ASSERT_EQUAL(num_key + 1, esp_efuse_find_unused_key_block());
        } else {
            TEST_ASSERT_EQUAL(EFUSE_BLK_KEY_MAX, esp_efuse_find_unused_key_block());
        }
    }
    esp_efuse_utility_debug_dump_blocks();
    printf("reset efuses on the FPGA board for the next test\n");
}

TEST_CASE("Test esp_efuse_write_keys", "[efuse]")
{
    esp_efuse_utility_reset();
#ifdef CONFIG_EFUSE_FPGA_TEST
    esp_efuse_utility_update_virt_blocks();
#else
    esp_efuse_utility_erase_virt_blocks();
#endif
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_EQUAL_MESSAGE(EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0, esp_efuse_count_unused_key_blocks(), "Efuses should be in initial state");
    esp_efuse_block_t key_block = EFUSE_BLK_MAX;

    enum { BLOCKS_NEEDED1 = 2 };
#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_256
    esp_efuse_purpose_t purpose1[BLOCKS_NEEDED1] = {
            ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1,
            ESP_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2,
    };
#else
    esp_efuse_purpose_t purpose1[BLOCKS_NEEDED1] = {
        ESP_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY,
        ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_ALL
    };
#endif
    uint8_t keys1[BLOCKS_NEEDED1][32] = {{0xEE}};

    for (int num_key = 0; num_key < BLOCKS_NEEDED1; ++num_key) {
        for (int i = 0; i < 32; ++i) {
            keys1[num_key][i] = purpose1[num_key] + i + 1;
        }
    }

    TEST_ESP_OK(esp_efuse_write_keys(purpose1, keys1, BLOCKS_NEEDED1));

    TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose1[0], &key_block));
    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY0, key_block);
    TEST_ESP_OK(s_check_key(key_block, keys1[0]));

    TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose1[1], &key_block));
    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY1, key_block);
    TEST_ESP_OK(s_check_key(key_block, keys1[1]));
    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0 - 2, esp_efuse_count_unused_key_blocks());

    enum { BLOCKS_NEEDED2 = 3 };
    esp_efuse_purpose_t purpose2[BLOCKS_NEEDED2] = {
            ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0,
            ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1,
            ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2,
    };
    uint8_t keys2[BLOCKS_NEEDED2][32] = {{0xDD}};

    for (int num_key = 0; num_key < BLOCKS_NEEDED2; ++num_key) {
        for (int i = 0; i < 32; ++i) {
            keys2[num_key][i] = purpose2[num_key] + i + 1;
        }
    }

    TEST_ESP_OK(esp_efuse_write_keys(purpose2, keys2, BLOCKS_NEEDED2));

    TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose2[0], &key_block));
    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY2, key_block);
    TEST_ESP_OK(s_check_key(key_block, keys2[0]));

    TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose2[1], &key_block));
    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY3, key_block);
    TEST_ESP_OK(s_check_key(key_block, keys2[1]));

    TEST_ASSERT_TRUE(esp_efuse_find_purpose(purpose2[2], &key_block));
    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY4, key_block);
    TEST_ESP_OK(s_check_key(key_block, keys2[2]));
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0 - 2 - 3, esp_efuse_count_unused_key_blocks());

    printf("reset efuses on the FPGA board for the next test\n");
}

TEST_CASE("Test esp_efuse_write_keys for returned errors", "[efuse]")
{
    esp_efuse_utility_reset();
#ifdef CONFIG_EFUSE_FPGA_TEST
    esp_efuse_utility_update_virt_blocks();
#else
    esp_efuse_utility_erase_virt_blocks();
#endif
    esp_efuse_utility_debug_dump_blocks();
    TEST_ASSERT_EQUAL_MESSAGE(EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0, esp_efuse_count_unused_key_blocks(), "Efuses should be in initial state");

    enum { BLOCKS_NEEDED = 4 };
    esp_efuse_purpose_t purpose[BLOCKS_NEEDED] = {
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST0,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST1,
        ESP_EFUSE_KEY_PURPOSE_SECURE_BOOT_DIGEST2,
        ESP_EFUSE_KEY_PURPOSE_MAX,                 // it leads  ESP_ERR_INVALID_ARG in esp_efuse_write_keys
    };
    uint8_t keys[BLOCKS_NEEDED][32] = {{0xEE}};

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_keys(NULL, keys, BLOCKS_NEEDED));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_keys(purpose, NULL, BLOCKS_NEEDED));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_keys(purpose, keys, (EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0) + 1));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_write_keys(purpose, keys, BLOCKS_NEEDED)); // ESP_EFUSE_KEY_PURPOSE_MAX is not a valid purpose.
    TEST_ASSERT_EQUAL(EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0, esp_efuse_count_unused_key_blocks());
    TEST_ESP_OK(esp_efuse_write_keys(purpose, keys, BLOCKS_NEEDED - 1));
    TEST_ASSERT_EQUAL((EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0) - (BLOCKS_NEEDED - 1), esp_efuse_count_unused_key_blocks());
    unsigned unused_keys = esp_efuse_count_unused_key_blocks();
    TEST_ESP_ERR(ESP_ERR_NOT_ENOUGH_UNUSED_KEY_BLOCKS, esp_efuse_write_keys(purpose, keys, unused_keys + 1));
}

#if SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY

TEST_CASE("Test revocation APIs", "[efuse]")
{
    esp_efuse_utility_reset();
#ifdef CONFIG_EFUSE_FPGA_TEST
    esp_efuse_utility_update_virt_blocks();
#else
    esp_efuse_utility_erase_virt_blocks();
#endif
    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(0));
    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(1));
    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(2));

    TEST_ASSERT_FALSE(esp_efuse_get_write_protect_of_digest_revoke(0));
    TEST_ASSERT_FALSE(esp_efuse_get_write_protect_of_digest_revoke(1));
    TEST_ASSERT_FALSE(esp_efuse_get_write_protect_of_digest_revoke(2));

    // esp_efuse_get_digest_revoke(3); // assert

    TEST_ESP_OK(esp_efuse_set_digest_revoke(0));
    TEST_ASSERT_TRUE(esp_efuse_get_digest_revoke(0));

    TEST_ESP_OK(esp_efuse_set_digest_revoke(1));
    TEST_ASSERT_TRUE(esp_efuse_get_digest_revoke(1));

    TEST_ESP_OK(esp_efuse_set_digest_revoke(2));
    TEST_ASSERT_TRUE(esp_efuse_get_digest_revoke(2));

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_set_digest_revoke(3));

    TEST_ESP_OK(esp_efuse_set_write_protect_of_digest_revoke(0));
    TEST_ASSERT_TRUE(esp_efuse_get_write_protect_of_digest_revoke(0));
    TEST_ESP_OK(esp_efuse_set_write_protect_of_digest_revoke(1));
    TEST_ASSERT_TRUE(esp_efuse_get_write_protect_of_digest_revoke(1));
    TEST_ESP_OK(esp_efuse_set_write_protect_of_digest_revoke(2));
    TEST_ASSERT_TRUE(esp_efuse_get_write_protect_of_digest_revoke(2));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_efuse_set_write_protect_of_digest_revoke(3));
}

TEST_CASE("Test set_write_protect_of_digest_revoke", "[efuse]")
{
    esp_efuse_utility_reset();
#ifdef CONFIG_EFUSE_FPGA_TEST
    esp_efuse_utility_update_virt_blocks();
#else
    esp_efuse_utility_erase_virt_blocks();
#endif
    esp_efuse_utility_debug_dump_blocks();

    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(0));
    TEST_ASSERT_FALSE(esp_efuse_get_write_protect_of_digest_revoke(0));
    TEST_ESP_OK(esp_efuse_set_write_protect_of_digest_revoke(0));
    TEST_ASSERT_TRUE(esp_efuse_get_write_protect_of_digest_revoke(0));

    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(1));
    TEST_ASSERT_FALSE(esp_efuse_get_write_protect_of_digest_revoke(1));
    TEST_ESP_OK(esp_efuse_set_write_protect_of_digest_revoke(1));
    TEST_ASSERT_TRUE(esp_efuse_get_write_protect_of_digest_revoke(1));

    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(2));
    TEST_ASSERT_FALSE(esp_efuse_get_write_protect_of_digest_revoke(2));
    TEST_ESP_OK(esp_efuse_set_write_protect_of_digest_revoke(2));
    TEST_ASSERT_TRUE(esp_efuse_get_write_protect_of_digest_revoke(2));

#if CONFIG_EFUSE_VIRTUAL
    TEST_ESP_OK(esp_efuse_set_digest_revoke(0));
    TEST_ESP_OK(esp_efuse_set_digest_revoke(1));
    TEST_ESP_OK(esp_efuse_set_digest_revoke(2));

    TEST_ASSERT_TRUE(esp_efuse_get_digest_revoke(0));
    TEST_ASSERT_TRUE(esp_efuse_get_digest_revoke(1));
    TEST_ASSERT_TRUE(esp_efuse_get_digest_revoke(2));
#endif // CONFIG_EFUSE_VIRTUAL

#if CONFIG_EFUSE_FPGA_TEST && !CONFIG_EFUSE_VIRTUAL
    TEST_ESP_ERR(ESP_FAIL, esp_efuse_set_digest_revoke(0));
    TEST_ESP_ERR(ESP_FAIL, esp_efuse_set_digest_revoke(1));
    TEST_ESP_ERR(ESP_FAIL, esp_efuse_set_digest_revoke(2));
    // the write protection bits are set and the revocation bits will not be changed.
    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(0));
    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(1));
    TEST_ASSERT_FALSE(esp_efuse_get_digest_revoke(2));
#endif // CONFIG_EFUSE_FPGA_TEST && !CONFIG_EFUSE_VIRTUAL
}
#endif // SOC_SUPPORT_SECURE_BOOT_REVOKE_KEY

#endif // CONFIG_EFUSE_VIRTUAL || CONFIG_EFUSE_FPGA_TEST
