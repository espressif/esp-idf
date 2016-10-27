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


/*               Notes about WiFi Programming
 *
 *  The esp32 WiFi programming model can be depicted as following picture:
 *
 *
 *                            default handler              user handler
 *  -------------             ---------------             ---------------
 *  |           |   event     |             | callback or |             |
 *  |   tcpip   | --------->  |    event    | ----------> | application |
 *  |   stack   |             |     task    |  event      |    task     |
 *  |-----------|             |-------------|             |-------------|
 *                                  /|\                          |
 *                                   |                           |
 *                            event  |                           |
 *                                   |                           |
 *                                   |                           |
 *                             ---------------                   |
 *                             |             |                   |
 *                             | WiFi Driver |/__________________|
 *                             |             |\     API call
 *                             |             |
 *                             |-------------|
 *
 * The WiFi driver can be consider as black box, it knows nothing about the high layer code, such as
 * TCPIP stack, application task, event task etc, all it can do is to receive API call from high layer
 * or post event queue to a specified Queue, which is initialized by API esp_wifi_init().
 *
 * The event task is a daemon task, which receives events from WiFi driver or from other subsystem, such
 * as TCPIP stack, event task will call the default callback function on receiving the event. For example,
 * on receiving event SYSTEM_EVENT_STA_CONNECTED, it will call tcpip_adapter_start() to start the DHCP
 * client in it's default handler.
 *
 * Application can register it's own event callback function by API esp_event_init, then the application callback
 * function will be called after the default callback. Also, if application doesn't want to execute the callback
 * in the event task, what it needs to do is to post the related event to application task in the application callback function.
 *
 * The application task (code) generally mixes all these thing together, it calls APIs to init the system/WiFi and
 * handle the events when necessary.
 *
 */

#ifndef __ESP_WIFI_H__
#define __ESP_WIFI_H__

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "rom/queue.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    system_event_handler_t event_handler;  /**< WiFi event handler */
} wifi_init_config_t;


#define WIFI_INIT_CONFIG_DEFAULT() { \
    .event_handler = &esp_event_send, \
};

/**
  * @brief  Init WiFi
  *         Alloc resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
  *         WiFi NVS structure etc, this WiFi also start WiFi task
  *
  * @attention 1. This API must be called before all other WiFi API can be called
  * @attention 2. Generally we should init event_q in *config, WiFi driver will post the event
  *               to this queue when event happens, such as, when station connects to WiFi, WiFi driver
  *               will post station connected event to this queue. If the queue is not initialized, WiFi
  *               will not post any events
  *
  * @param  wifi_init_config_t *config : provide WiFi init configuration
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_wifi_init(wifi_init_config_t *config);

/**
  * @brief  Deinit WiFi
  *         Free all resource allocated in esp_wifi_init and stop WiFi task
  *
  * @attention 1. This API should be called if you want to remove WiFi driver from the system
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
  * @param  wifi_mode_t *mode : store current WiFi mode
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_mode(wifi_mode_t *mode);

/**
  * @brief  Start WiFi according to current configuration
  *         If mode is WIFI_MODE_STA, it create station control block and start station
  *         If mode is WIFI_MODE_AP, it create soft-AP control block and start soft-AP
  *         If mode is WIFI_MODE_APSTA, it create soft-AP and station control block and start soft-AP and station
  *
  * @param  null
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_wifi_start(void);

/**
  * @brief  Stop WiFi
            If mode is WIFI_MODE_STA, it stop station and free station control block
  *         If mode is WIFI_MODE_AP, it stop soft-AP and free soft-AP control block
  *         If mode is WIFI_MODE_APSTA, it stop station/soft-AP and free station/soft-AP control block
  *
  * @param  null
  *
  * @return ESP_OK : succeed
  * @return others : fail
  */
esp_err_t esp_wifi_stop(void);

