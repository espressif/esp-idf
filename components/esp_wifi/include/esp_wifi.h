/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*               Notes about WiFi Programming
 *
 *  WiFi programming model can be depicted as following picture:
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
 * on receiving event WIFI_EVENT_STA_CONNECTED, it will call esp_netif API to start the DHCP
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
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_event.h"
#include "esp_wifi_crypto_types.h"
#include "esp_wifi_default.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_WIFI_NOT_INIT          (ESP_ERR_WIFI_BASE + 1)   /*!< WiFi driver was not installed by esp_wifi_init */
#define ESP_ERR_WIFI_NOT_STARTED       (ESP_ERR_WIFI_BASE + 2)   /*!< WiFi driver was not started by esp_wifi_start */
#define ESP_ERR_WIFI_NOT_STOPPED       (ESP_ERR_WIFI_BASE + 3)   /*!< WiFi driver was not stopped by esp_wifi_stop */
#define ESP_ERR_WIFI_IF                (ESP_ERR_WIFI_BASE + 4)   /*!< WiFi interface error */
#define ESP_ERR_WIFI_MODE              (ESP_ERR_WIFI_BASE + 5)   /*!< WiFi mode error */
#define ESP_ERR_WIFI_STATE             (ESP_ERR_WIFI_BASE + 6)   /*!< WiFi internal state error */
#define ESP_ERR_WIFI_CONN              (ESP_ERR_WIFI_BASE + 7)   /*!< WiFi internal control block of station or soft-AP error */
#define ESP_ERR_WIFI_NVS               (ESP_ERR_WIFI_BASE + 8)   /*!< WiFi internal NVS module error */
#define ESP_ERR_WIFI_MAC               (ESP_ERR_WIFI_BASE + 9)   /*!< MAC address is invalid */
#define ESP_ERR_WIFI_SSID              (ESP_ERR_WIFI_BASE + 10)   /*!< SSID is invalid */
#define ESP_ERR_WIFI_PASSWORD          (ESP_ERR_WIFI_BASE + 11)  /*!< Password is invalid */
#define ESP_ERR_WIFI_TIMEOUT           (ESP_ERR_WIFI_BASE + 12)  /*!< Timeout error */
#define ESP_ERR_WIFI_WAKE_FAIL         (ESP_ERR_WIFI_BASE + 13)  /*!< WiFi is in sleep state(RF closed) and wakeup fail */
#define ESP_ERR_WIFI_WOULD_BLOCK       (ESP_ERR_WIFI_BASE + 14)  /*!< The caller would block */
#define ESP_ERR_WIFI_NOT_CONNECT       (ESP_ERR_WIFI_BASE + 15)  /*!< Station still in disconnect status */

#define ESP_ERR_WIFI_POST              (ESP_ERR_WIFI_BASE + 18)  /*!< Failed to post the event to WiFi task */
#define ESP_ERR_WIFI_INIT_STATE        (ESP_ERR_WIFI_BASE + 19)  /*!< Invalid WiFi state when init/deinit is called */
#define ESP_ERR_WIFI_STOP_STATE        (ESP_ERR_WIFI_BASE + 20)  /*!< Returned when WiFi is stopping */
#define ESP_ERR_WIFI_NOT_ASSOC         (ESP_ERR_WIFI_BASE + 21)  /*!< The WiFi connection is not associated */
#define ESP_ERR_WIFI_TX_DISALLOW       (ESP_ERR_WIFI_BASE + 22)  /*!< The WiFi TX is disallowed */

#define ESP_ERR_WIFI_TWT_FULL          (ESP_ERR_WIFI_BASE + 23)  /*!< no available flow id */
#define ESP_ERR_WIFI_TWT_SETUP_TIMEOUT (ESP_ERR_WIFI_BASE + 24)  /*!< Timeout of receiving twt setup response frame, timeout times can be set during twt setup */
#define ESP_ERR_WIFI_TWT_SETUP_TXFAIL  (ESP_ERR_WIFI_BASE + 25)  /*!< TWT setup frame tx failed */
#define ESP_ERR_WIFI_TWT_SETUP_REJECT  (ESP_ERR_WIFI_BASE + 26)  /*!< The twt setup request was rejected by the AP */
#define ESP_ERR_WIFI_DISCARD           (ESP_ERR_WIFI_BASE + 27)  /*!< Discard frame */
#define ESP_ERR_WIFI_ROC_IN_PROGRESS   (ESP_ERR_WIFI_BASE + 28)  /*!< ROC op is in progress */

typedef struct wifi_osi_funcs_t wifi_osi_funcs_t;

/**
 * @brief WiFi stack configuration parameters passed to esp_wifi_init call.
 */
typedef struct {
    wifi_osi_funcs_t*      osi_funcs;              /**< WiFi OS functions */
    wpa_crypto_funcs_t     wpa_crypto_funcs;       /**< WiFi station crypto functions when connect */
    int                    static_rx_buf_num;      /**< WiFi static RX buffer number */
    int                    dynamic_rx_buf_num;     /**< WiFi dynamic RX buffer number */
    int                    tx_buf_type;            /**< WiFi TX buffer type */
    int                    static_tx_buf_num;      /**< WiFi static TX buffer number */
    int                    dynamic_tx_buf_num;     /**< WiFi dynamic TX buffer number */
    int                    rx_mgmt_buf_type;       /**< WiFi RX MGMT buffer type */
    int                    rx_mgmt_buf_num;        /**< WiFi RX MGMT buffer number */
    int                    cache_tx_buf_num;       /**< WiFi TX cache buffer number */
    int                    csi_enable;             /**< WiFi channel state information enable flag */
    int                    ampdu_rx_enable;        /**< WiFi AMPDU RX feature enable flag */
    int                    ampdu_tx_enable;        /**< WiFi AMPDU TX feature enable flag */
    int                    amsdu_tx_enable;        /**< WiFi AMSDU TX feature enable flag */
    int                    nvs_enable;             /**< WiFi NVS flash enable flag */
    int                    nano_enable;            /**< Nano option for printf/scan family enable flag */
    int                    rx_ba_win;              /**< WiFi Block Ack RX window size */
    int                    wifi_task_core_id;      /**< WiFi Task Core ID */
    int                    beacon_max_len;         /**< WiFi softAP maximum length of the beacon */
    int                    mgmt_sbuf_num;          /**< WiFi management short buffer number, the minimum value is 6, the maximum value is 32 */
    uint64_t               feature_caps;           /**< Enables additional WiFi features and capabilities */
    bool                   sta_disconnected_pm;    /**< WiFi Power Management for station at disconnected status */
    int                    espnow_max_encrypt_num; /**< Maximum encrypt number of peers supported by espnow */
    int                    tx_hetb_queue_num;      /**< WiFi TX HE TB QUEUE number for STA HE TB PPDU transmission */
    bool                   dump_hesigb_enable;     /**< enable dump sigb field */
    int                    magic;                  /**< WiFi init magic number, it should be the last field */
} wifi_init_config_t;

