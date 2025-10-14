/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lwip/dhcp.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/ip_addr.h"
#include "lwip/timeouts.h"
#include "lwip/etharp.h"
#include "lwip/prot/ethernet.h"

#include "dhcpserver/dhcpserver.h"
#include "dhcpserver/dhcpserver_options.h"

#if ESP_DHCPS

#ifdef LWIP_HOOK_FILENAME
#include LWIP_HOOK_FILENAME
#endif

#ifndef LWIP_HOOK_DHCPS_POST_APPEND_OPTS
#define LWIP_HOOK_DHCPS_POST_APPEND_OPTS(netif, dhcps, state, pp_opts)
#endif

#define BOOTP_BROADCAST 0x8000
#define BROADCAST_BIT_IS_SET(flag) (flag & BOOTP_BROADCAST)

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPS_SERVER_PORT  67
#define DHCPS_CLIENT_PORT  68

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7

#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_HOST_NAME    12
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_INTERFACE_MTU 26
#define DHCP_OPTION_PERFORM_ROUTER_DISCOVERY 31
#define DHCP_OPTION_BROADCAST_ADDRESS 28
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_CAPTIVEPORTAL_URI 114
#define DHCP_OPTION_END         255

//#define USE_CLASS_B_NET 1
#define DHCPS_DEBUG          0
#define DHCPS_LOG printf

#define IS_INVALID_SUBNET_MASK(x)  (((x-1) | x) != 0xFFFFFFFF)
/* Notes:
*  CIDR eliminates the traditional Class A, Class B and Class C addresses.
 */
#define IP_CLASS_HOST_NUM(mask)            (0xffffffff & ~mask)
#define DHCP_CHECK_SUBNET_MASK_IP(mask)                                                               \
    do {                                                                                                  \
        if (IS_INVALID_SUBNET_MASK(mask)) {                                                               \
            DHCPS_LOG("dhcps: Illegal subnet mask.\n");                                                   \
            return ERR_ARG;                                                                               \
        }                                                                                                 \
    } while (0)

#define DHCP_CHECK_IP_MATCH_SUBNET_MASK(mask, ip)                           \
    u32_t start_ip = 0;                                                     \
    u32_t end_ip = 0;                                                       \
    do {                                                                    \
        start_ip = ip & mask;                                               \
        end_ip = start_ip | ~mask;                                          \
        if (ip == end_ip || ip == start_ip) {                               \
            DHCPS_LOG("dhcps: ip address and subnet mask do not match.\n"); \
            return ERR_ARG;                                                 \
        }                                                                   \
    } while (0)

#define MAX_STATION_NUM CONFIG_LWIP_DHCPS_MAX_STATION_NUM

#define DHCPS_STATE_OFFER 1
#define DHCPS_STATE_DECLINE 2
#define DHCPS_STATE_ACK 3
#define DHCPS_STATE_NAK 4
#define DHCPS_STATE_IDLE 5
#define DHCPS_STATE_RELEASE 6

typedef enum {
    DHCPS_HANDLE_CREATED = 0,
    DHCPS_HANDLE_STARTED,
    DHCPS_HANDLE_STOPPED,
    DHCPS_HANDLE_DELETE_PENDING,
} dhcps_handle_state;

typedef struct list_node {
	void *pnode;
	struct list_node *pnext;
} list_node;

typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} ip_info_t;
////////////////////////////////////////////////////////////////////////////////////

static const u32_t magic_cookie  = 0x63538263;

struct dhcps_t {
    struct netif *dhcps_netif;
    ip4_addr_t broadcast_dhcps;
    ip4_addr_t server_address;
    ip4_addr_t dns_server[DNS_TYPE_MAX];
    ip4_addr_t client_address;
    ip4_addr_t client_address_plus;
    ip4_addr_t dhcps_mask;
    list_node *plist;
    bool renew;
    dhcps_lease_t dhcps_poll;
    dhcps_time_t dhcps_lease_time;
    dhcps_offer_t dhcps_offer;
    dhcps_offer_t dhcps_dns;
    char *dhcps_captiveportal_uri;
    dhcps_cb_t dhcps_cb;
    void* dhcps_cb_arg;
    struct udp_pcb *dhcps_pcb;
    dhcps_handle_state state;
    bool has_declined_ip;
#if CONFIG_LWIP_DHCPS_REPORT_CLIENT_HOSTNAME
    /* Temporary storage for option 12 parsed from the current packet */
    char opt_hostname[CONFIG_LWIP_DHCPS_MAX_HOSTNAME_LEN];
    bool opt_hostname_present;
#endif
};


static void dhcps_tmr(void* arg);


dhcps_t *dhcps_new(void)
{
    dhcps_t *dhcps = mem_calloc(1, sizeof(dhcps_t));

    if (dhcps == NULL) {
        return NULL;
    }
    dhcps->dhcps_netif = NULL;

    for (int i = 0; i < DNS_TYPE_MAX; i++) {
        dhcps->dns_server[i].addr = 0;
    }
#ifdef USE_CLASS_B_NET
    dhcps->dhcps_mask.addr = PP_HTONL(LWIP_MAKEU32(255, 240, 0, 0));
#else
    dhcps->dhcps_mask.addr = PP_HTONL(LWIP_MAKEU32(255, 255, 255, 0));
#endif
    dhcps->plist = NULL;
    dhcps->renew = false;
    dhcps->dhcps_lease_time = DHCPS_LEASE_TIME_DEF;
    dhcps->dhcps_offer = 0xFF;
    dhcps->dhcps_dns = 0x00;
    dhcps->dhcps_captiveportal_uri = NULL;
    dhcps->dhcps_pcb = NULL;
    dhcps->state = DHCPS_HANDLE_CREATED;
    dhcps->has_declined_ip = false;
    return dhcps;
}

void dhcps_delete(dhcps_t *dhcps)
{
    if (dhcps) {
        if (dhcps->state == DHCPS_HANDLE_STARTED) {
            // if the dhcp-server has started already, we have to postpone the deletion
            dhcps->state = DHCPS_HANDLE_DELETE_PENDING;
        } else {
            // otherwise, we're free to delete the handle immediately
            free(dhcps);
        }
    }
}

static void get_ip_info(struct netif * netif, ip_info_t *ip_info)
{
    if (netif != NULL && netif_is_up(netif)) {
        ip4_addr_set(&ip_info->ip, ip_2_ip4(&netif->ip_addr));
        ip4_addr_set(&ip_info->netmask, ip_2_ip4(&netif->netmask));
        ip4_addr_set(&ip_info->gw, ip_2_ip4(&netif->gw));
    }
}

static inline u8_t* dhcps_option_ip(u8_t *optptr, const ip4_addr_t *ip)
{
    LWIP_ASSERT("dhcps_option_ip: optptr must not be NULL", (optptr != NULL));
    LWIP_ASSERT("dhcps_option_ip: ip must not be NULL", (ip != NULL));

    *optptr++ = ip4_addr1(ip);
    *optptr++ = ip4_addr2(ip);
    *optptr++ = ip4_addr3(ip);
    *optptr++ = ip4_addr4(ip);
    return optptr;
}

