/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "esp_flash.h"
#include "esp_partition.h"
#include "soc/soc_caps.h"

#include "memory_checks.h"
#include "unity_fixture.h"
#include "xts_aes_params.h"

ESP_LOG_ATTR_TAG(TAG, "test_xts_aes");

const esp_partition_t *get_test_data_partition(void)
{
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    TEST_ASSERT_NOT_NULL(result);
    return result;
}

static void run_test_cases(const xts_aes_test *tests, size_t num_tests)
{
    const esp_partition_t *test_part = get_test_data_partition();

    /* Writing multiple lengths because this tests all the various block/offset combinations inside each 1024-bit block */
    const int lengths[] = { 16, 32, 48, 64, 96, 128 };

    for (int i = 0; i < num_tests; i++) {
        size_t address = tests[i].address;
        const uint8_t *ciphertext = tests[i].ciphertext;

        for (int l = 0; l < sizeof(lengths) / sizeof(int); l++) {
            size_t length = lengths[l];
            uint8_t readback[128] = { 0 };

            TEST_ASSERT_LESS_THAN_UINT32(test_part->address + test_part->size, address + length);

            ESP_LOGD(TAG, "Erasing");
            TEST_ASSERT_EQUAL(ESP_OK, esp_flash_erase_region(test_part->flash_chip, test_part->address, test_part->size));

            ESP_LOGD(TAG, "Writing %d encrypted bytes to 0x%x", length, address);
            TEST_ASSERT_EQUAL(ESP_OK, esp_flash_write_encrypted(test_part->flash_chip, address, plaintext, length));

            ESP_LOGD(TAG, "Reading %d bytes from 0x%x", length, address);
            TEST_ASSERT_EQUAL(ESP_OK, esp_flash_read(test_part->flash_chip, readback, address, length));

            ESP_LOGD(TAG, "Readback bytes:");
            ESP_LOG_BUFFER_HEXDUMP(TAG, readback, length, ESP_LOG_DEBUG);

            ESP_LOGD(TAG, "Expected ciphertext:");
            ESP_LOG_BUFFER_HEXDUMP(TAG, ciphertext, length, ESP_LOG_DEBUG);

            TEST_ASSERT_EQUAL_HEX8_ARRAY(ciphertext, readback, length);
        }
    }
}

TEST_GROUP(xts_aes);

TEST_SETUP(xts_aes)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(xts_aes)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

#if SOC_FLASH_ENCRYPTION_XTS_AES_128
TEST(xts_aes, xts_aes_128)
{
    run_test_cases(tests_xts_aes_128, sizeof(tests_xts_aes_128) / sizeof(xts_aes_test));
}
#endif

#if SOC_FLASH_ENCRYPTION_XTS_AES_256
TEST(xts_aes, xts_aes_256)
{
    run_test_cases(tests_xts_aes_256, sizeof(tests_xts_aes_256) / sizeof(xts_aes_test));
}
#endif

TEST_GROUP_RUNNER(xts_aes)
{
#if SOC_FLASH_ENCRYPTION_XTS_AES_128
    RUN_TEST_CASE(xts_aes, xts_aes_128);
#endif

#if SOC_FLASH_ENCRYPTION_XTS_AES_256
    RUN_TEST_CASE(xts_aes, xts_aes_256);
#endif
}
