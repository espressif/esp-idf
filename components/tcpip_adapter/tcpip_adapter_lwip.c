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

#include <stdio.h>
#include <string.h>

#include "tcpip_adapter.h"

#if CONFIG_TCPIP_LWIP

#include "lwip/inet.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/nd6.h"
#include "lwip/priv/tcpip_priv.h"
#if LWIP_DNS /* don't build if not configured for use in lwipopts.h */
#include "lwip/dns.h"
#endif
#include "netif/wlanif.h"
#include "netif/ethernetif.h"

#include "apps/dhcpserver.h"

#include "esp_event.h"
#include "esp_log.h"

static struct netif *esp_netif[TCPIP_ADAPTER_IF_MAX];
static tcpip_adapter_ip_info_t esp_ip[TCPIP_ADAPTER_IF_MAX];
static tcpip_adapter_ip6_info_t esp_ip6[TCPIP_ADAPTER_IF_MAX];

static tcpip_adapter_dhcp_status_t dhcps_status = TCPIP_ADAPTER_DHCP_INIT;
static tcpip_adapter_dhcp_status_t dhcpc_status[TCPIP_ADAPTER_IF_MAX] = {TCPIP_ADAPTER_DHCP_INIT};
static esp_err_t tcpip_adapter_start_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_stop_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_up_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_down_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_set_ip_info_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_create_ip6_linklocal_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_dhcps_start_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_dhcps_stop_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_dhcpc_start_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_dhcpc_stop_api(tcpip_adapter_api_msg_t * msg);
static esp_err_t tcpip_adapter_set_hostname_api(tcpip_adapter_api_msg_t * msg);
static sys_sem_t api_sync_sem = NULL;
extern sys_thread_t g_lwip_task;

#define TAG "tcpip_adapter"

static void tcpip_adapter_api_cb(void* api_msg)
{
    tcpip_adapter_api_msg_t *msg = (tcpip_adapter_api_msg_t*)api_msg;

    if (!msg || !msg->api_fn) {
        ESP_LOGD(TAG, "null msg/api_fn");
        return;
    }

    msg->ret = msg->api_fn(msg);
    ESP_LOGD(TAG, "call api in lwip: ret=0x%x, give sem", msg->ret);
    sys_sem_signal(&api_sync_sem);

    return;
}

void tcpip_adapter_init(void)
{
    static bool tcpip_inited = false;
    int ret;

    if (tcpip_inited == false) {
        tcpip_inited = true;

        tcpip_init(NULL, NULL);

        IP4_ADDR(&esp_ip[TCPIP_ADAPTER_IF_AP].ip, 192, 168 , 4, 1);
        IP4_ADDR(&esp_ip[TCPIP_ADAPTER_IF_AP].gw, 192, 168 , 4, 1);
        IP4_ADDR(&esp_ip[TCPIP_ADAPTER_IF_AP].netmask, 255, 255 , 255, 0);
        ret = sys_sem_new(&api_sync_sem, 0);
        if (ERR_OK != ret) {
            ESP_LOGD(TAG, "tcpip adatper api sync sem init fail");
        }
    }
}

static netif_init_fn tcpip_if_to_netif_init_fn(tcpip_adapter_if_t tcpip_if)
{
    switch(tcpip_if) {
#ifdef CONFIG_WIFI_ENABLED
    case TCPIP_ADAPTER_IF_AP:
    case TCPIP_ADAPTER_IF_STA:
        return wlanif_init;
#endif
#ifdef CONFIG_ETHERNET
        case TCPIP_ADAPTER_IF_ETH:
            return ethernetif_init;
#endif
        default:
            return NULL;
    }
}

static int tcpip_adapter_ipc_check(tcpip_adapter_api_msg_t *msg)
{
#if TCPIP_ADAPTER_TRHEAD_SAFE
    xTaskHandle local_task = xTaskGetCurrentTaskHandle();

    if (local_task == g_lwip_task) {
        return TCPIP_ADAPTER_IPC_LOCAL;
    }

    tcpip_send_api_msg((tcpip_callback_fn)tcpip_adapter_api_cb, msg, &api_sync_sem);

    return TCPIP_ADAPTER_IPC_REMOTE;
#else
    return TCPIP_ADAPTER_IPC_LOCAL;
#endif
}

