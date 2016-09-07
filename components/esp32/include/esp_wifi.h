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
#include "rom/queue.h"

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
    WIFI_COUNTRY_CN = 0, /**< country China, channel range [1, 14] */
    WIFI_COUNTRY_JP,     /**< country Japan, channel range [1, 14] */
    WIFI_COUNTRY_US,     /**< country USA, channel range [1, 11] */
    WIFI_COUNTRY_EU,     /**< country Europe, channel rane [1, 13] */
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
    WIFI_SECOND_CHAN_NONE = 0,  /**< the channel width is HT20 */
    WIFI_SECOND_CHAN_ABOVE,     /**< the channel width is HT40 and the second channel is above the primary channel */
    WIFI_SECOND_CHAN_BELOW,     /**< the channel width is HT40 and the second channel is below the primary channel */
} wifi_second_chan_t;

/**
  * @brief     startup wifi driver and register application specific callback function
  *
  * @attention 1. This API should be called in application startup code to init wifi driver
  * @attention 2. The callback fuction is used to provide application specific wifi configuration,
  *               such as, set the wifi mode, register the event callback, set ap ssid etc before
  *               wifi is startup
  * @attention 3. Avoid to create application task in the callback, otherwise you may get wrong behavior
  * @attention 4. If the callback return is not ESP_OK, the startup will fail!
  * @attention 5. Before this API can be called, system_init()/esp_event_init()/tcpip_adapter_init() should
  *               be called firstly
  *
  * @param  wifi_startup_cb_t cb : application specific callback function
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
typedef esp_err_t (* wifi_startup_cb_t)(void);
void esp_wifi_startup(wifi_startup_cb_t cb);

typedef struct {
    void    *event_q;                 /**< Wifi event q handler, it's a freertos queue */
    uint8_t rx_ba_win;                /**< TBC */
    uint8_t tx_ba_win;                /**< TBC */
    uint8_t rx_buf_cnt;               /**< TBC */
    uint8_t tx_buf_cnt;               /**< TBC */
} wifi_init_config_t;

/**
  * @brief  Init wifi
  *         Alloc resource for wifi driver, such as wifi control structure, rx/tx buffer,
  *         wifi nvs structure etc, this wifi also start wifi task
  *
  * @attention 1. This API must be called before all other wifi api can be called
  * @attention 2. Generally we should init event_q in *config, wifi driver will post the event
  *               to this queue when event happens, such as, when sta connects to api, wifi driver
  *               will post sta connected event to this queue. If the queue is not initialized, wifi
  *               will not post any events
  * @attention 3. For other paramters, currently it's not ready, just ignore it.
  *
  * @param  wifi_init_config_t *config : provide wifi init configuration
  * 
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_wifi_init(wifi_init_config_t *config);

/**
  * @brief  Deinit wifi 
  *         Free all resource allocated in esp_wifi_init and stop wifi task
  *
  * @attention 1. This API should be called if you want to remove wifi driver from the system
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_wifi_deinit(void);

/**
  * @brief     Set the WiFi operating mode
  *
  *            Set the WiFi operating mode as station, soft-AP or station+soft-AP,
  *            The default mode is soft-AP mode.
  *
  * @param     wifi_mode_t mode : WiFi operating modes:
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_mode(wifi_mode_t mode);

/**
  * @brief  Get current operating mode of WiFi
  *
  * @param  wifi_mode_t *mode : store current wifi mode
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_mode(wifi_mode_t *mode);

/**
  * @brief  Start wifi according to current configuration
  *         If mode is WIFI_MODE_STA, it create sta control block and start sta
  *         If mode is WIFI_MODE_AP, it create ap control block and start ap
  *         If mode is WIFI_MODE_APSTA, it create apd and sta control block and start ap and sta
  *
  * @param  null
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_wifi_start(void);

/**
  * @brief  Stop wifi 
            If mode is WIFI_MODE_STA, it stop sta and free sta control block
  *         If mode is WIFI_MODE_AP, it stop ap and free ap control block
  *         If mode is WIFI_MODE_APSTA, it stop sta/ap and free sta/ap control block
  *
  * @param  null
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_wifi_stop(void);

/**
  * @brief     Connect the ESP32 WiFi sta to the AP.
  *
  * @attention 1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
  * @attention 2. If the ESP32 is connected to an AP, call esp_wifi_disconnect to disconnect.
  *
  * @param     null
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_wifi_connect(void);

/**
  * @brief     Disconnect the ESP32 WiFi sta from the AP.
  *
  * @param     null
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_disconnect(void);

/**
  * @brief     Currently this API is just an stub API
  *
  * @param     null
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_clear_fast_connect(void);

/**
  * @brief     Kick the all sta or associated id equals to aid
  *
  * @param     uint16_t aid : when aid is 0, kick all sta, otherwise kick sta whose associated id is aid
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_kick_station(uint16_t aid);

typedef struct {
    char *ssid;          /**< SSID of AP */
    uint8_t *bssid;      /**< MAC address of AP */
    uint8_t channel;     /**< channel, scan the specific channel */
    bool show_hidden;    /**< enable to scan AP whose SSID is hidden */
} wifi_scan_config_t;

