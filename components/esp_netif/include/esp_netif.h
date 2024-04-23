/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_NETIF_H_
#define _ESP_NETIF_H_

#include <stdint.h>
#include "sdkconfig.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_types.h"
#include "esp_netif_defaults.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ESP_NETIF_INIT_API ESP-NETIF Initialization API
 * @brief Initialization and deinitialization of underlying TCP/IP stack and esp-netif instances
 *
 */

/** @addtogroup ESP_NETIF_INIT_API
 * @{
 */

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
 * @param[in]  esp_netif pointer to the object to be deleted
 */
void esp_netif_destroy(esp_netif_t *esp_netif);

/**
 * @brief   Configures driver related options of esp_netif object
 *
 * @param[inout]  esp_netif pointer to the object to be configured
 * @param[in]     driver_config pointer esp-netif io driver related configuration
 * @return
 *         - ESP_OK on success
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS if invalid parameters provided
 *
 */
esp_err_t esp_netif_set_driver_config(esp_netif_t *esp_netif,
                                      const esp_netif_driver_ifconfig_t *driver_config);

/**
 * @brief   Attaches esp_netif instance to the io driver handle
 *
 * Calling this function enables connecting specific esp_netif object
 * with already initialized io driver to update esp_netif object with driver
 * specific configuration (i.e. calls post_attach callback, which typically
 * sets io driver callbacks to esp_netif instance and starts the driver)
 *
 * @param[inout]  esp_netif pointer to esp_netif object to be attached
 * @param[in]  driver_handle pointer to the driver handle
 * @return
 *         - ESP_OK on success
 *         - ESP_ERR_ESP_NETIF_DRIVER_ATTACH_FAILED if driver's pot_attach callback failed
 */
esp_err_t esp_netif_attach(esp_netif_t *esp_netif, esp_netif_iodriver_handle driver_handle);

/**
 * @}
 */

/**
 * @defgroup ESP_NETIF_DATA_IO_API ESP-NETIF Input Output API
 * @brief Input and Output functions to pass data packets from communication media (IO driver)
 * to TCP/IP stack.
 *
 * These functions are usually not directly called from user code, but installed, or registered
 * as callbacks in either IO driver on one hand or TCP/IP stack on the other. More specifically
 * esp_netif_receive is typically called from io driver on reception callback to input the packets
 * to TCP/IP stack. Similarly esp_netif_transmit is called from the TCP/IP stack whenever
 * a packet ought to output to the communication media.
 *
 * @note These IO functions are registered (installed) automatically for default interfaces
 * (interfaces with the keys such as WIFI_STA_DEF, WIFI_AP_DEF, ETH_DEF). Custom interface
 * has to register these IO functions when creating interface using @ref esp_netif_new
 *
 */

/** @addtogroup ESP_NETIF_DATA_IO_API
 * @{
 */

/**
 * @brief  Passes the raw packets from communication media to the appropriate TCP/IP stack
 *
 * This function is called from the configured (peripheral) driver layer.
 * The data are then forwarded as frames to the TCP/IP stack.
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

/**
 * @brief Enables transmit/receive event reporting for a network interface.
 *
 * These functions enables transmit and receive events reporting for a given esp-netif instance.
 * Event reporting can be used to track data transfer activity and trigger application-specific actions.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK: Successfully enabled event reporting
 *         - ESP_FAIL: Event reporting not configured
 */
esp_err_t esp_netif_tx_rx_event_enable(esp_netif_t *esp_netif);

/**
 * @brief Disables transmit/receive event reporting for a network interface.
 *
 * These functions disables transmit and receive events reporting for a given esp-netif instance.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK: Successfully disabled event reporting
 *         - ESP_FAIL: Event reporting not configured
 */
esp_err_t esp_netif_tx_rx_event_disable(esp_netif_t *esp_netif);

/**
 * @}
 */

