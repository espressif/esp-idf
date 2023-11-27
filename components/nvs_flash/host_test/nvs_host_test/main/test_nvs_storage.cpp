/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include "nvs_storage.hpp"
#include "nvs_partition_manager.hpp"
#include "test_fixtures.hpp"
#include <iostream>

TEST_CASE("Storage iterator recognizes blob with VerOffset::VER_1_OFFSET", "[nvs_storage]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    uint8_t blob [] = {0x0, 0x1, 0x2, 0x3};
    uint8_t blob_new [] = {0x3, 0x2, 0x1, 0x0};
    nvs::Storage *storage = nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name("test");
    uint8_t ns_index;
    storage->createOrOpenNamespace("test_ns", true, ns_index);

    CHECK(storage->writeItem(ns_index, nvs::ItemType::BLOB, "test_blob", blob, sizeof(blob)) == ESP_OK);

    // changing provokes a blob with version offset 1 (VerOffset::VER_1_OFFSET)
    CHECK(storage->writeItem(ns_index, nvs::ItemType::BLOB, "test_blob", blob_new, sizeof(blob_new)) == ESP_OK);

    nvs_opaque_iterator_t it;
    it.storage = storage;
    it.type = NVS_TYPE_ANY;

    // Central check: does the iterator recognize the blob with version 1?
    REQUIRE(storage->findEntry(&it, "test_ns"));

    CHECK(string(it.entry_info.namespace_name) == string("test_ns"));
    CHECK(string(it.entry_info.key) == string("test_blob"));
    CHECK(it.entry_info.type == NVS_TYPE_BLOB);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);
}
