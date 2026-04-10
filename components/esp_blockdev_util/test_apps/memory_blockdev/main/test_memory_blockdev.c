/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "unity.h"
#include "unity_test_utils.h"

#include "esp_blockdev.h"
#include "esp_blockdev/memory.h"
#include "esp_heap_caps.h"

TEST_CASE("memory blockdev basic operations", "[memory_blockdev]")
{
    uint8_t backing[128];
    memset(backing, 0x5A, sizeof(backing));

    const esp_blockdev_geometry_t geometry = {
        .disk_size = sizeof(backing),
        .read_size = 1,
        .write_size = 1,
        .erase_size = 1,
        .recommended_write_size = 16,
        .recommended_read_size = 16,
        .recommended_erase_size = 32,
    };

    esp_blockdev_handle_t dev = NULL;
    TEST_ESP_OK(esp_blockdev_memory_get_from_buffer(backing, sizeof(backing), &geometry, false, &dev));
    TEST_ASSERT_NOT_NULL(dev);

    TEST_ASSERT_EQUAL_UINT64(geometry.disk_size, dev->geometry.disk_size);
    TEST_ASSERT_EQUAL_UINT32(geometry.read_size, dev->geometry.read_size);
    TEST_ASSERT_EQUAL_UINT32(geometry.write_size, dev->geometry.write_size);
    TEST_ASSERT_EQUAL_UINT32(geometry.erase_size, dev->geometry.erase_size);
    TEST_ASSERT_FALSE(dev->device_flags.read_only);
    TEST_ASSERT_FALSE(dev->device_flags.default_val_after_erase);

    uint8_t write_data[32];
    for (size_t i = 0; i < sizeof(write_data); ++i) {
        write_data[i] = (uint8_t)(i + 1);
    }

    TEST_ESP_OK(dev->ops->write(dev, write_data, 48, sizeof(write_data)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(write_data, backing + 48, sizeof(write_data));

    uint8_t read_buf[sizeof(write_data)];
    memset(read_buf, 0, sizeof(read_buf));
    TEST_ESP_OK(dev->ops->read(dev, read_buf, sizeof(read_buf), 48, sizeof(read_buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(write_data, read_buf, sizeof(write_data));

    TEST_ESP_OK(dev->ops->erase(dev, 48, sizeof(write_data)));
    TEST_ASSERT_EACH_EQUAL_UINT8(0x00, backing + 48, sizeof(write_data));

    TEST_ESP_OK(dev->ops->sync(dev));
    TEST_ESP_ERR(ESP_ERR_NOT_SUPPORTED, dev->ops->ioctl(dev, ESP_BLOCKDEV_CMD_USER_BASE, NULL));

    TEST_ESP_OK(dev->ops->release(dev));
}

TEST_CASE("memory blockdev enforces read-only mappings", "[memory_blockdev]")
{
    uint8_t backing[64];
    for (size_t i = 0; i < sizeof(backing); ++i) {
        backing[i] = (uint8_t)(i ^ 0xAA);
    }

    const esp_blockdev_geometry_t ro_geometry = {
        .disk_size = sizeof(backing),
        .read_size = 1,
        .write_size = 0,
        .erase_size = 0,
        .recommended_write_size = 0,
        .recommended_read_size = 8,
        .recommended_erase_size = 0,
    };

    esp_blockdev_handle_t dev = NULL;
    TEST_ESP_OK(esp_blockdev_memory_get_from_buffer(backing, sizeof(backing), &ro_geometry, true, &dev));
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_TRUE(dev->device_flags.read_only);

    uint8_t read_buf[16];
    TEST_ESP_OK(dev->ops->read(dev, read_buf, sizeof(read_buf), 0, sizeof(read_buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(backing, read_buf, sizeof(read_buf));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, dev->ops->write(dev, read_buf, 0, sizeof(read_buf)));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, dev->ops->erase(dev, 0, sizeof(read_buf)));

    TEST_ESP_OK(dev->ops->release(dev));

    /* Release must not take ownership of external buffers */
    const uint8_t expected_prefix[] = {0xAA, 0xAB, 0xA8, 0xA9};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_prefix, backing, sizeof(expected_prefix));
}

TEST_CASE("memory blockdev validates arguments", "[memory_blockdev]")
{
    uint8_t backing[32];
    memset(backing, 0x11, sizeof(backing));

    esp_blockdev_geometry_t geometry = {
        .disk_size = sizeof(backing),
        .read_size = 1,
        .write_size = 1,
        .erase_size = 1,
        .recommended_write_size = 0,
        .recommended_read_size = 0,
        .recommended_erase_size = 0,
    };

    esp_blockdev_handle_t dev = (esp_blockdev_handle_t)0xDEADBEEF;

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_blockdev_memory_get_from_buffer(NULL, sizeof(backing), &geometry, false, &dev));
    TEST_ASSERT_EQUAL_PTR(NULL, dev);

    dev = (esp_blockdev_handle_t)0xDEADBEEF;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_blockdev_memory_get_from_buffer(backing, sizeof(backing), NULL, false, &dev));
    TEST_ASSERT_EQUAL_PTR(NULL, dev);

    geometry.disk_size = sizeof(backing) + 1;
    dev = (esp_blockdev_handle_t)0xDEADBEEF;
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, esp_blockdev_memory_get_from_buffer(backing, sizeof(backing), &geometry, false, &dev));
    TEST_ASSERT_EQUAL_PTR(NULL, dev);

    geometry.disk_size = 0;
    dev = (esp_blockdev_handle_t)0xDEADBEEF;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_blockdev_memory_get_from_buffer(backing, sizeof(backing), &geometry, false, &dev));
    TEST_ASSERT_EQUAL_PTR(NULL, dev);

    geometry.disk_size = sizeof(backing);
    esp_blockdev_handle_t heap_dev = NULL;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_blockdev_memory_create_with_heap_caps(NULL, MALLOC_CAP_8BIT, &heap_dev));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_blockdev_memory_create_with_heap_caps(&geometry, MALLOC_CAP_8BIT, NULL));

    /* Successful create for subsequent bounds checks */
    TEST_ESP_OK(esp_blockdev_memory_get_from_buffer(backing, sizeof(backing), &geometry, false, &dev));
    TEST_ASSERT_NOT_NULL(dev);

    uint8_t read_buf[16];
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE, dev->ops->read(dev, read_buf, sizeof(read_buf) - 1, 0, sizeof(read_buf)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, dev->ops->read(dev, read_buf, sizeof(read_buf), sizeof(backing) - 4, sizeof(read_buf)));

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, dev->ops->write(dev, read_buf, sizeof(backing) - 8, sizeof(read_buf) + 1));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, dev->ops->erase(dev, sizeof(backing) - 4, sizeof(read_buf)));

    TEST_ESP_OK(dev->ops->release(dev));
}

