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
#include "nvs_partition.hpp"
#include "spi_flash_emulation.h"

#include "test_fixtures.hpp"

#include <iostream>

using namespace std;
using namespace nvs;

TEST_CASE("encrypted partition read size must be item size", "[nvs]")
{
    char foo [32] = { };
    nvs_sec_cfg_t xts_cfg;
    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        xts_cfg.eky[count] = 0x11;
        xts_cfg.tky[count] = 0x22;
    }
    EncryptedPartitionFixture fix(&xts_cfg);

    CHECK(fix.part.read(0, foo, sizeof (foo) -1) == ESP_ERR_INVALID_SIZE);
}

TEST_CASE("encrypted partition write size must be mod item size", "[nvs]")
{
    char foo [64] = { };
    nvs_sec_cfg_t xts_cfg;
    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        xts_cfg.eky[count] = 0x11;
        xts_cfg.tky[count] = 0x22;
    }
    EncryptedPartitionFixture fix(&xts_cfg);

    CHECK(fix.part.write(0, foo, sizeof (foo) -1) == ESP_ERR_INVALID_SIZE);
    CHECK(fix.part.write(0, foo, sizeof (foo)) == ESP_OK);
    CHECK(fix.part.write(0, foo, sizeof (foo) * 2) == ESP_OK);
}
