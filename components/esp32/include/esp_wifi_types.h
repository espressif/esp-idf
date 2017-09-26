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


#ifndef __ESP_WIFI_TYPES_H__
#define __ESP_WIFI_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include "rom/queue.h"
#include "esp_err.h"
#include "esp_interface.h"

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

typedef esp_interface_t wifi_interface_t;

#define WIFI_IF_STA ESP_IF_WIFI_STA
#define WIFI_IF_AP  ESP_IF_WIFI_AP

typedef enum {
    WIFI_COUNTRY_POLICY_AUTO,   /**< Country policy is auto, use the country info of AP to which the station is connected */
    WIFI_COUNTRY_POLICY_MANUAL, /**< Country policy is manual, always use the configured country info */
} wifi_country_policy_t;

typedef struct {
    char                  cc[3];   /**< country code string */
    uint8_t               schan;   /**< start channel */
    uint8_t               nchan;   /**< total channel number */
    wifi_country_policy_t policy;  /**< country policy */
} wifi_country_t;

typedef enum {
    WIFI_AUTH_OPEN = 0,         /**< authenticate mode : open */
    WIFI_AUTH_WEP,              /**< authenticate mode : WEP */
    WIFI_AUTH_WPA_PSK,          /**< authenticate mode : WPA_PSK */
    WIFI_AUTH_WPA2_PSK,         /**< authenticate mode : WPA2_PSK */
    WIFI_AUTH_WPA_WPA2_PSK,     /**< authenticate mode : WPA_WPA2_PSK */
    WIFI_AUTH_WPA2_ENTERPRISE,  /**< authenticate mode : WPA2_ENTERPRISE */
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
    WIFI_SECOND_CHAN_NONE = 0,  /**< the channel width is HT20 */
    WIFI_SECOND_CHAN_ABOVE,     /**< the channel width is HT40 and the second channel is above the primary channel */
    WIFI_SECOND_CHAN_BELOW,     /**< the channel width is HT40 and the second channel is below the primary channel */
} wifi_second_chan_t;

typedef enum {
    WIFI_SCAN_TYPE_ACTIVE = 0,  /**< active scan */
    WIFI_SCAN_TYPE_PASSIVE,     /**< passive scan */
} wifi_scan_type_t;

typedef struct {
    uint32_t min;  /**< minimum active scan time per channel, units: millisecond */
    uint32_t max;  /**< maximum active scan time per channel, units: millisecond, values above 1500ms may
                                          cause station to disconnect from AP and are not recommended.  */
} wifi_active_scan_time_t;

typedef union {
    wifi_active_scan_time_t active;  /**< active scan time per channel */
    uint32_t passive;                /**< passive scan time per channel, units: millisecond, values above 1500ms may
                                          cause station to disconnect from AP and are not recommended. */
} wifi_scan_time_t;

typedef struct {
    uint8_t *ssid;               /**< SSID of AP */
    uint8_t *bssid;              /**< MAC address of AP */
    uint8_t channel;             /**< channel, scan the specific channel */
    bool show_hidden;            /**< enable to scan AP whose SSID is hidden */
    wifi_scan_type_t scan_type;  /**< scan type, active or passive */
    wifi_scan_time_t scan_time;  /**< scan time per channel */
} wifi_scan_config_t;

typedef enum {
    WIFI_CIPHER_TYPE_NONE = 0,   /**< the cipher type is none */
    WIFI_CIPHER_TYPE_WEP40,      /**< the cipher type is WEP40 */
    WIFI_CIPHER_TYPE_WEP104,     /**< the cipher type is WEP104 */
    WIFI_CIPHER_TYPE_TKIP,       /**< the cipher type is TKIP */
    WIFI_CIPHER_TYPE_CCMP,       /**< the cipher type is CCMP */
    WIFI_CIPHER_TYPE_TKIP_CCMP,  /**< the cipher type is TKIP and CCMP */
    WIFI_CIPHER_TYPE_UNKNOWN,    /**< the cipher type is unknown */
} wifi_cipher_type_t;

