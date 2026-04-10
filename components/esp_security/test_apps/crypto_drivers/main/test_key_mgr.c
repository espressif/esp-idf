/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "unity.h"

#include "soc/soc_caps.h"
#include "esp_partition.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_err.h"
#if CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/efuse.h"
#endif
#include "soc/keymng_reg.h"
#include "esp_key_mgr.h"
#include "esp_system.h"
#include "unity_test_utils_memory.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include "hal/key_mgr_ll.h"

#if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
#include "esp_hmac.h"
#include "hmac_test_cases.h"
#endif /* SOC_KEY_MANAGER_HMAC_KEY_DEPLOY */

#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
#include "esp_ds.h"

#define NUM_RESULTS 10
typedef struct {
    uint8_t iv[ESP_DS_IV_LEN];
    esp_ds_p_data_t p_data;
    uint8_t expected_c[ESP_DS_C_LEN];
    uint8_t hmac_key_idx;
    uint32_t expected_results[NUM_RESULTS][SOC_RSA_MAX_BIT_LEN / 32];
} encrypt_testcase_t;

#if SOC_RSA_MAX_BIT_LEN == 4096
#include "digital_signature_test_cases_4096.h"

// Note: generated using the hmac_key_idx = 2 of digital_signature_test_cases_4096
#define DS_KEY_IDX 1
static const uint8_t k1_ds_encrypt[] = {
    0xf1, 0xbf, 0xa0, 0x27, 0x83, 0x02, 0x85, 0x94, 0x07, 0xb4, 0x20, 0x46, 0x1f, 0x1d, 0x4e, 0x24, 0x1d, 0x38, 0xcb, 0x21, 0xed, 0xcf, 0x85, 0x79, 0xe7, 0x17, 0xbf, 0x9d, 0xbb, 0xb8, 0x6f, 0xbd
};

#elif SOC_RSA_MAX_BIT_LEN == 3072
#include "digital_signature_test_cases_3072.h"

// Note: generated using the hmac_key_idx = 0 of digital_signature_test_cases_3072
#define DS_KEY_IDX 0
static const uint8_t k1_ds_encrypt[] = {
    0xdc, 0xcc, 0x8e, 0xd4, 0x66, 0xb4, 0x1a, 0x4b, 0x9f, 0xba, 0xa3, 0x2e, 0x99, 0x6b, 0x45, 0x07, 0xd4, 0xf6, 0xe4, 0x4e, 0x6f, 0x26, 0xca, 0x20, 0xbd, 0xb4, 0x98, 0x7c, 0x32, 0x4f, 0x21, 0x55
};
#endif
#endif /* SOC_KEY_MANAGER_DS_KEY_DEPLOY */

static const char *TAG = "key_mgr_test";

#define ENCRYPTED_DATA_SIZE 128
static const uint8_t plaintext_data[ENCRYPTED_DATA_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40,
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80,
};

static const uint8_t expected_ciphertext[ENCRYPTED_DATA_SIZE] = {
    0xa, 0x6, 0x88, 0xb9, 0x9f, 0x3e, 0xff, 0xf5, 0x63, 0x37, 0xa2, 0xd6, 0xbb, 0x71, 0x50, 0x89,
    0xa7, 0xbf, 0xa1, 0x5d, 0x3a, 0xc7, 0x18, 0x66, 0xa7, 0xab, 0x5f, 0x7, 0xd, 0x7c, 0xad, 0xcc,
    0x8c, 0x9a, 0xc7, 0xff, 0x10, 0xf6, 0xab, 0xef, 0x62, 0x71, 0xdf, 0xc, 0x60, 0x18, 0x4f, 0xa2,
    0x2c, 0x55, 0x14, 0x41, 0xd5, 0xfc, 0x87, 0x44, 0xe1, 0xd1, 0xd1, 0x78, 0x76, 0x9f, 0x43, 0x87,
    0xa0, 0xb4, 0xea, 0x66, 0xbc, 0x56, 0x35, 0x39, 0x53, 0x60, 0xa6, 0xea, 0x1e, 0x7d, 0x48, 0x17,
    0x6e, 0x58, 0x4c, 0x68, 0xc0, 0xfb, 0x54, 0x35, 0x92, 0x7b, 0x89, 0x9f, 0x69, 0x33, 0x1, 0x17,
    0x85, 0xe2, 0x69, 0x26, 0xfb, 0x61, 0x7e, 0x15, 0x11, 0x26, 0x59, 0xf1, 0x4f, 0xeb, 0x4d, 0x25,
    0x8e, 0xc5, 0xb9, 0x64, 0xdf, 0xc7, 0x52, 0x87, 0x2d, 0xb, 0xd6, 0xdd, 0xb1, 0xb3, 0xfe, 0xaf,
};

