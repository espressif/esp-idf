/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_vfs_fat.h"
#include "esp_partition.h"
#include "esp_check.h"

static const char *TAG = "flash_fat";

static wl_handle_t s_wlh = WL_INVALID_HANDLE;

esp_err_t esp_probe_init_spiflash_fatfs(const char *mount_point, const char *partition_label, uint32_t *size)
{
    // Get the partition info
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, partition_label);
    ESP_RETURN_ON_FALSE(part, ESP_ERR_NOT_FOUND, TAG, "Failed to find the partition with label %s", partition_label);
    ESP_LOGI(TAG, "Probe data partition base addr: 0x%lx size: 0x%lx", part->address, part->size);
    if (size) {
        *size = (uint32_t)part->size;
    }

    // Format and mount the partition of storage
    esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .disk_status_check_enable = false
    };
    ESP_RETURN_ON_ERROR(esp_vfs_fat_spiflash_mount_rw_wl(mount_point, partition_label, &mount_config, &s_wlh),
                        TAG, "Failed to mount FATFS");
    ESP_LOGI(TAG, "flash FATFS mounted");
    return ESP_OK;
}

void esp_probe_deinit_spiflash_fatfs(const char *mount_point)
{
    esp_vfs_fat_spiflash_unmount_rw_wl(mount_point, s_wlh);
}
