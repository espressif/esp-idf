/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "esp_err.h"
#include "driver/sdmmc_host.h"

/* structs, unions
   ********************************************************************* */

/**
 * @brief enum for storage media that are supported
 */
typedef enum {
    TINYUSB_STORAGE_INVALID,
    TINYUSB_STORAGE_SPI,
    TINYUSB_STORAGE_SDMMC,
    TINYUSB_STORAGE_MAX,
} tinyusb_storage_type_t;

#if SOC_SDMMC_HOST_SUPPORTED
/**
 * @brief Configuration structure for sdmmc initialization
 *
 * User configurable parameters that are used while
 * initializing the sdmmc media.
 */
typedef struct {
    sdmmc_host_t host;
    sdmmc_slot_config_t slot_config;
} tinyusb_config_sdmmc_t;
#endif

/**
 * @brief Configuration structure for spiffs initialization
 *
 * User configurable parameters that are used while
 * initializing the SPI Flash media.
 */
typedef struct {
    // Place holder. Currently, nothing to configure
} tinyusb_config_spiffs_t;

/**
 * @brief Configuration structure for storage initialization
 */
typedef struct {
    tinyusb_storage_type_t storage_type;
    union {
        tinyusb_config_spiffs_t *spiffs_config;
#if SOC_SDMMC_HOST_SUPPORTED
        tinyusb_config_sdmmc_t *sdmmc_config;
#endif
    };
} tinyusb_config_storage_t;
/*********************************************************************** structs, unions */
/* Public functions
   ********************************************************************* */

/**
 * @brief Initialize the Storage.
 *
 * Initialize the instance of storage media.
 * Once the storage is initialized, other storage functions can be used.
 *
 * @param config  pointer to tinyusb_config_storage_t to pass
 * user defined configuration during initialization.
 * @return esp_err_t
 *       - ESP_OK, if success;
 *       - ESP_ERR_NO_MEM, if there was no memory to allocate storage components;
 */
esp_err_t storage_init(const tinyusb_config_storage_t *config);

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
 *       - ESP_ERR_NOT_FOUND if the maximum count of volumes is already mounted
 *       - ESP_ERR_NO_MEM if not enough memory or too many VFSes already registered;
 */
esp_err_t storage_mount(const char *base_path);

/**
 * @brief Unmount the storage partition from the firmware application.
 *
 * Unmount the partition. Unregister diskio driver.
 * Unregister the SPI flash partition.
 * Finally, Un-register FATFS from VFS.
 * After this function is called, storage device can be seen (recognized) by host (PC).
 *
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if FATFS is not registered in VFS
 */
esp_err_t storage_unmount(void);

/**
 * @brief Get number of sectors in storage media
 *
 * @return usable size, in bytes
 */
uint32_t storage_get_sector_count(void);

/**
 * @brief Get sector size of storage media
 *
 * @return sector count
 */
uint32_t storage_get_sector_size(void);

/**
 * @brief Read data from the storage
 *
 * @param lba Logical block address of the location of block
 * @param offset offset within the lba
 * @param size Size of data to be read, in bytes.
 * @param dest Pointer to the buffer where data should be stored.
 *            Pointer must be non-NULL and buffer must be at least 'size' bytes long.
 * @return esp_err_t
 *       - ESP_OK, if data was read successfully;
 *       - ESP_ERR_INVALID_ARG, if src_offset exceeds partition size;
 *       - ESP_ERR_INVALID_SIZE, if read would go out of bounds of the partition;
 *       - or one of error codes from lower-level flash driver.
 */
esp_err_t storage_read_sector(uint32_t lba, uint32_t offset, size_t size, void *dest);

/**
 * @brief Write data to the storage
 *
 * Before writing data to flash, corresponding region of flash needs to be erased.
 *
 * @param lba Logical block address of the location of block
 * @param offset offset within the lba
 * @param size Size of data to be written, in bytes.
 * @param src Pointer to the source buffer.  Pointer must be non-NULL and
 *            buffer must be at least 'size' bytes long.
 * @return esp_err_t
 *       - ESP_OK, if data was written successfully;
 *       - ESP_ERR_INVALID_ARG, if dst_offset exceeds partition size;
 *       - ESP_ERR_INVALID_SIZE, if write would go out of bounds of the partition;
 *       - or one of error codes from lower-level flash driver.
 */
esp_err_t storage_write_sector(uint32_t lba, uint32_t offset, size_t size, const void *src);

/*********************************************************************** Public functions*/

#ifdef __cplusplus
}
#endif
