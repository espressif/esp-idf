/*
Copyright © 2021 Ci4Rail GmbH
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _ESP_CDC_ECM_DRIVER_H_
#define _ESP_CDC_ECM_DRIVER_H_

#include "esp_netif.h"

typedef struct {
    esp_netif_t* netif;
    uint8_t mac_address[6];
    esp_err_t (*stack_input)(uint8_t*, uint32_t, esp_netif_t*);
} esp_cdc_ecm_driver_t;

esp_err_t esp_cdc_ecm_driver_install(esp_cdc_ecm_driver_t** return_handle);

esp_err_t esp_cdc_ecm_driver_start(esp_cdc_ecm_driver_t* handle, esp_netif_t* netif);

esp_err_t esp_cdc_ecm_transmit(void* h, void* buffer, size_t len);

void esp_cdc_ecm_free_rx_buffer(void *h, void* buffer);

#endif //_ESP_CDC_ECM_DRIVER_H_
