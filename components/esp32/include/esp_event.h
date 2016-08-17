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

#ifndef __ESP_EVENT_H__
#define __ESP_EVENT_H__

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SYSTEM_EVENT_WIFI_READY = 0,           /**< ESP32 wifi ready */
    SYSTEM_EVENT_SCAN_DONE,                /**< ESP32 finish scanning AP */
    SYSTEM_EVENT_STA_START,                /**< ESP32 station start */
    SYSTEM_EVENT_STA_STOP,                 /**< ESP32 station start */
    SYSTEM_EVENT_STA_CONNECTED,            /**< ESP32 station connected to AP */
    SYSTEM_EVENT_STA_DISCONNECTED,         /**< ESP32 station disconnected to AP */
    SYSTEM_EVENT_STA_AUTHMODE_CHANGE,      /**< the auth mode of AP connected by ESP32 station changed */
    SYSTEM_EVENT_AP_START,                 /**< ESP32 softap start */
    SYSTEM_EVENT_AP_STOP,                  /**< ESP32 softap start */
    SYSTEM_EVENT_AP_STACONNECTED,          /**< a station connected to ESP32 soft-AP */
    SYSTEM_EVENT_AP_STADISCONNECTED,       /**< a station disconnected to ESP32 soft-AP */
    SYSTEM_EVENT_AP_PROBEREQRECVED,        /**< Receive probe request packet in soft-AP interface */
    SYSTEM_EVENT_MAX
} system_event_id_t;

typedef struct {
    uint32_t status;          /**< status of scanning APs*/
    uint8_t  number;
} system_event_sta_scan_done_t;

typedef struct {
    uint8_t ssid[32];         /**< SSID of connected AP */
    uint8_t ssid_len;         /**< SSID length of connected AP */
    uint8_t bssid[6];         /**< BSSID of connected AP*/
    uint8_t channel;          /**< channel of connected AP*/
} system_event_sta_connected_t;

typedef struct {
    uint8_t ssid[32];         /**< SSID of disconnected AP */
    uint8_t ssid_len;         /**< SSID length of disconnected AP */
    uint8_t bssid[6];         /**< BSSID of disconnected AP */
    uint8_t reason;           /**< reason of disconnection */
} system_event_sta_disconnected_t;

typedef struct {
    uint8_t old_mode;         /**< the old auth mode of AP */
    uint8_t new_mode;         /**< the new auth mode of AP */
} system_event_sta_authmode_change_t;

typedef struct {
    uint8_t mac[6];           /**< MAC address of the station connected to ESP32 soft-AP */
    uint8_t aid;              /**< the aid that ESP32 soft-AP gives to the station connected to  */
} system_event_ap_staconnected_t;

typedef struct {
    uint8_t mac[6];           /**< MAC address of the station disconnects to ESP32 soft-AP */
    uint8_t aid;              /**< the aid that ESP32 soft-AP gave to the station disconnects to  */
} system_event_ap_stadisconnected_t;

typedef struct {
    int rssi;                 /**< Received probe request signal strength */
    uint8_t mac[6];           /**< MAC address of the station which send probe request */
} system_event_ap_probe_req_rx_t;

typedef union {
    system_event_sta_connected_t               connected;          /**< ESP32 station connected to AP */
    system_event_sta_disconnected_t            disconnected;       /**< ESP32 station disconnected to AP */
    system_event_sta_scan_done_t               scan_done;          /**< ESP32 station scan (APs) done */
    system_event_sta_authmode_change_t         auth_change;        /**< the auth mode of AP ESP32 station connected to changed */
    system_event_ap_staconnected_t             sta_connected;      /**< a station connected to ESP32 soft-AP */
    system_event_ap_stadisconnected_t          sta_disconnected;   /**< a station disconnected to ESP32 soft-AP */
    system_event_ap_probe_req_rx_t             ap_probereqrecved;  /**< ESP32 softAP receive probe request packet */
} system_event_info_t;

typedef struct {
    system_event_id_t     event_id;      /**< even ID */
    system_event_info_t   event_info;    /**< event information */
} system_event_t;

typedef esp_err_t (*system_event_cb_t)(system_event_t *event);
system_event_cb_t esp_event_set_cb(system_event_cb_t cb);
esp_err_t esp_event_send(system_event_t * event);
void* esp_event_get_handler(void);
esp_err_t esp_event_init(system_event_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_EVENT_H__ */
