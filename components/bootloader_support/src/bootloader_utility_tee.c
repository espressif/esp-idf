/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <stdint.h>

#include "esp_attr.h"
#include "esp_log.h"

#include "esp_rom_sys.h"
#include "esp_rom_crc.h"

#include "hal/efuse_hal.h"

#include "esp_image_format.h"
#include "bootloader_config.h"
#include "bootloader_flash_priv.h"

#include "bootloader_utility.h"
#include "bootloader_utility_tee.h"
#include "esp_tee_ota_utils.h"

#include "sdkconfig.h"

static const char *TAG = "boot_tee";

static esp_err_t write_tee_otadata_sector(esp_tee_ota_select_entry_t *tee_otadata, uint32_t offset)
{
    if (tee_otadata == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = bootloader_flash_erase_sector(offset / FLASH_SECTOR_SIZE);
    if (err == ESP_OK) {
        bool write_encrypted = false;
#if !CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
        write_encrypted = efuse_hal_flash_encryption_enabled();
#endif
        err = bootloader_flash_write(offset, tee_otadata, sizeof(esp_tee_ota_select_entry_t), write_encrypted);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write otadata sector, 0x%x", err);
        }
    }

    return err;
}

static esp_err_t read_tee_otadata(const esp_partition_pos_t *tee_ota_info, esp_tee_ota_select_entry_t *two_otadata)
{
    if (tee_ota_info == NULL || two_otadata == NULL || tee_ota_info->offset == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    if (tee_ota_info->size < 2 * FLASH_SECTOR_SIZE) {
        return ESP_ERR_INVALID_SIZE;
    }

    ESP_LOGV(TAG, "TEE OTA data offset 0x%"PRIx32, tee_ota_info->offset);

    const esp_tee_ota_select_entry_t *ota_select_map = bootloader_mmap(tee_ota_info->offset, tee_ota_info->size);
    if (!ota_select_map) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%"PRIx32", 0x%"PRIx32") failed", tee_ota_info->offset, tee_ota_info->size);
        return ESP_FAIL;
    }

    memcpy(&two_otadata[0], (uint8_t *)ota_select_map, sizeof(esp_tee_ota_select_entry_t));
    memcpy(&two_otadata[1], (uint8_t *)ota_select_map + FLASH_SECTOR_SIZE, sizeof(esp_tee_ota_select_entry_t));

    bootloader_munmap(ota_select_map);

    return ESP_OK;
}

static esp_err_t write_tee_otadata(esp_tee_ota_select_entry_t *tee_otadata, const esp_partition_pos_t *tee_ota_info)
{
    esp_err_t err = write_tee_otadata_sector(tee_otadata, tee_ota_info->offset);
    if (err == ESP_OK) {
        err = write_tee_otadata_sector(tee_otadata, tee_ota_info->offset + FLASH_SECTOR_SIZE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to update otadata sector, 0x%x", err);
        }
    }

    return err;
}