#ifdef CONFIG_ESP_WIFI_STATIC_TX_BUFFER_NUM
#define WIFI_STATIC_TX_BUFFER_NUM CONFIG_ESP_WIFI_STATIC_TX_BUFFER_NUM
#else
#define WIFI_STATIC_TX_BUFFER_NUM 0
#endif

#if CONFIG_SPIRAM
#define WIFI_CACHE_TX_BUFFER_NUM  CONFIG_ESP_WIFI_CACHE_TX_BUFFER_NUM
#else
#define WIFI_CACHE_TX_BUFFER_NUM  0
#endif

#ifdef CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM
#define WIFI_DYNAMIC_TX_BUFFER_NUM CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM
#else
#define WIFI_DYNAMIC_TX_BUFFER_NUM 0
#endif

#ifdef CONFIG_ESP_WIFI_RX_MGMT_BUF_NUM_DEF
#define WIFI_RX_MGMT_BUF_NUM_DEF CONFIG_ESP_WIFI_RX_MGMT_BUF_NUM_DEF
#else
#define WIFI_RX_MGMT_BUF_NUM_DEF 0
#endif

#if CONFIG_ESP_WIFI_CSI_ENABLED
#define WIFI_CSI_ENABLED         1
#else
#define WIFI_CSI_ENABLED         0
#endif

#if CONFIG_ESP_WIFI_AMPDU_RX_ENABLED
#define WIFI_AMPDU_RX_ENABLED        1
#else
#define WIFI_AMPDU_RX_ENABLED        0
#endif

#if CONFIG_ESP_WIFI_AMPDU_TX_ENABLED
#define WIFI_AMPDU_TX_ENABLED        1
#else
#define WIFI_AMPDU_TX_ENABLED        0
#endif

#if CONFIG_ESP_WIFI_AMSDU_TX_ENABLED
#define WIFI_AMSDU_TX_ENABLED        1
#else
#define WIFI_AMSDU_TX_ENABLED        0
#endif

#if CONFIG_ESP_WIFI_NVS_ENABLED
#define WIFI_NVS_ENABLED          1
#else
#define WIFI_NVS_ENABLED          0
#endif

#if CONFIG_NEWLIB_NANO_FORMAT
#define WIFI_NANO_FORMAT_ENABLED  1
#else
#define WIFI_NANO_FORMAT_ENABLED  0
#endif

extern const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs;
extern wifi_osi_funcs_t g_wifi_osi_funcs;

#define WIFI_INIT_CONFIG_MAGIC    0x1F2F3F4F

#ifdef CONFIG_ESP_WIFI_AMPDU_RX_ENABLED
#define WIFI_DEFAULT_RX_BA_WIN CONFIG_ESP_WIFI_RX_BA_WIN
#else
#define WIFI_DEFAULT_RX_BA_WIN 0 /* unused if ampdu_rx_enable == false */
#endif

#if CONFIG_ESP_WIFI_TASK_PINNED_TO_CORE_1
#define WIFI_TASK_CORE_ID 1
#else
#define WIFI_TASK_CORE_ID 0
#endif

#ifdef CONFIG_ESP_WIFI_SOFTAP_BEACON_MAX_LEN
#define WIFI_SOFTAP_BEACON_MAX_LEN CONFIG_ESP_WIFI_SOFTAP_BEACON_MAX_LEN
#else
#define WIFI_SOFTAP_BEACON_MAX_LEN 752
#endif

#ifdef CONFIG_ESP_WIFI_MGMT_SBUF_NUM
#define WIFI_MGMT_SBUF_NUM CONFIG_ESP_WIFI_MGMT_SBUF_NUM
#else
#define WIFI_MGMT_SBUF_NUM 32
#endif

#if CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE
#define WIFI_STA_DISCONNECTED_PM_ENABLED true
#else
#define WIFI_STA_DISCONNECTED_PM_ENABLED false
#endif

#if CONFIG_ESP_WIFI_ENABLE_WPA3_SAE
#define WIFI_ENABLE_WPA3_SAE (1<<0)
#else
#define WIFI_ENABLE_WPA3_SAE 0
#endif

#if CONFIG_SPIRAM
#define WIFI_ENABLE_SPIRAM (1<<1)
#else
#define WIFI_ENABLE_SPIRAM 0
#endif

#if CONFIG_ESP_WIFI_FTM_INITIATOR_SUPPORT
#define WIFI_FTM_INITIATOR (1<<2)
#else
#define WIFI_FTM_INITIATOR 0
#endif

#if CONFIG_ESP_WIFI_FTM_RESPONDER_SUPPORT
#define WIFI_FTM_RESPONDER (1<<3)
#else
#define WIFI_FTM_RESPONDER 0
#endif

#if CONFIG_ESP_WIFI_ENABLE_DUMP_HESIGB && !WIFI_CSI_ENABLED
#define WIFI_DUMP_HESIGB_ENABLED  true
#else
#define WIFI_DUMP_HESIGB_ENABLED  false
#endif

#if CONFIG_ESP_WIFI_TX_HETB_QUEUE_NUM
#define WIFI_TX_HETB_QUEUE_NUM CONFIG_ESP_WIFI_TX_HETB_QUEUE_NUM
#else
#define WIFI_TX_HETB_QUEUE_NUM 1
#endif

#define CONFIG_FEATURE_WPA3_SAE_BIT     (1<<0)
#define CONFIG_FEATURE_CACHE_TX_BUF_BIT (1<<1)
#define CONFIG_FEATURE_FTM_INITIATOR_BIT (1<<2)
#define CONFIG_FEATURE_FTM_RESPONDER_BIT (1<<3)

/* Set additional WiFi features and capabilities */
#define WIFI_FEATURE_CAPS (WIFI_ENABLE_WPA3_SAE | \
                           WIFI_ENABLE_SPIRAM  | \
                           WIFI_FTM_INITIATOR | \
                           WIFI_FTM_RESPONDER)

