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
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_WIFI_OK          ESP_OK                    /*!< No error */
#define ESP_ERR_WIFI_FAIL        ESP_FAIL                  /*!< General fail code */
#define ESP_ERR_WIFI_NO_MEM      ESP_ERR_NO_MEM            /*!< Out of memory */
#define ESP_ERR_WIFI_ARG         ESP_ERR_INVALID_ARG       /*!< Invalid argument */
#define ESP_ERR_WIFI_NOT_SUPPORT ESP_ERR_NOT_SUPPORTED     /*!< Indicates that API is not supported yet */

#define ESP_ERR_WIFI_NOT_INIT    (ESP_ERR_WIFI_BASE + 1)   /*!< WiFi driver was not installed by esp_wifi_init */
#define ESP_ERR_WIFI_NOT_STARTED (ESP_ERR_WIFI_BASE + 2)   /*!< WiFi driver was not started by esp_wifi_start */
#define ESP_ERR_WIFI_IF          (ESP_ERR_WIFI_BASE + 3)   /*!< WiFi interface error */
#define ESP_ERR_WIFI_MODE        (ESP_ERR_WIFI_BASE + 4)   /*!< WiFi mode error */
#define ESP_ERR_WIFI_STATE       (ESP_ERR_WIFI_BASE + 5)   /*!< WiFi internal state error */
#define ESP_ERR_WIFI_CONN        (ESP_ERR_WIFI_BASE + 6)   /*!< WiFi internal control block of station or soft-AP error */
#define ESP_ERR_WIFI_NVS         (ESP_ERR_WIFI_BASE + 7)   /*!< WiFi internal NVS module error */
#define ESP_ERR_WIFI_MAC         (ESP_ERR_WIFI_BASE + 8)   /*!< MAC address is invalid */
#define ESP_ERR_WIFI_SSID        (ESP_ERR_WIFI_BASE + 9)   /*!< SSID is invalid */
#define ESP_ERR_WIFI_PASSWORD    (ESP_ERR_WIFI_BASE + 10)  /*!< Password is invalid */
#define ESP_ERR_WIFI_TIMEOUT     (ESP_ERR_WIFI_BASE + 11)  /*!< Timeout error */
#define ESP_ERR_WIFI_WAKE_FAIL   (ESP_ERR_WIFI_BASE + 12)  /*!< WiFi is in sleep state(RF closed) and wakeup fail */

/**
 * @brief WiFi stack configuration parameters passed to esp_wifi_init call.
 */
typedef struct {
    system_event_handler_t event_handler;          /**< WiFi event handler */
    int                    static_rx_buf_num;      /**< WiFi static RX buffer number */
    int                    dynamic_rx_buf_num;     /**< WiFi dynamic RX buffer number */
    int                    tx_buf_type;            /**< WiFi TX buffer type */
    int                    static_tx_buf_num;      /**< WiFi static TX buffer number */
    int                    dynamic_tx_buf_num;     /**< WiFi dynamic TX buffer number */
    int                    ampdu_enable;           /**< WiFi AMPDU feature enable flag */
    int                    nvs_enable;             /**< WiFi NVS flash enable flag */
    int                    nano_enable;            /**< Nano option for printf/scan family enable flag */
    int                    magic;                  /**< WiFi init magic number, it should be the last field */
} wifi_init_config_t;

#ifdef CONFIG_ESP32_WIFI_STATIC_TX_BUFFER_NUM
#define WIFI_STATIC_TX_BUFFER_NUM CONFIG_ESP32_WIFI_STATIC_TX_BUFFER_NUM
#else
#define WIFI_STATIC_TX_BUFFER_NUM 0
#endif

#ifdef CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM
#define WIFI_DYNAMIC_TX_BUFFER_NUM CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM
#else
#define WIFI_DYNAMIC_TX_BUFFER_NUM 0
#endif

#if CONFIG_ESP32_WIFI_AMPDU_ENABLED
#define WIFI_AMPDU_ENABLED        1
#else
#define WIFI_AMPDU_ENABLED        0
#endif

