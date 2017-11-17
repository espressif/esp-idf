/*
 * Tests for bootloader_support esp_load(ESP_IMAGE_VERIFY, ...)
 */

#include <esp_types.h>
#include <stdio.h>
#include "rom/ets_sys.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"

#include "esp_partition.h"
#include "esp_ota_ops.h"
#include "esp_image_format.h"

TEST_CASE("Verify bootloader image in flash", "[bootloader_support]")
{
    const esp_partition_pos_t fake_bootloader_partition = {
        .offset = ESP_BOOTLOADER_OFFSET,
        .size = ESP_PARTITION_TABLE_OFFSET - ESP_BOOTLOADER_OFFSET,
    };
    esp_image_metadata_t data = { 0 };
    TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_image_load(ESP_IMAGE_VERIFY, &fake_bootloader_partition, &data));
    TEST_ASSERT_NOT_EQUAL(0, data.image_len);

    uint32_t bootloader_length = 0;
    TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_image_verify_bootloader(&bootloader_length));
    TEST_ASSERT_EQUAL(data.image_len, bootloader_length);
}

TEST_CASE("Verify unit test app image", "[bootloader_support]")
{
    esp_image_metadata_t data = { 0 };
    const esp_partition_t *running = esp_ota_get_running_partition();
    TEST_ASSERT_NOT_EQUAL(NULL, running);
    const esp_partition_pos_t running_pos  = {
        .offset = running->address,
        .size = running->size,
    };

    TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_image_load(ESP_IMAGE_VERIFY, &running_pos, &data));
    TEST_ASSERT_NOT_EQUAL(0, data.image_len);
    TEST_ASSERT_TRUE(data.image_len <= running->size);
}

