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

using namespace nvs;

TEST_CASE("Partition manager initializes storage", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(NVSPartitionManager::get_instance()->init_custom(&f.part, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
    REQUIRE(NVSPartitionManager::get_instance()->deinit_partition(f.part.get_partition_name()) == ESP_OK);
}

TEST_CASE("Partition manager de-initializes storage", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(NVSPartitionManager::get_instance()->init_custom(&f.part, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
    CHECK(NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->lookup_storage_from_name("test") == nullptr);
}

TEST_CASE("Partition manager initializes multiple partitions", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);
    PartitionEmulation part_0(&emu, NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE, NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE, "test1");
    PartitionEmulation part_1(&emu, NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE, NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE, "test2");

    REQUIRE(NVSPartitionManager::get_instance()->init_custom(&part_0, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);
    // TODO: why does this work, actually? same sectors used as above
    REQUIRE(NVSPartitionManager::get_instance()->init_custom(&part_1, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);
    Storage *storage1 = NVSPartitionManager::get_instance()->lookup_storage_from_name("test1");
    REQUIRE(storage1 != nullptr);
    Storage *storage2 = NVSPartitionManager::get_instance()->lookup_storage_from_name("test2");
    REQUIRE(storage2 != nullptr);

    CHECK(storage1 != storage2);
    REQUIRE(NVSPartitionManager::get_instance()->deinit_partition(part_0.get_partition_name()) == ESP_OK);
    REQUIRE(NVSPartitionManager::get_instance()->deinit_partition(part_1.get_partition_name()) == ESP_OK);
}

TEST_CASE("Partition manager open fails on null handle", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(NVSPartitionManager::get_instance()->init_custom(&f.part, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, nullptr)
            == ESP_ERR_INVALID_ARG);

    NVSPartitionManager::get_instance()->deinit_partition("test");
}

TEST_CASE("Partition manager invalidates handle on partition de-init", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(NVSPartitionManager::get_instance()->init_custom(&f.part, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    NVSHandleSimple *handle;
    REQUIRE(NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle) == ESP_OK);
    CHECK(handle->erase_all() == ESP_OK);

    REQUIRE(NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);

    CHECK(handle->erase_all() == ESP_ERR_NVS_INVALID_HANDLE);

    delete handle;
}