/******************************************************************************
 * FunctionName : dhcps_option_info
 * Description  : get the DHCP message option info
 * Parameters   : op_id -- DHCP message option id
 *                opt_len -- DHCP message option length
 * Returns      : DHCP message option addr
*******************************************************************************/
void *dhcps_option_info(dhcps_t *dhcps, u8_t op_id, u32_t opt_len)
{
    void *option_arg = NULL;
    if (dhcps == NULL) {
        return NULL;
    }

    switch (op_id) {
        case IP_ADDRESS_LEASE_TIME:
            if (opt_len == sizeof(dhcps_time_t)) {
                option_arg = &dhcps->dhcps_lease_time;
            }

            break;

        case REQUESTED_IP_ADDRESS:
            if (opt_len == sizeof(dhcps_lease_t)) {
                option_arg = &dhcps->dhcps_poll;
            }

            break;

        case ROUTER_SOLICITATION_ADDRESS:
            if (opt_len == sizeof(dhcps_offer_t)) {
                option_arg = &dhcps->dhcps_offer;
            }

            break;

        case DOMAIN_NAME_SERVER:
            if (opt_len == sizeof(dhcps_offer_t)) {
                option_arg = &dhcps->dhcps_dns;
            }

            break;
        case SUBNET_MASK:
            if (opt_len == sizeof(dhcps->dhcps_mask)) {
                option_arg = &dhcps->dhcps_mask;
            }

            break;
        case CAPTIVEPORTAL_URI:
            option_arg = &dhcps->dhcps_captiveportal_uri;

            break;
        default:
            break;
    }

    return option_arg;
}