TEST_CASE("memory blockdev heap allocation creates zeroed storage", "[memory_blockdev]")
{
    const esp_blockdev_geometry_t geometry = {
        .disk_size = 64,
        .read_size = 1,
        .write_size = 1,
        .erase_size = 1,
        .recommended_write_size = 0,
        .recommended_read_size = 0,
        .recommended_erase_size = 0,
    };

    esp_blockdev_handle_t dev = NULL;
    TEST_ESP_OK(esp_blockdev_memory_create_with_heap_caps(&geometry, MALLOC_CAP_8BIT, &dev));
    TEST_ASSERT_NOT_NULL(dev);
    TEST_ASSERT_FALSE(dev->device_flags.read_only);
    TEST_ASSERT_FALSE(dev->device_flags.default_val_after_erase);

    uint8_t read_buf[64];
    TEST_ESP_OK(dev->ops->read(dev, read_buf, sizeof(read_buf), 0, sizeof(read_buf)));
    TEST_ASSERT_EACH_EQUAL_UINT8(0x00, read_buf, sizeof(read_buf));

    uint8_t pattern[8];
    memset(pattern, 0xCD, sizeof(pattern));

    TEST_ESP_OK(dev->ops->write(dev, pattern, 4, sizeof(pattern)));
    TEST_ESP_OK(dev->ops->read(dev, read_buf, sizeof(pattern), 4, sizeof(pattern)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(pattern, read_buf, sizeof(pattern));

    TEST_ESP_OK(dev->ops->release(dev));
}

void app_main(void)
{
    unity_run_menu();
}
