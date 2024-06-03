/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdkconfig.h"

static const char *TAG = "example";

// Mount path for the partition
const char *base_path = "/spiflash";

void write_file(const char *filename, const char *data)
{
    ESP_LOGI(TAG, "Opening file '%s' for writing", filename);

    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", strerror(errno));
        return;
    }

    ESP_LOGI(TAG, "Writing to file");

    fputs(data, f);

    ESP_LOGI(TAG, "File written");

    ESP_LOGI(TAG, "Closing file");
    fclose(f);
}

void read_file(const char *filename)
{
    ESP_LOGI(TAG, "Opening file '%s' for reading", filename);

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading: %s", strerror(errno));
        return;
    }

    ESP_LOGI(TAG, "Reading from file");

    char line[128];
    fgets(line, sizeof(line), f);

    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0'; // strip newline
    }

    ESP_LOGI(TAG, "Read from file: '%s'", line);

    ESP_LOGI(TAG, "Closing file '%s'", filename);
    fclose(f);
}

void stat_file(const char *filename)
{

    struct stat info;
    struct tm timeinfo;
    char buffer[32];

    ESP_LOGI(TAG, "Stating file '%s' for modification time", filename);

    if(stat(filename, &info) < 0){
        ESP_LOGE(TAG, "Failed to read file stats: %s", strerror(errno));
        return;
    }
    localtime_r(&info.st_mtime, &timeinfo);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);

    ESP_LOGI(TAG, "The file '%s' was modified at date: %s", filename, buffer);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Mounting FAT filesystem");

    // To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formatted before
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = false,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
            .use_one_fat = false,
    };

    esp_err_t err;

#ifdef CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY
    err = esp_vfs_fat_spiflash_mount_ro(base_path, "storage", &mount_config);
#else // CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY

    // Handle of the wear levelling library instance
    wl_handle_t wl_handle = WL_INVALID_HANDLE;
    err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage", &mount_config, &wl_handle);
#endif // CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)python $IDF_PATH/tools/ci/ci_build_apps.py . --target esp32 -vv --pytest-apps", esp_err_to_name(err));
        return;
    }

    // Read contents of a file
    read_file("/spiflash/filegeneratedonhost.txt");


#ifndef CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY
    // Create and write to a file
    write_file("/spiflash/messagefromthedevice.txt", "This is written by the device");
    read_file("/spiflash/messagefromthedevice.txt");
#endif // CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY

    // Check when the file was last modified
    stat_file("/spiflash/subdirectoryfromhost/innerfile.txt");

    // Unmount FATFS
    ESP_LOGI(TAG, "Unmounting FAT filesystem");

#ifdef CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY
        ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_ro(base_path, "storage"));
#else // CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY
        ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(base_path, wl_handle));
#endif // CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY

    ESP_LOGI(TAG, "Done");
}
