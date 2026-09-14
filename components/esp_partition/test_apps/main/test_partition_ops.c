/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_flash.h"
#include "esp_image_format.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_task_wdt.h"
#include "spi_flash_mmap.h"
#include "unity.h"
#include "unity_fixture.h"

/* Same Unity group as test_part_app.c; setup/teardown live there. */
TEST_GROUP(esp_partition);
void TEST_esp_partition_SETUP(void);
void TEST_esp_partition_TEAR_DOWN(void);

static const esp_partition_t *s_get_data_partition(void)
{
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                          ESP_PARTITION_SUBTYPE_ANY,
                                                          "storage1");
    TEST_ASSERT_NOT_NULL(part);
    return part;
}

static bool s_test_nonzero_sha_of_partition(const esp_partition_t *part, bool allow_invalid_image)
{
    uint8_t sha256[32] = {0};

    TEST_ASSERT_NOT_NULL(part);

    esp_err_t err = esp_partition_get_sha256(part, sha256);

    if (allow_invalid_image && err == ESP_ERR_IMAGE_INVALID) {
        printf("App partition at 0x%" PRIx32 " doesn't hold a valid app\n", part->address);
        return false;
    }

    TEST_ESP_OK(err);
    ESP_LOG_BUFFER_HEX("sha", sha256, sizeof(sha256));

    for (size_t i = 0; i < sizeof(sha256); i++) {
        if (sha256[i] != 0) {
            return true;
        }
    }
    TEST_FAIL_MESSAGE("SHA-256 of partition should not be all zeroes");
    abort();
}

TEST(esp_partition, test_erase_partition)
{
    const esp_partition_t *part = s_get_data_partition();

    TEST_ESP_OK(esp_partition_erase_range(part, 0, part->size));

    const static DRAM_ATTR char some_data[] = "abcdefghijklmn";
    const size_t data_len = strlen(some_data);
    for (size_t i = 0; i < part->size; i += 4096) {
        TEST_ESP_OK(esp_partition_write(part, i, some_data, data_len));
    }

    char buf[sizeof(some_data)];
    for (size_t i = 0; i < part->size; i += 4096) {
        memset(buf, 0x00, sizeof(buf));
        TEST_ESP_OK(esp_partition_read(part, i, buf, data_len));
        TEST_ASSERT_EQUAL_INT(0, strncmp(buf, some_data, data_len));
    }

    TEST_ESP_OK(esp_partition_erase_range(part, 0, part->size));

    for (size_t i = 0; i < part->size; i += 4096) {
        memset(buf, 0x00, sizeof(buf));
        TEST_ESP_OK(esp_partition_read(part, i, buf, data_len));
        for (size_t j = 0; j < data_len; j++) {
            TEST_ASSERT_EQUAL_HEX8(0xFF, buf[j]);
        }
    }
}

TEST(esp_partition, test_get_sha256_data)
{
    const esp_partition_t *part = s_get_data_partition();
    TEST_ASSERT_TRUE(s_test_nonzero_sha_of_partition(part, false));
}

TEST(esp_partition, test_get_sha256_app)
{
    bool found_valid_app = false;
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_APP,
                                                     ESP_PARTITION_SUBTYPE_ANY,
                                                     NULL);
    TEST_ASSERT_NOT_NULL(it);

    while (it != NULL) {
        const esp_partition_t *part = esp_partition_get(it);
        printf("Hashing app partition at 0x%" PRIx32 "\n", part->address);
        found_valid_app |= s_test_nonzero_sha_of_partition(part, true);
        it = esp_partition_next(it);
    }

    TEST_ASSERT_MESSAGE(found_valid_app, "At least one app partition should be a valid app partition");
}

TEST(esp_partition, test_get_sha256_big_partition)
{
    /* Leave one MMU page free, then hash a partition spanning the whole flash. */
    uint32_t size_flash_chip;
    TEST_ESP_OK(esp_flash_get_size(NULL, &size_flash_chip));
    printf("flash size = %" PRIu32 " bytes\n", size_flash_chip);

    uint32_t page_reservation_count = spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
    printf("available page pool = %" PRIu32 " pages\n", page_reservation_count);

    spi_flash_mmap_handle_t *handles = malloc(page_reservation_count * sizeof(spi_flash_mmap_handle_t));
    TEST_ASSERT_NOT_NULL(handles);

    const void *ptr = NULL;
    size_t flash_offset = 0;
    size_t mapped_pages_count = 0;

    esp_err_t err = ESP_FAIL;
    for (; mapped_pages_count < page_reservation_count && flash_offset < size_flash_chip;
         mapped_pages_count++, flash_offset += SPI_FLASH_MMU_PAGE_SIZE) {
        err = spi_flash_mmap(flash_offset, SPI_FLASH_MMU_PAGE_SIZE,
                             SPI_FLASH_MMAP_FLAG_DATA | SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE,
                             &ptr, &handles[mapped_pages_count]);
        if (err != ESP_OK) {
            break;
        }
        TEST_ASSERT_NOT_NULL(ptr);
        ptr = NULL;
    }

    if (err == ESP_OK || err == ESP_ERR_NO_MEM) {
        TEST_ASSERT(mapped_pages_count > 0);
        mapped_pages_count--;
        spi_flash_munmap(handles[mapped_pages_count]);
    } else {
        TEST_ESP_OK(err);
    }

    esp_partition_t partition = {
        .address = 0x00000000,
        .size = size_flash_chip,
        .type = ESP_PARTITION_TYPE_DATA,
    };

    uint8_t sha256[32] = {0};
#if CONFIG_ESP_TASK_WDT_EN
    /* Whole-flash SHA with most MMU pages held can exceed the default TWDT
     * (idle does not run during this loop). Bump timeout for this case only. */
    const esp_task_wdt_config_t twdt_cfg = {
        .timeout_ms = 60 * 1000,
        .idle_core_mask = (1U << CONFIG_FREERTOS_NUMBER_OF_CORES) - 1U,
        .trigger_panic = true,
    };
    TEST_ESP_OK(esp_task_wdt_reconfigure(&twdt_cfg));
#endif
    TEST_ESP_OK(esp_partition_get_sha256(&partition, sha256));
    ESP_LOG_BUFFER_HEX("sha", sha256, sizeof(sha256));

    for (size_t y = 0; y < mapped_pages_count; y++) {
        spi_flash_munmap(handles[y]);
    }

    free(handles);
}