/**
  * @brief     Scan all available APs.
  *
  * @attention If this API is called, the found APs are stored in wifi driver dynamic allocated memory and the
  *            will be freed in esp_wifi_get_ap_list, so generally, call esp_wifi_get_ap_list to cause
  *            the memory to be freed once the scan is done
  *
  * @param     struct scan_config *config : configuration of scanning
  * @param     bool block : if block is true, this api will block the caller until the scan is done, otherwise
  *                         it will return immeidately
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_scan_start(wifi_scan_config_t *conf, bool block);

/**
  * @brief     Stop the scan in process
  *
  * @param     null
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_scan_stop(void);

/**
  * @brief     Get number of APs found in last scan
  *
  * @param     uint16_t *number : store number of APIs found in last scan
  *
  * @attention This API can only be called when the scan is completed, otherwise it may get wrong value
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_ap_num(uint16_t *number);

typedef struct {
    uint8_t bssid[6];                     /**< MAC address of AP */
    uint8_t ssid[32];                     /**< SSID of AP */
    uint8_t primary;                      /**< channel of AP */
    wifi_second_chan_t second;            /**< second channel of AP */
    signed char rssi;                     /**< signal strength of AP */
    wifi_auth_mode_t authmode;            /**< authmode of AP */
}wifi_ap_list_t;

/**
  * @brief     Get AP list found in last scan
  *
  * @param     uint16_t *number : as input param, it stores max AP number ap_list can hold, as output param, it store
                                  the actual AP number this API returns
  * @param     wifi_ap_list_t *ap_list : a list to hold the found APs
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_ap_list(uint16_t *number, wifi_ap_list_t *ap_list);

typedef enum {
    WIFI_PS_NONE,    /**< No power save */
    WIFI_PS_MODEM,   /**< Modem power save */
    WIFI_PS_LIGHT,   /**< Light power save */
    WIFI_PS_MAC,     /**< MAC power save */
} wifi_ps_type_t;

/**
  * @brief     Set current power save type 
  *
  * @param     wifi_ps_type_t type : power save type
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_ps(wifi_ps_type_t type);

/**
  * @brief     Get current power save type
  *
  * @param     wifi_ps_type_t *type : store current power save type
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_ps(wifi_ps_type_t *type);

#define WIFI_PROTOCOL_11B         1
#define WIFI_PROTOCOL_11G         2
#define WIFI_PROTOCOL_11N         4

/**
  * @brief     Set protocol type of specified interface
  *            The default protocol is (WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)
  *
  * @attention Currently we only support 802.11b or 802.11bg or 802.11bgn mode
  * 
  * @param     wifi_interface_t ifx : interfaces 
  * @param     uint8_t protocol : wifi protocol bitmap
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap);

/**
  * @brief     Get the current protocol bitmap of specified ifx
  *
  * @param     wifi_interface_t ifx : interfaces 
  * @param     uint8_t protocol : store current wifi protocol bitmap of interface ifx
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap);

typedef enum {
    WIFI_BW_HT20 = 0, /* Bandwidth is HT20 */
    WIFI_BW_HT40,     /* Bandwidth is HT40 */
} wifi_bandwidth_t;

