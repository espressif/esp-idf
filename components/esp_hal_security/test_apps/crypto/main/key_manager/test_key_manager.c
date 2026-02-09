/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "esp_efuse_chip.h"
#include "esp_heap_caps.h"
#include "esp_rom_crc.h"
#include "hal/key_mgr_hal.h"
#include "hal/key_mgr_ll.h"
#include "hal/key_mgr_types.h"
#include "hal/huk_types.h"
#include "hal/huk_hal.h"
#include "esp_key_mgr.h"
#include "memory_checks.h"
#include "unity_fixture.h"
#include "hal_crypto_common.h"
#include "rom/key_mgr.h"
#include "esp_partition.h"
#include "esp_flash.h"
#include "key_manager_test_cases.h"
#include "esp_log.h"

#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
#include "hal/ecdsa_types.h"
#endif
#if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
#include "hal/hmac_types.h"
#endif
#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
#include "ds/ds_types.h"
#endif

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY
const esp_partition_t *get_test_storage_partition(void)
{
    /* This finds "storage" partition defined partition table */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                             ESP_PARTITION_SUBTYPE_ANY, "storage");
    if (!result) {
        /* means partition table set wrong */
        ESP_LOGE("", "ERROR in obtaining storage partition");
        return NULL;
    }
    return result;
}

static void test_xts_aes_key_aes_mode(test_data_aes_mode_t *test_data)
{
    const esp_partition_t *partition = get_test_storage_partition();
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

    uint8_t read_data[128];
    for (int i = 0; i < TEST_COUNT; i++) {
        memset(read_data, 0, sizeof(read_data));
        uint32_t address = test_data->xts_test_data[i].data_offset;
        uint32_t data_size = test_data->xts_test_data[i].data_size;

        ESP_ERROR_CHECK(esp_flash_write_encrypted(NULL, address, test_data->plaintext_data, data_size));
        ESP_ERROR_CHECK(esp_flash_read(NULL, read_data, address, data_size));

        TEST_ASSERT_EQUAL_HEX8_ARRAY(test_data->xts_test_data[i].ciphertext, read_data, data_size);
    }
}

