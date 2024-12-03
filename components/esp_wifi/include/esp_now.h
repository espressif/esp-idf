/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
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
#define ESP_ERR_ESPNOW_CHAN         (ESP_ERR_ESPNOW_BASE + 9) /*!< Channel error */

#define ESP_NOW_ETH_ALEN             6         /*!< Length of ESPNOW peer MAC address */
#define ESP_NOW_KEY_LEN              16        /*!< Length of ESPNOW peer local master key */

#define ESP_NOW_MAX_TOTAL_PEER_NUM   20        /*!< Maximum number of ESPNOW total peers */
#define ESP_NOW_MAX_ENCRYPT_PEER_NUM 6         /*!< Maximum number of ESPNOW encrypted peers */

#define ESP_NOW_MAX_IE_DATA_LEN      250       /**< Maximum data length in a vendor-specific element */
#define ESP_NOW_MAX_DATA_LEN  ESP_NOW_MAX_IE_DATA_LEN   /**< Maximum length of data sent in each ESPNOW transmission for v1.0 */

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
 * @brief ESPNOW packet information
 */
typedef struct esp_now_recv_info {
    uint8_t * src_addr;                      /**< Source address of ESPNOW packet */
    uint8_t * des_addr;                      /**< Destination address of ESPNOW packet */
    wifi_pkt_rx_ctrl_t * rx_ctrl;            /**< Rx control info of ESPNOW packet */
} esp_now_recv_info_t;

/**
 * @brief ESPNOW rate config
 */
typedef wifi_tx_rate_config_t esp_now_rate_config_t;

/**
  * @brief     Callback function of receiving ESPNOW data
  * @param     esp_now_info received ESPNOW packet information
  * @param     data received data
  * @param     data_len length of received data
  * @attention esp_now_info is a local variable，it can only be used in the callback.
  */
typedef void (*esp_now_recv_cb_t)(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len);

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
  * @brief     Get the version of ESPNOW. Currently, ESPNOW supports one version: v1.0.
  *
  *            The v1.0 devices can receive packets if the packet length is less than or equal to ESP_NOW_MAX_IE_DATA_LEN.
  *            For packets exceeding this length, the v1.0 devices will discard the packet entirely.
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
  *          - ESP_ERR_ESPNOW_IF : current Wi-Fi interface doesn't match that of peer
  *          - ESP_ERR_ESPNOW_CHAN: current Wi-Fi channel doesn't match that of peer
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
  * @deprecated please use esp_now_set_peer_rate_config() instead.
  *
  * @attention  1. This API should be called after esp_wifi_start().
  * @attention  2. This API only work when not use Wi-Fi 6 and esp_now_set_peer_rate_config() not called.
  *
  * @param      ifx  Interface to be configured.
  * @param      rate Phy rate to be configured.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_config_espnow_rate(wifi_interface_t ifx, wifi_phy_rate_t rate)
__attribute__((deprecated("This API can be only used when rate is non-HE rate, \
                                please use esp_now_set_peer_rate_config if you want full support of the rate.")));

/**
  * @brief      Set ESPNOW rate config for each peer
  *
  * @attention  1. This API should be called after esp_wifi_start() and esp_now_init().
  *
  * @param      peer_addr  peer MAC address
  * @param      config rate config to be configured.
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_ESPNOW_NOT_INIT : ESPNOW is not initialized
  *          - ESP_ERR_ESPNOW_ARG : invalid argument
  *          - ESP_ERR_ESPNOW_INTERNAL : internal error
  */
esp_err_t esp_now_set_peer_rate_config(const uint8_t *peer_addr, esp_now_rate_config_t *config);

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
  * @brief     Set wake window for esp_now to wake up in interval unit
  *
  * @param     window  Milliseconds would the chip keep waked each interval, from 0 to 65535.
  *
  * @attention 1. This configuration could work at connected status.
  *               When ESP_WIFI_STA_DISCONNECTED_PM_ENABLE is enabled, this configuration could work at disconnected status.
  * @attention 2. Default value is the maximum.
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
