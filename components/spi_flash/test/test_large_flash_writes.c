// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Test for spi_flash_write() with large buffers (in RAM or on flash)

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>

#include <unity.h>
#include <test_utils.h>
#include <esp_spi_flash.h>
#include <esp_log.h>
#include <rom/spi_flash.h>
#include "../cache_utils.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

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

TEST_CASE("Test spi_flash_write large const buffer", "[spi_flash]")
{
    // buffer in flash
    test_write_large_buffer(large_const_buffer, sizeof(large_const_buffer));
}

TEST_CASE("Test spi_flash_write large RAM buffer", "[spi_flash]")
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

    ESP_ERROR_CHECK( spi_flash_erase_range(part->address, (length + SPI_FLASH_SEC_SIZE) & ~(SPI_FLASH_SEC_SIZE-1)) );

    // note writing to unaligned address
    ESP_ERROR_CHECK( spi_flash_write(part->address + 1, source, length) );

    ESP_ERROR_CHECK( spi_flash_read(part->address + 1, buf, length) );

    TEST_ASSERT_EQUAL_HEX8_ARRAY(source, buf, length);

    free(buf);

    // check nothing was written at beginning or end
    uint8_t ends[8];

    ESP_ERROR_CHECK( spi_flash_read(part->address, ends, sizeof(ends)) );
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[0]);
    TEST_ASSERT_EQUAL_HEX8(source[0] , ends[1]);

    ESP_ERROR_CHECK( spi_flash_read(part->address + length, ends, sizeof(ends)) );

    TEST_ASSERT_EQUAL_HEX8(source[length-1], ends[0]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[1]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[2]);
    TEST_ASSERT_EQUAL_HEX8(0xFF, ends[3]);
}
