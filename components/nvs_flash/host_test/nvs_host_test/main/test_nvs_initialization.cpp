/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include "nvs.hpp"
#include "nvs_partition_manager.hpp"
#include "nvs_partition.hpp"
#include "test_fixtures.hpp"
#include <string.h>

TEST_CASE("nvs_flash_init_partition_ptr fails due to nullptr arg", "[nvs_custom_part]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    uint8_t *p_part_desc_addr_start;
    CHECK(esp_partition_file_mmap((const uint8_t **)&p_part_desc_addr_start) == ESP_OK);

    CHECK(nvs_flash_init_partition_ptr(nullptr) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_flash_init_partition_ptr inits one partition", "[nvs_custom_part]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    uint8_t *p_part_desc_addr_start;
    CHECK(esp_partition_file_mmap((const uint8_t **)&p_part_desc_addr_start) == ESP_OK);

    esp_partition_t partition = {};
    strcpy(partition.label, "test");
    partition.address = NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
    partition.size = NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE;

    CHECK(nvs_flash_init_partition_ptr(&partition) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
    CHECK(nvs::NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);
}
