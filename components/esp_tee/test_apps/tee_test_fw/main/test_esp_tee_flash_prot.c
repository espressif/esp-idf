/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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
#include "ccomp_timer.h"

#define BOOT_COUNT_NAMESPACE "boot_count"
#define TEST_PART_LABEL      "custom"
#define TEST_BUF_SZ          256

#define ESP_TEE_SEC_STG_PART_LABEL "secure_storage"

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
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, ESP_TEE_SEC_STG_PART_LABEL);
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

#if CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1
static void test_esp_partition_api_r(const esp_partition_t *part)
{
    TEST_ASSERT_NOT_NULL(part);
    uint8_t buf_r[128];
    memset(buf_r, 0x00, sizeof(buf_r));
    TEST_ESP_ERR(ESP_FAIL, esp_partition_read(part, 0x00, buf_r, sizeof(buf_r)));
}

static void test_esp_partition_api_w(const esp_partition_t *part)
{
    TEST_ASSERT_NOT_NULL(part);
    uint8_t buf_w[128];
    memset(buf_w, 0xA5, sizeof(buf_w));
    TEST_ESP_ERR(ESP_FAIL, esp_partition_write(part, 0x00, buf_w, sizeof(buf_w)));
}

static void test_esp_partition_api_e(const esp_partition_t *part)
{
    TEST_ASSERT_NOT_NULL(part);
    TEST_ESP_ERR(ESP_FAIL, esp_partition_erase_range(part, 0x00, SPI_FLASH_SEC_SIZE));
}

static void test_esp_partition_api(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);

    const esp_partition_t *part = NULL;
    switch (boot_count) {
    case 2:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_0, NULL);
        test_esp_partition_api_r(part);
        break;
    case 3:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_1, NULL);
        test_esp_partition_api_w(part);
        break;
    case 4:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, ESP_TEE_SEC_STG_PART_LABEL);
        test_esp_partition_api_w(part);
        break;
    case 5:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_TEE_OTA, NULL);
        test_esp_partition_api_e(part);
        break;
    default:
        TEST_FAIL_MESSAGE("Unexpected stage");
        break;
    }

    esp_restart();
}

TEST_CASE_MULTIPLE_STAGES("Test REE-TEE isolation: Flash - SPI1 (esp_partition)", "[flash_prot][timeout=60]",
                          test_initial_boot, test_esp_partition_api, test_esp_partition_api,
                          test_esp_partition_api, test_esp_partition_api);
#endif

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

/* ---------------------------------------------- API family 3: esp_flash ------------------------------------------------- */

#if CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1
static void test_esp_flash_api_r(uint32_t paddr)
{
    uint8_t buf_r[128];
    memset(buf_r, 0x00, sizeof(buf_r));
    TEST_ESP_ERR(ESP_FAIL, esp_flash_read(NULL, buf_r, paddr, sizeof(buf_r)));
}

static void test_esp_flash_api_w(uint32_t paddr)
{
    uint8_t buf_w[128];
    memset(buf_w, 0xA5, sizeof(buf_w));
    TEST_ESP_ERR(ESP_FAIL, esp_flash_write(NULL, buf_w, paddr, sizeof(buf_w)));
}

static void test_esp_flash_api_e(uint32_t paddr)
{
    TEST_ESP_ERR(ESP_FAIL, esp_flash_erase_region(NULL, paddr, SPI_FLASH_SEC_SIZE));
}

static void test_esp_flash_api(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);

    const esp_partition_t *part = NULL;

    switch (boot_count) {
    case 2:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_0, NULL);
        TEST_ASSERT_NOT_NULL(part);
        test_esp_flash_api_w(part->address);
        break;
    case 3:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_1, NULL);
        TEST_ASSERT_NOT_NULL(part);
        test_esp_flash_api_r(part->address);
        break;
    case 4:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, ESP_TEE_SEC_STG_PART_LABEL);
        TEST_ASSERT_NOT_NULL(part);
        test_esp_flash_api_e(part->address);
        break;
    case 5:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_TEE_OTA, NULL);
        TEST_ASSERT_NOT_NULL(part);
        test_esp_flash_api_w(part->address);
        break;
    default:
        TEST_FAIL_MESSAGE("Unexpected stage");
        break;
    }

    esp_restart();
}

TEST_CASE_MULTIPLE_STAGES("Test REE-TEE isolation: Flash - SPI1 (esp_flash)", "[flash_prot][timeout=60]",
                          test_initial_boot, test_esp_flash_api, test_esp_flash_api, test_esp_flash_api,
                          test_esp_flash_api);

/* ---------------------------------------------- API family 4: esp_rom ------------------------------------------------- */

