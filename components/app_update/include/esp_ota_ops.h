// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _OTA_OPS_H
#define _OTA_OPS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_spi_flash.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define OTA_SIZE_UNKNOWN 0xffffffff /*!< Used for esp_ota_begin() if new image size is unknown */

#define ESP_ERR_OTA_BASE                         0x1500                     /*!< Base error code for ota_ops api */
#define ESP_ERR_OTA_PARTITION_CONFLICT           (ESP_ERR_OTA_BASE + 0x01)  /*!< Error if request was to write or erase the current running partition */
#define ESP_ERR_OTA_SELECT_INFO_INVALID          (ESP_ERR_OTA_BASE + 0x02)  /*!< Error if OTA data partition contains invalid content */
#define ESP_ERR_OTA_VALIDATE_FAILED              (ESP_ERR_OTA_BASE + 0x03)  /*!< Error if OTA app image is invalid */

/**
 * @brief Opaque handle for an application OTA update
 *
 * esp_ota_begin() returns a handle which is then used for subsequent
 * calls to esp_ota_write() and esp_ota_end().
 */
typedef uint32_t esp_ota_handle_t;

/**
 * @brief   Commence an OTA update writing to the specified partition.

 * The specified partition is erased to the specified image size.
 *
 * If image size is not yet known, pass OTA_SIZE_UNKNOWN which will
 * cause the entire partition to be erased.
 *
 * On success, this function allocates memory that remains in use
 * until esp_ota_end() is called with the returned handle.
 *
 * @param partition Pointer to info for partition which will receive the OTA update. Required.
 * @param image_size Size of new OTA app image. Partition will be erased in order to receive this size of image. If 0 or OTA_SIZE_UNKNOWN, the entire partition is erased.
 * @param out_handle On success, returns a handle which should be used for subsequent esp_ota_write() and esp_ota_end() calls.

 * @return
 *    - ESP_OK: OTA operation commenced successfully.
 *    - ESP_ERR_INVALID_ARG: partition or out_handle arguments were NULL, or partition doesn't point to an OTA app partition.
 *    - ESP_ERR_NO_MEM: Cannot allocate memory for OTA operation.
 *    - ESP_ERR_OTA_PARTITION_CONFLICT: Partition holds the currently running firmware, cannot update in place.
 *    - ESP_ERR_NOT_FOUND: Partition argument not found in partition table.
 *    - ESP_ERR_OTA_SELECT_INFO_INVALID: The OTA data partition contains invalid data.
 *    - ESP_ERR_INVALID_SIZE: Partition doesn't fit in configured flash size.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 */
esp_err_t esp_ota_begin(const esp_partition_t* partition, size_t image_size, esp_ota_handle_t* out_handle);

/**
 * @brief   Write OTA update data to partition
 *
 * This function can be called multiple times as
 * data is received during the OTA operation. Data is written
 * sequentially to the partition.
 *
 * @param handle  Handle obtained from esp_ota_begin
 * @param data    Data buffer to write
 * @param size    Size of data buffer in bytes.
 *
 * @return
 *    - ESP_OK: Data was written to flash successfully.
 *    - ESP_ERR_INVALID_ARG: handle is invalid.
 *    - ESP_ERR_OTA_VALIDATE_FAILED: First byte of image contains invalid app image magic byte.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 *    - ESP_ERR_OTA_SELECT_INFO_INVALID: OTA data partition has invalid contents
 */
esp_err_t esp_ota_write(esp_ota_handle_t handle, const void* data, size_t size);

/**
 * @brief Finish OTA update and validate newly written app image.
 *
 * @param handle  Handle obtained from esp_ota_begin().
 *
 * @note After calling esp_ota_end(), the handle is no longer valid and any memory associated with it is freed (regardless of result).
 *
 * @return
 *    - ESP_OK: Newly written OTA app image is valid.
 *    - ESP_ERR_NOT_FOUND: OTA handle was not found.
 *    - ESP_ERR_INVALID_ARG: Handle was never written to.
 *    - ESP_ERR_OTA_VALIDATE_FAILED: OTA image is invalid (either not a valid app image, or - if secure boot is enabled - signature failed to verify.)
 *    - ESP_ERR_INVALID_STATE: If flash encryption is enabled, this result indicates an internal error writing the final encrypted bytes to flash.
 */
esp_err_t esp_ota_end(esp_ota_handle_t handle);

/**
 * @brief Configure OTA data for a new boot partition
 *
 * @note If this function returns ESP_OK, calling esp_restart() will boot the newly configured app partition.
 *
 * @param partition Pointer to info for partition containing app image to boot.
 *
 * @return
 *    - ESP_OK: OTA data updated, next reboot will use specified partition.
 *    - ESP_ERR_INVALID_ARG: partition argument was NULL or didn't point to a valid OTA partition of type "app".
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Partition contained invalid app image. Also returned if secure boot is enabled and signature validation failed.
 *    - ESP_ERR_NOT_FOUND: OTA data partition not found.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash erase or write failed.
 */
esp_err_t esp_ota_set_boot_partition(const esp_partition_t* partition);

/**
 * @brief Get partition info of currently configured boot app
 *
 * If esp_ota_set_boot_partition() has been called, the partition which was set by that function will be returned.
 *
 * If esp_ota_set_boot_partition() has not been called, the result is usually the same as esp_ota_get_running_partition().
 * The two results are not equal if the configured boot partition does not contain a valid app (meaning that the running partition
 * will be an app that the bootloader chose via fallback).
 *
 * If the OTA data partition is not present or not valid then the result is the first app partition found in the
 * partition table. In priority order, this means: the factory app, the first OTA app slot, or the test app partition.
 *
 * Note that there is no guarantee the returned partition is a valid app. Use esp_image_load(ESP_IMAGE_VERIFY, ...) to verify if the
 * returned partition contains a bootable image.
 *
 * @return Pointer to info for partition structure, or NULL if partition table is invalid or a flash read operation failed. Any returned pointer is valid for the lifetime of the application.
 */
const esp_partition_t* esp_ota_get_boot_partition(void);


/**
 * @brief Get partition info of currently running app
 *
 * This function is different to esp_ota_get_boot_partition() in that
 * it ignores any change of selected boot partition caused by
 * esp_ota_set_boot_partition(). Only the app whose code is currently
 * running will have its partition information returned.
 *
 * The partition returned by this function may also differ from esp_ota_get_boot_partition() if the configured boot
 * partition is somehow invalid, and the bootloader fell back to a different app partition at boot.
 *
 * @return Pointer to info for partition structure, or NULL if no partition is found or flash read operation failed. Returned pointer is valid for the lifetime of the application.
 */
const esp_partition_t* esp_ota_get_running_partition(void);


/**
 * @brief Return the next OTA app partition which should be written with a new firmware.
 *
 * Call this function to find an OTA app partition which can be passed to esp_ota_begin().
 *
 * Finds next partition round-robin, starting from the current running partition.
 *
 * @param start_from If set, treat this partition info as describing the current running partition. Can be NULL, in which case esp_ota_get_running_partition() is used to find the currently running partition. The result of this function is never the same as this argument.
 *
 * @return Pointer to info for partition which should be updated next. NULL result indicates invalid OTA data partition, or that no eligible OTA app slot partition was found.
 *
 */
const esp_partition_t* esp_ota_get_next_update_partition(const esp_partition_t *start_from);

#ifdef __cplusplus
}
#endif

#endif /* OTA_OPS_H */
