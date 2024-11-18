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
#include "sdkconfig.h"

static const char *TAG = "example";

// Mount path for the partition
const char *base_path = "/spiflash";

// Handle of the wear levelling library instance
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

void app_main(void)
{
    ESP_LOGI(TAG, "Mounting FAT filesystem");
    // To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formatted before
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4, // Number of files that can be open at a time
            .format_if_mount_failed = true, // If true, try to format the partition if mount fails
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE, // Size of allocation unit, cluster size.
            .use_one_fat = false, // Use only one FAT table (reduce memory usage), but decrease reliability of file system in case of power failure.
    };

    // Mount FATFS filesystem located on "storage" partition in read-write mode
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage", &mount_config, &s_wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Filesystem mounted");

    ESP_LOGI(TAG, "Opening file");

    const char *filename = "/spiflash/example.txt";

    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        perror("fopen"); // Print reason why fopen failed
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    fprintf(f, "Hello World!\n");
    fclose(f);

    ESP_LOGI(TAG, "File written");

    // Open file for reading
    ESP_LOGI(TAG, "Reading file");

    f = fopen(filename, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    char line[128];

    fgets(line, sizeof(line), f);
    fclose(f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }

    ESP_LOGI(TAG, "Read from file: '%s'", line);

    // Unmount FATFS
    ESP_LOGI(TAG, "Unmounting FAT filesystem");

    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_wl_handle));

    ESP_LOGI(TAG, "Done");
}
