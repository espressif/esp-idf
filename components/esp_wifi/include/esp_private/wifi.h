/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * All the APIs declared here are internal only APIs, it can only be used by
 * espressif internal modules, such as SSC, LWIP, esp-netif etc, espressif
 * customers are not recommended to use them.
 *
 * If someone really want to use specified APIs declared in here, please contact
 * espressif AE/developer to make sure you know the limitations or risk of
 * the API, otherwise you may get unexpected behavior!!!
 *
 */

#ifndef __ESP_WIFI_INTERNAL_H__
#define __ESP_WIFI_INTERNAL_H__

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sys/queue.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_smartconfig.h"
#include "wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    QueueHandle_t handle; /**< FreeRTOS queue handler */
    void *storage;        /**< storage for FreeRTOS queue */
} wifi_static_queue_t;

/**
  * @brief WiFi log level
  *
  */
typedef enum {
    WIFI_LOG_NONE = 0,
    WIFI_LOG_ERROR,       /*enabled by default*/
    WIFI_LOG_WARNING,     /*enabled by default*/
    WIFI_LOG_INFO,        /*enabled by default*/
    WIFI_LOG_DEBUG,       /*can be set in menuconfig*/
    WIFI_LOG_VERBOSE,     /*can be set in menuconfig*/
} wifi_log_level_t;

/**
  * @brief WiFi log module definition
  *
  */
typedef enum {
    WIFI_LOG_MODULE_ALL  = 0, /*all log modules */
    WIFI_LOG_MODULE_WIFI, /*logs related to WiFi*/
    WIFI_LOG_MODULE_COEX, /*logs related to WiFi and BT(or BLE) coexist*/
    WIFI_LOG_MODULE_MESH, /*logs related to Mesh*/
} wifi_log_module_t;

/**
  * @brief WiFi log submodule definition
  *
  */
#define WIFI_LOG_SUBMODULE_ALL   (0)    /*all log submodules*/
#define WIFI_LOG_SUBMODULE_INIT  (1)    /*logs related to initialization*/
#define WIFI_LOG_SUBMODULE_IOCTL (1<<1) /*logs related to API calling*/
#define WIFI_LOG_SUBMODULE_CONN  (1<<2) /*logs related to connecting*/
#define WIFI_LOG_SUBMODULE_SCAN  (1<<3) /*logs related to scanning*/

/**
 * @brief Initialize Wi-Fi Driver
 *     Alloc resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
 *     WiFi NVS structure among others.
 *
 * For the most part, you need not call this function directly. It gets called
 * from esp_wifi_init().
 *
 * This function may be called, if you only need to initialize the Wi-Fi driver
 * without having to use the network stack on top.
 *
 * @param  config provide WiFi init configuration
 *
 * @return
 *    - ESP_OK: succeed
 *    - ESP_ERR_NO_MEM: out of memory
 *    - others: refer to error code esp_err.h
 */
esp_err_t esp_wifi_init_internal(const wifi_init_config_t *config);

/**
 * @brief Deinitialize Wi-Fi Driver
 *     Free resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
 *     WiFi NVS structure among others.
 *
 * For the most part, you need not call this function directly. It gets called
 * from esp_wifi_deinit().
 *
 * This function may be called, if you call esp_wifi_init_internal to initialize
 * WiFi driver.
 *
 * @return
 *    - ESP_OK: succeed
 *    - others: refer to error code esp_err.h
 */
esp_err_t esp_wifi_deinit_internal(void);

/**
  * @brief  free the rx buffer which allocated by wifi driver
  *
  * @param  void* buffer: rx buffer pointer
  */
void esp_wifi_internal_free_rx_buffer(void* buffer);

