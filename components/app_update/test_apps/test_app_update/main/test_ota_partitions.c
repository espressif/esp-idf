/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_flash_partitions.h"
#include "esp_flash.h"
#include "spi_flash_mmap.h"
#include "esp_image_format.h"
#include "esp_system.h"
#include "esp_log.h"
#include "unity.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

ESP_LOG_ATTR_TAG(TAG, "test");

static uint8_t buffer[SPI_FLASH_SEC_SIZE];

// Find the unused offset after the last partition, checking that it is of the required size
static uint32_t find_unused_space(size_t required_size)
{
    esp_partition_iterator_t it = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
    TEST_ASSERT_NOT_NULL(it);
    const esp_partition_t* latest_partition = esp_partition_get(it);
    for (; it != NULL; it = esp_partition_next(it)) {
        const esp_partition_t *p = esp_partition_get(it);
        if (p->address > latest_partition->address) {
            latest_partition = p;
        }
    }
    esp_partition_iterator_release(it);
    TEST_ASSERT_NOT_NULL(latest_partition);

    uint32_t flash_chip_size;
    esp_err_t ret = esp_flash_get_size(esp_flash_default_chip, &flash_chip_size);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    uint32_t unused_offset = latest_partition->address + latest_partition->size;
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(required_size, flash_chip_size - unused_offset);
    return unused_offset;
}

static void check_after_reboot(void)
{
    ESP_LOGI(TAG, "App runs");
}

static void download_new_image_from_partition(esp_ota_handle_t update_handle, const esp_partition_t *copy_from_part)
{
    uint32_t offset = 0;
    ESP_LOGI(TAG, "Downloading image...");
    do {
        TEST_ESP_OK(esp_partition_read(copy_from_part, offset, buffer, sizeof(buffer)));
        TEST_ESP_OK(esp_ota_write(update_handle, buffer, sizeof(buffer)));
        offset += sizeof(buffer);
    } while (offset < copy_from_part->size);
}

static void start_bootloader_ota_update_via_ota_bootloader_part(void)
{
    const esp_partition_t *primary_bootloader;
    TEST_ESP_OK(esp_partition_register_external(NULL, ESP_PRIMARY_BOOTLOADER_OFFSET, ESP_BOOTLOADER_SIZE, "PrimaryBTLDR", ESP_PARTITION_TYPE_BOOTLOADER, ESP_PARTITION_SUBTYPE_BOOTLOADER_PRIMARY, &primary_bootloader));

    const esp_partition_t *ota_bootloader;
    const uint32_t ota_bootloader_offset = find_unused_space(ESP_BOOTLOADER_SIZE);
    TEST_ESP_OK(esp_partition_register_external(NULL, ota_bootloader_offset, ESP_BOOTLOADER_SIZE, "OtaBTLDR", ESP_PARTITION_TYPE_BOOTLOADER, ESP_PARTITION_SUBTYPE_BOOTLOADER_OTA, &ota_bootloader));

    esp_ota_handle_t update_handle;
    TEST_ESP_OK(esp_ota_begin(ota_bootloader, OTA_WITH_SEQUENTIAL_WRITES, &update_handle));
    TEST_ESP_OK(esp_ota_set_final_partition(update_handle, primary_bootloader, true));
    download_new_image_from_partition(update_handle, primary_bootloader);
    TEST_ESP_OK(esp_ota_end(update_handle));

    TEST_ESP_OK(esp_partition_deregister_external(primary_bootloader));
    TEST_ESP_OK(esp_partition_deregister_external(ota_bootloader));
    esp_restart();
}

TEST_CASE_MULTIPLE_STAGES("OTA update of bootloader via temp partition", "[bootloader_ota][reset=SW_CPU_RESET]", start_bootloader_ota_update_via_ota_bootloader_part, check_after_reboot);

static void start_bootloader_ota_update_via_primary_bootloader_part(void)
{
    const esp_partition_t *primary_bootloader;
    TEST_ESP_OK(esp_partition_register_external(NULL, ESP_PRIMARY_BOOTLOADER_OFFSET, ESP_BOOTLOADER_SIZE, "PrimaryBTLDR", ESP_PARTITION_TYPE_BOOTLOADER, ESP_PARTITION_SUBTYPE_BOOTLOADER_PRIMARY, &primary_bootloader));

    esp_ota_handle_t update_handle;
    TEST_ESP_OK(esp_ota_begin(primary_bootloader, OTA_WITH_SEQUENTIAL_WRITES, &update_handle));
    download_new_image_from_partition(update_handle, primary_bootloader);
    TEST_ESP_OK(esp_ota_end(update_handle));

    TEST_ESP_OK(esp_partition_deregister_external(primary_bootloader));
    esp_restart();
}

TEST_CASE_MULTIPLE_STAGES("OTA update of bootloader via primary partition", "[bootloader_ota][reset=SW_CPU_RESET]", start_bootloader_ota_update_via_primary_bootloader_part, check_after_reboot);