/**
 * @defgroup ESP_NETIF_LIFECYCLE ESP-NETIF Lifecycle control
 * @brief These APIS define basic building blocks to control network interface lifecycle, i.e.
 * start, stop, set_up or set_down. These functions can be directly used as event handlers
 * registered to follow the events from communication media.
 */

/** @addtogroup ESP_NETIF_LIFECYCLE
 * @{
 */

/**
 * @brief Default building block for network interface action upon IO driver start event
 * Creates network interface, if AUTOUP enabled turns the interface on,
 * if DHCPS enabled starts dhcp server
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_start(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IO driver stop event
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_stop(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IO driver connected event
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_connected(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IO driver disconnected event
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_disconnected(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon network got IP event
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_got_ip(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IPv6 multicast group join
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_join_ip6_multicast_group(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IPv6 multicast group leave
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_leave_ip6_multicast_group(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IPv6 address added by the underlying stack
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_add_ip6_address(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Default building block for network interface action upon IPv6 address removed by the underlying stack
 *
 * @note This API can be directly used as event handler
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param base The base type of the event
 * @param event_id The specific ID of the event
 * @param data Optional data associated with the event
 */
void esp_netif_action_remove_ip6_address(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);

/**
 * @brief Manual configuration of the default netif
 *
 * This API overrides the automatic configuration of the default interface based on the route_prio
 * If the selected netif is set default using this API, no other interface could be set-default disregarding
 * its route_prio number (unless the selected netif gets destroyed)
 *
 * @param[in] esp_netif Handle to esp-netif instance
 * @return ESP_OK on success
 */
esp_err_t esp_netif_set_default_netif(esp_netif_t *esp_netif);

/**
 * @brief Getter function of the default netif
 *
 * This API returns the selected default netif.
 *
 * @return Handle to esp-netif instance of the default netif.
 */
esp_netif_t* esp_netif_get_default_netif(void);

#if CONFIG_ESP_NETIF_BRIDGE_EN
/**
 * @brief Add a port to the bridge
 *
 * @param esp_netif_br Handle to bridge esp-netif instance
 * @param esp_netif_port Handle to port esp-netif instance
 * @return ESP_OK on success
 */
esp_err_t esp_netif_bridge_add_port(esp_netif_t *esp_netif_br, esp_netif_t *esp_netif_port);

/**
 * @brief Add a static entry to bridge forwarding database
 *
 * @param esp_netif_br Handle to bridge esp-netif instance
 * @param addr MAC address entry to be added
 * @param ports_mask Port(s) mask where to be the address forwarded
 * @return ESP_OK on success
 */
esp_err_t esp_netif_bridge_fdb_add(esp_netif_t *esp_netif_br, uint8_t *addr, uint64_t ports_mask);

/**
 * @brief Remove a static entry from bridge forwarding database
 *
 * @param esp_netif_br Handle to bridge esp-netif instance
 * @param addr MAC address entry to be removed
 * @return ESP_OK on success
 */
esp_err_t esp_netif_bridge_fdb_remove(esp_netif_t *esp_netif_br, uint8_t *addr);
#endif // CONFIG_ESP_NETIF_BRIDGE_EN

/**
 * @brief  Cause the TCP/IP stack to join a IPv6 multicast group
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
 * @brief  Cause the TCP/IP stack to leave a IPv6 multicast group
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
 * @}
 */

/**
 * @defgroup ESP_NETIF_GET_SET ESP-NETIF Runtime configuration
 * @brief Getters and setters for various TCP/IP related parameters
 */

/** @addtogroup ESP_NETIF_GET_SET
 * @{
 */

/**
 * @brief Set the mac address for the interface instance

 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  mac Desired mac address for the related network interface
 * @return
 *         - ESP_OK - success
 *         - ESP_ERR_ESP_NETIF_IF_NOT_READY - interface status error
 *         - ESP_ERR_NOT_SUPPORTED - mac not supported on this interface
 */
esp_err_t esp_netif_set_mac(esp_netif_t *esp_netif, uint8_t mac[]);