#define WIFI_INIT_CONFIG_DEFAULT() { \
    .osi_funcs = &g_wifi_osi_funcs, \
    .wpa_crypto_funcs = g_wifi_default_wpa_crypto_funcs, \
    .static_rx_buf_num = CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM,\
    .dynamic_rx_buf_num = CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM,\
    .tx_buf_type = CONFIG_ESP_WIFI_TX_BUFFER_TYPE,\
    .static_tx_buf_num = WIFI_STATIC_TX_BUFFER_NUM,\
    .dynamic_tx_buf_num = WIFI_DYNAMIC_TX_BUFFER_NUM,\
    .rx_mgmt_buf_type = CONFIG_ESP_WIFI_DYNAMIC_RX_MGMT_BUF,\
    .rx_mgmt_buf_num = WIFI_RX_MGMT_BUF_NUM_DEF,\
    .cache_tx_buf_num = WIFI_CACHE_TX_BUFFER_NUM,\
    .csi_enable = WIFI_CSI_ENABLED,\
    .ampdu_rx_enable = WIFI_AMPDU_RX_ENABLED,\
    .ampdu_tx_enable = WIFI_AMPDU_TX_ENABLED,\
    .amsdu_tx_enable = WIFI_AMSDU_TX_ENABLED,\
    .nvs_enable = WIFI_NVS_ENABLED,\
    .nano_enable = WIFI_NANO_FORMAT_ENABLED,\
    .rx_ba_win = WIFI_DEFAULT_RX_BA_WIN,\
    .wifi_task_core_id = WIFI_TASK_CORE_ID,\
    .beacon_max_len = WIFI_SOFTAP_BEACON_MAX_LEN, \
    .mgmt_sbuf_num = WIFI_MGMT_SBUF_NUM, \
    .feature_caps = WIFI_FEATURE_CAPS, \
    .sta_disconnected_pm = WIFI_STA_DISCONNECTED_PM_ENABLED,  \
    .espnow_max_encrypt_num = CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM, \
    .tx_hetb_queue_num = WIFI_TX_HETB_QUEUE_NUM, \
    .dump_hesigb_enable = WIFI_DUMP_HESIGB_ENABLED, \
    .magic = WIFI_INIT_CONFIG_MAGIC\
}

/**
  * @brief  Initialize WiFi
  *         Allocate resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
  *         WiFi NVS structure etc. This WiFi also starts WiFi task
  *
  * @attention 1. This API must be called before all other WiFi API can be called
  * @attention 2. Always use WIFI_INIT_CONFIG_DEFAULT macro to initialize the configuration to default values, this can
  *               guarantee all the fields get correct value when more fields are added into wifi_init_config_t
  *               in future release. If you want to set your own initial values, overwrite the default values
  *               which are set by WIFI_INIT_CONFIG_DEFAULT. Please be notified that the field 'magic' of
  *               wifi_init_config_t should always be WIFI_INIT_CONFIG_MAGIC!
  *
  * @param  config pointer to WiFi initialized configuration structure; can point to a temporary variable.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_NO_MEM: out of memory
  *    - others: refer to error code esp_err.h
  */
esp_err_t esp_wifi_init(const wifi_init_config_t *config);

/**
  * @brief  Deinit WiFi
  *         Free all resource allocated in esp_wifi_init and stop WiFi task
  *
  * @attention 1. This API should be called if you want to remove WiFi driver from the system
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_deinit(void);

/**
  * @brief     Set the WiFi operating mode
  *
  *            Set the WiFi operating mode as station, soft-AP, station+soft-AP or NAN.
  *            The default mode is station mode.
  *
  * @param     mode  WiFi operating mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
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
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_mode(wifi_mode_t *mode);

/**
  * @brief  Start WiFi according to current configuration
  *         If mode is WIFI_MODE_STA, it creates station control block and starts station
  *         If mode is WIFI_MODE_AP, it creates soft-AP control block and starts soft-AP
  *         If mode is WIFI_MODE_APSTA, it creates soft-AP and station control block and starts soft-AP and station
  *         If mode is WIFI_MODE_NAN, it creates NAN control block and starts NAN
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_NO_MEM: out of memory
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_FAIL: other WiFi internal errors
  */
esp_err_t esp_wifi_start(void);

/**
  * @brief  Stop WiFi
  *         If mode is WIFI_MODE_STA, it stops station and frees station control block
  *         If mode is WIFI_MODE_AP, it stops soft-AP and frees soft-AP control block
  *         If mode is WIFI_MODE_APSTA, it stops station/soft-AP and frees station/soft-AP control block
  *         If mode is WIFI_MODE_NAN, it stops NAN and frees NAN control block
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_stop(void);

/**
 * @brief  Restore WiFi stack persistent settings to default values
 *
 * This function will reset settings made using the following APIs:
 * - esp_wifi_set_bandwidth,
 * - esp_wifi_set_protocol,
 * - esp_wifi_set_config related
 * - esp_wifi_set_mode
 *
 * @return
 *    - ESP_OK: succeed
 *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
 */
esp_err_t esp_wifi_restore(void);

/**
  * @brief     Connect WiFi station to the AP.
  *
  * @attention 1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
  * @attention 2. If station interface is connected to an AP, call esp_wifi_disconnect to disconnect.
  * @attention 3. The scanning triggered by esp_wifi_scan_start() will not be effective until connection between device and the AP is established.
  *               If device is scanning and connecting at the same time, it will abort scanning and return a warning message and error
  *               number ESP_ERR_WIFI_STATE.
  * @attention 4. This API attempts to connect to an Access Point (AP) only once. To enable reconnection in case of a connection failure, please use
  *               the 'failure_retry_cnt' feature in the 'wifi_sta_config_t'. Users are suggested to implement reconnection logic in their application
  *               for scenarios where the specified AP does not exist, or reconnection is desired after the device has received a disconnect event.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_MODE: WiFi mode error
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_SSID: SSID of AP which station connects is invalid
  */
esp_err_t esp_wifi_connect(void);

/**
  * @brief     Disconnect WiFi station from the AP.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi was not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_FAIL: other WiFi internal errors
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
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_MODE: WiFi mode is wrong
  */
esp_err_t esp_wifi_deauth_sta(uint16_t aid);

/**
  * @brief     Scan all available APs.
  *
  * @attention If this API is called, the found APs are stored in WiFi driver dynamic allocated memory. And then
  *            can be freed in esp_wifi_scan_get_ap_records(), esp_wifi_scan_get_ap_record() or esp_wifi_clear_ap_list(),
  *            so call any one to free the memory once the scan is done.
  * @attention The values of maximum active scan time and passive scan time per channel are limited to 1500 milliseconds.
  *            Values above 1500ms may cause station to disconnect from AP and are not recommended.
  *
  * @param     config  configuration settings for scanning, if set to NULL default settings will be used
  *                    of which default values are show_hidden:false, scan_type:active, scan_time.active.min:0,
  *                    scan_time.active.max:120 milliseconds, scan_time.passive:360 milliseconds
  *                    home_chan_dwell_time:30ms
  *
  * @param     block if block is true, this API will block the caller until the scan is done, otherwise
  *                         it will return immediately
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_ERR_WIFI_TIMEOUT: blocking scan is timeout
  *    - ESP_ERR_WIFI_STATE: wifi still connecting when invoke esp_wifi_scan_start
  *    - others: refer to error code in esp_err.h
  */
