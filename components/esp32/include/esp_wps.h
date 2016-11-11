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

#ifndef __ESP_WPS_H__
#define __ESP_WPS_H__

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup WPS_APIs  WPS APIs
  * @brief ESP32 WPS APIs
  *
  * WPS can only be used when ESP32 station is enabled.
  *
  */

/** @addtogroup WPS_APIs
  * @{
  */

#define ESP_ERR_WIFI_REGISTRAR   (ESP_ERR_WIFI_BASE + 51)  /*!< WPS registrar is not supported */
#define ESP_ERR_WIFI_WPS_TYPE    (ESP_ERR_WIFI_BASE + 52)  /*!< WPS type error */
#define ESP_ERR_WIFI_WPS_SM      (ESP_ERR_WIFI_BASE + 53)  /*!< WPS state machine is not initialized */

typedef enum wps_type {
    WPS_TYPE_DISABLE = 0,
    WPS_TYPE_PBC,
    WPS_TYPE_PIN,
    WPS_TYPE_MAX,
} wps_type_t;

/**
  * @brief     Enable Wi-Fi WPS function.
  *
  * @attention WPS can only be used when ESP32 station is enabled.
  *
  * @param     wps_type_t wps_type : WPS type, so far only WPS_TYPE_PBC and WPS_TYPE_PIN is supported
  *
  * @return    
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_TYPE : wps type is invalid
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  *          - ESP_ERR_WIFI_FAIL : wps initialization fails
  */
esp_err_t esp_wifi_wps_enable(wps_type_t wps_type);

/**
  * @brief  Disable Wi-Fi WPS function and release resource it taken.
  *
  * @param  null
  *
  * @return    
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  */
esp_err_t esp_wifi_wps_disable(void);

/**
  * @brief     WPS starts to work.
  *
  * @attention WPS can only be used when ESP32 station is enabled.
  *
  * @param     timeout_ms : maximum blocking time before API return.
  *          - 0 : non-blocking
  *          - 1~120000 : blocking time (not supported in IDF v1.0)
  *
  * @return    
  *          - ESP_OK : succeed
  *          - ESP_ERR_WIFI_WPS_TYPE : wps type is invalid
  *          - ESP_ERR_WIFI_WPS_MODE : wifi is not in station mode or sniffer mode is on
  *          - ESP_ERR_WIFI_WPS_SM : wps state machine is not initialized
  *          - ESP_ERR_WIFI_FAIL : wps initialization fails
  */
esp_err_t esp_wifi_wps_start(int timeout_ms);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WPS_H__ */
