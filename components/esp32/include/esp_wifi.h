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

#ifndef __ESP_WIFI_H__
#define __ESP_WIFI_H__

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WIFI_MODE_NULL = 0,  /**< null mode */
    WIFI_MODE_STA,       /**< WiFi station mode */
    WIFI_MODE_AP,        /**< WiFi soft-AP mode */
    WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
    WIFI_MODE_MAX
} wifi_mode_t;

typedef enum {
    WIFI_IF_STA = 0,     /**< ESP32 station interface */
    WIFI_IF_AP,          /**< ESP32 soft-AP interface */
    WIFI_IF_MAX
} wifi_interface_t;

typedef enum {
    WIFI_COUNTRY_CN = 0,
    WIFI_COUNTRY_JP,
    WIFI_COUNTRY_US,
    WIFI_COUNTRY_EU,
    WIFI_COUNTRY_MAX
} wifi_country_t;

typedef enum {
    WIFI_AUTH_OPEN = 0,      /**< authenticate mode : open */
    WIFI_AUTH_WEP,           /**< authenticate mode : WEP */
    WIFI_AUTH_WPA_PSK,       /**< authenticate mode : WPA_PSK */
    WIFI_AUTH_WPA2_PSK,      /**< authenticate mode : WPA2_PSK */
    WIFI_AUTH_WPA_WPA2_PSK,  /**< authenticate mode : WPA_WPA2_PSK */
    WIFI_AUTH_MAX
} wifi_auth_mode_t;

enum {
    WIFI_REASON_UNSPECIFIED              = 1,
    WIFI_REASON_AUTH_EXPIRE              = 2,
    WIFI_REASON_AUTH_LEAVE               = 3,
    WIFI_REASON_ASSOC_EXPIRE             = 4,
    WIFI_REASON_ASSOC_TOOMANY            = 5,
    WIFI_REASON_NOT_AUTHED               = 6,
    WIFI_REASON_NOT_ASSOCED              = 7,
    WIFI_REASON_ASSOC_LEAVE              = 8,
    WIFI_REASON_ASSOC_NOT_AUTHED         = 9,
    WIFI_REASON_DISASSOC_PWRCAP_BAD      = 10,
    WIFI_REASON_DISASSOC_SUPCHAN_BAD     = 11,
    WIFI_REASON_IE_INVALID               = 13,
    WIFI_REASON_MIC_FAILURE              = 14,
    WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,
    WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
    WIFI_REASON_IE_IN_4WAY_DIFFERS       = 17,
    WIFI_REASON_GROUP_CIPHER_INVALID     = 18,
    WIFI_REASON_PAIRWISE_CIPHER_INVALID  = 19,
    WIFI_REASON_AKMP_INVALID             = 20,
    WIFI_REASON_UNSUPP_RSN_IE_VERSION    = 21,
    WIFI_REASON_INVALID_RSN_IE_CAP       = 22,
    WIFI_REASON_802_1X_AUTH_FAILED       = 23,
    WIFI_REASON_CIPHER_SUITE_REJECTED    = 24,

    WIFI_REASON_BEACON_TIMEOUT           = 200,
    WIFI_REASON_NO_AP_FOUND              = 201,
    WIFI_REASON_AUTH_FAIL                = 202,
    WIFI_REASON_ASSOC_FAIL               = 203,
    WIFI_REASON_HANDSHAKE_TIMEOUT        = 204,
};

typedef enum {
    WIFI_SECOND_CHAN_NONE = 0,
    WIFI_SECOND_CHAN_ABOVE,
    WIFI_SECOND_CHAN_BELOW,
} wifi_second_chan_t;

typedef esp_err_t (* wifi_startup_cb_t)(void);

void esp_wifi_startup(wifi_startup_cb_t cb);

typedef struct {
    void    *event_q;
    uint8_t rx_ba_win;                /**< TBC */
    uint8_t tx_ba_win;                /**< TBC */
    uint8_t rx_buf_cnt;               /**< TBC */
    uint8_t tx_buf_cnt;               /**< TBC */
} wifi_init_config_t;

esp_err_t esp_wifi_init(wifi_init_config_t *config);

esp_err_t esp_wifi_deinit(void);

esp_err_t esp_wifi_set_mode(wifi_mode_t mode);

esp_err_t esp_wifi_get_mode(wifi_mode_t *mode);

esp_err_t esp_wifi_start(void);

esp_err_t esp_wifi_stop(void);

