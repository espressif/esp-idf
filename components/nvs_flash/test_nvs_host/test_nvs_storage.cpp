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
#include "nvs_test_api.h"
#include "nvs.h"
#include "spi_flash_emulation.h"

#include <iostream>

using namespace std;

TEST_CASE("Storage iterator recognizes blob with VerOffset::VER_1_OFFSET", "[nvs_storage]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    SpiFlashEmulator emu(10);

    CHECK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    nvs_handle_t handle_1;
    uint8_t blob [] = {0x0, 0x1, 0x2, 0x3};
    uint8_t blob_new [] = {0x3, 0x2, 0x1, 0x0};

    // first, creating namespace...
    CHECK(nvs_open("test_ns", NVS_READWRITE, &handle_1) == ESP_OK);
    CHECK(nvs_set_blob(handle_1, "key", blob, sizeof(blob)) == ESP_OK);

    // changing provokes a blob with version offset 1 (VerOffset::VER_1_OFFSET)
    CHECK(nvs_set_blob(handle_1, "key", blob_new, sizeof(blob_new)) == ESP_OK);

    nvs_iterator_t it = nvs_entry_find(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_ANY);
    REQUIRE(it);
    nvs_entry_info_t info;
    nvs_entry_info(it, &info);

    CHECK(string(info.namespace_name) == "test_ns");
    CHECK(string(info.key) == "key");
    CHECK(info.type == NVS_TYPE_BLOB);

    nvs_close(handle_1);

    // without deinit it affects "nvs api tests"
    CHECK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME) == ESP_OK);
}

