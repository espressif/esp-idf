// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_WPA_H__
#define __ESP_WPA_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi_crypto_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup WPA_APIs  WPS APIs
  * @brief ESP32 Supplicant APIs
  *
  */

/** @addtogroup WPA_APIs
  * @{
  */
/* Crypto callback functions */
const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs;
/* Mesh crypto callback functions */
const mesh_crypto_funcs_t g_wifi_default_mesh_crypto_funcs;

/**
  * @brief     Supplicant initialization
  *
  * @return    
  *          - ESP_OK : succeed
  *          - ESP_ERR_NO_MEM : out of memory
  */
esp_err_t esp_supplicant_init(void);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WPA_H__ */
