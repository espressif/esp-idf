/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include <algorithm>
#include <cstring>
#include "nvs_handle_simple.hpp"
#include "nvs_partition_manager.hpp"
#include "test_fixtures.hpp"
#include <iostream>
#include <string>

#define TEMPORARILY_DISABLED(x)

TEST_CASE("NVSHandleSimple closes its reference in PartitionManager", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs::NVSHandleSimple *handle;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);

}

TEST_CASE("NVSHandleSimple multiple open and closes with PartitionManager", "[partition_mgr]")
{
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    PartitionEmulationFixture f(0, 10, "test");

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    nvs::NVSHandleSimple *handle1;
    nvs::NVSHandleSimple *handle2;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle1) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle("test", "ns_1", NVS_READWRITE, &handle2) == ESP_OK);

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 2);

    delete handle1;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle2;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition("test") == ESP_OK);

}

TEST_CASE("NVSHandleSimple readonly fails", "[partition_mgr]")
{
    PartitionEmulationFixture f(0, 10);

    nvs::NVSPartitionManager::get_instance()->deinit_partition(NVS_DEFAULT_PART_NAME);
    nvs::NVSHandleSimple *handle_1;
    nvs::NVSHandleSimple *handle_2;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    CHECK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);

    // first, creating namespace...
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READWRITE, &handle_1) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle_1;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READONLY, &handle_2) == ESP_OK);
    CHECK(handle_2->set_item("key", 47) == ESP_ERR_NVS_READ_ONLY);
    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 1);

    delete handle_2;

    CHECK(nvs::NVSPartitionManager::get_instance()->open_handles_size() == 0);
    // without deinit it affects "nvs api tests"
    CHECK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple set/get char", "[partition_mgr]")
{
    enum class TestEnum : char {
        FOO = -1,
        BEER,
        BAR
    };

    PartitionEmulationFixture f(0, 10);

    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    nvs::NVSHandleSimple *handle;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    char test_e = 'a';
    char test_e_read = 'z';

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(NVS_DEFAULT_PART_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets int enum", "[partition_mgr]")
{
    enum class TestEnum : int {
        FOO,
        BAR
    };

    PartitionEmulationFixture f(0, 10);

    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    nvs::NVSHandleSimple *handle;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    TestEnum test_e = TestEnum::BAR;
    TestEnum test_e_read = TestEnum::FOO;

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(NVS_DEFAULT_PART_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets int enum with negative values", "[partition_mgr]")
{
    enum class TestEnum : int {
        FOO = -1,
        BEER,
        BAR
    };

    PartitionEmulationFixture f(0, 10);

    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    nvs::NVSHandleSimple *handle;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    TestEnum test_e = TestEnum::FOO;
    TestEnum test_e_read = TestEnum::BEER;

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(NVS_DEFAULT_PART_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets uint8_t enum", "[partition_mgr]")
{
    enum class TestEnum : uint8_t {
        FOO,
        BAR
    };

    PartitionEmulationFixture f(0, 10);

    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    nvs::NVSHandleSimple *handle;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    TestEnum test_e = TestEnum::BAR;
    TestEnum test_e_read = TestEnum::FOO;

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(NVS_DEFAULT_PART_NAME) == ESP_OK);
}

TEST_CASE("NVSHandleSimple correctly sets/gets char enum", "[partition_mgr]")
{
    enum class TestEnum : char {
        FOO = -1,
        BEER,
        BAR
    };

    PartitionEmulationFixture f(0, 10);

    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    REQUIRE(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_OK);

    nvs::NVSHandleSimple *handle;
    REQUIRE(nvs::NVSPartitionManager::get_instance()->open_handle(NVS_DEFAULT_PART_NAME, "ns_1", NVS_READWRITE, &handle) == ESP_OK);

    TestEnum test_e = TestEnum::BAR;
    TestEnum test_e_read = TestEnum::FOO;

    CHECK(handle->set_item("key", test_e) == ESP_OK);

    CHECK(handle->get_item("key", test_e_read) == ESP_OK);
    CHECK(test_e == test_e_read);

    delete handle;

    REQUIRE(nvs::NVSPartitionManager::get_instance()->deinit_partition(NVS_DEFAULT_PART_NAME) == ESP_OK);
}
