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

#ifndef _ESP_NETIF_H_
#define _ESP_NETIF_H_

#include <stdint.h>
#include "esp_wifi_types.h"
#include "sdkconfig.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_types.h"
#include "esp_netif_defaults.h"

//
// 1) Initialization API:
//

/**
 * @brief  Initialize the underlying TCP/IP stack
 *
 * @return
 *         - ESP_OK on success
 *         - ESP_FAIL if initializing failed

 * @note This function should be called exactly once from application code, when the application starts up.
 */
esp_err_t esp_netif_init(void);

/**
 * @brief  Deinitialize the esp-netif component (and the underlying TCP/IP stack)
 *
 *          Note: Deinitialization is not supported yet
 *
 * @return
 *         - ESP_ERR_INVALID_STATE if esp_netif not initialized
 *         - ESP_ERR_NOT_SUPPORTED otherwise
 */
esp_err_t esp_netif_deinit(void);

/**
 * @brief   Creates an instance of new esp-netif object based on provided config
 *
 * @param[in]     esp_netif_config pointer esp-netif configuration
 *
 * @return
 *         - pointer to esp-netif object on success
 *         - NULL otherwise
 */
esp_netif_t *esp_netif_new(const esp_netif_config_t *esp_netif_config);

/**
 * @brief   Destroys the esp_netif object
 *
 * @param[in]  pointer to the object to be deleted
 */
void esp_netif_destroy(esp_netif_t *esp_netif);

/**
 * @brief   Configures the esp_netif object
 *
 * Note: if some of the configuration parameter is null, the corresponding config
 * option is skipped. This enables calling this function multiple times to configure
 * different parts related to for example network stack or io driver separately
 *
 * @param[inout]  pointer to the object to be configured
 * @param[in]     esp_netif_config pointer esp-netif configuration
 * @return
 *         - ESP_OK
 *
 */
esp_err_t esp_netif_configure(esp_netif_t *esp_netif,
                              const esp_netif_config_t *esp_netif_config);

esp_err_t esp_netif_attach(esp_netif_t *esp_netif, esp_netif_iodriver_handle driver_handle);

//
// 2) Input - Output APIs
//

/**
  * @brief  Outputs packets from the TCP/IP stack to the media to be transmitted
  *
  * This function gets called from network stack to output packets to IO driver.
  *
  * @note This function is installed automatically for default interfaces.
  * Custom interface may need to install if the rx buffer passed as pointer and the IO driver has to
  * deallocate the data in driver context
  *
  * @param[in]  esp_netif Handle to esp-netif instance
  * @param[in]  void* buffer: rx buffer pointer
  */
esp_err_t esp_netif_transmit(esp_netif_t *esp_netif, void* data, size_t len);

/**
  * @brief  Free the rx buffer allocated by the media driver
  *
  * This function gets called from network stack when the rx buffer to be freed in media driver context.
  *
  * @note This function is installed automatically for default interfaces.
  * Custom interface may need to install if the rx buffer passed as pointer and the IO driver has to
  * deallocate the data in driver context
  *
  * @param[in]  esp_netif Handle to esp-netif instance
  * @param[in]  void* buffer: rx buffer pointer
  */
void esp_netif_free_rx_buffer(void *esp_netif, void* buffer);

/**
 * @brief  Passes the raw packets from communication media to the appropriate TCP/IP stack
 *
 * This function is called from the configured (peripheral) driver layer.
 * The data are then forwarded as frames to the TCP/IP stack.
 *
 * @note Installation happens automatically for default interfaces; custom IO drivers must install
 * this function so that it is called on a reception of incoming packet
 *
 * @note Application code does not usually need to call this function directly.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  buffer Received data
 * @param[in]  len Length of the data frame
 * @param[in]  eb Pointer to internal buffer (used in Wi-Fi driver)
 *
 * @return
 *         - ESP_OK
 */
esp_err_t esp_netif_receive(esp_netif_t *esp_netif, void *buffer, size_t len, void *eb);

//
// 3) ESP-NETIF lifecycle
//


/**
 * @brief Default building block for network interface action upon IO driver start event
 * Creates network interface, if AUTOUP enabled turns the interface on,
 * if DHCPS enabled starts dhcp server
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base
 * @param event_id
 * @param data
 */