/**
  * @brief  transmit the buffer via wifi driver
  *
  * This API makes a copy of the input buffer and then forwards the buffer
  * copy to WiFi driver.
  *
  * @param  wifi_interface_t wifi_if : wifi interface id
  * @param  void *buffer : the buffer to be transmit
  * @param  uint16_t len : the length of buffer
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - ESP_ERR_NO_MEM: out of memory
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF : WiFi interface is invalid
  *    - ESP_ERR_WIFI_CONN : WiFi interface is not created, e.g. send the data to STA while WiFi mode is AP mode
  *    - ESP_ERR_WIFI_NOT_STARTED : WiFi is not started
  *    - ESP_ERR_WIFI_STATE : WiFi internal state is not ready, e.g. WiFi is not started
  *    - ESP_ERR_WIFI_NOT_ASSOC : WiFi is not associated
  *    - ESP_ERR_WIFI_TX_DISALLOW : WiFi TX is disallowed, e.g. WiFi hasn't pass the authentication
  *    - ESP_ERR_WIFI_POST : caller fails to post event to WiFi task
  */
int esp_wifi_internal_tx(wifi_interface_t wifi_if, void *buffer, uint16_t len);

/**
  * @brief     The net stack buffer reference counter callback function
  *
  */
typedef void (*wifi_netstack_buf_ref_cb_t)(void *netstack_buf);

/**
  * @brief     The net stack buffer free callback function
  *
  */
typedef void (*wifi_netstack_buf_free_cb_t)(void *netstack_buf);

/**
  * @brief  transmit the buffer by reference via wifi driver
  *
  * This API firstly increases the reference counter of the input buffer and
  * then forwards the buffer to WiFi driver. The WiFi driver will free the buffer
  * after processing it. Use esp_wifi_internal_tx() if the uplayer buffer doesn't
  * supports reference counter.
  *
  * @param  wifi_if : wifi interface id
  * @param  buffer : the buffer to be transmit
  * @param  len : the length of buffer
  * @param  netstack_buf : the netstack buffer related to buffer
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - ESP_ERR_NO_MEM: out of memory
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF : WiFi interface is invalid
  *    - ESP_ERR_WIFI_CONN : WiFi interface is not created, e.g. send the data to STA while WiFi mode is AP mode
  *    - ESP_ERR_WIFI_NOT_STARTED : WiFi is not started
  *    - ESP_ERR_WIFI_STATE : WiFi internal state is not ready, e.g. WiFi is not started
  *    - ESP_ERR_WIFI_NOT_ASSOC : WiFi is not associated
  *    - ESP_ERR_WIFI_TX_DISALLOW : WiFi TX is disallowed, e.g. WiFi hasn't pass the authentication
  *    - ESP_ERR_WIFI_POST : caller fails to post event to WiFi task
  */
esp_err_t esp_wifi_internal_tx_by_ref(wifi_interface_t ifx, void *buffer, size_t len, void *netstack_buf);

/**
  * @brief     Initialize WAPI function when wpa_supplicant initialize.
  *
  * This API is privately used, be careful not open to external applicantion.
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_WAPI_INTERNAL : Internal error
  */
esp_err_t esp_wifi_internal_wapi_init(void);

/**
  * @brief     De-initialize WAPI function when wpa_supplicant de-initialize.
  *
  * This API is privately used, be careful not open to external applicantion.
  *
  * @return
  *          - ESP_OK : succeed
  */
esp_err_t esp_wifi_internal_wapi_deinit(void);

/**
  * @brief  register the net stack buffer reference increasing and free callback
  *
  * @param  ref : net stack buffer reference callback
  * @param  free: net stack buffer free callback
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - others  : failed to register the callback
  */
esp_err_t esp_wifi_internal_reg_netstack_buf_cb(wifi_netstack_buf_ref_cb_t ref, wifi_netstack_buf_free_cb_t free);

/**
  * @brief     The WiFi RX callback function
  *
  *            Each time the WiFi need to forward the packets to high layer, the callback function will be called
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
  * @return
  *     - ESP_OK : succeed
  *     - others : fail
  */
esp_err_t esp_wifi_internal_reg_rxcb(wifi_interface_t ifx, wifi_rxcb_t fn);

/**
  * @brief     Notify WIFI driver that the station got ip successfully
  *
  * @return
  *     - ESP_OK : succeed
  *     - others : fail
  */
esp_err_t esp_wifi_internal_set_sta_ip(void);

