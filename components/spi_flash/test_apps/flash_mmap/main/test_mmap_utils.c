/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <inttypes.h>
#include <esp_partition.h>
#include <esp_efuse.h>

#include "test_utils.h"
#include "test_mmap_utils.h"
#include "esp_flash.h"


/* read-only region used for mmap tests, initialised in setup_mmap_tests() */
uint32_t test_start;
uint32_t test_end;

spi_flash_mmap_handle_t handle1, handle2, handle3;

static uint32_t buffer[1024];


esp_err_t spi_flash_read_maybe_encrypted(size_t src_addr, void *des_addr, size_t size)
{
    if (!esp_efuse_is_flash_encryption_enabled()) {
        return esp_flash_read(NULL, des_addr, src_addr, size);
    } else {
        return esp_flash_read_encrypted(NULL, src_addr, des_addr, size);
    }
}

esp_err_t spi_flash_write_maybe_encrypted(size_t des_addr, const void *src_addr, size_t size)
{
    if (!esp_efuse_is_flash_encryption_enabled()) {
        return esp_flash_write(NULL, src_addr, des_addr, size);
    } else {
        return esp_flash_write_encrypted(NULL, des_addr, src_addr, size);
    }
}

void setup_mmap_tests(void)
{
    if (test_start == 0) {
        const esp_partition_t *part = get_test_data_partition();
        test_start = part->address;
        test_end = part->address + part->size;
        printf("Test data partition @ 0x%"PRIx32" - 0x%"PRIx32"\n", test_start, test_end);
    }
    TEST_ASSERT(test_end > test_start);
    TEST_ASSERT(test_end - test_start >= 512 * 1024);

    /* clean up any mmap handles left over from failed tests */
    if (handle1) {
        spi_flash_munmap(handle1);
        handle1 = 0;
    }
    if (handle2) {
        spi_flash_munmap(handle2);
        handle2 = 0;
    }
    if (handle3) {
        spi_flash_munmap(handle3);
        handle3 = 0;
    }

    /* prepare flash contents */
    srand(0);
    for (int block = test_start / 0x10000; block < test_end / 0x10000; ++block) {
        for (int sector = 0; sector < 16; ++sector) {
            uint32_t abs_sector = (block * 16) + sector;
            uint32_t sector_offs = abs_sector * SPI_FLASH_SEC_SIZE;
            bool sector_needs_write = false;

            TEST_ESP_OK( spi_flash_read_maybe_encrypted(sector_offs, buffer, sizeof(buffer)) );

            for (uint32_t word = 0; word < 1024; ++word) {
                uint32_t val = rand();
                if (block == test_start / 0x10000 && sector == 0 && word == 0) {
                    printf("setup_mmap_tests(): first prepped word: 0x%08"PRIx32" (flash holds 0x%08"PRIx32")\n", val, buffer[word]);
                }
                if (buffer[word] != val) {
                    buffer[word] = val;
                    sector_needs_write = true;
                }
            }
            /* Only rewrite the sector if it has changed */
            if (sector_needs_write) {
                TEST_ESP_OK( esp_flash_erase_region(NULL, (uint16_t) abs_sector * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE) );
                TEST_ESP_OK( spi_flash_write_maybe_encrypted(sector_offs, (const uint8_t *) buffer, sizeof(buffer)) );
            }
        }
    }
}
