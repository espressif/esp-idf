/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_NETIF_TYPES_H_
#define _ESP_NETIF_TYPES_H_

#include <stdbool.h>
#include <stdint.h>
#include "esp_event_base.h"
#include "esp_err.h"
#include "esp_netif_ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Definition of ESP-NETIF based errors
 */
#define ESP_ERR_ESP_NETIF_BASE                  0x5000
#define ESP_ERR_ESP_NETIF_INVALID_PARAMS        ESP_ERR_ESP_NETIF_BASE + 0x01
#define ESP_ERR_ESP_NETIF_IF_NOT_READY          ESP_ERR_ESP_NETIF_BASE + 0x02
#define ESP_ERR_ESP_NETIF_DHCPC_START_FAILED    ESP_ERR_ESP_NETIF_BASE + 0x03
#define ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED  ESP_ERR_ESP_NETIF_BASE + 0x04
#define ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED  ESP_ERR_ESP_NETIF_BASE + 0x05
#define ESP_ERR_ESP_NETIF_NO_MEM                ESP_ERR_ESP_NETIF_BASE + 0x06
#define ESP_ERR_ESP_NETIF_DHCP_NOT_STOPPED      ESP_ERR_ESP_NETIF_BASE + 0x07
#define ESP_ERR_ESP_NETIF_DRIVER_ATTACH_FAILED  ESP_ERR_ESP_NETIF_BASE + 0x08
#define ESP_ERR_ESP_NETIF_INIT_FAILED           ESP_ERR_ESP_NETIF_BASE + 0x09
#define ESP_ERR_ESP_NETIF_DNS_NOT_CONFIGURED    ESP_ERR_ESP_NETIF_BASE + 0x0A
#define ESP_ERR_ESP_NETIF_MLD6_FAILED           ESP_ERR_ESP_NETIF_BASE + 0x0B
#define ESP_ERR_ESP_NETIF_IP6_ADDR_FAILED       ESP_ERR_ESP_NETIF_BASE + 0x0C
#define ESP_ERR_ESP_NETIF_DHCPS_START_FAILED    ESP_ERR_ESP_NETIF_BASE + 0x0D


/**
 * @brief Definition of ESP-NETIF bridge control
 */
#define ESP_NETIF_BR_FLOOD      -1
#define ESP_NETIF_BR_DROP        0
#define ESP_NETIF_BR_FDW_CPU    (1ULL << 63)

/** @brief Type of esp_netif_object server */
struct esp_netif_obj;

typedef struct esp_netif_obj esp_netif_t;


/** @brief Type of DNS server */
typedef enum {
    ESP_NETIF_DNS_MAIN= 0,       /**< DNS main server address*/
    ESP_NETIF_DNS_BACKUP,        /**< DNS backup server address (Wi-Fi STA and Ethernet only) */
    ESP_NETIF_DNS_FALLBACK,      /**< DNS fallback server address (Wi-Fi STA and Ethernet only) */
    ESP_NETIF_DNS_MAX
} esp_netif_dns_type_t;

/** @brief DNS server info */
typedef struct {
    esp_ip_addr_t ip; /**< IPV4 address of DNS server */
} esp_netif_dns_info_t;

/** @brief Status of DHCP client or DHCP server */
typedef enum {
    ESP_NETIF_DHCP_INIT = 0,    /**< DHCP client/server is in initial state (not yet started) */
    ESP_NETIF_DHCP_STARTED,     /**< DHCP client/server has been started */
    ESP_NETIF_DHCP_STOPPED,     /**< DHCP client/server has been stopped */
    ESP_NETIF_DHCP_STATUS_MAX
} esp_netif_dhcp_status_t;


/** @brief Mode for DHCP client or DHCP server option functions */
typedef enum{
    ESP_NETIF_OP_START = 0,
    ESP_NETIF_OP_SET,           /**< Set option */
    ESP_NETIF_OP_GET,           /**< Get option */
    ESP_NETIF_OP_MAX
} esp_netif_dhcp_option_mode_t;