esp_err_t tcpip_adapter_start(tcpip_adapter_if_t tcpip_if, uint8_t *mac, tcpip_adapter_ip_info_t *ip_info)
{
    netif_init_fn netif_init;

    TCPIP_ADAPTER_IPC_CALL(tcpip_if, mac, ip_info, 0, tcpip_adapter_start_api);

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || mac == NULL || ip_info == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (esp_netif[tcpip_if] == NULL) {
        esp_netif[tcpip_if] = calloc(1, sizeof(*esp_netif[tcpip_if]));
        if (esp_netif[tcpip_if] == NULL) {
            return ESP_ERR_NO_MEM;
        }
        memcpy(esp_netif[tcpip_if]->hwaddr, mac, NETIF_MAX_HWADDR_LEN);

        netif_init = tcpip_if_to_netif_init_fn(tcpip_if);
        assert(netif_init != NULL);
        netif_add(esp_netif[tcpip_if], &ip_info->ip, &ip_info->netmask, &ip_info->gw, NULL, netif_init, tcpip_input);
    }

    if (tcpip_if == TCPIP_ADAPTER_IF_AP) {
        netif_set_up(esp_netif[tcpip_if]);

        if (dhcps_status == TCPIP_ADAPTER_DHCP_INIT) {
            dhcps_start(esp_netif[tcpip_if], ip_info->ip);

            ESP_LOGD(TAG, "dhcp server start:(ip: " IPSTR ", mask: " IPSTR ", gw: " IPSTR ")",
                   IP2STR(&ip_info->ip), IP2STR(&ip_info->netmask), IP2STR(&ip_info->gw));

            dhcps_status = TCPIP_ADAPTER_DHCP_STARTED;
        }
    }

    /* if ap is on, choose ap as default if */
    if (esp_netif[TCPIP_ADAPTER_IF_AP]) {
        netif_set_default(esp_netif[TCPIP_ADAPTER_IF_AP]);
    } else if (esp_netif[TCPIP_ADAPTER_IF_STA]) {
        netif_set_default(esp_netif[TCPIP_ADAPTER_IF_STA]);
    } else if (esp_netif[TCPIP_ADAPTER_IF_ETH] ) {
        netif_set_default(esp_netif[TCPIP_ADAPTER_IF_ETH]);
    }

    return ESP_OK;
}

static esp_err_t tcpip_adapter_start_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_start(msg->tcpip_if, msg->mac, msg->ip_info);
}

esp_err_t tcpip_adapter_stop(tcpip_adapter_if_t tcpip_if)
{
    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, 0, tcpip_adapter_stop_api);

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (esp_netif[tcpip_if] == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
    }

    if (tcpip_if == TCPIP_ADAPTER_IF_AP) {
        dhcps_stop(esp_netif[tcpip_if]);    // TODO: dhcps checks status by its self
        if (TCPIP_ADAPTER_DHCP_STOPPED != dhcps_status) {
            dhcps_status = TCPIP_ADAPTER_DHCP_INIT;
        }
    } else if (tcpip_if == TCPIP_ADAPTER_IF_STA || tcpip_if == TCPIP_ADAPTER_IF_ETH) {
        dhcp_release(esp_netif[tcpip_if]);
        dhcp_stop(esp_netif[tcpip_if]);
        dhcp_cleanup(esp_netif[tcpip_if]);

        dhcpc_status[tcpip_if] = TCPIP_ADAPTER_DHCP_INIT;

        ip4_addr_set_zero(&esp_ip[tcpip_if].ip);
        ip4_addr_set_zero(&esp_ip[tcpip_if].gw);
        ip4_addr_set_zero(&esp_ip[tcpip_if].netmask);
    }

    netif_remove(esp_netif[tcpip_if]);

    free(esp_netif[tcpip_if]);
    esp_netif[tcpip_if] = NULL;

    /* in ap + sta mode, if stop ap, choose sta as default if */
    if (esp_netif[TCPIP_ADAPTER_IF_STA] && tcpip_if == TCPIP_ADAPTER_IF_AP) {
        netif_set_default(esp_netif[TCPIP_ADAPTER_IF_STA]);
    }

    return ESP_OK;
}

