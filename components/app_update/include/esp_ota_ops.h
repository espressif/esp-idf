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

#define OTA_SIZE_UNKNOWN 0xffffffff

#define ESP_ERR_OTA_BASE                         0x1500                     /*!< base error code for ota_ops api */
#define ESP_ERR_OTA_PARTITION_CONFLICT           (ESP_ERR_OTA_BASE + 0x01)  /*!< want to write or erase current running partition */
#define ESP_ERR_OTA_SELECT_INFO_INVALID          (ESP_ERR_OTA_BASE + 0x02)  /*!< ota data partition info is error */
#define ESP_ERR_OTA_VALIDATE_FAILED              (ESP_ERR_OTA_BASE + 0x03)  /*!< validate ota image failed */

/**
 * @brief Opaque handle for application update obtained from app_ops.
 */
typedef uint32_t esp_ota_handle_t;

/**
 * @brief   format input partition in flash to 0xFF as input image size,
 *          if unkown image size ,pass 0x0 or 0xFFFFFFFF, it will erase all the
 *          partition ,Otherwise, erase the required range
 *
 * @param   partition Pointer to partition structure which need to be updated
 *            Must be non-NULL.
 * @param   image_size size of image need to be updated
 * @param   out_handle handle which should be used for esp_ota_write or esp_ota_end call

 * @return: 
 *    - ESP_OK: if format ota image OK  
 *    - ESP_ERR_OTA_PARTITION_CONFLICT: operate current running bin  
 *    - ESP_ERR_OTA_SELECT_INFO_INVALID: ota bin select info invalid
 */
esp_err_t esp_ota_begin(const esp_partition_t* partition, size_t image_size, esp_ota_handle_t* out_handle);

/**
 * @brief   Write data to input input partition
 *
 * @param   handle  Handle obtained from esp_ota_begin
 * @param   data  Pointer to data write to flash
 * @param   size  data size of recieved data
 *
 * @return: 
 *    - ESP_OK: if write flash data OK 
 *    - ESP_ERR_OTA_PARTITION_CONFLICT: operate current running bin  
 *    - ESP_ERR_OTA_SELECT_INFO_INVALID: ota bin select info invalid
 */
esp_err_t esp_ota_write(esp_ota_handle_t handle, const void* data, size_t size);
 
/**
 * @brief   Finish the update and validate written data
 *
 * @param   handle  Handle obtained from esp_ota_begin.
 *
 * @note After calling esp_ota_end(), the handle is no longer valid and any memory associated with it is freed (regardless of result).
 *
 * @return:
 *    - ESP_OK: Newly written OTA app image is valid.
 *    - ESP_ERR_NOT_FOUND: OTA handle was not found.
 *    - ESP_ERR_INVALID_ARG: Handle was never written to.
 *    - ESP_ERR_OTA_VALIDATE_FAILED: OTA image is invalid (either not a valid app image, or - if secure boot is enabled - signature failed to verify.)
 *    - ESP_ERR_INVALID_STATE: If flash encryption is enabled, this result indicates an internal error writing the final encrypted bytes to flash.
 */
esp_err_t esp_ota_end(esp_ota_handle_t handle);

/**
 * @brief   Set next boot partition, call system_restart() will switch to run it
 *
 * @note    if you want switch to run a bin file 
 *          has never been checked before,please validate it's signature firstly
 *
 * @param   partition Pointer to partition structure which need to boot
 *
 * @return: 
 *    - ESP_OK: if set next boot partition OK
 *    - ESP_ERR_OTA_SELECT_INFO_INVALID: ota bin select info invalid
 */
esp_err_t esp_ota_set_boot_partition(const esp_partition_t* partition);

/**
 * @brief   Get partition info of current running image
 * 
 * @return  pointer to esp_partition_t structure, or NULL if no partition is found or 
 *          operate flash failed,This pointer is valid for the lifetime of the application.
 */
const esp_partition_t* esp_ota_get_boot_partition(void);

#ifdef __cplusplus
}
#endif

#endif /* OTA_OPS_H */
