/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_NOW_H__
#define __ESP_NOW_H__

#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WiFi_APIs WiFi Related APIs
  * @brief WiFi APIs
  */

/** @addtogroup WiFi_APIs
  * @{
  */

/** \defgroup ESPNOW_APIs  ESPNOW APIs
  * @brief ESP32 ESPNOW APIs
  *
  */

/** @addtogroup ESPNOW_APIs
  * @{
  */

#define ESP_ERR_ESPNOW_BASE         (ESP_ERR_WIFI_BASE + 100) /*!< ESPNOW error number base. */
#define ESP_ERR_ESPNOW_NOT_INIT     (ESP_ERR_ESPNOW_BASE + 1) /*!< ESPNOW is not initialized. */
#define ESP_ERR_ESPNOW_ARG          (ESP_ERR_ESPNOW_BASE + 2) /*!< Invalid argument */
#define ESP_ERR_ESPNOW_NO_MEM       (ESP_ERR_ESPNOW_BASE + 3) /*!< Out of memory */
#define ESP_ERR_ESPNOW_FULL         (ESP_ERR_ESPNOW_BASE + 4) /*!< ESPNOW peer list is full */
#define ESP_ERR_ESPNOW_NOT_FOUND    (ESP_ERR_ESPNOW_BASE + 5) /*!< ESPNOW peer is not found */
#define ESP_ERR_ESPNOW_INTERNAL     (ESP_ERR_ESPNOW_BASE + 6) /*!< Internal error */
#define ESP_ERR_ESPNOW_EXIST        (ESP_ERR_ESPNOW_BASE + 7) /*!< ESPNOW peer has existed */
#define ESP_ERR_ESPNOW_IF           (ESP_ERR_ESPNOW_BASE + 8) /*!< Interface error */

#define ESP_NOW_ETH_ALEN             6         /*!< Length of ESPNOW peer MAC address */
#define ESP_NOW_KEY_LEN              16        /*!< Length of ESPNOW peer local master key */

#define ESP_NOW_MAX_TOTAL_PEER_NUM   20        /*!< Maximum number of ESPNOW total peers */
#define ESP_NOW_MAX_ENCRYPT_PEER_NUM 6         /*!< Maximum number of ESPNOW encrypted peers */

#define ESP_NOW_MAX_DATA_LEN         250       /*!< Maximum length of ESPNOW data which is sent very time */

/**
 * @brief Status of sending ESPNOW data .
 */
typedef enum {
    ESP_NOW_SEND_SUCCESS = 0,       /**< Send ESPNOW data successfully */
    ESP_NOW_SEND_FAIL,              /**< Send ESPNOW data fail */
} esp_now_send_status_t;

/**
 * @brief ESPNOW peer information parameters.
 */
typedef struct esp_now_peer_info {
    uint8_t peer_addr[ESP_NOW_ETH_ALEN];    /**< ESPNOW peer MAC address that is also the MAC address of station or softap */
    uint8_t lmk[ESP_NOW_KEY_LEN];           /**< ESPNOW peer local master key that is used to encrypt data */
    uint8_t channel;                        /**< Wi-Fi channel that peer uses to send/receive ESPNOW data. If the value is 0,
                                                 use the current channel which station or softap is on. Otherwise, it must be
                                                 set as the channel that station or softap is on. */
    wifi_interface_t ifidx;                 /**< Wi-Fi interface that peer uses to send/receive ESPNOW data */
    bool encrypt;                           /**< ESPNOW data that this peer sends/receives is encrypted or not */
    void *priv;                             /**< ESPNOW peer private data */
} esp_now_peer_info_t;

/**
 * @brief Number of ESPNOW peers which exist currently.
 */
typedef struct esp_now_peer_num {
    int total_num;                           /**< Total number of ESPNOW peers, maximum value is ESP_NOW_MAX_TOTAL_PEER_NUM */
    int encrypt_num;                         /**< Number of encrypted ESPNOW peers, maximum value is ESP_NOW_MAX_ENCRYPT_PEER_NUM */
} esp_now_peer_num_t;

/**
  * @brief     Callback function of receiving ESPNOW data
  * @param     mac_addr peer MAC address
  * @param     data received data
  * @param     data_len length of received data
  */
typedef void (*esp_now_recv_cb_t)(const uint8_t *mac_addr, const uint8_t *data, int data_len);

/**
  * @brief     Callback function of sending ESPNOW data
  * @param     mac_addr peer MAC address
  * @param     status status of sending ESPNOW data (succeed or fail)
  */
typedef void (*esp_now_send_cb_t)(const uint8_t *mac_addr, esp_now_send_status_t status);

/**
  * @brief     Initialize ESPNOW function
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_INTERNAL : Internal error
  */
esp_err_t esp_now_init(void);

/**
  * @brief     De-initialize ESPNOW function
  *
  * @return
  *          - ESP_OK : succeed
  */
esp_err_t esp_now_deinit(void);

/**
  * @brief     Get the version of ESPNOW
  *
  * @param     version  ESPNOW version
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  */
esp_err_t esp_now_get_version(uint32_t *version);

/**
  * @brief     Register callback function of receiving ESPNOW data
  *
  * @param     cb  callback function of receiving ESPNOW data
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_INTERNAL : internal error
  */
