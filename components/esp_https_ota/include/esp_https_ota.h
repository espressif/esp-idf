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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    HTTPS OTA Firmware upgrade.
 *
 * This function performs HTTPS OTA Firmware upgrade
 *
 * @param[in]  config       pointer to esp_http_client_config_t structure.
 *
 * @note     For secure HTTPS updates, the `cert_pem` member of `config`
 *           structure must be set to the server certificate.
 *
 * @return
 *    - ESP_OK: OTA data updated, next reboot will use specified partition.
 *    - ESP_FAIL: For generic failure.
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Invalid app image
 *    - ESP_ERR_NO_MEM: Cannot allocate memory for OTA operation.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 *    - For other return codes, refer OTA documentation in esp-idf's app_update component.
 */
esp_err_t esp_https_ota(const esp_http_client_config_t *config);

#ifdef __cplusplus
}
#endif
