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

#ifndef _TCPIP_ADAPTER_H_
#define _TCPIP_ADAPTER_H_

#include <stdint.h>
#include "rom/queue.h"
#include "esp_wifi_types.h"
#include "sdkconfig.h"

#if CONFIG_TCPIP_LWIP
#include "lwip/ip_addr.h"
#include "dhcpserver/dhcpserver.h"

typedef dhcps_lease_t tcpip_adapter_dhcps_lease_t;

#ifdef __cplusplus
extern "C" {
#endif

#define IP2STR(ipaddr) ip4_addr1_16(ipaddr), \
    ip4_addr2_16(ipaddr), \
    ip4_addr3_16(ipaddr), \
    ip4_addr4_16(ipaddr)

#define IPSTR "%d.%d.%d.%d"

#define IPV62STR(ipaddr) IP6_ADDR_BLOCK1(&(ipaddr)),     \
    IP6_ADDR_BLOCK2(&(ipaddr)),     \
    IP6_ADDR_BLOCK3(&(ipaddr)),     \
    IP6_ADDR_BLOCK4(&(ipaddr)),     \
    IP6_ADDR_BLOCK5(&(ipaddr)),     \
    IP6_ADDR_BLOCK6(&(ipaddr)),     \
    IP6_ADDR_BLOCK7(&(ipaddr)),     \
    IP6_ADDR_BLOCK8(&(ipaddr))

#define IPV6STR "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"

/** @brief IPV4 IP address information
 */
typedef struct {
    ip4_addr_t ip;      /**< Interface IPV4 address */
    ip4_addr_t netmask; /**< Interface IPV4 netmask */
    ip4_addr_t gw;      /**< Interface IPV4 gateway address */
} tcpip_adapter_ip_info_t;

/** @brief IPV6 IP address information
 */
typedef struct {
    ip6_addr_t ip; /**< Interface IPV6 address */
} tcpip_adapter_ip6_info_t;

/** @brief IP address info of station connected to WLAN AP
 *
 * @note See also wifi_sta_info_t (MAC layer information only)
 */
typedef struct {
    uint8_t mac[6]; /**< Station MAC address */
    ip4_addr_t ip;  /**< Station assigned IP address */
} tcpip_adapter_sta_info_t;

/** @brief WLAN AP: Connected stations list
 *
 * Used to retrieve IP address information about connected stations.
 */
typedef struct {
    tcpip_adapter_sta_info_t sta[ESP_WIFI_MAX_CONN_NUM]; /**< Connected stations */
    int num; /**< Number of connected stations */
} tcpip_adapter_sta_list_t;

#endif

#define ESP_ERR_TCPIP_ADAPTER_BASE                  0x5000
#define ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS        ESP_ERR_TCPIP_ADAPTER_BASE + 0x01
#define ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY          ESP_ERR_TCPIP_ADAPTER_BASE + 0x02
#define ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED    ESP_ERR_TCPIP_ADAPTER_BASE + 0x03
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED  ESP_ERR_TCPIP_ADAPTER_BASE + 0x04
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED  ESP_ERR_TCPIP_ADAPTER_BASE + 0x05
#define ESP_ERR_TCPIP_ADAPTER_NO_MEM                ESP_ERR_TCPIP_ADAPTER_BASE + 0x06
#define ESP_ERR_TCPIP_ADAPTER_DHCP_NOT_STOPPED      ESP_ERR_TCPIP_ADAPTER_BASE + 0x07

/* @brief On-chip network interfaces */
typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< Wi-Fi STA (station) interface */
    TCPIP_ADAPTER_IF_AP,          /**< Wi-Fi soft-AP interface */
    TCPIP_ADAPTER_IF_ETH,         /**< Ethernet interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

/** @brief Type of DNS server */
typedef enum {
    TCPIP_ADAPTER_DNS_MAIN= 0,       /**< DNS main server address*/
    TCPIP_ADAPTER_DNS_BACKUP,        /**< DNS backup server address (Wi-Fi STA and Ethernet only) */
    TCPIP_ADAPTER_DNS_FALLBACK,      /**< DNS fallback server address (Wi-Fi STA and Ethernet only) */
    TCPIP_ADAPTER_DNS_MAX
} tcpip_adapter_dns_type_t;

/** @brief DNS server info */
typedef struct {
    ip_addr_t ip; /**< IPV4 address of DNS server */
} tcpip_adapter_dns_info_t;

/** @brief Status of DHCP client or DHCP server */
typedef enum {
    TCPIP_ADAPTER_DHCP_INIT = 0,    /**< DHCP client/server is in initial state (not yet started) */
    TCPIP_ADAPTER_DHCP_STARTED,     /**< DHCP client/server has been started */
    TCPIP_ADAPTER_DHCP_STOPPED,     /**< DHCP client/server has been stopped */
    TCPIP_ADAPTER_DHCP_STATUS_MAX
} tcpip_adapter_dhcp_status_t;

/** @brief Mode for DHCP client or DHCP server option functions */
typedef enum{
    TCPIP_ADAPTER_OP_START = 0,
    TCPIP_ADAPTER_OP_SET,           /**< Set option */
    TCPIP_ADAPTER_OP_GET,           /**< Get option */
    TCPIP_ADAPTER_OP_MAX
} tcpip_adapter_dhcp_option_mode_t;

/* Deprecated name for tcpip_adapter_dhcp_option_mode_t, to remove after ESP-IDF V4.0 */
typedef tcpip_adapter_dhcp_option_mode_t tcpip_adapter_option_mode_t;

/** @brief Supported options for DHCP client or DHCP server */
typedef enum{
    TCPIP_ADAPTER_DOMAIN_NAME_SERVER            = 6,    /**< Domain name server */
    TCPIP_ADAPTER_ROUTER_SOLICITATION_ADDRESS   = 32,   /**< Solicitation router address */
    TCPIP_ADAPTER_REQUESTED_IP_ADDRESS          = 50,   /**< Request specific IP address */
    TCPIP_ADAPTER_IP_ADDRESS_LEASE_TIME         = 51,   /**< Request IP address lease time */
    TCPIP_ADAPTER_IP_REQUEST_RETRY_TIME         = 52,   /**< Request IP address retry counter */
} tcpip_adapter_dhcp_option_id_t;

/* Deprecated name for tcpip_adapter_dhcp_option_id_t, to remove after ESP-IDF V4.0 */
typedef tcpip_adapter_dhcp_option_id_t tcpip_adapter_option_id_t;

/**
 * @brief  Initialize the underlying TCP/IP stack
 *
 * @note This function should be called exactly once from application code, when the application starts up.
 */
void tcpip_adapter_init(void);

/**
 * @brief  Cause the TCP/IP stack to start the Ethernet interface with specified MAC and IP
 *
 * @note This function should be called after the Ethernet MAC hardware is initialized. In the default configuration, application code does not need to call this function - it is called automatically by the default handler for the SYSTEM_EVENT_ETH_START event.
 *
 * @param[in]  mac Set MAC address of this interface
 * @param[in]  ip_info Set IP address of this interface
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         - ESP_ERR_NO_MEM
 */
esp_err_t tcpip_adapter_eth_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  Cause the TCP/IP stack to start the Wi-Fi station interface with specified MAC and IP
 *
 *
 * @note This function should be called after the Wi-Fi Station hardware is initialized. In the default configuration, application code does not need to call this function - it is called automatically by the default handler for the SYSTEM_EVENT_STA_START event.
 *
 * @param[in]  mac Set MAC address of this interface
 * @param[in]  ip_info Set IP address of this interface
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         - ESP_ERR_NO_MEM
 */
esp_err_t tcpip_adapter_sta_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  Cause the TCP/IP stack to start the Wi-Fi AP interface with specified MAC and IP
 *
 * @note This function should be called after the Wi-Fi AP hardware is initialized. In the default configuration, application code does not need to call this function - it is called automatically by the default handler for the SYSTEM_EVENT_AP_START event.
 *
 * DHCP server will be started automatically when this function is called.
 *
 * @param[in]  mac Set MAC address of this interface
 * @param[in]  ip_info Set IP address of this interface
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         - ESP_ERR_NO_MEM
 */
esp_err_t tcpip_adapter_ap_start(uint8_t *mac, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  Cause the TCP/IP stack to stop a network interface
 *
 * Causes TCP/IP stack to clean up this interface. This includes stopping the DHCP server or client, if they are started.
 *
 * @note This API is called by the default Wi-Fi and Ethernet event handlers if the underlying network driver reports that the
 * interface has stopped.
 *
 * @note To stop an interface from application code, call the network-specific API (esp_wifi_stop() or esp_eth_stop()).
 * The driver layer will then send a stop event and the event handler should call this API.
 * Otherwise, the driver and MAC layer will remain started.
 *
 * @param[in]  tcpip_if Interface which will be stopped
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         - ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Cause the TCP/IP stack to bring up an interface
 *
 * @note This function is called automatically by the default event handlers for the Wi-Fi Station and Ethernet interfaces,
 * in response to the SYSTEM_EVENT_STA_CONNECTED and SYSTEM_EVENT_ETH_CONNECTED events, respectively.
 *
 * @note This function is not normally used with Wi-Fi AP interface. If the AP interface is started, it is up.
 *
 * @param[in]  tcpip_if Interface to bring up
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if);

/**
 * @brief Cause the TCP/IP stack to shutdown an interface
 *
 * @note This function is called automatically by the default event handlers for the Wi-Fi Station and Ethernet interfaces,
 * in response to the SYSTEM_EVENT_STA_DISCONNECTED and SYSTEM_EVENT_ETH_DISCONNECTED events, respectively.
 *
 * @note This function is not normally used with Wi-Fi AP interface. If the AP interface is stopped, it is down.
 *
 * @param[in]  tcpip_if Interface to shutdown
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Get interface's IP address information
 *
 * If the interface is up, IP information is read directly from the TCP/IP stack.
 *
 * If the interface is down, IP information is read from a copy kept in the TCP/IP adapter
 * library itself.
 *
 * @param[in]   tcpip_if Interface to get IP information
 * @param[out]  ip_info If successful, IP information will be returned in this argument.
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  Set interface's IP address information
 *
 * This function is mainly used to set a static IP on an interface.
 *
 * If the interface is up, the new IP information is set directly in the TCP/IP stack.
 *
 * The copy of IP information kept in the TCP/IP adapter library is also updated (this
 * copy is returned if the IP is queried while the interface is still down.)
 *
 * @note DHCP client/server must be stopped before setting new IP information.
 *
 * @note Calling this interface for the Wi-Fi STA or Ethernet interfaces may generate a
 * SYSTEM_EVENT_STA_GOT_IP or SYSTEM_EVENT_ETH_GOT_IP event.
 *
 * @param[in] tcpip_if Interface to set IP information
 * @param[in] ip_info IP information to set on the specified interface
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *      - ESP_ERR_TCPIP_ADAPTER_DHCP_NOT_STOPPED If DHCP server or client is still running
 */
esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, const tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  Set DNS Server information
 *
 * This function behaves differently for different interfaces:
 *
 * - For Wi-Fi Station interface and Ethernet interface, up to three types of DNS server can be set (in order of priority):
 *   - Main DNS Server (TCPIP_ADAPTER_DNS_MAIN)
 *   - Backup DNS Server (TCPIP_ADAPTER_DNS_BACKUP)
 *   - Fallback DNS Server (TCPIP_ADAPTER_DNS_FALLBACK)
 *
 *   If DHCP client is enabled, main and backup DNS servers will be updated automatically from the DHCP lease if the relevant DHCP options are set. Fallback DNS Server is never updated from the DHCP lease and is designed to be set via this API.
 *
 *   If DHCP client is disabled, all DNS server types can be set via this API only.
 *
 * - For Wi-Fi AP interface, the Main DNS Server setting is used by the DHCP server to provide a DNS Server option to DHCP clients (Wi-Fi stations).
 *   - The default Main DNS server is the IP of the Wi-Fi AP interface itself.
 *   - This function can override it by setting server type TCPIP_ADAPTER_DNS_MAIN.
 *   - Other DNS Server types are not supported for the Wi-Fi AP interface.
 *
 * @param[in]  tcpip_if Interface to set DNS Server information
 * @param[in]  type Type of DNS Server to set: TCPIP_ADAPTER_DNS_MAIN, TCPIP_ADAPTER_DNS_BACKUP, TCPIP_ADAPTER_DNS_FALLBACK
 * @param[in]  dns  DNS Server address to set
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS invalid params
 */
esp_err_t tcpip_adapter_set_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns);

