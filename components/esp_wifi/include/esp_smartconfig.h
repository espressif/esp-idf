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
#include "esp_event_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SC_TYPE_ESPTOUCH = 0,       /**< protocol: ESPTouch */
    SC_TYPE_AIRKISS,            /**< protocol: AirKiss */
    SC_TYPE_ESPTOUCH_AIRKISS,   /**< protocol: ESPTouch and AirKiss */
    SC_TYPE_ESPTOUCH_V2,        /**< protocol: ESPTouch v2*/
} smartconfig_type_t;

/** Smartconfig event declarations */
typedef enum {
    SC_EVENT_SCAN_DONE,                /*!< ESP32 station smartconfig has finished to scan for APs */
    SC_EVENT_FOUND_CHANNEL,            /*!< ESP32 station smartconfig has found the channel of the target AP */
    SC_EVENT_GOT_SSID_PSWD,            /*!< ESP32 station smartconfig got the SSID and password */
    SC_EVENT_SEND_ACK_DONE,            /*!< ESP32 station smartconfig has sent ACK to cellphone */
} smartconfig_event_t;

/** @brief smartconfig event base declaration */
ESP_EVENT_DECLARE_BASE(SC_EVENT);

/** Argument structure for SC_EVENT_GOT_SSID_PSWD event */
typedef struct {
    uint8_t ssid[32];           /**< SSID of the AP. Null terminated string. */
    uint8_t password[64];       /**< Password of the AP. Null terminated string. */
    bool bssid_set;             /**< whether set MAC address of target AP or not. */
    uint8_t bssid[6];           /**< MAC address of target AP. */
    smartconfig_type_t type;    /**< Type of smartconfig(ESPTouch or AirKiss). */
    uint8_t token;              /**< Token from cellphone which is used to send ACK to cellphone. */
    uint8_t cellphone_ip[4];    /**< IP address of cellphone. */
} smartconfig_event_got_ssid_pswd_t;

/** Configure structure for esp_smartconfig_start */
typedef struct {
    bool enable_log;                /**< Enable smartconfig logs. */
    bool esp_touch_v2_enable_crypt; /**< Enable ESPTouch v2 crypt. */
    char *esp_touch_v2_key;         /**< ESPTouch v2 crypt key, len should be 16. */
} smartconfig_start_config_t;

#define SMARTCONFIG_START_CONFIG_DEFAULT() { \
    .enable_log = false, \
    .esp_touch_v2_enable_crypt = false,\
    .esp_touch_v2_key = NULL \
};

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
  * @param     config pointer to smartconfig start configure structure
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_smartconfig_start(const smartconfig_start_config_t *config);

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

/**
  * @brief     Get reserved data of ESPTouch v2.
  *
  * @param     rvd_data  reserved  data
  * @param     len length  of  reserved data
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_smartconfig_get_rvd_data(uint8_t *rvd_data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif
