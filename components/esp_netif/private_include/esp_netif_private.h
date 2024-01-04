/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_NETIF_PRIVATE_H_
#define _ESP_NETIF_PRIVATE_H_

#define ESP_NETIF_CALL_CHECK(info, api_call, ret) \
do{\
    esp_err_t __err = (api_call);\
    if ((ret) != __err) {\
        ESP_LOGE(TAG, "%s %d %s ret=0x%X", __FUNCTION__, __LINE__, (info), __err);\
        return;\
    }\
} while(0)

/**
 * @brief  Cause the TCP/IP stack to start the ESP-NETIF instance interface
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_NO_MEM
 */
esp_err_t esp_netif_start(esp_netif_t *esp_netif);

/**
 * @brief  Cause the TCP/IP stack to stop a network interface defined as ESP-NETIF instance
 *
 * Causes TCP/IP stack to clean up this interface. This includes stopping the DHCP server or client, if they are started.
 *
 * @note To stop an interface from application code, the media driver-specific API (esp_wifi_stop() or esp_eth_stop())
 * shall be called, the driver layer will then send a stop event and the event handler should call this API.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_IF_NOT_READY
 */
esp_err_t esp_netif_stop(esp_netif_t *esp_netif);

/**
 * @brief  Cause the TCP/IP stack to bring up an interface
 * This function is called automatically by default called from event handlers/actions
 *
 * @note This function is not normally used with Wi-Fi AP interface. If the AP interface is started, it is up.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_IF_NOT_READY
 */
esp_err_t esp_netif_up(esp_netif_t *esp_netif);

/**
 * @brief Cause the TCP/IP stack to shutdown an interface
 * This function is called automatically by default called from event handlers/actions
 *
 * @note This function is not normally used with Wi-Fi AP interface. If the AP interface is stopped, it is down.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS - parameter error
 */
esp_err_t esp_netif_down(esp_netif_t *esp_netif);

/**
 * @brief Returns true if underlying TCP/IP stack finds the ip_info as valid static address
 *
 * @param[in] ip_info
 * @return true if address assumed to be valid static IP address
 */
bool esp_netif_is_valid_static_ip(esp_netif_ip_info_t *ip_info);

/**
 * @brief Adds created interface to the list of netifs.
 * This function doesn't lock the list, so you need to call esp_netif_list_lock/unlock
 * manually before and after the call.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK -- Success
 *         - ESP_ERR_NO_MEM -- Cannot be added due to memory allocation failure
 */
esp_err_t esp_netif_add_to_list_unsafe(esp_netif_t* netif);

/**
 * @brief Removes interface to be destroyed from the list of netifs
 * This function doesn't lock the list, so you need to call esp_netif_list_lock/unlock
 * manually before and after the call.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK -- Success
 *         - ESP_ERR_NOT_FOUND -- This netif was not found in the netif list
 */
esp_err_t esp_netif_remove_from_list_unsafe(esp_netif_t* netif);

/**
 * @brief Iterates over list of registered interfaces to check if supplied netif is listed
 *
 * @param esp_netif network interface to check
 *
 * @return true if supplied interface is listed
 */
bool esp_netif_is_netif_listed(esp_netif_t *esp_netif);

/**
 * @brief Get esp_netif handle based on the if_key
 * This doesn't lock the list nor TCPIP context
 *
 * @param if_key
 * @return esp_netif handle if found, NULL otherwise
 */
esp_netif_t *esp_netif_get_handle_from_ifkey_unsafe(const char *if_key);

#endif //_ESP_NETIF_PRIVATE_H_