static void test_xts_aes_key_ecdh0_mode(test_data_ecdh0_mode_t *test_data)
{
    const esp_partition_t *partition = get_test_storage_partition();
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

    uint8_t read_data[128] = { 0 };
    uint32_t address = partition->address;
    uint32_t data_size = 32;

    ESP_LOG_BUFFER_HEXDUMP("Plaintext data", test_data->plaintext_data, data_size, ESP_LOG_DEBUG);

    ESP_ERROR_CHECK(esp_flash_write_encrypted(NULL, address, test_data->plaintext_data, data_size));
    ESP_ERROR_CHECK(esp_flash_read(NULL, read_data, address, data_size));

    ESP_LOG_BUFFER_HEXDUMP("Encrypted data", read_data, data_size, ESP_LOG_DEBUG);
}

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 || SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
static void key_mgr_test_xts_aes_key_aes_mode(esp_key_mgr_key_len_t key_len, test_data_aes_mode_t *test_data)
{
    static esp_key_mgr_aes_key_config_t key_config;
    memcpy(key_config.k2_info, (uint8_t*) test_data->k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config.k1_encrypted[0], (uint8_t*) test_data->k1_encrypted[0], KEY_MGR_K1_ENCRYPTED_SIZE);

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
    if (key_len == ESP_KEY_MGR_XTS_AES_LEN_256) {
        memcpy(key_config.k1_encrypted[1], (uint8_t*) test_data->k1_encrypted[1], KEY_MGR_K1_ENCRYPTED_SIZE);
    }
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */

    memcpy(key_config.sw_init_key, (uint8_t*) test_data->init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config.use_pre_generated_sw_init_key = 1;
    key_config.key_type = ESP_KEY_MGR_FLASH_XTS_AES_KEY;
    key_config.key_len = key_len;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(&key_config, &key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_aes_mode(test_data);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

static void key_mgr_test_xts_aes_key_ecdh0_mode(esp_key_mgr_key_len_t key_len)
{
    static esp_key_mgr_ecdh0_key_config_t key_config;
    memcpy(key_config.k1_G[0], (uint8_t*) test_data_ecdh0.k1_G[0], KEY_MGR_ECDH0_INFO_SIZE);

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
    if (key_len == ESP_KEY_MGR_XTS_AES_LEN_256) {
        memcpy(key_config.k1_G[1], (uint8_t*) test_data_ecdh0.k1_G[1], KEY_MGR_ECDH0_INFO_SIZE);
    }
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */

    key_config.key_type = ESP_KEY_MGR_FLASH_XTS_AES_KEY;
    key_config.key_len = key_len;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    static esp_key_mgr_ecdh0_info_t ecdh0_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(&key_config, &key_recovery_info, &ecdh0_info));

    ESP_LOG_BUFFER_HEXDUMP("K2_G_0", ecdh0_info.k2_G[0], KEY_MGR_ECDH0_INFO_SIZE, ESP_LOG_DEBUG);
#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
    if (key_len == ESP_KEY_MGR_XTS_AES_LEN_256) {
        ESP_LOG_BUFFER_HEXDUMP("K2_G_1", ecdh0_info.k2_G[1], KEY_MGR_ECDH0_INFO_SIZE, ESP_LOG_DEBUG);
    }
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_ecdh0_mode(&test_data_ecdh0);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 || SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128
static void key_mgr_test_xts_aes_128_aes_mode(void)
{
    key_mgr_test_xts_aes_key_aes_mode(ESP_KEY_MGR_XTS_AES_LEN_128, &test_data_xts_aes_128);
}

static void key_mgr_test_xts_aes_128_ecdh0_mode(void)
{
    key_mgr_test_xts_aes_key_ecdh0_mode(ESP_KEY_MGR_XTS_AES_LEN_128);
}
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 */

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
static void key_mgr_test_xts_aes_256_aes_mode(void)
{
    key_mgr_test_xts_aes_key_aes_mode(ESP_KEY_MGR_XTS_AES_LEN_256, &test_data_xts_aes_256);
}

static void key_mgr_test_xts_aes_256_ecdh0_mode(void)
{
    key_mgr_test_xts_aes_key_ecdh0_mode(ESP_KEY_MGR_XTS_AES_LEN_256);
}
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */

#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS
#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 || SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
static void test_xts_aes_key_random_mode(void)
{
    const esp_partition_t *partition = get_test_storage_partition();
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
    uint8_t plaintext_data[1024] = {[0 ... 1023] = 0xBE};
    const int write_size = 16;
    for (int i = 0; i < sizeof(plaintext_data) / write_size; i++) {
        ESP_LOGI("", " i = %d", i);
        ESP_ERROR_CHECK(esp_flash_write_encrypted(NULL, partition->address + (i * write_size), plaintext_data, write_size));
        static uint8_t read_data[128];
        ESP_ERROR_CHECK(esp_partition_read(partition, write_size * i, read_data, write_size));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext_data + (i * write_size), read_data, write_size);
    }
}

static void key_mgr_test_xts_aes_key_random_mode(esp_key_mgr_key_len_t key_len)
{
    static esp_key_mgr_random_key_config_t key_config;
    key_config.key_type = ESP_KEY_MGR_FLASH_XTS_AES_KEY;
    key_config.key_len = key_len;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(&key_config, &key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_random_mode();
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 || SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128
static void key_mgr_test_xts_aes_128_random_mode(void)
{
    key_mgr_test_xts_aes_key_random_mode(ESP_KEY_MGR_XTS_AES_LEN_128);
}
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 */

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
static void key_mgr_test_xts_aes_256_random_mode(void)
{
    key_mgr_test_xts_aes_key_random_mode(ESP_KEY_MGR_XTS_AES_LEN_256);
}
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */
#endif /* CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS */
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY */

#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
#if SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
extern void test_ecdsa_export_pubkey(ecdsa_curve_t curve, uint8_t *ecdsa_pub_x, uint8_t *ecdsa_pub_y, bool use_km_key);
extern void test_ecdsa_export_pubkey_inner(ecdsa_curve_t curve, uint8_t *exported_pub_x, uint8_t *exported_pub_y, bool use_km_key, uint16_t *len);
#endif

extern void test_ecdsa_sign(ecdsa_curve_t curve, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, bool use_km_key, ecdsa_sign_type_t k_type);
extern int test_ecdsa_verify(ecdsa_curve_t curve, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, uint8_t *pub_x, uint8_t *pub_y);
extern void test_ecdsa_sign_and_verify(ecdsa_curve_t curve, uint8_t* sha, uint8_t* pub_x, uint8_t* pub_y, bool use_km_key, ecdsa_sign_type_t k_type);

/*
const uint8_t message[32] = { 0xDF, 0xDE, 0xD7, 0x4A, 0x47, 0xB1, 0x4F, 0x73, 0x00, 0x21, 0x62, 0xC7, 0x66, 0x6D, 0xA3, 0x95, 0x66, 0x19, 0x62, 0x7F, 0x71, 0x7B, 0x3C, 0x66, 0x82, 0xD3, 0x9F, 0x71, 0xAC, 0x9C, 0xC3, 0x39 };
*/
/* sha384 digest of the above message */
uint8_t sha_digest[48] = { 0xF0, 0x94, 0xC4, 0x4A, 0xF0, 0xEE, 0x68, 0xDB, 0x5B, 0x6A, 0x12, 0x84, 0xAC, 0xAF, 0x49, 0x0C, 0x24, 0xED, 0x70, 0x41, 0xE6, 0xE3, 0xBD, 0x74, 0x2B, 0x8D, 0xCF, 0x46, 0x19, 0xE1, 0xC2, 0x61, 0xCA, 0x79, 0xF3, 0x86, 0xF9, 0x04, 0xC0, 0x63, 0xC6, 0xF0, 0xEE, 0x36, 0x7C, 0x5C, 0x82, 0x89 };

void test_ecdsa_key_aes_mode(ecdsa_curve_t curve, uint8_t *sha_digest, uint8_t *pub_x, uint8_t *pub_y, ecdsa_sign_type_t k_type)
{
    test_ecdsa_sign_and_verify(curve, sha_digest, pub_x, pub_y, 1, k_type);
#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    test_ecdsa_export_pubkey(curve, pub_x, pub_y, 1);
#endif
}

void key_mgr_test_ecdsa_key(esp_key_mgr_key_len_t key_len, ecdsa_sign_type_t k_type)
{
    uint8_t pub_x[48] = {};
    uint8_t pub_y[48] = {};
    uint8_t r_le[48] = {0};
    uint8_t s_le[48] = {0};

    uint16_t sha_digest_len = 0;

    ecdsa_curve_t curve = ECDSA_CURVE_SECP192R1;

    switch (key_len) {
    case ESP_KEY_MGR_ECDSA_LEN_192:
        sha_digest_len = 24;
        curve = ECDSA_CURVE_SECP192R1;
        break;
    case ESP_KEY_MGR_ECDSA_LEN_256:
        sha_digest_len = 32;
        curve = ECDSA_CURVE_SECP256R1;
        break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
    case ESP_KEY_MGR_ECDSA_LEN_384:
        sha_digest_len = 48;
        curve = ECDSA_CURVE_SECP384R1;
        break;
#endif
    default:
        TEST_FAIL_MESSAGE("Unsupported key length");
        return;
    }

    test_ecdsa_sign(curve, sha_digest, r_le, s_le, 1, k_type);

    ESP_LOG_BUFFER_HEXDUMP("ECDSA message digest", sha_digest, sha_digest_len, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("ECDSA signature r_le", r_le, sizeof(r_le), ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("ECDSA signature s_le", s_le, sizeof(s_le), ESP_LOG_DEBUG);

    // Export the pubkey from ECDSA peripheral
    uint16_t pubkey_len = 0;
    test_ecdsa_export_pubkey_inner(curve, pub_x, pub_y, 1, &pubkey_len);

    ESP_LOG_BUFFER_HEXDUMP("ECDSA key pubx", pub_x, pubkey_len, ESP_LOG_DEBUG);
    ESP_LOG_BUFFER_HEXDUMP("ECDSA key puby", pub_y, pubkey_len, ESP_LOG_DEBUG);
    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(curve, sha_digest, r_le, s_le, pub_x, pub_y));
}

/* Generic ECDSA AES mode test function */
static void key_mgr_test_ecdsa_key_aes_mode(esp_key_mgr_key_len_t key_len, test_data_aes_mode_t *test_data)
{
    static esp_key_mgr_aes_key_config_t key_config;
    ecdsa_curve_t curve = ECDSA_CURVE_SECP192R1;
    uint8_t *pub_x = NULL;
    uint8_t *pub_y = NULL;

    memcpy(key_config.k2_info, (uint8_t*) test_data->k2_info, KEY_MGR_K2_INFO_SIZE);

    if (key_len == ESP_KEY_MGR_ECDSA_LEN_192) {
        memcpy(key_config.k1_encrypted[0], (uint8_t*) test_data->k1_encrypted[0], KEY_MGR_K1_ENCRYPTED_SIZE);
        pub_x = test_data->ecdsa_test_data.ecdsa_p192_pubx;
        pub_y = test_data->ecdsa_test_data.ecdsa_p192_puby;
        curve = ECDSA_CURVE_SECP192R1;
    } else if (key_len == ESP_KEY_MGR_ECDSA_LEN_256) {
        memcpy(key_config.k1_encrypted[0], (uint8_t*) test_data->k1_encrypted[1], KEY_MGR_K1_ENCRYPTED_SIZE);
        pub_x = test_data->ecdsa_test_data.ecdsa_p256_pubx;
        pub_y = test_data->ecdsa_test_data.ecdsa_p256_puby;
        curve = ECDSA_CURVE_SECP256R1;
    }
#if SOC_ECDSA_SUPPORT_CURVE_P384
    else if (key_len == ESP_KEY_MGR_ECDSA_LEN_384) {
        memcpy(key_config.k1_encrypted[0], (uint8_t*) test_data->k1_encrypted[2], KEY_MGR_K1_ENCRYPTED_SIZE);
        memcpy(key_config.k1_encrypted[1], (uint8_t*) test_data->k1_encrypted[3], KEY_MGR_K1_ENCRYPTED_SIZE);
        pub_x = test_data->ecdsa_test_data.ecdsa_p384_pubx;
        pub_y = test_data->ecdsa_test_data.ecdsa_p384_puby;
        curve = ECDSA_CURVE_SECP384R1;
    }
#endif
    memcpy(key_config.sw_init_key, (uint8_t*) test_data->init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config.use_pre_generated_sw_init_key = 1;
    key_config.key_type = ESP_KEY_MGR_ECDSA_KEY;
    key_config.key_len = key_len;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(&key_config, &key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));

#if SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    test_ecdsa_key_aes_mode(curve, sha_digest, pub_x, pub_y, ECDSA_K_TYPE_DETERMINISITIC);
#endif
    test_ecdsa_key_aes_mode(curve, sha_digest, pub_x, pub_y, ECDSA_K_TYPE_TRNG);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

/* Generic ECDSA ECDH0 mode test function */
static void key_mgr_test_ecdsa_key_ecdh0_mode(esp_key_mgr_key_len_t key_len)
{
    static esp_key_mgr_ecdh0_key_config_t key_config;
    memcpy(key_config.k1_G[0], (uint8_t*) test_data_ecdh0.k1_G[0], KEY_MGR_ECDH0_INFO_SIZE);
#if SOC_ECDSA_SUPPORT_CURVE_P384
    // For 384-bit keys, copy the second k1_G block
    if (key_len == ESP_KEY_MGR_ECDSA_LEN_384) {
        memcpy(key_config.k1_G[1], (uint8_t*) test_data_ecdh0.k1_G[1], KEY_MGR_ECDH0_INFO_SIZE);
    }
#endif
    key_config.key_type = ESP_KEY_MGR_ECDSA_KEY;
    key_config.key_len = key_len;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    static esp_key_mgr_ecdh0_info_t ecdh0_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(&key_config, &key_recovery_info, &ecdh0_info));

    ESP_LOG_BUFFER_HEXDUMP("K2_G_0", ecdh0_info.k2_G[0], KEY_MGR_ECDH0_INFO_SIZE, ESP_LOG_DEBUG);
#if SOC_ECDSA_SUPPORT_CURVE_P384
    if (key_len == ESP_KEY_MGR_ECDSA_LEN_384) {
        ESP_LOG_BUFFER_HEXDUMP("K2_G_1", ecdh0_info.k2_G[1], KEY_MGR_ECDH0_INFO_SIZE, ESP_LOG_DEBUG);
    }
#endif
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));

#if SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    key_mgr_test_ecdsa_key(key_len, ECDSA_K_TYPE_DETERMINISITIC);
#endif
    key_mgr_test_ecdsa_key(key_len, ECDSA_K_TYPE_TRNG);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

/* Generic ECDSA random mode test function */
static void key_mgr_test_ecdsa_key_random_mode(esp_key_mgr_key_len_t key_len)
{
    static esp_key_mgr_random_key_config_t key_config;
    key_config.key_type = ESP_KEY_MGR_ECDSA_KEY;
    key_config.key_len = key_len;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(&key_config, &key_recovery_info));

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));

    if (key_len == ESP_KEY_MGR_ECDSA_LEN_256) {
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
        key_mgr_test_ecdsa_key(ECDSA_CURVE_SECP256R1, ECDSA_K_TYPE_DETERMINISITIC);
#endif
        key_mgr_test_ecdsa_key(ECDSA_CURVE_SECP256R1, ECDSA_K_TYPE_TRNG);
    }
#if SOC_ECDSA_SUPPORT_CURVE_P384
    else if (key_len == ESP_KEY_MGR_ECDSA_LEN_384) {
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
        key_mgr_test_ecdsa_key(ECDSA_CURVE_SECP384R1, ECDSA_K_TYPE_DETERMINISITIC);
#endif
        key_mgr_test_ecdsa_key(ECDSA_CURVE_SECP384R1, ECDSA_K_TYPE_TRNG);
    }
#endif
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}
#endif /* SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY */

#if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
extern esp_err_t hmac_calculate(uint32_t key_id, const void *message, size_t message_len, uint8_t *hmac);

static void key_mgr_test_hmac_key_aes_mode(test_data_aes_mode_t *test_data)
{
    uint8_t hmac[32] = {0};
    TEST_ASSERT_EQUAL(ESP_OK, hmac_calculate(HMAC_KEY_KM, test_data->hmac_test_data.message, sizeof(test_data->hmac_test_data.message), hmac));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test_data->hmac_test_data.hmac_result, hmac, sizeof(test_data->hmac_test_data.hmac_result));
}