static void start_partition_table_ota_update_via_ota_part_table(void)
{
    const esp_partition_t *primary_partition_table;
    const esp_partition_t *ota_partition_table;
    TEST_ESP_OK(esp_partition_register_external(NULL, ESP_PRIMARY_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_SIZE, "PrimaryPrtTable", ESP_PARTITION_TYPE_PARTITION_TABLE, ESP_PARTITION_SUBTYPE_PARTITION_TABLE_PRIMARY, &primary_partition_table));

    uint32_t ota_partition_table_offset = find_unused_space(ESP_PARTITION_TABLE_SIZE);
    TEST_ESP_OK(esp_partition_register_external(NULL, ota_partition_table_offset, ESP_PARTITION_TABLE_SIZE, "OtaPrtTable", ESP_PARTITION_TYPE_PARTITION_TABLE, ESP_PARTITION_SUBTYPE_PARTITION_TABLE_OTA, &ota_partition_table));

    esp_ota_handle_t update_handle;
    TEST_ESP_OK(esp_ota_begin(ota_partition_table, OTA_WITH_SEQUENTIAL_WRITES, &update_handle));
    TEST_ESP_OK(esp_ota_set_final_partition(update_handle, primary_partition_table, true));
    download_new_image_from_partition(update_handle, primary_partition_table);
    TEST_ESP_OK(esp_ota_end(update_handle));

    TEST_ESP_OK(esp_partition_deregister_external(primary_partition_table));
    TEST_ESP_OK(esp_partition_deregister_external(ota_partition_table));
    esp_restart();
}

TEST_CASE_MULTIPLE_STAGES("OTA update of partition_table via temp partition", "[partition_table_ota][reset=SW_CPU_RESET]", start_partition_table_ota_update_via_ota_part_table, check_after_reboot);

static void start_partition_table_ota_update_via_primary_part_table(void)
{
    const esp_partition_t *primary_partition_table;
    TEST_ESP_OK(esp_partition_register_external(NULL, ESP_PRIMARY_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_SIZE, "PrimaryPrtTable", ESP_PARTITION_TYPE_PARTITION_TABLE, ESP_PARTITION_SUBTYPE_PARTITION_TABLE_PRIMARY, &primary_partition_table));

    esp_ota_handle_t update_handle;
    TEST_ESP_OK(esp_ota_begin(primary_partition_table, OTA_WITH_SEQUENTIAL_WRITES, &update_handle));
    download_new_image_from_partition(update_handle, primary_partition_table);
    TEST_ESP_OK(esp_ota_end(update_handle));

    TEST_ESP_OK(esp_partition_deregister_external(primary_partition_table));
    esp_restart();
}

TEST_CASE_MULTIPLE_STAGES("OTA update of partition_table via primary partition", "[partition_table_ota][reset=SW_CPU_RESET]", start_partition_table_ota_update_via_primary_part_table, check_after_reboot);

TEST_CASE("OTA update of NVS partition", "[nvs_ota]")
{
    // initialize "nvs" partition and define a var (magic_value).
    TEST_ESP_OK(nvs_flash_erase());
    TEST_ESP_OK(nvs_flash_init());
    nvs_handle_t my_handle;
    TEST_ESP_OK(nvs_open("namespace", NVS_READWRITE, &my_handle));
    uint32_t magic_value = 0x0729FEED;
    TEST_ESP_OK(nvs_set_u32(my_handle, "magic_value", magic_value));
    TEST_ESP_OK(nvs_commit(my_handle));
    magic_value = 0;
    TEST_ESP_OK(nvs_get_u32(my_handle, "magic_value", &magic_value));
    TEST_ASSERT_EQUAL_HEX(0x0729FEED, magic_value);
    nvs_close(my_handle);
    TEST_ESP_OK(nvs_flash_deinit());

    // register a new "nvs2" partition
    const esp_partition_t *nvs_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
    const esp_partition_t *nvs2_part;
    TEST_ESP_OK(esp_partition_register_external(NULL, find_unused_space(nvs_part->size), nvs_part->size, "nvs2", ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, &nvs2_part));
    ESP_LOGI(TAG, "Use %s partition (0x%08" PRIx32 ") to load a new image", nvs2_part->label, nvs2_part->address);
    TEST_ESP_OK(nvs_flash_erase_partition("nvs2"));

    // OTA update of the new "nvs2" partition, taking "nvs" partition as source.
    esp_ota_handle_t update_handle;
    TEST_ESP_OK(esp_ota_begin(nvs2_part, OTA_WITH_SEQUENTIAL_WRITES, &update_handle));
    download_new_image_from_partition(update_handle, nvs_part);
    TEST_ESP_OK(esp_ota_end(update_handle));

    // init "nvs2" partition and check if the magic_value == 0x0729FEED
    TEST_ESP_OK(nvs_flash_init_partition("nvs2"));
    nvs_handle_t my_handle2;
    TEST_ESP_OK(nvs_open_from_partition("nvs2", "namespace", NVS_READWRITE, &my_handle2));
    magic_value = 0;
    TEST_ESP_OK(nvs_get_u32(my_handle2, "magic_value", &magic_value));
    TEST_ASSERT_EQUAL_HEX(0x0729FEED, magic_value);
    nvs_close(my_handle2);
    TEST_ESP_OK(nvs_flash_deinit_partition("nvs2"));

    // deregister "nvs2"
    TEST_ESP_OK(esp_partition_deregister_external(nvs2_part));

    TEST_ESP_OK(nvs_flash_erase());
}