static esp_err_t tcpip_adapter_stop_api(tcpip_adapter_api_msg_t * msg)
{
    msg->ret = tcpip_adapter_stop(msg->tcpip_if);
    return msg->ret;
}

esp_err_t tcpip_adapter_up(tcpip_adapter_if_t tcpip_if)
{
    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, 0, tcpip_adapter_up_api);

    if (tcpip_if == TCPIP_ADAPTER_IF_STA ||  tcpip_if == TCPIP_ADAPTER_IF_ETH ) {
        if (esp_netif[tcpip_if] == NULL) {
            return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        }

        /* use last obtained ip, or static ip */
        netif_set_addr(esp_netif[tcpip_if], &esp_ip[tcpip_if].ip, &esp_ip[tcpip_if].netmask, &esp_ip[tcpip_if].gw);
        netif_set_up(esp_netif[tcpip_if]);
    }

    return ESP_OK;
}

static esp_err_t tcpip_adapter_up_api(tcpip_adapter_api_msg_t * msg)
{
    msg->ret = tcpip_adapter_up(msg->tcpip_if);
    return msg->ret;
}

esp_err_t tcpip_adapter_down(tcpip_adapter_if_t tcpip_if)
{
    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, 0, tcpip_adapter_down_api);

    if (tcpip_if == TCPIP_ADAPTER_IF_STA ||  tcpip_if == TCPIP_ADAPTER_IF_ETH ) {
        if (esp_netif[tcpip_if] == NULL) {
            return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        }

        if (dhcpc_status[tcpip_if] == TCPIP_ADAPTER_DHCP_STARTED) {
            dhcp_stop(esp_netif[tcpip_if]);

            dhcpc_status[tcpip_if] = TCPIP_ADAPTER_DHCP_INIT;

            ip4_addr_set_zero(&esp_ip[tcpip_if].ip);
            ip4_addr_set_zero(&esp_ip[tcpip_if].gw);
            ip4_addr_set_zero(&esp_ip[tcpip_if].netmask);
        }

        /* Modify ip address to trigger tcp/udp pcb cleanup */
        netif_set_addr(esp_netif[tcpip_if], IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY);

        netif_set_down(esp_netif[tcpip_if]);
    }

    return ESP_OK;
}

static esp_err_t tcpip_adapter_down_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_down(msg->tcpip_if);
}

esp_err_t tcpip_adapter_get_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    struct netif *p_netif;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || ip_info == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];

    if (p_netif != NULL && netif_is_up(p_netif)) {
        ip4_addr_set(&ip_info->ip, ip_2_ip4(&p_netif->ip_addr));
        ip4_addr_set(&ip_info->netmask, ip_2_ip4(&p_netif->netmask));
        ip4_addr_set(&ip_info->gw, ip_2_ip4(&p_netif->gw));

        return ESP_OK;
    }

    ip4_addr_copy(ip_info->ip, esp_ip[tcpip_if].ip);
    ip4_addr_copy(ip_info->gw, esp_ip[tcpip_if].gw);
    ip4_addr_copy(ip_info->netmask, esp_ip[tcpip_if].netmask);

    return ESP_OK;
}

