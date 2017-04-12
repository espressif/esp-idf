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

/**
 * @brief TCPIP adapter library
 *
 * The aim of this adapter is to provide an abstract layer upon TCPIP stack.
 * With this layer, switch to other TCPIP stack is possible and easy in esp-idf.
 *
 * If users want to use other TCPIP stack, all those functions should be implemented
 * by using the specific APIs of that stack. The macros in CONFIG_TCPIP_LWIP should be
 * re-defined.
 *
 * tcpip_adapter_init should be called in the start of app_main for only once.
 *
 * Currently most adapter APIs are called in event_default_handlers.c.
 *
 * We recommend users only use set/get IP APIs, DHCP server/client APIs,
 * get free station list APIs in application side. Other APIs are used in esp-idf internal,
 * otherwise the state maybe wrong.
 *
 * TODO: ipv6 support will be added, use menuconfig to disable CONFIG_TCPIP_LWIP
 */

#include <stdint.h>
#include "rom/queue.h"
#include "esp_wifi_types.h"

#define CONFIG_TCPIP_LWIP 1
#define CONFIG_DHCP_STA_LIST 1

#if CONFIG_TCPIP_LWIP
#include "lwip/ip_addr.h"
#include "apps/dhcpserver.h"

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

typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} tcpip_adapter_ip_info_t;

typedef struct {
    ip6_addr_t ip;
} tcpip_adapter_ip6_info_t;

typedef dhcps_lease_t tcpip_adapter_dhcps_lease_t;

#if CONFIG_DHCP_STA_LIST 
typedef struct {
    uint8_t mac[6];
    ip4_addr_t ip;
} tcpip_adapter_sta_info_t;

typedef struct {
    tcpip_adapter_sta_info_t sta[ESP_WIFI_MAX_CONN_NUM];
    int num;
} tcpip_adapter_sta_list_t;
#endif

#endif

#define ESP_ERR_TCPIP_ADAPTER_BASE      0x5000      // TODO: move base address to esp_err.h

#define ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS        ESP_ERR_TCPIP_ADAPTER_BASE + 0x00
#define ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY          ESP_ERR_TCPIP_ADAPTER_BASE + 0x01
#define ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED    ESP_ERR_TCPIP_ADAPTER_BASE + 0x02
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED  ESP_ERR_TCPIP_ADAPTER_BASE + 0x03
#define ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED  ESP_ERR_TCPIP_ADAPTER_BASE + 0x04
#define ESP_ERR_TCPIP_ADAPTER_NO_MEM                ESP_ERR_TCPIP_ADAPTER_BASE + 0x05
#define ESP_ERR_TCPIP_ADAPTER_DHCP_NOT_STOPPED      ESP_ERR_TCPIP_ADAPTER_BASE + 0x06

/* TODO: add Ethernet interface */
typedef enum {
    TCPIP_ADAPTER_IF_STA = 0,     /**< ESP32 station interface */
    TCPIP_ADAPTER_IF_AP,          /**< ESP32 soft-AP interface */
    TCPIP_ADAPTER_IF_ETH,         /**< ESP32 ethernet interface */
    TCPIP_ADAPTER_IF_MAX
} tcpip_adapter_if_t;

/* status of DHCP client or DHCP server */
typedef enum {
    TCPIP_ADAPTER_DHCP_INIT = 0,    /**< DHCP client/server in initial state */
    TCPIP_ADAPTER_DHCP_STARTED,     /**< DHCP client/server already been started */
    TCPIP_ADAPTER_DHCP_STOPPED,     /**< DHCP client/server already been stopped */
    TCPIP_ADAPTER_DHCP_STATUS_MAX
} tcpip_adapter_dhcp_status_t;

/* set the option mode for DHCP client or DHCP server */
typedef enum{
    TCPIP_ADAPTER_OP_START = 0,
    TCPIP_ADAPTER_OP_SET,           /**< set option mode */
    TCPIP_ADAPTER_OP_GET,           /**< get option mode */
    TCPIP_ADAPTER_OP_MAX
} tcpip_adapter_option_mode_t;

typedef enum{
    TCPIP_ADAPTER_ROUTER_SOLICITATION_ADDRESS   = 32,   /**< solicitation router address */
    TCPIP_ADAPTER_REQUESTED_IP_ADDRESS          = 50,   /**< request IP address pool */
    TCPIP_ADAPTER_IP_ADDRESS_LEASE_TIME         = 51,   /**< request IP address lease time */
    TCPIP_ADAPTER_IP_REQUEST_RETRY_TIME         = 52,   /**< request IP address retry counter */
} tcpip_adapter_option_id_t;