static void key_mgr_test_hmac_key_ecdh0_mode(const uint8_t *message, size_t message_len)
{
    uint8_t hmac[32] = {0};
    TEST_ASSERT_EQUAL(ESP_OK, hmac_calculate(HMAC_KEY_KM, message, message_len, hmac));
    // We cannot verify the result here as the HMAC key deployed is unknown.
}

static void key_mgr_test_hmac_key_aes_random_mode(const uint8_t *message, size_t message_len)
{
    uint8_t hmac[32] = {0};
    TEST_ASSERT_EQUAL(ESP_OK, hmac_calculate(HMAC_KEY_KM, message, message_len, hmac));
    // We cannot verify the result here as the HMAC key deployed is unknown.
}

static void key_mgr_test_hmac_aes_mode(void)
{
    static esp_key_mgr_aes_key_config_t key_config;
    memcpy(key_config.k2_info, (uint8_t*) test_data_hmac.k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config.k1_encrypted, (uint8_t*) test_data_hmac.k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config.sw_init_key, (uint8_t*) test_data_hmac.init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config.use_pre_generated_sw_init_key = 1;
    key_config.key_type = ESP_KEY_MGR_HMAC_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(&key_config, &key_recovery_info));
    ESP_LOG_BUFFER_HEXDUMP("key_info", key_recovery_info.key_info, sizeof(esp_key_mgr_key_info_t), ESP_LOG_DEBUG);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    key_mgr_test_hmac_key_aes_mode(&test_data_hmac);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

static void key_mgr_test_hmac_ecdh0_mode(void)
{
    static esp_key_mgr_ecdh0_key_config_t key_config;
    memcpy(key_config.k1_G[0], (uint8_t*) test_data_ecdh0.k1_G[0], KEY_MGR_ECDH0_INFO_SIZE);
    key_config.key_type = ESP_KEY_MGR_HMAC_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    static esp_key_mgr_ecdh0_info_t ecdh0_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(&key_config, &key_recovery_info, &ecdh0_info));

    ESP_LOG_BUFFER_HEXDUMP("K2_G", ecdh0_info.k2_G[0], KEY_MGR_ECDH0_INFO_SIZE, ESP_LOG_DEBUG);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    key_mgr_test_hmac_key_ecdh0_mode(test_data_hmac.hmac_test_data.message, sizeof(test_data_hmac.hmac_test_data.message));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

static void key_mgr_test_hmac_random_mode(void)
{
    static esp_key_mgr_random_key_config_t key_config;
    key_config.key_type = ESP_KEY_MGR_HMAC_KEY;
    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(&key_config, &key_recovery_info));

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    key_mgr_test_hmac_key_aes_random_mode(test_data_hmac.hmac_test_data.message, sizeof(test_data_hmac.hmac_test_data.message));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}
#endif /* SOC_KEY_MANAGER_HMAC_KEY_DEPLOY */