esp_err_t tcpip_adapter_set_ip_info(tcpip_adapter_if_t tcpip_if, tcpip_adapter_ip_info_t *ip_info)
{
    struct netif *p_netif;
    tcpip_adapter_dhcp_status_t status;

    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, ip_info, 0, tcpip_adapter_set_ip_info_api);

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || ip_info == NULL ||
            ip4_addr_isany_val(ip_info->ip) || ip4_addr_isany_val(ip_info->netmask)) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (tcpip_if == TCPIP_ADAPTER_IF_AP) {
        tcpip_adapter_dhcps_get_status(tcpip_if, &status);

        if (status != TCPIP_ADAPTER_DHCP_STOPPED) {
            return ESP_ERR_TCPIP_ADAPTER_DHCP_NOT_STOPPED;
        }
    } else if (tcpip_if == TCPIP_ADAPTER_IF_STA || tcpip_if == TCPIP_ADAPTER_IF_ETH ) {
        tcpip_adapter_dhcpc_get_status(tcpip_if, &status);

        if (status != TCPIP_ADAPTER_DHCP_STOPPED) {
            return ESP_ERR_TCPIP_ADAPTER_DHCP_NOT_STOPPED;
        }
#if LWIP_DNS /* don't build if not configured for use in lwipopts.h */
        u8_t numdns = 0;
        for (numdns = 0; numdns < DNS_MAX_SERVERS; numdns ++) {
            dns_setserver(numdns, NULL);
        }
#endif
    }

    ip4_addr_copy(esp_ip[tcpip_if].ip, ip_info->ip);
    ip4_addr_copy(esp_ip[tcpip_if].gw, ip_info->gw);
    ip4_addr_copy(esp_ip[tcpip_if].netmask, ip_info->netmask);

    p_netif = esp_netif[tcpip_if];

    if (p_netif != NULL && netif_is_up(p_netif)) {
        netif_set_addr(p_netif, &ip_info->ip, &ip_info->netmask, &ip_info->gw);
    }

    return ESP_OK;
}

static esp_err_t tcpip_adapter_set_ip_info_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_set_ip_info(msg->tcpip_if, msg->ip_info);
}

static void tcpip_adapter_nd6_cb(struct netif *p_netif, uint8_t ip_idex)
{
    tcpip_adapter_ip6_info_t *ip6_info;

    if (!p_netif) {
        ESP_LOGD(TAG, "null p_netif=%p", p_netif);
        return;
    }

    if (p_netif == esp_netif[TCPIP_ADAPTER_IF_STA]) {
        ip6_info = &esp_ip6[TCPIP_ADAPTER_IF_STA];
    } else if (p_netif == esp_netif[TCPIP_ADAPTER_IF_AP]) {
        ip6_info = &esp_ip6[TCPIP_ADAPTER_IF_AP];
    } else {
        return;
    }

    system_event_t evt;

    ip6_addr_set(&ip6_info->ip, ip_2_ip6(&p_netif->ip6_addr[ip_idex]));

    //notify event
    evt.event_id = SYSTEM_EVENT_AP_STA_GOT_IP6;
    memcpy(&evt.event_info.got_ip6.ip6_info, ip6_info, sizeof(tcpip_adapter_ip6_info_t));
    esp_event_send(&evt);
}

esp_err_t tcpip_adapter_create_ip6_linklocal(tcpip_adapter_if_t tcpip_if)
{
    struct netif *p_netif;

    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, 0, tcpip_adapter_create_ip6_linklocal_api);

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];
    if (p_netif != NULL && netif_is_up(p_netif)) {
        netif_create_ip6_linklocal_address(p_netif, 1);
        nd6_set_cb(p_netif, tcpip_adapter_nd6_cb);

        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

static esp_err_t tcpip_adapter_create_ip6_linklocal_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_create_ip6_linklocal(msg->tcpip_if);
}

esp_err_t tcpip_adapter_get_ip6_linklocal(tcpip_adapter_if_t tcpip_if, ip6_addr_t *if_ip6)
{
    struct netif *p_netif;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || if_ip6 == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];
    if (p_netif != NULL && netif_is_up(p_netif) && ip6_addr_ispreferred(netif_ip6_addr_state(p_netif, 0))) {
        memcpy(if_ip6, &p_netif->ip6_addr[0], sizeof(ip6_addr_t));
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}

#if 0
esp_err_t tcpip_adapter_get_mac(tcpip_adapter_if_t tcpip_if, uint8_t mac[6])
{
    struct netif *p_netif;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || mac == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];

    if (p_netif != NULL) {
        memcpy(mac, p_netif->hwaddr, NETIF_MAX_HWADDR_LEN);

        return ESP_OK;
    }

    return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
}