esp_err_t esp_wifi_scan_start(const wifi_scan_config_t *config, bool block);

/**
  * @brief     Set default parameters used for scanning by station.
  *
  * @attention The values set using this API are also used for scans used while connecting.
  *
  * @attention The values of maximum active scan time and passive scan time per channel are limited to 1500 milliseconds.
  *
  * @attention The home_chan_dwell_time needs to be a minimum of 30ms and a maximum of 150ms.
  *
  * @attention Set any of the parameters to 0 to indicate using the default parameters -
  *            scan_time.active.min : 0ms, scan_time.active.max : 120ms home_chan_dwell_time : 30ms
  *            scan_time.passive : 360ms
  *
  * @attention Default values can be retrieved using the macro WIFI_SCAN_PARAMS_DEFAULT_CONFIG()
  *
  * @attention Set the config parameter to NULL to reset previously set scan parameters to their default values.
  *
  * @param     config  default configuration settings for all scans by stations
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_FAIL: failed as station mode has not been started yet
  *    - ESP_ERR_INVALID_ARG: values provided do not satisfy the requirements
  *    - ESP_ERR_NOT_SUPPORTED: This API is not supported in AP mode yet
  *    - ESP_ERR_INVALID_STATE: a scan/connect is in progress right now, cannot change scan parameters
  *    - others: refer to error code in esp_err.h
  */
esp_err_t esp_wifi_set_scan_parameters(const wifi_scan_default_params_t *config);

/**
  * @brief     Get default parameters used for scanning by station.
  *
  * @param     config  structure variable within which scan default params will be stored
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: passed parameter does not point to a valid memory
  *    - others: refer to error code in esp_err.h
  */
esp_err_t esp_wifi_get_scan_parameters(wifi_scan_default_params_t *config);

/**
  * @brief     Stop the scan in process
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  */
esp_err_t esp_wifi_scan_stop(void);

/**
  * @brief     Get number of APs found in last scan
  *
  * @param[out] number  store number of APs found in last scan
  *
  * @attention This API can only be called when the scan is completed, otherwise it may get wrong value.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_scan_get_ap_num(uint16_t *number);

/**
  * @brief     Get AP list found in last scan.
  *
  * @attention  This API will free all memory occupied by scanned AP list.
  *
  * @param[inout]  number As input param, it stores max AP number ap_records can hold.
  *                As output param, it receives the actual AP number this API returns.
  * @param         ap_records  wifi_ap_record_t array to hold the found APs
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_NO_MEM: out of memory
  */
esp_err_t esp_wifi_scan_get_ap_records(uint16_t *number, wifi_ap_record_t *ap_records);

/**
 * @brief      Get one AP record from the scanned AP list.
 *
 * @attention  Different from esp_wifi_scan_get_ap_records(), this API only gets one AP record
 *             from the scanned AP list each time. This API will free the memory of one AP record,
 *             if the user doesn't get all records in the scannned AP list, then needs to call esp_wifi_clear_ap_list()
 *             to free the remaining memory.
 *
 * @param[out] ap_record  pointer to one AP record
 *
 * @return
 *    - ESP_OK: succeed
 *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
 *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
 *    - ESP_ERR_INVALID_ARG: invalid argument
 *    - ESP_FAIL: scan APs is NULL, means all AP records fetched or no AP found
 */
esp_err_t esp_wifi_scan_get_ap_record(wifi_ap_record_t *ap_record);

/**
  * @brief     Clear AP list found in last scan
  *
  * @attention This API will free all memory occupied by scanned AP list.
  *            When the obtained AP list fails, AP records must be cleared,otherwise it may cause memory leakage.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_MODE: WiFi mode is wrong
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_clear_ap_list(void);

/**
  * @brief     Get information of AP to which the device is associated with
  *
  * @attention When the obtained country information is empty, it means that the AP does not carry country information
  *
  * @param     ap_info  the wifi_ap_record_t to hold AP information
  *            sta can get the connected ap's phy mode info through the struct member
  *            phy_11b，phy_11g，phy_11n，phy_lr in the wifi_ap_record_t struct.
  *            For example, phy_11b = 1 imply that ap support 802.11b mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_CONN: The station interface don't initialized
  *    - ESP_ERR_WIFI_NOT_CONNECT: The station is in disconnect status
  */
esp_err_t esp_wifi_sta_get_ap_info(wifi_ap_record_t *ap_info);

/**
  * @brief     Set current WiFi power save type
  *
  * @attention Default power save type is WIFI_PS_MIN_MODEM.
  *
  * @param     type  power save type
  *
  * @return    ESP_OK: succeed
  */
esp_err_t esp_wifi_set_ps(wifi_ps_type_t type);

/**
  * @brief     Get current WiFi power save type
  *
  * @attention Default power save type is WIFI_PS_MIN_MODEM.
  *
  * @param[out]  type: store current power save type
  *
  * @return    ESP_OK: succeed
  */
esp_err_t esp_wifi_get_ps(wifi_ps_type_t *type);

/**
  * @brief     Set protocol type of specified interface
  *            The default protocol is (WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N).
  *            if CONFIG_SOC_WIFI_HE_SUPPORT and band is 2.4G, the default protocol is (WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_11AX).
  *            if CONFIG_SOC_WIFI_HE_SUPPORT and band is 5G, the default protocol is (WIFI_PROTOCOL_11A|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_11AC|WIFI_PROTOCOL_11AX).
  *
  * @attention 2.4G: Support 802.11b or 802.11bg or 802.11bgn or 802.11bgnax or LR mode
  *            5G: Support 802.11a or 802.11an or 802.11anac or 802.11anacax
  *
  * @param     ifx  interfaces
  * @param     protocol_bitmap  WiFi protocol bitmap
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
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
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - others: refer to error codes in esp_err.h
  */
esp_err_t esp_wifi_get_protocol(wifi_interface_t ifx, uint8_t *protocol_bitmap);

/**
  * @brief     Set the bandwidth of specified interface
  *
  * @attention 1. API return false if try to configure an interface that is not enabled
  * @attention 2. WIFI_BW_HT40 is supported only when the interface support 11N
  *
  * @param     ifx  interface to be configured
  * @param     bw  bandwidth
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - others: refer to error codes in esp_err.h
  */
esp_err_t esp_wifi_set_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t bw);