/**
  * @brief     Set the bandwidth of ESP32 specified interface
  *
  * @attention 1. API return false if try to configure a interface that is not enable
  * @attention 2. WIFI_BW_HT40 is supported only when the interface support 11N
  *
  * @param     wifi_interface_t ifx : interface to be configured
  * @param     wifi_bandwidth_t bw : bandwidth
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw);

/**
  * @brief     Get the bandwidth of ESP32 specified interface
  *
  * @attention 1. API return false if try to get a interface that is not enable
  *
  * @param     wifi_interface_t ifx : interface to be configured
  * @param     wifi_bandwidth_t *bw : store bandwidth  of interface ifx
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw);

/**
  * @brief     Set primary/second channel of ESP32
  *
  * @attention 1. This is a special API for sniffer
  *
  * @param     uint8_t primary : for HT20, primary is the channel number, for HT40, primary is the primary channel
  * @param     wifi_second_chan_t second : for HT20, second is ignored, for HT40, second is the second channel
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);

/**
  * @brief     Get the primary/second channel of ESP32
  *
  * @attention 1. API return false if try to get a interface that is not enable
  *
  * @param     uint8_t *primary : store current primary channel
  * @param     wifi_second_chan_t *second : store current second channel
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_channel(uint8_t *primary, wifi_second_chan_t *second);

/**
  * @brief     Set country code
  *            The default value is WIFI_COUNTRY_CN
  *
  * @param     wifi_country_t country : country type
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_country(wifi_country_t country);

/**
  * @brief     Get country code
  *
  * @param     wifi_country_t country : store current country
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_country(wifi_country_t *country);

/**
  * @brief     Set MAC address of the ESP32 WiFi station or the soft-AP interface.
  *
  * @attention 1. This API can only be called when the interface is disabled
  * @attention 2. ESP32 soft-AP and station have different MAC addresses, do not set them to be the same.
  *    - The bit0 of the first byte of ESP32 MAC address can not be 1. For example, the MAC address
  *      can set to be "1a:XX:XX:XX:XX:XX", but can not be "15:XX:XX:XX:XX:XX".
  *
  * @param     wifi_interface_t ifx : interface 
  * @param     uint8 mac[6]: the MAC address.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, uint8_t mac[6]);

/**
  * @brief     Get mac of specified interface
  *
  * @param     uint8_t mac[6] : store mac of this interface ifx
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_mac(wifi_interface_t ifx, uint8_t mac[6]);

/**
  * @brief The RX callback function in the promiscuous mode.
  *
  *        Each time a packet is received, the callback function will be called.
  *
  * @param uint8 *buf : the data received
  * @param uint16 len : data length
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
typedef void (* wifi_promiscuous_cb_t)(void *buf, uint16_t len);

/**
  * @brief Register the RX callback function in the promiscuous mode.
  *
  *        Each time a packet is received, the registered callback function will be called.
  *
  * @param wifi_promiscuous_cb_t cb : callback
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

/**
  * @brief     Enable the promiscuous mode.
  *
  * @param     uint8 promiscuous : 0 - disable / 1 - enable 
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_promiscuous(uint8_t enable);

/**
  * @brief     Get the promiscuous mode.
  *
  * @param     uint8 *enable : store the current status of promiscuous mode
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
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
    wifi_ap_config_t  ap;  /**< configuration of AP */
    wifi_sta_config_t sta; /**< configuration of STA */
} wifi_config_t;

/**
  * @brief     Set the configuration of the ESP32 STA or AP
  *
  * @attention 1. This api can be called only when specified interface is enabled, otherwise, API fail
  * @attention 2. For sta configuration, bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.
  * @attention 3. ESP32 is limited to only one channel, so when in the soft-AP+station mode, the soft-AP will adjust its channel automatically to be the same as
  *               the channel of the ESP32 station.
  *
  * @param     wifi_interface_t ifx : interface 
  * @param     wifi_config_t *conf : sta or ap configuration
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_config(wifi_interface_t ifx, wifi_config_t *conf);

/**
  * @brief     Get configuration of specified interface
  *
  * @param     wifi_interface_t ifx : interface 
  * @param     wifi_config_t *conf : sta or ap configuration
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_config(wifi_interface_t ifx, wifi_config_t *conf);

struct station_info {
    STAILQ_ENTRY(station_info) next;
    uint8_t bssid[6];
};

/**
  * @brief     Get STAs associated with soft-AP 
  *
  * @attention SSC only API
  *
  * @param     struct station_info **station :  sta list
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_station_list(struct station_info **station);

esp_err_t esp_wifi_free_station_list(void);

typedef enum {
    WIFI_STORAGE_FLASH,  /**< all configuration will strore in both memory and flash */
    WIFI_STORAGE_RAM,    /**< all configuration will only store in the memory */
} wifi_storage_t;

/**
  * @brief     Set the wifi API configuration storage type
  *
  * @attention 1. The default value is WIFI_STORAGE_FLASH
  *
  * @param     wifi_storage_t storage : storage type
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_storage(wifi_storage_t storage);

/**
  * @brief     Set the wifi API configuration storage type
  *
  *            Each time the wifi need to forward the packets to high layer, the callback function will be called 
  *
  */
typedef esp_err_t (*wifi_rxcb_t)(void *buffer, uint16_t len, void* eb);

/**
  * @brief     Set the wifi rx callback
  *
  * @attention 1. Currently we support only one rx callback for each interface
  *
  * @param     wifi_interface_t ifx : interface 
  * @param     wifi_rxcb_t fn : wifi rx callback
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_reg_rxcb(wifi_interface_t ifx, wifi_rxcb_t fn);

/**
  * @brief     Set auto connect 
  *            The default value is true
  *
  * @attention 1. 
  *
  * @param     bool en : true - enable auto connect / false - disable auto connect
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_auto_connect(bool en);

/**
  * @brief     Get the auto connect flag
  *
  * @param     bool *en : store current auto connect configuration
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_auto_connect(bool *en);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
