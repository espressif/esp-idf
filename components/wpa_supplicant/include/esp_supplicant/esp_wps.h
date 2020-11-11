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

#ifndef __ESP_WPS_H__
#define __ESP_WPS_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi_crypto_types.h"
#include "esp_compiler.h"

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

#define WPS_MAX_MANUFACTURER_LEN 65
#define WPS_MAX_MODEL_NUMBER_LEN 33
#define WPS_MAX_MODEL_NAME_LEN   33
#define WPS_MAX_DEVICE_NAME_LEN  33

typedef struct {
    char manufacturer[WPS_MAX_MANUFACTURER_LEN]; /*!< Manufacturer, null-terminated string. The default manufcturer is used if the string is empty */
    char model_number[WPS_MAX_MODEL_NUMBER_LEN]; /*!< Model number, null-terminated string. The default model number is used if the string is empty */
    char model_name[WPS_MAX_MODEL_NAME_LEN];     /*!< Model name, null-terminated string. The default model name is used if the string is empty */
    char device_name[WPS_MAX_DEVICE_NAME_LEN];   /*!< Device name, null-terminated string. The default device name is used if the string is empty */
} wps_factory_information_t;

typedef struct {
    wps_type_t wps_type;
    wps_factory_information_t factory_info;
} esp_wps_config_t;

#define WPS_CONFIG_INIT_DEFAULT(type) { \
    .wps_type = type, \
    .factory_info = {   \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(manufacturer, "ESPRESSIF")  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(model_number, "ESP32")  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(model_name, "ESPRESSIF IOT")  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(device_name, "ESP STATION")  \
    }  \
}

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
  *          - ESP_FAIL : wps initialization fails
  */
esp_err_t esp_wifi_wps_enable(const esp_wps_config_t *config);

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
  *          - ESP_FAIL : wps initialization fails
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
