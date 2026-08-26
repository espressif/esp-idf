/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_flash.h"
#include "spi_flash_mmap.h"
#include "esp_partition.h"
#include "unity.h"

TEST_CASE("Basic handling of a partition in external flash", "[partition]")
{
    esp_flash_t flash = {
            .size = 1 * 1024 * 1024,
    };

    const esp_partition_type_t t = ESP_PARTITION_TYPE_DATA;
    const esp_partition_subtype_t st = ESP_PARTITION_SUBTYPE_DATA_FAT;
    const char* label = "ext_fat";

    /* can register */
    const esp_partition_t* ext_partition;
    TEST_ESP_OK(esp_partition_register_external(&flash, 0, flash.size, label, t, st, &ext_partition));

    /* can find the registered partition */
    const esp_partition_t* found = esp_partition_find_first(t, st, label);
    TEST_ASSERT_EQUAL_HEX(ext_partition, found);
    TEST_ASSERT_EQUAL(found->size, flash.size);
    TEST_ASSERT_EQUAL(found->address, 0);

    /* can deregister */
    TEST_ESP_OK(esp_partition_deregister_external(ext_partition));

    /* can not deregister one of the partitions on the main flash chip */
    const esp_partition_t* nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    TEST_ASSERT_NOT_NULL(nvs_partition);
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_partition_deregister_external(nvs_partition));

    /* can not deregister an unknown partition */
    esp_partition_t dummy_partition = {};
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_partition_deregister_external(&dummy_partition));

    /* can not register a partition larger than the flash size */
    TEST_ESP_ERR(ESP_ERR_INVALID_SIZE,
            esp_partition_register_external(&flash, 0, 2 * flash.size, "ext_fat", t, st, &ext_partition));

    /* can not register an overlapping partition */
    TEST_ESP_OK(esp_partition_register_external(&flash, 0, 2 * SPI_FLASH_SEC_SIZE, "p1", t, st, &ext_partition));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_partition_register_external(&flash, SPI_FLASH_SEC_SIZE, 2 * SPI_FLASH_SEC_SIZE,
            "p2", t, st, NULL));
    TEST_ESP_OK(esp_partition_deregister_external(ext_partition));
}
