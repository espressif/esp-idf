/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <catch2/catch_test_macros.hpp>     // for Catch2 test macros
#include "nvs_handle_simple.hpp"            // for NVSHandleSimple class
#include "nvs_partition_manager.hpp"        // for NVSPartitionManager class
#include "test_fixtures.hpp"                // for test fixtures

using namespace std;

#define TEST_DEFAULT_PARTITION_NAME "nvs"
#define TEST_SECONDARY_PARTITION_NAME "nvs_sec"

TEST_CASE("Partition manager initializes storage", "[partition_mgr]")
{
    // Positive TC for partition manager initializing storage
    // This test initializes a partition with a specific name and checks if it is registered in the NVSPartitionManager.

    // Erase the partition to ensure it is clean
    // We cannot use nvs_flash_erase_partition here because it is actually using the NVSPartitionManager we want to test.
    REQUIRE(NVSPartitionTestHelper::erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(TEST_DEFAULT_PARTITION_NAME) != nullptr);
    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("Partition manager de-initializes storage", "[partition_mgr]")
{
    // Positive TC for partition manager de-initializing storage
    // This test initializes a partition with a specific name and checks if it is registered in the NVSPartitionManager.
    // After de-initialization, the storage should no longer be found

    // Erase the partition to ensure it is clean
    // We cannot use nvs_flash_erase_partition here because it is actually using the NVSPartitionManager we want to test.
    REQUIRE(NVSPartitionTestHelper::erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(TEST_DEFAULT_PARTITION_NAME) != nullptr);
    CHECK(nvs::NVSPartitionManager::get_instance()->deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(TEST_DEFAULT_PARTITION_NAME) == nullptr);
}

TEST_CASE("Partition manager open fails on null handle", "[partition_mgr]")
{
    // Negative TC for partition manager open_handle failing on null handle pointer
    // This test initializes a partition with a specific name.
    // The handle shouldn't be null, so the open_handle should return ESP_ERR_INVALID_ARG

    // Erase the partition to ensure it is clean
    // We cannot use nvs_flash_erase_partition here because it is actually using the NVSPartitionManager we want to test.
    REQUIRE(NVSPartitionTestHelper::erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_DEFAULT_PARTITION_NAME, "ns_1", NVS_READWRITE, nullptr)
          == ESP_ERR_INVALID_ARG);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("Partition manager invalidates handle on partition de-init", "[partition_mgr]")
{
    // Positive TC for partition manager invalidating handle on partition de-init
    // This test initializes a partition with a specific name and checks if it is registered in the NVSPartitionManager.
    // After de-initialization, the handle should be invalidated

    // Erase the partition to ensure it is clean
    // We cannot use nvs_flash_erase_partition here because it is actually using the NVSPartitionManager we want to test.
    REQUIRE(NVSPartitionTestHelper::erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_DEFAULT_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);
    CHECK(handle->erase_all() == ESP_OK);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(handle->erase_all() == ESP_ERR_NVS_INVALID_HANDLE);

    delete handle;
}

TEST_CASE("Partition manager initializes multiple partitions", "[partition_mgr]")
{
    // Positive TC for partition manager initializing multiple partitions
    // This test initializes two partitions with specific names and checks if they are registered in the NVSPartitionManager.
    // After de-initialization, both partitions shouldn't be found

    // Erase the partitions to ensure they are clean
    // We cannot use nvs_flash_erase_partition here because it is actually using the NVSPartitionManager we want to test.
    REQUIRE(NVSPartitionTestHelper::erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
    REQUIRE(NVSPartitionTestHelper::erase_partition(TEST_SECONDARY_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the alternative partition
    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_partition(TEST_SECONDARY_PARTITION_NAME) == ESP_OK);

    // Check if both partitions are registered
    nvs::Storage *storage1 = nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(TEST_DEFAULT_PARTITION_NAME);
    REQUIRE(storage1 != nullptr);
    nvs::Storage *storage2 = nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(TEST_SECONDARY_PARTITION_NAME);
    REQUIRE(storage2 != nullptr);

    // Check if the two storages are different
    CHECK(storage1 != storage2);

    // De-initialize both partitions
    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(TEST_SECONDARY_PARTITION_NAME) == ESP_OK);
}