static esp_err_t get_valid_tee_otadata(const esp_partition_pos_t *tee_ota_info, esp_tee_ota_select_entry_t *tee_otadata)
{
    esp_tee_ota_select_entry_t two_otadata[2] = {0};
    if (read_tee_otadata(tee_ota_info, two_otadata) != ESP_OK) {
        return ESP_ERR_NOT_FOUND;
    }

    esp_tee_ota_select_entry_t blank_otadata;
    memset(&blank_otadata, 0xff, sizeof(esp_tee_ota_select_entry_t));

    // Check if the contents of both the otadata sectors match
    bool sectors_match = (memcmp(&two_otadata[0], &two_otadata[1], sizeof(esp_tee_ota_select_entry_t)) == 0);
    if (sectors_match) {
        if (memcmp(&two_otadata[0], &blank_otadata, sizeof(esp_tee_ota_select_entry_t)) != 0) {
            uint32_t crc = esp_rom_crc32_le(0, (uint8_t const *)two_otadata, (sizeof(esp_tee_ota_select_entry_t) - sizeof(uint32_t)));
            if (two_otadata[0].magic != TEE_OTADATA_MAGIC || crc != two_otadata[0].crc) {
                ESP_LOGE(TAG, "TEE otadata[0] magic or CRC verification failed");
                return ESP_FAIL;
            }
        }
        memcpy(tee_otadata, &two_otadata[0], sizeof(esp_tee_ota_select_entry_t));
        ESP_LOGV(TAG, "Both tee_otadata sectors are the same");
    } else {
        uint32_t crc_otadata0 = esp_rom_crc32_le(0, (uint8_t const *)&two_otadata[0], (sizeof(esp_tee_ota_select_entry_t) - sizeof(uint32_t)));
        uint32_t crc_otadata1 = esp_rom_crc32_le(0, (uint8_t const *)&two_otadata[1], (sizeof(esp_tee_ota_select_entry_t) - sizeof(uint32_t)));

        if (crc_otadata0 == two_otadata[0].crc) {
            ESP_LOGV(TAG, "Second tee_otadata sector is invalid - copying contents from first sector");
            // Copy contents of first tee_otadata sector into second
            write_tee_otadata_sector(&two_otadata[0], tee_ota_info->offset + FLASH_SECTOR_SIZE);
            memcpy(tee_otadata, &two_otadata[0], sizeof(esp_tee_ota_select_entry_t));
        } else if (crc_otadata1 == two_otadata[1].crc) {
            ESP_LOGV(TAG, "First tee_otadata sector is invalid - copying contents from second sector");
            // Copy contents of second tee_otadata sector into first
            write_tee_otadata_sector(&two_otadata[1], tee_ota_info->offset);
            memcpy(tee_otadata, &two_otadata[1], sizeof(esp_tee_ota_select_entry_t));
        } else {
            ESP_LOGE(TAG, "Both tee_otadata sectors are invalid!");
            abort();
        }
    }

    return ESP_OK;
}

static esp_err_t update_tee_otadata(const esp_partition_pos_t *tee_ota_info, uint8_t boot_partition, uint8_t ota_state)
{
    esp_tee_ota_select_entry_t otadata = {
        .magic = TEE_OTADATA_MAGIC,
        .boot_partition = boot_partition,
        .ota_state = ota_state,
    };
    otadata.crc = esp_rom_crc32_le(0, (uint8_t const *)&otadata, (sizeof(esp_tee_ota_select_entry_t) - sizeof(uint32_t)));
    return write_tee_otadata(&otadata, tee_ota_info);
}

