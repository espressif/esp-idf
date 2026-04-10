/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <catch2/catch_test_macros.hpp>
#include "nvs_flash.h"
#include "nvs_handle_simple.hpp"
#include "nvs_partition_manager.hpp"

#include <string>
#include <vector>

using namespace std;

#define TEST_DEFAULT_PARTITION_NAME "nvs"

TEST_CASE("NVSHandleSimple CXX api open invalid arguments", "[nvs cxx]")
{
    // Test case for invalid arguments in open_nvs_handle_from_partition
    // Combination of nullptr partition name and namespace name

    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    handle = nvs::open_nvs_handle_from_partition(nullptr, "ns_1", NVS_READWRITE, &result);
    CHECK(result == ESP_ERR_INVALID_ARG);
    CHECK(!handle);

    handle = nvs::open_nvs_handle_from_partition(TEST_DEFAULT_PARTITION_NAME, nullptr, NVS_READWRITE, &result);
    CHECK(result == ESP_ERR_INVALID_ARG);
    CHECK(!handle);

    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);
}

TEST_CASE("NVSHandleSimple CXX api open partition uninitialized", "[nvs cxx]")
{
    // Negative TC for opening a handle from a partition that has not been initialized

    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    handle = nvs::open_nvs_handle_from_partition(TEST_DEFAULT_PARTITION_NAME, "ns_1", NVS_READWRITE, &result);
    bool result_expected = result == ESP_ERR_NVS_NOT_INITIALIZED || result == ESP_ERR_NVS_PART_NOT_FOUND;
    CHECK(result_expected);
    CHECK(!handle);
}

TEST_CASE("NVSHandleSimple CXX api open successful", "[nvs cxx]")
{
    // Positive TC for opening a handle using open_nvs_handle_from_partition from an initialized partition

    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    handle = nvs::open_nvs_handle_from_partition(TEST_DEFAULT_PARTITION_NAME, "ns_1", NVS_READWRITE, &result);
    CHECK(result == ESP_OK);
    CHECK(handle);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    handle.reset();

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);
}

TEST_CASE("NVSHandleSimple CXX api open default part successful", "[nvs cxx]")
{
    // Positive TC for opening a handle using open_nvs_handle from an initialized default partition

    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    // Open handle without specifying partition name
    // This should open the default partition "nvs"
    handle = nvs::open_nvs_handle_from_partition(TEST_DEFAULT_PARTITION_NAME, "ns_1", NVS_READWRITE, &result);
    CHECK(result == ESP_OK);
    CHECK(handle);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    handle.reset();

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);
}

TEST_CASE("NVSHandleSimple CXX api open default part ns NULL", "[nvs cxx]")
{
    // Negative TC for opening a handle using open_nvs_handle from a partition name specified as nullptr

    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    handle = nvs::open_nvs_handle_from_partition(TEST_DEFAULT_PARTITION_NAME, nullptr, NVS_READWRITE, &result);
    CHECK(result == ESP_ERR_INVALID_ARG);
    CHECK(!handle);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);
}

TEST_CASE("NVSHandleSimple CXX api read/write string", "[nvs cxx]")
{
    // Positive TC for reading and writing a string using NVSHandleSimple CXX API

    char read_buffer [256];
    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    handle = nvs::open_nvs_handle_from_partition(TEST_DEFAULT_PARTITION_NAME, "test_ns", NVS_READWRITE, &result);
    CHECK(result == ESP_OK);
    REQUIRE(handle);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    CHECK(handle->set_string("test", "test string") == ESP_OK);
    CHECK(handle->commit() == ESP_OK);
    CHECK(handle->get_string("test", read_buffer, sizeof(read_buffer)) == ESP_OK);

    CHECK(string(read_buffer) == "test string");

    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);
}

TEST_CASE("NVSHandleSimple CXX api read/write blob", "[nvs cxx]")
{
    // Positive TC for reading and writing a blob using NVSHandleSimple CXX API

    const char blob [6] = {15, 16, 17, 18, 19};
    char read_blob[6] = {0};
    esp_err_t result;
    shared_ptr<nvs::NVSHandle> handle;

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    handle = nvs::open_nvs_handle_from_partition(TEST_DEFAULT_PARTITION_NAME, "test_ns", NVS_READWRITE, &result);
    CHECK(result == ESP_OK);
    REQUIRE(handle);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    CHECK(handle->set_blob("test", blob, sizeof(blob)) == ESP_OK);
    CHECK(handle->commit() == ESP_OK);
    CHECK(handle->get_blob("test", read_blob, sizeof(read_blob)) == ESP_OK);

    CHECK(vector<char>(blob, blob + sizeof(blob)) == vector<char>(read_blob, read_blob + sizeof(read_blob)));

    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);
}