/**
  * @brief     Get the bandwidth of specified interface
  *
  * @attention 1. API return false if try to get a interface that is not enable
  *
  * @param     ifx interface to be configured
  * @param[out] bw  store bandwidth of interface ifx
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_bandwidth(wifi_interface_t ifx, wifi_bandwidth_t *bw);

/**
  * @brief     Set primary/secondary channel of device
  *
  * @attention 1. This API should be called after esp_wifi_start() and before esp_wifi_stop()
  * @attention 2. When device is in STA mode, this API should not be called when STA is scanning or connecting to an external AP
  * @attention 3. When device is in softAP mode, this API should not be called when softAP has connected to external STAs
  * @attention 4. When device is in STA+softAP mode, this API should not be called when in the scenarios described above
  * @attention 5. The channel info set by this API will not be stored in NVS. So If you want to remember the channel used before wifi stop,
  *               you need to call this API again after wifi start, or you can call `esp_wifi_set_config()` to store the channel info in NVS.
  *
  * @param     primary  for HT20, primary is the channel number, for HT40, primary is the primary channel
  * @param     second   for HT20, second is ignored, for HT40, second is the second channel
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  */
esp_err_t esp_wifi_set_channel(uint8_t primary, wifi_second_chan_t second);

/**
  * @brief     Get the primary/secondary channel of device
  *
  * @attention 1. API return false if try to get a interface that is not enable
  *
  * @param     primary   store current primary channel
  * @param[out]  second  store current second channel
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_channel(uint8_t *primary, wifi_second_chan_t *second);

/**
  * @brief     configure country info
  *
  * @attention 1. It is discouraged to call this API since this doesn't validate the per-country rules,
  *               it's up to the user to fill in all fields according to local regulations.
  *               Please use esp_wifi_set_country_code instead.
  * @attention 2. The default country is "01" (world safe mode) {.cc="01", .schan=1, .nchan=11, .policy=WIFI_COUNTRY_POLICY_AUTO}.
  * @attention 3. The third octet of country code string is one of the following: ' ', 'O', 'I', 'X', otherwise it is considered as ' '.
  * @attention 4. When the country policy is WIFI_COUNTRY_POLICY_AUTO, the country info of the AP to which
  *               the station is connected is used. E.g. if the configured country info is {.cc="US", .schan=1, .nchan=11}
  *               and the country info of the AP to which the station is connected is {.cc="JP", .schan=1, .nchan=14}
  *               then the country info that will be used is {.cc="JP", .schan=1, .nchan=14}. If the station disconnected
  *               from the AP the country info is set back to the country info of the station automatically,
  *               {.cc="US", .schan=1, .nchan=11} in the example.
  * @attention 5. When the country policy is WIFI_COUNTRY_POLICY_MANUAL, then the configured country info is used always.
  * @attention 6. When the country info is changed because of configuration or because the station connects to a different
  *               external AP, the country IE in probe response/beacon of the soft-AP is also changed.
  * @attention 7. The country configuration is stored into flash.
  * @attention 8. When this API is called, the PHY init data will switch to the PHY init data type corresponding to the
  *               country info.
  *
  * @param     country   the configured country info
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_set_country(const wifi_country_t *country);

/**
  * @brief     get the current country info
  *
  * @param     country  country info
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_country(wifi_country_t *country);

/**
  * @brief     Set MAC address of WiFi station, soft-AP or NAN interface.
  *
  * @attention 1. This API can only be called when the interface is disabled
  * @attention 2. Above mentioned interfaces have different MAC addresses, do not set them to be the same.
  * @attention 3. The bit 0 of the first byte of MAC address can not be 1. For example, the MAC address
  *      can set to be "1a:XX:XX:XX:XX:XX", but can not be "15:XX:XX:XX:XX:XX".
  *
  * @param     ifx  interface
  * @param     mac  the MAC address
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_WIFI_MAC: invalid mac address
  *    - ESP_ERR_WIFI_MODE: WiFi mode is wrong
  *    - others: refer to error codes in esp_err.h
  */
esp_err_t esp_wifi_set_mac(wifi_interface_t ifx, const uint8_t mac[6]);

/**
  * @brief     Get mac of specified interface
  *
  * @param      ifx  interface
  * @param[out] mac  store mac of the interface ifx
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
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
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_set_promiscuous_rx_cb(wifi_promiscuous_cb_t cb);

/**
  * @brief     Enable the promiscuous mode.
  *
  * @param     en  false - disable, true - enable
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_set_promiscuous(bool en);

/**
  * @brief     Get the promiscuous mode.
  *
  * @param[out] en  store the current status of promiscuous mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_promiscuous(bool *en);

/**
  * @brief Enable the promiscuous mode packet type filter.
  *
  * @note The default filter is to filter all packets except WIFI_PKT_MISC
  *
  * @param filter the packet type filtered in promiscuous mode.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_set_promiscuous_filter(const wifi_promiscuous_filter_t *filter);

/**
  * @brief     Get the promiscuous filter.
  *
  * @param[out] filter  store the current status of promiscuous filter
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_promiscuous_filter(wifi_promiscuous_filter_t *filter);

/**
  * @brief Enable subtype filter of the control packet in promiscuous mode.
  *
  * @note The default filter is to filter none control packet.
  *
  * @param filter the subtype of the control packet filtered in promiscuous mode.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_set_promiscuous_ctrl_filter(const wifi_promiscuous_filter_t *filter);

/**
  * @brief     Get the subtype filter of the control packet in promiscuous mode.
  *
  * @param[out] filter  store the current status of subtype filter of the control packet in promiscuous mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_promiscuous_ctrl_filter(wifi_promiscuous_filter_t *filter);

/**
  * @brief     Set the configuration of the STA, AP or NAN
  *
  * @attention 1. This API can be called only when specified interface is enabled, otherwise, API fail
  * @attention 2. For station configuration, bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.
  * @attention 3. ESP devices are limited to only one channel, so when in the soft-AP+station mode, the soft-AP will adjust its channel automatically to be the same as
  *               the channel of the station.
  * @attention 4. The configuration will be stored in NVS for station and soft-AP
  *
  * @param     interface  interface
  * @param     conf  station, soft-AP or NAN configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF: invalid interface
  *    - ESP_ERR_WIFI_MODE: invalid mode
  *    - ESP_ERR_WIFI_PASSWORD: invalid password
  *    - ESP_ERR_WIFI_NVS: WiFi internal NVS error
  *    - others: refer to the error code in esp_err.h
  */
esp_err_t esp_wifi_set_config(wifi_interface_t interface, wifi_config_t *conf);

/**
  * @brief     Get configuration of specified interface
  *
  * @param     interface  interface
  * @param[out]  conf  station or soft-AP configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF: invalid interface
  */
