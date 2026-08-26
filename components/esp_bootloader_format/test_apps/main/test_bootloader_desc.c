/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>

#include "esp_ota_ops.h"
#include "esp_bootloader_desc.h"
#include "unity.h"
#include "unity_fixture.h"
#include "unity_internals.h"

TEST_GROUP(esp_bootloader_format);

TEST_SETUP(esp_bootloader_format)
{
}

TEST_TEAR_DOWN(esp_bootloader_format)
{
}

TEST(esp_bootloader_format, esp_ota_get_bootloader_description)
{
    esp_bootloader_desc_t desc;
    printf("\n");
    TEST_ESP_OK(esp_ota_get_bootloader_description(NULL, &desc));
    TEST_ASSERT_EQUAL(desc.magic_byte, ESP_BOOTLOADER_DESC_MAGIC_BYTE);
    TEST_ASSERT_EQUAL(desc.version, CONFIG_BOOTLOADER_PROJECT_VER);
    printf("\tESP-IDF version from 2nd stage bootloader: %s\n", desc.idf_ver);
    printf("\tESP-IDF version from app: %s\n", IDF_VER);
    TEST_ASSERT_EQUAL(0, memcmp(desc.idf_ver, IDF_VER, sizeof(IDF_VER)));
}

TEST_GROUP_RUNNER(esp_bootloader_format)
{
    RUN_TEST_CASE(esp_bootloader_format, esp_ota_get_bootloader_description)
}

void app_main(void)
{
    UNITY_MAIN(esp_bootloader_format);
}
