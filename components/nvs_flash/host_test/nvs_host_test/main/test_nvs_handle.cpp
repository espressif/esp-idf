/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <catch2/catch_test_macros.hpp>
#include "nvs_flash.h"
#include "nvs_handle_simple.hpp"
#include "nvs_partition_manager.hpp"

using namespace std;

#define TEST_DEFAULT_PARTITION_NAME "nvs"               // Default partition name used in the tests - 10 sectors
#define TEST_3SEC_PARTITION_NAME "nvs_3sec"             // Partition used in the space constrained tests - 3 sectors

TEST_CASE("NVSHandleSimple closes its reference in PartitionManager", "[partition_mgr]")
{
    // Positive TC verifying that NVSHandleSimple closes its reference in PartitionManager
    // when it is deleted.

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_DEFAULT_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    REQUIRE(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple multiple open and closes with PartitionManager", "[partition_mgr]")
{
    // Positive TC for multiple open and close of NVSHandleSimple with PartitionManager

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs::NVSHandleSimple *handle1 = nullptr;
    nvs::NVSHandleSimple *handle2 = nullptr;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_DEFAULT_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle1) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_DEFAULT_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle2) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 2);

    delete handle1;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle2;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    REQUIRE(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple readonly fails", "[partition_mgr]")
{
    // Negative TC for opening a handle with read-only mode and trying to write to it

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle_1 = nullptr;
    nvs::NVSHandleSimple *handle_2 = nullptr;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    // first, creating namespace...
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle_1) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle_1;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READONLY, &handle_2) == ESP_OK);
    CHECK(handle_2->set_item("key", 47) == ESP_ERR_NVS_READ_ONLY);
    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle_2;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    // without deinit it affects "nvs api tests"
    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple set/get char", "[partition_mgr]")
{
    // Positive TC for setting and getting a char using NVSHandleSimple
    enum class TestEnum : char {
        FOO = -1,
        BEER,
        BAR
    };

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    char test_e = 'a';
    char test_e_read = 'z';

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets int enum", "[partition_mgr]")
{
    // Positive TC for setting and getting an int enum using NVSHandleSimple
    enum class TestEnum : int {
        FOO,
        BAR
    };

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    TestEnum test_e = TestEnum::BAR;
    TestEnum test_e_read = TestEnum::FOO;

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets int enum with negative values", "[partition_mgr]")
{
    // Positive TC for setting and getting an int enum with negative values using NVSHandleSimple
    enum class TestEnum : int {
        FOO = -1,
        BEER,
        BAR
    };

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    TestEnum test_e = TestEnum::FOO;
    TestEnum test_e_read = TestEnum::BEER;

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets uint8_t enum", "[partition_mgr]")
{
    // Positive TC for setting and getting a uint8_t enum using NVSHandleSimple
    enum class TestEnum : uint8_t {
        FOO,
        BAR
    };

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    TestEnum test_e = TestEnum::BAR;
    TestEnum test_e_read = TestEnum::FOO;

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets char enum", "[partition_mgr]")
{
    // Positive TC for setting and getting a char enum using NVSHandleSimple
    enum class TestEnum : char {
        FOO = -1,
        BEER,
        BAR
    };

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    TestEnum test_e = TestEnum::BAR;
    TestEnum test_e_read = TestEnum::FOO;

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}