/**
 * @brief Get the mac address for the interface instance

 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out]  mac Resultant mac address for the related network interface
 * @return
 *         - ESP_OK - success
 *         - ESP_ERR_ESP_NETIF_IF_NOT_READY - interface status error
 *         - ESP_ERR_NOT_SUPPORTED - mac not supported on this interface
 */
esp_err_t esp_netif_get_mac(esp_netif_t *esp_netif, uint8_t mac[]);

/**
 * @brief  Set the hostname of an interface
 *
 * The configured hostname overrides the default configuration value CONFIG_LWIP_LOCAL_HOSTNAME.
 * Please note that when the hostname is altered after interface started/connected the changes
 * would only be reflected once the interface restarts/reconnects
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

/**
 * @brief  Get net interface index from network stack implementation
 *
 * @note This index could be used in `setsockopt()` to bind socket with multicast interface
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         implementation specific index of interface represented with supplied esp_netif
 */
int esp_netif_get_netif_impl_index(esp_netif_t *esp_netif);

/**
 * @brief  Get net interface name from network stack implementation
 *
 * @note This name could be used in `setsockopt()` to bind socket with appropriate interface
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out]  name Interface name as specified in underlying TCP/IP stack. Note that the
 * actual name will be copied to the specified buffer, which must be allocated to hold
 * maximum interface name size (6 characters for lwIP)
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
*/
esp_err_t esp_netif_get_netif_impl_name(esp_netif_t *esp_netif, char* name);

/**
 * @brief  Enable NAPT on an interface
 *
 * @note Enable operation can be performed only on one interface at a time.
 * NAPT cannot be enabled on multiple interfaces according to this implementation.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_FAIL
 *         - ESP_ERR_NOT_SUPPORTED
*/

esp_err_t esp_netif_napt_enable(esp_netif_t *esp_netif);

/**
 * @brief  Disable NAPT on an interface.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return
 *         - ESP_OK
 *         - ESP_FAIL
 *         - ESP_ERR_NOT_SUPPORTED
*/
esp_err_t esp_netif_napt_disable(esp_netif_t *esp_netif);

/**
 * @}
 */

/**
 * @defgroup ESP_NETIF_NET_DHCP ESP-NETIF DHCP Settings
 * @brief Network stack related interface to DHCP client and server
 */

/** @addtogroup ESP_NETIF_NET_DHCP
 * @{
 */

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
esp_err_t
esp_netif_dhcps_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id,
                       void *opt_val, uint32_t opt_len);

/**
 * @brief  Set or Get DHCP client option
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
esp_err_t
esp_netif_dhcpc_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id,
                       void *opt_val, uint32_t opt_len);

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

/**
 * @brief  Populate IP addresses of clients connected to DHCP server listed by their MAC addresses
 *
 * @param[in] esp_netif Handle to esp-netif instance
 * @param[in] num Number of clients with specified MAC addresses in the array of pairs
 * @param[in,out] mac_ip_pair Array of pairs of MAC and IP addresses (MAC are inputs, IP outputs)
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_ESP_NETIF_INVALID_PARAMS on invalid params
 *      - ESP_ERR_NOT_SUPPORTED if DHCP server not enabled
 */
esp_err_t esp_netif_dhcps_get_clients_by_mac(esp_netif_t *esp_netif, int num, esp_netif_pair_mac_ip_t *mac_ip_pair);

/**
 * @}
 */

/**
 * @defgroup ESP_NETIF_NET_DNS ESP-NETIF DNS Settings
 * @brief Network stack related interface to NDS
 */

/** @addtogroup ESP_NETIF_NET_DNS
 * @{
 */

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
 *   - The default Main DNS server is typically the IP of the DHCP server itself.
 *   - This function can override it by setting server type ESP_NETIF_DNS_MAIN.
 *   - Other DNS Server types are not supported for the DHCP server.
 *   - To propagate the DNS info to client, please stop the DHCP server before using this API.
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
 * @}
 */

