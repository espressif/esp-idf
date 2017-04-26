/*
 * Tests for bootloader_support esp_image_basic_verify()
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
    uint32_t image_len = 0;
    TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_image_basic_verify(0x1000, true, &image_len));
    TEST_ASSERT_NOT_EQUAL(0, image_len);
}

TEST_CASE("Verify unit test app image", "[bootloader_support]")
{
    uint32_t image_len = 0;
    const esp_partition_t *running = esp_ota_get_running_partition();
    TEST_ASSERT_NOT_EQUAL(NULL, running);

    TEST_ASSERT_EQUAL_HEX(ESP_OK, esp_image_basic_verify(running->address, true, &image_len));
    TEST_ASSERT_NOT_EQUAL(0, image_len);
    TEST_ASSERT_TRUE(image_len <= running->size);
}