typedef struct {
    uint8_t bssid[6];                     /**< MAC address of AP */
    uint8_t ssid[33];                     /**< SSID of AP */
    uint8_t primary;                      /**< channel of AP */
    wifi_second_chan_t second;            /**< second channel of AP */
    int8_t  rssi;                         /**< signal strength of AP */
    wifi_auth_mode_t authmode;            /**< authmode of AP */
    wifi_cipher_type_t pairwise_cipher;   /**< pairwise cipher of AP */
    wifi_cipher_type_t group_cipher;      /**< group cipher of AP */
    uint32_t phy_11b:1;                   /**< bit: 0 flag to identify if 11b mode is enabled or not */
    uint32_t phy_11g:1;                   /**< bit: 1 flag to identify if 11g mode is enabled or not */
    uint32_t phy_11n:1;                   /**< bit: 2 flag to identify if 11n mode is enabled or not */
    uint32_t phy_lr:1;                    /**< bit: 3 flag to identify if low rate is enabled or not */
    uint32_t wps:1;                       /**< bit: 4 flag to identify if WPS is supported or not */
    uint32_t reserved:27;                 /**< bit: 5..31 reserved */
} wifi_ap_record_t;

typedef enum {
    WIFI_FAST_SCAN = 0,                   /**< Do fast scan, scan will end after find SSID match AP */
    WIFI_ALL_CHANNEL_SCAN,                /**< All channel scan, scan will end after scan all the channel */
}wifi_scan_method_t;

typedef enum {
    WIFI_CONNECT_AP_BY_SIGNAL = 0,        /**< Sort match AP in scan list by RSSI */
    WIFI_CONNECT_AP_BY_SECURITY,          /**< Sort match AP in scan list by security mode */
}wifi_sort_method_t;

typedef enum {
    WIFI_PS_NONE,    /**< No power save */
    WIFI_PS_MODEM,   /**< Modem power save */
} wifi_ps_type_t;

#define WIFI_PROTOCOL_11B         1
#define WIFI_PROTOCOL_11G         2
#define WIFI_PROTOCOL_11N         4
#define WIFI_PROTOCOL_LR          8

typedef enum {
    WIFI_BW_HT20 = 1, /* Bandwidth is HT20 */
    WIFI_BW_HT40,     /* Bandwidth is HT40 */
} wifi_bandwidth_t;

typedef struct {
    uint8_t ssid[32];           /**< SSID of ESP32 soft-AP */
    uint8_t password[64];       /**< Password of ESP32 soft-AP */
    uint8_t ssid_len;           /**< Length of SSID. If softap_config.ssid_len==0, check the SSID until there is a termination character; otherwise, set the SSID length according to softap_config.ssid_len. */
    uint8_t channel;            /**< Channel of ESP32 soft-AP */
    wifi_auth_mode_t authmode;  /**< Auth mode of ESP32 soft-AP. Do not support AUTH_WEP in soft-AP mode */
    uint8_t ssid_hidden;        /**< Broadcast SSID or not, default 0, broadcast the SSID */
    uint8_t max_connection;     /**< Max number of stations allowed to connect in, default 4, max 4 */
    uint16_t beacon_interval;   /**< Beacon interval, 100 ~ 60000 ms, default 100 ms */
} wifi_ap_config_t;

typedef struct {
    uint8_t ssid[32];      /**< SSID of target AP*/
    uint8_t password[64];  /**< password of target AP*/
    wifi_scan_method_t scan_method;    /**< do all channel scan or fast scan */
    bool bssid_set;        /**< whether set MAC address of target AP or not. Generally, station_config.bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.*/
    uint8_t bssid[6];     /**< MAC address of target AP*/
    uint8_t channel;       /**< channel of target AP. Set to 1~13 to scan starting from the specified channel before connecting to AP. If the channel of AP is unknown, set it to 0.*/
    wifi_sort_method_t sort_method;    /**< sort the connect AP in the list by rssi or security mode */
} wifi_sta_config_t;

typedef union {
    wifi_ap_config_t  ap;  /**< configuration of AP */
    wifi_sta_config_t sta; /**< configuration of STA */
} wifi_config_t;

typedef struct {
    uint8_t mac[6];  /**< mac address of sta that associated with ESP32 soft-AP */
} wifi_sta_info_t;

#define ESP_WIFI_MAX_CONN_NUM  (10)       /**< max number of stations which can connect to ESP32 soft-AP */

typedef struct {
    wifi_sta_info_t sta[ESP_WIFI_MAX_CONN_NUM]; /**< station list */
    int       num; /**< number of station that associated with ESP32 soft-AP */
} wifi_sta_list_t;

typedef enum {
    WIFI_STORAGE_FLASH,  /**< all configuration will strore in both memory and flash */
    WIFI_STORAGE_RAM,    /**< all configuration will only store in the memory */
} wifi_storage_t;

/**
  * @brief     Vendor Information Element type
  *
  * Determines the frame type that the IE will be associated with.
  */
