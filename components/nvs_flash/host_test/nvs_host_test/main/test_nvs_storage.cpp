/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <catch2/catch_test_macros.hpp>
#include "nvs_flash.h"
#include "nvs_storage.hpp"
#include "nvs_partition_manager.hpp"

using namespace std;

#define TEST_DEFAULT_PARTITION_NAME "nvs"
#define TEST_DEFAULT_PURGE_AFTER_ERASE true        // erase with purge after erase

TEST_CASE("Storage iterator recognizes blob with VerOffset::VER_1_OFFSET", "[nvs_storage]")
{
    // Positive TC for Storage iterator recognizing a blob with VerOffset::VER_1_OFFSET

    // Erase the partition before testing
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    uint8_t blob [] = {0x0, 0x1, 0x2, 0x3};
    uint8_t blob_new [] = {0x3, 0x2, 0x1, 0x0};
    nvs::Storage *storage = nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(TEST_DEFAULT_PARTITION_NAME);
    uint8_t ns_index;

    REQUIRE(storage != nullptr);
    storage->createOrOpenNamespace("test_ns", true, ns_index);

    CHECK(storage->writeItem(ns_index, nvs::ItemType::BLOB, "test_blob", blob, sizeof(blob), TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);

    // changing provokes a blob with version offset 1 (VerOffset::VER_1_OFFSET)
    CHECK(storage->writeItem(ns_index, nvs::ItemType::BLOB, "test_blob", blob_new, sizeof(blob_new), TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    nvs_opaque_iterator_t it;
    it.storage = storage;
    it.type = NVS_TYPE_ANY;

    // Central check: does the iterator recognize the blob with version 1?
    REQUIRE(storage->findEntry(&it, "test_ns"));

    CHECK(string(it.entry_info.namespace_name) == string("test_ns"));
    CHECK(string(it.entry_info.key) == string("test_blob"));
    CHECK(it.entry_info.type == NVS_TYPE_BLOB);

    REQUIRE(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
}
