/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * FatFS over BDL (Block Device Layer) - Wear-Levelling stack example
 *
 * Demonstrates building a BDL stack and mounting FatFS on top of it:
 *
 *    +-----------+
 *    |  FatFS    |    <- file system (VFS + FatFS)
 *    +-----------+
 *    | diskio_bdl|    <- FatFS diskio driver for BDL devices
 *    +-----------+
 *    |  WL BDL   |    <- wear-levelling BDL layer (wl_get_blockdev)
 *    +-----------+
 *    |  Part BDL |    <- partition BDL layer (esp_partition_get_blockdev)
 *    +-----------+
 *    | SPI Flash |    <- physical storage
 *    +-----------+
 *
 * The BDL approach decouples FatFS from any specific storage driver.
 * The same diskio_bdl adapter works with any BDL-compatible bottom device:
 *   - partition BDL  (flash partition)
 *   - sdmmc BDL      (SD/eMMC card)
 *   - memory BDL     (RAM disk for testing)
 *   - or any custom BDL implementation
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_partition.h"
#include "esp_blockdev.h"
#include "wear_levelling.h"

static const char *TAG = "example";

const char *base_path = "/spiflash";

void app_main(void)
{
    /* ------------------------------------------------------------------ */
    /* Step 1: Build the BDL stack                                        */
    /* ------------------------------------------------------------------ */

    ESP_LOGI(TAG, "Creating partition BDL for 'storage' partition");

    esp_blockdev_handle_t part_bdl = NULL;
    ESP_ERROR_CHECK(esp_partition_get_blockdev(
            ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT,
            "storage", &part_bdl));

    ESP_LOGI(TAG, "  Partition BDL: disk_size=%llu, erase_size=%u",
             (unsigned long long)part_bdl->geometry.disk_size,
             (unsigned)part_bdl->geometry.erase_size);

    ESP_LOGI(TAG, "Creating WL BDL on top of partition BDL");

    esp_blockdev_handle_t wl_bdl = NULL;
    ESP_ERROR_CHECK(wl_get_blockdev(part_bdl, &wl_bdl));

    ESP_LOGI(TAG, "  WL BDL: disk_size=%llu, erase_size=%u",
             (unsigned long long)wl_bdl->geometry.disk_size,
             (unsigned)wl_bdl->geometry.erase_size);

    /* ------------------------------------------------------------------ */
    /* Step 2: Mount FatFS on the BDL device                              */
    /* ------------------------------------------------------------------ */

    ESP_LOGI(TAG, "Mounting FAT filesystem via BDL");

    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .use_one_fat = false,
    };

    ESP_ERROR_CHECK(esp_vfs_fat_bdl_mount(base_path, wl_bdl, &mount_config));

    ESP_LOGI(TAG, "Filesystem mounted");

    /* ------------------------------------------------------------------ */
    /* Step 3: Use POSIX file operations                                  */
    /* ------------------------------------------------------------------ */

    const char *filename = "/spiflash/example.txt";

    ESP_LOGI(TAG, "Opening file");
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello from FatFS over BDL!\n");
    fclose(f);
    ESP_LOGI(TAG, "File written");

    ESP_LOGI(TAG, "Reading file");
    f = fopen(filename, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[128];
    fgets(line, sizeof(line), f);
    fclose(f);

    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    /* ------------------------------------------------------------------ */
    /* Step 4: Unmount and tear down the BDL stack                        */
    /* ------------------------------------------------------------------ */

    ESP_LOGI(TAG, "Unmounting FAT filesystem");
    ESP_ERROR_CHECK(esp_vfs_fat_bdl_unmount(base_path, wl_bdl));

    ESP_LOGI(TAG, "Releasing BDL devices");
    wl_bdl->ops->release(wl_bdl);
    part_bdl->ops->release(part_bdl);

    ESP_LOGI(TAG, "Done");
}
