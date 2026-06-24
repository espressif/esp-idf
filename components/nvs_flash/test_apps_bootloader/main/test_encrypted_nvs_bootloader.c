/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include "esp_err.h"
#include "esp_efuse.h"
#include "esp_partition.h"
#include "nvs_sec_provider.h"
#include "unity.h"

#include "nvs_bootloader.h"

static esp_err_t configure_nvs_sec_cfg(nvs_sec_cfg_t *cfg, nvs_sec_scheme_t **sec_scheme_handle)
{
    const esp_partition_t* nvs_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    TEST_ASSERT(nvs_part && "partition table must have an NVS partition");
    printf("\n nvs_part size:%" PRId32 "\n", nvs_part->size);
    ESP_ERROR_CHECK(esp_partition_erase_range(nvs_part, 0, nvs_part->size));

#if CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC
    if (!esp_efuse_is_flash_encryption_enabled()) {
        TEST_IGNORE_MESSAGE("flash encryption disabled, skipping nvs_api tests with encryption enabled");
    }

    extern const char nvs_key_start[] asm("_binary_encryption_keys_bin_start");
    extern const char nvs_key_end[]   asm("_binary_encryption_keys_bin_end");
    extern const char nvs_data_sch0_start[] asm("_binary_partition_encrypted_bin_start");
    extern const char nvs_data_sch0_end[] asm("_binary_partition_encrypted_bin_end");

    const esp_partition_t* key_part = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);

    assert(key_part && "partition table must have a KEY partition");
    TEST_ASSERT_TRUE((nvs_key_end - nvs_key_start - 1) == key_part->erase_size);

    ESP_ERROR_CHECK(esp_partition_erase_range(key_part, 0, key_part->size));

    for (int i = 0; i < key_part->size; i+= key_part->erase_size) {
        ESP_ERROR_CHECK( esp_partition_write(key_part, i, nvs_key_start + i, key_part->erase_size) );
    }

    const int content_size = nvs_data_sch0_end - nvs_data_sch0_start - 1;
    TEST_ASSERT_TRUE((content_size % key_part->erase_size) == 0);

    const int size_to_write = MIN(content_size, nvs_part->size);
    for (int i = 0; i < size_to_write; i+= nvs_part->erase_size) {
        ESP_ERROR_CHECK( esp_partition_write(nvs_part, i, nvs_data_sch0_start + i, nvs_part->erase_size) );
    }

    nvs_sec_config_flash_enc_t sec_scheme_cfg = {
        .nvs_keys_part = key_part
    };

    TEST_ESP_OK(nvs_sec_provider_register_flash_enc(&sec_scheme_cfg, sec_scheme_handle));
    return nvs_flash_read_security_cfg_v2(*sec_scheme_handle, cfg);

#elif SOC_HMAC_SUPPORTED
    extern const char nvs_data_sch1_start[]    asm("_binary_partition_encrypted_hmac_bin_start");
    extern const char nvs_data_sch1_end[]    asm("_binary_partition_encrypted_hmac_bin_end");

    const int content_size = nvs_data_sch1_end - nvs_data_sch1_start - 1;
    TEST_ASSERT_TRUE((content_size % nvs_part->erase_size) == 0);

    const int size_to_write = MIN(content_size, nvs_part->size);
    for (int i = 0; i < size_to_write; i+= nvs_part->erase_size) {
        ESP_ERROR_CHECK( esp_partition_write(nvs_part, i, nvs_data_sch1_start + i, nvs_part->erase_size) );
    }

#ifndef CONFIG_NVS_ENCRYPTION
    nvs_sec_config_hmac_t sec_scheme_cfg = {
        .hmac_key_id = HMAC_KEY0,
    };
#else
    nvs_sec_config_hmac_t sec_scheme_cfg = NVS_SEC_PROVIDER_CFG_HMAC_DEFAULT();
#endif /* CONFIG_NVS_ENCRYPTION */

    TEST_ESP_OK(nvs_sec_provider_register_hmac(&sec_scheme_cfg, sec_scheme_handle));
    return nvs_flash_read_security_cfg_v2(*sec_scheme_handle, cfg);
#endif

    return ESP_FAIL;
}

static void restore_nvs_partition(void)
{
    const esp_partition_t* nvs_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    TEST_ASSERT(nvs_part && "partition table must have an NVS partition");
    printf("\n nvs_part size:%" PRId32 "\n", nvs_part->size);
    ESP_ERROR_CHECK(esp_partition_erase_range(nvs_part, 0, nvs_part->size));

    extern const char nvs_data_start[]    asm("_binary_nvs_partition_bin_start");
    extern const char nvs_data_end[]    asm("_binary_nvs_partition_bin_end");

    const int content_size = nvs_data_end - nvs_data_start - 1;
    TEST_ASSERT_TRUE((content_size % nvs_part->erase_size) == 0);

    const int size_to_write = MIN(content_size, nvs_part->size);
    for (int i = 0; i < size_to_write; i+= nvs_part->erase_size) {
        ESP_ERROR_CHECK(esp_partition_write(nvs_part, i, nvs_data_start + i, nvs_part->erase_size));
    }
}

