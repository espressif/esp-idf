// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "catch.hpp"
#include <algorithm>
#include <cstring>
#include "nvs_test_api.h"
#include "nvs_handle_simple.hpp"
#include "nvs_partition_manager.hpp"
#include "spi_flash_emulation.h"
#include "nvs_test_api.h"

using namespace nvs;

TEST_CASE("Partition manager initializes storage", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    REQUIRE(NVSPartitionManager::get_instance()->init_custom("test", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
}

TEST_CASE("Partition manager de-initializes storage", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    REQUIRE(NVSPartitionManager::get_instance()->init_custom("test", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
    CHECK(NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->lookup_storage_from_name("test") == nullptr);
}

TEST_CASE("Partition manager initializes multiple partitions", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    REQUIRE(NVSPartitionManager::get_instance()->init_custom("test1", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);
    // TODO: why does this work, actually? same sectors used as above
    REQUIRE(NVSPartitionManager::get_instance()->init_custom("test2", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);
    Storage *storage1 = NVSPartitionManager::get_instance()->lookup_storage_from_name("test1");
    REQUIRE(storage1 != nullptr);
    Storage *storage2 = NVSPartitionManager::get_instance()->lookup_storage_from_name("test2");
    REQUIRE(storage2 != nullptr);

    CHECK(storage1 != storage2);
}

TEST_CASE("Partition manager invalidates handle on partition de-init", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    REQUIRE(NVSPartitionManager::get_instance()->init_custom("test", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    NVSHandleSimple *handle;
    REQUIRE(NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle) == ESP_OK);
    CHECK(handle->erase_all() == ESP_OK);

    REQUIRE(NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);

    CHECK(handle->erase_all() == ESP_ERR_NVS_INVALID_HANDLE);

    delete handle;
}

