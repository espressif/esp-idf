/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 * Tests for bootloader_support esp_load(ESP_IMAGE_VERIFY, ...)
 */

#include <esp_types.h>
#include <stdio.h>
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "bootloader_common.h"
#include "bootloader_util.h"
#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "esp_image_format.h"

TEST_CASE("Verify bootloader image in flash", "[bootloader_support][qemu-ignore-c3]")
{
    const esp_partition_pos_t fake_bootloader_partition = {
        .offset = ESP_BOOTLOADER_OFFSET,
        .size = ESP_PARTITION_TABLE_OFFSET - ESP_BOOTLOADER_OFFSET,
    };
    esp_image_metadata_t data = { 0 };
    TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_image_verify(ESP_IMAGE_VERIFY, &fake_bootloader_partition, &data));
    TEST_ASSERT_NOT_EQUAL(0, data.image_len);

    uint32_t bootloader_length = 0;
    TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_image_verify_bootloader(&bootloader_length));
    TEST_ASSERT_EQUAL(data.image_len, bootloader_length);
}

TEST_CASE("Verify unit test app image", "[bootloader_support][qemu-ignore-c3]")
{
    esp_image_metadata_t data = { 0 };
    const esp_partition_t *running = esp_ota_get_running_partition();
    TEST_ASSERT_NOT_EQUAL(NULL, running);
    const esp_partition_pos_t running_pos  = {
        .offset = running->address,
        .size = running->size,
    };
    TEST_ASSERT_GREATER_THAN(0, running->address);
    TEST_ASSERT_GREATER_THAN(0, running->size);

    TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_image_verify(ESP_IMAGE_VERIFY, &running_pos, &data));
    TEST_ASSERT_NOT_EQUAL(0, data.image_len);
    TEST_ASSERT_TRUE(data.image_len <= running->size);
}

void check_label_search (int num_test, const char *list, const char *t_label, bool result)
{
    // gen_esp32part.py trims up to 16 characters
    // and the string may not have a null terminal symbol.
    // below is cutting as it does the generator.
    char label[16 + 1] = {0};
    strncpy(label, t_label, sizeof(label) - 1);

    bool ret = bootloader_common_label_search(list, label);
    if (ret != result) {
        printf("%d) %s  |  %s \n", num_test, list, label);
    }
    TEST_ASSERT_MESSAGE(ret == result, "Test failed");
}

TEST_CASE("Test label_search", "[bootloader_support]")
{
    TEST_ASSERT_FALSE(bootloader_common_label_search(NULL, NULL));
    TEST_ASSERT_FALSE(bootloader_common_label_search("nvs", NULL));

    check_label_search(1,   "nvs",                   "nvs",      true);
    check_label_search(2,   "nvs, ",                 "nvs",      true);
    check_label_search(3,   "nvs1",                  "nvs",      false);
    check_label_search(3,   "nvs1, ",                "nvs",      false);
    check_label_search(4,   "nvs1nvs1, phy",         "nvs1",     false);
    check_label_search(5,   "nvs1, nvs1, phy",       "nvs1",     true);
    check_label_search(6,   "nvs12, nvs12, phy",     "nvs1",     false);
    check_label_search(7,   "nvs12, nvs1, phy",      "nvs1",     true);
    check_label_search(8,   "nvs12, nvs3, phy, nvs1","nvs1",     true);
    check_label_search(9,   "nvs1nvs1, phy, nvs",    "nvs",      true);
    check_label_search(10,  "nvs1nvs1, phy, nvs1",   "nvs",      false);
    check_label_search(11,  "nvs1,  nvs, phy, nvs1", "nvs",      true);
    check_label_search(12,  "nvs1, nvs2,  phy,  nvs","nvs",      true);
    check_label_search(13,  "ota_data, backup_nvs",  "nvs",      false);
    check_label_search(14,  "nvs1, nvs2, ota, nvs",  "vs1",      false);

    check_label_search(20,  "12345678901234, phy, nvs1",    "12345678901234",       true);
    check_label_search(21,  "123456789012345, phy, nvs1",   "123456789012345",      true);
    check_label_search(22,  "1234567890123456, phy, nvs1",  "1234567890123456",     true);
    check_label_search(23,  "12345678901234567, phy, nvs1", "12345678901234567",    false);
    check_label_search(24,  "1234567890123456, phy, nvs1",  "12345678901234567",    true);
    check_label_search(25,  "phy, 1234567890123456, nvs1",  "12345678901234567",    true);

}


TEST_CASE("Test regions_overlap", "[bootloader_support]")
{
    TEST_ASSERT( bootloader_util_regions_overlap(1, 2, 1, 2) );

    TEST_ASSERT( bootloader_util_regions_overlap(1, 2, 0, 2) );
    TEST_ASSERT( bootloader_util_regions_overlap(1, 2, 1, 3) );
    TEST_ASSERT( bootloader_util_regions_overlap(1, 2, 0, 3) );

    TEST_ASSERT( bootloader_util_regions_overlap(0, 2, 1, 2) );
    TEST_ASSERT( bootloader_util_regions_overlap(1, 3, 1, 2) );
    TEST_ASSERT( bootloader_util_regions_overlap(0, 3, 1, 2) );

    TEST_ASSERT( !bootloader_util_regions_overlap(2, 3, 1, 2) );
    TEST_ASSERT( !bootloader_util_regions_overlap(1, 2, 2, 3) );

    TEST_ASSERT( !bootloader_util_regions_overlap(3, 4, 1, 2) );
    TEST_ASSERT( !bootloader_util_regions_overlap(1, 2, 3, 4) );
}
