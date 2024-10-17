/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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

// For ECDSA tests
#include "hal/ecdsa_hal.h"

const esp_partition_t *get_test_storage_partition(void)
{
    /* This finds "storage" partition defined partition table */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                             ESP_PARTITION_SUBTYPE_ANY, "storage");
    if (!result) {
        /* means partition table set wrong */
        printf("ERROR in obtaining storage partition");
        return NULL;
    }
    return result;
}

static void print_data_in_hex(const uint8_t *data, int size, const char *info_str)
{
    printf("%s: 0x", info_str);
    for(int i = 0 ; i < size; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

static void test_xts_aes_key_aes_mode(test_data_aes_mode_t *test_data)
{
    const esp_partition_t *partition = get_test_storage_partition();
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

    for (int i = 0; i < TEST_COUNT; i++) {
        uint32_t address = test_data->xts_test_data[i].data_offset;
        uint32_t data_size = test_data->xts_test_data[i].data_size;
        ESP_ERROR_CHECK(esp_flash_write_encrypted(NULL, address, test_data->plaintext_data, data_size));
        static uint8_t read_data[128];
        ESP_ERROR_CHECK(esp_flash_read(NULL, read_data, address, data_size));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(test_data->xts_test_data[i].ciphertext, read_data, data_size);
    }
}

static void test_xts_aes_key_ecdh0_mode(test_data_ecdh0_mode_t *test_data)
{
    const esp_partition_t *partition = get_test_storage_partition();
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

    uint32_t address = partition->address;
    uint32_t data_size = 32;

    print_data_in_hex(test_data->plaintext_data, data_size, "Plaintext data");

    ESP_ERROR_CHECK(esp_flash_write_encrypted(NULL, address, test_data->plaintext_data, data_size));
    static uint8_t read_data[128];
    ESP_ERROR_CHECK(esp_flash_read(NULL, read_data, address, data_size));
    print_data_in_hex(read_data, data_size, "Encrypted data");
}

static void key_mgr_test_xts_aes_128(void)
{
    static esp_key_mgr_aes_key_config_t key_config;
    memcpy(key_config.k2_info, (uint8_t*) test_data_xts_aes_128.k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config.k1_encrypted, (uint8_t*) test_data_xts_aes_128.k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config.sw_init_key, (uint8_t*) test_data_xts_aes_128.init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config.use_pre_generated_sw_init_key = 1;
    key_config.key_type = ESP_KEY_MGR_XTS_AES_128_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(&key_config, &key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_aes_mode(&test_data_xts_aes_128);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));

}

static void key_mgr_test_xts_aes_256_aes_mode(void)
{
    static esp_key_mgr_aes_key_config_t key_config;
    memcpy(key_config.k2_info, (uint8_t*) test_data_xts_aes_256.k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config.k1_encrypted[0], (uint8_t*) test_data_xts_aes_256.k1_encrypted[0], KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config.k1_encrypted[1], (uint8_t*) test_data_xts_aes_256.k1_encrypted[1], KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config.sw_init_key, (uint8_t*) test_data_xts_aes_256.init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config.use_pre_generated_sw_init_key = 1;
    key_config.key_type = ESP_KEY_MGR_XTS_AES_256_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(&key_config, &key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_aes_mode(&test_data_xts_aes_256);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
extern void test_ecdsa_export_pubkey(bool is_p256, uint8_t *ecdsa_pub_x, uint8_t *ecdsa_pub_y, bool use_km_key);
extern void test_ecdsa_export_pubkey_inner(bool is_p256, uint8_t *exported_pub_x, uint8_t *exported_pub_y, bool use_km_key, uint16_t *len);
#endif

extern void test_ecdsa_sign_and_verify(bool is_p256, uint8_t* sha, uint8_t* pub_x, uint8_t* pub_y, bool use_km_key, ecdsa_sign_type_t k_type);

/*
const uint8_t message[32] = { 0xDF, 0xDE, 0xD7, 0x4A, 0x47, 0xB1, 0x4F, 0x73, 0x00, 0x21, 0x62, 0xC7, 0x66, 0x6D, 0xA3, 0x95, 0x66, 0x19, 0x62, 0x7F, 0x71, 0x7B, 0x3C, 0x66, 0x82, 0xD3, 0x9F, 0x71, 0xAC, 0x9C, 0xC3, 0x39 };
*/

/* sha256 digest of the above message */
uint8_t sha256_digest[32] = { 0x47, 0xA6, 0xEF, 0xBE, 0x39, 0x5E, 0xE4, 0xAE, 0x2B, 0xEC, 0x83, 0xB1, 0xED, 0xAF, 0xC6, 0x78, 0x57, 0x7A, 0x16, 0x8C, 0x22, 0x16, 0x13, 0xE2, 0xAC, 0xA8, 0x50, 0xD5, 0x67, 0x95, 0x9F, 0x71 };

void test_ecdsa_key_aes_mode(test_data_aes_mode_t *ecdsa_test_data, ecdsa_sign_type_t k_type)
{
    test_ecdsa_sign_and_verify(1, sha256_digest, ecdsa_test_data->ecdsa_test_data.pubx, ecdsa_test_data->ecdsa_test_data.puby, 1, k_type);
#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    test_ecdsa_export_pubkey(1, ecdsa_test_data->ecdsa_test_data.pubx, ecdsa_test_data->ecdsa_test_data.puby, 1);
#endif
}


extern void test_ecdsa_sign(bool is_p256, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, bool use_km_key, ecdsa_sign_type_t k_type);

extern int test_ecdsa_verify(bool is_p256, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, uint8_t *pub_x, uint8_t *pub_y);

void key_mgr_test_ecdsa_key(bool is_p256, ecdsa_sign_type_t k_type)
{
    uint8_t pub_x[32] = {};
    uint8_t pub_y[32] = {};
    uint8_t r_le[32] = {0};
    uint8_t s_le[32] = {0};

    test_ecdsa_sign(is_p256, sha256_digest, r_le, s_le, 1, k_type);

    print_data_in_hex(sha256_digest, sizeof(sha256_digest), "ECDSA message sha256 digest");
    print_data_in_hex(r_le, sizeof(r_le), "ECDSA signature r_le");
    print_data_in_hex(s_le, sizeof(s_le), "ECDSA signature s_le");

    // Export the pubkey from ECDSA peripheral
    uint16_t pubkey_len = 0;
    test_ecdsa_export_pubkey_inner(is_p256, pub_x, pub_y, 1, &pubkey_len);

    print_data_in_hex(pub_x, pubkey_len, "ECDSA key pubx");
    print_data_in_hex(pub_y, pubkey_len, "ECDSA key puby");
    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(is_p256, sha256_digest, r_le, s_le, pub_x, pub_y));

}

static void key_mgr_test_ecdsa_p256_aes_mode(void)
{
    static esp_key_mgr_aes_key_config_t key_config;
    memcpy(key_config.k2_info, (uint8_t*) test_data_ecdsa.k2_info, KEY_MGR_K2_INFO_SIZE);
    memcpy(key_config.k1_encrypted, (uint8_t*) test_data_ecdsa.k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);
    memcpy(key_config.sw_init_key, (uint8_t*) test_data_ecdsa.init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    key_config.use_pre_generated_sw_init_key = 1;
    key_config.key_type = ESP_KEY_MGR_ECDSA_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_aes_mode(&key_config, &key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    test_ecdsa_key_aes_mode(&test_data_ecdsa, ECDSA_K_TYPE_DETERMINISITIC);
#endif
    test_ecdsa_key_aes_mode(&test_data_ecdsa, ECDSA_K_TYPE_TRNG);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

static void key_mgr_test_xts_aes_128_ecdh0_mode(void)
{
    printf("\nKey Manager ECDH0 deployment: XTS_AES_128 key\n");
    static esp_key_mgr_ecdh0_key_config_t key_config;
    memcpy(key_config.k1_G[0], (uint8_t*) test_data_ecdh0.k1_G[0], KEY_MGR_ECDH0_INFO_SIZE);
    key_config.key_type = ESP_KEY_MGR_XTS_AES_128_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    static esp_key_mgr_ecdh0_info_t ecdh0_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(&key_config, &key_recovery_info, &ecdh0_info));

    print_data_in_hex(ecdh0_info.k2_G[0], KEY_MGR_ECDH0_INFO_SIZE, "K2_G");

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_ecdh0_mode(&test_data_ecdh0);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

static void key_mgr_test_xts_aes_256_ecdh0_mode(void)
{
    printf("\nKey Manager ECDH0 deployment: XTS_AES_256 key\n");
    static esp_key_mgr_ecdh0_key_config_t key_config;
    memcpy(key_config.k1_G[0], (uint8_t*) test_data_ecdh0.k1_G[0], KEY_MGR_ECDH0_INFO_SIZE);
    memcpy(key_config.k1_G[1], (uint8_t*) test_data_ecdh0.k1_G[1], KEY_MGR_ECDH0_INFO_SIZE);
    key_config.key_type = ESP_KEY_MGR_XTS_AES_256_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    static esp_key_mgr_ecdh0_info_t ecdh0_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(&key_config, &key_recovery_info, &ecdh0_info));

    print_data_in_hex(ecdh0_info.k2_G[0], KEY_MGR_ECDH0_INFO_SIZE, "K2_G_0");
    print_data_in_hex(ecdh0_info.k2_G[1], KEY_MGR_ECDH0_INFO_SIZE, "K2_G_1");

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_ecdh0_mode(&test_data_ecdh0);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

static void key_mgr_test_ecdsa_ecdh0_mode(void)
{
    printf("\nKey Manager ECDH0 deployment: ECDSA_256 key\n");
    static esp_key_mgr_ecdh0_key_config_t key_config;
    memcpy(key_config.k1_G[0], (uint8_t*) test_data_ecdh0.k1_G[0], KEY_MGR_ECDH0_INFO_SIZE);
    key_config.key_type = ESP_KEY_MGR_ECDSA_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    static esp_key_mgr_ecdh0_info_t ecdh0_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_ecdh0_mode(&key_config, &key_recovery_info, &ecdh0_info));

    print_data_in_hex(ecdh0_info.k2_G[0], KEY_MGR_ECDH0_INFO_SIZE, "K2_G");

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    key_mgr_test_ecdsa_key(1, ECDSA_K_TYPE_DETERMINISITIC);
#endif
    key_mgr_test_ecdsa_key(1, ECDSA_K_TYPE_TRNG);
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

static void key_mgr_test_ecdsa_random_mode(void)
{
    printf("\nKey Manager Random deployment: ECDSA_256 key\n");
    static esp_key_mgr_random_key_config_t key_config;
    key_config.key_type = ESP_KEY_MGR_ECDSA_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(&key_config, &key_recovery_info));

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));

#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    key_mgr_test_ecdsa_key(1, ECDSA_K_TYPE_DETERMINISITIC);
#endif
    key_mgr_test_ecdsa_key(1, ECDSA_K_TYPE_TRNG);

    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}

#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS

static void test_xts_aes_key_random_mode(void)
{
    const esp_partition_t *partition = get_test_storage_partition();
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
    uint8_t plaintext_data[1024] = {[0 ... 1023] = 0xBE};
    const int write_size = 16;
    for (int i = 0; i < sizeof(plaintext_data) / write_size; i++) {
        printf("\n i = %d\n", i);
        ESP_ERROR_CHECK(esp_flash_write_encrypted(NULL, partition->address + (i * write_size), plaintext_data, write_size));
        static uint8_t read_data[128];
        ESP_ERROR_CHECK(esp_partition_read(partition, write_size * i, read_data, write_size));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(plaintext_data + (i * write_size), read_data, write_size);
    }
}


static void key_mgr_test_xts_aes_128_random_mode(void)
{
    static esp_key_mgr_random_key_config_t key_config;
    key_config.key_type = ESP_KEY_MGR_XTS_AES_128_KEY;

    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(&key_config, &key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_random_mode();
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));

}

static void key_mgr_test_xts_aes_256_random_mode(void)
{
    static esp_key_mgr_random_key_config_t key_config;
    key_config.key_type = ESP_KEY_MGR_XTS_AES_256_KEY;
    static esp_key_mgr_key_recovery_info_t key_recovery_info;
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deploy_key_in_random_mode(&key_config, &key_recovery_info));
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_activate_key(&key_recovery_info));
    test_xts_aes_key_random_mode();
    TEST_ASSERT_EQUAL(ESP_OK, esp_key_mgr_deactivate_key(key_recovery_info.key_type));
}
#endif

TEST_GROUP(key_manager);

TEST_SETUP(key_manager)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(700, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(key_manager)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(key_manager, xts_aes_128_key_aes_deployment)
{
    key_mgr_test_xts_aes_128();
}

TEST(key_manager, xts_aes_256_key_aes_deployment)
{
    key_mgr_test_xts_aes_256_aes_mode();
}

TEST(key_manager, ecdsa_key_aes_deployment)
{
    key_mgr_test_ecdsa_p256_aes_mode();
}

TEST(key_manager, xts_key_ecdh0_deployment)
{
    key_mgr_test_xts_aes_128_ecdh0_mode();
    key_mgr_test_xts_aes_256_ecdh0_mode();
}

TEST(key_manager, ecdsa_key_ecdh0_deployment)
{
    key_mgr_test_ecdsa_ecdh0_mode();
}

TEST(key_manager, ecdsa_key_random_deployment)
{
    key_mgr_test_ecdsa_random_mode();
}

#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS
TEST(key_manager, xts_key_random_deployment)
{
    key_mgr_test_xts_aes_128_random_mode();
    key_mgr_test_xts_aes_256_random_mode();
}
#endif

TEST_GROUP_RUNNER(key_manager)
{
    RUN_TEST_CASE(key_manager, xts_aes_128_key_aes_deployment);
    RUN_TEST_CASE(key_manager, xts_aes_256_key_aes_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_key_aes_deployment);
    RUN_TEST_CASE(key_manager, xts_key_ecdh0_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_key_ecdh0_deployment);
    RUN_TEST_CASE(key_manager, ecdsa_key_random_deployment);
#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS
    RUN_TEST_CASE(key_manager, xts_key_random_deployment);
#endif

}
