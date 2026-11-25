/*
 * SPDX-FileCopyrightText: 2018-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_vfs_fat.h"
#include "diskio_impl.h"
#include "esp_partition.h"
#include "esp_blockdev.h"
#ifndef CONFIG_IDF_TARGET_LINUX
#include "sdmmc_cmd.h"
#endif
#include <sys/param.h>
#include <stddef.h>

/* Internal view of a FATFS instance; this is what VFS operations receive as their context */
typedef struct esp_vfs_fat_ctx_priv vfs_fat_ctx_t;

/* The internal view is the first member of the public context */
static inline esp_vfs_fat_ctx_t *vfs_fat_outer(vfs_fat_ctx_t *fat_ctx)
{
    return (esp_vfs_fat_ctx_t *)fat_ctx;
}

/**
 * @brief Allocate a context together with max_files file slots in a single block
 *
 * @return context to be released with vfs_fat_free(), or NULL if out of memory
 */
esp_vfs_fat_ctx_t *vfs_fat_alloc(size_t max_files, esp_vfs_fat_file_t **out_files);

/**
 * @brief Free a context allocated by vfs_fat_alloc()
 */
void vfs_fat_free(esp_vfs_fat_ctx_t *ctx);

/**
 * @brief Prepare the VFS-facing part of an instance (lock, file slots, drive prefix)
 *
 * Zeroes `fat_ctx` and `files`; does not touch FATFS drives or VFS registration.
 */
void vfs_fat_ctx_setup(vfs_fat_ctx_t *fat_ctx, esp_vfs_fat_file_t *files, size_t max_files, const char *fat_drive);

/**
 * @brief Release resources acquired by vfs_fat_ctx_setup() and by open files
 */
void vfs_fat_ctx_teardown(vfs_fat_ctx_t *fat_ctx);

/**
 * @brief Use caller-provided sector buffers for the volume window and the file slots
 *
 * Call after vfs_fat_ctx_setup() and before mounting. `sector_buffers` holds
 * ESP_VFS_FAT_SECTOR_BUFFERS_SIZE(max_files, sector_size) bytes.
 */
void vfs_fat_ctx_attach_buffers(vfs_fat_ctx_t *fat_ctx, uint8_t *sector_buffers, size_t sector_size);

/**
 * @brief Allocate the volume window and switch the instance to wrapper-allocated sector buffers
 *
 * Used when the caller did not supply sector_buffers. File windows of `sector_size` bytes are
 * then allocated on open and freed on close. Everything is freed by vfs_fat_ctx_teardown().
 * No effect unless CONFIG_FATFS_USE_DYN_BUFFERS is enabled.
 */
esp_err_t vfs_fat_alloc_sector_buffers(vfs_fat_ctx_t *fat_ctx, size_t sector_size);

/**
 * @brief Give the instance's FATFS its LFN working buffer
 *
 * Call after vfs_fat_ctx_setup() and before mounting. No effect unless CONFIG_FATFS_LFN_HEAP is
 * selected. A NULL `lfn_buffer` makes the wrapper allocate ESP_VFS_FAT_LFN_BUFFER_SIZE bytes with
 * the capabilities selected by CONFIG_FATFS_HEAP_CAPS, freed by vfs_fat_ctx_teardown(). A caller
 * buffer is left to the caller.
 */
esp_err_t vfs_fat_attach_lfn_buffer(vfs_fat_ctx_t *fat_ctx, void *lfn_buffer);

/**
 * @brief Register an instance with VFS at base_path and make it findable by path
 *
 * Path-based functions such as esp_vfs_fat_info() only work for instances registered this way.
 */
esp_err_t vfs_fat_register_path(vfs_fat_ctx_t *fat_ctx, const char *base_path);

/**
 * @brief Undo vfs_fat_register_path()
 */
esp_err_t vfs_fat_unregister_path(vfs_fat_ctx_t *fat_ctx);

/**
 * @brief Find an instance registered with vfs_fat_register_path()
 *
 * @return instance, or NULL if nothing is registered at base_path
 */
vfs_fat_ctx_t *vfs_fat_find_by_path(const char *base_path);

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