#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
extern esp_err_t esp_ds_sign(const void *message,
                             const esp_ds_data_t *data,
                             uint32_t key_id,
                             void *signature);

static void key_mgr_test_ds_key_aes_mode(test_data_aes_mode_t *test_data)
{
    esp_ds_data_t esp_ds_data = {0};
    esp_ds_data.rsa_length = test_data->ds_test_data.ds_key_size / 32 - 1;
    memcpy(esp_ds_data.iv, test_data->ds_test_data.ds_encrypted_input_params_iv, sizeof(esp_ds_data.iv));
    memcpy(esp_ds_data.c, test_data->ds_test_data.ds_encrypted_input_params, sizeof(esp_ds_data.c));

    uint8_t signature[4096 / 8] = {0}; // Max possible RSA signature size

    esp_err_t ds_r = esp_ds_sign(test_data->ds_test_data.ds_message,
                                 &esp_ds_data,
                                 HMAC_KEY_KM,
                                 signature);

    TEST_ASSERT_EQUAL(ESP_OK, ds_r);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test_data->ds_test_data.ds_result, signature, sizeof(test_data->ds_test_data.ds_result));
}

static void key_mgr_test_ds_aes_mode(void)
{
    static esp_key_mgr_aes_key_config_t key_config;
    memcpy(key_config.k2_info, (uint8_t*) test_data_ds.k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config.k1_encrypted, (uint8_t*) test_data_ds.k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config.sw_init_key, (uint8_t*) test_data_ds.init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config.use_pre_generated_sw_init_key = 1;
    key_config.key_type = ESP_KEY_MGR_DS_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(&key_config, &key_recovery_info));

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    key_mgr_test_ds_key_aes_mode(&test_data_ds);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}
#endif /* SOC_KEY_MANAGER_DS_KEY_DEPLOY */

