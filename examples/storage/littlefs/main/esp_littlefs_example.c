/*
 * SPDX-FileCopyrightText: 2023 Brian Pugh
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_littlefs.h"

static const char *TAG = "esp_littlefs";

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing LittleFS");

    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/littlefs",
        .partition_label = "storage",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    // Use settings defined above to initialize and mount LittleFS filesystem.
    // Note: esp_vfs_littlefs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));
        esp_littlefs_format(conf.partition_label);
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    // Use POSIX and C standard library functions to work with files.
    // First create a file.
    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen("/littlefs/hello.txt", "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello World!\n");
    fclose(f);
    ESP_LOGI(TAG, "File written");

    // Check if destination file exists before renaming
    struct stat st;
    if (stat("/littlefs/foo.txt", &st) == 0) {
        // Delete it if it exists
        unlink("/littlefs/foo.txt");
    }

    // Rename original file
    ESP_LOGI(TAG, "Renaming file");
    if (rename("/littlefs/hello.txt", "/littlefs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        return;
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/littlefs/foo.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    char line[128];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char*pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    ESP_LOGI(TAG, "Reading from flashed filesystem example.txt");
    f = fopen("/littlefs/example.txt", "r");
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

    // All done, unmount partition and disable LittleFS
    esp_vfs_littlefs_unregister(conf.partition_label);
    ESP_LOGI(TAG, "LittleFS unmounted");
}
