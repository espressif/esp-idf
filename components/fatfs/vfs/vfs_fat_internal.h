/*
 * SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs_fat.h"
#include "diskio_impl.h"
#include "esp_partition.h"
#ifndef CONFIG_IDF_TARGET_LINUX
#include "sdmmc_cmd.h"
#endif
#include <sys/param.h>
#include <stddef.h>

typedef struct vfs_fat_spiflash_ctx_t {
    const esp_partition_t *partition;           //The partition where the FAT is located
    bool by_label;                              //If the partition is mounted by label or not
    BYTE pdrv;                                  //Drive number that is mounted
    FATFS *fs;                                  //FAT structure pointer that is registered
    wl_handle_t wlhandle;                       //WL handle
    esp_vfs_fat_mount_config_t mount_config;    //Mount configuration
    vfs_fat_x_ctx_flags_t flags;                //Flags
} vfs_fat_spiflash_ctx_t;

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

#ifndef CONFIG_IDF_TARGET_LINUX
typedef struct vfs_fat_sd_ctx_t {
    BYTE pdrv;                                  //Drive number that is mounted
    esp_vfs_fat_mount_config_t mount_config;    //Mount configuration
    FATFS *fs;                                  //FAT structure pointer that is registered
    sdmmc_card_t *card;                         //Card info
    char *base_path;                            //Path where partition is registered
    vfs_fat_x_ctx_flags_t flags;                //Flags
} vfs_fat_sd_ctx_t;

vfs_fat_spiflash_ctx_t* get_vfs_fat_spiflash_ctx(wl_handle_t wlhandle);
vfs_fat_sd_ctx_t* get_vfs_fat_get_sd_ctx(const sdmmc_card_t *card);

/**
 * @brief Helper function to initialize SD card using SDMMC peripheral
 *
 * @param host_config  pointer to structure describing SDMMC host
 * @param slot_config  pointer to structure with slot configuration
 * @param card         pointer to structure with card information
 * @param[out] out_host_inited  if not NULL, pointer to boolean variable will be set to true if host was initialized
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the arguments other than `out_host_inited` is NULL
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t esp_vfs_fat_sdmmc_sdcard_init(const sdmmc_host_t* host_config, const void* slot_config, sdmmc_card_t* card, bool* out_host_inited);

/**
 * @brief Helper function to initialize SD card using SDSPI peripheral
 *
 * @param host_config  pointer to structure describing SDSPI host
 * @param slot_config  pointer to structure with slot configuration
 * @param card         pointer to structure with card information
 * @param[out] out_host_inited  if not NULL, pointer to boolean variable will be set to true if host was initialized
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the arguments other than `out_host_inited` is NULL
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t esp_vfs_fat_sdspi_sdcard_init(const sdmmc_host_t* host_config, const void* slot_config, sdmmc_card_t* card, bool* out_host_inited);

/**
 * @brief Convenience function to get FAT filesystem registered and mounted in VFS with an initialized card handle
 *
 * @note This function is intended to be used when the card is already initialized and the user wants to mount the FAT filesystem on it.
 *       Can be used to mount multiple partitions on a single card.
 *
 * @param card  pointer to the card handle, which should be initialised by calling `esp_vfs_fat_sdmmc_sdcard_init` or `esp_vfs_fat_sdspi_sdcard_init` first
 * @param base_path  path where partition should be registered (e.g. "/sdcard" or "/part1", etc.)
 * @param mount_config  pointer to structure with extra parameters for mounting FATFS
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any of the arguments is NULL
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t esp_vfs_fat_mount_initialized(sdmmc_card_t* card,
    const char* base_path,
    const esp_vfs_fat_mount_config_t* mount_config);

/**
 * @brief Format FATFS partition on a drive to FAT filesystem
 *
 * @param ldrv  logical or physical drive number
 * @param mount_config  pointer to structure with extra parameters for mounting FATFS
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if mount_config is NULL or ldrv is not valid
 *      - ESP_ERR_NO_MEM if memory allocation failed
 *      - ESP_FAIL if f_mkfs failed
 */
esp_err_t esp_vfs_fat_format_drive(uint8_t ldrv, const esp_vfs_fat_mount_config_t* mount_config);

/**
 * @brief Partition the drive to multiple FATFS partitions
 *
 * @param pdrv  physical drive number
 * @param drive_divide  array of 4 LBA_t values, specifying the size of each partition
 *
 * @note `esp_vfs_fat_drive_divide_arr_t drive_divide`
 *       - When the value of item is less than or equal to 100, it specifies the partition size in percentage of the entire drive space.
 *       When it is larger than 100, it specifies number of sectors. The partition map table is terminated by a zero,
 *       4th partition in MBR format or no remaining space for next allocation. If the specified size is larger than remaining space on the drive,
 *       the partition is truncated at end of the drive.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if pdrv is not valid
 *      - ESP_ERR_NO_MEM if memory allocation failed
 *      - ESP_FAIL if f_fdisk failed
 */
esp_err_t esp_vfs_fat_partition_drive(uint8_t pdrv, const esp_vfs_fat_drive_divide_arr_t drive_divide);
#endif
