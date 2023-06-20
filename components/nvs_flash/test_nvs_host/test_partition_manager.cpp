/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include <algorithm>
#include <cstring>
#include "nvs_test_api.h"
#include "nvs_handle_simple.hpp"
#include "nvs_partition_manager.hpp"
#include "spi_flash_emulation.h"
#include "nvs_test_api.h"
#include "test_fixtures.hpp"

/*
TEST_CASE("Partition manager initializes multiple partitions", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);
    PartitionEmulation part_0(&emu, NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE, NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE, "test1");
    PartitionEmulation part_1(&emu, NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE, NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE, "test2");

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(&part_0, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);
    // TODO: why does this work, actually? same sectors used as above
    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(&part_1, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);
    nvs::Storage *storage1 = nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name("test1");
    REQUIRE(storage1 != nullptr);
    nvs::Storage *storage2 = nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name("test2");
    REQUIRE(storage2 != nullptr);

    CHECK(storage1 != storage2);
    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(part_0.get_partition_name()) == ESP_OK);
    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(part_1.get_partition_name()) == ESP_OK);
}
*/
