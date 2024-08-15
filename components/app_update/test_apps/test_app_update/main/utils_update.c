/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_rom_spiflash.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_partition.h"
#include "esp_flash_partitions.h"
#include "esp_image_format.h"
#include "../bootloader_flash/include/bootloader_flash_priv.h"
#include "esp_sleep.h"
#include "esp_ota_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "test_utils.h"
#include "utils_update.h"
#include "unity.h"
#include "sdkconfig.h"

#define BOOT_COUNT_NAMESPACE "boot_count"

static const char *TAG = "ota_test";


void set_boot_count_in_nvs(uint8_t boot_count)
{
    nvs_handle_t boot_count_handle;
    esp_err_t err = nvs_open(BOOT_COUNT_NAMESPACE, NVS_READWRITE, &boot_count_handle);
    if (err != ESP_OK) {
        TEST_ESP_OK(nvs_flash_erase());
        TEST_ESP_OK(nvs_flash_init());
        TEST_ESP_OK(nvs_open(BOOT_COUNT_NAMESPACE, NVS_READWRITE, &boot_count_handle));
    }
    TEST_ESP_OK(nvs_set_u8(boot_count_handle, "boot_count", boot_count));
    TEST_ESP_OK(nvs_commit(boot_count_handle));
    nvs_close(boot_count_handle);
}

uint8_t get_boot_count_from_nvs(void)
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

/* @brief Copies a current app to next partition using handle.
 *
 * @param[in] update_handle - Handle of API ota.
 * @param[in] cur_app - Current app.
 */
void copy_app_partition(esp_ota_handle_t update_handle, const esp_partition_t *curr_app)
{
    const void *partition_bin = NULL;
    esp_partition_mmap_handle_t data_map;
    ESP_LOGI(TAG, "start the copy process");
    TEST_ESP_OK(esp_partition_mmap(curr_app, 0, curr_app->size, ESP_PARTITION_MMAP_DATA, &partition_bin, &data_map));
    TEST_ESP_OK(esp_ota_write(update_handle, (const void *)partition_bin, curr_app->size));
    esp_partition_munmap(data_map);
    ESP_LOGI(TAG, "finish the copy process");
}

/* @brief Copies a current app to next partition using handle.
 *
 * @param[in] update_handle - Handle of API ota.
 * @param[in] cur_app - Current app.
 */
void copy_app_partition_with_offset(esp_ota_handle_t update_handle, const esp_partition_t *curr_app)
{
    const void *partition_bin = NULL;
    esp_partition_mmap_handle_t  data_map;
    ESP_LOGI(TAG, "start the copy process");
    uint32_t offset = 0, bytes_to_write = curr_app->size;
    uint32_t write_bytes;
    while (bytes_to_write > 0) {
        write_bytes = (bytes_to_write > (4 * 1024)) ? (4 * 1024) : bytes_to_write;
        TEST_ESP_OK(esp_partition_mmap(curr_app, offset, write_bytes, ESP_PARTITION_MMAP_DATA, &partition_bin, &data_map));
        TEST_ESP_OK(esp_ota_write_with_offset(update_handle, (const void *)partition_bin, write_bytes, offset));
        esp_partition_munmap(data_map);
        bytes_to_write -= write_bytes;
        offset += write_bytes;
    }
    ESP_LOGI(TAG, "finish the copy process");
}

/* @brief Get the next partition of OTA for the update.
 *
 * @return The next partition of OTA(OTA0-15).
 */
const esp_partition_t * get_next_update_partition(void)
{
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, update_partition);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%"PRIx32, update_partition->subtype, update_partition->address);
    return update_partition;
}

/* @brief Copies a current app to next partition (OTA0-15) and then configure OTA data for a new boot partition.
 *
 * @param[in] cur_app_partition - Current app.
 * @param[in] next_app_partition - Next app for boot.
 */