/**
  * @brief  enable or disable transmitting WiFi MAC frame with fixed rate
  *
  * @attention 1. If fixed rate is enabled, both management and data frame are transmitted with fixed rate
  * @attention 2. Make sure that the receiver is able to receive the frame with the fixed rate if you want the frame to be received
  * @attention 3. Not support to set fix rate for espnow and 80211_tx
  *
  * @param  ifx : wifi interface
  * @param  en : false - disable, true - enable
  * @param  rate : PHY rate
  *
  * @return
  *    - ERR_OK  : succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_ERR_WIFI_IF : invalid WiFi interface
  *    - ESP_ERR_INVALID_ARG : invalid rate
  *    - ESP_ERR_NOT_SUPPORTED : do not support to set fixed rate if TX AMPDU is enabled
  */
esp_err_t esp_wifi_internal_set_fix_rate(wifi_interface_t ifx, bool en, wifi_phy_rate_t rate);

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
esp_err_t esp_smartconfig_internal_start(const smartconfig_start_config_t *config);

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
esp_err_t esp_smartconfig_internal_stop(void);

/**
  * @brief     Check the MD5 values of the OS adapter header files in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_osi_funcs_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the crypto types header files in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_crypto_funcs_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi_types.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_wifi_type_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi_he_types.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_wifi_he_type_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_esp_wifi_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi_he.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_esp_wifi_he_md5_check(const char *md5);

/**
  * @brief     Allocate a chunk of memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_malloc(size_t size);

/**
  * @brief     Reallocate a chunk of memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     void * ptr  : Pointer to previously allocated memory, or NULL for a new allocation.
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_realloc(void *ptr, size_t size);

/**
  * @brief     Callocate memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     size_t n    : Number of continuing chunks of memory to allocate
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_calloc(size_t n, size_t size);

/**
  * @brief     Update WiFi MAC time
  *
  * @param     uint32_t time_delta : time duration since the WiFi/BT common clock is disabled
  *
  * @return    Always returns ESP_OK
  */
typedef esp_err_t (* wifi_mac_time_update_cb_t)(uint32_t time_delta);

/**
  * @brief     Update WiFi MAC time
  *
  * @param     uint32_t time_delta : time duration since the WiFi/BT common clock is disabled
  *
  * @return    Always returns ESP_OK
  */
esp_err_t esp_wifi_internal_update_mac_time(uint32_t time_delta);

/**
  * @brief     Set current WiFi log level
  *
  * @param     level   Log level.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_FAIL: level is invalid
  */
esp_err_t esp_wifi_internal_set_log_level(wifi_log_level_t level);

/**
  * @brief     Set current log module and submodule
  *
  * @param     module      Log module
  * @param     submodule   Log submodule
  * @param     enable      enable or disable
  *            If module == 0 && enable == 0, all log modules are disabled.
  *            If module == 0 && enable == 1, all log modules are enabled.
  *            If submodule == 0 && enable == 0, all log submodules are disabled.
  *            If submodule == 0 && enable == 1, all log submodules are enabled.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_internal_set_log_mod(wifi_log_module_t module, uint32_t submodule, bool enable);

/**
  * @brief     Get current WiFi log info
  *
  * @param     log_level  the return log level.
  * @param     log_mod    the return log module and submodule
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_internal_get_log(wifi_log_level_t *log_level, uint32_t *log_mod);

/**
  * @brief     A general API to set/get WiFi internal configuration, it's for debug only
  *
  * @param     cmd : ioctl command type
  * @param     cfg : configuration for the command
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_internal_ioctl(int cmd, wifi_ioctl_config_t *cfg);

/**
  * @brief     Get the user-configured channel info
  *
  * @param     ifx : WiFi interface
  * @param     primary : store the configured primary channel
  * @param     second : store the configured second channel
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_internal_get_config_channel(wifi_interface_t ifx, uint8_t *primary, uint8_t *second);

/**
  * @brief     Get the negotiated channel info after WiFi connection established
  *
  * @param     ifx : WiFi interface
  * @param     aid : the connection number when a STA connects to the softAP
  * @param     primary : store the negotiated primary channel
  * @param     second : store the negotiated second channel
  * @attention the aid param is only works when the device in softAP/softAP+STA mode
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_internal_get_negotiated_channel(wifi_interface_t ifx, uint8_t aid, uint8_t *primary, uint8_t *second);

/**
  * @brief     Get the negotiated bandwidth info after WiFi connection established
  *
  * @param     ifx : WiFi interface
  * @param     bw : store the negotiated bandwidth
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_internal_get_negotiated_bandwidth(wifi_interface_t ifx, uint8_t aid, uint8_t *bw);

#if SOC_WIFI_HW_TSF
/**
  * @brief     Check if WiFi TSF is active
  *
  * @return
  *    - true: Active
  *    - false: Not active
  */