struct tcpip_adapter_api_msg_s;
typedef int (*tcpip_adapter_api_fn)(struct tcpip_adapter_api_msg_s *msg);
typedef struct tcpip_adapter_api_msg_s {
    int type;  /**< The first field MUST be int */
    int ret;
    tcpip_adapter_api_fn api_fn;
    tcpip_adapter_if_t tcpip_if;
    tcpip_adapter_ip_info_t *ip_info;
    uint8_t *mac;
    const char *hostname;
} tcpip_adapter_api_msg_t;

#define TCPIP_ADAPTER_TRHEAD_SAFE 1
#define TCPIP_ADAPTER_IPC_LOCAL   0 
#define TCPIP_ADAPTER_IPC_REMOTE  1

#define TCPIP_ADAPTER_IPC_CALL(_if, _mac, _ip, _hostname, _fn) do {\
    tcpip_adapter_api_msg_t msg;\
    memset(&msg, 0, sizeof(msg));\
    msg.tcpip_if = (_if);\
    msg.mac      = (_mac);\
    msg.ip_info  = (_ip);\
    msg.hostname = (_hostname);\
    msg.api_fn   = (_fn);\
    if (TCPIP_ADAPTER_IPC_REMOTE == tcpip_adapter_ipc_check(&msg)) {\
        ESP_LOGD(TAG, "check: remote, if=%d fn=%p\n", (_if), (_fn));\
        return msg.ret;\
    } else {\
        ESP_LOGD(TAG, "check: local, if=%d fn=%p\n", (_if), (_fn));\
    }\
}while(0)


/**
 * @brief  Initialize tcpip adapter
 *
 * This will initialize TCPIP stack inside.
 */
void tcpip_adapter_init(void);

/**
 * @brief  Start an interface with specific MAC and IP
 *
 * softAP or station interface will be initialized, connect WiFi stack with TCPIP stack.
 *
 * For softAP interface, DHCP server will be started automatically.
 *
 * @param[in]  tcpip_if: the interface which we will start
 * @param[in]  mac: set MAC address of this interface
 * @param[in]  ip_info: set IP address of this interface
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         ESP_ERR_NO_MEM
 */
esp_err_t tcpip_adapter_start(tcpip_adapter_if_t tcpip_if, uint8_t *mac, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  Stop an interface
 *
 * The interface will be cleanup in this API, if DHCP server/client are started, will be stopped.
 *
 * @param[in]  tcpip_if: the interface which will be started
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Bring up an interface
 *
 * Only station interface need to be brought up, since station interface will be shut down when disconnect.
 *
 * @param[in]  tcpip_if: the interface which will be up
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Shut down an interface
 *
 * Only station interface need to be shut down, since station interface will be brought up when connect.
 *
 * @param[in]  tcpip_if: the interface which will be down
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Get interface's IP information
 *
 * There has an IP information copy in adapter library, if interface is up, get IP information from
 * interface, otherwise get from copy.
 *
 * @param[in]   tcpip_if: the interface which we want to get IP information
 * @param[out]  ip_info: If successful, IP information will be returned in this argument.
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  Set interface's IP information
 *
 * There has an IP information copy in adapter library, if interface is up, also set interface's IP.
 * DHCP client/server should be stopped before set new IP information.
 *
 * This function is mainly used for setting static IP.
 *
 * @param[in]  tcpip_if: the interface which we want to set IP information
 * @param[in]  ip_info: If successful, IP information will be returned in this argument.
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info);

/**
 * @brief  create interface's linklocal IPv6 information
 *
 * @note this function will create a linklocal IPv6 address about input interface,
 *       if this address status changed to preferred, will call event call back ,
 *       notify user linklocal IPv6 address has been verified
 *
 * @param[in]  tcpip_if: the interface which we want to set IP information
 *
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_create_ip6_linklocal(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  get interface's linkloacl IPv6 information
 *
 * There has an IPv6 information copy in adapter library, if interface is up,and IPv6 info
 * is preferred,it will get IPv6 linklocal IP successfully
 *
 * @param[in]  tcpip_if: the interface which we want to set IP information
 * @param[in]  if_ip6: If successful, IPv6 information will be returned in this argument.
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_get_ip6_linklocal(tcpip_adapter_if_t tcpip_if, ip6_addr_t *if_ip6);

#if 0
esp_err_t tcpip_adapter_get_mac(tcpip_adapter_if_t tcpip_if, uint8_t *mac);

esp_err_t tcpip_adapter_set_mac(tcpip_adapter_if_t tcpip_if, uint8_t *mac);
#endif

/**
 * @brief  Get DHCP server's status
 *
 * @param[in]   tcpip_if: the interface which we will get status of DHCP server
 * @param[out]  status: If successful, the status of DHCP server will be return in this argument.
 *
 * @return ESP_OK
 */
esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);

/**
 * @brief  Set or Get DHCP server's option
 *
 * @param[in]  opt_op: option operate type, 1 for SET, 2 for GET.
 * @param[in]  opt_id: option index, 32 for ROUTER, 50 for IP POLL, 51 for LEASE TIME, 52 for REQUEST TIME
 * @param[in]  opt_val: option parameter
 * @param[in]  opt_len: option length
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED
 *         ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED
 */