void copy_current_app_to_next_part(const esp_partition_t *cur_app_partition, const esp_partition_t *next_app_partition)
{
    esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, next_app_partition);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%"PRIx32, next_app_partition->subtype, next_app_partition->address);

    esp_ota_handle_t update_handle = 0;
    TEST_ESP_OK(esp_ota_begin(next_app_partition, OTA_SIZE_UNKNOWN, &update_handle));

    copy_app_partition(update_handle, cur_app_partition);

    TEST_ESP_OK(esp_ota_end(update_handle));
    TEST_ESP_OK(esp_ota_set_boot_partition(next_app_partition));
}

/* @brief Copies a current app to next partition (OTA0-15) and then configure OTA data for a new boot partition.
 *
 * @param[in] cur_app_partition - Current app.
 * @param[in] next_app_partition - Next app for boot.
 */
void copy_current_app_to_next_part_with_offset(const esp_partition_t *cur_app_partition, const esp_partition_t *next_app_partition)
{
    esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, next_app_partition);
    ESP_LOGI(TAG, "Writing to partition subtype %d at offset 0x%"PRIx32, next_app_partition->subtype, next_app_partition->address);

    esp_ota_handle_t update_handle = 0;
    TEST_ESP_OK(esp_ota_begin(next_app_partition, OTA_SIZE_UNKNOWN, &update_handle));

    copy_app_partition_with_offset(update_handle, cur_app_partition);

    TEST_ESP_OK(esp_ota_end(update_handle));
    TEST_ESP_OK(esp_ota_set_boot_partition(next_app_partition));
}

/* @brief Erase otadata partition
 */
void erase_ota_data(void)
{
    const esp_partition_t *data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, data_partition);
    TEST_ESP_OK(esp_partition_erase_range(data_partition, 0, 2 * data_partition->erase_size));
}

/* @brief Reboots ESP using mode deep sleep. This mode guaranty that RTC_DATA_ATTR variables is not reset.
 */
void reboot_as_deep_sleep(void)
{
    ESP_LOGI(TAG, "reboot as deep sleep");
    esp_deep_sleep(20000);
    TEST_FAIL_MESSAGE("Should never be reachable except when sleep is rejected, abort");
}

/* @brief Copies a current app to next partition (OTA0-15), after that ESP is rebooting and run this (the next) OTAx.
 */
void copy_current_app_to_next_part_and_reboot(void)
{
    const esp_partition_t *cur_app = esp_ota_get_running_partition();
    ESP_LOGI(TAG, "copy current app to next part");
    copy_current_app_to_next_part(cur_app, get_next_update_partition());
    reboot_as_deep_sleep();
}

/* @brief Copies a current app to next partition (OTA0-15) using esp_ota_write_with_offest(), after that ESP is rebooting and run this (the next) OTAx.
 */
void copy_current_app_to_next_part_with_offset_and_reboot(void)
{
    const esp_partition_t *cur_app = esp_ota_get_running_partition();
    ESP_LOGI(TAG, "copy current app to next part");
    copy_current_app_to_next_part_with_offset(cur_app, get_next_update_partition());
    reboot_as_deep_sleep();
}

/* @brief Get running app.
 *
 * @return The next partition of OTA(OTA0-15).
 */
const esp_partition_t* get_running_firmware(void)
{
    const esp_partition_t *configured = esp_ota_get_boot_partition();
    const esp_partition_t *running = esp_ota_get_running_partition();
    // If a reboot hasn't occurred after app_update(), the configured and running partitions may differ
    ESP_LOGI(TAG, "Running partition type %d subtype %d (offset 0x%08"PRIx32")",
            running->type, running->subtype, running->address);
    ESP_LOGI(TAG, "Configured partition type %d subtype %d (offset 0x%08"PRIx32")",
            configured->type, configured->subtype, configured->address);
    TEST_ASSERT_NOT_EQUAL(NULL, configured);
    TEST_ASSERT_NOT_EQUAL(NULL, running);
    return running;
}

/* @brief Get two copies ota_data from otadata partition.
 *
 * @param[in] otadata_partition - otadata partition.
 * @param[out] ota_data_0 - First copy from otadata_partition.
 * @param[out] ota_data_1 - Second copy from otadata_partition.
 */