/** @brief Supported options for DHCP client or DHCP server */
typedef enum{
    ESP_NETIF_SUBNET_MASK                   = 1,    /**< Network mask */
    ESP_NETIF_DOMAIN_NAME_SERVER            = 6,    /**< Domain name server */
    ESP_NETIF_ROUTER_SOLICITATION_ADDRESS   = 32,   /**< Solicitation router address */
    ESP_NETIF_REQUESTED_IP_ADDRESS          = 50,   /**< Request specific IP address */
    ESP_NETIF_IP_ADDRESS_LEASE_TIME         = 51,   /**< Request IP address lease time */
    ESP_NETIF_IP_REQUEST_RETRY_TIME         = 52,   /**< Request IP address retry counter */
    ESP_NETIF_VENDOR_CLASS_IDENTIFIER       = 60,   /**< Vendor Class Identifier of a DHCP client */
    ESP_NETIF_VENDOR_SPECIFIC_INFO          = 43,   /**< Vendor Specific Information of a DHCP server */
} esp_netif_dhcp_option_id_t;

/** IP event declarations */
typedef enum {
    IP_EVENT_STA_GOT_IP,               /*!< station got IP from connected AP */
    IP_EVENT_STA_LOST_IP,              /*!< station lost IP and the IP is reset to 0 */
    IP_EVENT_AP_STAIPASSIGNED,         /*!< soft-AP assign an IP to a connected station */
    IP_EVENT_GOT_IP6,                  /*!< station or ap or ethernet interface v6IP addr is preferred */
    IP_EVENT_ETH_GOT_IP,               /*!< ethernet got IP from connected AP */
    IP_EVENT_ETH_LOST_IP,              /*!< ethernet lost IP and the IP is reset to 0 */
    IP_EVENT_PPP_GOT_IP,               /*!< PPP interface got IP */
    IP_EVENT_PPP_LOST_IP,              /*!< PPP interface lost IP */
    IP_EVENT_TX_RX,                    /*!< transmitting/receiving data packet */
} ip_event_t;

/** @brief IP event base declaration */
ESP_EVENT_DECLARE_BASE(IP_EVENT);

/** Event structure for IP_EVENT_STA_GOT_IP, IP_EVENT_ETH_GOT_IP events  */

typedef struct {
    esp_ip4_addr_t ip;      /**< Interface IPV4 address */
    esp_ip4_addr_t netmask; /**< Interface IPV4 netmask */
    esp_ip4_addr_t gw;      /**< Interface IPV4 gateway address */
} esp_netif_ip_info_t;

/** @brief IPV6 IP address information
 */
typedef struct {
    esp_ip6_addr_t ip; /**< Interface IPV6 address */
} esp_netif_ip6_info_t;


/**
 * @brief Event structure for IP_EVENT_GOT_IP event
 *
 */
typedef struct {
    esp_netif_t *esp_netif;          /*!< Pointer to corresponding esp-netif object */
    esp_netif_ip_info_t ip_info;     /*!< IP address, netmask, gateway IP address */
    bool ip_changed;                 /*!< Whether the assigned IP has changed or not */
} ip_event_got_ip_t;

/** Event structure for IP_EVENT_GOT_IP6 event */
typedef struct {
    esp_netif_t *esp_netif;          /*!< Pointer to corresponding esp-netif object */
    esp_netif_ip6_info_t ip6_info;   /*!< IPv6 address of the interface */
    int ip_index;                    /*!< IPv6 address index */
} ip_event_got_ip6_t;

/** Event structure for ADD_IP6 event */
typedef struct {
    esp_ip6_addr_t addr;            /*!< The address to be added to the interface */
    bool preferred;                 /*!< The default preference of the address */
} ip_event_add_ip6_t;

/** Event structure for IP_EVENT_AP_STAIPASSIGNED event */
typedef struct {
    esp_netif_t *esp_netif; /*!< Pointer to the associated netif handle */
    esp_ip4_addr_t ip; /*!< IP address which was assigned to the station */
    uint8_t mac[6];    /*!< MAC address of the connected client */
} ip_event_ap_staipassigned_t;

typedef enum {
    ESP_NETIF_TX = 0,  // Data is being transmitted.
    ESP_NETIF_RX = 1,  // Data is being received.
} esp_netif_tx_rx_direction_t;

#ifdef CONFIG_ESP_NETIF_REPORT_DATA_TRAFFIC
/** Event structure for IP_EVENT_TRANSMIT and IP_EVENT_RECEIVE */
typedef struct {
    esp_netif_t *esp_netif; /*!< Pointer to the associated netif handle */
    size_t len;             /*!< Length of the data */
    esp_netif_tx_rx_direction_t dir; /*!< Directions for data transfer >*/
} ip_event_tx_rx_t;
#endif

