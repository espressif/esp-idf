/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __DHCPS_H__
#define __DHCPS_H__

#include "sdkconfig.h"
#include <stdbool.h>
#include "lwip/ip_addr.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dhcps_state{
        s16_t state;
} dhcps_state;

typedef struct dhcps_msg {
        u8_t op, htype, hlen, hops;
        u8_t xid[4];
        u16_t secs, flags;
        u8_t ciaddr[4];
        u8_t yiaddr[4];
        u8_t siaddr[4];
        u8_t giaddr[4];
        u8_t chaddr[16];
        u8_t sname[64];
        u8_t file[128];
        u8_t options[312];
}dhcps_msg;

/*   Defined in esp_misc.h */
typedef struct {
	bool enable;
	ip4_addr_t start_ip;
	ip4_addr_t end_ip;
} dhcps_lease_t;

enum dhcps_offer_option{
	OFFER_START = 0x00,
	OFFER_ROUTER = 0x01,
	OFFER_DNS = 0x02,
	OFFER_END
};

/** @brief DHCP server's description of compile time configuration values in dhcpserver.c
 *
 * - DHCPS_DEBUG: Prints very detailed debug messages if set to 1, hardcoded to 0
 * - USE_CLASS_B_NET: Use class B network mask if enabled, not-defined (could be enabled as CC_FLAGS)
 * - MAX_STATION_NUM: Maximum number of clients, set to Kconfig value CONFIG_LWIP_DHCPS_MAX_STATION_NUM
 * - LWIP_HOOK_DHCPS_POST_STATE: Used to inject user code after parsing DHCP message, not defined
 *      - could be enabled in lwipopts.h or via CC_FLAGS
 *      - basic usage of the hook to print hex representation of the entire option field is below:
 *          #define LWIP_HOOK_DHCPS_POST_STATE(msg, len, state) \
 *          ({ s16_t ret = state; if (state == DHCPS_STATE_ACK) { ESP_LOG_BUFFER_HEXDUMP("DHCPS",msg->options, 312, ESP_LOG_INFO);} ret; })
 */

/**
 * @brief Definitions related to lease time, units and limits
 */
#define DHCPS_COARSE_TIMER_SECS  1
#define DHCPS_MAX_LEASE 0x64
#define DHCPS_LEASE_TIME_DEF (120)
#define DHCPS_LEASE_UNIT CONFIG_LWIP_DHCPS_LEASE_UNIT

struct dhcps_pool{
	ip4_addr_t ip;
	u8_t mac[6];
	u32_t lease_timer;
};

typedef u32_t dhcps_time_t;
typedef u8_t dhcps_offer_t;

typedef struct {
        dhcps_offer_t dhcps_offer;
        dhcps_offer_t dhcps_dns;
        dhcps_time_t  dhcps_time;
        dhcps_lease_t dhcps_poll;
} dhcps_options_t;

typedef void (*dhcps_cb_t)(void* cb_arg, u8_t client_ip[4], u8_t client_mac[6]);

static inline bool dhcps_router_enabled (dhcps_offer_t offer)
{
    return (offer & OFFER_ROUTER) != 0;
}

static inline bool dhcps_dns_enabled (dhcps_offer_t offer)
{
    return (offer & OFFER_DNS) != 0;
}

typedef struct dhcps_t dhcps_t;

/**
 * @brief Creates new DHCP server object
 *
 * @return Pointer to the DHCP server handle on success, NULL on error
 */
dhcps_t *dhcps_new(void);

/**
 * @brief Deletes supplied DHPC server object
 *
 * @warning This may not delete the handle immediately if the server wasn't
 * stopped properly, but mark for deleting once the timer callback occurs
 *
 * @param dhcps Pointer to the DHCP handle
 */
void dhcps_delete(dhcps_t *dhcps);

/**
 * @brief Starts the DHCP server on the specified network interface
 *
 * @param dhcps Pointer to the DHCP handle
 * @param netif Pointer to the lwIP's network interface struct
 * @param ip DHCP server's address
 * @return ERR_ARG if invalid args, ERR_OK on success
 */
err_t dhcps_start(dhcps_t *dhcps, struct netif *netif, ip4_addr_t ip);

/**
 * @brief Stops the DHCP server on the specified netif
 * @param dhcps Pointer to the DHCP handle
 * @param netif Pointer to the lwIP's network interface struct
 * @return ERR_ARG if invalid args, ERR_OK on success
 */
err_t dhcps_stop(dhcps_t *dhcps, struct netif *netif);

/**
 * @brief Gets the DHCP server option info
 * @param dhcps Pointer to the DHCP handle
 * @param op_id DHCP message option id
 * @param opt_len DHCP message option length
 * @return DHCP message option addr
 */
void *dhcps_option_info(dhcps_t *dhcps, u8_t op_id, u32_t opt_len);

/**
 * @brief Sets the DHCP server option info
 * @param dhcps Pointer to the DHCP handle
 * @param op_id DHCP message option id
 * @param opt_info DHCP message option info
 * @param opt_len DHCP message option length
 * @return ERR_ARG if invalid args, ERR_OK on success
 */
err_t dhcps_set_option_info(dhcps_t *dhcps, u8_t op_id, void *opt_info, u32_t opt_len);

/**
 * @brief Tries to find IP address corresponding to the supplied MAC
 * @param dhcps Pointer to the DHCP handle
 * @param mac Supplied MAC address
 * @param ip Pointer to the resultant IP address
 * @return True if the IP address has been found
 */
bool dhcp_search_ip_on_mac(dhcps_t *dhcps, u8_t *mac, ip4_addr_t *ip);

/**
 * @brief Sets DNS server address for the DHCP server
 * @param dhcps Pointer to the DHCP handle
 * @param dnsserver Address of the DNS server
 * @return ERR_ARG if invalid handle, ERR_OK on success
 */
err_t dhcps_dns_setserver(dhcps_t *dhcps, const ip_addr_t *dnsserver);

/**
 * @brief Gets DNS server associated with this DHCP server
 * @param dhcps Pointer to the DHCP handle
 * @param dnsserver Address of the DNS server
 * @return ERR_ARG if invalid handle, ERR_OK on success
 */
err_t dhcps_dns_getserver(dhcps_t *dhcps, ip4_addr_t *dnsserver);

/**
 * @brief Sets callback on assigning an IP to the connected client
 * @param dhcps Pointer to the DHCP handle
 * @param cb Callback for dhcp server
 * @param cb_arg Context pointer to be added to the callback
 * @return ERR_ARG if invalid handle, ERR_OK on success
 */
err_t dhcps_set_new_lease_cb(dhcps_t *dhcps, dhcps_cb_t cb, void* cb_arg);

#ifdef __cplusplus
}
#endif

#endif /* __DHCPS_H__ */