/* Big endian */
static const uint8_t init_key[] = {
    0xee, 0x89, 0x95, 0xda, 0x3c, 0x8a, 0x43, 0x83, 0xa9, 0x4b, 0x25, 0x5b, 0x04, 0x7e, 0xf1, 0x57,
    0xb8, 0xe8, 0x06, 0x45, 0x87, 0x76, 0xee, 0x1b, 0x4e, 0x2e, 0x55, 0xa7, 0x1f, 0x25, 0xe1, 0x94,
};

/* Big endian */
static const uint8_t k2_info[] = {
    0xba, 0x7f, 0xe1, 0x43, 0xce, 0x19, 0xaf, 0xe8, 0xee, 0x04, 0x17, 0xcf, 0x67, 0x9d, 0xf4, 0xeb,
    0x44, 0x12, 0x6f, 0xee, 0x9f, 0xfe, 0x9f, 0x5f, 0x20, 0x18, 0x9f, 0x27, 0x3f, 0xcb, 0x7b, 0xba,
    0x92, 0x37, 0x07, 0x92, 0x47, 0xee, 0x1a, 0xbe, 0xa8, 0x7e, 0x32, 0x70, 0xc1, 0x05, 0x42, 0xe4,
    0x88, 0x9d, 0x0b, 0x2b, 0x40, 0x87, 0x6e, 0x58, 0xf3, 0xf3, 0x63, 0x62, 0xd1, 0x5c, 0x9a, 0xe8
};

/* Big endian */
static const uint8_t k1_xts_aes_encrypt[] = {
    0x0a, 0x43, 0x2a, 0xee, 0xb5, 0xd0, 0x6b, 0x58, 0x34, 0x2d, 0x0d, 0xbf, 0xf7, 0x04, 0x60, 0x81,
    0x08, 0x79, 0x38, 0xb3, 0xb5, 0xdf, 0x25, 0xb4, 0x38, 0x8e, 0x93, 0x5d, 0xbf, 0xd1, 0x91, 0x6d,
};

static const uint8_t k1_hmac_encrypt[] = {
    0x02, 0xf7, 0x05, 0x75, 0xfd, 0x12, 0x99, 0xc2, 0xb9, 0xc0, 0x2e, 0x40, 0xd0, 0x69, 0xb0, 0x83,
    0x13, 0xfc, 0x74, 0xc3, 0xec, 0x4f, 0xf4, 0xd4, 0xe2, 0x35, 0xb5, 0x42, 0x4e, 0x61, 0x0f, 0x6d,
};

static const uint8_t k1_G[] = {
    0x25, 0x8c, 0x48, 0x4d, 0x0b, 0x4d, 0x3f, 0xbf, 0xde, 0xcf, 0x00, 0xc9, 0x4b, 0x0b, 0xf1, 0x14,
    0xb4, 0x31, 0x97, 0x79, 0x5a, 0xd3, 0x48, 0x72, 0x44, 0x2d, 0xab, 0x76, 0x29, 0xb9, 0x8b, 0x05,
    0xf5, 0x6b, 0xfb, 0xb4, 0xe4, 0xde, 0x81, 0x83, 0xa7, 0x0a, 0x90, 0xe4, 0x33, 0x41, 0x92, 0xaa,
    0xc5, 0xed, 0x93, 0xe0, 0x76, 0x2b, 0xe2, 0x4b, 0xdd, 0xa2, 0x8e, 0xe1, 0xc9, 0xe2, 0x94, 0x50,
};