/******************************************************************************
 * FunctionName : dhcps_set_option_info
 * Description  : set the DHCP message option info
 * Parameters   : op_id -- DHCP message option id
 *                opt_info -- DHCP message option info
 *                opt_len -- DHCP message option length
 * Returns      : none
*******************************************************************************/
err_t dhcps_set_option_info(dhcps_t *dhcps, u8_t op_id, void *opt_info, u32_t opt_len)
{
    if (dhcps == NULL || opt_info == NULL) {
        return ERR_ARG;
    }
    switch (op_id) {
        case IP_ADDRESS_LEASE_TIME:
            if (opt_len == sizeof(dhcps_time_t)) {
                dhcps->dhcps_lease_time = *(dhcps_time_t *)opt_info;
            }

            break;

        case REQUESTED_IP_ADDRESS:
            if (opt_len == sizeof(dhcps_lease_t)) {
                dhcps->dhcps_poll = *(dhcps_lease_t *)opt_info;
            }

            break;

        case ROUTER_SOLICITATION_ADDRESS:
            if (opt_len == sizeof(dhcps_offer_t)) {
                dhcps->dhcps_offer = *(dhcps_offer_t *)opt_info;
            }

            break;

        case DOMAIN_NAME_SERVER:
            if (opt_len == sizeof(dhcps_offer_t)) {
                dhcps->dhcps_dns = *(dhcps_offer_t *)opt_info;
            }
            break;

        case SUBNET_MASK:
            if (opt_len == sizeof(dhcps->dhcps_mask)) {
                dhcps->dhcps_mask = *(ip4_addr_t *)opt_info;
            }

            break;

        case CAPTIVEPORTAL_URI:
            dhcps->dhcps_captiveportal_uri = (char *)opt_info;
            break;

        default:
            break;
    }
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : node_insert_to_list
 * Description  : insert the node to the list
 * Parameters   : phead -- the head node of the list
 *                pinsert -- the insert node of the list
 * Returns      : none
*******************************************************************************/
static void node_insert_to_list(list_node **phead, list_node *pinsert)
{
    list_node *plist = NULL;
    struct dhcps_pool *pdhcps_pool = NULL;
    struct dhcps_pool *pdhcps_node = NULL;

    if (*phead == NULL) {
        *phead = pinsert;
    } else {
        plist = *phead;
        pdhcps_node = pinsert->pnode;
        pdhcps_pool = plist->pnode;

        if (pdhcps_node->ip.addr < pdhcps_pool->ip.addr) {
            pinsert->pnext = plist;
            *phead = pinsert;
        } else {
            while (plist->pnext != NULL) {
                pdhcps_pool = plist->pnext->pnode;

                if (pdhcps_node->ip.addr < pdhcps_pool->ip.addr) {
                    pinsert->pnext = plist->pnext;
                    plist->pnext = pinsert;
                    break;
                }

                plist = plist->pnext;
            }

            if (plist->pnext == NULL) {
                plist->pnext = pinsert;
            }
        }
    }

//	pinsert->pnext = NULL;
}

/******************************************************************************
 * FunctionName : node_delete_from_list
 * Description  : remove the node from list
 * Parameters   : phead -- the head node of the list
 *                pdelete -- the remove node of the list
 * Returns      : none
*******************************************************************************/
void node_remove_from_list(list_node **phead, list_node *pdelete)
{
    list_node *plist = NULL;

    plist = *phead;

    if (plist == NULL) {
        *phead = NULL;
    } else {
        if (plist == pdelete) {
            // Note: Ignoring the "use after free" warnings, as it could only happen
            // if the linked list contains loops
            *phead = plist->pnext; // NOLINT(clang-analyzer-unix.Malloc)
            pdelete->pnext = NULL;
        } else {
            while (plist != NULL) {
                if (plist->pnext == pdelete) { // NOLINT(clang-analyzer-unix.Malloc)
                    plist->pnext = pdelete->pnext;
                    pdelete->pnext = NULL;
                }

                plist = plist->pnext;
            }
        }
    }
}

/******************************************************************************
 * FunctionName : add_msg_type
 * Description  : add TYPE option of DHCP message
 * Parameters   : optptr -- the addr of DHCP message option
 * Returns      : the addr of DHCP message option
*******************************************************************************/
static u8_t *add_msg_type(u8_t *optptr, u8_t type)
{
    *optptr++ = DHCP_OPTION_MSG_TYPE;
    *optptr++ = 1;
    *optptr++ = type;
    return optptr;
}

/******************************************************************************
 * FunctionName : add_offer_options
 * Description  : add OFFER option of DHCP message
 * Parameters   : optptr -- the addr of DHCP message option
 * Returns      : the addr of DHCP message option
*******************************************************************************/
static u8_t *add_offer_options(dhcps_t *dhcps, u8_t *optptr)
{
    u32_t i;
    ip4_addr_t ipadd;

    ipadd.addr = *((u32_t *) &dhcps->server_address);

    *optptr++ = DHCP_OPTION_SUBNET_MASK;
    *optptr++ = 4;
    optptr = dhcps_option_ip(optptr, &dhcps->dhcps_mask);

    *optptr++ = DHCP_OPTION_LEASE_TIME;
    *optptr++ = 4;
    *optptr++ = ((dhcps->dhcps_lease_time * DHCPS_LEASE_UNIT) >> 24) & 0xFF;
    *optptr++ = ((dhcps->dhcps_lease_time * DHCPS_LEASE_UNIT) >> 16) & 0xFF;
    *optptr++ = ((dhcps->dhcps_lease_time * DHCPS_LEASE_UNIT) >> 8) & 0xFF;
    *optptr++ = ((dhcps->dhcps_lease_time * DHCPS_LEASE_UNIT) >> 0) & 0xFF;

    *optptr++ = DHCP_OPTION_SERVER_ID;
    *optptr++ = 4;
    optptr = dhcps_option_ip(optptr, &ipadd);

    if (dhcps_router_enabled(dhcps->dhcps_offer)) {
        ip_info_t if_ip = { 0 };
        get_ip_info(dhcps->dhcps_netif, &if_ip);

        ip4_addr_t* gw_ip = (ip4_addr_t*)&if_ip.gw;

        if (!ip4_addr_isany_val(*gw_ip)) {
            *optptr++ = DHCP_OPTION_ROUTER;
            *optptr++ = 4;
            optptr = dhcps_option_ip(optptr, gw_ip);
        }
    }

    // Add DNS option if either main or backup DNS is set
    if (dhcps_dns_enabled(dhcps->dhcps_dns) &&
        (dhcps->dns_server[DNS_TYPE_MAIN].addr || dhcps->dns_server[DNS_TYPE_BACKUP].addr)) {

        uint8_t size = 0;

        if (dhcps->dns_server[DNS_TYPE_MAIN].addr) {
            size += 4;
        }
        if (dhcps->dns_server[DNS_TYPE_BACKUP].addr) {
            size += 4;
        }

        *optptr++ = DHCP_OPTION_DNS_SERVER;
        *optptr++ = size;

        if (dhcps->dns_server[DNS_TYPE_MAIN].addr) {
            optptr = dhcps_option_ip(optptr, &dhcps->dns_server[DNS_TYPE_MAIN]);
        }
        if (dhcps->dns_server[DNS_TYPE_BACKUP].addr) {
            optptr = dhcps_option_ip(optptr, &dhcps->dns_server[DNS_TYPE_BACKUP]);
        }
    } else {
        *optptr++ = DHCP_OPTION_DNS_SERVER;
        *optptr++ = 4;
        optptr = dhcps_option_ip(optptr, &ipadd);
    }

    ip4_addr_t broadcast_addr = { .addr = (ipadd.addr & dhcps->dhcps_mask.addr) | ~dhcps->dhcps_mask.addr };
    *optptr++ = DHCP_OPTION_BROADCAST_ADDRESS;
    *optptr++ = 4;
    optptr = dhcps_option_ip(optptr, &broadcast_addr);

    *optptr++ = DHCP_OPTION_INTERFACE_MTU;
    *optptr++ = 2;
    *optptr++ = 0x05;
    *optptr++ = 0xdc;

    if (dhcps->dhcps_captiveportal_uri) {
        size_t length = strlen(dhcps->dhcps_captiveportal_uri);

        *optptr++ = DHCP_OPTION_CAPTIVEPORTAL_URI;
        *optptr++ = length;
        for (i = 0; i < length; i++) {
            *optptr++ = dhcps->dhcps_captiveportal_uri[i];
        }
    }

    *optptr++ = DHCP_OPTION_PERFORM_ROUTER_DISCOVERY;
    *optptr++ = 1;
    *optptr++ = 0x00;

    *optptr++ = 43;
    *optptr++ = 6;

    *optptr++ = 0x01;
    *optptr++ = 4;
    *optptr++ = 0x00;
    *optptr++ = 0x00;
    *optptr++ = 0x00;
    *optptr++ = 0x02;

    return optptr;
}

/******************************************************************************
 * FunctionName : add_end
 * Description  : add end option of DHCP message
 * Parameters   : optptr -- the addr of DHCP message option
 * Returns      : the addr of DHCP message option
*******************************************************************************/
static u8_t *add_end(u8_t *optptr)
{
    *optptr++ = DHCP_OPTION_END;
    return optptr;
}

/******************************************************************************
 * FunctionName : create_msg
 * Description  : create response message
 * Parameters   : m -- DHCP message info
 * Returns      : none
*******************************************************************************/
static void create_msg(dhcps_t *dhcps, struct dhcps_msg *m)
{
    ip4_addr_t client;


    client.addr = *((uint32_t *) &dhcps->client_address);

    m->op = DHCP_REPLY;
    m->htype = DHCP_HTYPE_ETHERNET;
    m->hlen = 6;

#if !ETHARP_SUPPORT_STATIC_ENTRIES
    /* If the DHCP server does not support sending unicast message to the client,
     * need to set the 'flags' field to broadcast */
    m->flags = htons(BOOTP_BROADCAST);
#endif

    memcpy((char *) m->yiaddr, (char *) &client.addr, sizeof(m->yiaddr));
    memcpy((char *) m->options, &magic_cookie, sizeof(magic_cookie));
}

/******************************************************************************
 * FunctionName : dhcps_response_ip_set
 * Description  : set the ip address for sending to the DHCP client
 * Parameters   : m -- DHCP message info
 *                ip4_out -- ip address for sending
 * Returns      : none
*******************************************************************************/
static void dhcps_response_ip_set(dhcps_t *dhcps, struct dhcps_msg *m, ip4_addr_t *ip4_out)
{
#if ETHARP_SUPPORT_STATIC_ENTRIES
    ip4_addr_t ip4_giaddr;
    ip4_addr_t ip4_ciaddr;
    ip4_addr_t ip4_yiaddr;

    struct eth_addr chaddr;
    memcpy(chaddr.addr, m->chaddr, sizeof(chaddr.addr));
    memcpy((char *)&ip4_giaddr.addr, (char *)m->giaddr, sizeof(m->giaddr));
    memcpy((char *)&ip4_ciaddr.addr, (char *)m->ciaddr, sizeof(m->ciaddr));
    memcpy((char *)&ip4_yiaddr.addr, (char *)m->yiaddr, sizeof(m->yiaddr));

    if (!ip4_addr_isany_val(ip4_giaddr)) {
        /* If the 'giaddr' field is non-zero, send return message to the address in 'giaddr'. (RFC 2131)*/
        ip4_addr_set(ip4_out, &ip4_giaddr);
        /* add the IP<->MAC as static entry into the arp table. */
        etharp_add_static_entry(&ip4_giaddr, &chaddr);
    } else {
        if (!ip4_addr_isany_val(ip4_ciaddr)) {
            /* If the 'giaddr' field is zero and the 'ciaddr' is nonzero,
             * the server unicasts DHCPOFFER and DHCPACK message to the address in 'ciaddr'*/
            ip4_addr_set(ip4_out, &ip4_ciaddr);
            etharp_add_static_entry(&ip4_ciaddr, &chaddr);
        } else if (!BROADCAST_BIT_IS_SET(htons(m->flags))) {
            /* If the 'giaddr' is zero and 'ciaddr' is zero, and the broadcast bit is not set,
             * the server unicasts DHCPOFFER and DHCPACK message to the client's hardware address and
             * 'yiaddr' address. */
            ip4_addr_set(ip4_out, &ip4_yiaddr);
            etharp_add_static_entry(&ip4_yiaddr, &chaddr);
        } else {
            /* The server broadcast DHCPOFFER and DHCPACK message to 0xffffffff*/
            ip4_addr_set(ip4_out, &dhcps->broadcast_dhcps);
        }
    }
#else
    ip4_addr_set(ip4_out, &dhcps->broadcast_dhcps);
#endif
}


struct pbuf * dhcps_pbuf_alloc(u16_t len)
{
    u16_t mlen = sizeof(struct dhcps_msg);

    if (len > mlen) {
#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: len=%d mlen=%d", len, mlen);
#endif
        mlen = len;
    }

    return pbuf_alloc(PBUF_TRANSPORT, mlen, PBUF_RAM);
}

/******************************************************************************
 * FunctionName : send_offer
 * Description  : DHCP message OFFER Response
 * Parameters   : m -- DHCP message info
 * Returns      : none
*******************************************************************************/
static void send_offer(dhcps_t *dhcps, struct dhcps_msg *m, u16_t len)
{
    u8_t *end;
    struct pbuf *p, *q;
    u8_t *data;
    u16_t cnt = 0;
    u16_t i;
#if DHCPS_DEBUG
    err_t SendOffer_err_t;
#endif
    create_msg(dhcps, m);

    end = add_msg_type(&m->options[4], DHCPOFFER);
    end = add_offer_options(dhcps, end);
    LWIP_HOOK_DHCPS_POST_APPEND_OPTS(dhcps->dhcps_netif, dhcps, DHCPOFFER, &end)
    end = add_end(end);

    p = dhcps_pbuf_alloc(len);
#if DHCPS_DEBUG
    DHCPS_LOG("udhcp: send_offer>>p->ref = %d\n", p->ref);
#endif

    if (p != NULL) {

#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: send_offer>>pbuf_alloc succeed\n");
        DHCPS_LOG("dhcps: send_offer>>p->tot_len = %d\n", p->tot_len);
        DHCPS_LOG("dhcps: send_offer>>p->len = %d\n", p->len);
#endif
        q = p;

        while (q != NULL) {
            data = (u8_t *)q->payload;

            for (i = 0; i < q->len; i++) {
                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG
                DHCPS_LOG("%02x ", data[i]);

                if ((i + 1) % 16 == 0) {
                    DHCPS_LOG("\n");
                }

#endif
            }

            q = q->next;
        }
    } else {

#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: send_offer>>pbuf_alloc failed\n");
#endif
        return;
    }

    ip_addr_t ip_temp = IPADDR4_INIT(0x0);
    dhcps_response_ip_set(dhcps, m, ip_2_ip4(&ip_temp));
#if DHCPS_DEBUG
    SendOffer_err_t = udp_sendto(dhcps->dhcps_pcb, p, &ip_temp, DHCPS_CLIENT_PORT);
    DHCPS_LOG("dhcps: send_offer>>udp_sendto result %x\n", SendOffer_err_t);
#else
    udp_sendto(dhcps->dhcps_pcb, p, &ip_temp, DHCPS_CLIENT_PORT);
#endif

#if ETHARP_SUPPORT_STATIC_ENTRIES
    /* remove the IP<->MAC from the arp table. */
    etharp_remove_static_entry(ip_2_ip4(&ip_temp));
#endif

    if (p->ref != 0) {
#if DHCPS_DEBUG
        DHCPS_LOG("udhcp: send_offer>>free pbuf\n");
#endif
        pbuf_free(p);
    }
}

/******************************************************************************
 * FunctionName : send_nak
 * Description  : DHCP message NACK Response
 * Parameters   : m -- DHCP message info
 * Returns      : none
*******************************************************************************/
static void send_nak(dhcps_t *dhcps, struct dhcps_msg *m, u16_t len)
{
    u8_t *end;
    struct pbuf *p, *q;
    u8_t *data;
    u16_t cnt = 0;
    u16_t i;
#if DHCPS_DEBUG
    err_t SendNak_err_t;
#endif
    create_msg(dhcps, m);

    end = add_msg_type(&m->options[4], DHCPNAK);
    LWIP_HOOK_DHCPS_POST_APPEND_OPTS(dhcps->dhcps_netif, dhcps, DHCPNAK, &end)
    end = add_end(end);

    p = dhcps_pbuf_alloc(len);
#if DHCPS_DEBUG
    DHCPS_LOG("udhcp: send_nak>>p->ref = %d\n", p->ref);
#endif

    if (p != NULL) {

#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: send_nak>>pbuf_alloc succeed\n");
        DHCPS_LOG("dhcps: send_nak>>p->tot_len = %d\n", p->tot_len);
        DHCPS_LOG("dhcps: send_nak>>p->len = %d\n", p->len);
#endif
        q = p;

        while (q != NULL) {
            data = (u8_t *)q->payload;

            for (i = 0; i < q->len; i++) {
                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG
                DHCPS_LOG("%02x ", data[i]);

                if ((i + 1) % 16 == 0) {
                    DHCPS_LOG("\n");
                }

#endif
            }

            q = q->next;
        }
    } else {

#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: send_nak>>pbuf_alloc failed\n");
#endif
        return;
    }

    ip_addr_t ip_temp = IPADDR4_INIT(0x0);

#if ETHARP_SUPPORT_STATIC_ENTRIES
    ip4_addr_t ip4_giaddr;
    struct eth_addr chaddr;
    memcpy(chaddr.addr, m->chaddr, sizeof(chaddr.addr));
    memcpy((char *)&ip4_giaddr.addr, (char *)m->giaddr, sizeof(m->giaddr));

    if (!ip4_addr_isany_val(ip4_giaddr)) {
        ip4_addr_set(ip_2_ip4(&ip_temp), &ip4_giaddr);
        /* add the IP<->MAC as static entry into the arp table. */
        etharp_add_static_entry(&ip4_giaddr, &chaddr);
    } else {
        /* when 'giaddr' is zero, the server broadcasts any DHCPNAK message to 0xffffffff. (RFC 2131)*/
        ip4_addr_set(ip_2_ip4(&ip_temp), &dhcps->broadcast_dhcps);
    }
#else
    ip4_addr_set(ip_2_ip4(&ip_temp), &dhcps->broadcast_dhcps);
#endif

#if DHCPS_DEBUG
    SendNak_err_t = udp_sendto(dhcps->dhcps_pcb, p, &ip_temp, DHCPS_CLIENT_PORT);
    DHCPS_LOG("dhcps: send_nak>>udp_sendto result %x\n", SendNak_err_t);
#else
    udp_sendto(dhcps->dhcps_pcb, p, &ip_temp, DHCPS_CLIENT_PORT);
#endif

#if ETHARP_SUPPORT_STATIC_ENTRIES
    /* remove the IP<->MAC from the arp table. */
    etharp_remove_static_entry(ip_2_ip4(&ip_temp));
#endif

    if (p->ref != 0) {
#if DHCPS_DEBUG
        DHCPS_LOG("udhcp: send_nak>>free pbuf\n");
#endif
        pbuf_free(p);
    }
}

/******************************************************************************
 * FunctionName : send_ack
 * Description  : DHCP message ACK Response
 * Parameters   : m -- DHCP message info
 * Returns      : none
*******************************************************************************/
static void send_ack(dhcps_t *dhcps, struct dhcps_msg *m, u16_t len)
{
    u8_t *end;
    struct pbuf *p, *q;
    u8_t *data;
    u16_t cnt = 0;
    u16_t i;
    err_t SendAck_err_t;
    create_msg(dhcps, m);

    end = add_msg_type(&m->options[4], DHCPACK);
    end = add_offer_options(dhcps, end);
    LWIP_HOOK_DHCPS_POST_APPEND_OPTS(dhcps->dhcps_netif, dhcps, DHCPACK, &end)
    end = add_end(end);

    p = dhcps_pbuf_alloc(len);
#if DHCPS_DEBUG
    DHCPS_LOG("udhcp: send_ack>>p->ref = %d\n", p->ref);
#endif

    if (p != NULL) {

#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: send_ack>>pbuf_alloc succeed\n");
        DHCPS_LOG("dhcps: send_ack>>p->tot_len = %d\n", p->tot_len);
        DHCPS_LOG("dhcps: send_ack>>p->len = %d\n", p->len);
#endif
        q = p;

        while (q != NULL) {
            data = (u8_t *)q->payload;

            for (i = 0; i < q->len; i++) {
                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG
                DHCPS_LOG("%02x ", data[i]);

                if ((i + 1) % 16 == 0) {
                    DHCPS_LOG("\n");
                }

#endif
            }

            q = q->next;
        }
    } else {

#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: send_ack>>pbuf_alloc failed\n");
#endif
        return;
    }

    ip_addr_t ip_temp = IPADDR4_INIT(0x0);
    dhcps_response_ip_set(dhcps, m, ip_2_ip4(&ip_temp));
    SendAck_err_t = udp_sendto(dhcps->dhcps_pcb, p, &ip_temp, DHCPS_CLIENT_PORT);
#if DHCPS_DEBUG
    DHCPS_LOG("dhcps: send_ack>>udp_sendto result %x\n", SendAck_err_t);
#endif

#if ETHARP_SUPPORT_STATIC_ENTRIES
    /* remove the IP<->MAC from the arp table. */
    etharp_remove_static_entry(ip_2_ip4(&ip_temp));
#endif

    if (SendAck_err_t == ERR_OK) {
        dhcps->dhcps_cb(dhcps->dhcps_cb_arg, m->yiaddr, m->chaddr);
    }

    if (p->ref != 0) {
#if DHCPS_DEBUG
        DHCPS_LOG("udhcp: send_ack>>free pbuf\n");
#endif
        pbuf_free(p);
    }
}

/******************************************************************************
 * FunctionName : parse_options
 * Description  : parse DHCP message options
 * Parameters   : optptr -- DHCP message option info
 *                len -- DHCP message option length
 * Returns      : none
*******************************************************************************/
static u8_t parse_options(dhcps_t *dhcps, u8_t *optptr, s16_t len)
{
    ip4_addr_t client;
    bool is_dhcp_parse_end = false;
    struct dhcps_state s;

    client.addr = *((uint32_t *) &dhcps->client_address);

    u8_t *end = optptr + len;
    u16_t type = 0;

    s.state = DHCPS_STATE_IDLE;

    while (optptr < end) {
#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: (s16_t)*optptr = %d\n", (s16_t)*optptr);
#endif

        switch ((s16_t) *optptr) {

            case DHCP_OPTION_MSG_TYPE:	//53
                type = *(optptr + 2);
                break;

#if CONFIG_LWIP_DHCPS_REPORT_CLIENT_HOSTNAME
            case DHCP_OPTION_HOST_NAME: {
                /* option format: code(1) len(1) value(len) */
                u8_t olen = *(optptr + 1);
                const u8_t *oval = optptr + 2;
                if (olen > 0) {
                    /* clamp to configured max, keep room for NUL */
                    size_t copy_len = olen;
                    if (copy_len >= CONFIG_LWIP_DHCPS_MAX_HOSTNAME_LEN) {
                        copy_len = CONFIG_LWIP_DHCPS_MAX_HOSTNAME_LEN - 1;
                    }
                    size_t j = 0;
                    for (; j < copy_len; ++j) {
                        char c = (char)oval[j];
                        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_') {
                            dhcps->opt_hostname[j] = c;
                        } else {
                            dhcps->opt_hostname[j] = '-';
                        }
                    }
                    dhcps->opt_hostname[j] = '\0';
                    dhcps->opt_hostname_present = true;
                }
                break;
            }
#endif

            case DHCP_OPTION_REQ_IPADDR://50
                if (memcmp((char *) &client.addr, (char *) optptr + 2, 4) == 0) {
#if DHCPS_DEBUG
                    DHCPS_LOG("dhcps: DHCP_OPTION_REQ_IPADDR = 0 ok\n");
#endif
                    s.state = DHCPS_STATE_ACK;
                } else {
#if DHCPS_DEBUG
                    DHCPS_LOG("dhcps: DHCP_OPTION_REQ_IPADDR != 0 err\n");
#endif
                    s.state = DHCPS_STATE_NAK;
                }

                break;

            case DHCP_OPTION_END: {
                is_dhcp_parse_end = true;
            }
            break;
        }

        if (is_dhcp_parse_end) {
            break;
        }

        optptr += optptr[1] + 2;
    }

    switch (type) {

        case DHCPDISCOVER://1
            s.state = DHCPS_STATE_OFFER;
#if DHCPS_DEBUG
            DHCPS_LOG("dhcps: DHCPD_STATE_OFFER\n");
#endif
            break;

        case DHCPREQUEST://3
            if (!(s.state == DHCPS_STATE_ACK || s.state == DHCPS_STATE_NAK)) {
                if (dhcps->renew == true) {
                    s.state = DHCPS_STATE_ACK;
                } else {
                    s.state = DHCPS_STATE_NAK;
                }

#if DHCPS_DEBUG
                DHCPS_LOG("dhcps: DHCPD_STATE_NAK\n");
#endif
            }

            break;

        case DHCPDECLINE://4
            s.state = DHCPS_STATE_DECLINE;
#if DHCPS_DEBUG
            DHCPS_LOG("dhcps: DHCPS_STATE_DECLINE\n");
#endif
            break;

        case DHCPRELEASE://7
            s.state = DHCPS_STATE_RELEASE;
#if DHCPS_DEBUG
            DHCPS_LOG("dhcps: DHCPD_STATE_IDLE\n");
#endif
            break;
    }

#if DHCPS_DEBUG
    DHCPS_LOG("dhcps: return s.state = %d\n", s.state);
#endif
    return s.state;
}

