/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Test for spi_flash_{read,write}.

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include <unity.h>
#include <test_utils.h>
#include <esp_image_format.h>
#include <esp_log.h>
#include <esp_partition.h>
#include <esp_attr.h>
#include "esp_flash.h"
#include "spi_flash_mmap.h"

TEST_CASE("Test erase partition", "[spi_flash][esp_flash]")
{
    const esp_partition_t *part = get_test_data_partition();

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
    spi_flash_reset_counters();
#endif

    // erase whole partition
    ESP_ERROR_CHECK( esp_partition_erase_range(part, 0, part->size) );

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
    spi_flash_dump_counters();
#endif

    // put some dummy data on sector boundaries
    const static DRAM_ATTR char some_data[] = "abcdefghijklmn";
    for (int i = 0; i < part->size; i+= 4096) {
        ESP_ERROR_CHECK( esp_partition_write(part, i, some_data, strlen(some_data)) );
    }

    // check it's there!
    char buf[strlen(some_data)];
    for (int i = 0; i < part->size; i+= 4096) {
        memset(buf, 0x00, sizeof(buf));
        ESP_ERROR_CHECK( esp_partition_read(part, i, buf, sizeof(buf)) );
        TEST_ASSERT_EQUAL_INT(0, strncmp(buf, some_data, sizeof(buf)));
    }

    // erase the whole thing again
    ESP_ERROR_CHECK( esp_partition_erase_range(part, 0, part->size) );

    // check it's gone
    for (int i = 0; i < part->size; i+= 4096) {
        memset(buf, 0x00, sizeof(buf));
        ESP_ERROR_CHECK( esp_partition_read(part, i, buf, sizeof(buf)) );
        for (int i = 0; i < sizeof(buf); i++) {
            TEST_ASSERT_EQUAL_HEX8(0xFF, buf[i]);
        }
    }
}

static bool s_test_nonzero_sha_of_partition(const esp_partition_t *part, bool allow_invalid_image)
{
    uint8_t sha256[32] = { 0 };

    TEST_ASSERT_NOT_NULL(part);

    esp_err_t err = esp_partition_get_sha256(part, sha256);

    if (allow_invalid_image && err == ESP_ERR_IMAGE_INVALID) {
        printf("App partition at 0x%x doesn't hold a valid app\n", part->address);
        return false;
    }

    // Otherwise, err should be ESP_OK
    ESP_ERROR_CHECK(err);

    ESP_LOG_BUFFER_HEX("sha", sha256, sizeof(sha256));

    for (int i = 0; i < sizeof(sha256); i++) {
        if (sha256[i] != 0) {
            return true; // At least one non-zero byte!
        }
    }
    TEST_FAIL_MESSAGE("SHA-256 of data partition should not be all zeroes");
    abort(); // unreachable
}

TEST_CASE("Test esp_partition_get_sha256() with data", "[spi_flash]")
{
    const esp_partition_t *part = get_test_data_partition();
    s_test_nonzero_sha_of_partition(part, false);
}

TEST_CASE("Test esp_partition_get_sha256() with app", "[spi_flash]")
{
    bool found_valid_app = false;
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP,
                                                     ESP_PARTITION_SUBTYPE_ANY,
                                                     NULL);
    TEST_ASSERT_NOT_NULL(it); /* has to be at least one app partition */

    while (it != NULL) {
        const esp_partition_t *part = esp_partition_get(it);
        printf("Hashing app partition at 0x%x\n", part->address);
        bool valid = s_test_nonzero_sha_of_partition(part, true);
        found_valid_app |= valid;

        it = esp_partition_next(it);
    }

    TEST_ASSERT_MESSAGE(found_valid_app, "At least one app partition should be a valid app partition");
}

TEST_CASE("Test esp_partition_get_sha256() that it can handle a big partition", "[spi_flash]")
{
    esp_partition_t partition;
    const void *ptr;
    spi_flash_mmap_handle_t handle;

    uint8_t sha256[32] = { 0 };
    uint32_t size_flash_chip;
    esp_flash_get_size(NULL, &size_flash_chip);

    printf("size_flash_chip = %d bytes\n", size_flash_chip);

    ESP_ERROR_CHECK(spi_flash_mmap(0x00000000, size_flash_chip * 7 / 10, SPI_FLASH_MMAP_DATA, &ptr, &handle));
    TEST_ASSERT_NOT_NULL(ptr);

    partition.address   = 0x00000000;
    partition.size      = size_flash_chip;
    partition.type      = ESP_PARTITION_TYPE_DATA;

    ESP_ERROR_CHECK(esp_partition_get_sha256(&partition, sha256));
    ESP_LOG_BUFFER_HEX("sha", sha256, sizeof(sha256));

    spi_flash_munmap(handle);
}