esp_err_t tcpip_adapter_set_mac(tcpip_adapter_if_t tcpip_if, uint8_t mac[6])
{
    struct netif *p_netif;

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || mac == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];

    if (p_netif != NULL) {
        memcpy(p_netif->hwaddr, mac, NETIF_MAX_HWADDR_LEN);

        return ESP_OK;
    }

    return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
}
#endif

esp_err_t tcpip_adapter_dhcps_option(tcpip_adapter_option_mode_t opt_op, tcpip_adapter_option_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    void *opt_info = dhcps_option_info(opt_id, opt_len);

    if (opt_info == NULL || opt_val == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (opt_op == TCPIP_ADAPTER_OP_GET) {
        if (dhcps_status == TCPIP_ADAPTER_DHCP_STOPPED) {
            return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED;
        }

        switch (opt_id) {
        case IP_ADDRESS_LEASE_TIME: {
            *(uint32_t *)opt_val = *(uint32_t *)opt_info;
            break;
        }
        case REQUESTED_IP_ADDRESS: {
            memcpy(opt_val, opt_info, opt_len);
            break;
        }
        case ROUTER_SOLICITATION_ADDRESS: {
            *(uint8_t *)opt_val = (*(uint8_t *)opt_info) & OFFER_ROUTER;
            break;
        }
        default:
            break;
        }
    } else if (opt_op == TCPIP_ADAPTER_OP_SET) {
        if (dhcps_status == TCPIP_ADAPTER_DHCP_STARTED) {
            return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED;
        }

        switch (opt_id) {
        case IP_ADDRESS_LEASE_TIME: {
            if (*(uint32_t *)opt_val != 0) {
                *(uint32_t *)opt_info = *(uint32_t *)opt_val;
            } else {
                *(uint32_t *)opt_info = DHCPS_LEASE_TIME_DEF;
            }
            break;
        }
        case REQUESTED_IP_ADDRESS: {
            tcpip_adapter_ip_info_t info;
            uint32_t softap_ip = 0;
            uint32_t start_ip = 0;
            uint32_t end_ip = 0;
            dhcps_lease_t *poll = opt_val;

            if (poll->enable) {
                memset(&info, 0x00, sizeof(tcpip_adapter_ip_info_t));
                tcpip_adapter_get_ip_info(ESP_IF_WIFI_AP, &info);
                softap_ip = htonl(info.ip.addr);
                start_ip = htonl(poll->start_ip.addr);
                end_ip = htonl(poll->end_ip.addr);

                /*config ip information can't contain local ip*/
                if ((start_ip <= softap_ip) && (softap_ip <= end_ip)) {
                    return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
                }

                /*config ip information must be in the same segment as the local ip*/
                softap_ip >>= 8;
                if ((start_ip >> 8 != softap_ip)
                        || (end_ip >> 8 != softap_ip)) {
                    return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
                }

                if (end_ip - start_ip > DHCPS_MAX_LEASE) {
                    return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
                }
            }

            memcpy(opt_info, opt_val, opt_len);
            break;
        }
        case ROUTER_SOLICITATION_ADDRESS: {
            *(uint8_t *)opt_info = (*(uint8_t *)opt_val) & OFFER_ROUTER;
            break;
        }
        default:
            break;
        }
    } else {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcps_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    *status = dhcps_status;

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcps_start(tcpip_adapter_if_t tcpip_if)
{
    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, 0, tcpip_adapter_dhcps_start_api);

    /* only support ap now */
    if (tcpip_if != TCPIP_ADAPTER_IF_AP || tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        ESP_LOGD(TAG, "dhcp server invalid if=%d", tcpip_if);
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (dhcps_status != TCPIP_ADAPTER_DHCP_STARTED) {
        struct netif *p_netif = esp_netif[tcpip_if];

        if (p_netif != NULL && netif_is_up(p_netif)) {
            tcpip_adapter_ip_info_t default_ip;
            tcpip_adapter_get_ip_info(ESP_IF_WIFI_AP, &default_ip);
            dhcps_start(p_netif, default_ip.ip);
            dhcps_status = TCPIP_ADAPTER_DHCP_STARTED;
            ESP_LOGD(TAG, "dhcp server start successfully");
            return ESP_OK;
        } else {
            ESP_LOGD(TAG, "dhcp server re init");
            dhcps_status = TCPIP_ADAPTER_DHCP_INIT;
            return ESP_OK;
        }
    }

    ESP_LOGD(TAG, "dhcp server already start");
    return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED;
}

static esp_err_t tcpip_adapter_dhcps_start_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_dhcps_start(msg->tcpip_if);
}


esp_err_t tcpip_adapter_dhcps_stop(tcpip_adapter_if_t tcpip_if)
{
    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, 0, tcpip_adapter_dhcps_stop_api);

    /* only support ap now */
    if (tcpip_if != TCPIP_ADAPTER_IF_AP || tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        ESP_LOGD(TAG, "dhcp server invalid if=%d", tcpip_if);
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (dhcps_status == TCPIP_ADAPTER_DHCP_STARTED) {
        struct netif *p_netif = esp_netif[tcpip_if];

        if (p_netif != NULL) {
            dhcps_stop(p_netif);
        } else {
            ESP_LOGD(TAG, "dhcp server if not ready");
            return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        }
    } else if (dhcps_status == TCPIP_ADAPTER_DHCP_STOPPED) {
        ESP_LOGD(TAG, "dhcp server already stoped");
        return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED;
    }

    ESP_LOGD(TAG, "dhcp server stop successfully");
    dhcps_status = TCPIP_ADAPTER_DHCP_STOPPED;
    return ESP_OK;
}

static esp_err_t tcpip_adapter_dhcps_stop_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_dhcps_stop(msg->tcpip_if);
}

esp_err_t tcpip_adapter_dhcpc_option(tcpip_adapter_option_mode_t opt_op, tcpip_adapter_option_id_t opt_id, void *opt_val, uint32_t opt_len)
{
    // TODO: when dhcp request timeout,change the retry count
    return ESP_OK;
}

static void tcpip_adapter_dhcpc_cb(struct netif *netif)
{
    if (!netif) {
        ESP_LOGD(TAG, "null netif=%p", netif);
        return;
    }

    if (netif != esp_netif[TCPIP_ADAPTER_IF_STA] && netif != esp_netif[TCPIP_ADAPTER_IF_ETH]) {
        ESP_LOGD(TAG, "err netif=%p", netif);
        return;
    }

    if ( !ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), IP4_ADDR_ANY) ) {
        tcpip_adapter_ip_info_t *ip_info = NULL;
        if( netif == esp_netif[TCPIP_ADAPTER_IF_STA] ) {
            ip_info = &esp_ip[TCPIP_ADAPTER_IF_STA];
        } else if(netif == esp_netif[TCPIP_ADAPTER_IF_ETH] ) {
            ip_info = &esp_ip[TCPIP_ADAPTER_IF_ETH];
        } 

        //check whether IP is changed
        if ( !ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), &ip_info->ip) ||
                !ip4_addr_cmp(ip_2_ip4(&netif->netmask), &ip_info->netmask) ||
                !ip4_addr_cmp(ip_2_ip4(&netif->gw), &ip_info->gw) ) {
            system_event_t evt;

            ip4_addr_set(&ip_info->ip, ip_2_ip4(&netif->ip_addr));
            ip4_addr_set(&ip_info->netmask, ip_2_ip4(&netif->netmask));
            ip4_addr_set(&ip_info->gw, ip_2_ip4(&netif->gw));

            //notify event
            if (netif == esp_netif[TCPIP_ADAPTER_IF_STA]) {
                evt.event_id = SYSTEM_EVENT_STA_GOT_IP;
            } else if (netif == esp_netif[TCPIP_ADAPTER_IF_ETH]) {
                evt.event_id = SYSTEM_EVENT_ETH_GOT_IP;
            }

            memcpy(&evt.event_info.got_ip.ip_info, ip_info, sizeof(tcpip_adapter_ip_info_t));

            esp_event_send(&evt);
        } else {
            ESP_LOGD(TAG, "ip unchanged");
        }
    }

    return;
}