/**
  * @brief     Connect the ESP32 WiFi station to the AP.
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
  * @brief     Disconnect the ESP32 WiFi station from the AP.
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
  * @brief     deauthenticate all stations or associated id equals to aid
  *
  * @param     uint16_t aid : when aid is 0, deauthenticate all stations, otherwise deauthenticate station whose associated id is aid
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_deauth_sta(uint16_t aid);

/**
  * @brief     Scan all available APs.
  *
  * @attention If this API is called, the found APs are stored in WiFi driver dynamic allocated memory and the
  *            will be freed in esp_wifi_get_ap_list, so generally, call esp_wifi_get_ap_list to cause
  *            the memory to be freed once the scan is done
  *
  * @param     struct scan_config *config : configuration of scanning
  * @param     bool block : if block is true, this API will block the caller until the scan is done, otherwise
  *                         it will return immediately
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
esp_err_t esp_wifi_scan_get_ap_num(uint16_t *number);

/**
  * @brief     Get AP list found in last scan
  *
  * @param     uint16_t *number : as input param, it stores max AP number ap_records can hold, as output param, it store
                                  the actual AP number this API returns
  * @param     wifi_ap_record_t *ap_records: wifi_ap_record_t array to hold the found APs
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records);


/**
  * @brief     Get information of AP associated with ESP32 station
  *
  * @param     wifi_ap_record_t *ap_info: the wifi_ap_record_t to hold station assocated AP
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_sta_get_ap_info(wifi_ap_record_t *ap_info);

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

/**
  * @brief     Set protocol type of specified interface
  *            The default protocol is (WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)
  *
  * @attention Currently we only support 802.11b or 802.11bg or 802.11bgn mode
  *
  * @param     wifi_interface_t ifx : interfaces
  * @param     uint8_t protocol : WiFi protocol bitmap
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap);

/**
  * @brief     Get the current protocol bitmap of specified ifx
  *
  * @param     wifi_interface_t ifx : interfaces
  * @param     uint8_t protocol : store current WiFi protocol bitmap of interface ifx
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap);

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
  * @param void *buf : the data received
  * @param uint16_t len : data length
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
  * @param     bool promiscuous : false - disable / true - enable
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_promiscuous(bool en);

/**
  * @brief     Get the promiscuous mode.
  *
  * @param     bool *enable : store the current status of promiscuous mode
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_promiscuous(bool *en);

/**
  * @brief     Set the configuration of the ESP32 STA or AP
  *
  * @attention 1. This API can be called only when specified interface is enabled, otherwise, API fail
  * @attention 2. For station configuration, bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.
  * @attention 3. ESP32 is limited to only one channel, so when in the soft-AP+station mode, the soft-AP will adjust its channel automatically to be the same as
  *               the channel of the ESP32 station.
  *
  * @param     wifi_interface_t ifx : interface
  * @param     wifi_config_t *conf : station or soft-AP configuration
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_config(wifi_interface_t ifx, wifi_config_t *conf);

/**
  * @brief     Get configuration of specified interface
  *
  * @param     wifi_interface_t ifx : interface
  * @param     wifi_config_t *conf : station or soft-AP configuration
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_get_config(wifi_interface_t ifx, wifi_config_t *conf);

/**
  * @brief     Get STAs associated with soft-AP
  *
  * @attention SSC only API
  *
  * @param     wifi_sta_list_t *sta:  station list
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_ap_get_sta_list(wifi_sta_list_t *sta);


/**
  * @brief     Set the WiFi API configuration storage type
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
  * @brief     The WiFi RX callback function
  *
  *            Each time the WiFi need to forward the packets to high layer, the callback function will be called
  *
  */
typedef esp_err_t (*wifi_rxcb_t)(void *buffer, uint16_t len, void *eb);

/**
  * @brief     Set the WiFi RX callback
  *
  * @attention 1. Currently we support only one RX callback for each interface
  *
  * @param     wifi_interface_t ifx : interface
  * @param     wifi_rxcb_t fn : WiFi RX callback
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

/**
  * @brief     Set vendor specific element
  *
  * @param     bool enable : enable or not
  * @param     wifi_vendor_ie_type_t type : 0 - WIFI_VND_IE_TYPE_BEACON
  *                           1 - WIFI_VND_IE_TYPE_PROBE_REQ
  *                           2 - WIFI_VND_IE_TYPE_PROBE_RESP
  *                           3 - WIFI_VND_IE_TYPE_ASSOC_REQ
  *                           4 - WIFI_VND_IE_TYPE_ASSOC_RESP
  * @param     wifi_vendor_ie_id_t idx : 0 - WIFI_VND_IE_ID_0
                                    1 - WIFI_VND_IE_ID_1
  * @param     uint8_t *vnd_ie : pointer to a vendor specific element
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_vendor_ie(bool enable, wifi_vendor_ie_type_t type, wifi_vendor_ie_id_t idx, uint8_t *vnd_ie);

/**
  * @brief     Define function pointer for vendor specific element callback
  * @param     void *ctx : reserved
  * @param     wifi_vendor_ie_type_t type : 0 - WIFI_VND_IE_TYPE_BEACON
  *                           1 - WIFI_VND_IE_TYPE_PROBE_REQ
  *                           2 - WIFI_VND_IE_TYPE_PROBE_RESP
  *                           3 - WIFI_VND_IE_TYPE_ASSOC_REQ
  *                           4 - WIFI_VND_IE_TYPE_ASSOC_RESP
  * @param     const uint8_t sa[6] : source address
  * @param     const uint8_t *vnd_ie : pointer to a vendor specific element
  * @param     int rssi : received signal strength indication
  */
typedef void (*esp_vendor_ie_cb_t) (void *ctx, wifi_vendor_ie_type_t type, const uint8_t sa[6], const uint8_t *vnd_ie, int rssi);

/**
  * @brief     Set vendor specific element callback
  *
  * @param     esp_vendor_ie_cb_t cb : callback function
  * @param     void *ctx : reserved
  *
  * @return    ESP_OK : succeed
  * @return    others : fail
  */
esp_err_t esp_wifi_set_vendor_ie_cb(esp_vendor_ie_cb_t cb, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