static IRAM_ATTR void test_esp_rom_spiflash_api_r(uint32_t paddr)
{
    uint32_t buf_r[32];
    memset(buf_r, 0x00, sizeof(buf_r));
    esp_rom_spiflash_result_t rc = esp_rom_spiflash_read(paddr, buf_r, sizeof(buf_r));
    TEST_ASSERT_EQUAL_HEX(ESP_ROM_SPIFLASH_RESULT_OK, rc);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buf_r, sizeof(buf_r), ESP_LOG_INFO);
}

static IRAM_ATTR void test_esp_rom_spiflash_api_w(uint32_t paddr)
{
    uint32_t buf_w[32];
    memset(buf_w, 0xA5, sizeof(buf_w));
    spi_flash_disable_interrupts_caches_and_other_cpu();
    esp_rom_spiflash_result_t rc = esp_rom_spiflash_write(paddr, buf_w, sizeof(buf_w));
    spi_flash_enable_interrupts_caches_and_other_cpu();
    TEST_ASSERT_EQUAL_HEX(ESP_ROM_SPIFLASH_RESULT_OK, rc);
}

static IRAM_ATTR void test_esp_rom_spiflash_api_e(uint32_t paddr)
{
    spi_flash_disable_interrupts_caches_and_other_cpu();
    esp_rom_spiflash_result_t rc = esp_rom_spiflash_erase_area(paddr, SPI_FLASH_SEC_SIZE);
    spi_flash_enable_interrupts_caches_and_other_cpu();
    TEST_ASSERT_EQUAL_HEX(ESP_ROM_SPIFLASH_RESULT_OK, rc);
}

static void test_esp_rom_spiflash_api(void)
{
    uint8_t boot_count = get_boot_count_from_nvs();
    boot_count++;
    set_boot_count_in_nvs(boot_count);

    const esp_partition_t *part = NULL;

    switch (boot_count) {
    case 2:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_0, NULL);
        TEST_ASSERT_NOT_NULL(part);
        test_esp_rom_spiflash_api_r(part->address);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    case 3:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_TEE_1, NULL);
        TEST_ASSERT_NOT_NULL(part);
        test_esp_rom_spiflash_api_w(part->address);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    case 4:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, ESP_TEE_SEC_STG_PART_LABEL);
        TEST_ASSERT_NOT_NULL(part);
        test_esp_rom_spiflash_api_e(part->address);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    case 5:
        part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_TEE_OTA, NULL);
        TEST_ASSERT_NOT_NULL(part);
        test_esp_rom_spiflash_api_w(part->address);
        TEST_FAIL_MESSAGE("System fault should have been generated");
        break;
    default:
        TEST_FAIL_MESSAGE("Unexpected stage");
        break;
    }
}

TEST_CASE_MULTIPLE_STAGES("Test REE-TEE isolation: Flash - SPI1 (esp_rom_spiflash)", "[flash_prot][timeout=60]",
                          test_initial_boot, test_esp_rom_spiflash_api, test_esp_rom_spiflash_api,
                          test_esp_rom_spiflash_api, test_esp_rom_spiflash_api);
#endif

TEST_CASE("Test TEE flash read/write performance", "[flash_prot]")
{
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, TEST_PART_LABEL);
    TEST_ASSERT_NOT_NULL(part);

    TEST_ESP_OK(esp_partition_erase_range(part, 0x00, part->size));
    TEST_ASSERT_TRUE((part->size % TEST_BUF_SZ) == 0);

    ESP_LOGI(TAG, "R/W operations over a %luKB partition in %luB chunks...", part->size / 1024, TEST_BUF_SZ);

    uint8_t buf_w[TEST_BUF_SZ];
    memset(buf_w, 0xA5, sizeof(buf_w));

    float write_usec, read_usec;
    ccomp_timer_start();
    for (size_t offs = 0; offs < part->size; offs += TEST_BUF_SZ) {
        TEST_ESP_OK(esp_partition_write(part, offs, buf_w, TEST_BUF_SZ));
    }
    write_usec = ccomp_timer_stop();
    ESP_LOGI(TAG, "[Time taken] Write: %.2fus", write_usec);

    uint8_t buf_r[TEST_BUF_SZ] = {};

    ccomp_timer_start();
    for (size_t offs = 0; offs < part->size; offs += TEST_BUF_SZ) {
        TEST_ESP_OK(esp_partition_read(part, offs, buf_r, TEST_BUF_SZ));
    }
    read_usec = ccomp_timer_stop();

    TEST_ASSERT_EQUAL_HEX8_ARRAY(buf_w, buf_r, TEST_BUF_SZ);
    ESP_LOGI(TAG, "[Time taken] Read: %.2fus", read_usec);
}