/******************************************************************************
 * FunctionName : parse_msg
 * Description  : parse DHCP message from netif
 * Parameters   : m -- DHCP message info
 *                len -- DHCP message length
 * Returns      : DHCP message type
*******************************************************************************/
static s16_t parse_msg(dhcps_t *dhcps, struct dhcps_msg *m, u16_t len)
{
    u32_t lease_timer = (dhcps->dhcps_lease_time * DHCPS_LEASE_UNIT)/DHCPS_COARSE_TIMER_SECS;

    if (memcmp((char *)m->options, &magic_cookie, sizeof(magic_cookie)) == 0) {
#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: len = %d\n", len);
#endif
        ip4_addr_t addr_tmp;

        struct dhcps_pool *pdhcps_pool = NULL;
        list_node *pnode = NULL;
        list_node *pback_node = NULL;
        ip4_addr_t first_address;
        bool flag = false;

        first_address.addr = dhcps->dhcps_poll.start_ip.addr;
        dhcps->client_address.addr = dhcps->client_address_plus.addr;
        dhcps->renew = false;

        if (dhcps->plist != NULL) {
            if (dhcps->has_declined_ip) {
                dhcps->has_declined_ip = false;
            }

            for (pback_node = dhcps->plist; pback_node != NULL; pback_node = pback_node->pnext) {
                pdhcps_pool = pback_node->pnode;

                if (memcmp(pdhcps_pool->mac, m->chaddr, sizeof(pdhcps_pool->mac)) == 0) {
                    if (memcmp(&pdhcps_pool->ip.addr, m->ciaddr, sizeof(pdhcps_pool->ip.addr)) == 0) {
                        dhcps->renew = true;
                    }

                    dhcps->client_address.addr = pdhcps_pool->ip.addr;
                    pdhcps_pool->lease_timer = lease_timer;
                    pnode = pback_node;
                    goto POOL_CHECK;
                } else if (pdhcps_pool->ip.addr == dhcps->client_address_plus.addr) {
                    addr_tmp.addr = htonl(dhcps->client_address_plus.addr);
                    addr_tmp.addr++;
                    dhcps->client_address_plus.addr = htonl(addr_tmp.addr);
                    dhcps->client_address.addr = dhcps->client_address_plus.addr;
                }

                if (flag == false) { // search the first unused ip
                    if (first_address.addr < pdhcps_pool->ip.addr) {
                        flag = true;
                    } else {
                        addr_tmp.addr = htonl(first_address.addr);
                        addr_tmp.addr++;
                        first_address.addr = htonl(addr_tmp.addr);
                    }
                }
            }
        } else {
            if (dhcps->has_declined_ip) {
                dhcps->has_declined_ip = false;
            } else {
                dhcps->client_address.addr = dhcps->dhcps_poll.start_ip.addr;
            }
        }

        if (dhcps->client_address_plus.addr > dhcps->dhcps_poll.end_ip.addr) {
            dhcps->client_address.addr = first_address.addr;
        }

        if (dhcps->client_address.addr > dhcps->dhcps_poll.end_ip.addr) {
            dhcps->client_address_plus.addr = dhcps->dhcps_poll.start_ip.addr;
            pdhcps_pool = NULL;
            pnode = NULL;
        } else {
            pdhcps_pool = (struct dhcps_pool *)mem_calloc(1, sizeof(struct dhcps_pool));

            pdhcps_pool->ip.addr = dhcps->client_address.addr;
            memcpy(pdhcps_pool->mac, m->chaddr, sizeof(pdhcps_pool->mac));
            pdhcps_pool->lease_timer = lease_timer;
            pnode = (list_node *)mem_calloc(1, sizeof(list_node));

            pnode->pnode = pdhcps_pool;
            pnode->pnext = NULL;
            node_insert_to_list(&dhcps->plist, pnode);

            if (dhcps->client_address.addr == dhcps->dhcps_poll.end_ip.addr) {
                dhcps->client_address_plus.addr = dhcps->dhcps_poll.start_ip.addr;
            } else {
                addr_tmp.addr = htonl(dhcps->client_address.addr);
                addr_tmp.addr++;
                dhcps->client_address_plus.addr = htonl(addr_tmp.addr);
            }
        }

POOL_CHECK:

        if ((dhcps->client_address.addr > dhcps->dhcps_poll.end_ip.addr) || (ip4_addr_isany(&dhcps->client_address))) {
            if (pnode != NULL) {
                node_remove_from_list(&dhcps->plist, pnode);
                free(pnode);
                pnode = NULL;
            }

            if (pdhcps_pool != NULL) {
                free(pdhcps_pool);
                pdhcps_pool = NULL;
            }

            return 4;
        }

        s16_t ret = parse_options(dhcps, &m->options[4], len);

        if (ret == DHCPS_STATE_RELEASE || ret == DHCPS_STATE_NAK || ret ==  DHCPS_STATE_DECLINE) {
            if (pnode != NULL) {
                node_remove_from_list(&dhcps->plist, pnode);
                free(pnode);
                pnode = NULL;
            }

            if (pdhcps_pool != NULL) {
                free(pdhcps_pool);
                pdhcps_pool = NULL;
            }

            if (ret ==  DHCPS_STATE_DECLINE) {
                dhcps->has_declined_ip = true;
            }
            memset(&dhcps->client_address, 0x0, sizeof(dhcps->client_address));
        }

#if CONFIG_LWIP_DHCPS_REPORT_CLIENT_HOSTNAME
        /* If we parsed a hostname from options and we have a lease entry, store it */
        if (pdhcps_pool != NULL) {
            if (dhcps->opt_hostname_present) {
                size_t n = strnlen(dhcps->opt_hostname, CONFIG_LWIP_DHCPS_MAX_HOSTNAME_LEN);
                if (n > 0) {
                    memset(pdhcps_pool->hostname, 0, sizeof(pdhcps_pool->hostname));
                    memcpy(pdhcps_pool->hostname, dhcps->opt_hostname, n);
                } else {
                    pdhcps_pool->hostname[0] = '\0';
                }
            }
        }
#endif

#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: xid changed\n");
        DHCPS_LOG("dhcps: client_address.addr = %x\n", dhcps->client_address.addr);
#endif
        return ret;
    }

    return 0;
}