TEST_CASE("Verify encrypted nvs bootloader read_list result_code and value if bootloader read is successful", "[nvs_encrypted_bootloader]")
{
    nvs_sec_cfg_t xts_cfg;
    nvs_sec_scheme_t *sec_scheme_handle = NULL;
    TEST_ESP_OK(configure_nvs_sec_cfg(&xts_cfg, &sec_scheme_handle));

    nvs_bootloader_read_list_t read_list[] = {
//      {namespace_name, key_name, value_type, result_code, value, namespace_index}}
        { .namespace_name = "storage", .key_name = "u8_key",  .value_type = NVS_TYPE_U8 },      //0 OK
        { .namespace_name = "storage", .key_name = "u16_key", .value_type = NVS_TYPE_U16 },     //1 OK
        { .namespace_name = "storage", .key_name = "u32_key", .value_type = NVS_TYPE_U32 },     //2 OK
        { .namespace_name = "storage", .key_name = "i32_key", .value_type = NVS_TYPE_I32 },     //3 OK
        { .namespace_name = "storage", .key_name = "i8_key",  .value_type = NVS_TYPE_U8  },     //4 Type mismatch
        { .namespace_name = "storage", .key_name = "i16_key", .value_type = NVS_TYPE_I16 },     //5 Not found
    };
    uint8_t size = sizeof(read_list) / sizeof(read_list[0]);

    TEST_ESP_OK(nvs_bootloader_secure_init(&xts_cfg));
    TEST_ESP_OK(nvs_bootloader_read("nvs", size, read_list));
    nvs_bootloader_secure_deinit();

    TEST_ASSERT_EQUAL(ESP_OK, read_list[0].result_code);
    TEST_ASSERT_EQUAL(ESP_OK, read_list[1].result_code);
    TEST_ASSERT_EQUAL(ESP_OK, read_list[2].result_code);
    TEST_ASSERT_EQUAL(ESP_OK, read_list[3].result_code);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_TYPE_MISMATCH, read_list[4].result_code);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, read_list[5].result_code);

    TEST_ASSERT_EQUAL(255, read_list[0].value.u8_val);
    TEST_ASSERT_EQUAL(65535, read_list[1].value.u16_val);
    TEST_ASSERT_EQUAL(4294967295, read_list[2].value.u32_val);
    TEST_ASSERT_EQUAL(-2147483648, read_list[3].value.i32_val);

    TEST_ESP_OK(nvs_sec_provider_deregister(sec_scheme_handle));
    restore_nvs_partition();
}

TEST_CASE("Verify encrypted nvs bootloader read_list result_code if bootloader read fails", "[nvs_encrypted_bootloader]")
{
    nvs_sec_cfg_t xts_cfg;
    nvs_sec_scheme_t *sec_scheme_handle = NULL;
    TEST_ESP_OK(configure_nvs_sec_cfg(&xts_cfg, &sec_scheme_handle));

    nvs_bootloader_read_list_t read_list[] = {
//      {namespace_name, key_name, value_type, result_code, value, namespace_index}}
        {  .namespace_name = "too_long_namespace", .key_name = "i32_key",           .value_type = NVS_TYPE_I32  },  //0 Invalid name
        {  .namespace_name = "nvs",                .key_name = "too_long_key_name", .value_type = NVS_TYPE_I32  },  //1 Key too long
        {  .namespace_name = "nvs",                .key_name = "str_key",           .value_type = NVS_TYPE_BLOB },  //2 Invalid arg
        {  .namespace_name = "nvs",                .key_name = "i32_key",           .value_type = NVS_TYPE_I32  },  //3 Not found
    };
    uint8_t size = sizeof(read_list) / sizeof(read_list[0]);

    TEST_ESP_OK(nvs_bootloader_secure_init(&xts_cfg));
    esp_err_t ret = nvs_bootloader_read("nvs", size, read_list);

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ret);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_INVALID_NAME, read_list[0].result_code);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_KEY_TOO_LONG, read_list[1].result_code);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, read_list[2].result_code);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, read_list[3].result_code);

    nvs_bootloader_secure_deinit();

    TEST_ESP_OK(nvs_sec_provider_deregister(sec_scheme_handle));
    restore_nvs_partition();
}

TEST_CASE("Verify nvs_bootloader_read_encrypted failure cases", "[nvs_encrypted_bootloader]")
{
    nvs_sec_cfg_t xts_cfg;
    nvs_sec_scheme_t *sec_scheme_handle = NULL;
    TEST_ESP_OK(configure_nvs_sec_cfg(&xts_cfg, &sec_scheme_handle));

    nvs_bootloader_read_list_t read_list[] = {
//      {namespace_name, key_name, value_type, result_code, value, namespace_index}}
        { "nvs", "i32_key", NVS_TYPE_I32, ESP_OK, {0}, 0}
    };
    uint8_t size = sizeof(read_list) / sizeof(read_list[0]);

    TEST_ESP_OK(nvs_bootloader_secure_init(&xts_cfg));
    esp_err_t ret = nvs_bootloader_read("nvs_partition_name_too_long", size, read_list);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_INVALID_NAME, ret);

    ret = nvs_bootloader_read("nvs_part", size, read_list);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_PART_NOT_FOUND, ret);

    nvs_bootloader_secure_deinit();
    TEST_ESP_OK(nvs_sec_provider_deregister(sec_scheme_handle));
    restore_nvs_partition();
}