void get_ota_data(const esp_partition_t *otadata_partition, esp_ota_select_entry_t *ota_data_0, esp_ota_select_entry_t *ota_data_1)
{
    uint32_t offset = otadata_partition->address;
    uint32_t size   = otadata_partition->size;
    if (offset != 0) {
        const esp_ota_select_entry_t *ota_select_map;
        ota_select_map = bootloader_mmap(offset, size);
        TEST_ASSERT_NOT_EQUAL(NULL, ota_select_map);

        memcpy(ota_data_0, ota_select_map, sizeof(esp_ota_select_entry_t));
        memcpy(ota_data_1, (uint8_t *)ota_select_map + otadata_partition->erase_size, sizeof(esp_ota_select_entry_t));
        bootloader_munmap(ota_select_map);
    }
}

/* @brief Writes a ota_data into required sector of otadata_partition.
 *
 * @param[in] otadata_partition - Partition information otadata.
 * @param[in] ota_data - otadata structure.
 * @param[in] sec_id - Sector number 0 or 1.
 */
void write_ota_data(const esp_partition_t *otadata_partition, esp_ota_select_entry_t *ota_data, int sec_id)
{
    esp_partition_write(otadata_partition, otadata_partition->erase_size * sec_id, &ota_data[sec_id], sizeof(esp_ota_select_entry_t));
}

/* @brief Makes a corrupt of ota_data.
 * @param[in] err - type error
 */
void corrupt_ota_data(corrupt_ota_data_t err)
{
    esp_ota_select_entry_t ota_data[2];

    const esp_partition_t *otadata_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_OTA, NULL);
    TEST_ASSERT_NOT_EQUAL(NULL, otadata_partition);
    get_ota_data(otadata_partition, &ota_data[0], &ota_data[1]);

    if (err & CORR_CRC_1_SECTOR_OTA_DATA) {
        ota_data[0].crc = 0;
    }
    if (err & CORR_CRC_2_SECTOR_OTA_DATA) {
        ota_data[1].crc = 0;
    }
    TEST_ESP_OK(esp_partition_erase_range(otadata_partition, 0, otadata_partition->size));
    write_ota_data(otadata_partition, &ota_data[0], 0);
    write_ota_data(otadata_partition, &ota_data[1], 1);
}

#if defined(CONFIG_BOOTLOADER_FACTORY_RESET) || defined(CONFIG_BOOTLOADER_APP_TEST)
/* @brief Sets the pin number to output and sets output level as low. After reboot (deep sleep) this pin keep the same level.
 *
 * The output level of the pad will be force locked and can not be changed.
 * Power down or call gpio_hold_dis will disable this function.
 *
 * @param[in] num_pin - Pin number
 */
void set_output_pin(uint32_t num_pin)
{
    TEST_ESP_OK(gpio_hold_dis(num_pin));

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << num_pin);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    TEST_ESP_OK(gpio_config(&io_conf));

    TEST_ESP_OK(gpio_set_level(num_pin, 0));
    TEST_ESP_OK(gpio_hold_en(num_pin));
}

/* @brief Unset the pin number hold function.
 */
void reset_output_pin(uint32_t num_pin)
{
    TEST_ESP_OK(gpio_hold_dis(num_pin));
    TEST_ESP_OK(gpio_reset_pin(num_pin));
}
#endif

void mark_app_valid(void)
{
#ifdef CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
    TEST_ESP_OK(esp_ota_mark_app_valid_cancel_rollback());
#endif
}

const esp_partition_t* app_update(void)
{
    const esp_partition_t *cur_app = get_running_firmware();
    const esp_partition_t* update_partition = esp_ota_get_next_update_partition(NULL);
    TEST_ASSERT_NOT_NULL(update_partition);
    esp_ota_handle_t update_handle = 0;
    TEST_ESP_OK(esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle));
    copy_app_partition(update_handle, cur_app);
    TEST_ESP_OK(esp_ota_end(update_handle));
    TEST_ESP_OK(esp_ota_set_boot_partition(update_partition));
    return update_partition;
}
