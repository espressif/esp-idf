/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>

#include "esp_app_desc.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(esp_app_format);

TEST_SETUP(esp_app_format)
{
}

TEST_TEAR_DOWN(esp_app_format)
{
}

TEST(esp_app_format, esp_app_get_elf_sha256_test)
{
    const int sha256_hex_len = CONFIG_APP_RETRIEVE_LEN_ELF_SHA;
    char dst[sha256_hex_len + 2];
    const char fill = 0xcc;
    int res;
    size_t len;

    char ref_sha256[sha256_hex_len + 1];
    const esp_app_desc_t* desc = esp_app_get_description();
    for (int i = 0; i < sizeof(ref_sha256) / 2; ++i) {
        snprintf(ref_sha256 + 2 * i, 3, "%02x", desc->app_elf_sha256[i]);
    }
    ref_sha256[sha256_hex_len] = 0;

    printf("\nRef: %s\n", ref_sha256);

    memset(dst, fill, sizeof(dst));
    len = sizeof(dst);
    res = esp_app_get_elf_sha256(dst, len);
    printf("%d: %s (%d)\n", len, dst, res);
    TEST_ASSERT_EQUAL(sha256_hex_len + 1, res);
    TEST_ASSERT_EQUAL(0, memcmp(dst, ref_sha256, res - 1));
    TEST_ASSERT_EQUAL_HEX(0, dst[sha256_hex_len]);
    TEST_ASSERT_EQUAL_HEX(fill, dst[sha256_hex_len + 1]);

    memset(dst, fill, sizeof(dst));
    len = 9;
    res = esp_app_get_elf_sha256(dst, len);
    printf("%d: %s (%d)\n", len, dst, res);
    TEST_ASSERT_EQUAL(9, res);
    TEST_ASSERT_EQUAL(0, memcmp(dst, ref_sha256, res - 1));
    TEST_ASSERT_EQUAL_HEX(0, dst[8]);
    TEST_ASSERT_EQUAL_HEX(fill, dst[9]);

    memset(ref_sha256, 0xCC, sizeof(ref_sha256));
    strncpy(ref_sha256, esp_app_get_elf_sha256_str(), sizeof(ref_sha256));
    len = strlen(ref_sha256);
    TEST_ASSERT_EQUAL(CONFIG_APP_RETRIEVE_LEN_ELF_SHA, len);
    printf("\n_Ref: %s (len=%d with null)\n", ref_sha256, len);

    TEST_ASSERT_EQUAL(0, esp_app_get_elf_sha256(dst, 0));
    TEST_ASSERT_EQUAL(0, esp_app_get_elf_sha256(dst, 1));
    TEST_ASSERT_EQUAL(0, esp_app_get_elf_sha256(NULL, 99));

    for (size_t req_len = 2; req_len <= CONFIG_APP_RETRIEVE_LEN_ELF_SHA + 1; req_len++) {
        memset(dst, 0xCC, sizeof(dst));
        TEST_ASSERT_EQUAL(req_len, esp_app_get_elf_sha256(dst, req_len));
        len = strlen(dst) + 1; // + 1 for the null terminator
        printf("_%02d_: %-15s (len=%d with null)\n", req_len, dst, len);
        TEST_ASSERT_EQUAL(req_len, len);
        TEST_ASSERT_EQUAL_STRING_LEN(ref_sha256, dst, len - 1); // -1 without null terminator
    }

    memset(dst, 0xCC, sizeof(dst));
    size_t max_len = CONFIG_APP_RETRIEVE_LEN_ELF_SHA + 1; // + 1 for the null terminator
    TEST_ASSERT_EQUAL(max_len, esp_app_get_elf_sha256(dst, 99));
    len = strlen(dst) + 1; // + 1 for the null terminator
    printf("_99_: %-15s (len=%d with null)\n", dst, len);
    TEST_ASSERT_EQUAL(max_len, len);
    TEST_ASSERT_EQUAL_STRING_LEN(ref_sha256, dst, len - 1); // -1 without null terminator
}

TEST_GROUP_RUNNER(esp_app_format)
{
    RUN_TEST_CASE(esp_app_format, esp_app_get_elf_sha256_test)
}

void app_main(void)
{
    UNITY_MAIN(esp_app_format);
}