/**
 * @defgroup ESP_NETIF_NET_IP ESP-NETIF IP address related interface
 * @brief Network stack related interface to IP
 */

/** @addtogroup ESP_NETIF_NET_IP
 * @{
 */
#if CONFIG_LWIP_IPV6
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
 * @brief  Get interface global IPv6 address
 *
 * If the specified interface is up and a preferred global IPv6 address
 * has been created for the interface, return a copy of it.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out] if_ip6 IPv6 information will be returned in this argument if successful.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL If interface is down, does not have a global IPv6 address,
 *        or the global IPv6 address is not a preferred address.
 */
esp_err_t esp_netif_get_ip6_global(esp_netif_t *esp_netif, esp_ip6_addr_t *if_ip6);

/**
 * @brief  Get all IPv6 addresses of the specified interface
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[out] if_ip6 Array of IPv6 addresses will be copied to the argument
 *
 * @return
 *      number of returned IPv6 addresses
 */
int esp_netif_get_all_ip6(esp_netif_t *esp_netif, esp_ip6_addr_t if_ip6[]);

/**
 * @brief  Cause the TCP/IP stack to add an IPv6 address to the interface
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 * @param[in]  addr      The address to be added
 * @param[in]  preferred The preferred status of the address
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_ESP_NETIF_INVALID_PARAMS
 *         - ESP_ERR_ESP_NETIF_IP6_ADDR_FAILED
 *         - ESP_ERR_NO_MEM
 */
esp_err_t esp_netif_add_ip6_address(esp_netif_t *esp_netif, const esp_ip6_addr_t addr, bool preferred);

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
#endif

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
char *esp_ip4addr_ntoa(const esp_ip4_addr_t *addr, char *buf, int buflen);

/**
 * @brief Ascii internet address interpretation routine
 * The value returned is in network order.
 *
 * @param addr IP address in ascii representation (e.g. "127.0.0.1")
 * @return ip address in network order
*/
uint32_t esp_ip4addr_aton(const char *addr);

/**
 * @brief Converts Ascii internet IPv4 address into esp_ip4_addr_t
 *
 * @param[in] src IPv4 address in ascii representation (e.g. "127.0.0.1")
 * @param[out] dst Address of the target esp_ip4_addr_t structure to receive converted address
 * @return
 *         - ESP_OK on success
 *         - ESP_FAIL if conversion failed
 *         - ESP_ERR_INVALID_ARG if invalid parameter is passed into
 */
esp_err_t esp_netif_str_to_ip4(const char *src, esp_ip4_addr_t *dst);

/**
 * @brief Converts Ascii internet IPv6 address into esp_ip4_addr_t
 * Zeros in the IP address can be stripped or completely omitted: "2001:db8:85a3:0:0:0:2:1" or "2001:db8::2:1")
 *
 * @param[in] src IPv6 address in ascii representation (e.g. ""2001:0db8:85a3:0000:0000:0000:0002:0001")
 * @param[out] dst Address of the target esp_ip6_addr_t structure to receive converted address
 * @return
 *         - ESP_OK on success
 *         - ESP_FAIL if conversion failed
 *         - ESP_ERR_INVALID_ARG if invalid parameter is passed into
 */
esp_err_t esp_netif_str_to_ip6(const char *src, esp_ip6_addr_t *dst);

/**
 * @}
 */

/**
 * @defgroup ESP_NETIF_CONVERT ESP-NETIF Conversion utilities
 * @brief  ESP-NETIF conversion utilities to related keys, flags, implementation handle
 */

/** @addtogroup ESP_NETIF_CONVERT
 * @{
 */

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
const char *esp_netif_get_ifkey(esp_netif_t *esp_netif);

/**
 * @brief Returns configured interface type for this esp-netif instance
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return Enumerated type of this interface, such as station, AP, ethernet
 */
const char *esp_netif_get_desc(esp_netif_t *esp_netif);

/**
 * @brief Returns configured routing priority number
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return Integer representing the instance's route-prio, or -1 if invalid parameters
 */
