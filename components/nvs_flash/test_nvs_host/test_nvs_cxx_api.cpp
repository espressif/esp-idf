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

TEST_CASE("NVSHandleSimple CXX api open invalid arguments", "[nvs cxx]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);
    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->
            init_custom("test", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);

    handle = nvs::open_nvs_handle_from_partition(nullptr, "ns_1", NVS_READWRITE, &result);
    CHECK(result == ESP_ERR_INVALID_ARG);
    CHECK(!handle);

    handle = nvs::open_nvs_handle_from_partition("test", nullptr, NVS_READWRITE, &result);
    CHECK(result == ESP_ERR_INVALID_ARG);
    CHECK(!handle);

    nvs::NVSPartitionManager::get_instance()->deinit_partition("test");
}

TEST_CASE("NVSHandleSimple CXX api open partition uninitialized", "[nvs cxx]")
{
    SpiFlashEmulator emu(10);
    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    handle = nvs::open_nvs_handle_from_partition("test", "ns_1", NVS_READWRITE, &result);
    bool result_expected = result == ESP_ERR_NVS_NOT_INITIALIZED || result == ESP_ERR_NVS_PART_NOT_FOUND;
    CHECK(result_expected);
    CHECK(!handle);
}

TEST_CASE("NVSHandleSimple CXX api open successful", "[nvs cxx]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);
    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom("test", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    handle = nvs::open_nvs_handle_from_partition("test", "ns_1", NVS_READWRITE, &result);
    CHECK(result == ESP_OK);
    CHECK(handle);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    handle.reset();

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs::NVSPartitionManager::get_instance()->deinit_partition("test");
}

TEST_CASE("NVSHandleSimple CXX api open default part successful", "[nvs cxx]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);
    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom("nvs", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    handle = nvs::open_nvs_handle("ns_1", NVS_READWRITE, &result);
    CHECK(result == ESP_OK);
    CHECK(handle);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    handle.reset();

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs::NVSPartitionManager::get_instance()->deinit_partition("nvs");
}

TEST_CASE("NVSHandleSimple CXX api open default part ns NULL", "[nvs cxx]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);
    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom("nvs", NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    handle = nvs::open_nvs_handle(nullptr, NVS_READWRITE, &result);
    CHECK(result == ESP_ERR_INVALID_ARG);
    CHECK(!handle);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs::NVSPartitionManager::get_instance()->deinit_partition("nvs");
}