esp_err_t esp_wifi_get_config(wifi_interface_t interface, wifi_config_t *conf);

/**
 * @brief Forward declare wifi_sta_list_t. The definition depends on the target device
 * that implements esp_wifi
 */
typedef struct wifi_sta_list_t wifi_sta_list_t;

/**
  * @brief     Get STAs associated with soft-AP
  *
  * @attention SSC only API
  *
  * @param[out] sta  station list
  *             ap can get the connected sta's phy mode info through the struct member
  *             phy_11b，phy_11g，phy_11n，phy_lr in the wifi_sta_info_t struct.
  *             For example, phy_11b = 1 imply that sta support 802.11b mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_MODE: WiFi mode is wrong
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, the station/soft-AP control block is invalid
  */
esp_err_t esp_wifi_ap_get_sta_list(wifi_sta_list_t *sta);

/**
  * @brief     Get AID of STA connected with soft-AP
  *
  * @param     mac  STA's mac address
  * @param[out]  aid  Store the AID corresponding to STA mac
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_NOT_FOUND: Requested resource not found
  *    - ESP_ERR_WIFI_MODE: WiFi mode is wrong
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, the station/soft-AP control block is invalid
  */
esp_err_t esp_wifi_ap_get_sta_aid(const uint8_t mac[6], uint16_t *aid);

/**
  * @brief     Set the WiFi API configuration storage type
  *
  * @attention 1. The default value is WIFI_STORAGE_FLASH
  *
  * @param     storage : storage type
  *
  * @return
  *   - ESP_OK: succeed
  *   - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *   - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_set_storage(wifi_storage_t storage);

/**
  * @brief     Function signature for received Vendor-Specific Information Element callback.
  * @param     ctx Context argument, as passed to esp_wifi_set_vendor_ie_cb() when registering callback.
  * @param     type Information element type, based on frame type received.
  * @param     sa Source 802.11 address.
  * @param     vnd_ie Pointer to the vendor specific element data received.
  * @param     rssi Received signal strength indication.
  */
typedef void (*esp_vendor_ie_cb_t)(void *ctx, wifi_vendor_ie_type_t type, const uint8_t sa[6], const vendor_ie_data_t *vnd_ie, int rssi);

/**
  * @brief     Set 802.11 Vendor-Specific Information Element
  *
  * @param     enable If true, specified IE is enabled. If false, specified IE is removed.
  * @param     type Information Element type. Determines the frame type to associate with the IE.
  * @param     idx  Index to set or clear. Each IE type can be associated with up to two elements (indices 0 & 1).
  * @param     vnd_ie Pointer to vendor specific element data. First 6 bytes should be a header with fields matching vendor_ie_data_t.
  *            If enable is false, this argument is ignored and can be NULL. Data does not need to remain valid after the function returns.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init()
  *    - ESP_ERR_INVALID_ARG: Invalid argument, including if first byte of vnd_ie is not WIFI_VENDOR_IE_ELEMENT_ID (0xDD)
  *      or second byte is an invalid length.
  *    - ESP_ERR_NO_MEM: Out of memory
  */
esp_err_t esp_wifi_set_vendor_ie(bool enable, wifi_vendor_ie_type_t type, wifi_vendor_ie_id_t idx, const void *vnd_ie);

/**
  * @brief     Register Vendor-Specific Information Element monitoring callback.
  *
  * @param     cb   Callback function
  * @param     ctx  Context argument, passed to callback function.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_set_vendor_ie_cb(esp_vendor_ie_cb_t cb, void *ctx);

/**
  * @brief     Set maximum transmitting power after WiFi start.
  *
  * @attention 1. Maximum power before wifi startup is limited by PHY init data bin.
  * @attention 2. The value set by this API will be mapped to the max_tx_power of the structure wifi_country_t variable.
  * @attention 3. Mapping Table {Power, max_tx_power} = {{8,   2}, {20,  5}, {28,  7}, {34,  8}, {44, 11},
  *                                                      {52, 13}, {56, 14}, {60, 15}, {66, 16}, {72, 18}, {80, 20}}.
  * @attention 4. Param power unit is 0.25dBm, range is [8, 84] corresponding to 2dBm - 20dBm.
  * @attention 5. Relationship between set value and actual value. As follows: {set value range, actual value} = {{[8,  19],8}, {[20, 27],20}, {[28, 33],28}, {[34, 43],34}, {[44, 51],44}, {[52, 55],52}, {[56, 59],56}, {[60, 65],60}, {[66, 71],66}, {[72, 79],72}, {[80, 84],80}}.
  *
  * @param     power  Maximum WiFi transmitting power.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_INVALID_ARG: invalid argument, e.g. parameter is out of range
  */
esp_err_t esp_wifi_set_max_tx_power(int8_t power);

/**
  * @brief     Get maximum transmitting power after WiFi start
  *
  * @param     power Maximum WiFi transmitting power, unit is 0.25dBm.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_max_tx_power(int8_t *power);

/**
  * @brief     Set mask to enable or disable some WiFi events
  *
  * @attention 1. Mask can be created by logical OR of various WIFI_EVENT_MASK_ constants.
  *               Events which have corresponding bit set in the mask will not be delivered to the system event handler.
  * @attention 2. Default WiFi event mask is WIFI_EVENT_MASK_AP_PROBEREQRECVED.
  * @attention 3. There may be lots of stations sending probe request data around.
  *               Don't unmask this event unless you need to receive probe request data.
  *
  * @param     mask  WiFi event mask.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_set_event_mask(uint32_t mask);

/**
  * @brief     Get mask of WiFi events
  *
  * @param     mask  WiFi event mask.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_event_mask(uint32_t *mask);

/**
  * @brief     Send raw ieee80211 data
  *
  * @attention Currently only support for sending beacon/probe request/probe response/action and non-QoS
  *            data frame
  *
  * @param     ifx interface if the Wi-Fi mode is Station, the ifx should be WIFI_IF_STA. If the Wi-Fi
  *            mode is SoftAP, the ifx should be WIFI_IF_AP. If the Wi-Fi mode is Station+SoftAP, the
  *            ifx should be WIFI_IF_STA or WIFI_IF_AP. If the ifx is wrong, the API returns ESP_ERR_WIFI_IF.
  * @param     buffer raw ieee80211 buffer
  * @param     len the length of raw buffer, the len must be <= 1500 Bytes and >= 24 Bytes
  * @param     en_sys_seq indicate whether use the internal sequence number. If en_sys_seq is false, the
  *            sequence in raw buffer is unchanged, otherwise it will be overwritten by WiFi driver with
  *            the system sequence number.
  *            Generally, if esp_wifi_80211_tx is called before the Wi-Fi connection has been set up, both
  *            en_sys_seq==true and en_sys_seq==false are fine. However, if the API is called after the Wi-Fi
  *            connection has been set up, en_sys_seq must be true, otherwise ESP_ERR_INVALID_ARG is returned.
  *
  * @return
  *    - ESP_OK: success
  *    - ESP_ERR_WIFI_IF: Invalid interface
  *    - ESP_ERR_INVALID_ARG: Invalid parameter
  *    - ESP_ERR_WIFI_NO_MEM: out of memory
  */

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

