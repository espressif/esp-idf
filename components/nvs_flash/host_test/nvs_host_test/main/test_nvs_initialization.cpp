/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"                          // for CONFIG_NVS_BDL_STACK
#include <catch2/catch_test_macros.hpp>
#include "nvs_flash.h"
#include "nvs_partition_manager.hpp"
#include "nvs_partition.hpp"
#include "esp_partition.h"                      // for esp_partition_find_first
#include "bdl_ramdisk.hpp"                      // for bdl_ramdisk

using namespace std;

#define TEST_DEFAULT_PARTITION_NAME "nvs"

#ifndef CONFIG_NVS_BDL_STACK
TEST_CASE("nvs_flash_init_partition_ptr fails due to nullptr arg", "[nvs_custom_part]")
{
    // This test checks that nvs_flash_init_partition_ptr fails when passed a nullptr as partition argument.

    CHECK(nvs_flash_init_partition_ptr(nullptr) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_flash_init_partition_ptr inits one partition", "[nvs_custom_part]")
{
     // This test initializes a partition with a specific name and checks if it is registered in the NVSPartitionManager.
     // The test works around the intnernal partition lookup by handing over a partition structure retrieved from esp_partition_find_first.

    // Erase the partition to ensure it is clean
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, TEST_DEFAULT_PARTITION_NAME);
    REQUIRE(partition != nullptr);

    CHECK(nvs_flash_init_partition_ptr(partition) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(TEST_DEFAULT_PARTITION_NAME) != nullptr);

    REQUIRE(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
}
#else
TEST_CASE("nvs_flash_init_partition_bdl fails due to nullptr arg", "[nvs_custom_part]")
{
    // This test checks that nvs_flash_init_partition_bdl fails when nullptr is provided as a block device argument
    // or the partition name argument is null.

    // Check nullptr block device
    CHECK(nvs_flash_init_partition_bdl("NullBDL", nullptr) == ESP_ERR_INVALID_ARG);

    // Check nullptr partition name
    bdl_ramdisk ramdisk(3 * NVS_CONST_PAGE_SIZE, NVS_CONST_PAGE_SIZE);
    CHECK(ramdisk.get_handle() != nullptr);
    CHECK(nvs_flash_init_partition_bdl(nullptr, ramdisk.get_handle()) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_flash_init_partition_bdl fails due to ineligible block device", "[nvs_custom_part]")
{
    // This test checks that nvs_flash_init_partition_bdl fails when passed an ineligible block device.

    const char* part_name = "BadBDL";
    bdl_ramdisk ramdisk(3 * NVS_CONST_PAGE_SIZE, NVS_CONST_PAGE_SIZE); // Ramdisk of 3 NVS pages size
    CHECK(ramdisk.get_handle() != nullptr);

    // Cripple the ramdisk to make it ineligible (erase size not matching NVS requirements)
    ramdisk.get_handle()->geometry.erase_size = NVS_CONST_PAGE_SIZE - 1;

    CHECK(nvs_flash_init_partition_bdl(part_name, ramdisk.get_handle()) == ESP_ERR_NOT_SUPPORTED);
}

TEST_CASE("nvs_flash_init_partition_bdl inits externally supplied block device", "[nvs_custom_part]")
{
     // This test initializes NVS on the externally supplied block device with a specific name and checks if
     // it is registered in the NVSPartitionManager.
     // The test works around the intnernal partition lookup by handing over a partition structure retrieved from esp_partition_find_first.

    // Erase the partition to ensure it is clean
    const char* part_name = "GoodBDL";
    bdl_ramdisk ramdisk(3 * NVS_CONST_PAGE_SIZE, NVS_CONST_PAGE_SIZE); // Ramdisk of 3 NVS pages size
    CHECK(ramdisk.get_handle() != nullptr);

    CHECK(nvs_flash_init_partition_bdl(part_name, ramdisk.get_handle()) == ESP_OK);
    CHECK(nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(part_name) != nullptr);

    REQUIRE(nvs_flash_deinit_partition(part_name) == ESP_OK);
}

#endif // CONFIG_NVS_BDL_STACK