/******************************************************************************
 * FunctionName : handle_dhcp
 * Description  : If an incoming DHCP message is in response to us, then trigger the state machine
 * Parameters   : arg -- arg user supplied argument (udp_pcb.recv_arg)
 * 				  pcb -- the udp_pcb which received data
 * 			      p -- the packet buffer that was received
 * 				  addr -- the remote IP address from which the packet was received
 * 				  port -- the remote port from which the packet was received
 * Returns      : none
*******************************************************************************/
static void handle_dhcp(void *arg,
                        struct udp_pcb *pcb,
                        struct pbuf *p,
                        const ip_addr_t *addr,
                        u16_t port)
{
    struct dhcps_t *dhcps = arg;
    struct dhcps_msg *pmsg_dhcps = NULL;
    s16_t tlen, malloc_len;
    u16_t i;
    u16_t dhcps_msg_cnt = 0;
    u8_t *p_dhcps_msg = NULL;
    u8_t *data;
    s16_t state;

#if DHCPS_DEBUG
    DHCPS_LOG("dhcps: handle_dhcp-> receive a packet\n");
#endif

    if (p == NULL) {
        return;
    }

    malloc_len = sizeof(struct dhcps_msg);
#if DHCPS_DEBUG
    DHCPS_LOG("dhcps: handle_dhcp malloc_len=%d rx_len=%d", malloc_len, p->tot_len);
#endif
    if (malloc_len < p->tot_len) {
        malloc_len = p->tot_len;
    }

    pmsg_dhcps = (struct dhcps_msg *)mem_calloc(1, malloc_len);
    if (NULL == pmsg_dhcps) {
        pbuf_free(p);
        return;
    }

    p_dhcps_msg = (u8_t *)pmsg_dhcps;
    tlen = p->tot_len;
    data = p->payload;

#if DHCPS_DEBUG
    DHCPS_LOG("dhcps: handle_dhcp-> p->tot_len = %d\n", tlen);
    DHCPS_LOG("dhcps: handle_dhcp-> p->len = %d\n", p->len);
#endif

    for (i = 0; i < p->len; i++) {
        p_dhcps_msg[dhcps_msg_cnt++] = data[i];
#if DHCPS_DEBUG
        DHCPS_LOG("%02x ", data[i]);

        if ((i + 1) % 16 == 0) {
            DHCPS_LOG("\n");
        }

#endif
    }

    if (p->next != NULL) {
#if DHCPS_DEBUG
        DHCPS_LOG("dhcps: handle_dhcp-> p->next != NULL\n");
        DHCPS_LOG("dhcps: handle_dhcp-> p->next->tot_len = %d\n", p->next->tot_len);
        DHCPS_LOG("dhcps: handle_dhcp-> p->next->len = %d\n", p->next->len);
#endif

        data = p->next->payload;

        for (i = 0; i < p->next->len; i++) {
            p_dhcps_msg[dhcps_msg_cnt++] = data[i];
#if DHCPS_DEBUG
            DHCPS_LOG("%02x ", data[i]);

            if ((i + 1) % 16 == 0) {
                DHCPS_LOG("\n");
            }

#endif
        }
    }

#if DHCPS_DEBUG
    DHCPS_LOG("dhcps: handle_dhcp-> parse_msg(p)\n");
#endif

    state = parse_msg(dhcps, pmsg_dhcps, tlen - 240);
#ifdef LWIP_HOOK_DHCPS_POST_STATE
    state = LWIP_HOOK_DHCPS_POST_STATE(pmsg_dhcps, malloc_len, state);
#endif /* LWIP_HOOK_DHCPS_POST_STATE */

    switch (state) {
        case DHCPS_STATE_OFFER://1
#if DHCPS_DEBUG
            DHCPS_LOG("dhcps: handle_dhcp-> DHCPD_STATE_OFFER\n");
#endif
            send_offer(dhcps, pmsg_dhcps, malloc_len);
            break;

        case DHCPS_STATE_ACK://3
#if DHCPS_DEBUG
            DHCPS_LOG("dhcps: handle_dhcp-> DHCPD_STATE_ACK\n");
#endif
            send_ack(dhcps, pmsg_dhcps, malloc_len);
            break;

        case DHCPS_STATE_NAK://4
#if DHCPS_DEBUG
            DHCPS_LOG("dhcps: handle_dhcp-> DHCPD_STATE_NAK\n");
#endif
            send_nak(dhcps, pmsg_dhcps, malloc_len);
            break;

        default :
            break;
    }

#if DHCPS_DEBUG
    DHCPS_LOG("dhcps: handle_dhcp-> pbuf_free(p)\n");
#endif
    pbuf_free(p);
    free(pmsg_dhcps);
    pmsg_dhcps = NULL;
}