esp_err_t esp_wifi_connect(void);

esp_err_t esp_wifi_disconnect(void);

typedef struct {
    char *ssid;          /**< SSID of AP */
    uint8_t *bssid;      /**< MAC address of AP */
    uint8_t channel;     /**< channel, scan the specific channel */
    bool show_hidden;   /**< enable to scan AP whose SSID is hidden */
} wifi_scan_config_t;

esp_err_t esp_wifi_scan_start(wifi_scan_config_t *conf);

esp_err_t esp_wifi_scan_stop(void);

esp_err_t esp_wifi_get_ap_num(uint16_t *number);

typedef struct {
    uint8_t bssid[6];                     /**< MAC address of AP */
    uint8_t ssid[32];                     /**< SSID of AP */
    uint8_t primary;                      /**< channel of AP */
    wifi_second_chan_t second;            /**< second channel of AP */
    char    rssi;                         /**< single strength of AP */
    wifi_auth_mode_t authmode;            /**< authmode of AP */
}wifi_ap_list_t;

esp_err_t esp_wifi_get_ap_list(uint16_t *number, wifi_ap_list_t *ap_list);

typedef enum {
    WIFI_PS_NONE,
    WIFI_PS_MODEM,
    WIFI_PS_LIGHT,
    WIFI_PS_MAC,
} wifi_ps_type_t;

esp_err_t esp_wifi_set_ps(wifi_ps_type_t type);

esp_err_t esp_wifi_get_ps(wifi_ps_type_t *type);

#define WIFI_PROTOCOL_11B         1
#define WIFI_PROTOCOL_11G         2
#define WIFI_PROTOCOL_11N         4

esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol);

esp_err_t esp_wifi_get_protocol(wifi_interface_t ifx, uint8_t *protocol);

typedef enum {
    WIFI_BW_HT20 = 0,
    WIFI_BW_HT40,
} wifi_bandwidth_t;

esp_err_t esp_wifi_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw);

esp_err_t esp_wifi_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw);

esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);

esp_err_t esp_wifi_get_channel(uint8_t *primary, wifi_second_chan_t *second);

esp_err_t esp_wifi_set_country(wifi_country_t country);

esp_err_t esp_wifi_get_country(wifi_country_t *country);

esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, uint8_t mac[6]);

esp_err_t esp_wifi_get_mac(wifi_interface_t ifx, uint8_t mac[6]);

typedef void (* wifi_promiscuous_cb_t)(void *buf, uint16_t len);

wifi_promiscuous_cb_t wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

esp_err_t esp_wifi_set_promiscuous(uint8_t enable);

esp_err_t esp_wifi_get_promiscuous(uint8_t *enable);

typedef struct {
    char ssid[32];              /**< SSID of ESP32 soft-AP */
    char password[64];          /**< Password of ESP32 soft-AP */
    uint8_t ssid_len;           /**< Length of SSID. If softap_config.ssid_len==0, check the SSID until there is a termination character; otherwise, set the SSID length according to softap_config.ssid_len. */
    uint8_t channel;            /**< Channel of ESP32 soft-AP */
    wifi_auth_mode_t authmode;  /**< Auth mode of ESP32 soft-AP. Do not support AUTH_WEP in soft-AP mode */
    uint8_t ssid_hidden;        /**< Broadcast SSID or not, default 0, broadcast the SSID */
    uint8_t max_connection;     /**< Max number of stations allowed to connect in, default 4, max 4 */
    uint16_t beacon_interval;   /**< Beacon interval, 100 ~ 60000 ms, default 100 */
} wifi_ap_config_t;

typedef struct {
    char ssid[32];         /**< SSID of target AP*/
    char password[64];     /**< password of target AP*/
    bool bssid_set;        /**< whether set MAC address of target AP or not. Generally, station_config.bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.*/
    uint8_t bssid[6];     /**< MAC address of target AP*/
} wifi_sta_config_t;

typedef union {
    wifi_ap_config_t  ap;
    wifi_sta_config_t sta;
} wifi_config_t;

esp_err_t esp_wifi_set_config(wifi_interface_t ifx, wifi_config_t *conf);

esp_err_t esp_wifi_get_config(wifi_interface_t ifx, wifi_config_t *conf);

typedef esp_err_t (*wifi_rxcb_t)(void *buffer, uint16_t len, void* eb);

esp_err_t esp_wifi_reg_rxcb(wifi_interface_t ifx, wifi_rxcb_t fn);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
