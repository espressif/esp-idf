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

#ifndef _ESP_CDC_ECM_NETIF_GLUE_H_
#define _ESP_CDC_ECM_NETIF_GLUE_H_

#include "esp_cdc_ecm_driver.h"

void *esp_cdc_ecm_new_netif_glue(esp_cdc_ecm_driver_t* driver_handle);

esp_err_t esp_cdc_ecm_set_default_handlers(void *esp_netif);

#endif //_ESP_CDC_ECM_NETIF_GLUE_H_
