/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_crypto_periph_clk.h"
#include "esp_log.h"
#include "memory_checks.h"
#include "unity_fixture.h"

#include "rom/efuse.h"
#include "rom/hmac.h"
#include "rom/ets_sys.h"
#include "soc/hwcrypto_reg.h"
#include "soc/system_reg.h"
#include "hmac_params.h"
#include "hal_crypto_common.h"

static ets_efuse_block_t convert_key_type(uint32_t key_id)
{
    return ETS_EFUSE_BLOCK_KEY0 + (ets_efuse_block_t) key_id;
}

static esp_err_t hmac_jtag_disable(void)
{
    REG_WRITE(HMAC_SET_INVALIDATE_JTAG_REG, 1);
    return ESP_OK;
}

#if !CONFIG_IDF_TARGET_ESP32S2

#include "hal/hmac_hal.h"
#include "hal/hmac_ll.h"
#include "hal/ds_ll.h"
#include "hal/sha_ll.h"
#include "esp_private/periph_ctrl.h"

#define SHA256_BLOCK_SZ 64
#define SHA256_PAD_SZ 8

static esp_err_t hmac_jtag_enable(uint32_t key_id, const uint8_t *token)
{
    int ets_status;
    esp_err_t err = ESP_OK;
    ets_status = ets_jtag_enable_temporarily(token, convert_key_type(key_id));

    if (ets_status != ETS_OK) {
        err = ESP_FAIL;
    }

    ets_hmac_disable();
    return err;
}

static void write_and_padd(uint8_t *block, const uint8_t *data, uint16_t data_len)
{
    memcpy(block, data, data_len);
    block[data_len] = 0x80;
    bzero(block + data_len + 1, SHA256_BLOCK_SZ - data_len - 1);
}

esp_err_t hmac_calculate(uint32_t key_id, const void *message, size_t message_len, uint8_t *hmac)
{
    const uint8_t *message_bytes = (const uint8_t *)message;

    esp_crypto_hmac_enable_periph_clk(true);
    esp_crypto_sha_enable_periph_clk(true);
    esp_crypto_ds_enable_periph_clk(true);

    hmac_hal_start();

    uint32_t conf_error = hmac_hal_configure(HMAC_OUTPUT_USER, key_id);
    if (conf_error) {
        return ESP_FAIL;
    }

    if (message_len + 1 + SHA256_PAD_SZ <= SHA256_BLOCK_SZ) {
        uint8_t block[SHA256_BLOCK_SZ];
        uint64_t bit_len = __builtin_bswap64(message_len * 8 + 512);

        write_and_padd(block, message_bytes, message_len);

        memcpy(block + SHA256_BLOCK_SZ - sizeof(bit_len),
               &bit_len, sizeof(bit_len));
        hmac_hal_write_one_block_512(block);
    } else {
        size_t remaining_blocks = message_len / SHA256_BLOCK_SZ;
        for (int i = 1; i < remaining_blocks; i++) {
            hmac_hal_write_block_512(message_bytes);
            message_bytes += SHA256_BLOCK_SZ;
            hmac_hal_next_block_normal();
        }

        if (remaining_blocks) {
            hmac_hal_write_block_512(message_bytes);
            message_bytes += SHA256_BLOCK_SZ;
        }

        size_t remaining = message_len % SHA256_BLOCK_SZ;

        uint8_t block[SHA256_BLOCK_SZ];
        uint64_t bit_len = __builtin_bswap64(message_len * 8 + 512);

        if (remaining >= SHA256_BLOCK_SZ - SHA256_PAD_SZ) {
            write_and_padd(block, message_bytes, remaining);
            hmac_hal_next_block_normal();
            hmac_hal_write_block_512(block);
            bzero(block, SHA256_BLOCK_SZ);
        } else {
            write_and_padd(block, message_bytes, remaining);
        }
        memcpy(block + SHA256_BLOCK_SZ - sizeof(bit_len),
               &bit_len, sizeof(bit_len));
        hmac_hal_next_block_padding();
        hmac_hal_write_block_512(block);
    }

    hmac_hal_read_result_256(hmac);

    esp_crypto_hmac_enable_periph_clk(false);
    esp_crypto_sha_enable_periph_clk(false);
    esp_crypto_ds_enable_periph_clk(false);

    return ESP_OK;
}

#else /* !CONFIG_IDF_TARGET_ESP32S2 */

static esp_err_t hmac_calculate(uint32_t key_id,
                                const void *message,
                                size_t message_len,
                                uint8_t *hmac)
{
    int hmac_ret;
    ets_hmac_enable();
    hmac_ret = ets_hmac_calculate_message(convert_key_type(key_id), message, message_len, hmac);
    ets_hmac_disable();

    if (hmac_ret != 0) {
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }
}

static esp_err_t hmac_jtag_enable(uint32_t key_id, const uint8_t *token)
{
    int ets_status;
    esp_err_t err = ESP_OK;

    ets_hmac_enable();

    /* Token updating into HMAC module. */
    for (int i = 0; i < 32; i += 4) {
        uint32_t key_word;
        memcpy(&key_word, &token[i], 4);
        REG_WRITE(DPORT_JTAG_CTRL_0_REG + i, __builtin_bswap32(key_word));
    }

    ets_status = ets_hmac_calculate_downstream(convert_key_type(key_id), ETS_EFUSE_KEY_PURPOSE_HMAC_DOWN_JTAG);

    if (ets_status != ETS_OK) {
        err = ESP_FAIL;
    }

    ets_hmac_disable();
    return err;
}

#endif /* !CONFIG_IDF_TARGET_ESP32S2 */

TEST_GROUP(hmac);

TEST_SETUP(hmac)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(hmac)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(hmac, hmac_downstream_jtag_enable_mode)
{
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(ESP_OK, hmac_jtag_enable(HMAC_KEY_BLOCK_1, jtag_enable_token_data),
                                    "JTAG should be re-enabled now, please manually verify");
}

TEST(hmac, hmac_downstream_jtag_disable)
{
    TEST_ASSERT_EQUAL_HEX32_MESSAGE(ESP_OK, hmac_jtag_disable(), "JTAG should be disabled now, please manually verify");
}

TEST(hmac, hmac_upstream_mac_generation_with_zeroes)
{
    uint8_t hmac[32];
    const size_t num_zero_results = sizeof(zero_results) / sizeof(hmac_result);

    for (int i = 0; i < num_zero_results; i++) {
        TEST_ESP_OK(hmac_calculate(HMAC_KEY_BLOCK_2, zeroes, zero_results[i].msglen, hmac));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(zero_results[i].result, hmac, sizeof(hmac));
    }
}

TEST(hmac, hmac_upstream_MAC_generation_from_data)
{
    uint8_t hmac[32];

    for (int i = 0; i < sizeof(results) / sizeof(hmac_result); i++) {
        TEST_ESP_OK(hmac_calculate(HMAC_KEY_BLOCK_2, message, results[i].msglen, hmac));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(results[i].result, hmac, sizeof(hmac));
    }
}

TEST_GROUP_RUNNER(hmac)
{
    RUN_TEST_CASE(hmac, hmac_downstream_jtag_enable_mode);
    RUN_TEST_CASE(hmac, hmac_downstream_jtag_disable);
    RUN_TEST_CASE(hmac, hmac_upstream_mac_generation_with_zeroes);
    RUN_TEST_CASE(hmac, hmac_upstream_MAC_generation_from_data);
}
