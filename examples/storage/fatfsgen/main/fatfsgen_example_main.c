/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdkconfig.h"

#if CONFIG_EXAMPLE_FATFS_MODE_READ_ONLY
#define EXAMPLE_FATFS_MODE_READ_ONLY true
#else
#define EXAMPLE_FATFS_MODE_READ_ONLY false
#endif

#if CONFIG_FATFS_LFN_NONE
#define EXAMPLE_FATFS_LONG_NAMES false
#else
#define EXAMPLE_FATFS_LONG_NAMES true
#endif

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
            .max_files = 4,
            .format_if_mount_failed = false,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
            .use_one_fat = false,
    };
    esp_err_t err;
    if (EXAMPLE_FATFS_MODE_READ_ONLY){
        err = esp_vfs_fat_spiflash_mount_ro(base_path, "storage", &mount_config);
    } else {
        err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage", &mount_config, &s_wl_handle);
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }

    char line[128];
    char *device_filename;
    if (EXAMPLE_FATFS_LONG_NAMES){
        device_filename = "/spiflash/innerbutverylongname.txt";
    } else {
        device_filename = "/spiflash/inner.txt";
    }

    if (!EXAMPLE_FATFS_MODE_READ_ONLY){
        // Open file for reading
        ESP_LOGI(TAG, "Opening file");
        FILE *f;
        for(int i = 0; i < CONFIG_EXAMPLE_FATFS_WRITE_COUNT; i++){
            f = fopen(device_filename, "wb");
            if (f == NULL) {
                ESP_LOGE(TAG, "Failed to open file for writing");
                return;
            }
            fprintf(f, "This is written by the device");
            fclose(f);
        }

        ESP_LOGI(TAG, "File written");

        // Open file for reading
        ESP_LOGI(TAG, "Reading file");
        f = fopen(device_filename, "rb");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for reading");
            return;
        }
        fgets(line, sizeof(line), f);
        fclose(f);
        // strip newline
        char *pos = strchr(line, '\n');
        if (pos) {
            *pos = '\0';
        }
        ESP_LOGI(TAG, "Read from file: '%s'", line);

    }
    FILE *f;
    char *pos;
    ESP_LOGI(TAG, "Reading file");
    char *host_filename1;
    char *host_filename2;

    if (EXAMPLE_FATFS_LONG_NAMES){
        host_filename1 = "/spiflash/sublongnames/testlongfilenames.txt";
        host_filename2 = "/spiflash/hellolongname.txt";
    } else{
        host_filename1 = "/spiflash/sub/test.txt";
        host_filename2 = "/spiflash/hello.txt";
    }

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


    if (EXAMPLE_FATFS_MODE_READ_ONLY){
        f = fopen(host_filename1, "rb");
    } else {
        f = fopen(host_filename2, "rb");
    }
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
    if (EXAMPLE_FATFS_MODE_READ_ONLY){
        ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_ro(base_path, "storage"));
    } else {
        ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_wl_handle));
    }
    ESP_LOGI(TAG, "Done");
}