/**
 * @brief  Get DNS Server information
 *
 * Return the currently configured DNS Server address for the specified interface and Server type.
 *
 * This may be result of a previous call to tcpip_adapter_set_dns_info(). If the interface's DHCP client is enabled,
 * the Main or Backup DNS Server may be set by the current DHCP lease.
 *
 * @param[in]  tcpip_if Interface to get DNS Server information
 * @param[in]  type Type of DNS Server to get: TCPIP_ADAPTER_DNS_MAIN, TCPIP_ADAPTER_DNS_BACKUP, TCPIP_ADAPTER_DNS_FALLBACK
 * @param[out] dns  DNS Server result is written here on success
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS invalid params
 */
esp_err_t tcpip_adapter_get_dns_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dns_type_t type, tcpip_adapter_dns_info_t *dns);

/**
 * @brief  Get interface's old IP information
 *
 * Returns an "old" IP address previously stored for the interface when the valid IP changed.
 *
 * If the IP lost timer has expired (meaning the interface was down for longer than the configured interval)
 * then the old IP information will be zero.
 *
 * @param[in]   tcpip_if Interface to get old IP information
 * @param[out]  ip_info If successful, IP information will be returned in this argument.
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_get_old_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  Set interface old IP information
 *
 * This function is called from the DHCP client for the Wi-Fi STA and Ethernet interfaces, before a new IP is set. It is also called from the default handlers for the SYSTEM_EVENT_STA_CONNECTED and SYSTEM_EVENT_ETH_CONNECTED events.
 *
 * Calling this function stores the previously configured IP, which can be used to determine if the IP changes in the future.
 *
 * If the interface is disconnected or down for too long, the "IP lost timer" will expire (after the configured interval) and set the old IP information to zero.
 *
 * @param[in]  tcpip_if Interface to set old IP information
 * @param[in]  ip_info Store the old IP information for the specified interface
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_set_old_ip_info(tcpip_adapter_if_t tcpip_if, const tcpip_adapter_ip_info_t *ip_info);


/**
 * @brief  Create interface link-local IPv6 address
 *
 * Cause the TCP/IP stack to create a link-local IPv6 address for the specified interface.
 *
 * This function also registers a callback for the specified interface, so that if the link-local address becomes verified as the preferred address then a SYSTEM_EVENT_GOT_IP6 event will be sent.
 *
 * @param[in]  tcpip_if Interface to create a link-local IPv6 address
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_create_ip6_linklocal(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Get interface link-local IPv6 address
 *
 * If the specified interface is up and a preferred link-local IPv6 address
 * has been created for the interface, return a copy of it.
 *
 * @param[in]  tcpip_if Interface to get link-local IPv6 address
 * @param[out] if_ip6 IPv6 information will be returned in this argument if successful.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL If interface is down, does not have a link-local IPv6 address, or the link-local IPv6 address is not a preferred address.
 */