#if CONFIG_ESP32_WIFI_NVS_ENABLED
#define WIFI_NVS_ENABLED          1
#else
#define WIFI_NVS_ENABLED          0
#endif

#if CONFIG_NEWLIB_NANO_FORMAT
#define WIFI_NANO_FORMAT_ENABLED  1
#else
#define WIFI_NANO_FORMAT_ENABLED  0
#endif
 
#define WIFI_INIT_CONFIG_MAGIC    0x1F2F3F4F
#ifdef CONFIG_WIFI_ENABLED
#define WIFI_INIT_CONFIG_DEFAULT() { \
    .event_handler = &esp_event_send, \
    .static_rx_buf_num = CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM,\
    .dynamic_rx_buf_num = CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM,\
    .tx_buf_type = CONFIG_ESP32_WIFI_TX_BUFFER_TYPE,\
    .static_tx_buf_num = WIFI_STATIC_TX_BUFFER_NUM,\
    .dynamic_tx_buf_num = WIFI_DYNAMIC_TX_BUFFER_NUM,\
    .ampdu_enable = WIFI_AMPDU_ENABLED,\
    .nvs_enable = WIFI_NVS_ENABLED,\
    .nano_enable = WIFI_NANO_FORMAT_ENABLED,\
    .magic = WIFI_INIT_CONFIG_MAGIC\
};
#else
#define WIFI_INIT_CONFIG_DEFAULT() {0}; _Static_assert(0, "please enable wifi in menuconfig to use esp_wifi.h");
#endif

/**
  * @brief  Init WiFi
  *         Alloc resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
  *         WiFi NVS structure etc, this WiFi also start WiFi task
  *
  * @attention 1. This API must be called before all other WiFi API can be called
  * @attention 2. Always use WIFI_INIT_CONFIG_DEFAULT macro to init the config to default values, this can
  *               guarantee all the fields got correct value when more fields are added into wifi_init_config_t
  *               in future release. If you want to set your owner initial values, overwrite the default values
  *               which are set by WIFI_INIT_CONFIG_DEFAULT, please be notified that the field 'magic' of 
  *               wifi_init_config_t should always be WIFI_INIT_CONFIG_MAGIC!
  *
  * @param  config provide WiFi init configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NO_MEM: out of memory
  *    - others: refer to error code esp_err.h
  */
esp_err_t esp_wifi_init(wifi_init_config_t *config);

/**
  * @brief  Deinit WiFi
  *         Free all resource allocated in esp_wifi_init and stop WiFi task
  *
  * @attention 1. This API should be called if you want to remove WiFi driver from the system
  *
  * @return ESP_OK: succeed
  */
esp_err_t esp_wifi_deinit(void);

/**
  * @brief     Set the WiFi operating mode
  *
  *            Set the WiFi operating mode as station, soft-AP or station+soft-AP,
  *            The default mode is soft-AP mode.
  *
  * @param     mode  WiFi operating mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - others: refer to error code in esp_err.h
  */
esp_err_t esp_wifi_set_mode(wifi_mode_t mode);

/**
  * @brief  Get current operating mode of WiFi
  *
  * @param[out]  mode  store current WiFi mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  */
esp_err_t esp_wifi_get_mode(wifi_mode_t *mode);

/**
  * @brief  Start WiFi according to current configuration
  *         If mode is WIFI_MODE_STA, it create station control block and start station
  *         If mode is WIFI_MODE_AP, it create soft-AP control block and start soft-AP
  *         If mode is WIFI_MODE_APSTA, it create soft-AP and station control block and start soft-AP and station
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_NO_MEM: out of memory
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_FAIL: other WiFi internal errors
  */
esp_err_t esp_wifi_start(void);

/**
  * @brief  Stop WiFi
  *         If mode is WIFI_MODE_STA, it stop station and free station control block
  *         If mode is WIFI_MODE_AP, it stop soft-AP and free soft-AP control block
  *         If mode is WIFI_MODE_APSTA, it stop station/soft-AP and free station/soft-AP control block
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  */
esp_err_t esp_wifi_stop(void);