int esp_netif_get_route_prio(esp_netif_t *esp_netif);

/**
 * @brief Returns configured event for this esp-netif instance and supplied event type
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @param event_type (either get or lost IP)
 *
 * @return specific event id which is configured to be raised if the interface lost or acquired IP address
 *         -1 if supplied event_type is not known
 */
int32_t esp_netif_get_event_id(esp_netif_t *esp_netif, esp_netif_ip_event_type_t event_type);

/**
 * @}
 */

/**
 * @defgroup ESP_NETIF_LIST ESP-NETIF List of interfaces
 * @brief  APIs to enumerate all registered interfaces
 */

/** @addtogroup ESP_NETIF_LIST
 * @{
 */

/**
 * @brief Iterates over list of interfaces. Returns first netif if NULL given as parameter
 *
 * @note This API doesn't lock the list, nor the TCPIP context, as this it's usually required
 * to get atomic access between iteration steps rather that within a single iteration.
 * Therefore it is recommended to iterate over the interfaces inside esp_netif_tcpip_exec()
 *
 * @note This API is deprecated. Please use esp_netif_next_unsafe() directly if all the system
 * interfaces are under your control and you can safely iterate over them.
 * Otherwise, iterate over interfaces using esp_netif_tcpip_exec(), or use esp_netif_find_if()
 * to search in the list of netifs with defined predicate.
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return First netif from the list if supplied parameter is NULL, next one otherwise
 */
esp_netif_t *esp_netif_next(esp_netif_t *esp_netif)
__attribute__((deprecated("use esp_netif_next_unsafe() either directly or via esp_netif_tcpip_exec")));

/**
 * @brief Iterates over list of interfaces without list locking. Returns first netif if NULL given as parameter
 *
 * Used for bulk search loops within TCPIP context, e.g. using esp_netif_tcpip_exec(), or if we're sure
 * that the iteration is safe from our application perspective (e.g. no interface is removed between iterations)
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return First netif from the list if supplied parameter is NULL, next one otherwise
 */
esp_netif_t* esp_netif_next_unsafe(esp_netif_t* esp_netif);

/**
 * @brief Predicate callback for esp_netif_find_if() used to find interface
 *        which meets defined criteria
 */
typedef bool (*esp_netif_find_predicate_t)(esp_netif_t *netif, void *ctx);

/**
 * @brief Return a netif pointer for the first interface that meets criteria defined
 * by the callback
 *
 * @param fn Predicate function returning true for the desired interface
 * @param ctx Context pointer passed to the predicate, typically a descriptor to compare with
 * @return valid netif pointer if found, NULL if not
 */
esp_netif_t *esp_netif_find_if(esp_netif_find_predicate_t fn, void *ctx);

/**
 * @brief Returns number of registered esp_netif objects
 *
 * @return Number of esp_netifs
 */
size_t esp_netif_get_nr_of_ifs(void);

/**
 * @brief increase the reference counter of net stack buffer
 *
 * @param[in]  netstack_buf the net stack buffer
 *
 */
void esp_netif_netstack_buf_ref(void *netstack_buf);

/**
 * @brief free the netstack buffer
 *
 * @param[in]  netstack_buf the net stack buffer
 *
 */
void esp_netif_netstack_buf_free(void *netstack_buf);

/**
 * @}
 */

/** @addtogroup ESP_NETIF_TCPIP_EXEC
 * @{
 */

/**
 * @brief  TCPIP thread safe callback used with esp_netif_tcpip_exec()
 */
typedef esp_err_t (*esp_netif_callback_fn)(void *ctx);

/**
 * @brief Utility to execute the supplied callback in TCP/IP context
 * @param fn Pointer to the callback
 * @param ctx Parameter to the callback
 * @return The error code (esp_err_t) returned by the callback
 */
esp_err_t esp_netif_tcpip_exec(esp_netif_callback_fn fn, void *ctx);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /*  _ESP_NETIF_H_ */