/******************************************************************************
 * FunctionName : dhcps_poll_set
 * Description  : set ip poll from start to end for station
 * Parameters   : ip -- The current ip addr
 * Returns      : none
*******************************************************************************/
static void dhcps_poll_set(dhcps_t *dhcps, u32_t ip)
{
    u32_t server_ip = 0;
    u32_t start_ip = 0;
    u32_t end_ip = 0;
    u32_t range_start_ip = 0;
    u32_t range_end_ip = 0;
    dhcps_lease_t *dhcps_poll = &dhcps->dhcps_poll;
    if (dhcps_poll->enable == true) {
        server_ip = htonl(ip);
        start_ip = htonl(dhcps_poll->start_ip.addr);
        end_ip = htonl(dhcps_poll->end_ip.addr);

        /*config ip information can't contain local ip*/
        if ((server_ip >= start_ip) && (server_ip <= end_ip)) {
            dhcps_poll->enable = false;
        } else {
            /*config ip information must be in the same segment as the local ip*/

            if (!ip4_addr_netcmp(&dhcps_poll->start_ip, &dhcps->server_address, &dhcps->dhcps_mask)
                  || !ip4_addr_netcmp(&dhcps_poll->end_ip, &dhcps->server_address, &dhcps->dhcps_mask)
                  || (end_ip - start_ip + 1 > DHCPS_MAX_LEASE)) {
                dhcps_poll->enable = false;
            }
        }
    }

    if (dhcps_poll->enable == false) {
        server_ip = htonl(ip);
        range_start_ip = server_ip & htonl(dhcps->dhcps_mask.addr);
        range_end_ip = range_start_ip | ~htonl(dhcps->dhcps_mask.addr);

        if (server_ip - range_start_ip > range_end_ip - server_ip) {
            range_start_ip = range_start_ip + 1;
            range_end_ip = server_ip - 1;
        } else {
            range_start_ip = server_ip + 1;
            range_end_ip = range_end_ip - 1;
        }
        if (range_end_ip - range_start_ip + 1 > DHCPS_MAX_LEASE) {
            range_end_ip = range_start_ip + DHCPS_MAX_LEASE - 1;
        }
        bzero(dhcps_poll, sizeof(*dhcps_poll));
        dhcps_poll->start_ip.addr = range_start_ip;
        dhcps_poll->end_ip.addr = range_end_ip;
        dhcps_poll->start_ip.addr = htonl(dhcps_poll->start_ip.addr);
        dhcps_poll->end_ip.addr = htonl(dhcps_poll->end_ip.addr);
        dhcps_poll->enable = true;
    }

}


