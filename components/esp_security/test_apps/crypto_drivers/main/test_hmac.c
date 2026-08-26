/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_log.h"
#include "esp_hmac.h"
#include "hmac_test_cases.h"

#if CONFIG_ESP_SECURITY_ENABLE_FPGA_TESTS

static void setup_keyblock(esp_efuse_block_t key_block, esp_efuse_purpose_t purpose)
{
    esp_err_t status = esp_efuse_write_key(key_block, purpose, key_data, sizeof(key_data));

    if (status == ESP_OK) {
        printf("Written key!\n");
    } else if (ESP_ERR_EFUSE_REPEATED_PROG) {
        printf("Key written already.\n");
    } else {
        printf("ERROR while writing key.\n");
    }
}

TEST_CASE("HMAC 'downstream' JTAG Enable mode", "[hw_crypto]")
{
    int ets_status;

    setup_keyblock(EFUSE_BLK_KEY4, ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG);

    TEST_ASSERT_MESSAGE(ESP_OK == esp_efuse_batch_write_begin(),
                        "Error programming security efuse.\n");

    ets_status = esp_efuse_write_field_cnt(ESP_EFUSE_SOFT_DIS_JTAG, ESP_EFUSE_SOFT_DIS_JTAG[0]->bit_count);

    TEST_ASSERT_MESSAGE(ets_status == ESP_OK || ets_status == ESP_ERR_EFUSE_CNT_IS_FULL,
                        "JTAG Disable temporarily failed.\n");

    TEST_ASSERT_MESSAGE(ESP_OK == esp_efuse_batch_write_commit(),
                        "Error programming security efuse.\n");

    TEST_ASSERT_EQUAL_HEX32_MESSAGE(ESP_OK, esp_hmac_jtag_enable(HMAC_KEY4, token_data),
                                    "JTAG should be re-enabled now, please manually verify");
}

TEST_CASE("HMAC 'downstream' JTAG Disable", "[hw_crypto]")
{
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(ESP_OK, esp_hmac_jtag_disable(),
                                    "JTAG should be disabled now, please manually verify");
}

TEST_CASE("HMAC 'upstream' MAC generation with zeroes", "[hw_crypto]")
{
    uint8_t hmac[32];

    setup_keyblock(EFUSE_BLK_KEY5, ESP_EFUSE_KEY_PURPOSE_HMAC_UP);

    const uint8_t zeroes[128] = { };

    const size_t num_zero_results = sizeof(zero_results) / sizeof(hmac_result);

    for (int i = 0; i < num_zero_results; i++) {
        TEST_ESP_OK(esp_hmac_calculate(HMAC_KEY5, zeroes, zero_results[i].msglen, hmac));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(zero_results[i].result, hmac, sizeof(hmac));
    }
}

TEST_CASE("HMAC 'upstream' MAC generation from data", "[hw_crypto]")
{
    uint8_t hmac[32];

    setup_keyblock(EFUSE_BLK_KEY5, ESP_EFUSE_KEY_PURPOSE_HMAC_UP);

    for (int i = 0; i < sizeof(results) / sizeof(hmac_result); i++) {
        TEST_ESP_OK(esp_hmac_calculate(HMAC_KEY5, message, results[i].msglen, hmac));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(results[i].result, hmac, sizeof(hmac));
    }
}

TEST_CASE("HMAC 'upstream' wait lock", "[hw_crypto]")
{
    uint8_t hmac[32];

    setup_keyblock(EFUSE_BLK_KEY5, ESP_EFUSE_KEY_PURPOSE_HMAC_UP);

    for (int i = 0; i < sizeof(results) / sizeof(hmac_result); i++) {
        TEST_ESP_OK(esp_hmac_calculate(HMAC_KEY5, message, results[i].msglen, hmac));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(results[i].result, hmac, sizeof(hmac));
    }
}

#endif // CONFIG_ESP_SECURITY_ENABLE_FPGA_TESTS

/**
 * This test is just a parameter test and does not write any keys to efuse.
 * It can be done safely on any chip which supports HMAC.
 */
TEST_CASE("HMAC key out of range", "[hw_crypto]")
{
    uint8_t hmac[32];

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_hmac_calculate(HMAC_KEY0 - 1, message, 47, hmac));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, esp_hmac_calculate(HMAC_KEY_MAX, message, 47, hmac));
}
