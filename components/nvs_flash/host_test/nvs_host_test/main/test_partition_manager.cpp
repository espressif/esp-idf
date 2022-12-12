/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include <algorithm>
#include <cstring>
#include "nvs_handle_simple.hpp"
#include "nvs_partition_manager.hpp"
#include "nvs_test_api.h"
#include "test_fixtures.hpp"

TEST_CASE("Partition manager initializes storage", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(f.part()->get_partition_name()) == ESP_OK);
}

TEST_CASE("Partition manager de-initializes storage", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
    CHECK(nvs::NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name("test") == nullptr);
}

TEST_CASE("Partition manager open fails on null handle", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, nullptr)
          == ESP_ERR_INVALID_ARG);

    nvs::NVSPartitionManager::get_instance()->deinit_partition("test");
}

TEST_CASE("Partition manager invalidates handle on partition de-init", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    nvs::NVSHandleSimple *handle;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle) == ESP_OK);
    CHECK(handle->erase_all() == ESP_OK);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);

    CHECK(handle->erase_all() == ESP_ERR_NVS_INVALID_HANDLE);

    delete handle;
}
