/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>

#include "esp_log.h"
#include "esp_system.h"
#include "esp_heap_caps.h"

#include "esp_flash.h"
#include "esp_rom_spiflash.h"
#include "spi_flash_mmap.h"
#include "esp_private/cache_utils.h"
#include "esp_partition.h"
#include "nvs_flash.h"

#include "esp_tee.h"
#include "esp_tee_sec_storage.h"
#include "secure_service_num.h"

#include "unity.h"

#define BOOT_COUNT_NAMESPACE "boot_count"

static const char *TAG = "test_esp_tee_flash_prot";

static void set_boot_count_in_nvs(uint8_t boot_count)
{
    nvs_handle_t boot_count_handle;
    TEST_ESP_OK(nvs_open(BOOT_COUNT_NAMESPACE, NVS_READWRITE, &boot_count_handle));
    TEST_ESP_OK(nvs_set_u8(boot_count_handle, "boot_count", boot_count));
    TEST_ESP_OK(nvs_commit(boot_count_handle));
    nvs_close(boot_count_handle);
}

static uint8_t get_boot_count_from_nvs(void)
{
    nvs_handle_t boot_count_handle;
    esp_err_t err = nvs_open(BOOT_COUNT_NAMESPACE, NVS_READONLY, &boot_count_handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        set_boot_count_in_nvs(0);
    }
    uint8_t boot_count;
    TEST_ESP_OK(nvs_get_u8(boot_count_handle, "boot_count", &boot_count));
    nvs_close(boot_count_handle);
    return boot_count;
}

static void test_initial_boot(void)
{
    ESP_LOGI(TAG, "Boot: 1");
    set_boot_count_in_nvs(1);
    esp_restart();
}

/* ---------------------------------------------- API family 1: esp_partition ------------------------------------------------- */

static void test_esp_partition_mmap_api(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);

    const esp_partition_t *part = NULL;
    esp_partition_mmap_handle_t out_handle;
    const void *outptr = NULL;

    switch (boot_count) {
    case 2:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_0, NULL);
        TEST_ASSERT_NOT_NULL(part);
        TEST_ESP_OK(esp_partition_mmap(part, 0, part->size, ESP_PARTITION_MMAP_DATA, &outptr, &out_handle));
        ESP_LOG_BUFFER_HEXDUMP(TAG, outptr, 0x20, ESP_LOG_INFO);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    case 3:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_1, NULL);
        TEST_ASSERT_NOT_NULL(part);
        TEST_ESP_OK(esp_partition_mmap(part, 0, part->size, ESP_PARTITION_MMAP_INST, &outptr, &out_handle));
        ESP_LOG_BUFFER_HEXDUMP(TAG, outptr, 0x20, ESP_LOG_INFO);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    case 4:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_TEE_OTA, NULL);
        TEST_ASSERT_NOT_NULL(part);
        TEST_ESP_OK(esp_partition_mmap(part, 0, part->size, ESP_PARTITION_MMAP_DATA, &outptr, &out_handle));
        ESP_LOG_BUFFER_HEXDUMP(TAG, outptr, 0x20, ESP_LOG_INFO);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    case 5:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_TEE_SEC_STORAGE, NULL);
        TEST_ASSERT_NOT_NULL(part);
        TEST_ESP_OK(esp_partition_mmap(part, 0, part->size, ESP_PARTITION_MMAP_DATA, &outptr, &out_handle));
        ESP_LOG_BUFFER_HEXDUMP(TAG, outptr, 0x20, ESP_LOG_INFO);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    default:
        TEST_FAIL_MESSAGE("Unexpected stage");
        break;
    }
}

TEST_CASE_MULTIPLE_STAGES("Test REE-TEE isolation: Flash - SPI0 (esp_partition_mmap)", "[flash_prot][timeout=60]",
                          test_initial_boot, test_esp_partition_mmap_api, test_esp_partition_mmap_api,
                          test_esp_partition_mmap_api, test_esp_partition_mmap_api);

/* ---------------------------------------------- API family 2: spi_flash ------------------------------------------------- */

static void test_spi_flash_mmap_api(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);

    const esp_partition_t *part = NULL;
    spi_flash_mmap_handle_t handle;
    const void *ptr = NULL;

    switch (boot_count) {
    case 2:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_0, NULL);
        TEST_ASSERT_NOT_NULL(part);
        TEST_ESP_OK(spi_flash_mmap(part->address, part->size, SPI_FLASH_MMAP_DATA, &ptr, &handle));
        ESP_LOG_BUFFER_HEXDUMP(TAG, ptr, 0x20, ESP_LOG_INFO);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    case 3:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_1, NULL);
        TEST_ASSERT_NOT_NULL(part);
        TEST_ESP_OK(spi_flash_mmap(part->address, part->size, SPI_FLASH_MMAP_INST, &ptr, &handle));
        ESP_LOG_BUFFER_HEXDUMP(TAG, ptr, 0x20, ESP_LOG_INFO);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    case 4:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_TEE_OTA, NULL);
        TEST_ASSERT_NOT_NULL(part);
        TEST_ESP_OK(spi_flash_mmap(part->address, part->size, SPI_FLASH_MMAP_DATA, &ptr, &handle));
        ESP_LOG_BUFFER_HEXDUMP(TAG, ptr, 0x20, ESP_LOG_INFO);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    default:
        TEST_FAIL_MESSAGE("Unexpected stage");
        break;
    }
}

TEST_CASE_MULTIPLE_STAGES("Test REE-TEE isolation: Flash - SPI0 (spi_flash_mmap)", "[flash_prot][timeout=60]",
                          test_initial_boot, test_spi_flash_mmap_api, test_spi_flash_mmap_api,
                          test_spi_flash_mmap_api);
