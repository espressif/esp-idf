/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include "wear_levelling.h"
#include "esp_partition.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"
#include "ff.h"
#include "esp_debug_helpers.h"

static const char* TAG = "Test dynamic buffers";

static volatile bool g_alloc_count_enable = false;
static volatile int g_buffer_alloc_count = 0;

static esp_vfs_fat_mount_config_t g_mount_config = {
    .format_if_mount_failed = true,
    .max_files = 5,
};

void esp_heap_trace_alloc_hook(void* ptr, size_t size, uint32_t caps)
{
    (void) ptr;
    (void) caps;

    if (!g_alloc_count_enable) {
        return;
    }

    // This will work only on SPI flash
    // Different flash types might break this check
    if (size == FF_MAX_SS) {
        g_buffer_alloc_count++;
    }
}

void app_main(void)
{
    esp_err_t err = ESP_OK;

    wl_handle_t wl_handle;

    err = esp_vfs_fat_spiflash_format_cfg_rw_wl("/spiflash", NULL, &g_mount_config);

    ESP_LOGI(TAG, "Mounting FATFS");

    g_mount_config.format_if_mount_failed = false,

    g_alloc_count_enable = true;

    err = esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &g_mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "FATFS mount failed with error: %d", err);
        return;
    }

    ESP_LOGI(TAG, "Mounted");

    int fd = open("/spiflash/test.txt", O_RDWR|O_CREAT);
    if (fd < 0) {
        ESP_LOGE(TAG, "Failed opening file");
    }

    close(fd);

    g_alloc_count_enable = false;

    ESP_LOGI(TAG, "Unmounting FATFS");

    esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle);

    ESP_LOGI(TAG, "Unmounted");

    ESP_LOGI(TAG, "Allocs called:\n\tBuffer: %d"
                , g_buffer_alloc_count);

#if CONFIG_FATFS_USE_DYN_BUFFERS

    if (g_buffer_alloc_count != 2) {
        ESP_LOGE(TAG, "FATFS buffer should have been allocated once for each context (file and fatfs)");
        return;
    }
#else

    if (g_buffer_alloc_count != 0) {
        ESP_LOGE(TAG, "FATFS buffer should not have been allocated");
        return;
    }

#endif

    ESP_LOGI(TAG, "Done");
}
