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

// Test for spi_flash_{read,write}.

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>

#include <unity.h>
#include <test_utils.h>
#include <esp_partition.h>
#include <esp_attr.h>

TEST_CASE("Test erase partition", "[spi_flash]")
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
