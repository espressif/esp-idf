// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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
 * @brief Adds created interface to the list of netifs
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK -- Success
 *         - ESP_ERR_NO_MEM -- Cannot be added due to memory allocation failure
 */
esp_err_t esp_netif_add_to_list(esp_netif_t* netif);

/**
 * @brief Removes interface to be destroyed from the list of netifs
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK -- Success
 *         - ESP_ERR_NOT_FOUND -- This netif was not found in the netif list
 */
esp_err_t esp_netif_remove_from_list(esp_netif_t* netif);

/**
 * @brief Iterates over list of interfaces without list locking. Returns first netif if NULL given as parameter
 *
 * Used for bulk search loops to avoid locking and unlocking every iteration. esp_netif_list_lock and esp_netif_list_unlock
 * must be used to guard the search loop
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return First netif from the list if supplied parameter is NULL, next one otherwise
 */
esp_netif_t* esp_netif_next_unsafe(esp_netif_t* netif);

/**
 * @brief Locking network interface list. Use only in connection with esp_netif_next_unsafe
 *
 * @return ESP_OK on success, specific mutex error if failed to lock
 */
esp_err_t esp_netif_list_lock(void);

/**
 * @brief Unlocking network interface list. Use only in connection with esp_netif_next_unsafe
 *
 */
void esp_netif_list_unlock(void);

/**
 * @brief Iterates over list of registered interfaces to check if supplied netif is listed
 *
 * @param esp_netif network interface to check
 *
 * @return true if supplied interface is listed
 */
bool esp_netif_is_netif_listed(esp_netif_t *esp_netif);

/**
 * @brief  Cause the TCP/IP stack to join a multicast group
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  addr      The multicast group to join
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_MLD6_FAILED
 *         - ESP_ERR_NO_MEM
 */
esp_err_t esp_netif_join_ip6_multicast_group(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr);

/**
 * @brief  Cause the TCP/IP stack to leave a multicast group
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  addr      The multicast group to leave
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_MLD6_FAILED
 *         - ESP_ERR_NO_MEM
 */
esp_err_t esp_netif_leave_ip6_multicast_group(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr);

/**
 * @brief  Cause the TCP/IP stack to add an IPv6 address to the interface
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  addr      The address to be added
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_IP6_ADDR_FAILED
 *         - ESP_ERR_NO_MEM
 */
esp_err_t esp_netif_add_ip6_address(esp_netif_t *esp_netif, const ip_event_add_ip6_t *addr);

/**
 * @brief  Cause the TCP/IP stack to remove an IPv6 address from the interface
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  addr      The address to be removed
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_IP6_ADDR_FAILED
 *         - ESP_ERR_NO_MEM
 */
esp_err_t esp_netif_remove_ip6_address(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr);

#endif //_ESP_NETIF_PRIVATE_H_