int bootloader_utility_tee_get_boot_partition(const esp_partition_pos_t *tee_ota_info)
{
    esp_tee_ota_select_entry_t otadata = {}, blank_otadata;
    const int default_tee_app_slot = PART_SUBTYPE_TEE_0;

    esp_err_t err = get_valid_tee_otadata(tee_ota_info, &otadata);
    if (err == ESP_ERR_NOT_FOUND) {
        ESP_LOGV(TAG, "otadata partition not found, booting from first partition");
        return default_tee_app_slot;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get valid otadata, 0x%x", err);
        return -1;
    }
    memset(&blank_otadata, 0xff, sizeof(esp_tee_ota_select_entry_t));
    if (!memcmp(&blank_otadata, &otadata, sizeof(esp_tee_ota_select_entry_t))) {
        ESP_LOGV(TAG, "otadata partition empty, booting from first partition");
        /* NOTE: The first TEE partition will always be valid as it is flashed manually */
        if (update_tee_otadata(tee_ota_info, default_tee_app_slot, ESP_TEE_OTA_IMG_VALID) != ESP_OK) {
            ESP_LOGW(TAG, "Failed to setup TEE otadata as per the first partition!");
        }
        return default_tee_app_slot;
    }

    int boot_partition = 0;

#if BOOTLOADER_BUILD
    switch(otadata.ota_state) {
        case ESP_TEE_OTA_IMG_NEW:
            ESP_LOGD(TAG, "TEE otadata - Current image state: NEW");
            boot_partition = otadata.boot_partition;
            if (update_tee_otadata(tee_ota_info, otadata.boot_partition, ESP_TEE_OTA_IMG_PENDING_VERIFY) != ESP_OK) {
                return -1;
            }
            break;
        case ESP_TEE_OTA_IMG_UNDEFINED:
        case ESP_TEE_OTA_IMG_PENDING_VERIFY:
            ESP_LOGD(TAG, "TEE otadata - Current image state: PENDING_VERIFY/UNDEFINED");
            boot_partition = (otadata.boot_partition == PART_SUBTYPE_TEE_0) ? PART_SUBTYPE_TEE_1 : PART_SUBTYPE_TEE_0;
            if (update_tee_otadata(tee_ota_info, boot_partition, ESP_TEE_OTA_IMG_INVALID) != ESP_OK) {
                return -1;
            }
            break;
        case ESP_TEE_OTA_IMG_INVALID:
            ESP_LOGD(TAG, "TEE otadata - Current image state: INVALID");
            bootloader_reset();
            break;
        case ESP_TEE_OTA_IMG_VALID:
            ESP_LOGD(TAG, "TEE otadata - Current image state: VALID");
            boot_partition = otadata.boot_partition;
            break;
            break;
        default:
            break;
    }
#else
    boot_partition = otadata.boot_partition;
#endif

    return boot_partition;
}

esp_err_t bootloader_utility_tee_set_boot_partition(const esp_partition_pos_t *tee_ota_info, const esp_partition_info_t *tee_try_part)
{
    if (tee_ota_info == NULL || tee_try_part == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (tee_try_part->subtype != PART_SUBTYPE_TEE_0 && tee_try_part->subtype != PART_SUBTYPE_TEE_1) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_image_metadata_t data = {};
    if (esp_image_verify(ESP_IMAGE_VERIFY, &tee_try_part->pos, &data) != ESP_OK) {
        return ESP_ERR_IMAGE_INVALID;
    }

    return update_tee_otadata(tee_ota_info, tee_try_part->subtype, ESP_TEE_OTA_IMG_NEW);
}

int bootloader_utility_tee_get_next_update_partition(const esp_partition_pos_t *tee_ota_info)
{
    esp_tee_ota_select_entry_t otadata = {}, blank_otadata;
    const int default_tee_next_app_slot = PART_SUBTYPE_TEE_1;

    esp_err_t err = get_valid_tee_otadata(tee_ota_info, &otadata);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get valid otadata, 0x%x", err);
        return -1;
    }
    memset(&blank_otadata, 0xff, sizeof(esp_tee_ota_select_entry_t));
    if (!memcmp(&blank_otadata, &otadata, sizeof(esp_tee_ota_select_entry_t))) {
        return default_tee_next_app_slot;
    }

    return (otadata.boot_partition == PART_SUBTYPE_TEE_0) ? PART_SUBTYPE_TEE_1 : PART_SUBTYPE_TEE_0;
}

esp_err_t bootloader_utility_tee_mark_app_valid_and_cancel_rollback(const esp_partition_pos_t *tee_ota_info)
{
    esp_tee_ota_select_entry_t two_otadata[2];

    esp_err_t err = read_tee_otadata(tee_ota_info, two_otadata);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fetch TEE otadata!");
        return err;
    }

    if (two_otadata[0].ota_state == ESP_TEE_OTA_IMG_VALID) {
        ESP_LOGD(TAG, "TEE otadata - Current image already has been marked VALID");
        return ESP_ERR_INVALID_STATE;
    }

    int tee_app_slot = bootloader_utility_tee_get_boot_partition(tee_ota_info);
    return update_tee_otadata(tee_ota_info, (uint8_t)tee_app_slot, ESP_TEE_OTA_IMG_VALID);
}