/**
  * @brief The RX callback function of Channel State Information(CSI)  data.
  *
  *        Each time a CSI data is received, the callback function will be called.
  *
  * @param ctx context argument, passed to esp_wifi_set_csi_rx_cb() when registering callback function.
  * @param data CSI data received. The memory that it points to will be deallocated after callback function returns.
  *
  */
typedef void (* wifi_csi_cb_t)(void *ctx, wifi_csi_info_t *data);

/**
  * @brief Register the RX callback function of CSI data.
  *
  *        Each time a CSI data is received, the callback function will be called.
  *
  * @param cb  callback
  * @param ctx context argument, passed to callback function
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */

esp_err_t esp_wifi_set_csi_rx_cb(wifi_csi_cb_t cb, void *ctx);

/**
  * @brief Set CSI data configuration
  *
  * @param config configuration
  *
  * return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start or promiscuous mode is not enabled
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_set_csi_config(const wifi_csi_config_t *config);

/**
  * @brief Get CSI data configuration
  *
  * @param config configuration
  *
  * return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start or promiscuous mode is not enabled
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_csi_config(wifi_csi_config_t *config);

/**
  * @brief Enable or disable CSI
  *
  * @param en true - enable, false - disable
  *
  * return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start or promiscuous mode is not enabled
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_set_csi(bool en);

/**
  * @brief     Set antenna GPIO configuration
  *
  * @param     config  Antenna GPIO configuration.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: Invalid argument, e.g. parameter is NULL, invalid GPIO number etc
  */
esp_err_t esp_wifi_set_ant_gpio(const wifi_ant_gpio_config_t *config) __attribute__((deprecated("Please use esp_phy_set_ant_gpio instead")));

/**
  * @brief     Get current antenna GPIO configuration
  *
  * @param     config  Antenna GPIO configuration.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument, e.g. parameter is NULL
  */
esp_err_t esp_wifi_get_ant_gpio(wifi_ant_gpio_config_t *config) __attribute__((deprecated("Please use esp_phy_get_ant_gpio instead")));

/**
  * @brief     Set antenna configuration
  *
  * @param     config  Antenna configuration.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: Invalid argument, e.g. parameter is NULL, invalid antenna mode or invalid GPIO number
  */
esp_err_t esp_wifi_set_ant(const wifi_ant_config_t *config) __attribute__((deprecated("Please use esp_phy_set_ant instead")));

/**
  * @brief     Get current antenna configuration
  *
  * @param     config  Antenna configuration.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument, e.g. parameter is NULL
  */
esp_err_t esp_wifi_get_ant(wifi_ant_config_t *config) __attribute__((deprecated("Please use esp_phy_get_ant instead")));

/**
 * @brief      Get the TSF time
 *             In Station mode or SoftAP+Station mode if station is not connected or station doesn't receive at least
 *             one beacon after connected, will return 0
 *
 * @attention  Enabling power save may cause the return value inaccurate, except WiFi modem sleep
 *
 * @param      interface The interface whose tsf_time is to be retrieved.
 *
 * @return     0 or the TSF time
 */
int64_t esp_wifi_get_tsf_time(wifi_interface_t interface);

/**
  * @brief     Set the inactive time of the STA or AP
  *
  * @attention 1. For Station, If the station does not receive a beacon frame from the connected SoftAP during the inactive time,
  *               disconnect from SoftAP. Default 6s.
  * @attention 2. For SoftAP, If the softAP doesn't receive any data from the connected STA during inactive time,
  *               the softAP will force deauth the STA. Default is 300s.
  * @attention 3. The inactive time configuration is not stored into flash
  *
  * @param     ifx  interface to be configured.
  * @param     sec  Inactive time. Unit seconds.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_INVALID_ARG: invalid argument, For Station, if sec is less than 3. For SoftAP, if sec is less than 10.
  */
esp_err_t esp_wifi_set_inactive_time(wifi_interface_t ifx, uint16_t sec);

/**
  * @brief     Get inactive time of specified interface
  *
  * @param     ifx  Interface to be configured.
  * @param     sec  Inactive time. Unit seconds.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_inactive_time(wifi_interface_t ifx, uint16_t *sec);

/**
  * @brief     Dump WiFi statistics
  *
  * @param     modules statistic modules to be dumped
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_statis_dump(uint32_t modules);

/**
  * @brief      Set RSSI threshold, if average rssi gets lower than threshold, WiFi task will post event WIFI_EVENT_STA_BSS_RSSI_LOW.
  *
  * @attention  If the user wants to receive another WIFI_EVENT_STA_BSS_RSSI_LOW event after receiving one, this API needs to be
  *             called again with an updated/same RSSI threshold.
  *
  * @param      rssi threshold value in dbm between -100 to 10
  *             Note that in some rare cases where signal strength is very strong, rssi values can be slightly positive.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_set_rssi_threshold(int32_t rssi);

/**
  * @brief      Start an FTM Initiator session by sending FTM request
  *             If successful, event WIFI_EVENT_FTM_REPORT is generated with the result of the FTM procedure
  *
  * @attention  1. Use this API only in Station mode.
  * @attention  2. If FTM is initiated on a different channel than Station is connected in or internal SoftAP is started in,
  *                FTM defaults to a single burst in ASAP mode.
  *
  * @param      cfg  FTM Initiator session configuration
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_ftm_initiate_session(wifi_ftm_initiator_cfg_t *cfg);

/**
  * @brief      End the ongoing FTM Initiator session
  *
  * @attention  This API works only on FTM Initiator
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_ftm_end_session(void);

/**
  * @brief      Set offset in cm for FTM Responder. An equivalent offset is calculated in picoseconds
  *             and added in TOD of FTM Measurement frame (T1).
  *
  * @attention  Use this API only in AP mode before performing FTM as responder
  *
  * @param      offset_cm  T1 Offset to be added in centimeters
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_ftm_resp_set_offset(int16_t offset_cm);

/**
  * @brief      Get FTM measurements report copied into a user provided buffer.
  *
  * @attention  1. To get the FTM report, user first needs to allocate a buffer of size
  *                (sizeof(wifi_ftm_report_entry_t) * num_entries) where the API will fill up to num_entries
  *                valid FTM measurements in the buffer. Total number of entries can be found in the event
  *                WIFI_EVENT_FTM_REPORT as ftm_report_num_entries
  * @attention  2. The internal FTM report is freed upon use of this API which means the API can only be used
  *                once after every FTM session initiated
  * @attention  3. Passing the buffer as NULL merely frees the FTM report
  *
  * @param      report  Pointer to the buffer for receiving the FTM report
  * @param      num_entries Number of FTM report entries to be filled in the report
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_ftm_get_report(wifi_ftm_report_entry_t *report, uint8_t num_entries);

/**
  * @brief      Enable or disable 11b rate of specified interface
  *
  * @attention  1. This API should be called after esp_wifi_init() and before esp_wifi_start().
  * @attention  2. Only when really need to disable 11b rate call this API otherwise don't call this.
  *
  * @param      ifx  Interface to be configured.
  * @param      disable true means disable 11b rate while false means enable 11b rate.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_config_11b_rate(wifi_interface_t ifx, bool disable);

#define ESP_WIFI_CONNECTIONLESS_INTERVAL_DEFAULT_MODE 0
/**
  * @brief      Set wake interval for connectionless modules to wake up periodically.
  *
  * @attention 1. Only one wake interval for all connectionless modules.
  * @attention 2. This configuration could work at connected status.
  *               When ESP_WIFI_STA_DISCONNECTED_PM_ENABLE is enabled, this configuration could work at disconnected status.
  * @attention 3. Event WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START would be posted each time wake interval starts.
  * @attention 4. Recommend to configure interval in multiples of hundred. (e.g. 100ms)
  * @attention 5. Recommend to configure interval to ESP_WIFI_CONNECTIONLESS_INTERVAL_DEFAULT_MODE to get stable performance at coexistence mode.
  *
  * @param      wake_interval  Milliseconds after would the chip wake up, from 1 to 65535.
  */