/**
 * @brief  Restore WiFi stack persistent settings to default values
 *
 * This function will reset settings made using the following APIs:
 * - esp_wifi_get_auto_connect,
 * - esp_wifi_set_protocol,
 * - esp_wifi_set_config related
 * - esp_wifi_set_mode
 *
 * @return
 *    - ESP_OK: succeed
 *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
 */
esp_err_t esp_wifi_restore(void);

/**
  * @brief     Connect the ESP32 WiFi station to the AP.
  *
  * @attention 1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
  * @attention 2. If the ESP32 is connected to an AP, call esp_wifi_disconnect to disconnect.
  *
  * @return 
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_NOT_START: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_SSID: SSID of AP which station connects is invalid
  */
esp_err_t esp_wifi_connect(void);

/**
  * @brief     Disconnect the ESP32 WiFi station from the AP.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi was not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_ERR_WIFI_FAIL: other WiFi internal errors
  */
esp_err_t esp_wifi_disconnect(void);

/**
  * @brief     Currently this API is just an stub API
  *

  * @return
  *    - ESP_OK: succeed
  *    - others: fail
  */
esp_err_t esp_wifi_clear_fast_connect(void);

/**
  * @brief     deauthenticate all stations or associated id equals to aid
  *
  * @param     aid  when aid is 0, deauthenticate all stations, otherwise deauthenticate station whose associated id is aid
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_MODE: WiFi mode is wrong
  */
esp_err_t esp_wifi_deauth_sta(uint16_t aid);

/**
  * @brief     Scan all available APs.
  *
  * @attention If this API is called, the found APs are stored in WiFi driver dynamic allocated memory and the
  *            will be freed in esp_wifi_get_ap_list, so generally, call esp_wifi_get_ap_list to cause
  *            the memory to be freed once the scan is done
  * @attention The values of maximum active scan time and passive scan time per channel are limited to 1500 milliseconds.
  *            Values above 1500ms may cause station to disconnect from AP and are not recommended.
  *
  * @param     config  configuration of scanning
  * @param     block if block is true, this API will block the caller until the scan is done, otherwise
  *                         it will return immediately
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_ERR_WIFI_TIMEOUT: blocking scan is timeout
  *    - others: refer to error code in esp_err.h
  */
esp_err_t esp_wifi_scan_start(wifi_scan_config_t *config, bool block);

/**
  * @brief     Stop the scan in process
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  */
esp_err_t esp_wifi_scan_stop(void);

/**
  * @brief     Get number of APs found in last scan
  *
  * @param[out] number  store number of APIs found in last scan
  *
  * @attention This API can only be called when the scan is completed, otherwise it may get wrong value.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_ARG: invalid argument
  */
esp_err_t esp_wifi_scan_get_ap_num(uint16_t *number);

/**
  * @brief     Get AP list found in last scan
  *
  * @param[inout]  number As input param, it stores max AP number ap_records can hold. 
  *                As output param, it receives the actual AP number this API returns.
  * @param         ap_records  wifi_ap_record_t array to hold the found APs
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_NO_MEM: out of memory
  */
esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records);


/**
  * @brief     Get information of AP which the ESP32 station is associated with
  *
  * @param     ap_info  the wifi_ap_record_t to hold AP information
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: fail
  */
esp_err_t esp_wifi_sta_get_ap_info(wifi_ap_record_t *ap_info);

/**
  * @brief     Set current power save type
  *
  * @attention Default power save type is WIFI_PS_NONE.
  *
  * @param     type  power save type
  *
  * @return    ESP_ERR_WIFI_NOT_SUPPORT: not supported yet
  */
esp_err_t esp_wifi_set_ps(wifi_ps_type_t type);

/**
  * @brief     Get current power save type
  *
  * @attention Default power save type is WIFI_PS_NONE.
  *
  * @param[out]  type: store current power save type
  *
  * @return    ESP_ERR_WIFI_NOT_SUPPORT: not supported yet
  */