/******************************************************************************
 * FunctionName : dhcps_set_new_lease_cb
 * Description  : set callback for dhcp server when it assign an IP
 *                to the connected dhcp client
 * Parameters   : cb -- callback for dhcp server
 * Returns      : ERR_OK on success
*******************************************************************************/
err_t dhcps_set_new_lease_cb(dhcps_t *dhcps, dhcps_cb_t cb, void* cb_arg)
{
    if (dhcps == NULL) {
        return ERR_ARG;
    }
    dhcps->dhcps_cb = cb;
    dhcps->dhcps_cb_arg = cb_arg;
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : dhcps_start
 * Description  : start dhcp server function
 * Parameters   : netif -- The current netif addr
 *              : info  -- The current ip info
 * Returns      : none
*******************************************************************************/
err_t dhcps_start(dhcps_t *dhcps, struct netif *netif, ip4_addr_t ip)
{
    if (dhcps == NULL || netif == NULL) {
        return ERR_ARG;
    }
    dhcps->dhcps_netif = netif;
    if (dhcps->dhcps_pcb != NULL) {
        udp_remove(dhcps->dhcps_pcb);
    }

    dhcps->dhcps_pcb = udp_new();

    if (dhcps->dhcps_pcb == NULL || ip4_addr_isany_val(ip)) {
        DHCPS_LOG("dhcps_start(): could not obtain pcb\n");
        return ERR_ARG;
    }

    IP4_ADDR(&dhcps->broadcast_dhcps, 255, 255, 255, 255);

    dhcps->server_address.addr = ip.addr;
    DHCP_CHECK_SUBNET_MASK_IP(htonl(dhcps->dhcps_mask.addr));
    DHCP_CHECK_IP_MATCH_SUBNET_MASK(htonl(dhcps->dhcps_mask.addr), htonl(ip.addr));
    dhcps_poll_set(dhcps, dhcps->server_address.addr);

    dhcps->client_address_plus.addr = dhcps->dhcps_poll.start_ip.addr;

    udp_bind_netif(dhcps->dhcps_pcb, dhcps->dhcps_netif);
    udp_bind(dhcps->dhcps_pcb, &netif->ip_addr, DHCPS_SERVER_PORT);
    udp_recv(dhcps->dhcps_pcb, handle_dhcp, dhcps);
#if DHCPS_DEBUG
    DHCPS_LOG("dhcps:dhcps_start->udp_recv function Set a receive callback handle_dhcp for UDP_PCB pcb_dhcps\n");
#endif
    dhcps->state = DHCPS_HANDLE_STARTED;
    sys_timeout(DHCP_COARSE_TIMER_MSECS, dhcps_tmr, dhcps);
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : dhcps_stop
 * Description  : stop dhcp server function
 * Parameters   : netif -- The current netif addr
 * Returns      : none
*******************************************************************************/
err_t dhcps_stop(dhcps_t *dhcps, struct netif *netif)
{
    if (dhcps == NULL || netif == NULL || dhcps->dhcps_netif != netif) {
#if DHCPS_DEBUG
        DHCPS_LOG("dhcps_stop: netif is NULL or invalid\n");
#endif
        return ERR_ARG;
    }

    if (dhcps->dhcps_pcb != NULL) {
        udp_disconnect(dhcps->dhcps_pcb);
        udp_remove(dhcps->dhcps_pcb);
        dhcps->dhcps_pcb = NULL;
    }

    list_node *pnode = NULL;
    list_node *pback_node = NULL;
    pnode = dhcps->plist;

    while (pnode != NULL) {
        pback_node = pnode;
        pnode = pback_node->pnext;
        node_remove_from_list(&dhcps->plist, pback_node);
        free(pback_node->pnode);
        pback_node->pnode = NULL;
        free(pback_node);
        pback_node = NULL;
    }
    sys_untimeout(dhcps_tmr, dhcps);
    dhcps->state = DHCPS_HANDLE_STOPPED;
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : kill_oldest_dhcps_pool
 * Description  : remove the oldest node from list
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void kill_oldest_dhcps_pool(dhcps_t *dhcps)
{
    list_node *pre = NULL, *p = NULL;
    list_node *minpre = NULL, *minp = NULL;
    struct dhcps_pool *pdhcps_pool = NULL, *pmin_pool = NULL;
    pre = dhcps->plist;
    assert(pre != NULL && pre->pnext != NULL); // Expect the list to have at least 2 nodes
    p = pre->pnext;
    minpre = pre;
    minp = pre;

    while (p != NULL) {
        pdhcps_pool = p->pnode;
        pmin_pool = minp->pnode;

        if (pdhcps_pool->lease_timer < pmin_pool->lease_timer) {
            minp = p;
            minpre = pre;
        }

        pre = p;
        p = p->pnext;
    }
    if (minp == dhcps->plist) {
        dhcps->plist = minp->pnext;
    } else {
        minpre->pnext = minp->pnext;
    }
    free(minp->pnode);
    minp->pnode = NULL;
    free(minp);
    minp = NULL;
}

/******************************************************************************
 * FunctionName : dhcps_coarse_tmr
 * Description  : the lease time count
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
static void dhcps_tmr(void *arg)
{
    dhcps_t *dhcps = arg;
    dhcps_handle_state state = dhcps->state;
    if (state == DHCPS_HANDLE_DELETE_PENDING) {
        free(dhcps);
        return;
    }

    if (state != DHCPS_HANDLE_STARTED) {
        return;
    }
    sys_timeout(DHCP_COARSE_TIMER_MSECS, dhcps_tmr, dhcps);
    u8_t num_dhcps_pool = 0;
    list_node *pback_node = NULL;
    list_node *pnode = NULL;
    struct dhcps_pool *pdhcps_pool = NULL;
    pnode = dhcps->plist;

    while (pnode != NULL) {
        pdhcps_pool = pnode->pnode;
        pdhcps_pool->lease_timer --;

        if (pdhcps_pool->lease_timer == 0) {
            pback_node = pnode;
            pnode = pback_node->pnext;
            node_remove_from_list(&dhcps->plist, pback_node);
            free(pback_node->pnode);
            pback_node->pnode = NULL;
            free(pback_node);
            pback_node = NULL;
        } else {
            pnode = pnode ->pnext;
            num_dhcps_pool ++;
        }
    }

    if (num_dhcps_pool > MAX_STATION_NUM) {
        kill_oldest_dhcps_pool(dhcps);
    }
}

/******************************************************************************
 * FunctionName : dhcp_search_ip_on_mac
 * Description  : Search ip address based on mac address
 * Parameters   : mac -- The MAC addr
 *				  ip  -- The IP info
 * Returns      : true or false
*******************************************************************************/
bool dhcp_search_ip_on_mac(dhcps_t *dhcps, u8_t *mac, ip4_addr_t *ip)
{
    struct dhcps_pool *pdhcps_pool = NULL;
    list_node *pback_node = NULL;
    bool ret = false;

    if (dhcps == NULL) {
        return false;
    }

    for (pback_node = dhcps->plist; pback_node != NULL; pback_node = pback_node->pnext) {
        pdhcps_pool = pback_node->pnode;

        if (memcmp(pdhcps_pool->mac, mac, sizeof(pdhcps_pool->mac)) == 0) {
            memcpy(&ip->addr, &pdhcps_pool->ip.addr, sizeof(pdhcps_pool->ip.addr));
            ret = true;
            break;
        }
    }

    return ret;
}

/******************************************************************************
 * FunctionName : dhcps_dns_setserver_by_type
 * Description  : Set the DNS server address for dhcpserver with a specific type
 * Parameters   : dnsserver -- The DNS server address
 *                type      -- The DNS type
 * Returns      : ERR_ARG if invalid handle, ERR_VAL if invalid type, ERR_OK on success
*******************************************************************************/
err_t dhcps_dns_setserver_by_type(dhcps_t *dhcps, const ip_addr_t *dnsserver, dns_type_t type)
{
    if (dhcps == NULL) {
        return ERR_ARG;
    }

    if (type >= DNS_TYPE_MAX) {
        return ERR_VAL;
    }

    if (dnsserver != NULL) {
        dhcps->dns_server[type] = *(ip_2_ip4(dnsserver));
    } else {
        dhcps->dns_server[type] = *(ip_2_ip4(IP_ADDR_ANY));
    }
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : dhcps_dns_setserver
 * Description  : Set the main DNS server address for dhcpserver
 * Parameters   : dnsserver -- The DNS server address
 * Returns      : ERR_ARG if invalid handle, ERR_VAL if invalid type, ERR_OK on success
*******************************************************************************/
err_t dhcps_dns_setserver(dhcps_t *dhcps, const ip_addr_t *dnsserver)
{
    return dhcps_dns_setserver_by_type(dhcps, dnsserver, DNS_TYPE_MAIN);
}

/******************************************************************************
 * FunctionName : dhcps_dns_getserver_by_type
 * Description  : Get the DNS server address for dhcpserver
 * Parameters   : dnsserver -- The DNS server address
 *                type      -- The DNS type
 * Returns      : ERR_ARG if invalid handle, ERR_VAL if invalid type, ERR_OK on success
*******************************************************************************/
err_t dhcps_dns_getserver_by_type(dhcps_t *dhcps, ip4_addr_t *dnsserver, dns_type_t type)
{
    if ((dhcps == NULL) || (dnsserver == NULL)) {
        return ERR_ARG;
    }

    if (type >= DNS_TYPE_MAX) {
        return ERR_VAL;
    }

    *dnsserver = dhcps->dns_server[type];
    return ERR_OK;
}

/******************************************************************************
 * FunctionName : dhcps_dns_getserver_by_type
 * Description  : Get the main DNS server address for dhcpserver
 * Parameters   : dnsserver -- The DNS server address
 * Returns      : ERR_ARG if invalid handle, ERR_VAL if invalid type, ERR_OK on success
*******************************************************************************/
err_t dhcps_dns_getserver(dhcps_t *dhcps, ip4_addr_t *dnsserver)
{
    return dhcps_dns_getserver_by_type(dhcps, dnsserver, DNS_TYPE_MAIN);
}

#if CONFIG_LWIP_DHCPS_REPORT_CLIENT_HOSTNAME
bool dhcps_get_hostname_on_mac(dhcps_t *dhcps, const u8_t *mac, char *out, size_t out_len)
{
    if ((dhcps == NULL) || (mac == NULL) || (out == NULL) || (out_len == 0)) {
        return false;
    }
    list_node *pnode = dhcps->plist;
    while (pnode) {
        struct dhcps_pool *pool = pnode->pnode;
        if (memcmp(pool->mac, mac, sizeof(pool->mac)) == 0) {
            size_t maxcpy = (out_len > 0) ? out_len - 1 : 0;
            size_t srclen = 0;
            /* hostname may be empty string */
            srclen = strnlen(pool->hostname, CONFIG_LWIP_DHCPS_MAX_HOSTNAME_LEN);
            if (maxcpy > 0) {
                if (srclen > maxcpy) srclen = maxcpy;
                if (srclen) memcpy(out, pool->hostname, srclen);
                out[srclen] = '\0';
            }
            return true;
        }
        pnode = pnode->pnext;
    }
    return false;
}
#else
bool dhcps_get_hostname_on_mac(dhcps_t *dhcps, const u8_t *mac, char *out, size_t out_len)
{
    LWIP_UNUSED_ARG(dhcps);
    LWIP_UNUSED_ARG(mac);
    if (out && out_len) {
        out[0] = '\0';
    }
    return false;
}
#endif
#endif // ESP_DHCPS
