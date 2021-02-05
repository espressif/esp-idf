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

#define TEST_ESP_OK(rc) CHECK((rc) == ESP_OK)

TEST_CASE("nvs_flash_init_partition_ptr fails due to nullptr arg", "[nvs_custom_part]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    CHECK(nvs_flash_init_partition_ptr(nullptr) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_flash_init_partition_ptr fails due to external partition", "[nvs_custom_part]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    struct esp_flash_t spi_flash = {};
    esp_partition_t partition = {};
    strcpy(partition.label, "test");
    partition.address = NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
    partition.size = NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE;
    partition.flash_chip = &spi_flash;
    CHECK(nvs_flash_init_partition_ptr(&partition) == ESP_ERR_NOT_SUPPORTED);
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
    partition.flash_chip = esp_flash_default_chip;

    CHECK(nvs_flash_init_partition_ptr(&partition) == ESP_OK);
    CHECK(NVSPartitionManager::get_instance()->lookup_storage_from_name("test") != nullptr);
    CHECK(NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);
}

TEST_CASE("deinit partition doesn't affect other partition's open handles", "[nvs]")
{
    const char *OTHER_PARTITION_NAME = "other_part";
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;

    SpiFlashEmulator emu_default(10);
    emu_default.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    esp_partition_t part_default = {};
    strcpy(part_default.label, NVS_DEFAULT_PART_NAME);
    part_default.address = NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
    part_default.size = NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE;

    SpiFlashEmulator emu_other(10);
    emu_other.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    esp_partition_t part_other = {};
    strcpy(part_other.label, OTHER_PARTITION_NAME);
    part_other.address = NVS_FLASH_SECTOR * SPI_FLASH_SEC_SIZE;
    part_other.size = NVS_FLASH_SECTOR_COUNT_MIN * SPI_FLASH_SEC_SIZE;

    const char* str = "value 0123456789abcdef0123456789abcdef";
    const uint8_t blob[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};

    nvs_handle_t handle_1;

    TEST_ESP_OK(NVSPartitionManager::get_instance()->init_custom(NVS_DEFAULT_PART_NAME,
            NVS_FLASH_SECTOR,
            NVS_FLASH_SECTOR_COUNT_MIN));
    TEST_ESP_OK(NVSPartitionManager::get_instance()->init_custom(OTHER_PARTITION_NAME,
            NVS_FLASH_SECTOR,
            NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_OK(nvs_open_from_partition(OTHER_PARTITION_NAME, "ns", NVS_READWRITE, &handle_1));

    // Deinitializing must not interfere with the open handle from the other partition.
    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));

    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x3456789a));
    nvs_close(handle_1);

    TEST_ESP_OK(nvs_flash_deinit_partition(OTHER_PARTITION_NAME));
}