esp_err_t tcpip_adapter_dhcps_option(tcpip_adapter_option_mode_t opt_op, tcpip_adapter_option_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief  Start DHCP server
 *
 * @note   Currently DHCP server is bind to softAP interface.
 *
 * @param[in]  tcpip_if: the interface which we will start DHCP server
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED
 */
esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Stop DHCP server
 *
 * @note   Currently DHCP server is bind to softAP interface.
 *
 * @param[in]  tcpip_if: the interface which we will stop DHCP server
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPED
 *         ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Get DHCP client status
 *
 * @param[in]  tcpip_if: the interface which we will get status of DHCP client
 * @param[out]  status: If successful, the status of DHCP client will be return in this argument.
 *
 * @return ESP_OK
 */
esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status);

/**
 * @brief  Set or Get DHCP client's option
 *
 * @note   This function is not implement now.
 *
 * @param[in]  opt_op: option operate type, 1 for SET, 2 for GET.
 * @param[in]  opt_id: option index, 32 for ROUTER, 50 for IP POLL, 51 for LEASE TIME, 52 for REQUEST TIME
 * @param[in]  opt_val: option parameter
 * @param[in]  opt_len: option length
 *
 * @return ESP_OK
 */
esp_err_t tcpip_adapter_dhcpc_option(tcpip_adapter_option_mode_t opt_op, tcpip_adapter_option_id_t opt_id, void *opt_val, uint32_t opt_len);

/**
 * @brief  Start DHCP client
 *
 * @note   Currently DHCP client is bind to station interface.
 *
 * @param[in]  tcpip_if: the interface which we will start DHCP client
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED
 *         ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED
 */
esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if);

/**
 * @brief  Stop DHCP client
 *
 * @note   Currently DHCP client is bind to station interface.
 *
 * @param[in]  tcpip_if: the interface which we will stop DHCP client
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 *         ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPED
 *         ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY
 */
esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if);



esp_err_t tcpip_adapter_eth_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief  Get data from station interface
 *
 * This function should be installed by esp_wifi_reg_rxcb, so WiFi packets will be forward to TCPIP stack.
 *
 * @param[in]  void *buffer: the received data point
 * @param[in]  uint16_t len: the received data length
 * @param[in]  void *eb: parameter
 *
 * @return ESP_OK
 */
esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief  Get data from softAP interface
 *
 * This function should be installed by esp_wifi_reg_rxcb, so WiFi packets will be forward to TCPIP stack.
 *
 * @param[in]  void *buffer: the received data point
 * @param[in]  uint16_t len: the received data length
 * @param[in]  void *eb: parameter
 *
 * @return ESP_OK
 */
esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void *eb);

/**
 * @brief  Get WiFi interface index
 *
 * Get WiFi interface from TCPIP interface struct pointer.
 *
 * @param[in]  void *dev: adapter interface
 *
 * @return ESP_IF_WIFI_STA
 *         ESP_IF_WIFI_AP
           ESP_IF_ETH
 *         ESP_IF_MAX
 */
esp_interface_t tcpip_adapter_get_esp_if(void *dev);

/**
 * @brief  Get the station information list
 *
 * @param[in]   wifi_sta_list_t *wifi_sta_list: station list info
 * @param[out]  tcpip_adapter_sta_list_t *tcpip_sta_list: station list info
 *
 * @return ESP_OK
 *         ESP_ERR_TCPIP_ADAPTER_NO_MEM
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS
 */
esp_err_t tcpip_adapter_get_sta_list(wifi_sta_list_t *wifi_sta_list, tcpip_adapter_sta_list_t *tcpip_sta_list);

#define TCPIP_HOSTNAME_MAX_SIZE    32
/**
 * @brief  Set the hostname to the interface
 *
 * @param[in]   tcpip_if: the interface which we will set the hostname
 * @param[in]   hostname: the host name for set the interface, the max length of hostname is 32 bytes
 *
 * @return ESP_OK:success
 *         ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY:interface status error
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS:parameter error
 */
esp_err_t tcpip_adapter_set_hostname(tcpip_adapter_if_t tcpip_if, const char *hostname);

/**
 * @brief  Get the hostname from the interface
 *
 * @param[in]   tcpip_if: the interface which we will get the hostname
 * @param[in]   hostname: the host name from the interface
 *
 * @return ESP_OK:success
 *         ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY:interface status error
 *         ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS:parameter error
 */
esp_err_t tcpip_adapter_get_hostname(tcpip_adapter_if_t tcpip_if, const char **hostname);

#ifdef __cplusplus
}
#endif

#endif /*  _TCPIP_ADAPTER_H_ */

