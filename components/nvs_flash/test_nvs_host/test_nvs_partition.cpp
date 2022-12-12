/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include <algorithm>
#include <cstring>
#include "nvs_test_api.h"
#include "nvs_handle_simple.hpp"
#include "nvs_partition.hpp"
#include "spi_flash_emulation.h"
#include "test_fixtures.hpp"
#include <iostream>

TEST_CASE("encrypted partition read size must be item size", "[nvs]")
{
    char foo [32] = { };
    nvs_sec_cfg_t xts_cfg;
    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        xts_cfg.eky[count] = 0x11;
        xts_cfg.tky[count] = 0x22;
    }
    EncryptedPartitionFixture fix(&xts_cfg);

    CHECK(fix.part.read(0, foo, sizeof (foo) - 1) == ESP_ERR_INVALID_SIZE);
}

TEST_CASE("encrypted partition write size must be mod item size", "[nvs]")
{
    char foo [64] = { };
    nvs_sec_cfg_t xts_cfg;
    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        xts_cfg.eky[count] = 0x11;
        xts_cfg.tky[count] = 0x22;
    }
    EncryptedPartitionFixture fix(&xts_cfg);

    CHECK(fix.part.write(0, foo, sizeof (foo) - 1) == ESP_ERR_INVALID_SIZE);
    CHECK(fix.part.write(0, foo, sizeof (foo) / 2) == ESP_OK);
    CHECK(fix.part.write(sizeof(foo) / 2, foo, sizeof (foo)) == ESP_OK);
}