bool esp_wifi_internal_is_tsf_active(void);

/**
  * @brief     Update WIFI light sleep wake ahead time
  *
  */
void esp_wifi_internal_update_light_sleep_wake_ahead_time(uint32_t);

/**
  * @brief     Update WiFi TSF tick interval
  *
  * @return
  *    - true: Active
  *    - false: Not active
  */
esp_err_t esp_wifi_update_tsf_tick_interval(void);
#endif

/**
 * @brief Wifi power domain power on
 */
void esp_wifi_power_domain_on(void);

/**
 * @brief Wifi power domain power off
 */
void esp_wifi_power_domain_off(void);

#if (CONFIG_FREERTOS_USE_TICKLESS_IDLE && SOC_PM_MODEM_RETENTION_BY_REGDMA)
/**
  * @brief     Get wifi mac sleep retention hardware context configuration and size
  *
  * @param     config_size: the wifi mac hardware context configuration size
  *
  * @return    A pointer that point to wifi mac sleep renteiton hardware context configuration table
  */
void * esp_wifi_internal_mac_retention_context_get(int *config_size);
#endif

#if CONFIG_MAC_BB_PD
/**
  * @brief     Enable or disable powering down MAC and baseband when Wi-Fi is sleeping.
  *
  * @param     enable : enable or disable
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_internal_set_mac_sleep(bool enable);

/**
 * @brief mac bb sleep.
 */
void pm_mac_sleep(void);

/**
 * @brief mac bb wakeup.
 */
void pm_mac_wakeup(void);
#endif

/**
  * @brief    TxDone callback function type. Should be registered using esp_wifi_set_tx_done_cb()
  *
  * @param    ifidx The interface id that the tx callback has been triggered from
  * @param    data Pointer to the data transmitted
  * @param    data_len Length of the data transmitted
  * @param    txStatus True:if the data was transmitted successfully False: if data transmission failed
  */
typedef void (* wifi_tx_done_cb_t)(uint8_t ifidx, uint8_t *data, uint16_t *data_len, bool txStatus);

/**
  * @brief    Register the txDone callback function of type wifi_tx_done_cb_t
  *
  * @param    cb The callback function
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  */
esp_err_t esp_wifi_set_tx_done_cb(wifi_tx_done_cb_t cb);

/**
 * @brief     Set device spp amsdu attributes
 *
 * @param     ifx: WiFi interface
 * @param     spp_cap: spp amsdu capable
 * @param     spp_req: spp amsdu require
 *
 * @return
 *     - ESP_OK: succeed
 *     - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
 *     - ESP_ERR_WIFI_IF : invalid WiFi interface
 */
esp_err_t esp_wifi_internal_set_spp_amsdu(wifi_interface_t ifidx, bool spp_cap, bool spp_req);

/**
 * @brief   Update WIFI light sleep default parameters
 *
 * @param   min_freq_mhz: minimum frequency of DFS
 * @param   max_freq_mhz: maximum frequency of DFS
 */
void esp_wifi_internal_update_light_sleep_default_params(int min_freq_mhz, int max_freq_mhz);

/**
 * @brief   Set the min active time for wifi to enter the sleep state when light sleep
 *
 * @param   min_active_time: minimum timeout time  for waiting to receive
 *                      data, when no data is received during the timeout period,
 *                      the wifi enters the sleep process.
 */
void esp_wifi_set_sleep_min_active_time(uint32_t min_active_time);

/**
 * @brief   Set wifi keep alive time
 *
 * @param   keep_alive_time: keep alive time
 */