esp_err_t tcpip_adapter_dhcpc_get_status(tcpip_adapter_if_t tcpip_if, tcpip_adapter_dhcp_status_t *status)
{
    *status = dhcpc_status[tcpip_if];

    return ESP_OK;
}

esp_err_t tcpip_adapter_dhcpc_start(tcpip_adapter_if_t tcpip_if)
{
    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, 0, tcpip_adapter_dhcpc_start_api);

    /* only support sta now, need to support ethernet */
    if ((tcpip_if != TCPIP_ADAPTER_IF_STA && tcpip_if != TCPIP_ADAPTER_IF_ETH)  || tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        ESP_LOGD(TAG, "dhcp client invalid if=%d", tcpip_if);
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (dhcpc_status[tcpip_if] != TCPIP_ADAPTER_DHCP_STARTED) {
        struct netif *p_netif = esp_netif[tcpip_if];

        ip4_addr_set_zero(&esp_ip[tcpip_if].ip);
        ip4_addr_set_zero(&esp_ip[tcpip_if].gw);
        ip4_addr_set_zero(&esp_ip[tcpip_if].netmask);

        if (p_netif != NULL) {
            if (netif_is_up(p_netif)) {
                ESP_LOGD(TAG, "dhcp client init ip/mask/gw to all-0");
                ip_addr_set_zero(&p_netif->ip_addr);
                ip_addr_set_zero(&p_netif->netmask);
                ip_addr_set_zero(&p_netif->gw);
            } else {
                ESP_LOGD(TAG, "dhcp client re init");
                dhcpc_status[tcpip_if] = TCPIP_ADAPTER_DHCP_INIT;
                return ESP_OK;
            }

            if (dhcp_start(p_netif) != ERR_OK) {
                ESP_LOGD(TAG, "dhcp client start failed");
                return ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED;
            }

            dhcp_set_cb(p_netif, tcpip_adapter_dhcpc_cb);

            ESP_LOGD(TAG, "dhcp client start successfully");
            dhcpc_status[tcpip_if] = TCPIP_ADAPTER_DHCP_STARTED;
            return ESP_OK;
        } else {
            ESP_LOGD(TAG, "dhcp client re init");
            dhcpc_status[tcpip_if] = TCPIP_ADAPTER_DHCP_INIT;
            return ESP_OK;
        }
    }

    ESP_LOGD(TAG, "dhcp client already started");
    return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STARTED;
}

