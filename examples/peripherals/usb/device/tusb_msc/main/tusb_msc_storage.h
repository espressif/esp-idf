/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "esp_err.h"

/* Public functions
   ********************************************************************* */

/**
 * @brief Initialize the Storage.
 *
 * First find 'first partition' based on one or more parameters (ie DATA, FAT).
 * Mount WL for defined partition.
 * Initialize the instance of WL instance.
 * Once the storage is initialized, other storage functions can be used.
 *
 * @return esp_err_t
 *       - ESP_OK, if success;
 *       - ESP_ERR_NOT_FOUND, if Failed to find FATFS partition;
 *       - ESP_ERR_INVALID_ARG, if WL allocation was unsuccessful;
 *       - ESP_ERR_NO_MEM, if there was no memory to allocate WL components;
 */
esp_err_t storage_init(void);

/**
 * @brief Mount the storage partition locally on the firmware application.
 *
 * Get the available drive number. Register spi flash partition.
 * Connect POSIX and C standard library IO function with FATFS.
 * Mounts the partition.
 * This API is used by the firmware application. If the storage partition is
 * mounted by this API, host (PC) can't access the storage via MSC.
 *
 * @param base_path  path prefix where FATFS should be registered
 * @return esp_err_t
 *       - ESP_OK, if success;
 *       - ESP_ERR_NO_MEM if not enough memory or too many VFSes already registered;
 */
esp_err_t storage_mount(const char *base_path);

/**
 * @brief Unmount the storage partition from the firmware application.
 *
 * Unmount the partition. Unregister diskio driver.
 * Unregister the SPI flash partition.
 * Finally, Un-register FATFS from VFS.
 * After this function is called, storage device can be seen (recongnized) by host (PC).
 *
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if FATFS is not registered in VFS
 */
esp_err_t storage_unmount(void);

/**
 * @brief Get size of the WL storage
 *
 * @return usable size, in bytes
 */
size_t storage_get_size(void);

/**
 * @brief Get sector size of the WL instance
 *
 * @return sector size, in bytes
 */
size_t storage_get_sector_size(void);

/**
 * @brief Read data from the WL storage
 *
 * @param addr Address of the data to be read, relative to the
 *            beginning of the partition.
 * @param size Size of data to be read, in bytes.
 * @param dest Pointer to the buffer where data should be stored.
 *            Pointer must be non-NULL and buffer must be at least 'size' bytes long.
 * @return esp_err_t
 *       - ESP_OK, if data was read successfully;
 *       - ESP_ERR_INVALID_ARG, if src_offset exceeds partition size;
 *       - ESP_ERR_INVALID_SIZE, if read would go out of bounds of the partition;
 *       - or one of error codes from lower-level flash driver.
 */
esp_err_t storage_read_sector(size_t addr, size_t size, void *dest);

/**
 * @brief Write data to the WL storage
 *
 * Before writing data to flash, corresponding region of flash needs to be erased.
 * This is done internally using wl_erase_range function.
 *
 * @param addr Address where the data should be written, relative to the
 *            beginning of the partition.
 * @param size Size of data to be written, in bytes.
 * @param src Pointer to the source buffer.  Pointer must be non-NULL and
 *            buffer must be at least 'size' bytes long.
 * @return esp_err_t
 *       - ESP_OK, if data was written successfully;
 *       - ESP_ERR_INVALID_ARG, if dst_offset exceeds partition size;
 *       - ESP_ERR_INVALID_SIZE, if write would go out of bounds of the partition;
 *       - or one of error codes from lower-level flash driver.
 */
esp_err_t storage_write_sector(size_t addr, size_t size, const void *src);

/*********************************************************************** Public functions*/

#ifdef __cplusplus
}
#endif
