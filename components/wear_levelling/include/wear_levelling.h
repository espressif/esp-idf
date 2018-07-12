// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _wear_levelling_H_
#define _wear_levelling_H_

#include "esp_log.h"
#include "esp_partition.h"

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
*       - ESP_OK, if the allocation was successfully;
*       - ESP_ERR_INVALID_ARG, if WL allocation was unsuccessful;
*       - ESP_ERR_NO_MEM, if there was no memory to allocate WL components;
*/
esp_err_t wl_mount(const esp_partition_t *partition, wl_handle_t *out_handle);

/**
* @brief Unmount WL for defined partition
*
* @param handle WL partition handle
*
* @return
*       - ESP_OK, if the operation completed successfully;
*       - or one of error codes from lower-level flash driver.
*/
esp_err_t wl_unmount(wl_handle_t handle);

/**
* @brief Erase part of the WL storage
*
* @param handle WL handle that are related to the partition
* @param start_addr Address where erase operation should start. Must be aligned
*                   to the result of function wl_sector_size(...).
* @param size Size of the range which should be erased, in bytes.
*                   Must be divisible by result of function wl_sector_size(...)..
*
* @return
*       - ESP_OK, if the range was erased successfully;
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
* @param handle WL handle that are related to the partition
* @param dest_addr Address where the data should be written, relative to the
*                   beginning of the partition.
* @param src Pointer to the source buffer.  Pointer must be non-NULL and
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
*            Pointer must be non-NULL and buffer must be at least 'size' bytes long.
* @param src_addr Address of the data to be read, relative to the
*                   beginning of the partition.
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
* @brief Get size of the WL storage
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


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _wear_levelling_H_