esp_err_t tcpip_adapter_get_ip6_linklocal(tcpip_adapter_if_t tcpip_if, ip6_addr_t *if_ip6);

#if 0
esp_err_t tcpip_adapter_get_mac(tcpip_adapter_if_t tcpip_if, uint8_t *mac);

esp_err_t tcpip_adapter_set_mac(tcpip_adapter_if_t tcpip_if, uint8_t *mac);
#endif

/**
 * @brief  Get DHCP Server status
 *
 * @param[in]   tcpip_if Interface to get status of DHCP server.
 * @param[out]  status If successful, the status of the DHCP server will be returned in this argument.
 *
 * @return
 *         - ESP_OK
 */
esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);

/**
 * @brief  Set or Get DHCP server option
 *
 * @param[in] opt_op TCPIP_ADAPTER_OP_SET to set an option, TCPIP_ADAPTER_OP_GET to get an option.
 * @param[in] opt_id Option index to get or set, must be one of the supported enum values.
 * @param[inout] opt_val Pointer to the option parameter.
 * @param[in] opt_len Length of the option parameter.
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         - ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED
 *         - ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED
 */
esp_err_t tcpip_adapter_dhcps_option(tcpip_adapter_dhcp_option_mode_t opt_op, tcpip_adapter_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief  Start DHCP server
 *
 * @note   Currently DHCP server is only supported on the Wi-Fi AP interface.
 *
 * @param[in]  tcpip_if Interface to start DHCP server. Must be TCPIP_ADAPTER_IF_AP.
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         - ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED
 */
esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Stop DHCP server
 *
 * @note   Currently DHCP server is only supported on the Wi-Fi AP interface.
 *
 * @param[in]  tcpip_if Interface to stop DHCP server. Must be TCPIP_ADAPTER_IF_AP.
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *      - ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED
 *      - ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Get DHCP client status
 *
 * @param[in]  tcpip_if Interface to get status of DHCP client
 * @param[out] status If successful, the status of DHCP client will be returned in this argument.
 *
 * @return
 *         - ESP_OK
 */
esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);