void esp_netif_action_start(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IO driver stop event
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base
 * @param event_id
 * @param data
 */
void esp_netif_action_stop(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IO driver connected event
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base
 * @param event_id
 * @param data
 */
void esp_netif_action_connected(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IO driver disconnected event
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base
 * @param event_id
 * @param data
 */
void esp_netif_action_disconnected(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon network got IP event
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base
 * @param event_id
 * @param data
 */
void esp_netif_action_got_ip(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

//
/// 4) Configuration (getter - setters)
//
/**
 * @brief  Set the hostname of an interface
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]   hostname New hostname for the interface. Maximum length 32 bytes.
 *
 * @return
 *         - ESP_OK - success
 *         - ESP_ERR_ESP_NETIF_IF_NOT_READY - interface status error
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS - parameter error
 */
esp_err_t esp_netif_set_hostname(esp_netif_t *esp_netif, const char *hostname);

/**
 * @brief  Get interface hostname.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out]   hostname Returns a pointer to the hostname. May be NULL if no hostname is set. If set non-NULL, pointer remains valid (and string may change if the hostname changes).
 *
 * @return
 *         - ESP_OK - success
 *         - ESP_ERR_ESP_NETIF_IF_NOT_READY - interface status error
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS - parameter error
 */
esp_err_t esp_netif_get_hostname(esp_netif_t *esp_netif, const char **hostname);

/**
 * @brief  Test if supplied interface is up or down
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - true - Interface is up
 *         - false - Interface is down
 */
bool esp_netif_is_netif_up(esp_netif_t *esp_netif);

/**
 * @brief  Get interface's IP address information
 *
 * If the interface is up, IP information is read directly from the TCP/IP stack.
 * If the interface is down, IP information is read from a copy kept in the ESP-NETIF instance
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out]  ip_info If successful, IP information will be returned in this argument.
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 */
esp_err_t esp_netif_get_ip_info(esp_netif_t *esp_netif, esp_netif_ip_info_t *ip_info);

/**
 * @brief  Get interface's old IP information
 *
 * Returns an "old" IP address previously stored for the interface when the valid IP changed.
 *
 * If the IP lost timer has expired (meaning the interface was down for longer than the configured interval)
 * then the old IP information will be zero.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out]  ip_info If successful, IP information will be returned in this argument.
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 */
esp_err_t esp_netif_get_old_ip_info(esp_netif_t *esp_netif, esp_netif_ip_info_t *ip_info);

/**
 * @brief  Set interface's IP address information
 *
 * This function is mainly used to set a static IP on an interface.
 *
 * If the interface is up, the new IP information is set directly in the TCP/IP stack.
 *
 * The copy of IP information kept in the ESP-NETIF instance is also updated (this
 * copy is returned if the IP is queried while the interface is still down.)
 *
 * @note DHCP client/server must be stopped (if enabled for this interface) before setting new IP information.
 *
 * @note Calling this interface for may generate a SYSTEM_EVENT_STA_GOT_IP or SYSTEM_EVENT_ETH_GOT_IP event.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in] ip_info IP information to set on the specified interface
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *      - ESP_ERR_ESP_NETIF_DHCP_NOT_STOPPED If DHCP server or client is still running
 */
esp_err_t esp_netif_set_ip_info(esp_netif_t *esp_netif, const esp_netif_ip_info_t *ip_info);

/**
 * @brief  Set interface old IP information
 *
 * This function is called from the DHCP client (if enabled), before a new IP is set.
 * It is also called from the default handlers for the SYSTEM_EVENT_STA_CONNECTED and SYSTEM_EVENT_ETH_CONNECTED events.
 *
 * Calling this function stores the previously configured IP, which can be used to determine if the IP changes in the future.
 *
 * If the interface is disconnected or down for too long, the "IP lost timer" will expire (after the configured interval) and set the old IP information to zero.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  ip_info Store the old IP information for the specified interface
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 */
esp_err_t esp_netif_set_old_ip_info(esp_netif_t *esp_netif, const esp_netif_ip_info_t *ip_info);


//
// 5) Network stack related API
//

//
// DHCP
//
/**
 * @brief  Set or Get DHCP server option
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in] opt_op ESP_NETIF_OP_SET to set an option, ESP_NETIF_OP_GET to get an option.
 * @param[in] opt_id Option index to get or set, must be one of the supported enum values.
 * @param[inout] opt_val Pointer to the option parameter.
 * @param[in] opt_len Length of the option parameter.
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED
 *         - ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED
 */
esp_err_t esp_netif_dhcps_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len);

esp_err_t esp_netif_dhcpc_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief Start DHCP client (only if enabled in interface object)
 *
 * @note The default event handlers for the SYSTEM_EVENT_STA_CONNECTED and SYSTEM_EVENT_ETH_CONNECTED events call this function.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED
 *         - ESP_ERR_ESP_NETIF_DHCPC_START_FAILED
 */
esp_err_t esp_netif_dhcpc_start(esp_netif_t *esp_netif);

/**
 * @brief  Stop DHCP client (only if enabled in interface object)
 *
 * @note Calling action_netif_stop() will also stop the DHCP Client if it is running.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *      - ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED
 *      - ESP_ERR_ESP_NETIF_IF_NOT_READY
 */
esp_err_t esp_netif_dhcpc_stop(esp_netif_t *esp_netif);

/**
 * @brief  Get DHCP client status
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out] status If successful, the status of DHCP client will be returned in this argument.
 *
 * @return
 *         - ESP_OK
 */
esp_err_t esp_netif_dhcpc_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status);

/**
 * @brief  Get DHCP Server status
 *
 * @param[in]   esp_netif Handle to esp-netif instance
 * @param[out]  status If successful, the status of the DHCP server will be returned in this argument.
 *
 * @return
 *         - ESP_OK
 */
esp_err_t esp_netif_dhcps_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status);

/**
 * @brief  Start DHCP server (only if enabled in interface object)
 *
 * @param[in]   esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED
 */
esp_err_t esp_netif_dhcps_start(esp_netif_t *esp_netif);

/**
 * @brief  Stop DHCP server (only if enabled in interface object)
 *
 * @param[in]   esp_netif Handle to esp-netif instance
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *      - ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED
 *      - ESP_ERR_ESP_NETIF_IF_NOT_READY
 */
esp_err_t esp_netif_dhcps_stop(esp_netif_t *esp_netif);

//
// DNS:
//
/**
 * @brief  Set DNS Server information
 *
 * This function behaves differently if DHCP server or client is enabled
 *
 *   If DHCP client is enabled, main and backup DNS servers will be updated automatically 
 *   from the DHCP lease if the relevant DHCP options are set. Fallback DNS Server is never updated from the DHCP lease 
 *   and is designed to be set via this API.
 *   If DHCP client is disabled, all DNS server types can be set via this API only.
 *   
 *   If DHCP server is enabled, the Main DNS Server setting is used by the DHCP server to provide a DNS Server option 
 *   to DHCP clients (Wi-Fi stations).
 *   - The default Main DNS server is typically the IP of the Wi-Fi AP interface itself.
 *   - This function can override it by setting server type ESP_NETIF_DNS_MAIN.
 *   - Other DNS Server types are not supported for the Wi-Fi AP interface.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  type Type of DNS Server to set: ESP_NETIF_DNS_MAIN, ESP_NETIF_DNS_BACKUP, ESP_NETIF_DNS_FALLBACK
 * @param[in]  dns  DNS Server address to set
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS invalid params
 */
esp_err_t esp_netif_set_dns_info(esp_netif_t *esp_netif, esp_netif_dns_type_t type, esp_netif_dns_info_t *dns);

/**
 * @brief  Get DNS Server information
 *
 * Return the currently configured DNS Server address for the specified interface and Server type.
 *
 * This may be result of a previous call to esp_netif_set_dns_info(). If the interface's DHCP client is enabled,
 * the Main or Backup DNS Server may be set by the current DHCP lease.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  type Type of DNS Server to get: ESP_NETIF_DNS_MAIN, ESP_NETIF_DNS_BACKUP, ESP_NETIF_DNS_FALLBACK
 * @param[out] dns  DNS Server result is written here on success
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS invalid params
 */
esp_err_t esp_netif_get_dns_info(esp_netif_t *esp_netif, esp_netif_dns_type_t type, esp_netif_dns_info_t *dns);
/**
 * @brief Set the mac address for the interface instance

 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  mac Desired mac address for the related network interface
 * @return     ESP_OK
 */
esp_err_t esp_netif_set_mac(esp_netif_t *esp_netif, uint8_t mac[]);

/**
 * @brief  Create interface link-local IPv6 address
 *
 * Cause the TCP/IP stack to create a link-local IPv6 address for the specified interface.
 *
 * This function also registers a callback for the specified interface, so that if the link-local address becomes
 * verified as the preferred address then a SYSTEM_EVENT_GOT_IP6 event will be sent.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 */
esp_err_t esp_netif_create_ip6_linklocal(esp_netif_t *esp_netif);

/**
 * @brief  Get interface link-local IPv6 address
 *
 * If the specified interface is up and a preferred link-local IPv6 address
 * has been created for the interface, return a copy of it.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out] if_ip6 IPv6 information will be returned in this argument if successful.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL If interface is down, does not have a link-local IPv6 address,
 *        or the link-local IPv6 address is not a preferred address.
 */
esp_err_t esp_netif_get_ip6_linklocal(esp_netif_t *esp_netif, esp_ip6_addr_t *if_ip6);

/**
 * @brief Sets IPv4 address to the specified octets
 *
 * @param[out] addr IP address to be set
 * @param a the first octet (127 for IP 127.0.0.1)
 * @param b
 * @param c
 * @param d
 */
void esp_netif_set_ip4_addr(esp_ip4_addr_t *addr, uint8_t a, uint8_t b, uint8_t c, uint8_t d);


/**
 * @brief Converts numeric IP address into decimal dotted ASCII representation.
 *
 * @param addr ip address in network order to convert
 * @param buf target buffer where the string is stored
 * @param buflen length of buf
 * @return either pointer to buf which now holds the ASCII
 *         representation of addr or NULL if buf was too small
 */
char * esp_ip4addr_ntoa(const esp_ip4_addr_t *addr, char *buf, int buflen);

//
// 6) Driver conversion utilities to related keys, flags, implementation handle, list of netifs
//

/**
 * @brief Gets media driver handle for this esp-netif instance
 * 
 * @param[in]  esp_netif Handle to esp-netif instance
 * 
 * @return opaque pointer of related IO driver
 */
esp_netif_iodriver_handle esp_netif_get_io_driver(esp_netif_t *esp_netif);

/**
 * @brief Searches over a list of created objects to find an instance with supplied if key
 * 
 * @param if_key Textual description of network interface
 * 
 * @return Handle to esp-netif instance
 */
esp_netif_t *esp_netif_get_handle_from_ifkey(const char *if_key);

/**
 * @brief Returns configured flags for this interface
 * 
 * @param[in]  esp_netif Handle to esp-netif instance
 * 
 * @return Configuration flags 
 */
esp_netif_flags_t esp_netif_get_flags(esp_netif_t *esp_netif);

/**
 * @brief Returns configured interface key for this esp-netif instance
 * 
 * @param[in]  esp_netif Handle to esp-netif instance
 * 
 * @return Textual description of related interface
 */
char *esp_netif_get_ifkey(esp_netif_t *esp_netif);

/**
 * @brief Returns configured interface type for this esp-netif instance
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return Enumerated type of this interface, such as station, AP, ethernet
 */
esp_netif_type_t esp_netif_get_type(esp_netif_t *esp_netif);

/**
 * @brief Returns configured event for this esp-netif instance and supplied event type
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @param event_type (either get or lost IP)
 * 
 * @return specific event id which is configured to be raised if the interface lost or acquired IP address
 */
uint32_t esp_netif_get_event_id(esp_netif_t *esp_netif, esp_netif_ip_event_type_t event_type);

//
// 7) esp_netif list API
//
/**
 * @brief Iterates over list of interfaces. Returns first netif if NULL given as parameter
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return First netif from the list if supplied parameter is NULL, next one otherwise
 */
esp_netif_t* esp_netif_next(esp_netif_t* netif);

/**
 * @brief Returns number of registered esp_netif objects
 *
 * @return Number of esp_netifs
 */
size_t esp_netif_get_nr_of_ifs(void);

//
// 8) MISC: List of STAs
//

/**
 * @brief  Get IP information for stations connected to the Wi-Fi AP interface
 *
 * @param[in]   wifi_sta_list Wi-Fi station info list, returned from esp_wifi_ap_get_sta_list()
 * @param[out]  tcpip_sta_list IP layer station info list, corresponding to MAC addresses provided in wifi_sta_list
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_NO_MEM
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 */
esp_err_t esp_netif_get_sta_list(const wifi_sta_list_t *wifi_sta_list, esp_netif_sta_list_t *tcpip_sta_list);

int esp_netif_get_netif_index(esp_netif_t *esp_netif);

#if CONFIG_ESP_NETIF_USE_TCPIP_ADAPTER_COMPATIBLE_LAYER
//
// 8) Compatibility layer
//
#include "tcpip_adapter.h"
#endif

#endif /*  _ESP_NETIF_H_ */
