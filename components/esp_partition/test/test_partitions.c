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
    esp_flash_reset_counters();
#endif

    // erase whole partition
    ESP_ERROR_CHECK( esp_partition_erase_range(part, 0, part->size) );

#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
    esp_flash_dump_counters(stdout);
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
    /* This test verifies the function 'esp_partition_get_sha256()' working correctly under the following conditions:
     * - there is only 1 MMU page left for memory mapping (ideal case)
     * - the partition to hash is significantly larger than a common use-case partition size
     * The test case is implemented as follows:
     *      1. SPI Flash space is mmapped by MMU page size chunks, one by one
     *      2. the iteration stops when either whole SPI Flash range is exhausted or the MMU page pool is fully occupied (ESP_ERR_NO_MEM)
     *      3. the last successfully mapped MMU page is released, all the rest remains occupied
     *      4. pseudo partition of DATA type is created over all the SPI Flash capacity
     *      5. esp_partition_get_sha256() is calculated for the partition defined in 4. (printed to standard output on successful completion)
     *      6. all the resources allocated directly by the test are released
     * NOTE: the test is chip-agnostic
     * */

    uint32_t size_flash_chip;
    TEST_ESP_OK(esp_flash_get_size(NULL, &size_flash_chip));
    printf("flash size = %d bytes\n", size_flash_chip);

    uint32_t page_reservation_count = spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
    printf("available page pool = %d pages\n", page_reservation_count);

    spi_flash_mmap_handle_t* handles = malloc(page_reservation_count * sizeof(spi_flash_mmap_handle_t));
    TEST_ASSERT_NOT_NULL(handles);

    const void *ptr = NULL;
    size_t flash_offset = 0;
    size_t mapped_pages_count = 0;

    esp_err_t err = ESP_FAIL;
    for (; mapped_pages_count<page_reservation_count && flash_offset<size_flash_chip; mapped_pages_count++, flash_offset+=SPI_FLASH_MMU_PAGE_SIZE) {
        err = spi_flash_mmap(flash_offset, SPI_FLASH_MMU_PAGE_SIZE, SPI_FLASH_MMAP_DATA, &ptr, &handles[mapped_pages_count]);
        if (err != ESP_OK) break;
        TEST_ASSERT_NOT_NULL(ptr);
        ptr = NULL;
    }

    if (err == ESP_OK || err == ESP_ERR_NO_MEM) {
        TEST_ASSERT(mapped_pages_count>0);
        mapped_pages_count--;
        spi_flash_munmap(handles[mapped_pages_count]);
    }
    else {
        TEST_ESP_OK(err);
    }

    esp_partition_t partition = {
            .address = 0x00000000,
            .size = size_flash_chip,
            .type = ESP_PARTITION_TYPE_DATA
    };

    uint8_t sha256[32] = {0};
    TEST_ESP_OK(esp_partition_get_sha256(&partition, sha256));
    ESP_LOG_BUFFER_HEX("sha", sha256, sizeof(sha256));

    for(size_t y=0; y<mapped_pages_count; y++) {
        spi_flash_munmap(handles[y]);
    }

    free(handles);
}
