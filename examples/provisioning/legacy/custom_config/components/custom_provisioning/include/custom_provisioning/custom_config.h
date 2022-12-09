// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _CUSTOM_PROV_CONFIG_H_
#define _CUSTOM_PROV_CONFIG_H_

/**
 * @brief   Custom config data received by device
 */
typedef struct {
    char info[128];
    int  version;
} custom_config_t;

/**
 * @brief   Internal handler for receiving and responding to protocomm
 *          requests from master
 *
 * This is to be passed as priv_data for protocomm request handler
 * (refer to `custom_prov_config_data_handler()`) when calling `protocomm_add_endpoint()`.
 */
typedef esp_err_t (*custom_prov_config_handler_t) (const custom_config_t *config);

/**
 * @brief   Handler for receiving and responding to requests from master
 *
 * This is to be registered as the `wifi_config` endpoint handler
 * (protocomm `protocomm_req_handler_t`) using `protocomm_add_endpoint()`
 */
esp_err_t custom_prov_config_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                          uint8_t **outbuf, ssize_t *outlen, void *priv_data);

#endif
