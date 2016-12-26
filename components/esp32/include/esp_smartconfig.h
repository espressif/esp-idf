// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef __ESP_SMARTCONFIG_H__
#define __ESP_SMARTCONFIG_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SC_STATUS_WAIT = 0,             /**< Waiting to start connect */
    SC_STATUS_FIND_CHANNEL,         /**< Finding target channel */
    SC_STATUS_GETTING_SSID_PSWD,    /**< Getting SSID and password of target AP */
    SC_STATUS_LINK,                 /**< Connecting to target AP */
    SC_STATUS_LINK_OVER,            /**< Connected to AP successfully */
} smartconfig_status_t;

typedef enum {
    SC_TYPE_ESPTOUCH = 0,       /**< protocol: ESPTouch */
    SC_TYPE_AIRKISS,            /**< protocol: AirKiss */
    SC_TYPE_ESPTOUCH_AIRKISS,   /**< protocol: ESPTouch and AirKiss */
} smartconfig_type_t;

/**
  * @brief  The callback of SmartConfig, executed when smart-config status changed.
  *
  * @param  status  Status of SmartConfig:
  *    - SC_STATUS_GETTING_SSID_PSWD : pdata is a pointer of smartconfig_type_t, means config type.
  *    - SC_STATUS_LINK : pdata is a pointer of struct station_config.
  *    - SC_STATUS_LINK_OVER : pdata is a pointer of phone's IP address(4 bytes) if pdata unequal NULL.
  *    - otherwise : parameter void *pdata is NULL.
  * @param  pdata  According to the different status have different values.
  *
  */
typedef void (*sc_callback_t)(smartconfig_status_t status, void *pdata);

/**
  * @brief  Get the version of SmartConfig.
  *
  * @return
  *     - SmartConfig version const char.
  */
const char *esp_smartconfig_get_version(void);

/**
  * @brief     Start SmartConfig, config ESP device to connect AP. You need to broadcast information by phone APP.
  *            Device sniffer special packets from the air that containing SSID and password of target AP.
  *
  * @attention 1. This API can be called in station or softAP-station mode.
  * @attention 2. Can not call esp_smartconfig_start twice before it finish, please call
  *               esp_smartconfig_stop first.
  *
  * @param     cb  SmartConfig callback function.
  * @param     ... log  1: UART output logs; 0: UART only outputs the result.
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_smartconfig_start(sc_callback_t cb, ...);

/**
  * @brief     Stop SmartConfig, free the buffer taken by esp_smartconfig_start.
  *
  * @attention Whether connect to AP succeed or not, this API should be called to free
  *            memory taken by smartconfig_start.
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_smartconfig_stop(void);

/**
  * @brief     Set timeout of SmartConfig process.
  *
  * @attention Timing starts from SC_STATUS_FIND_CHANNEL status. SmartConfig will restart if timeout.
  *
  * @param     time_s  range 15s~255s, offset:45s.
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_esptouch_set_timeout(uint8_t time_s);

/**
  * @brief     Set protocol type of SmartConfig.
  *
  * @attention If users need to set the SmartConfig type, please set it before calling
  *            esp_smartconfig_start.
  *
  * @param     type  Choose from the smartconfig_type_t.
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_smartconfig_set_type(smartconfig_type_t type);

/**
  * @brief     Set mode of SmartConfig. default normal mode.
  *
  * @attention 1. Please call it before API esp_smartconfig_start.
  * @attention 2. Fast mode have corresponding APP(phone).
  * @attention 3. Two mode is compatible.
  *
  * @param     enable  false-disable(default); true-enable;
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_smartconfig_fast_mode(bool enable);

#ifdef __cplusplus
}
#endif

#endif