static esp_err_t tcpip_adapter_dhcpc_start_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_dhcpc_start(msg->tcpip_if);
}

esp_err_t tcpip_adapter_dhcpc_stop(tcpip_adapter_if_t tcpip_if)
{
    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, 0, tcpip_adapter_dhcpc_stop_api);

    /* only support sta now, need to support ethernet */
    if (tcpip_if != TCPIP_ADAPTER_IF_STA || tcpip_if >= TCPIP_ADAPTER_IF_MAX) {
        ESP_LOGD(TAG, "dhcp client invalid if=%d", tcpip_if);
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (dhcpc_status[tcpip_if] == TCPIP_ADAPTER_DHCP_STARTED) {
        struct netif *p_netif = esp_netif[tcpip_if];

        if (p_netif != NULL) {
            dhcp_stop(p_netif);

            ip4_addr_set_zero(&esp_ip[tcpip_if].ip);
            ip4_addr_set_zero(&esp_ip[tcpip_if].gw);
            ip4_addr_set_zero(&esp_ip[tcpip_if].netmask);
        } else {
            ESP_LOGD(TAG, "dhcp client if not ready");
            return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
        }
    } else if (dhcpc_status[tcpip_if] == TCPIP_ADAPTER_DHCP_STOPPED) {
        ESP_LOGD(TAG, "dhcp client already stoped");
        return ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED;
    }

    ESP_LOGD(TAG, "dhcp client stop successfully");
    dhcpc_status[tcpip_if] = TCPIP_ADAPTER_DHCP_STOPPED;
    return ESP_OK;
}

