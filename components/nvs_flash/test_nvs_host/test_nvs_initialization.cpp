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
#include "nvs_partition_manager.hpp"
#include "spi_flash_emulation.h"
#include "esp_partition.h"
#include "nvs.h"
#include <string.h>

using namespace nvs;

TEST_CASE("nvs_flash_init_partition_ptr fails due to nullptr arg", "[nvs_custom_part]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    CHECK(nvs_flash_init_partition_ptr(nullptr) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_flash_init_partition_ptr inits one partition", "[nvs_custom_part]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    esp_partition_t partition = {};
    strcpy(partition.label, "test");
    partition.address = NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
    partition.size = NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE;

    CHECK(nvs_flash_init_partition_ptr(&partition) == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
    CHECK(NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);
}