esp_err_t esp_now_register_recv_cb(esp_now_recv_cb_t cb);

/**
  * @brief     Unregister callback function of receiving ESPNOW data
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  */
esp_err_t esp_now_unregister_recv_cb(void);

/**
  * @brief     Register callback function of sending ESPNOW data
  *
  * @param     cb  callback function of sending ESPNOW data
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_INTERNAL : internal error
  */
esp_err_t esp_now_register_send_cb(esp_now_send_cb_t cb);

/**
  * @brief     Unregister callback function of sending ESPNOW data
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  */
esp_err_t esp_now_unregister_send_cb(void);

/**
  * @brief     Send ESPNOW data
  *
  * @attention 1. If peer_addr is not NULL, send data to the peer whose MAC address matches peer_addr
  * @attention 2. If peer_addr is NULL, send data to all of the peers that are added to the peer list
  * @attention 3. The maximum length of data must be less than ESP_NOW_MAX_DATA_LEN
  * @attention 4. The buffer pointed to by data argument does not need to be valid after esp_now_send returns
  *
  * @param     peer_addr  peer MAC address
  * @param     data  data to send
  * @param     len  length of data
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  *          - ESP_ERR_ESPNOW_INTERNAL : internal error
  *          - ESP_ERR_ESPNOW_NO_MEM : out of memory, when this happens, you can delay a while before sending the next data
  *          - ESP_ERR_ESPNOW_NOT_FOUND : peer is not found
  *          - ESP_ERR_ESPNOW_IF : current WiFi interface doesn't match that of peer
  */
esp_err_t esp_now_send(const uint8_t *peer_addr, const uint8_t *data, size_t len);

/**
  * @brief     Add a peer to peer list
  *
  * @param     peer  peer information
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  *          - ESP_ERR_ESPNOW_FULL : peer list is full
  *          - ESP_ERR_ESPNOW_NO_MEM : out of memory
  *          - ESP_ERR_ESPNOW_EXIST : peer has existed
  */
esp_err_t esp_now_add_peer(const esp_now_peer_info_t *peer);

/**
  * @brief     Delete a peer from peer list
  *
  * @param     peer_addr  peer MAC address
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  *          - ESP_ERR_ESPNOW_NOT_FOUND : peer is not found
  */
esp_err_t esp_now_del_peer(const uint8_t *peer_addr);

/**
  * @brief     Modify a peer
  *
  * @param     peer  peer information
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  *          - ESP_ERR_ESPNOW_FULL : peer list is full
  */
esp_err_t esp_now_mod_peer(const esp_now_peer_info_t *peer);

/**
  * @brief      Config ESPNOW rate of specified interface
  *
  * @attention  1. This API should be called after esp_wifi_start().
  *
  * @param      ifx  Interface to be configured.
  * @param      rate Phy rate to be configured.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_config_espnow_rate(wifi_interface_t ifx, wifi_phy_rate_t rate);

/**
  * @brief     Get a peer whose MAC address matches peer_addr from peer list
  *
  * @param     peer_addr  peer MAC address
  * @param     peer  peer information
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  *          - ESP_ERR_ESPNOW_NOT_FOUND : peer is not found
  */
esp_err_t esp_now_get_peer(const uint8_t *peer_addr, esp_now_peer_info_t *peer);

/**
  * @brief     Fetch a peer from peer list. Only return the peer which address is unicast, for the multicast/broadcast address, the function will ignore and try to find the next in the peer list.
  *
  * @param     from_head  fetch from head of list or not
  * @param     peer  peer information
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  *          - ESP_ERR_ESPNOW_NOT_FOUND : peer is not found
  */
esp_err_t esp_now_fetch_peer(bool from_head, esp_now_peer_info_t *peer);

/**
  * @brief     Peer exists or not
  *
  * @param     peer_addr  peer MAC address
  *
  * @return
  *          - true : peer exists
  *          - false : peer not exists
  */
bool esp_now_is_peer_exist(const uint8_t *peer_addr);

/**
  * @brief     Get the number of peers
  *
  * @param     num  number of peers
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  */
esp_err_t esp_now_get_peer_num(esp_now_peer_num_t *num);

/**
  * @brief     Set the primary master key
  *
  * @param     pmk  primary master key
  *
  * @attention 1. primary master key is used to encrypt local master key
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  */
esp_err_t esp_now_set_pmk(const uint8_t *pmk);

/**
  * @brief     Set esp_now wake window for sta_disconnected power management
  *
  * @param     window  how much microsecond would the chip keep waked each interval, vary from 0 to 65535
  *
  * @attention 1. Only when ESP_WIFI_STA_DISCONNECTED_PM_ENABLE is enabled, this configuration could work
  * @attention 2. This configuration only work for station mode and disconnected status
  * @attention 3. If more than one module has configured its wake_window, chip would choose the largest one to stay waked
  * @attention 4. If the gap between interval and window is smaller than 5ms, the chip would keep waked all the time
  * @attention 5. If never configured wake_window, the chip would keep waked at disconnected once it uses esp_now
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  */
esp_err_t esp_now_set_wake_window(uint16_t window);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_NOW_H__ */