typedef enum esp_netif_flags {
    ESP_NETIF_DHCP_CLIENT = 1 << 0,
    ESP_NETIF_DHCP_SERVER = 1 << 1,
    ESP_NETIF_FLAG_AUTOUP = 1 << 2,
    ESP_NETIF_FLAG_GARP   = 1 << 3,
    ESP_NETIF_FLAG_EVENT_IP_MODIFIED = 1 << 4,
    ESP_NETIF_FLAG_IS_PPP = 1 << 5,
    ESP_NETIF_FLAG_IS_BRIDGE = 1 << 6,
    ESP_NETIF_FLAG_MLDV6_REPORT = 1 << 7,
} esp_netif_flags_t;

typedef enum esp_netif_ip_event_type {
    ESP_NETIF_IP_EVENT_GOT_IP = 1,
    ESP_NETIF_IP_EVENT_LOST_IP = 2,
} esp_netif_ip_event_type_t;


/** LwIP bridge configuration */
typedef struct bridgeif_config {
    uint16_t max_fdb_dyn_entries; /*!< maximum number of entries in dynamic forwarding database */
    uint16_t max_fdb_sta_entries; /*!< maximum number of entries in static forwarding database */
    uint8_t max_ports;            /*!< maximum number of ports the bridge can consist of */
} bridgeif_config_t;

//
//    ESP-NETIF interface configuration:
//      1) general (behavioral) config (esp_netif_config_t)
//      2) (peripheral) driver specific config (esp_netif_driver_ifconfig_t)
//      3) network stack specific config (esp_netif_net_stack_ifconfig_t) -- no publicly available
//

/**
 * @brief ESP-netif inherent config parameters
 *
 */
typedef struct esp_netif_inherent_config {
    esp_netif_flags_t flags;         /*!< flags that define esp-netif behavior */
    uint8_t mac[6];                  /*!< initial mac address for this interface */
    const esp_netif_ip_info_t* ip_info;    /*!< initial ip address for this interface */
    uint32_t get_ip_event;           /*!< event id to be raised when interface gets an IP */
    uint32_t lost_ip_event;          /*!< event id to be raised when interface losts its IP */
    const char * if_key;             /*!< string identifier of the interface */
    const char * if_desc;            /*!< textual description of the interface */
    int route_prio;                  /*!< numeric priority of this interface to become a default
                                          routing if (if other netifs are up).
                                          A higher value of route_prio indicates
                                          a higher priority */
    bridgeif_config_t *bridge_info;  /*!< LwIP bridge configuration */
} esp_netif_inherent_config_t;

typedef struct esp_netif_config esp_netif_config_t;

/**
 * @brief  IO driver handle type
 */
typedef void * esp_netif_iodriver_handle;

/**
 * @brief ESP-netif driver base handle
 *
 */
typedef struct esp_netif_driver_base_s {
    esp_err_t (*post_attach)(esp_netif_t *netif, esp_netif_iodriver_handle h); /*!< post attach function pointer */
    esp_netif_t *netif; /*!< netif handle */
} esp_netif_driver_base_t;

/**
 * @brief  Specific IO driver configuration
 */
struct esp_netif_driver_ifconfig {
    esp_netif_iodriver_handle handle; /*!< io-driver handle */
    esp_err_t (*transmit)(void *h, void *buffer, size_t len); /*!< transmit function pointer */
    esp_err_t (*transmit_wrap)(void *h, void *buffer, size_t len, void *netstack_buffer); /*!< transmit wrap function pointer */
    void (*driver_free_rx_buffer)(void *h, void* buffer); /*!< free rx buffer function pointer */
};

typedef struct esp_netif_driver_ifconfig esp_netif_driver_ifconfig_t;

/**
 * @brief  Specific L3 network stack configuration
 */

typedef struct esp_netif_netstack_config esp_netif_netstack_config_t;

/**
 * @brief  Generic esp_netif configuration
 */
struct esp_netif_config {
    const esp_netif_inherent_config_t *base; /*!< base config */
    const esp_netif_driver_ifconfig_t *driver; /*!< driver config */
    const esp_netif_netstack_config_t *stack; /*!< stack config */
};

/**
 * @brief DHCP client's addr info (pair of MAC and IP address)
 */
typedef struct {
    uint8_t mac[6];         /**< Clients MAC address */
    esp_ip4_addr_t ip;      /**< Clients IP address */
} esp_netif_pair_mac_ip_t;

/**
 * @brief  ESP-NETIF Receive function type
 */
typedef esp_err_t (*esp_netif_receive_t)(esp_netif_t *esp_netif, void *buffer, size_t len, void *eb);

#ifdef __cplusplus
}
#endif

#endif // _ESP_NETIF_TYPES_H_
