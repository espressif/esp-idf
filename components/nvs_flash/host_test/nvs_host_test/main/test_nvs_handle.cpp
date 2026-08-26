/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <catch2/catch_test_macros.hpp>
#include <cmath>
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

TEST_CASE("NVSHandleSimple correctly sets/gets float", "[partition_mgr]")
{
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    float test_val = 3.14159265f;
    float test_val_read = 0.0f;

    CHECK(handle->set_item("key", test_val) == ESP_OK);
    CHECK(handle->get_item("key", test_val_read) == ESP_OK);
    CHECK(test_val == test_val_read);

    delete handle;
    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets double", "[partition_mgr]")
{
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    double test_val = 2.718281828459045;
    double test_val_read = 0.0;

    CHECK(handle->set_item("key", test_val) == ESP_OK);
    CHECK(handle->get_item("key", test_val_read) == ESP_OK);
    CHECK(test_val == test_val_read);

    delete handle;
    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple float type mismatch with int", "[partition_mgr]")
{
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    float test_float = 1.5f;
    CHECK(handle->set_item("key", test_float) == ESP_OK);

    int32_t read_int = 0;
    CHECK(handle->get_item("key", read_int) == ESP_ERR_NVS_NOT_FOUND);

    uint32_t read_uint = 0;
    CHECK(handle->get_item("key", read_uint) == ESP_ERR_NVS_NOT_FOUND);

    delete handle;
    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple double type mismatch with float", "[partition_mgr]")
{
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    double test_double = 1.5;
    CHECK(handle->set_item("key", test_double) == ESP_OK);

    float read_float = 0.0f;
    CHECK(handle->get_item("key", read_float) == ESP_ERR_NVS_NOT_FOUND);

    int64_t read_int64 = 0;
    CHECK(handle->get_item("key", read_int64) == ESP_ERR_NVS_NOT_FOUND);

    delete handle;
    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple readonly rejects float and double writes", "[partition_mgr]")
{
    REQUIRE(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
    REQUIRE(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);

    nvs::NVSHandleSimple *rw_handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READWRITE, &rw_handle) == ESP_OK);
    CHECK(rw_handle->set_item("fkey", 1.0f) == ESP_OK);
    CHECK(rw_handle->set_item("dkey", 2.0) == ESP_OK);
    delete rw_handle;

    nvs::NVSHandleSimple *ro_handle = nullptr;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(TEST_3SEC_PARTITION_NAME, "ns_1", NVS_READONLY, &ro_handle) == ESP_OK);

    CHECK(ro_handle->set_item("fkey", 9.0f) == ESP_ERR_NVS_READ_ONLY);
    CHECK(ro_handle->set_item("dkey", 9.0) == ESP_ERR_NVS_READ_ONLY);

    float fval = 0.0f;
    CHECK(ro_handle->get_item("fkey", fval) == ESP_OK);
    CHECK(fval == 1.0f);

    double dval = 0.0;
    CHECK(ro_handle->get_item("dkey", dval) == ESP_OK);
    CHECK(dval == 2.0);

    delete ro_handle;
    REQUIRE(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME) == ESP_OK);
}