void esp_wifi_set_keep_alive_time(uint32_t keep_alive_time);

/**
  * @brief      Set the min broadcast data wait time for wifi to enter the sleep state
  *
  * @attention  Default sleep wait broadcast data time is 15000, Uint µs.
  *
  * @param      time: When the station knows through the beacon that the AP
  *                   will send broadcast packet, it will wait for a minimum of
  *                   wait_broadcast_data_time before entering the sleep process.
  */
void esp_wifi_set_sleep_wait_broadcast_data_time(uint32_t time);

/**
 * @brief   Configure wifi beacon montior default parameters
 *
 * @param   config: the configuration parameters for wifi beacon monitor
 */
void esp_wifi_beacon_monitor_configure(wifi_beacon_monitor_config_t *config);

/**
 * @brief   Set modem state mode to require WiFi to enable or disable Advanced DTIM sleep function
 *
 * @param   require_modem_state: true for require WiFi to enable Advanced DTIM sleep function,
 *                              false for require WiFi to disable Advanced DTIM sleep function.
 * @return
 *    - ESP_OK: succeed
 */
void esp_wifi_internal_modem_state_configure(bool require_modem_state);

/**
 * @brief   Set light sleep mode to require WiFi to enable or disable Advanced DTIM sleep function
 *
 * @param   light_sleep_enable: true for light sleep mode is enabled, false for light sleep mode is disabled.
 * @return
 *    - ESP_OK: succeed
 */
void esp_wifi_internal_light_sleep_configure(bool light_sleep_enable);

/**
  * @brief      Start Publishing a service in the NAN cluster
  *
  * @attention  This API should be called after esp_wifi_start() in NAN Mode.
  *
  * @param      publish_cfg  Configuration parameters for publishing a service.
  * @param      id  Identifier for the Publish service.
  * @param      cancel  Cancel the service identified by the id.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_publish_service(const wifi_nan_publish_cfg_t *publish_cfg,
                                           uint8_t *id, bool cancel);

/**
  * @brief      Subscribe for a service within the NAN cluster
  *
  * @attention  This API should be called after esp_wifi_start() in NAN Mode.
  *
  * @param      subscribe_cfg  Configuration parameters for subscribing for a service.
  * @param      id  Identifier for the Subscribe service.
  * @param      cancel  Cancel the service identified by the id.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_subscribe_service(const wifi_nan_subscribe_cfg_t *subscribe_cfg,
                                             uint8_t *id, bool cancel);

/**
  * @brief      Send Follow-up to the Publisher with matching service
  *
  * @attention  This API should be called after WIFI_EVENT_NAN_SVC_MATCH event is received.
  *
  * @param      fup_params  Configuration parameters for sending a Follow-up to the Peer.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_send_followup(const wifi_nan_followup_params_t *fup_params);

/**
  * @brief      Send Datapath Request to the Publisher with matching service
  *
  * @attention  This API should be called after WIFI_EVENT_NAN_SVC_MATCH event is received.
  *
  * @param      req  NAN Datapath Request parameters.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_datapath_req(wifi_nan_datapath_req_t *req, uint8_t *ndp_id);

/**
  * @brief      Send Datapath Response to accept or reject the received request
  *
  * @attention  This API should be called on the Publisher after receiving WIFI_EVENT_NDP_INDICATION event.
  *
  * @param      resp  NAN Datapath Response parameters.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_datapath_resp(wifi_nan_datapath_resp_t *resp);

/**
  * @brief      End NAN Datapath that is active
  *
  * @attention  This API should be called after receiving WIFI_EVENT_NDP_CONFIRM event.
  *
  * @param      req  NAN Datapath end request parameters.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_datapath_end(wifi_nan_datapath_end_req_t *req);

/**
  * @brief     Connect WiFi station to the AP.
  *
  * @attention 1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_MODE: WiFi mode error
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_SSID: SSID of AP which station connects is invalid
  */
esp_err_t esp_wifi_connect_internal(void);

/**
  * @brief     Disconnect WiFi station from the AP.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi was not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_FAIL: other WiFi internal errors
  *
  */
esp_err_t esp_wifi_disconnect_internal(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
