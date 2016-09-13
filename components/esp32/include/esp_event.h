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
#include "esp_wifi.h"

#include "tcpip_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SYSTEM_EVENT_WIFI_READY = 0,           /**< ESP32 WiFi ready */
    SYSTEM_EVENT_SCAN_DONE,                /**< ESP32 finish scanning AP */
    SYSTEM_EVENT_STA_START,                /**< ESP32 station start */
    SYSTEM_EVENT_STA_STOP,                 /**< ESP32 station stop */
    SYSTEM_EVENT_STA_CONNECTED,            /**< ESP32 station connected to AP */
    SYSTEM_EVENT_STA_DISCONNECTED,         /**< ESP32 station disconnected from AP */
    SYSTEM_EVENT_STA_AUTHMODE_CHANGE,      /**< the auth mode of AP connected by ESP32 station changed */
    SYSTEM_EVENT_STA_GOT_IP,               /**< ESP32 station got IP from connected AP */
    SYSTEM_EVENT_AP_START,                 /**< ESP32 soft-AP start */
    SYSTEM_EVENT_AP_STOP,                  /**< ESP32 soft-AP stop */
    SYSTEM_EVENT_AP_STACONNECTED,          /**< a station connected to ESP32 soft-AP */
    SYSTEM_EVENT_AP_STADISCONNECTED,       /**< a station disconnected from ESP32 soft-AP */
    SYSTEM_EVENT_AP_PROBEREQRECVED,        /**< Receive probe request packet in soft-AP interface */
    SYSTEM_EVENT_MAX
} system_event_id_t;

typedef struct {
    uint32_t status;          /**< status of scanning APs */
    uint8_t  number;
    uint8_t  scan_id;
} system_event_sta_scan_done_t;

typedef struct {
    uint8_t ssid[32];         /**< SSID of connected AP */
    uint8_t ssid_len;         /**< SSID length of connected AP */
    uint8_t bssid[6];         /**< BSSID of connected AP*/
    uint8_t channel;          /**< channel of connected AP*/
    wifi_auth_mode_t authmode;
} system_event_sta_connected_t;

typedef struct {
    uint8_t ssid[32];         /**< SSID of disconnected AP */
    uint8_t ssid_len;         /**< SSID length of disconnected AP */
    uint8_t bssid[6];         /**< BSSID of disconnected AP */
    uint8_t reason;           /**< reason of disconnection */
} system_event_sta_disconnected_t;

typedef struct {
    wifi_auth_mode_t old_mode;         /**< the old auth mode of AP */
    wifi_auth_mode_t new_mode;         /**< the new auth mode of AP */
} system_event_sta_authmode_change_t;

typedef struct {
    tcpip_adapter_ip_info_t ip_info;
} system_event_sta_got_ip_t;

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
    system_event_sta_got_ip_t                  got_ip;             /**< ESP32 station got IP */
    system_event_ap_staconnected_t             sta_connected;      /**< a station connected to ESP32 soft-AP */
    system_event_ap_stadisconnected_t          sta_disconnected;   /**< a station disconnected to ESP32 soft-AP */
    system_event_ap_probe_req_rx_t             ap_probereqrecved;  /**< ESP32 soft-AP receive probe request packet */
} system_event_info_t;

typedef struct {
    system_event_id_t     event_id;      /**< even ID */
    system_event_info_t   event_info;    /**< event information */
} system_event_t;

/**
  * @brief  Application specified event callback function
  *
  * @param  void *ctx : reversed for user
  * @param  system_event_t *event : event type defined in this file
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
typedef esp_err_t (*system_event_cb_t)(void *ctx, system_event_t *event);

/**
  * @brief  Set application specified event callback function
  *
  * @attention 1. If cb is NULL, means application don't need to handle
  *               If cb is not NULL, it will be call when an event is received, after the default event callback is completed
  *
  * @param  system_event_cb_t cb : callback
  * @param  void *ctx : reversed for user
  *
  * @return system_event_cb_t : old callback
  */
system_event_cb_t esp_event_set_cb(system_event_cb_t cb, void *ctx);

/**
  * @brief  Send a event to event task
  *
  * @attention 1. Other task/modules, such as the TCPIP module, can call this API to send an event to event task
  *
  * @param  system_event_t * event : event
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_event_send(system_event_t *event);

/**
  * @brief  Get the event handler
  *
  * @attention : currently this API returns event queue handler, generally this handler is used to
  *
  * @param  null
  *
  * @return void* : event queue pointer
  */
void *esp_event_get_handler(void);

/**
  * @brief  Init the event module
  *         Create the event handler and task
  *
  * @param  system_event_cb_t cb : application specified event callback, it can be modified by call esp_event_set_cb
  * @param  void *ctx : reversed for user
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_event_init(system_event_cb_t cb, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_EVENT_H__ */