#if SOC_KEY_MANAGER_FE_KEY_DEPLOY
const esp_partition_t *get_test_storage_partition(void)
{
    /* This finds "storage" partition defined partition table */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                             ESP_PARTITION_SUBTYPE_ANY, "storage");
    if (!result) {
        /* means partition table set wrong */
        ESP_LOGE(TAG, "ERROR in obtaining storage partition");
        return NULL;
    }
    return result;
}

static esp_err_t test_xts_aes_key(bool verify)
{
    const esp_partition_t *partition = get_test_storage_partition();
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

    uint32_t address = partition->address;
    ESP_ERROR_CHECK(esp_flash_write_encrypted(NULL, address, plaintext_data, sizeof(plaintext_data)));
    uint8_t read_data[ENCRYPTED_DATA_SIZE];
    ESP_ERROR_CHECK(esp_flash_read(NULL, read_data, address, sizeof(read_data)));
    if (verify) {
        TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_ciphertext, read_data, sizeof(expected_ciphertext));
    }
    return ESP_OK;
}

TEST_CASE("Key Manager AES mode: XTS-AES-128 key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_aes_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_aes_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    memcpy(key_config->k2_info, (uint8_t*) k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config->k1_encrypted, (uint8_t*) k1_xts_aes_encrypt, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config->sw_init_key, (uint8_t*) init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config->use_pre_generated_sw_init_key = 1;
    key_config->key_type = ESP_KEY_MGR_FLASH_XTS_AES_KEY;
    key_config->key_len = ESP_KEY_MGR_XTS_AES_LEN_128;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(key_config, key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, test_xts_aes_key(true));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
}

TEST_CASE("Key Manager ECDH0 mode: XTS-AES-128 key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_ecdh0_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_ecdh0_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    memcpy(key_config->k1_G, (uint8_t*) k1_G, KEY_MGR_ECDH0_INFO_SIZE);
    key_config->key_type = ESP_KEY_MGR_FLASH_XTS_AES_KEY;
    key_config->key_len = ESP_KEY_MGR_XTS_AES_LEN_128;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    esp_key_mgr_ecdh0_info_t *ecdh0_info = calloc(1, sizeof(esp_key_mgr_ecdh0_info_t));
    TEST_ASSERT_NOT_NULL(ecdh0_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(key_config, key_recovery_info, ecdh0_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, test_xts_aes_key(false));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
    free(ecdh0_info);
}

TEST_CASE("Key Manager Random mode: XTS-AES-128 key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_random_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_random_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    key_config->key_type = ESP_KEY_MGR_FLASH_XTS_AES_KEY;
    key_config->key_len = ESP_KEY_MGR_XTS_AES_LEN_128;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(key_config, key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, test_xts_aes_key(false));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
}
#endif /* SOC_KEY_MANAGER_FE_KEY_DEPLOY */

#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
TEST_CASE("Key Manager random mode: ECDSA key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_random_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_random_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    key_config->key_type = ESP_KEY_MGR_ECDSA_KEY;
    key_config->key_len = ESP_KEY_MGR_ECDSA_LEN_256;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(key_config, key_recovery_info));

    free(key_config);
    free(key_recovery_info);
}
#endif /* SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY */

#if SOC_KEY_MANAGER_HMAC_KEY_DEPLOY
static esp_err_t test_hmac_key(bool verify)
{
    uint8_t hmac[32];
    for (int i = 0; i < sizeof(results) / sizeof(hmac_result); i++) {
        TEST_ESP_OK(esp_hmac_calculate(HMAC_KEY_KM, message, results[i].msglen, hmac));
        if (verify) {
            TEST_ASSERT_EQUAL_HEX8_ARRAY(results[i].result, hmac, sizeof(hmac));
        }
    }
    return ESP_OK;
}

TEST_CASE("Key Manager AES mode: HMAC key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_aes_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_aes_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    memcpy(key_config->k2_info, (uint8_t*) k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config->k1_encrypted, (uint8_t*) k1_hmac_encrypt, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config->sw_init_key, (uint8_t*) init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config->use_pre_generated_sw_init_key = 1;
    key_config->key_type = ESP_KEY_MGR_HMAC_KEY;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(key_config, key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, test_hmac_key(true));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
}

