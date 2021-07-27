
// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef ESP_WPA3_H
#define ESP_WPA3_H

#include "esp_wifi_driver.h"

#ifdef CONFIG_WPA3_SAE

void esp_wifi_register_wpa3_cb(struct wpa_funcs *wpa_cb);
void esp_wpa3_free_sae_data(void);

#else /* CONFIG_WPA3_SAE */

static inline void esp_wifi_register_wpa3_cb(struct wpa_funcs *wpa_cb)
{
    wpa_cb->wpa3_build_sae_msg = NULL;
    wpa_cb->wpa3_parse_sae_msg = NULL;
}

static inline void esp_wpa3_free_sae_data(void)
{
}

#endif /* CONFIG_WPA3_SAE */
#endif /* ESP_WPA3_H */
