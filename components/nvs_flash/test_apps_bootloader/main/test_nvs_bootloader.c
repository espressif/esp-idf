/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "string.h"
#include "esp_err.h"

#include "unity.h"
#include "nvs_bootloader.h"

TEST_CASE("Verify nvs bootloader read_list result_code and value if bootloader read is successful", "[nvs_bootloader]")
{
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

    TEST_ESP_OK(nvs_bootloader_read("nvs", size, read_list));

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
}

TEST_CASE("Verify nvs bootloader read_list result_code if bootloader read fails", "[nvs_bootloader]")
{
    nvs_bootloader_read_list_t read_list[] = {
//      {namespace_name, key_name, value_type, result_code, value, namespace_index}}
        {  .namespace_name = "too_long_namespace", .key_name = "i32_key",           .value_type = NVS_TYPE_I32  },  //0 Invalid name
        {  .namespace_name = "nvs",                .key_name = "too_long_key_name", .value_type = NVS_TYPE_I32  },  //1 Key too long
        {  .namespace_name = "nvs",                .key_name = "str_key",           .value_type = NVS_TYPE_BLOB },  //2 Invalid arg
        {  .namespace_name = "nvs",                .key_name = "i32_key",           .value_type = NVS_TYPE_I32  },  //3 Not found
    };
    uint8_t size = sizeof(read_list) / sizeof(read_list[0]);

    esp_err_t ret = nvs_bootloader_read("nvs", size, read_list);

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, ret);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_INVALID_NAME, read_list[0].result_code);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_KEY_TOO_LONG, read_list[1].result_code);
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, read_list[2].result_code);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_NOT_FOUND, read_list[3].result_code);
}

TEST_CASE("Verify nvs_bootloader_read failure cases", "[nvs_bootloader]")
{
    nvs_bootloader_read_list_t read_list[] = {
//      {namespace_name, key_name, value_type, result_code, value, namespace_index}}
        { "nvs", "i32_key", NVS_TYPE_I32, ESP_OK, {0}, 0}
    };
    uint8_t size = sizeof(read_list) / sizeof(read_list[0]);

    esp_err_t ret = nvs_bootloader_read("nvs_partition_name_too_long", size, read_list);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_INVALID_NAME, ret);

    ret = nvs_bootloader_read("nvs_part", size, read_list);
    TEST_ASSERT_EQUAL(ESP_ERR_NVS_PART_NOT_FOUND, ret);
}
