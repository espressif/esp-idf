/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_err.h"
#include "sdkconfig.h"

static const char *TAG = "example";

// Mount path for the partition
const char *base_path = "/spiflash";


void app_main(void)
{
    // To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formatted before
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = false,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
            .use_one_fat = false,
    };

#if CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY
    ESP_LOGI(TAG, "Mounting FAT filesystem in read-only mode");
    esp_err_t err = esp_vfs_fat_spiflash_mount_ro(base_path, "storage", &mount_config);
#else
    ESP_LOGI(TAG, "Mounting FAT filesystem in read/write mode");
    static wl_handle_t wl_handle = WL_INVALID_HANDLE;
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage", &mount_config, &wl_handle);
#endif

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    char line[128];

    FILE *f;
    char *pos;
    ESP_LOGI(TAG, "Reading file");

    const char *host_filename1 = "/spiflash/subdir/testlongfilenames.txt";

    struct stat info;
    struct tm timeinfo;
    char buffer[32];

    if(stat(host_filename1, &info) < 0){
        ESP_LOGE(TAG, "Failed to read file stats");
        return;
    }
    localtime_r(&info.st_mtime, &timeinfo);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);

    ESP_LOGI(TAG, "The file '%s' was modified at date: %s", host_filename1, buffer);


    f = fopen(host_filename1, "rb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    // Unmount FATFS
    ESP_LOGI(TAG, "Unmounting FAT filesystem");

#if CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_ro(base_path, "storage"));
#else
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(base_path, wl_handle));
#endif

    ESP_LOGI(TAG, "Done");
}