/**
 * @brief  Set or Get DHCP client's option
 *
 * @note This function is not yet implemented
 *
 * @param[in] opt_op TCPIP_ADAPTER_OP_SET to set an option, TCPIP_ADAPTER_OP_GET to get an option.
 * @param[in] opt_id Option index to get or set, must be one of the supported enum values.
 * @param[inout] opt_val Pointer to the option parameter.
 * @param[in] opt_len Length of the option parameter.
 *
 * @return
 *         - ESP_ERR_NOT_SUPPORTED (not implemented)
 */
esp_err_t tcpip_adapter_dhcpc_option(tcpip_adapter_dhcp_option_mode_t opt_op, tcpip_adapter_dhcp_option_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief Start DHCP client
 *
 * @note DHCP Client is only supported for the Wi-Fi station and Ethernet interfaces.
 *
 * @note The default event handlers for the SYSTEM_EVENT_STA_CONNECTED and SYSTEM_EVENT_ETH_CONNECTED events call this function.
 *
 * @param[in]  tcpip_if Interface to start the DHCP client
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         - ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED
 *         - ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED
 */
esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Stop DHCP client
 *
 * @note DHCP Client is only supported for the Wi-Fi station and Ethernet interfaces.
 *
 * @note Calling tcpip_adapter_stop() or tcpip_adapter_down() will also stop the DHCP Client if it is running.
 *
 * @param[in] tcpip_if Interface to stop the DHCP client
 *
 * @return
 *      - ESP_OK
 *      - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *      - ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED
 *      - ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Receive an Ethernet frame from the Ethernet interface
 *
 * This function will automatically be installed by esp_eth_init(). The Ethernet driver layer will then call this function to forward frames to the TCP/IP stack.
 *
 * @note Application code does not usually need to use this function directly.
 *
 * @param[in]  buffer Received data
 * @param[in]  len Length of the data frame
 * @param[in]  eb Pointer to internal Wi-Fi buffer (ignored for Ethernet)
 *
 * @return
 *         - ESP_OK
 */
esp_err_t tcpip_adapter_eth_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief  Receive an 802.11 data frame from the Wi-Fi Station interface
 *
 * This function should be installed by calling esp_wifi_reg_rxcb(). The Wi-Fi driver layer will then call this function to forward frames to the TCP/IP stack.
 *
 * @note Installation happens automatically in the default handler for the SYSTEM_EVENT_STA_CONNECTED event.
 *
 * @note Application code does not usually need to call this function directly.
 *
 * @param[in]  buffer Received data
 * @param[in]  len Length of the data frame
 * @param[in]  eb Pointer to internal Wi-Fi buffer
 *
 * @return
 *         - ESP_OK
 */
esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief  Receive an 802.11 data frame from the Wi-Fi AP interface
 *
 * This function should be installed by calling esp_wifi_reg_rxcb(). The Wi-Fi driver layer will then call this function to forward frames to the TCP/IP stack.
 *
 * @note Installation happens automatically in the default handler for the SYSTEM_EVENT_AP_START event.
 *
 * @note Application code does not usually need to call this function directly.
 *
 * @param[in]  buffer Received data
 * @param[in]  len Length of the data frame
 * @param[in]  eb Pointer to internal Wi-Fi buffer
 *
 * @return
 *         - ESP_OK
 */
esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief  Get network interface index
 *
 * Get network interface from TCP/IP implementation-specific interface pointer.
 *
 * @param[in]  dev Implementation-specific TCP/IP stack interface pointer.
 *
 * @return
 *         - ESP_IF_WIFI_STA
 *         - ESP_IF_WIFI_AP
 *         - ESP_IF_ETH
 *         - ESP_IF_MAX - invalid parameter
 */
esp_interface_t tcpip_adapter_get_esp_if(void *dev);

/**
 * @brief  Get IP information for stations connected to the Wi-Fi AP interface
 *
 * @param[in]   wifi_sta_list Wi-Fi station info list, returned from esp_wifi_ap_get_sta_list()
 * @param[out]  tcpip_sta_list IP layer station info list, corresponding to MAC addresses provided in wifi_sta_list
 *
 * @return
 *         - ESP_OK
 *         - ESP_ERR_TCPIP_ADAPTER_NO_MEM
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_get_sta_list(const wifi_sta_list_t *wifi_sta_list, tcpip_adapter_sta_list_t *tcpip_sta_list);

#define TCPIP_HOSTNAME_MAX_SIZE    32
/**
 * @brief  Set the hostname of an interface
 *
 * @param[in]   tcpip_if Interface to set the hostname
 * @param[in]   hostname New hostname for the interface. Maximum length 32 bytes.
 *
 * @return
 *         - ESP_OK - success
 *         - ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY - interface status error
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS - parameter error
 */
esp_err_t tcpip_adapter_set_hostname(tcpip_adapter_if_t tcpip_if, const char *hostname);

/**
 * @brief  Get interface hostname.
 *
 * @param[in]   tcpip_if Interface to get the hostname
 * @param[out]   hostname Returns a pointer to the hostname. May be NULL if no hostname is set. If set non-NULL, pointer remains valid (and string may change if the hostname changes).
 *
 * @return
 *         - ESP_OK - success
 *         - ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY - interface status error
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS - parameter error
 */
esp_err_t tcpip_adapter_get_hostname(tcpip_adapter_if_t tcpip_if, const char **hostname);

/**
 * @brief  Get the TCP/IP stack-specific interface that is assigned to a given interface
 *
 * @note For lwIP, this returns a pointer to a netif structure.
 *
 * @param[in]  tcpip_if Interface to get the implementation-specific interface
 * @param[out] netif Pointer to the implementation-specific interface
 *
 * @return
 *         - ESP_OK - success
 *         - ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY - interface status error
 *         - ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS - parameter error
 */
esp_err_t tcpip_adapter_get_netif(tcpip_adapter_if_t tcpip_if, void ** netif);

/**
 * @brief  Test if supplied interface is up or down
 *
 * @param[in]   tcpip_if Interface to test up/down status
 *
 * @return
 *         - true - Interface is up
 *         - false - Interface is down
 */
bool tcpip_adapter_is_netif_up(tcpip_adapter_if_t tcpip_if);

#ifdef __cplusplus
}
#endif

#endif /*  _TCPIP_ADAPTER_H_ */