esp_err_t esp_wifi_get_ps(wifi_ps_type_t *type);

/**
  * @brief     Set protocol type of specified interface
  *            The default protocol is (WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)
  *
  * @attention Currently we only support 802.11b or 802.11bg or 802.11bgn mode
  *
  * @param     ifx  interfaces
  * @param     protocol_bitmap  WiFi protocol bitmap
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - others: refer to error codes in esp_err.h
  */
esp_err_t esp_wifi_set_protocol(wifi_interface_t ifx, uint8_t protocol_bitmap);

/**
  * @brief     Get the current protocol bitmap of the specified interface
  *
  * @param     ifx  interface
  * @param[out] protocol_bitmap  store current WiFi protocol bitmap of interface ifx
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - others: refer to error codes in esp_err.h
  */
esp_err_t esp_wifi_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap);

/**
  * @brief     Set the bandwidth of ESP32 specified interface
  *
  * @attention 1. API return false if try to configure an interface that is not enabled
  * @attention 2. WIFI_BW_HT40 is supported only when the interface support 11N
  *
  * @param     ifx  interface to be configured
  * @param     bw  bandwidth
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - others: refer to error codes in esp_err.h
  */
esp_err_t esp_wifi_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw);

/**
  * @brief     Get the bandwidth of ESP32 specified interface
  *
  * @attention 1. API return false if try to get a interface that is not enable
  *
  * @param     ifx interface to be configured
  * @param[out] bw  store bandwidth of interface ifx
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_WIFI_ARG: invalid argument
  */
esp_err_t esp_wifi_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw);

/**
  * @brief     Set primary/secondary channel of ESP32
  *
  * @attention 1. This is a special API for sniffer
  * @attention 2. This API should be called after esp_wifi_start() or esp_wifi_set_promiscuous()
  *
  * @param     primary  for HT20, primary is the channel number, for HT40, primary is the primary channel
  * @param     second   for HT20, second is ignored, for HT40, second is the second channel
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_WIFI_ARG: invalid argument
  */
esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);

/**
  * @brief     Get the primary/secondary channel of ESP32
  *
  * @attention 1. API return false if try to get a interface that is not enable
  *
  * @param     primary   store current primary channel
  * @param[out]  second  store current second channel
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  */
esp_err_t esp_wifi_get_channel(uint8_t *primary, wifi_second_chan_t *second);

/**
  * @brief     Set country code
  *            The default value is WIFI_COUNTRY_CN
  *
  * @param     country  country type
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - others: refer to error code in esp_err.h
  */
esp_err_t esp_wifi_set_country(wifi_country_t country);

/**
  * @brief     Get country code
  *
  * @param     country  store current country
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  */
esp_err_t esp_wifi_get_country(wifi_country_t *country);

/**
  * @brief     Set MAC address of the ESP32 WiFi station or the soft-AP interface.
  *
  * @attention 1. This API can only be called when the interface is disabled
  * @attention 2. ESP32 soft-AP and station have different MAC addresses, do not set them to be the same.
  * @attention 3. The bit 0 of the first byte of ESP32 MAC address can not be 1. For example, the MAC address
  *      can set to be "1a:XX:XX:XX:XX:XX", but can not be "15:XX:XX:XX:XX:XX".
  *
  * @param     ifx  interface
  * @param     mac  the MAC address
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_WIFI_MAC: invalid mac address
  *    - ESP_ERR_WIFI_MODE: WiFi mode is wrong
  *    - others: refer to error codes in esp_err.h
  */
esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, uint8_t mac[6]);

/**
  * @brief     Get mac of specified interface
  *
  * @param      ifx  interface
  * @param[out] mac  store mac of the interface ifx
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF: invalid interface
  */
esp_err_t esp_wifi_get_mac(wifi_interface_t ifx, uint8_t mac[6]);

