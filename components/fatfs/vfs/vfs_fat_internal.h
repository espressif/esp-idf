/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs_fat.h"
#include "diskio_impl.h"
#include "esp_partition.h"
#include "sdmmc_cmd.h"
#include <sys/param.h>
#include <stddef.h>

typedef enum {
    FORMATTED_DURING_LAST_MOUNT = 1 << 0, // The FATFS partition was formatted during the last mount
} vfs_fat_x_ctx_flags_t;

typedef struct vfs_fat_spiflash_ctx_t {
    const esp_partition_t *partition;           //The partition where the FAT is located
    bool by_label;                              //If the partition is mounted by label or not
    BYTE pdrv;                                  //Drive number that is mounted
    FATFS *fs;                                  //FAT structure pointer that is registered
    wl_handle_t wlhandle;                       //WL handle
    esp_vfs_fat_mount_config_t mount_config;    //Mount configuration
    vfs_fat_x_ctx_flags_t flags;                //Flags
} vfs_fat_spiflash_ctx_t;

typedef struct vfs_fat_sd_ctx_t {
    BYTE pdrv;                                  //Drive number that is mounted
    esp_vfs_fat_mount_config_t mount_config;    //Mount configuration
    FATFS *fs;                                  //FAT structure pointer that is registered
    sdmmc_card_t *card;                         //Card info
    char *base_path;                            //Path where partition is registered
    vfs_fat_x_ctx_flags_t flags;                //Flags
} vfs_fat_sd_ctx_t;

static inline size_t esp_vfs_fat_get_allocation_unit_size(
        size_t sector_size, size_t requested_size)
{
    size_t alloc_unit_size = requested_size;
    const size_t max_sectors_per_cylinder = 128;
    const size_t max_size = sector_size * max_sectors_per_cylinder;
    alloc_unit_size = MAX(alloc_unit_size, sector_size);
    alloc_unit_size = MIN(alloc_unit_size, max_size);
    return alloc_unit_size;
}

vfs_fat_spiflash_ctx_t* get_vfs_fat_spiflash_ctx(wl_handle_t wlhandle);
vfs_fat_sd_ctx_t* get_vfs_fat_get_sd_ctx(const sdmmc_card_t *card);
