/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "lwip/netif.h"
#include "lwip/esp_netif_net_stack.h"
#ifdef CONFIG_LWIP_DHCPS
#include "dhcpserver/dhcpserver.h"
#endif

struct esp_netif_api_msg_s;

typedef int (*esp_netif_api_fn)(struct esp_netif_api_msg_s *msg);

typedef struct esp_netif_api_msg_s {
    int type;  /**< The first field MUST be int */
    int ret;
    esp_netif_api_fn api_fn;
    union {
        esp_netif_t *esp_netif;         /* esp_netif as input param */
        esp_netif_t **p_esp_netif;      /* esp_netif as output */
        esp_netif_callback_fn user_fn;  /* user callback */
    };              /* Commonly used parameters what calling api_fn */
    void    *data;
} esp_netif_api_msg_t;


typedef struct esp_netif_dns_param_s {
    esp_netif_dns_type_t dns_type;
    esp_netif_dns_info_t *dns_info;
} esp_netif_dns_param_t;

typedef struct esp_netif_ip_lost_timer_s {
    bool timer_running;
} esp_netif_ip_lost_timer_t;

/**
 * @brief Check the netif if of a specific P2P type
 */
#if CONFIG_PPP_SUPPORT
#define ESP_NETIF_IS_POINT2POINT_TYPE(netif, type) (netif->related_data && netif->related_data->is_point2point && netif->related_data->netif_type == type)
#else
#define ESP_NETIF_IS_POINT2POINT_TYPE(netif, type) false
#endif

/**
 * @brief Additional netif types when related data are needed
 */
enum netif_types {
    COMMON_LWIP_NETIF,
    PPP_LWIP_NETIF,
};

/**
 * @brief Related data to esp-netif (additional data for some special types of netif
 * (typically for point-point network types, such as PPP)
 */
typedef struct netif_related_data {
    bool is_point2point;
    enum netif_types netif_type;
} netif_related_data_t;

/**
 * @brief Main esp-netif container with interface related information
 */
struct esp_netif_obj {
    // default interface addresses
    uint8_t mac[NETIF_MAX_HWADDR_LEN];
    esp_netif_ip_info_t* ip_info;
    esp_netif_ip_info_t* ip_info_old;

    // lwip netif related
    struct netif *lwip_netif;
    err_t (*lwip_init_fn)(struct netif*);
    esp_netif_recv_ret_t (*lwip_input_fn)(void *input_netif_handle, void *buffer, size_t len, void *eb);
    void * netif_handle;    // netif impl context (either vanilla lwip-netif or ppp_pcb)
    netif_related_data_t *related_data; // holds additional data for specific netifs
#if ESP_DHCPS
    dhcps_t *dhcps;
#endif
    // io driver related
    void* driver_handle;
    esp_err_t (*driver_transmit)(void *h, void *buffer, size_t len);
    esp_err_t (*driver_transmit_wrap)(void *h, void *buffer, size_t len, void *pbuf);
    void (*driver_free_rx_buffer)(void *h, void* buffer);
    esp_err_t (*driver_set_mac_filter)(void *h, const uint8_t *mac, size_t mac_len, bool add);

    // dhcp related
    esp_netif_dhcp_status_t dhcpc_status;
    esp_netif_dhcp_status_t dhcps_status;
    bool timer_running;

    // event translation
    ip_event_t get_ip_event;
    ip_event_t lost_ip_event;
#ifdef CONFIG_ESP_NETIF_REPORT_DATA_TRAFFIC
    bool tx_rx_events_enabled;
#endif

    // misc flags, types, keys, priority
    esp_netif_flags_t flags;
    char * hostname;
    char * if_key;
    char * if_desc;
    int route_prio;

#if CONFIG_ESP_NETIF_BRIDGE_EN
    // bridge configuration
    uint16_t max_fdb_dyn_entries;
    uint16_t max_fdb_sta_entries;
    uint8_t max_ports;
#endif // CONFIG_ESP_NETIF_BRIDGE_EN
    // mldv6 timer
    bool mldv6_report_timer_started;

#ifdef CONFIG_ESP_NETIF_SET_DNS_PER_DEFAULT_NETIF
    ip_addr_t dns[DNS_MAX_SERVERS];
#endif
};

typedef enum esp_netif_set_default_state {
    ESP_NETIF_UNDEF,
    ESP_NETIF_STARTED,
    ESP_NETIF_GOT_IP,
    ESP_NETIF_STOPPED,
    ESP_NETIF_LOST_IP,
    ESP_NETIF_SET_DEFAULT,
} esp_netif_route_prio_action_t;

esp_err_t esp_netif_update_default_netif(esp_netif_t *esp_netif, esp_netif_route_prio_action_t action);