/**
  * @brief The RX callback function in the promiscuous mode. 
  *        Each time a packet is received, the callback function will be called.
  *
  * @param buf  Data received. Type of data in buffer (wifi_promiscuous_pkt_t or wifi_pkt_rx_ctrl_t) indicated by 'type' parameter.
  * @param type  promiscuous packet type.
  *
  */
typedef void (* wifi_promiscuous_cb_t)(void *buf, wifi_promiscuous_pkt_type_t type);

/**
  * @brief Register the RX callback function in the promiscuous mode.
  *
  * Each time a packet is received, the registered callback function will be called.
  *
  * @param cb  callback
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  */
esp_err_t esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

/**
  * @brief     Enable the promiscuous mode.
  *
  * @param     en  false - disable, true - enable
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  */
esp_err_t esp_wifi_set_promiscuous(bool en);

/**
  * @brief     Get the promiscuous mode.
  *
  * @param[out] en  store the current status of promiscuous mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
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
  * @param     ifx  interface
  * @param     conf  station or soft-AP configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_WIFI_MODE: invalid mode
  *    - ESP_ERR_WIFI_PASSWORD: invalid password
  *    - ESP_ERR_WIFI_NVS: WiFi internal NVS error
  *    - others: refer to the erro code in esp_err.h
  */
esp_err_t esp_wifi_set_config(wifi_interface_t ifx, wifi_config_t *conf);

/**
  * @brief     Get configuration of specified interface
  *
  * @param     ifx  interface
  * @param[out]  conf  station or soft-AP configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF: invalid interface
  */
esp_err_t esp_wifi_get_config(wifi_interface_t ifx, wifi_config_t *conf);

/**
  * @brief     Get STAs associated with soft-AP
  *
  * @attention SSC only API
  *
  * @param[out] sta  station list
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_MODE: WiFi mode is wrong
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, the station/soft-AP control block is invalid
  */
esp_err_t esp_wifi_ap_get_sta_list(wifi_sta_list_t *sta);


/**
  * @brief     Set the WiFi API configuration storage type
  *
  * @attention 1. The default value is WIFI_STORAGE_FLASH
  *
  * @param     storage : storage type
  *
  * @return
  *   - ESP_OK: succeed
  *   - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *   - ESP_ERR_WIFI_ARG: invalid argument
  */
esp_err_t esp_wifi_set_storage(wifi_storage_t storage);

/**
  * @brief     Set auto connect
  *            The default value is true
  *
  * @param     en : true - enable auto connect / false - disable auto connect
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_MODE: WiFi internal error, the station/soft-AP control block is invalid
  *    - others: refer to error code in esp_err.h
  */
esp_err_t esp_wifi_set_auto_connect(bool en);

/**
  * @brief     Get the auto connect flag
  *
  * @param[out] en  store current auto connect configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  */
esp_err_t esp_wifi_get_auto_connect(bool *en);

/**
  * @brief     Set vendor specific element
  *
  * @param     enable  enable or not
  * @param     type  information element type
  * @param     idx  information element index
  * @param     vnd_ie  pointer to a vendor specific element
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  *    - ESP_ERR_WIFI_ARG: invalid argument
  *    - ESP_ERR_WIFI_NO_MEM: out of memory
  */
esp_err_t esp_wifi_set_vendor_ie(bool enable, wifi_vendor_ie_type_t type, wifi_vendor_ie_id_t idx, uint8_t *vnd_ie);

/**
  * @brief     Define function pointer for vendor specific element callback
  * @param     ctx  reserved
  * @param     type  information element type 
  * @param     sa  source address
  * @param     vnd_ie  pointer to a vendor specific element
  * @param     rssi  received signal strength indication
  */
typedef void (*esp_vendor_ie_cb_t) (void *ctx, wifi_vendor_ie_type_t type, const uint8_t sa[6], const uint8_t *vnd_ie, int rssi);

/**
  * @brief     Set vendor specific element callback
  *
  * @param     cb   callback function
  * @param     ctx  reserved
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by eps_wifi_init
  */
esp_err_t esp_wifi_set_vendor_ie_cb(esp_vendor_ie_cb_t cb, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
