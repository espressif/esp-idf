// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include <stddef.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/sdmmc_types.h"
#include "driver/sdmmc_host.h"
#include "ff.h"

/**
 * @brief Register FATFS with VFS component
 *
 * This function registers given FAT drive in VFS, at the specified base path.
 * If only one drive is used, fat_drive argument can be an empty string.
 * Refer to FATFS library documentation on how to specify FAT drive.
 * This function also allocates FATFS structure which should be used for f_mount
 * call.
 *
 * @note This function doesn't mount the drive into FATFS, it just connects
 *       POSIX and C standard library IO function with FATFS. You need to mount
 *       desired drive into FATFS separately.
 *
 * @param base_path  path prefix where FATFS should be registered
 * @param fat_drive  FATFS drive specification; if only one drive is used, can be an empty string
 * @param max_files  maximum number of files which can be open at the same time
 * @param[out] out_fs  pointer to FATFS structure which can be used for FATFS f_mount call is returned via this argument.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_register was already called
 *      - ESP_ERR_NO_MEM if not enough memory or too many VFSes already registered
 */
esp_err_t esp_vfs_fat_register(const char* base_path, const char* fat_drive,
        size_t max_files, FATFS** out_fs);

/**
 * @brief Un-register FATFS from VFS
 *
 * @note FATFS structure returned by esp_vfs_fat_register is destroyed after
 *       this call. Make sure to call f_mount function to unmount it before
 *       calling esp_vfs_fat_unregister.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if FATFS is not registered in VFS
 */
esp_err_t esp_vfs_fat_unregister();

/**
 * @brief Configuration arguments for esp_vfs_fat_sdmmc_mount function
 */
typedef struct {
	bool format_if_mount_failed;    ///< If FAT partition can not be mounted, and this parameter is true, create partition table and format the filesystem
	int max_files;                  ///< Max number of open files
} esp_vfs_fat_sdmmc_mount_config_t;

/**
 * @brief Convenience function to get FAT filesystem on SD card registered in VFS
 *
 * This is an all-in-one function which does the following:
 * - initializes SD/MMC peripheral with configuration in host_config
 * - initializes SD/MMC card with configuration in slot_config
 * - mounts FAT partition on SD/MMC card using FATFS library, with configuration in mount_config
 * - registers FATFS library with VFS, with prefix given by base_prefix variable
 *
 * This function is intended to make example code more compact.
 * For real world applications, developers should implement the logic of
 * probing SD card, locating and mounting partition, and registering FATFS in VFS,
 * with proper error checking and handling of exceptional conditions.
 *
 * @param base_path     path where partition should be registered (e.g. "/sdcard")
 * @param host_config   pointer to structure describing SDMMC host
 * @param slot_config   pointer to structure with extra SDMMC slot configuration
 * @param mount_config  pointer to structure with extra parameters for mounting FATFS
 * @param[out] out_card  if not NULL, pointer to the card information structure will be returned via this argument
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from SDMMC host, SDMMC protocol, or FATFS drivers
 */
esp_err_t esp_vfs_fat_sdmmc_mount(const char* base_path,
    const sdmmc_host_t* host_config,
    const sdmmc_slot_config_t* slot_config,
    const esp_vfs_fat_sdmmc_mount_config_t* mount_config,
    sdmmc_card_t** out_card);

/**
 * @brief Unmount FAT filesystem and release resources acquired using esp_vfs_fat_sdmmc_mount
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount hasn't been called
 */
esp_err_t esp_vfs_fat_sdmmc_unmount();