typedef enum {
    WIFI_VND_IE_TYPE_BEACON,
    WIFI_VND_IE_TYPE_PROBE_REQ,
    WIFI_VND_IE_TYPE_PROBE_RESP,
    WIFI_VND_IE_TYPE_ASSOC_REQ,
    WIFI_VND_IE_TYPE_ASSOC_RESP,
} wifi_vendor_ie_type_t;

/**
  * @brief     Vendor Information Element index
  *
  * Each IE type can have up to two associated vendor ID elements.
  */
typedef enum {
    WIFI_VND_IE_ID_0,
    WIFI_VND_IE_ID_1,
} wifi_vendor_ie_id_t;

#define WIFI_VENDOR_IE_ELEMENT_ID 0xDD

/**
 * @brief Vendor Information Element header
 *
 * The first bytes of the Information Element will match this header. Payload follows.
 */
typedef struct {
    uint8_t element_id;      /**< Should be set to WIFI_VENDOR_IE_ELEMENT_ID (0xDD) */
    uint8_t length;          /**< Length of all bytes in the element data following this field. Minimum 4. */
    uint8_t vendor_oui[3];   /**< Vendor identifier (OUI). */
    uint8_t vendor_oui_type; /**< Vendor-specific OUI type. */
    uint8_t payload[0];      /**< Payload. Length is equal to value in 'length' field, minus 4. */
} vendor_ie_data_t;

typedef struct {
    signed rssi:8;            /**< signal intensity of packet */
    unsigned rate:5;          /**< data rate */
    unsigned :1;              /**< reserve */
    unsigned sig_mode:2;      /**< 0:is not 11n packet; 1:is 11n packet */
    unsigned :16;             /**< reserve */
    unsigned mcs:7;           /**< if is 11n packet, shows the modulation(range from 0 to 76) */
    unsigned cwb:1;           /**< if is 11n packet, shows if is HT40 packet or not */
    unsigned :16;             /**< reserve */
    unsigned smoothing:1;     /**< reserve */
    unsigned not_sounding:1;  /**< reserve */
    unsigned :1;              /**< reserve */
    unsigned aggregation:1;   /**< Aggregation */
    unsigned stbc:2;          /**< STBC */
    unsigned fec_coding:1;    /**< if is 11n packet, shows if is LDPC packet or not */
    unsigned sgi:1;           /**< SGI */
    unsigned noise_floor:8;   /**< noise floor */
    unsigned ampdu_cnt:8;     /**< ampdu cnt */
    unsigned channel:4;       /**< which channel this packet in */
    unsigned :12;             /**< reserve */
    unsigned timestamp:32;    /**< timestamp */
    unsigned :32;             /**< reserve */
    unsigned :32;             /**< reserve */
    unsigned sig_len:12;      /**< It is really lenth of packet */
    unsigned :12;             /**< reserve */
    unsigned rx_state:8;      /**< rx state */
} wifi_pkt_rx_ctrl_t;

typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl;
    uint8_t payload[0];       /**< ieee80211 packet buff, The length of payload is described by sig_len */
} wifi_promiscuous_pkt_t;

/**
  * @brief     Promiscuous frame type
  *
  */
typedef enum {
    WIFI_PKT_MGMT,  /**< management type, receive packet buf is wifi_promiscuous_pkt_t */
    WIFI_PKT_DATA,  /**< data type, receive packet buf is wifi_promiscuous_pkt_t */
    WIFI_PKT_MISC,  /**< other type, such as MIMO etc, receive packet buf is wifi_promiscuous_pkt_t but the payload is NULL!!! */
} wifi_promiscuous_pkt_type_t;


#define WIFI_PROMIS_FILTER_MASK_ALL         (0xFFFFFFFF)  /**< filter all packets */
#define WIFI_PROMIS_FILTER_MASK_MGMT        (1)           /**< filter the packets with type of WIFI_PKT_MGMT */
#define WIFI_PROMIS_FILTER_MASK_DATA        (1<<1)        /**< filter the packets with type of WIFI_PKT_DATA */
#define WIFI_PROMIS_FILTER_MASK_MISC        (1<<2)        /**< filter the packets with type of WIFI_PKT_MISC */
#define WIFI_PROMIS_FILTER_MASK_DATA_MPDU   (1<<3)        /**< filter the MPDU which is a kind of WIFI_PKT_DATA */
#define WIFI_PROMIS_FILTER_MASK_DATA_AMPDU  (1<<4)        /**< filter the AMPDU which is a kind of WIFI_PKT_DATA */

typedef struct {
    uint32_t filter_mask;
} wifi_promiscuous_filter_t;

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_TYPES_H__ */
