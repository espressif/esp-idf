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
#ifndef __ESP_SMARTCONFIG_H__
#define __ESP_SMARTCONFIG_H__
#include <stdint.h>
#include <esp_types.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SC_STATUS_WAIT = 0,             /**< waiting, do not start connection in this phase */
    SC_STATUS_FIND_CHANNEL,         /**< find target channel, start connection by APP in this phase */
    SC_STATUS_GETTING_SSID_PSWD,    /**< getting SSID and password of target AP */
    SC_STATUS_LINK,                 /**< connecting to target AP */
    SC_STATUS_LINK_OVER,            /**< got IP, connect to AP successfully */
} smartconfig_status_t;

typedef enum {
    SC_TYPE_ESPTOUCH = 0,       /**< protocol: ESPTouch */
    SC_TYPE_AIRKISS,            /**< protocol: AirKiss */
    SC_TYPE_ESPTOUCH_AIRKISS,   /**< protocol: ESPTouch and AirKiss */
} smartconfig_type_t;

/**
  * @brief  The callback of SmartConfig, executed when smart-config status changed.
  *
  * @param  smartconfig_status_t status : status of SmartConfig:
  *    -  if status == SC_STATUS_GETTING_SSID_PSWD, parameter void *pdata is a pointer
                       of smartconfig_type_t, means SmartConfig type: AirKiss or ESP-TOUCH.
  *    -  if status == SC_STATUS_LINK, parameter void *pdata is a pointer of struct station_config;
  *    -  if status == SC_STATUS_LINK_OVER, parameter void *pdata is a pointer of mobile
  *                    phone's IP address, 4 bytes. This is only available in ESPTOUCH, otherwise,
  *                    it is NULL.
  *    -  otherwise, parameter void *pdata is NULL.
  * @param  void *pdata : data of SmartConfig
  *
  * @return null
  */
typedef void (*sc_callback_t)(smartconfig_status_t status, void *pdata);

/**
  * @brief  Get the version of SmartConfig.
  *
  * @param  null
  *
  * @return SmartConfig version
  */
const char *esp_smartconfig_get_version(void);

/**
  * @brief     Start SmartConfig mode.
  *
  *            Start SmartConfig mode, to connect ESP32 station to AP, by sniffing
  *            for special packets from the air, containing SSID and password of desired AP.
  *            You need to broadcast the SSID and password (e.g. from mobile device or computer)
  *            with the SSID and password encoded.
  *
  * @attention 1. This API can only be called in station mode.
  * @attention 2. During SmartConfig, ESP32 station and soft-AP are disabled.
  * @attention 3. Can not call esp_smartconfig_start twice before it finish, please call
  *               esp_smartconfig_stop first.
  * @attention 4. Don't call any other APIs during SmartConfig, please call esp_smartconfig_stop first.
  *
  * @param     sc_callback_t cb : SmartConfig callback; executed when SmartConfig status changed;
  * @param     uint8 log : 1, UART output logs; otherwise, UART only outputs the result.
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_smartconfig_start(sc_callback_t cb, ...);

/**
  * @brief     Stop SmartConfig, free the buffer taken by esp_smartconfig_start.
  *
  * @attention Whether connect to AP succeed or not, this API should be called to free
  *            memory taken by smartconfig_start.
  *
  * @param     null
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_smartconfig_stop(void);

/**
  * @brief     Set timeout of SmartConfig.
  *
  * @attention SmartConfig timeout start at SC_STATUS_FIND_CHANNEL, SmartConfig will
  *            restart if timeout.
  *
  * @param     uint8 time_s : range 15s~255s, offset:45s.
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_esptouch_set_timeout(uint8_t time_s);

/**
  * @brief     Set protocol type of SmartConfig.
  *
  * @attention If users need to set the SmartConfig type, please set it before calling
  *            esp_smartconfig_start.
  *
  * @param     smartconfig_type_t type : AirKiss, ESP-TOUCH or both.
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_smartconfig_set_type(smartconfig_type_t type);

/**
  * @brief     Set mode of SmartConfig. default normal mode.
  *
  * @attention If users need to set the SmartConfig mode, please set it before calling
  *            esp_smartconfig_start. Different mode should match different APP(phone).
  *
  * @param     bool enable : false-disable(default); true-enable; 
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_smartconfig_fast_mode(bool enable);


#ifdef __cplusplus
}
#endif

#endif
