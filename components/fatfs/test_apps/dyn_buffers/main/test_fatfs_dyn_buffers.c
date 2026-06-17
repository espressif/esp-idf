/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "wear_levelling.h"
#include "diskio_wl.h"
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

// Some resources are lazy allocated, the threshold is left for that case
#define TEST_MEMORY_LEAK_THRESHOLD (1280)

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    esp_partition_unload_all();     //clean up some of the esp_partition's allocations
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

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

TEST_CASE("(dyn_buffers) basic mounting and file operations", "[fatfs][dyn_buffers]")
{
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };

    wl_handle_t wl_handle;
    esp_vfs_fat_spiflash_format_cfg_rw_wl("/spiflash", NULL, &mount_config);

    ESP_LOGI(TAG, "Mounting FATFS");

    mount_config.format_if_mount_failed = false,

    g_alloc_count_enable = true;

    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash", NULL, &mount_config, &wl_handle));

    ESP_LOGI(TAG, "Mounted");

    int fd = open("/spiflash/test.txt", O_RDWR|O_CREAT);
    TEST_ASSERT(fd >= 0);

    close(fd);

    g_alloc_count_enable = false;

    ESP_LOGI(TAG, "Unmounting FATFS");

    esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash", wl_handle);

    ESP_LOGI(TAG, "Unmounted");

    ESP_LOGI(TAG, "Allocs called:\n\tBuffer: %d"
                , g_buffer_alloc_count);

#if CONFIG_FATFS_USE_DYN_BUFFERS
    TEST_ASSERT_MESSAGE(g_buffer_alloc_count == 2, "FATFS buffer should have been allocated once for each context (file and fatfs)");
#else
    TEST_ASSERT_MESSAGE(g_buffer_alloc_count == 0, "FATFS buffer should not have been allocated");
#endif

    ESP_LOGI(TAG, "Done");
}

TEST_CASE("(dyn_buffers) File system data does not exist, format and remount", "[fatfs][dyn_buffers]")
{
    const char test_par_lab[] = "storage2";

    const esp_partition_t * part  = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, test_par_lab);
    TEST_ASSERT(part != NULL);
    esp_partition_erase_range(part, 0, part->size);


    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };
    wl_handle_t wl_handle;
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash2", test_par_lab, &mount_config, &wl_handle));

    esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash2", wl_handle);
}

TEST_CASE("(dyn_buffers) Unmount and then close the file", "[fatfs][dyn_buffers]")
{
    const char test_par_lab[] = "storage2";

    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
    };
    wl_handle_t wl_handle;
    TEST_ESP_OK(esp_vfs_fat_spiflash_mount_rw_wl("/spiflash2", test_par_lab, &mount_config, &wl_handle));
    BYTE pdrv = ff_diskio_get_pdrv_wl(wl_handle);
    char drv[3] = {(char)('0' + pdrv), ':', 0};
    const esp_vfs_fat_conf_t conf = {
        .base_path = "/spiflash2",
    };
    FATFS *fs = NULL;
    esp_vfs_fat_register(&conf, &fs);
    TEST_ASSERT_NOT_NULL(fs);

    int fd = open("/spiflash2/test.txt", O_RDWR|O_CREAT);
    TEST_ASSERT(fd >= 0);
    // Unmount fatfs only
    f_mount(0, drv, 0);
    close(fd);

    f_mount(fs, drv, 1);
    fd = open("/spiflash2/test.txt", O_RDWR|O_CREAT);
    TEST_ASSERT(fd >= 0);
    // Unmount fatfs and vfs
    esp_vfs_fat_spiflash_unmount_rw_wl("/spiflash2", wl_handle);
    close(fd);
}

void app_main(void)
{
    unity_run_menu();
}
