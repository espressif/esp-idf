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

#include <iostream>

using namespace std;
using namespace nvs;

TEST_CASE("NVSHandleSimple closes its reference in PartitionManager", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    REQUIRE(NVSPartitionManager::get_instance()->init_custom("test", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 0);

    NVSHandleSimple *handle;
    REQUIRE(NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle;

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 0);

    REQUIRE(NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);

}

TEST_CASE("NVSHandleSimple multiple open and closes with PartitionManager", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    REQUIRE(NVSPartitionManager::get_instance()->init_custom("test", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 0);

    NVSHandleSimple *handle1;
    NVSHandleSimple *handle2;

    REQUIRE(NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle1) == ESP_OK);

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 1);

    REQUIRE(NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle2) == ESP_OK);

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 2);

    delete handle1;

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle2;

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 0);

    REQUIRE(NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);

}

TEST_CASE("nvshandle readonly fails", "[partition_mgr]")
{
    SpiFlashEmulator emu(10);

    NVSPartitionManager::get_instance()->deinit_partition(NVS_DEFAULT_PART_NAME);
    NVSHandleSimple *handle_1;
    NVSHandleSimple *handle_2;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    CHECK(NVSPartitionManager::get_instance()->init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 0);

    // first, creating namespace...
    REQUIRE(NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READWRITE, &handle_1) == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle_1;

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 0);
    REQUIRE(NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READONLY, &handle_2) == ESP_OK);
    CHECK(handle_2->set_item("key", 47) == ESP_ERR_NVS_READ_ONLY);
    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle_2;

    CHECK(NVSPartitionManager::get_instance()->open_handles_size() == 0);
    // without deinit it affects "nvs api tests"
    CHECK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME) == ESP_OK);
}

