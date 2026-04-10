/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _wear_levelling_H_
#define _wear_levelling_H_

#include "esp_log.h"
#include "esp_partition.h"
#include "esp_blockdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief wear levelling handle
*/
typedef int32_t wl_handle_t;

#define WL_INVALID_HANDLE -1

/**
* @brief Mount WL for defined partition
*
* @param partition that will be used for access
* @param out_handle handle of the WL instance
*
* @return
*       - ESP_OK, if the WL allocation is successful;
*       - ESP_ERR_INVALID_ARG, if the arguments for WL configuration are not valid;
*       - ESP_ERR_NO_MEM, if the WL allocation fails because of insufficient memory;
*/
esp_err_t wl_mount(const esp_partition_t *partition, wl_handle_t *out_handle);

/**
* @brief Unmount WL for defined partition
*
* @param handle WL partition handle
*
* @return
*       - ESP_OK, if the operation is successful;
*       - or one of error codes from lower-level flash driver.
*/
esp_err_t wl_unmount(wl_handle_t handle);

/**
* @brief Erase part of the WL storage
*
* @param handle WL handle that are related to the partition
* @param start_addr Address from where erase operation should start. Must be aligned
*                   to the result of function wl_sector_size(...).
* @param size Size of the range which should be erased, in bytes.
*                   Must be divisible by the result of function wl_sector_size(...)..
*
* @return
*       - ESP_OK, if the given range was erased successfully;
*       - ESP_ERR_INVALID_ARG, if iterator or dst are NULL;
*       - ESP_ERR_INVALID_SIZE, if erase would go out of bounds of the partition;
*       - or one of error codes from lower-level flash driver.
*/
esp_err_t wl_erase_range(wl_handle_t handle, size_t start_addr, size_t size);

/**
* @brief Write data to the WL storage
*
* Before writing data to flash, corresponding region of flash needs to be erased.
* This can be done using wl_erase_range function.
*
* @param handle WL handle corresponding to the WL partition
* @param dest_addr Address where the data should be written, relative to the
*                  beginning of the partition.
* @param src Pointer to the source buffer. Pointer must be non-NULL and
*            buffer must be at least 'size' bytes long.
* @param size Size of data to be written, in bytes.
*
* @note Prior to writing to WL storage, make sure it has been erased with
*       wl_erase_range call.
*
* @return
*       - ESP_OK, if data was written successfully;
*       - ESP_ERR_INVALID_ARG, if dst_offset exceeds partition size;
*       - ESP_ERR_INVALID_SIZE, if write would go out of bounds of the partition;
*       - or one of error codes from lower-level flash driver.
*/
esp_err_t wl_write(wl_handle_t handle, size_t dest_addr, const void *src, size_t size);

/**
* @brief Read data from the WL storage
*
* @param handle WL module instance that was initialized before
* @param dest Pointer to the buffer where data should be stored.
*             The Pointer must be non-NULL and the buffer must be at least 'size' bytes long.
* @param src_addr Address of the data to be read, relative to the
*                 beginning of the partition.
* @param size Size of data to be read, in bytes.
*
* @return
*       - ESP_OK, if data was read successfully;
*       - ESP_ERR_INVALID_ARG, if src_offset exceeds partition size;
*       - ESP_ERR_INVALID_SIZE, if read would go out of bounds of the partition;
*       - or one of error codes from lower-level flash driver.
*/
esp_err_t wl_read(wl_handle_t handle, size_t src_addr, void *dest, size_t size);

/**
* @brief Get the actual flash size in use for the WL storage partition
*
* @param handle WL module handle that was initialized before
* @return usable size, in bytes
*/
size_t wl_size(wl_handle_t handle);

/**
* @brief Get sector size of the WL instance
*
* @param handle WL module handle that was initialized before
* @return sector size, in bytes
*/
size_t wl_sector_size(wl_handle_t handle);

/* -------------------------------------------------------------- */
/* BDL support                                                    */
/* -------------------------------------------------------------- */

 /**
  * @brief  Creates a wear-levelling BDL instance on top of the given bottom block device.
  *
  * All I/O is routed exclusively through @p bdl_bottom_device, keeping the BDL stack
  * fully independent of the legacy wl_handle_t driver.  The underlying device can be
  * an esp_partition BDL, a memory-backed disk, or any other block device implementation.
  *
  * Internally allocates a WL_Flash engine (deriving its configuration from the bottom
  * device geometry) and a BDL_Access adapter.  Releasing the returned BDL handle
  * flushes and destroys the wear-levelling instance.
  *
  * The output device inherits geometry constraints (read_size, write_size) and device
  * flags (read_only, encrypted, etc.) from @p bdl_bottom_device.  The disk_size and
  * erase_size are determined by the WL engine.
  *
  * @note The WL configuration stores partition size as uint32_t.  If the bottom device
  *       reports a disk_size exceeding UINT32_MAX the function fails immediately with
  *       ESP_ERR_INVALID_SIZE to prevent silent truncation (fail-fast).
  *
  * @param[in]  bdl_bottom_device Handle of the bottom block device layer instance (e.g. partition BDL)
  * @param[out] out_bdl_handle_ptr Handle of the created wear-levelling block device instance
  *
  * @return
  *      - ESP_OK on success
  *      - ESP_ERR_NO_MEM if memory allocation has failed
  *      - ESP_ERR_INVALID_ARG if any of the arguments are invalid
  *      - ESP_ERR_INVALID_SIZE if @p bdl_bottom_device disk_size exceeds UINT32_MAX
  */
  esp_err_t wl_get_blockdev(const esp_blockdev_handle_t bdl_bottom_device, esp_blockdev_handle_t *out_bdl_handle_ptr);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _wear_levelling_H_