esp_err_t esp_wifi_connectionless_module_set_wake_interval(uint16_t wake_interval);

/**
  * @brief      Request extra reference of Wi-Fi radio.
  *             Wi-Fi keep active state(RF opened) to be able to receive packets.
  *
  * @attention  Please pair the use of `esp_wifi_force_wakeup_acquire` with `esp_wifi_force_wakeup_release`.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  */
esp_err_t esp_wifi_force_wakeup_acquire(void);

/**
  * @brief      Release extra reference of Wi-Fi radio.
  *             Wi-Fi go to sleep state(RF closed) if no more use of radio.
  *
  * @attention  Please pair the use of `esp_wifi_force_wakeup_acquire` with `esp_wifi_force_wakeup_release`.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  */
esp_err_t esp_wifi_force_wakeup_release(void);

/**
  * @brief     configure country
  *
  * @attention 1. When ieee80211d_enabled, the country info of the AP to which
  *               the station is connected is used. E.g. if the configured country is US
  *               and the country info of the AP to which the station is connected is JP
  *               then the country info that will be used is JP. If the station disconnected
  *               from the AP the country info is set back to the country info of the station automatically,
  *               US in the example.
  * @attention 2. When ieee80211d_enabled is disabled, then the configured country info is used always.
  * @attention 3. When the country info is changed because of configuration or because the station connects to a different
  *               external AP, the country IE in probe response/beacon of the soft-AP is also changed.
  * @attention 4. The country configuration is stored into flash.
  * @attention 5. When this API is called, the PHY init data will switch to the PHY init data type corresponding to the
  *               country info.
  * @attention 6. Supported country codes are "01"(world safe mode) "AT","AU","BE","BG","BR",
  *               "CA","CH","CN","CY","CZ","DE","DK","EE","ES","FI","FR","GB","GR","HK","HR","HU",
  *               "IE","IN","IS","IT","JP","KR","LI","LT","LU","LV","MT","MX","NL","NO","NZ","PL","PT",
  *               "RO","SE","SI","SK","TW","US"
  *
  * @attention 7. When country code "01" (world safe mode) is set, SoftAP mode won't contain country IE.
  * @attention 8. The default country is "01" (world safe mode) and ieee80211d_enabled is TRUE.
  * @attention 9. The third octet of country code string is one of the following: ' ', 'O', 'I', 'X', otherwise it is considered as ' '.
  *
  * @param     country   the configured country ISO code
  * @param     ieee80211d_enabled   802.11d is enabled or not
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_set_country_code(const char *country, bool ieee80211d_enabled);

/**
  * @brief     get the current country code
  *
  * @param     country  country code
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_country_code(char *country);

/**
  * @brief      Config 80211 tx rate of specified interface
  *
  * @attention  1. This API should be called after esp_wifi_init() and before esp_wifi_start().
  *
  * @param      ifx  Interface to be configured.
  * @param      rate Phy rate to be configured.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_config_80211_tx_rate(wifi_interface_t ifx, wifi_phy_rate_t rate);

/**
  * @brief      Disable PMF configuration for specified interface
  *
  * @attention  This API should be called after esp_wifi_set_config() and before esp_wifi_start().
  *
  * @param      ifx  Interface to be configured.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_disable_pmf_config(wifi_interface_t ifx);

/**
  * @brief     Get the Association id assigned to STA by AP
  *
  * @param[out] aid  store the aid
  *
  * @attention aid = 0 if station is not connected to AP.
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_sta_get_aid(uint16_t *aid);

/**
  * @brief     Get the negotiated phymode after connection.
  *
  * @param[out] phymode  store the negotiated phymode.
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_sta_get_negotiated_phymode(wifi_phy_mode_t *phymode);

/**
  * @brief      Config dynamic carrier sense
  *
  * @attention  This API should be called after esp_wifi_start().
  *
  * @param      enabled Dynamic carrier sense is enabled or not.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_set_dynamic_cs(bool enabled);

/**
  * @brief      Get the rssi information of AP to which the device is associated with
  *
  * @attention 1. This API should be called after station connected to AP.
  * @attention 2. Use this API only in WIFI_MODE_STA or WIFI_MODE_APSTA mode.
  *
  * @param      rssi store the rssi info received from last beacon.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_FAIL: failed
  */
esp_err_t esp_wifi_sta_get_rssi(int *rssi);

#if SOC_WIFI_HE_SUPPORT_5G
/**
  * @brief     Set wifi band.
  *
  * @param[in]    band wifi band 2.4G / 5G / 2.4G + 5G
  *
    * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_set_band(wifi_band_t band);

/**
  * @brief     Get wifi band.
  *
  * @param[in]    band store band of wifi
  *
    * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_get_band(wifi_band_t* band);
#endif /* SOC_WIFI_HE_SUPPORT_5G */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