static void start_bootloader_ota_update_via_app_part(void)
{
    const esp_partition_t *primary_bootloader;
    TEST_ESP_OK(esp_partition_register_external(NULL, ESP_PRIMARY_BOOTLOADER_OFFSET, ESP_BOOTLOADER_SIZE, "PrimaryBTLDR", ESP_PARTITION_TYPE_BOOTLOADER, ESP_PARTITION_SUBTYPE_BOOTLOADER_PRIMARY, &primary_bootloader));

    const esp_partition_t *free_app_ota_partition = esp_ota_get_next_update_partition(NULL);
    esp_ota_handle_t update_handle;
    TEST_ESP_OK(esp_ota_begin(free_app_ota_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle));
    TEST_ESP_OK(esp_ota_set_final_partition(update_handle, primary_bootloader, true));
    download_new_image_from_partition(update_handle, primary_bootloader);
    TEST_ESP_OK(esp_ota_end(update_handle));

    TEST_ESP_OK(esp_partition_deregister_external(primary_bootloader));
    esp_restart();
}

TEST_CASE_MULTIPLE_STAGES("OTA update of bootloader via a free ota partition", "[bootloader_ota][reset=SW_CPU_RESET]", start_bootloader_ota_update_via_app_part, check_after_reboot);

static void start_partition_table_ota_update_via_app_part(void)
{
    const esp_partition_t *primary_partition_table;
    TEST_ESP_OK(esp_partition_register_external(NULL, ESP_PRIMARY_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_SIZE, "PrimaryPrtTable", ESP_PARTITION_TYPE_PARTITION_TABLE, ESP_PARTITION_SUBTYPE_PARTITION_TABLE_PRIMARY, &primary_partition_table));

    const esp_partition_t *free_app_ota_partition = esp_ota_get_next_update_partition(NULL);
    esp_ota_handle_t update_handle;
    TEST_ESP_OK(esp_ota_begin(free_app_ota_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle));
    TEST_ESP_OK(esp_ota_set_final_partition(update_handle, primary_partition_table, true));
    download_new_image_from_partition(update_handle, primary_partition_table);
    TEST_ESP_OK(esp_ota_end(update_handle));

    TEST_ESP_OK(esp_partition_deregister_external(primary_partition_table));
    esp_restart();
}

TEST_CASE_MULTIPLE_STAGES("OTA update of partition_table via a free ota partition", "[partition_table_ota][reset=SW_CPU_RESET]", start_partition_table_ota_update_via_app_part, check_after_reboot);

TEST_CASE("OTA update of NVS partition via a free ota partition", "[nvs_ota]")
{
    // initialize "nvs" partition and define a var (magic_value).
    const esp_partition_t *nvs_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
    TEST_ESP_OK(nvs_flash_erase());
    TEST_ESP_OK(nvs_flash_init());
    nvs_handle_t my_handle;
    TEST_ESP_OK(nvs_open("namespace", NVS_READWRITE, &my_handle));
    uint32_t magic_value = 0x0729FEED;
    TEST_ESP_OK(nvs_set_u32(my_handle, "magic_value", magic_value));
    TEST_ESP_OK(nvs_commit(my_handle));
    magic_value = 0;
    TEST_ESP_OK(nvs_get_u32(my_handle, "magic_value", &magic_value));
    TEST_ASSERT_EQUAL_HEX(0x0729FEED, magic_value);
    nvs_close(my_handle);
    TEST_ESP_OK(nvs_flash_deinit());

    // 1. OTA update nvs partition into free_app_ota_partition
    // 2. copy free_app_ota_partition into the original nvs partition (which was erased before coping)
    const esp_partition_t *free_app_ota_partition = esp_ota_get_next_update_partition(NULL);
    esp_ota_handle_t update_handle;
    TEST_ESP_OK(esp_ota_begin(free_app_ota_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle));
    TEST_ESP_OK(esp_ota_set_final_partition(update_handle, nvs_part, true));
    download_new_image_from_partition(update_handle, nvs_part);
    TEST_ESP_OK(esp_ota_end(update_handle));

    // Check if the magic_value == 0x0729FEED
    TEST_ESP_OK(nvs_flash_init());
    TEST_ESP_OK(nvs_open("namespace", NVS_READONLY, &my_handle));
    magic_value = 0;
    TEST_ESP_OK(nvs_get_u32(my_handle, "magic_value", &magic_value));
    TEST_ASSERT_EQUAL_HEX(0x0729FEED, magic_value);
    nvs_close(my_handle);
    TEST_ESP_OK(nvs_flash_deinit());
    TEST_ESP_OK(nvs_flash_erase());
}