TEST_GROUP(key_manager);

TEST_SETUP(key_manager)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(800, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(key_manager)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY
#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128
TEST(key_manager, xts_aes_128_key_aes_deployment)
{
    key_mgr_test_xts_aes_128_aes_mode();
}

TEST(key_manager, xts_key_128_ecdh0_deployment)
{
    key_mgr_test_xts_aes_128_ecdh0_mode();
}

#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS
TEST(key_manager, xts_key_128_random_deployment)
{
    if (efuse_hal_flash_encryption_enabled()) {
        key_mgr_test_xts_aes_128_random_mode();
    } else {
        ESP_LOGI("", "Flash encryption is not enabled, skipping test");
    }
}
#endif /* CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS */
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 */

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
TEST(key_manager, xts_aes_256_key_aes_deployment)
{
    key_mgr_test_xts_aes_256_aes_mode();
}

TEST(key_manager, xts_key_256_ecdh0_deployment)
{
    key_mgr_test_xts_aes_256_ecdh0_mode();
}

#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS
TEST(key_manager, xts_key_256_random_deployment)
{
    if (efuse_hal_flash_encryption_enabled()) {
        key_mgr_test_xts_aes_256_random_mode();
    } else {
        ESP_LOGI("", "Flash encryption is not enabled, skipping test");
    }
}
#endif /* CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS */
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY */

#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
TEST(key_manager, ecdsa_p192_key_aes_deployment)
{
    key_mgr_test_ecdsa_key_aes_mode(ESP_KEY_MGR_ECDSA_LEN_192, &test_data_ecdsa);
}

TEST(key_manager, ecdsa_p192_key_ecdh0_deployment)
{
    key_mgr_test_ecdsa_key_ecdh0_mode(ESP_KEY_MGR_ECDSA_LEN_192);
}

TEST(key_manager, ecdsa_p192_key_random_deployment)
{
    key_mgr_test_ecdsa_key_random_mode(ESP_KEY_MGR_ECDSA_LEN_192);
}

TEST(key_manager, ecdsa_p256_key_aes_deployment)
{
    key_mgr_test_ecdsa_key_aes_mode(ESP_KEY_MGR_ECDSA_LEN_256, &test_data_ecdsa);
}

TEST(key_manager, ecdsa_p256_key_ecdh0_deployment)
{
    key_mgr_test_ecdsa_key_ecdh0_mode(ESP_KEY_MGR_ECDSA_LEN_256);
}

TEST(key_manager, ecdsa_p256_key_random_deployment)
{
    key_mgr_test_ecdsa_key_random_mode(ESP_KEY_MGR_ECDSA_LEN_256);
}

#if SOC_ECDSA_SUPPORT_CURVE_P384
TEST(key_manager, ecdsa_p384_key_aes_deployment)
{
    key_mgr_test_ecdsa_key_aes_mode(ESP_KEY_MGR_ECDSA_LEN_384, &test_data_ecdsa);
}

TEST(key_manager, ecdsa_p384_key_ecdh0_deployment)
{
    key_mgr_test_ecdsa_key_ecdh0_mode(ESP_KEY_MGR_ECDSA_LEN_384);
}

TEST(key_manager, ecdsa_p384_key_random_deployment)
{
    key_mgr_test_ecdsa_key_random_mode(ESP_KEY_MGR_ECDSA_LEN_384);
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
#endif /* SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY */

#if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
TEST(key_manager, hmac_key_aes_deployment)
{
    key_mgr_test_hmac_aes_mode();
}

TEST(key_manager, hmac_key_ecdh0_deployment)
{
    key_mgr_test_hmac_ecdh0_mode();
}

TEST(key_manager, hmac_key_random_deployment)
{
    key_mgr_test_hmac_random_mode();
}
#endif /* SOC_KEY_MANAGER_HMAC_KEY_DEPLOY */

#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
TEST(key_manager, ds_key_aes_deployment)
{
    key_mgr_test_ds_aes_mode();
}
#endif /* SOC_KEY_MANAGER_DS_KEY_DEPLOY */

TEST_GROUP_RUNNER(key_manager)
{
#if SOC_KEY_MANAGER_FE_KEY_DEPLOY
#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128
    RUN_TEST_CASE(key_manager, xts_aes_128_key_aes_deployment);
    RUN_TEST_CASE(key_manager, xts_key_128_ecdh0_deployment);
#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS
    // This tests expects Flash encryption to be enabled as the test compares the decrypted flash data with the plaintext data
    RUN_TEST_CASE(key_manager, xts_key_128_random_deployment);
#endif /* CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS */
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_128 */

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256
    RUN_TEST_CASE(key_manager, xts_aes_256_key_aes_deployment);
    RUN_TEST_CASE(key_manager, xts_key_256_ecdh0_deployment);
#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS
    RUN_TEST_CASE(key_manager, xts_key_256_random_deployment);
#endif /* CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS */
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY_XTS_AES_256 */
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY */

#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
    RUN_TEST_CASE(key_manager, ecdsa_p192_key_aes_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_p192_key_ecdh0_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_p192_key_random_deployment);

    RUN_TEST_CASE(key_manager, ecdsa_p256_key_aes_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_p256_key_ecdh0_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_p256_key_random_deployment);

#if SOC_ECDSA_SUPPORT_CURVE_P384
    RUN_TEST_CASE(key_manager, ecdsa_p384_key_aes_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_p384_key_ecdh0_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_p384_key_random_deployment);
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
#endif /* SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY */

#if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
    RUN_TEST_CASE(key_manager, hmac_key_aes_deployment);
    RUN_TEST_CASE(key_manager, hmac_key_ecdh0_deployment);
    RUN_TEST_CASE(key_manager, hmac_key_random_deployment);
#endif /* SOC_KEY_MANAGER_HMAC_KEY_DEPLOY */

#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
    RUN_TEST_CASE(key_manager, ds_key_aes_deployment);
    // Verifying deployment of a DS key using the ECDH0 and Random mode of the Key Manager
    // is not possible as the deployed DS key is not known to user in these modes to
    // pre-generate the ciphertext input data.
#endif /* SOC_KEY_MANAGER_DS_KEY_DEPLOY */
}
