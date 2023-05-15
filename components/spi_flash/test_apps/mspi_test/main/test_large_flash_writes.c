/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Test for spi_flash_write() with large buffers (in RAM or on flash)

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>

#include "unity.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "esp_rom_spiflash.h"
#include "esp_private/cache_utils.h"
#include "soc/timer_periph.h"
#include "esp_flash.h"
#include "esp_partition.h"

static const uint8_t large_const_buffer[16400] = {
    203, // first byte
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,
    [50 ... 99] = 2,
    [1600 ... 2000] = 3,
    [8000 ... 9000] = 77,
    [15000 ... 16398] = 8,
    43 // last byte
};

static void test_write_large_buffer(const uint8_t *source, size_t length);

const esp_partition_t *get_test_data_partition(void)
{
    /* This finds "flash_test" partition defined in partition_table_unit_test_app.csv */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_ANY, "flash_test");
    TEST_ASSERT_NOT_NULL(result); /* means partition table set wrong */
    return result;
}

TEST_CASE("Test flash write large const buffer", "[spi_flash][esp_flash]")
{
    // buffer in flash
    test_write_large_buffer(large_const_buffer, sizeof(large_const_buffer));
}

TEST_CASE("Test flash write large RAM buffer", "[spi_flash][esp_flash]")
{
    // buffer in RAM
    uint8_t *source_buf = malloc(sizeof(large_const_buffer));
    TEST_ASSERT_NOT_NULL(source_buf);
    memcpy(source_buf, large_const_buffer, sizeof(large_const_buffer));
    test_write_large_buffer(source_buf, sizeof(large_const_buffer));
    free(source_buf);
}

static void test_write_large_buffer(const uint8_t *source, size_t length)
{
    const esp_partition_t *part = get_test_data_partition();
    TEST_ASSERT(part->size > length + 2 + SPI_FLASH_SEC_SIZE);

    printf("Writing %d bytes from source %p\n", length, source);

    uint8_t *buf = malloc(length);
    TEST_ASSERT_NOT_NULL(buf);

    TEST_ESP_OK( esp_flash_erase_region(NULL, part->address, (length + SPI_FLASH_SEC_SIZE) & ~(SPI_FLASH_SEC_SIZE-1)) );

    // note writing to unaligned address
    TEST_ESP_OK( esp_flash_write(NULL, source, part->address + 1, length) );

    TEST_ESP_OK( esp_flash_read(NULL, buf, part->address + 1, length) );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(source, buf, length);

    free(buf);

    // check nothing was written at beginning or end
    uint8_t ends[8];

    TEST_ESP_OK( esp_flash_read(NULL, ends, part->address, sizeof(ends)) );
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[0]);
    TEST_ASSERT_EQUAL_HEX8(source[0] , ends[1]);

    TEST_ESP_OK( esp_flash_read(NULL, ends, part->address + length, sizeof(ends)) );
    TEST_ASSERT_EQUAL_HEX8(source[length-1], ends[0]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[1]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[2]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[3]);
}
