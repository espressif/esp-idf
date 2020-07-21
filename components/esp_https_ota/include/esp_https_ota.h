// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include <esp_http_client.h>
#include <bootloader_common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *esp_https_ota_handle_t;
typedef esp_err_t(*http_client_init_cb_t)(esp_http_client_handle_t);

/**
 * @brief ESP HTTPS OTA configuration
 */
typedef struct {
    const esp_http_client_config_t *http_config;   /*!< ESP HTTP client configuration */
    http_client_init_cb_t http_client_init_cb;     /*!< Callback after ESP HTTP client is initialised */
    bool bulk_flash_erase;                         /*!< Erase entire flash partition during initialization. By default flash partition is erased during write operation and in chunk of 4K sector size */
} esp_https_ota_config_t;

#define ESP_ERR_HTTPS_OTA_BASE            (0x9000)
#define ESP_ERR_HTTPS_OTA_IN_PROGRESS     (ESP_ERR_HTTPS_OTA_BASE + 1)  /* OTA operation in progress */

/**
 * @brief    HTTPS OTA Firmware upgrade.
 *
 * This function allocates HTTPS OTA Firmware upgrade context, establishes HTTPS connection, 
 * reads image data from HTTP stream and writes it to OTA partition and 
 * finishes HTTPS OTA Firmware upgrade operation.
 * This API supports URL redirection, but if CA cert of URLs differ then it
 * should be appended to `cert_pem` member of `config`.
 * 
 * @param[in]  config       pointer to esp_http_client_config_t structure.
 *
 * @note     This API handles the entire OTA operation, so if this API is being used
 *           then no other APIs from `esp_https_ota` component should be called.
 *           If more information and control is needed during the HTTPS OTA process,
 *           then one can use `esp_https_ota_begin` and subsequent APIs. If this API returns
 *           successfully, esp_restart() must be called to boot from the new firmware image.
 *
 * @return
 *    - ESP_OK: OTA data updated, next reboot will use specified partition.
 *    - ESP_FAIL: For generic failure.
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Invalid app image
 *    - ESP_ERR_NO_MEM: Cannot allocate memory for OTA operation.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 *    - For other return codes, refer OTA documentation in esp-idf's app_update component.
 */
esp_err_t esp_https_ota(const esp_http_client_config_t *config);

/**
 * @brief    Start HTTPS OTA Firmware upgrade
 *
 * This function initializes ESP HTTPS OTA context and establishes HTTPS connection.
 * This function must be invoked first. If this function returns successfully, then `esp_https_ota_perform` should be
 * called to continue with the OTA process and there should be a call to `esp_https_ota_finish` on
 * completion of OTA operation or on failure in subsequent operations.
 * This API supports URL redirection, but if CA cert of URLs differ then it
 * should be appended to `cert_pem` member of `http_config`, which is a part of `ota_config`.
 * In case of error, this API explicitly sets `handle` to NULL.
 *
 * @param[in]   ota_config       pointer to esp_https_ota_config_t structure
 * @param[out]  handle           pointer to an allocated data of type `esp_https_ota_handle_t`
 *                               which will be initialised in this function
 *
 * @note     This API is blocking, so setting `is_async` member of `http_config` structure will
 *           result in an error.
 *
 * @return
 *    - ESP_OK: HTTPS OTA Firmware upgrade context initialised and HTTPS connection established
 *    - ESP_FAIL: For generic failure.
 *    - ESP_ERR_INVALID_ARG: Invalid argument (missing/incorrect config, certificate, etc.)
 *    - For other return codes, refer documentation in app_update component and esp_http_client 
 *      component in esp-idf.
 */
esp_err_t esp_https_ota_begin(esp_https_ota_config_t *ota_config, esp_https_ota_handle_t *handle);

/**
 * @brief    Read image data from HTTP stream and write it to OTA partition 
 *
 * This function reads image data from HTTP stream and writes it to OTA partition. This function 
 * must be called only if esp_https_ota_begin() returns successfully.
 * This function must be called in a loop since it returns after every HTTP read operation thus 
 * giving you the flexibility to stop OTA operation midway.
 * 
 * @param[in]  https_ota_handle  pointer to esp_https_ota_handle_t structure
 *
 * @return
 *    - ESP_ERR_HTTPS_OTA_IN_PROGRESS: OTA update is in progress, call this API again to continue.
 *    - ESP_OK: OTA update was successful
 *    - ESP_FAIL: OTA update failed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Invalid app image
 *    - ESP_ERR_NO_MEM: Cannot allocate memory for OTA operation.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 *    - For other return codes, refer OTA documentation in esp-idf's app_update component.
 */
esp_err_t esp_https_ota_perform(esp_https_ota_handle_t https_ota_handle);

/**
 * @brief   Checks if complete data was received or not
 *
 * @note    This API can be called just before esp_https_ota_finish() to validate if the complete image was indeed received.
 *
 * @param[in]   https_ota_handle pointer to esp_https_ota_handle_t structure
 *
 * @return
 *    - false
 *    - true
 */
bool esp_https_ota_is_complete_data_received(esp_https_ota_handle_t https_ota_handle);

/**
 * @brief    Clean-up HTTPS OTA Firmware upgrade and close HTTPS connection
 *
 * This function closes the HTTP connection and frees the ESP HTTPS OTA context.
 * This function switches the boot partition to the OTA partition containing the
 * new firmware image.
 *
 * @note     If this API returns successfully, esp_restart() must be called to
 *           boot from the new firmware image
 *
 * @param[in]  https_ota_handle   pointer to esp_https_ota_handle_t structure
 *
 * @return
 *    - ESP_OK: Clean-up successful
 *    - ESP_ERR_INVALID_STATE
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Invalid app image
 */
esp_err_t esp_https_ota_finish(esp_https_ota_handle_t https_ota_handle);


/**
 * @brief   Reads app description from image header. The app description provides information
 *          like the "Firmware version" of the image.
 * 
 * @note    This API can be called only after esp_https_ota_begin() and before esp_https_ota_perform().
 *          Calling this API is not mandatory.
 *
 * @param[in]   https_ota_handle   pointer to esp_https_ota_handle_t structure
 * @param[out]  new_app_info       pointer to an allocated esp_app_desc_t structure
 * 
 * @return 
 *    - ESP_ERR_INVALID_ARG: Invalid arguments
 *    - ESP_FAIL: Failed to read image descriptor
 *    - ESP_OK: Successfully read image descriptor
 */
esp_err_t esp_https_ota_get_img_desc(esp_https_ota_handle_t https_ota_handle, esp_app_desc_t *new_app_info);


/**
* @brief  This function returns OTA image data read so far.
*
* @note   This API should be called only if `esp_https_ota_perform()` has been called atleast once or
*         if `esp_https_ota_get_img_desc` has been called before.
*
* @param[in]   https_ota_handle   pointer to esp_https_ota_handle_t structure
*
* @return
*    - -1    On failure
*    - total bytes read so far
*/
int esp_https_ota_get_image_len_read(esp_https_ota_handle_t https_ota_handle);

#ifdef __cplusplus
}
#endif