TEST_CASE("Key Manager ECDH0 mode: HMAC key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_ecdh0_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_ecdh0_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    memcpy(key_config->k1_G, (uint8_t*) k1_G, KEY_MGR_ECDH0_INFO_SIZE);
    key_config->key_type = ESP_KEY_MGR_HMAC_KEY;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    esp_key_mgr_ecdh0_info_t *ecdh0_info = calloc(1, sizeof(esp_key_mgr_ecdh0_info_t));
    TEST_ASSERT_NOT_NULL(ecdh0_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(key_config, key_recovery_info, ecdh0_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, test_hmac_key(false));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
    free(ecdh0_info);
}

TEST_CASE("Key Manager random mode: HMAC key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_random_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_random_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    key_config->key_type = ESP_KEY_MGR_HMAC_KEY;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(key_config, key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, test_hmac_key(false));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
}
#endif /* SOC_KEY_MANAGER_HMAC_KEY_DEPLOY */

#if SOC_KEY_MANAGER_DS_KEY_DEPLOY
static esp_err_t test_ds_key(void)
{
    esp_ds_data_t ds_data = { };
    uint8_t signature[ESP_DS_SIGNATURE_MAX_BIT_LEN / 8] = { 0 };
    esp_err_t ds_r = ESP_FAIL;

    for (int i = 0; i < NUM_MESSAGES; i++) {
        printf("Running test case %d...\n", i);
        const encrypt_testcase_t *t = &test_cases[0];
        assert(t->hmac_key_idx == DS_KEY_IDX); // as the key deployed using Key Manager is the HMAC key with index DS_KEY_IDX

        // copy encrypt parameter test case into ds_data structure
        memcpy(ds_data.iv, t->iv, ESP_DS_IV_LEN);
        memcpy(ds_data.c, t->expected_c, ESP_DS_C_LEN);
        ds_data.rsa_length = t->p_data.length;

        ds_r = esp_ds_sign(test_messages[i],
                           &ds_data,
                           HMAC_KEY_KM,
                           signature);

        TEST_ASSERT_EQUAL(ESP_OK, ds_r);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(t->expected_results[i], signature, sizeof(signature));
    }
    return ESP_OK;
}

TEST_CASE("Key Manager AES mode: DS key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_aes_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_aes_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    memcpy(key_config->k2_info, (uint8_t*) k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config->k1_encrypted, (uint8_t*) k1_ds_encrypt, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config->sw_init_key, (uint8_t*) init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config->use_pre_generated_sw_init_key = 1;
    key_config->key_type = ESP_KEY_MGR_DS_KEY;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(key_config, key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, test_ds_key());
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
}

TEST_CASE("Key Manager ECDH0 mode: DS key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_ecdh0_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_ecdh0_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    memcpy(key_config->k1_G, (uint8_t*) k1_G, KEY_MGR_ECDH0_INFO_SIZE);
    key_config->key_type = ESP_KEY_MGR_DS_KEY;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    esp_key_mgr_ecdh0_info_t *ecdh0_info = calloc(1, sizeof(esp_key_mgr_ecdh0_info_t));
    TEST_ASSERT_NOT_NULL(ecdh0_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(key_config, key_recovery_info, ecdh0_info));
    // Generate the deployed DS key and use ds_encrypt_params to generate encrypted input params
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
    free(ecdh0_info);
}

TEST_CASE("Key Manager random mode: DS key deployment", "[hw_crypto] [key_mgr]")
{
    esp_key_mgr_random_key_config_t *key_config = calloc(1, sizeof(esp_key_mgr_random_key_config_t));
    TEST_ASSERT_NOT_NULL(key_config);

    key_config->key_type = ESP_KEY_MGR_DS_KEY;

    esp_key_mgr_key_recovery_info_t *key_recovery_info = calloc(1, sizeof(esp_key_mgr_key_recovery_info_t));
    TEST_ASSERT_NOT_NULL(key_recovery_info);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(key_config, key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(key_recovery_info));
    // No way to generate encrypted input params when DS key deployed in random mode
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info->key_type));

    free(key_config);
    free(key_recovery_info);
}
#endif /* SOC_KEY_MANAGER_DS_KEY_DEPLOY */
#endif /* SOC_KEY_MANAGER_SUPPORTED */