static esp_err_t tcpip_adapter_dhcpc_stop_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_dhcpc_stop(msg->tcpip_if);
}

esp_err_t tcpip_adapter_eth_input(void *buffer, uint16_t len, void *eb)
{
    ethernetif_input(esp_netif[TCPIP_ADAPTER_IF_ETH], buffer, len);
    return ESP_OK;
}

esp_err_t tcpip_adapter_sta_input(void *buffer, uint16_t len, void *eb)
{
    wlanif_input(esp_netif[TCPIP_ADAPTER_IF_STA], buffer, len, eb);
    return ESP_OK;
}

esp_err_t tcpip_adapter_ap_input(void *buffer, uint16_t len, void *eb)
{
    wlanif_input(esp_netif[TCPIP_ADAPTER_IF_AP], buffer, len, eb);
    return ESP_OK;
}

#if 0
bool tcpip_dep_output(wifi_interface_t wifi_if, void *buffer, uint16_t len)
{

    return true;
}
#endif

esp_interface_t tcpip_adapter_get_esp_if(void *dev)
{
    struct netif *p_netif = (struct netif *)dev;

    if (p_netif == esp_netif[TCPIP_ADAPTER_IF_STA]) {
        return ESP_IF_WIFI_STA;
    } else if (p_netif == esp_netif[TCPIP_ADAPTER_IF_AP]) {
        return ESP_IF_WIFI_AP;
    } else if (p_netif == esp_netif[TCPIP_ADAPTER_IF_ETH]) {
        return ESP_IF_ETH;
    }

    return ESP_IF_MAX;
}

esp_err_t tcpip_adapter_get_sta_list(wifi_sta_list_t *wifi_sta_list, tcpip_adapter_sta_list_t *tcpip_sta_list)
{
    int i;

    if ((wifi_sta_list == NULL) || (tcpip_sta_list == NULL)) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    memset(tcpip_sta_list, 0, sizeof(tcpip_adapter_sta_list_t));
    tcpip_sta_list->num = wifi_sta_list->num;
    for (i = 0; i < wifi_sta_list->num; i++) {
        memcpy(tcpip_sta_list->sta[i].mac, wifi_sta_list->sta[i].mac, 6);
        dhcp_search_ip_on_mac(tcpip_sta_list->sta[i].mac, &tcpip_sta_list->sta[i].ip);
    }

    return ESP_OK;
}

esp_err_t tcpip_adapter_set_hostname(tcpip_adapter_if_t tcpip_if, const char *hostname)
{
#if LWIP_NETIF_HOSTNAME
    TCPIP_ADAPTER_IPC_CALL(tcpip_if, 0, 0, hostname, tcpip_adapter_set_hostname_api);
    struct netif *p_netif;
    static char hostinfo[TCPIP_ADAPTER_IF_MAX][TCPIP_HOSTNAME_MAX_SIZE + 1];

    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || hostname == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    if (strlen(hostname) > TCPIP_HOSTNAME_MAX_SIZE) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];
    if (p_netif != NULL) {
        memset(hostinfo[tcpip_if], 0, sizeof(hostinfo[tcpip_if]));
        strlcpy(hostinfo[tcpip_if], hostname, sizeof(hostinfo[tcpip_if]));
        p_netif->hostname = hostinfo[tcpip_if];
        return ESP_OK;
    } else {
        return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
    }
#else
    return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
#endif
}

static esp_err_t tcpip_adapter_set_hostname_api(tcpip_adapter_api_msg_t * msg)
{
    return tcpip_adapter_set_hostname(msg->tcpip_if, msg->hostname);
}

esp_err_t tcpip_adapter_get_hostname(tcpip_adapter_if_t tcpip_if, const char **hostname)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *p_netif = NULL;
    if (tcpip_if >= TCPIP_ADAPTER_IF_MAX || hostname == NULL) {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }

    p_netif = esp_netif[tcpip_if];
    if (p_netif != NULL) {
        *hostname = p_netif->hostname;
        return ESP_OK;
    } else {
        return ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS;
    }
#else
    return ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY;
#endif
}

#endif /* CONFIG_TCPIP_LWIP */
