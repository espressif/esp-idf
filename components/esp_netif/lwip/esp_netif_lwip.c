/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <inttypes.h>
#include <lwip/ip_addr.h>
#include <lwip/sockets.h>

#include "esp_check.h"
#include "esp_netif_lwip_internal.h"
#include "lwip/esp_netif_net_stack.h"


#include "esp_netif.h"
#include "esp_netif_private.h"
#include "esp_random.h"
#include "esp_system.h"

#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/mld6.h"
#include "lwip/prot/mld6.h"
#include "lwip/nd6.h"
#include "lwip/snmp.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/netif.h"
#include "lwip/etharp.h"
#if CONFIG_ESP_NETIF_BRIDGE_EN
#include "netif/bridgeif.h"
#endif // CONFIG_ESP_NETIF_BRIDGE_EN
#if LWIP_DNS /* don't build if not configured for use in lwipopts.h */
#include "lwip/dns.h"
#endif // LWIP_DNS

#if CONFIG_LWIP_HOOK_TCP_ISN_DEFAULT
#include "lwip_default_hooks.h"
#endif // CONFIG_LWIP_HOOK_TCP_ISN_DEFAULT

#include "esp_netif_lwip_ppp.h"
#if ESP_DHCPS
#include "dhcpserver/dhcpserver.h"
#include "dhcpserver/dhcpserver_options.h"
#endif
#include "netif/dhcp_state.h"
#include "esp_event.h"
#include "esp_log.h"
#if IP_NAPT
#include "lwip/lwip_napt.h"
#endif


//
// This is the main module implementing lwip interaction with esp-netif
//

#define ESP_NETIF_HOSTNAME_MAX_SIZE    32

#define DHCP_CB_CHANGE (LWIP_NSC_IPV4_SETTINGS_CHANGED | LWIP_NSC_IPV4_ADDRESS_CHANGED | LWIP_NSC_IPV4_GATEWAY_CHANGED | LWIP_NSC_IPV4_NETMASK_CHANGED)

/**
 * @brief lwip thread safe tcpip function utility macros
 */
#define _RUN_IN_LWIP_TASK(function, netif, param) { return esp_netif_lwip_ipc_call(function, netif, (void *)(param)); }

/**
 * @brief macros to check netif related data to evaluate interface type
 */
#if CONFIG_PPP_SUPPORT
#define _IS_NETIF_ANY_POINT2POINT_TYPE(netif) (netif->related_data && netif->related_data->is_point2point)
#else
#define _IS_NETIF_ANY_POINT2POINT_TYPE(netif) false
#endif
#define _RUN_IN_LWIP_TASK_IF_SUPPORTED(function, netif, param) \
{                                                              \
    if (_IS_NETIF_ANY_POINT2POINT_TYPE(netif)) {               \
        return ESP_ERR_NOT_SUPPORTED;                          \
    }                                                          \
    return esp_netif_lwip_ipc_call(function, netif, (void *)(param)); \
}

/**
 * @brief Utility macros to convert esp-ip addresses (both IPv6+IPv4 versions unconditionally)
 * and lwip-ip addresses (contain only enabled portion of the address for each IP stack)
*/

#if LWIP_IPV4 && LWIP_IPV6
#define ESPIP_TO_IP(espip, ip) memcpy((ip), (espip), sizeof(ip_addr_t));
#define IP_TO_ESPIP(ip, espip) memcpy((espip), (ip), sizeof(ip_addr_t));
#elif LWIP_IPV4
#define ESPIP_TO_IP(espip, ip) memcpy((ip), &((espip)->u_addr.ip4), sizeof(ip_addr_t));
#define IP_TO_ESPIP(ip, espip) do { memcpy(&((espip)->u_addr.ip4), (ip), sizeof(ip4_addr_t)); \
                                    (espip)->type = ESP_IPADDR_TYPE_V4;                       \
                               } while(0)
#elif LWIP_IPV6
#define ESPIP_TO_IP(espip, ip) memcpy((ip), &((espip)->u_addr.ip6), sizeof(ip_addr_t));
#define IP_TO_ESPIP(ip, espip) do { memcpy(&((espip)->u_addr.ip6), (ip), sizeof(ip6_addr_t)); \
                                    (espip)->type = ESP_IPADDR_TYPE_V6;                       \
                              } while(0)
#endif

/**
 * @brief If netif protocol not enabled in menuconfig, log the error and return appropriate code indicating failure
*/

#define LOG_NETIF_DISABLED_AND_DO(proto, action)                                                \
do {                                                                                            \
    ESP_LOGE(TAG, "%s not supported, please enable it in lwIP component configuration", proto); \
    action;                                                                                     \
} while(0)

//
//  Internal types
//
typedef enum esp_netif_action {
    ESP_NETIF_UNDEF,
    ESP_NETIF_STARTED,
    ESP_NETIF_STOPPED,
    ESP_NETIF_SET_DEFAULT,
} esp_netif_action_t;

//
//  Internal variables for this module
//
static const char *TAG = "esp_netif_lwip";

#if LWIP_ESP_NETIF_DATA
static u8_t lwip_netif_client_id = 0xff;
#endif
static esp_netif_t *s_last_default_esp_netif = NULL;
static bool s_is_last_default_esp_netif_overridden = false;
static netif_ext_callback_t netif_callback = { .callback_fn = NULL, .next = NULL };

#if LWIP_IPV4
static void esp_netif_internal_dhcpc_cb(struct netif *netif);
#endif
#if LWIP_IPV6
static void esp_netif_internal_nd6_cb(struct netif *p_netif, uint8_t ip_index);
static void netif_set_mldv6_flag(struct netif *netif);
static void netif_unset_mldv6_flag(struct netif *netif);
#endif /* LWIP_IPV6 */

static esp_err_t esp_netif_destroy_api(esp_netif_api_msg_t *msg);

static void netif_callback_fn(struct netif* netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t* args)
{
#if LWIP_IPV4
    if (reason & DHCP_CB_CHANGE) {
        esp_netif_internal_dhcpc_cb(netif);
    }
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
    if ((reason & LWIP_NSC_IPV6_ADDR_STATE_CHANGED) && (args != NULL)) {
        s8_t addr_idx = args->ipv6_addr_state_changed.addr_index;
        if (netif_ip6_addr_state(netif, addr_idx) & IP6_ADDR_VALID)  {
            /* address is valid -> call the callback function */
            esp_netif_internal_nd6_cb(netif, addr_idx);
        }
    }
#endif /* #if LWIP_IPV6 */
}

#ifdef CONFIG_LWIP_GARP_TMR_INTERVAL

static void netif_send_garp(void *arg)
{
    struct netif *netif = arg;
    if (!ip4_addr_cmp(netif_ip4_addr(netif), IP4_ADDR_ANY4)) {  // Send GARP requests only if we have a valid IP
        etharp_gratuitous(netif);
    }
    sys_timeout(CONFIG_LWIP_GARP_TMR_INTERVAL*1000, netif_send_garp, netif);
}

static void netif_set_garp_flag(struct netif *netif)
{
    sys_timeout(CONFIG_LWIP_GARP_TMR_INTERVAL*1000, netif_send_garp, netif);
}

static void netif_unset_garp_flag(struct netif *netif)
{
    sys_untimeout(netif_send_garp, netif);
}

#endif  // CONFIG_LWIP_GARP_TMR_INTERVAL

#if !LWIP_TCPIP_CORE_LOCKING
static sys_sem_t api_sync_sem = NULL;
static sys_sem_t api_lock_sem = NULL;
#endif

/**
 * @brief Api callback from tcpip thread used to call esp-netif
 * function in lwip task context
 */
static void esp_netif_api_cb(void *api_msg)
{
    esp_netif_api_msg_t *msg = (esp_netif_api_msg_t *)api_msg;

    if (!msg || !msg->api_fn) {
        ESP_LOGD(TAG, "null msg/api_fn");
        return;
    }

    msg->ret = msg->api_fn(msg);
    ESP_LOGD(TAG, "call api in lwip: ret=0x%x, give sem", msg->ret);
#if !LWIP_TCPIP_CORE_LOCKING
    sys_sem_signal(&api_sync_sem);
#endif
}


/**
 * @brief Initiates a tcpip remote call if called from another task
 * or calls the function directly if executed from lwip task
 */
static inline esp_err_t esp_netif_lwip_ipc_call_msg(esp_netif_api_msg_t *msg)
{
    if (!sys_thread_tcpip(LWIP_CORE_LOCK_QUERY_HOLDER)) {
        ESP_LOGD(TAG, "check: remote, if=%p fn=%p", msg->esp_netif, msg->api_fn);
#if LWIP_TCPIP_CORE_LOCKING
        tcpip_send_msg_wait_sem((tcpip_callback_fn)esp_netif_api_cb, msg, NULL);
#else
        sys_arch_sem_wait(&api_lock_sem, 0);
        tcpip_send_msg_wait_sem((tcpip_callback_fn)esp_netif_api_cb, msg, &api_sync_sem);
        sys_sem_signal(&api_lock_sem);
#endif /* LWIP_TCPIP_CORE_LOCKING */
        return msg->ret;
    }
    ESP_LOGD(TAG, "check: local, if=%p fn=%p",  msg->esp_netif, msg->api_fn);
    return msg->api_fn(msg);
}

static inline esp_err_t esp_netif_lwip_ipc_call(esp_netif_api_fn fn, esp_netif_t* netif, void *data)
{
    esp_netif_api_msg_t msg = {
            .esp_netif = netif,
            .data = data,
            .api_fn = fn
    };
    return esp_netif_lwip_ipc_call_msg(&msg);
}

static inline esp_err_t esp_netif_lwip_ipc_call_fn(esp_netif_api_fn fn, esp_netif_callback_fn user_fn, void *ctx)
{
    esp_netif_api_msg_t msg = {
            .user_fn = user_fn,
            .data = ctx,
            .api_fn = fn
    };
    return esp_netif_lwip_ipc_call_msg(&msg);
}

static inline esp_err_t esp_netif_lwip_ipc_call_get_netif(esp_netif_api_fn fn, esp_netif_t **netif, void *ctx)
{
    esp_netif_api_msg_t msg = {
            .p_esp_netif = netif,
            .data = ctx,
            .api_fn = fn
    };
    return esp_netif_lwip_ipc_call_msg(&msg);
}

static inline esp_err_t esp_netif_lwip_ipc_no_args(esp_netif_api_fn fn)
{
    esp_netif_api_msg_t msg = {
            .api_fn = fn
    };
    return esp_netif_lwip_ipc_call_msg(&msg);
}

/**
 * @brief Check if supplied esp_netif handle is active, i.e. available within registered interfaces
 * as it might have already been destroyed. Returns the supplied handle if active, nullptr otherwise
 *
 * @param esp_netif handle to check if available in the list of registered interfaces
 * @return esp_netif handle if available, or NULL if it wasn't found
 */
static esp_netif_t* esp_netif_is_active(esp_netif_t *arg)
{
    // looking for the netif in the list of registered interfaces
    //  as it might have already been destroyed
    if (esp_netif_is_netif_listed(arg)) {
        return arg;
    }
    return NULL;
}

/**
 * @brief This function sets default netif no matter which implementation used
 *
 * @param esp_netif handle to network interface
 *
 * @note: This function must be called from lwip thread
 */
static void esp_netif_set_default_netif_internal(esp_netif_t *esp_netif)
{
    if (ESP_NETIF_IS_POINT2POINT_TYPE(esp_netif, PPP_LWIP_NETIF)) {
#if CONFIG_PPP_SUPPORT
        esp_netif_ppp_set_default_netif(esp_netif->netif_handle);
#endif
    } else {
        netif_set_default(esp_netif->lwip_netif);
    }
}

/**
 * @brief tcpip thread version of esp_netif_update_default_netif
 *
 * @note This function and all functions called from this must be called from lwip task context
 */
static esp_err_t esp_netif_update_default_netif_lwip(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    esp_netif_action_t action = (esp_netif_action_t)msg->data;

    ESP_LOGD(TAG, "%s %p", __func__, esp_netif);

    if (s_is_last_default_esp_netif_overridden && action != ESP_NETIF_SET_DEFAULT) {
        // check if manually configured default interface hasn't been destroyed
        s_last_default_esp_netif = esp_netif_is_active(s_last_default_esp_netif);
        if (s_last_default_esp_netif != NULL) {
            return ESP_OK; // still valid -> don't update default netif
        }
        // invalid -> reset the manual override and perform auto update
        s_is_last_default_esp_netif_overridden = false;
    }
    switch (action) {
        case ESP_NETIF_SET_DEFAULT:
            s_last_default_esp_netif = esp_netif;
            s_is_last_default_esp_netif_overridden = true;
            esp_netif_set_default_netif_internal(s_last_default_esp_netif);
        break;
        case ESP_NETIF_STARTED:
        {
            // check if previously default interface hasn't been destroyed in the meantime
            s_last_default_esp_netif = esp_netif_is_active(s_last_default_esp_netif);
            if (s_last_default_esp_netif && esp_netif_is_netif_up(s_last_default_esp_netif)
                && (s_last_default_esp_netif->route_prio > esp_netif->route_prio)) {
                esp_netif_set_default_netif_internal(s_last_default_esp_netif);
            } else if (esp_netif_is_netif_up(esp_netif)) {
                s_last_default_esp_netif = esp_netif;
                esp_netif_set_default_netif_internal(s_last_default_esp_netif);
            }
        }
        break;

        default:
        case ESP_NETIF_STOPPED:
        {
            s_last_default_esp_netif = NULL;
            esp_netif_t *netif = esp_netif_next_unsafe(NULL);
            while (netif) {
                if (esp_netif_is_netif_up(netif)) {
                    if (s_last_default_esp_netif && esp_netif_is_netif_up(s_last_default_esp_netif)) {
                        if (netif->route_prio > s_last_default_esp_netif->route_prio) {
                            s_last_default_esp_netif = netif;
                        } // else not needed, as the s_last_default_esp_netif is correct
                    } else {
                        // s_last_default is either not set or down, current netif is up
                        s_last_default_esp_netif = netif;
                    }
                }
                netif = esp_netif_next_unsafe(netif);
            }
            if (s_last_default_esp_netif && esp_netif_is_netif_up(s_last_default_esp_netif)) {
                esp_netif_set_default_netif_internal(s_last_default_esp_netif);
            }
        }
        break;
    }
    return ESP_OK;
}

/**
 * @brief This function sets default routing netif based on priorities of all interfaces which are up
 *
 * @param esp_netif current interface which just updated state
 * @param action updating action (on-off)
 */
static esp_err_t esp_netif_update_default_netif(esp_netif_t *esp_netif, esp_netif_action_t action)
{
    return esp_netif_lwip_ipc_call(esp_netif_update_default_netif_lwip, esp_netif, (void*)action);
}

esp_err_t esp_netif_set_default_netif(esp_netif_t *esp_netif)
{
    return esp_netif_update_default_netif(esp_netif, ESP_NETIF_SET_DEFAULT);
}

esp_netif_t *esp_netif_get_default_netif(void)
{
    return s_last_default_esp_netif;
}

static inline esp_netif_t* lwip_get_esp_netif(struct netif *netif)
{
#if LWIP_ESP_NETIF_DATA
    return (esp_netif_t*)netif_get_client_data(netif, lwip_netif_client_id);
#else
    return (esp_netif_t*)netif->state;
#endif
}

static inline void lwip_set_esp_netif(struct netif *netif, esp_netif_t* esp_netif)
{
#if LWIP_ESP_NETIF_DATA
    netif_set_client_data(netif, lwip_netif_client_id, esp_netif);
#else
    netif->state = esp_netif;
#endif
}

#if CONFIG_ESP_NETIF_BRIDGE_EN
esp_err_t esp_netif_bridge_add_port(esp_netif_t *esp_netif_br, esp_netif_t *esp_netif_port)
{
    if (ERR_OK != bridgeif_add_port(esp_netif_br->lwip_netif, esp_netif_port->lwip_netif)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_netif_bridge_fdb_add(esp_netif_t *esp_netif_br, uint8_t *addr, uint64_t ports_mask)
{
    bridgeif_portmask_t ports = (bridgeif_portmask_t)ports_mask;
    if (ports_mask & ESP_NETIF_BR_FDW_CPU) {
        ports |= 1 << BRIDGEIF_MAX_PORTS;
    }

    if (ERR_OK != bridgeif_fdb_add(esp_netif_br->lwip_netif, (const struct eth_addr *)addr, ports)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_netif_bridge_fdb_remove(esp_netif_t *esp_netif_br, uint8_t *addr)
{
    if (ERR_OK != bridgeif_fdb_remove(esp_netif_br->lwip_netif, (const struct eth_addr *)addr)) {
        return ESP_FAIL;
    }
    return ESP_OK;
}
#endif // CONFIG_ESP_NETIF_BRIDGE_EN

#if CONFIG_LWIP_IPV4
void esp_netif_set_ip4_addr(esp_ip4_addr_t *addr, uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    ip4_addr_t *address = (ip4_addr_t*)addr;
    IP4_ADDR(address, a, b, c, d);
}

char * esp_ip4addr_ntoa(const esp_ip4_addr_t *addr, char *buf, int buflen)
{
    return ip4addr_ntoa_r((ip4_addr_t *)addr, buf, buflen);
}

uint32_t esp_ip4addr_aton(const char *addr)
{
    return ipaddr_addr(addr);
}
#endif

esp_err_t esp_netif_str_to_ip4(const char *src, esp_ip4_addr_t *dst)
{
    if (src == NULL || dst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int err = inet_pton(AF_INET, src, dst);
    return err == 1 ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_netif_str_to_ip6(const char *src, esp_ip6_addr_t *dst)
{
    if (src == NULL || dst == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    int err = inet_pton(AF_INET6, src, dst);
    return err == 1 ? ESP_OK : ESP_FAIL;
}

esp_netif_iodriver_handle esp_netif_get_io_driver(esp_netif_t *esp_netif)
{
    return esp_netif->driver_handle;
}

esp_netif_t* esp_netif_get_handle_from_netif_impl(void *dev)
{
    // ppp_pcb ptr would never get to app code, so this function only works with vanilla lwip impl
    struct netif *lwip_netif = dev;
    return lwip_get_esp_netif(lwip_netif);
}

void* esp_netif_get_netif_impl(esp_netif_t *esp_netif)
{
    // get impl ptr only for vanilla lwip impl (ppp_pcb not supported)
    if (esp_netif && !ESP_NETIF_IS_POINT2POINT_TYPE(esp_netif, PPP_LWIP_NETIF)) {
        return esp_netif->lwip_netif;
    }
    return NULL;
}

static void tcpip_init_done(void *arg)
{
    sys_sem_t *init_sem = arg;

#if LWIP_ESP_NETIF_DATA
    if (lwip_netif_client_id == 0xFF) {
        lwip_netif_client_id = netif_alloc_client_data_id();
    }
#endif

    sys_sem_signal(init_sem);
}

esp_err_t esp_netif_init(void)
{
    if (!sys_thread_tcpip(LWIP_CORE_IS_TCPIP_INITIALIZED)) {
#if CONFIG_LWIP_HOOK_TCP_ISN_DEFAULT
        uint8_t rand_buf[16];
        /*
         * This is early startup code where WiFi/BT is yet to be enabled and hence
         * relevant entropy source is not available. However, bootloader enables
         * SAR ADC based entropy source at its initialization, and our requirement
         * of random bytes is pretty small (16), so we can assume that following
         * API will provide sufficiently random data.
         */
        esp_fill_random(rand_buf, sizeof(rand_buf));
        lwip_init_tcp_isn(esp_log_timestamp(), rand_buf);
#endif
        sys_sem_t init_sem;
        if (sys_sem_new(&init_sem, 0) != ERR_OK) {
            ESP_LOGE(TAG, "esp netif cannot create tcpip_init semaphore");
            return ESP_FAIL;
        }
#if LWIP_TCPIP_CORE_LOCKING
        /* TCPIP thread is not initialized yet,
         * pretend that the calling thread is holder
         * to correctly set up the TCPIP task */
        sys_thread_tcpip(LWIP_CORE_LOCK_MARK_HOLDER);
#endif
        tcpip_init(tcpip_init_done, &init_sem);
        sys_sem_wait(&init_sem);
        sys_sem_free(&init_sem);
        ESP_LOGD(TAG, "LwIP stack has been initialized");
    }

#if !LWIP_TCPIP_CORE_LOCKING
    if (!api_sync_sem) {
        if (ERR_OK != sys_sem_new(&api_sync_sem, 0)) {
            ESP_LOGE(TAG, "esp netif api sync sem init fail");
            return ESP_FAIL;
        }
    }

    if (!api_lock_sem) {
        if (ERR_OK != sys_sem_new(&api_lock_sem, 1)) {
            ESP_LOGE(TAG, "esp netif api lock sem init fail");
            return ESP_FAIL;
        }
    }
#endif

    ESP_LOGD(TAG, "esp-netif has been successfully initialized");
    return ESP_OK;
}

esp_err_t esp_netif_deinit(void)
{
    if (sys_thread_tcpip(LWIP_CORE_IS_TCPIP_INITIALIZED)) {
        /* deinit of LwIP not supported:
         * do not deinit semaphores and states,
         * so init could be called multiple times
         *
        sys_sem_free(&api_sync_sem);
        sys_sem_free(&api_lock_sem);
         */
        return ESP_ERR_NOT_SUPPORTED;

    }
    return ESP_ERR_INVALID_STATE;
}

static esp_err_t esp_netif_init_configuration(esp_netif_t *esp_netif, const esp_netif_config_t *cfg)
{
    // Basic esp_netif and lwip is a mandatory configuration and cannot be updated after esp_netif_new()
    if (cfg == NULL || cfg->base == NULL || cfg->stack == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    // Configure general esp-netif properties
    memcpy(esp_netif->mac, cfg->base->mac, NETIF_MAX_HWADDR_LEN);
#if CONFIG_LWIP_IPV4
    if (cfg->base->ip_info == NULL) {
        ip4_addr_set_zero(&esp_netif->ip_info->ip);
        ip4_addr_set_zero(&esp_netif->ip_info->gw);
        ip4_addr_set_zero(&esp_netif->ip_info->netmask);
    } else {
        memcpy(esp_netif->ip_info, cfg->base->ip_info, sizeof(esp_netif_ip_info_t));
    }
    memcpy(esp_netif->ip_info_old, esp_netif->ip_info, sizeof(esp_netif_ip_info_t));
#endif
    // Setup main config parameters
    esp_netif->lost_ip_event = cfg->base->lost_ip_event;
    esp_netif->get_ip_event = cfg->base->get_ip_event;
    esp_netif->flags = cfg->base->flags;

    if (cfg->base->if_key) {
        esp_netif->if_key = strdup(cfg->base->if_key);
    }
    if (cfg->base->if_desc) {
        esp_netif->if_desc = strdup(cfg->base->if_desc);
    }
    if (cfg->base->route_prio) {
        esp_netif->route_prio = cfg->base->route_prio;
    }

#if CONFIG_ESP_NETIF_BRIDGE_EN
    // Setup bridge configuration if the interface is to be bridge
    if (cfg->base->flags & ESP_NETIF_FLAG_IS_BRIDGE) {
        if (cfg->base->bridge_info != NULL) {
            esp_netif->max_fdb_dyn_entries = cfg->base->bridge_info->max_fdb_dyn_entries;
            esp_netif->max_fdb_sta_entries = cfg->base->bridge_info->max_fdb_sta_entries;
            esp_netif->max_ports = cfg->base->bridge_info->max_ports;
        } else {
            return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
        }
    }
#endif // CONFIG_ESP_NETIF_BRIDGE_EN

    // Install network stack functions -- connects netif and L3 stack
    const esp_netif_netstack_config_t *esp_netif_stack_config = cfg->stack;
    if (cfg->base->flags & ESP_NETIF_FLAG_IS_PPP) {
#if CONFIG_PPP_SUPPORT
        esp_netif->related_data = esp_netif_new_ppp(esp_netif, esp_netif_stack_config);
        if (esp_netif->related_data == NULL) {
            return ESP_ERR_ESP_NETIF_INIT_FAILED;
        }
        esp_netif->lwip_input_fn = esp_netif_stack_config->lwip_ppp.input_fn;
        // Make the netif handle (used for tcpip input function) the ppp_netif
        esp_netif->netif_handle = esp_netif->related_data;
#else
        LOG_NETIF_DISABLED_AND_DO("PPP", return ESP_ERR_NOT_SUPPORTED);
#endif
    } else {
        if (esp_netif_stack_config-> lwip.init_fn) {
            esp_netif->lwip_init_fn = esp_netif_stack_config->lwip.init_fn;
        }
        if (esp_netif_stack_config->lwip.input_fn) {
            esp_netif->lwip_input_fn = esp_netif_stack_config->lwip.input_fn;
        }
        // Make the netif handle (used for tcpip input function) the lwip_netif itself
        esp_netif->netif_handle = esp_netif->lwip_netif;

    }

    // Install IO functions only if provided -- connects driver and netif
    // this configuration could be updated after esp_netif_new(), typically in post_attach callback
    if (cfg->driver) {
        const esp_netif_driver_ifconfig_t *esp_netif_driver_config = cfg->driver;
        if (esp_netif_driver_config->handle) {
            esp_netif->driver_handle = esp_netif_driver_config->handle;
        }
        if (esp_netif_driver_config->transmit) {
            esp_netif->driver_transmit = esp_netif_driver_config->transmit;
        }
        if (esp_netif_driver_config->transmit_wrap) {
            esp_netif->driver_transmit_wrap = esp_netif_driver_config->transmit_wrap;
        }
        if (esp_netif_driver_config->driver_free_rx_buffer) {
            esp_netif->driver_free_rx_buffer = esp_netif_driver_config->driver_free_rx_buffer;
        }
    }
    return ESP_OK;
}

static esp_err_t tcpip_exec_api(esp_netif_api_msg_t *msg)
{
    return msg->user_fn(msg->data);
}

esp_err_t esp_netif_tcpip_exec(esp_netif_callback_fn fn, void*ctx)
{
    return esp_netif_lwip_ipc_call_fn(tcpip_exec_api, fn, ctx);
}

static esp_err_t esp_netif_new_api(esp_netif_api_msg_t *msg)
{
    const esp_netif_config_t *esp_netif_config = msg->data;
    // mandatory configuration must be provided when creating esp_netif object
    if (esp_netif_config == NULL ||
        esp_netif_config->base->if_key == NULL ||
        NULL != esp_netif_get_handle_from_ifkey_unsafe(esp_netif_config->base->if_key)) {
        ESP_LOGE(TAG, "%s: Failed to configure netif with config=%p (config or if_key is NULL or duplicate key)",
        __func__,  esp_netif_config);
        return ESP_FAIL;
    }

#if ESP_DHCPS
    // DHCP server and client cannot be configured together
    if((esp_netif_config->base->flags & ESP_NETIF_DHCP_SERVER) &&
       (esp_netif_config->base->flags & ESP_NETIF_DHCP_CLIENT)) {
        ESP_LOGE(TAG, "%s: Failed to configure netif with config=%p (DHCP server and client cannot be configured together)",
        __func__,  esp_netif_config);
        return ESP_FAIL;
    }
#endif

    // Create parent esp-netif object
    esp_netif_t *esp_netif = calloc(1, sizeof(struct esp_netif_obj));
    if (!esp_netif) {
        ESP_LOGE(TAG, "Failed to allocate %" PRIu32 " bytes (free heap size %" PRIu32 ")", (uint32_t)sizeof(struct esp_netif_obj),
                 esp_get_free_heap_size());
        return ESP_FAIL;
    }

    // Create ip info
    esp_netif_ip_info_t *ip_info = calloc(1, sizeof(esp_netif_ip_info_t));
    if (!ip_info) {
        ESP_LOGE(TAG, "Failed to allocate %" PRIu32 " bytes (free heap size %" PRIu32 ")", (uint32_t)sizeof(esp_netif_ip_info_t),
                 esp_get_free_heap_size());
        free(esp_netif);
        return ESP_FAIL;
    }
    esp_netif->ip_info = ip_info;

    // creating another ip info (to store old ip)
    ip_info = calloc(1, sizeof(esp_netif_ip_info_t));
    if (!ip_info) {
        ESP_LOGE(TAG, "Failed to allocate %" PRIu32 " bytes (free heap size %" PRIu32 ")", (uint32_t)sizeof(esp_netif_ip_info_t),
                 esp_get_free_heap_size());
        free(esp_netif->ip_info);
        free(esp_netif);
        return ESP_FAIL;
    }
    esp_netif->ip_info_old = ip_info;

    // Create underlying lwip netif
    struct netif * lwip_netif = calloc(1, sizeof(struct netif));
    if (!lwip_netif) {
        ESP_LOGE(TAG, "Failed to allocate %" PRIu32 " bytes (free heap size %" PRIu32 ")", (uint32_t)sizeof(struct netif),
                 esp_get_free_heap_size());
        free(esp_netif->ip_info_old);
        free(esp_netif->ip_info);
        free(esp_netif);
        return ESP_FAIL;
    }

    esp_netif->lwip_netif = lwip_netif;

    esp_netif_add_to_list_unsafe(esp_netif);

#if ESP_DHCPS
    // Create DHCP server structure
    if (esp_netif_config->base->flags & ESP_NETIF_DHCP_SERVER) {
        esp_netif->dhcps = dhcps_new();
        if (esp_netif->dhcps == NULL) {
            ESP_LOGE(TAG, "Failed to create dhcp server handle");
            esp_netif_api_msg_t to_destroy = { .esp_netif = esp_netif};
            esp_netif_destroy_api(&to_destroy);
            return ESP_FAIL;
        }
    }
#endif

    // Configure the created object with provided configuration
    esp_err_t ret =  esp_netif_init_configuration(esp_netif, esp_netif_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Initial configuration of esp_netif failed with %d", ret);
        esp_netif_api_msg_t to_destroy = { .esp_netif = esp_netif};
        esp_netif_destroy_api(&to_destroy);
        return ESP_FAIL;
    }
    lwip_set_esp_netif(lwip_netif, esp_netif);

    if (netif_callback.callback_fn == NULL ) {
        netif_add_ext_callback(&netif_callback, netif_callback_fn);
    }

    *msg->p_esp_netif = esp_netif;
    return ESP_OK;
}

esp_netif_t *esp_netif_new(const esp_netif_config_t *config)
{
    esp_netif_t *netif = NULL;
    esp_netif_lwip_ipc_call_get_netif(esp_netif_new_api, &netif, (void *)config);
    return netif;
}

static esp_err_t get_handle_from_ifkey_api(esp_netif_api_msg_t *msg)
{
    *msg->p_esp_netif = esp_netif_get_handle_from_ifkey_unsafe(msg->data);
    return ESP_OK;
}

esp_netif_t *esp_netif_get_handle_from_ifkey(const char *if_key)
{
    esp_netif_t *netif = NULL;
    esp_netif_lwip_ipc_call_get_netif(get_handle_from_ifkey_api, &netif, (void*)if_key);
    return netif;
}

typedef struct find_if_api {
    esp_netif_find_predicate_t fn;
    void *ctx;
} find_if_api_t;

static esp_err_t esp_netif_find_if_api(esp_netif_api_msg_t *msg)
{
    find_if_api_t *find_if_api = msg->data;
    esp_netif_t *esp_netif = NULL;
    while ((esp_netif = esp_netif_next_unsafe(esp_netif)) != NULL) {
        if (find_if_api->fn(esp_netif, find_if_api->ctx)) {
            *msg->p_esp_netif = esp_netif;
            return ESP_OK;
        }
    }
    return ESP_FAIL;
}

esp_netif_t *esp_netif_find_if(esp_netif_find_predicate_t fn, void *ctx)
{
    esp_netif_t *netif = NULL;
    find_if_api_t find_if_api = { .fn = fn, .ctx = ctx };
    if (esp_netif_lwip_ipc_call_get_netif(esp_netif_find_if_api, &netif, &find_if_api) == ESP_OK) {
        return netif;
    }
    return NULL;
}

static void esp_netif_lwip_remove(esp_netif_t *esp_netif)
{
    if (esp_netif->lwip_netif) {
        if (netif_is_up(esp_netif->lwip_netif)) {
            netif_set_down(esp_netif->lwip_netif);
        }
        netif_remove(esp_netif->lwip_netif);
#if ESP_GRATUITOUS_ARP
        if (esp_netif->flags & ESP_NETIF_FLAG_GARP) {
            netif_unset_garp_flag(esp_netif->lwip_netif);
        }
#endif
#if ESP_MLDV6_REPORT && LWIP_IPV6
        if (esp_netif->flags & ESP_NETIF_FLAG_MLDV6_REPORT) {
            netif_unset_mldv6_flag(esp_netif->lwip_netif);
        }
#endif
        if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT) {
#if CONFIG_LWIP_IPV4
            dhcp_cleanup(esp_netif->lwip_netif);
#endif
        }

    }
}

static esp_err_t esp_netif_lwip_add(esp_netif_t *esp_netif)
{
    if (esp_netif->lwip_netif == NULL) {
        esp_netif->lwip_netif = calloc(1, sizeof(struct netif));
        if (esp_netif->lwip_netif == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }
    if (esp_netif->flags & ESP_NETIF_FLAG_IS_PPP) {
#if CONFIG_PPP_SUPPORT
        err_t err = esp_netif->lwip_init_fn(NULL);
        if (err != ERR_OK) {
            ESP_LOGE(TAG, "Init netif failed with  %" PRId8 "", err);
            return ESP_ERR_ESP_NETIF_INIT_FAILED;
        }
#else
        LOG_NETIF_DISABLED_AND_DO("PPP", return ESP_ERR_NOT_SUPPORTED);
#endif
    }

#if CONFIG_ESP_NETIF_BRIDGE_EN
    if (esp_netif->flags & ESP_NETIF_FLAG_IS_BRIDGE) {
        bridgeif_initdata_t bridge_initdata = {
            .max_fdb_dynamic_entries = esp_netif->max_fdb_dyn_entries,
            .max_fdb_static_entries = esp_netif->max_fdb_sta_entries,
            .max_ports = esp_netif->max_ports
        };
        memcpy(&bridge_initdata.ethaddr, esp_netif->mac, ETH_HWADDR_LEN);
        if (NULL == netif_add(esp_netif->lwip_netif, (struct ip4_addr*)&esp_netif->ip_info->ip,
                        (struct ip4_addr*)&esp_netif->ip_info->netmask, (struct ip4_addr*)&esp_netif->ip_info->gw,
                        &bridge_initdata, esp_netif->lwip_init_fn, tcpip_input)) {
            esp_netif_lwip_remove(esp_netif);
            return ESP_ERR_ESP_NETIF_IF_NOT_READY;
        }
    } else {
#endif // CONFIG_ESP_NETIF_BRIDGE_EN
        if (NULL == netif_add(esp_netif->lwip_netif,
#if CONFIG_LWIP_IPV4
                            (struct ip4_addr*)&esp_netif->ip_info->ip,
                            (struct ip4_addr*)&esp_netif->ip_info->netmask,
                            (struct ip4_addr*)&esp_netif->ip_info->gw,
#endif
                            esp_netif, esp_netif->lwip_init_fn, tcpip_input)) {
            esp_netif_lwip_remove(esp_netif);
            return ESP_ERR_ESP_NETIF_IF_NOT_READY;
        }
#if CONFIG_ESP_NETIF_BRIDGE_EN
    }
#endif // CONFIG_ESP_NETIF_BRIDGE_EN
    lwip_set_esp_netif(esp_netif->lwip_netif, esp_netif);
    return ESP_OK;
}

static void esp_netif_destroy_related(esp_netif_t *esp_netif)
{
    if (ESP_NETIF_IS_POINT2POINT_TYPE(esp_netif, PPP_LWIP_NETIF)) {
#if CONFIG_PPP_SUPPORT
        esp_netif_destroy_ppp(esp_netif->related_data);
#endif
    }
}

static esp_err_t esp_netif_destroy_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    esp_netif_remove_from_list_unsafe(esp_netif);
    if (esp_netif_get_nr_of_ifs() == 0) {
        netif_remove_ext_callback(&netif_callback);
        netif_callback.callback_fn = NULL;
    }
    free(esp_netif->ip_info);
    free(esp_netif->ip_info_old);
    free(esp_netif->if_key);
    free(esp_netif->if_desc);
    esp_netif_lwip_remove(esp_netif);
    esp_netif_destroy_related(esp_netif);
    free(esp_netif->lwip_netif);
    free(esp_netif->hostname);
    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STOPPED);
#if ESP_DHCPS
    dhcps_delete(esp_netif->dhcps);
#endif
    free(esp_netif);
    return ESP_OK;
}

void esp_netif_destroy(esp_netif_t *esp_netif)
{
    if (esp_netif == NULL) {
        return;
    }
    esp_netif_lwip_ipc_call(esp_netif_destroy_api, esp_netif, NULL);
}

esp_err_t esp_netif_attach(esp_netif_t *esp_netif, esp_netif_iodriver_handle driver_handle)
{
    esp_netif_driver_base_t *base_driver = driver_handle;

    esp_netif->driver_handle = driver_handle;
    if (base_driver->post_attach) {
        esp_err_t ret = base_driver->post_attach(esp_netif, driver_handle);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Post-attach callback of driver(%p) failed with %d", driver_handle, ret);
            return ESP_ERR_ESP_NETIF_DRIVER_ATTACH_FAILED;
        }
    }
    return ESP_OK;
}

esp_err_t esp_netif_set_driver_config(esp_netif_t *esp_netif,
                                      const esp_netif_driver_ifconfig_t *driver_config)
{
    if (esp_netif == NULL || driver_config == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
    esp_netif->driver_handle = driver_config->handle;
    esp_netif->driver_transmit = driver_config->transmit;
    esp_netif->driver_transmit_wrap = driver_config->transmit_wrap;
    esp_netif->driver_free_rx_buffer = driver_config->driver_free_rx_buffer;
    return ESP_OK;
}

#if CONFIG_LWIP_IPV4
static esp_err_t esp_netif_reset_ip_info(esp_netif_t *esp_netif)
{
    ip4_addr_set_zero(&(esp_netif->ip_info->ip));
    ip4_addr_set_zero(&(esp_netif->ip_info->gw));
    ip4_addr_set_zero(&(esp_netif->ip_info->netmask));
    return ESP_OK;
}
#endif

esp_err_t esp_netif_set_mac_api(esp_netif_api_msg_t *msg)
{
    uint8_t *mac = msg->data;
    esp_netif_t* esp_netif = msg->esp_netif;
    memcpy(esp_netif->mac, mac, NETIF_MAX_HWADDR_LEN);
    memcpy(esp_netif->lwip_netif->hwaddr, mac, NETIF_MAX_HWADDR_LEN);
    return ESP_OK;
}

esp_err_t esp_netif_set_mac(esp_netif_t *esp_netif, uint8_t mac[])
{
    if (esp_netif == NULL || esp_netif->lwip_netif == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
    if (_IS_NETIF_ANY_POINT2POINT_TYPE(esp_netif)) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return esp_netif_lwip_ipc_call(esp_netif_set_mac_api, esp_netif, mac);
}

esp_err_t esp_netif_get_mac(esp_netif_t *esp_netif, uint8_t mac[])
{
    if (esp_netif == NULL || esp_netif->lwip_netif == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
    if (_IS_NETIF_ANY_POINT2POINT_TYPE(esp_netif)) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (esp_netif_is_netif_up(esp_netif)) {
        memcpy(mac, esp_netif->lwip_netif->hwaddr, NETIF_MAX_HWADDR_LEN);
        return ESP_OK;
    }
    memcpy(mac, esp_netif->mac, NETIF_MAX_HWADDR_LEN);
    return ESP_OK;
}

#if ESP_DHCPS
static void esp_netif_dhcps_cb(void* arg, uint8_t ip[4], uint8_t mac[6])
{
    esp_netif_t *esp_netif = arg;
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);
    ip_event_ap_staipassigned_t evt = { .esp_netif = esp_netif };
    memcpy((char *)&evt.ip.addr, (char *)ip, sizeof(evt.ip.addr));
    memcpy((char *)&evt.mac, mac, sizeof(evt.mac));
    ESP_LOGI(TAG, "DHCP server assigned IP to a client, IP is: " IPSTR, IP2STR(&evt.ip));
    ESP_LOGD(TAG, "Client's MAC: %x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    int ret = esp_event_post(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &evt, sizeof(evt), 0);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "dhcps cb: failed to post IP_EVENT_AP_STAIPASSIGNED (%x)", ret);
    }
}
#endif

static esp_err_t esp_netif_config_sanity_check(const esp_netif_t * esp_netif)
{
    if (esp_netif == NULL) {
        ESP_LOGE(TAG,  "Cannot start esp_netif: esp_netif must not be null");
        return ESP_ERR_INVALID_STATE;
    }

    if ((!(esp_netif->flags & ESP_NETIF_FLAG_IS_BRIDGE) && (esp_netif->driver_transmit == NULL ||
        esp_netif->driver_handle == NULL || esp_netif->lwip_input_fn == NULL)) ||
        esp_netif->lwip_init_fn == NULL) {
        ESP_LOGE(TAG,  "Cannot start esp_netif: Missing mandatory configuration:\n"
                       "esp_netif->driver_transmit: %p, esp_netif->driver_handle:%p, "
                       "esp_netif->lwip_input_fn: %p, esp_netif->lwip_init_fn:%p",
                       esp_netif->driver_transmit,  esp_netif->driver_handle,
                       esp_netif->lwip_input_fn,  esp_netif->lwip_init_fn);

         return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

static esp_err_t esp_netif_start_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t * esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s %p", __func__, esp_netif);
    if (ESP_NETIF_IS_POINT2POINT_TYPE(esp_netif, PPP_LWIP_NETIF)) {
#if CONFIG_PPP_SUPPORT
        esp_err_t ret = esp_netif_start_ppp(esp_netif);
        if (ret == ESP_OK) {
            esp_netif_update_default_netif(esp_netif, ESP_NETIF_STARTED);
        }
        return ret;
#endif
    }

    ESP_ERROR_CHECK(esp_netif_config_sanity_check(esp_netif));

    ESP_ERROR_CHECK(esp_netif_lwip_add(esp_netif));

#if ESP_IPV6_AUTOCONFIG
    esp_netif->lwip_netif->ip6_autoconfig_enabled = 1;
#endif
    if (esp_netif->flags&ESP_NETIF_FLAG_GARP) {
#if ESP_GRATUITOUS_ARP
        netif_set_garp_flag(esp_netif->lwip_netif);
#else
        ESP_LOGW(TAG,"CONFIG_LWIP_ESP_GRATUITOUS_ARP not enabled, but esp-netif configured with ESP_NETIF_FLAG_GARP");
#endif
    }
    struct netif *p_netif = esp_netif->lwip_netif;

    if (esp_netif->flags&ESP_NETIF_FLAG_AUTOUP) {
        ESP_LOGD(TAG, "%s Setting the lwip netif%p UP", __func__, p_netif);
        netif_set_up(p_netif);
        netif_set_link_up(p_netif);
    }
    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
#if ESP_DHCPS
        if (esp_netif->dhcps_status == ESP_NETIF_DHCP_INIT) {
            if (p_netif != NULL && netif_is_up(p_netif)) {
                esp_netif_ip_info_t *default_ip = esp_netif->ip_info;
                ip4_addr_t lwip_ip;
                ip4_addr_t lwip_netmask;
                memcpy(&lwip_ip, &default_ip->ip, sizeof(struct ip4_addr));
                memcpy(&lwip_netmask, &default_ip->netmask, sizeof(struct ip4_addr));
                dhcps_set_new_lease_cb(esp_netif->dhcps, esp_netif_dhcps_cb, esp_netif);
                dhcps_set_option_info(esp_netif->dhcps, SUBNET_MASK, (void*)&lwip_netmask, sizeof(lwip_netmask));
                if (dhcps_start(esp_netif->dhcps, p_netif, lwip_ip) != ERR_OK) {
                    ESP_LOGE(TAG, "DHCP server cannot be started");
                    esp_netif->dhcps_status = ESP_NETIF_DHCP_INIT;
                    return ESP_ERR_ESP_NETIF_DHCPS_START_FAILED;
                }
                esp_netif->dhcps_status = ESP_NETIF_DHCP_STARTED;
                ESP_LOGI(TAG, "DHCP server started on interface %s with IP: " IPSTR, esp_netif->if_key, IP2STR(&lwip_ip));
                esp_netif_update_default_netif(esp_netif, ESP_NETIF_STARTED);
                return ESP_OK;
            } else {
                ESP_LOGD(TAG, "DHCP server re init");
                esp_netif->dhcps_status = ESP_NETIF_DHCP_INIT;
                return ESP_OK;
            }
        } else if (esp_netif->dhcps_status == ESP_NETIF_DHCP_STARTED) {
            ESP_LOGD(TAG, "DHCP server already started");
            return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED;
        }
        return ESP_OK;
#else
        LOG_NETIF_DISABLED_AND_DO("DHCP Server", return ESP_ERR_NOT_SUPPORTED);
#endif
    } else if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT) {
#if CONFIG_LWIP_IPV4
        if (esp_netif->dhcpc_status != ESP_NETIF_DHCP_STARTED) {
            if (p_netif != NULL) {
                struct dhcp *dhcp_data = NULL;
                dhcp_data = netif_dhcp_data(p_netif);
                if (dhcp_data == NULL) {
                    dhcp_data = (struct dhcp *)malloc(sizeof(struct dhcp));
                    if (dhcp_data == NULL) {
                        return ESP_ERR_NO_MEM;
                    }
                    dhcp_set_struct(p_netif, dhcp_data);
                }
            }
        }
#else
        LOG_NETIF_DISABLED_AND_DO("IPv4's DHCP Client", return ESP_ERR_NOT_SUPPORTED);
#endif
    }

    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STARTED);

    return ESP_OK;
}

esp_err_t esp_netif_start(esp_netif_t *esp_netif)
{
    return esp_netif_lwip_ipc_call(esp_netif_start_api, esp_netif, NULL);
}

static esp_err_t esp_netif_stop_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    if (ESP_NETIF_IS_POINT2POINT_TYPE(esp_netif, PPP_LWIP_NETIF)) {
#if CONFIG_PPP_SUPPORT
        esp_err_t ret = esp_netif_stop_ppp(esp_netif->related_data);
        if (ret == ESP_OK) {
            esp_netif_update_default_netif(esp_netif, ESP_NETIF_STOPPED);
        }
        return ret;
#endif
    }

    struct netif *lwip_netif = esp_netif->lwip_netif;
    if (lwip_netif == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }

    if (!netif_is_up(lwip_netif)) {
        esp_netif_lwip_remove(esp_netif);
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }

    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
#if ESP_DHCPS
        if (dhcps_stop(esp_netif->dhcps, lwip_netif) != ERR_OK ||
            esp_netif->dhcps_status != ESP_NETIF_DHCP_STOPPED) {
            esp_netif->dhcps_status = ESP_NETIF_DHCP_INIT;
        }
#else
        LOG_NETIF_DISABLED_AND_DO("DHCP Server", return ESP_ERR_NOT_SUPPORTED);
#endif
    } else if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT) {
#if CONFIG_LWIP_IPV4
        dhcp_release(lwip_netif);
        dhcp_stop(lwip_netif);
        dhcp_cleanup(lwip_netif);

        esp_netif->dhcpc_status = ESP_NETIF_DHCP_INIT;

        esp_netif_reset_ip_info(esp_netif);
#else
        LOG_NETIF_DISABLED_AND_DO("IPv4's DHCP Client", return ESP_ERR_NOT_SUPPORTED);
#endif
    }

    netif_set_down(lwip_netif);
    esp_netif_lwip_remove(esp_netif);
    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STOPPED);

    return ESP_OK;
}

esp_err_t esp_netif_stop(esp_netif_t *esp_netif)
{
    return esp_netif_lwip_ipc_call(esp_netif_stop_api, esp_netif, NULL);
}

void esp_netif_netstack_buf_ref(void *pbuf)
{
    pbuf_ref(pbuf);
}

void esp_netif_netstack_buf_free(void *pbuf)
{
    pbuf_free(pbuf);
}

//
// IO translate functions
//
void esp_netif_free_rx_buffer(void *h, void* buffer)
{
    esp_netif_t *esp_netif = h;
    esp_netif->driver_free_rx_buffer(esp_netif->driver_handle, buffer);
}

esp_err_t esp_netif_transmit(esp_netif_t *esp_netif, void* data, size_t len)
{
    return (esp_netif->driver_transmit)(esp_netif->driver_handle, data, len);
}

esp_err_t esp_netif_transmit_wrap(esp_netif_t *esp_netif, void *data, size_t len, void *pbuf)
{
    return (esp_netif->driver_transmit_wrap)(esp_netif->driver_handle, data, len, pbuf);
}

esp_err_t esp_netif_receive(esp_netif_t *esp_netif, void *buffer, size_t len, void *eb)
{
#ifdef CONFIG_ESP_NETIF_RECEIVE_REPORT_ERRORS
    return esp_netif->lwip_input_fn(esp_netif->netif_handle, buffer, len, eb);
#else
    esp_netif->lwip_input_fn(esp_netif->netif_handle, buffer, len, eb);
    return ESP_OK;
#endif
}

#if CONFIG_LWIP_IPV4
static esp_err_t esp_netif_start_ip_lost_timer(esp_netif_t *esp_netif);

//
// DHCP:
//
static void esp_netif_internal_dhcpc_cb(struct netif *netif)
{
    esp_netif_t *esp_netif;
    ESP_LOGD(TAG, "%s lwip-netif:%p", __func__, netif);
    if (netif == NULL || (esp_netif = lwip_get_esp_netif(netif)) == NULL) {
        // internal pointer hasn't been configured yet (probably in the interface init_fn())
        return;
    }

    esp_netif_ip_info_t *ip_info = esp_netif->ip_info;
    esp_netif_ip_info_t *ip_info_old = esp_netif->ip_info_old;

    if ( !ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), IP4_ADDR_ANY4) ) {

        //check whether IP is changed (or if we're an PPP interface)
        if ( (!ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), (&ip_info->ip)) ||
              !ip4_addr_cmp(ip_2_ip4(&netif->netmask), (&ip_info->netmask)) ||
              !ip4_addr_cmp(ip_2_ip4(&netif->gw), (&ip_info->gw)))
          // post IP event for PPP interfaces even if IP hasn't changed
             || (_IS_NETIF_ANY_POINT2POINT_TYPE(esp_netif))) {
            ip_event_got_ip_t evt = {
                    .esp_netif = esp_netif,
                    .ip_changed = false,
            };
            ip_event_t evt_id = esp_netif_get_event_id(esp_netif, ESP_NETIF_IP_EVENT_GOT_IP);
            int ret;

            ip4_addr_set(&ip_info->ip, ip_2_ip4(&netif->ip_addr));
            ip4_addr_set(&ip_info->netmask, ip_2_ip4(&netif->netmask));
            ip4_addr_set(&ip_info->gw, ip_2_ip4(&netif->gw));

            //notify event
            if (memcmp(ip_info, ip_info_old, sizeof(esp_netif_ip_info_t))) {
                evt.ip_changed = true;
            }

            memcpy(&evt.ip_info, ip_info, sizeof(esp_netif_ip_info_t));
            memcpy(ip_info_old, ip_info, sizeof(esp_netif_ip_info_t));
            ESP_LOGD(TAG, "if%p ip changed=%d", esp_netif, evt.ip_changed);
            ret = esp_event_post(IP_EVENT, evt_id, &evt, sizeof(evt), 0);
            if (ESP_OK != ret) {
                ESP_LOGE(TAG, "dhcpc cb: failed to post got ip event (%x)", ret);
            }
#ifdef CONFIG_LWIP_DHCP_RESTORE_LAST_IP
            dhcp_ip_addr_store(netif);
#endif /* CONFIG_LWIP_DHCP_RESTORE_LAST_IP */
        } else {
            ESP_LOGD(TAG, "if%p ip unchanged", esp_netif);
        }
    } else {
        if (!ip4_addr_cmp(&ip_info->ip, IP4_ADDR_ANY4)) {
            esp_netif_start_ip_lost_timer(esp_netif);
        }
    }
}

static void esp_netif_ip_lost_timer(void *arg)
{
    esp_netif_t *esp_netif = esp_netif_is_active(arg);

    if (esp_netif == NULL) {
        ESP_LOGD(TAG, "%s esp_netif=%p not active any more", __func__, arg);
        return;
    }

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    esp_netif->timer_running = false;

    struct netif *netif = esp_netif->lwip_netif;

    if ( (!netif) || (netif && ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), IP4_ADDR_ANY4))) {
        ip_event_got_ip_t evt = {
                .esp_netif = esp_netif,
        };
        int ret;

        ESP_LOGD(TAG, "if%p ip lost tmr: raise ip lost event", esp_netif);
        memset(esp_netif->ip_info_old, 0, sizeof(esp_netif_ip_info_t));
        if (esp_netif->lost_ip_event) {
            ret = esp_event_post(IP_EVENT, esp_netif->lost_ip_event,
                                          &evt, sizeof(evt), 0);
            if (ESP_OK != ret) {
                ESP_LOGE(TAG, "ip lost timer: failed to post lost ip event (%x)", ret);
            }
        }
    } else {
        ESP_LOGD(TAG, "if%p ip lost tmr: no need raise ip lost event", esp_netif);
    }
}

static esp_err_t esp_netif_start_ip_lost_timer(esp_netif_t *esp_netif)
{
    esp_netif_ip_info_t *ip_info_old = esp_netif->ip_info;
    struct netif *netif = esp_netif->lwip_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif->timer_running) {
        ESP_LOGD(TAG, "if%p start ip lost tmr: already started", esp_netif);
        return ESP_OK;
    }

    if ( netif && (CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL > 0)) {
        esp_netif->timer_running = true;
        sys_timeout(CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL * 1000, esp_netif_ip_lost_timer, (void *)esp_netif);
        ESP_LOGD(TAG, "if%p start ip lost tmr: interval=%d", esp_netif, CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL);
        return ESP_OK;
    }

    ESP_LOGD(TAG, "if%p start ip lost tmr: no need start because netif=%p interval=%d ip=%" PRIx32,
             esp_netif, netif, (CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL), ip_info_old->ip.addr);

    return ESP_OK;
}

static esp_err_t esp_netif_dhcpc_stop_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif == NULL) {
        ESP_LOGE(TAG, "dhcp client stop called with NULL api");
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (esp_netif->dhcpc_status == ESP_NETIF_DHCP_STARTED) {
        struct netif *p_netif = esp_netif->lwip_netif;

        if (p_netif != NULL) {
            dhcp_stop(p_netif);
            esp_netif_reset_ip_info(esp_netif);
            esp_netif_start_ip_lost_timer(esp_netif);
        } else {
            ESP_LOGD(TAG, "dhcp client if not ready");
            return ESP_ERR_ESP_NETIF_IF_NOT_READY;
        }
    } else if (esp_netif->dhcpc_status == ESP_NETIF_DHCP_STOPPED) {
        ESP_LOGD(TAG, "dhcp client already stoped");
        return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED;
    }

    ESP_LOGD(TAG, "dhcp client stop successfully");
    esp_netif->dhcpc_status = ESP_NETIF_DHCP_STOPPED;

#ifdef CONFIG_LWIP_DHCP_RESTORE_LAST_IP
    dhcp_ip_addr_erase(esp_netif->lwip_netif);
#endif /* CONFIG_LWIP_DHCP_RESTORE_LAST_IP */

    return ESP_OK;
}

esp_err_t esp_netif_dhcpc_stop(esp_netif_t *esp_netif) _RUN_IN_LWIP_TASK_IF_SUPPORTED(esp_netif_dhcpc_stop_api, esp_netif, NULL)

static void dns_clear_servers(bool keep_fallback)
{
    u8_t numdns = 0;

    for (numdns = 0; numdns < DNS_MAX_SERVERS; numdns ++) {
        if (keep_fallback && numdns == DNS_FALLBACK_SERVER_INDEX) {
            continue;
        }

        dns_setserver(numdns, NULL);
    }
}

static esp_err_t esp_netif_dhcpc_start_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_netif->dhcpc_status == ESP_NETIF_DHCP_STARTED) {
        ESP_LOGD(TAG, "dhcp client already started");
        return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED;
    }

    struct netif *p_netif = esp_netif->lwip_netif;

    esp_netif_reset_ip_info(esp_netif);

#if LWIP_DNS
    dns_clear_servers(true);
#endif

    if (p_netif != NULL) {
        if (netif_is_up(p_netif)) {
            ip_addr_set_zero(&p_netif->ip_addr);
            ip_addr_set_zero(&p_netif->netmask);
            ip_addr_set_zero(&p_netif->gw);
            esp_netif_start_ip_lost_timer(esp_netif);
        } else {
            ESP_LOGD(TAG, "dhcp client re init");
            esp_netif->dhcpc_status = ESP_NETIF_DHCP_INIT;
            return ESP_OK;
        }
        ESP_LOGD(TAG, "starting dhcp client");

        if (dhcp_start(p_netif) != ERR_OK) {
            ESP_LOGE(TAG, "dhcp client start failed");
            return ESP_ERR_ESP_NETIF_DHCPC_START_FAILED;
        }

        esp_netif->dhcpc_status = ESP_NETIF_DHCP_STARTED;
        return ESP_OK;
    } else {
        ESP_LOGD(TAG, "dhcp client re init");
        esp_netif->dhcpc_status = ESP_NETIF_DHCP_INIT;
        return ESP_OK;
    }
}

esp_err_t esp_netif_dhcpc_start(esp_netif_t *esp_netif) _RUN_IN_LWIP_TASK_IF_SUPPORTED(esp_netif_dhcpc_start_api, esp_netif, NULL)
#endif /* CONFIG_LWIP_IPV4 */

#if ESP_DHCPS
esp_err_t esp_netif_dhcps_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status)
{
    if (!esp_netif || (esp_netif->flags & ESP_NETIF_DHCP_CLIENT) || _IS_NETIF_ANY_POINT2POINT_TYPE(esp_netif)) {
        return ESP_ERR_INVALID_ARG;
    }

    *status = esp_netif->dhcps_status;
    return ESP_OK;
}
#endif

esp_err_t esp_netif_dhcpc_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status)
{
    if (!esp_netif || (esp_netif->flags & ESP_NETIF_DHCP_SERVER) || _IS_NETIF_ANY_POINT2POINT_TYPE(esp_netif)) {
        return ESP_ERR_INVALID_ARG;
    }

    *status = esp_netif->dhcpc_status;
    return ESP_OK;
}

#if ESP_DHCPS
static esp_err_t esp_netif_dhcps_start_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_ARG;
    }

    if (esp_netif->dhcps_status == ESP_NETIF_DHCP_STARTED) {
        ESP_LOGD(TAG, "dhcp server already started");
        return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED;
    }

    struct netif *p_netif = esp_netif->lwip_netif;
    if (p_netif != NULL && netif_is_up(p_netif)) {
        esp_netif_ip_info_t *default_ip = esp_netif->ip_info;
        ip4_addr_t lwip_ip;
        ip4_addr_t lwip_netmask;
        memcpy(&lwip_ip, &default_ip->ip, sizeof(struct ip4_addr));
        memcpy(&lwip_netmask, &default_ip->netmask, sizeof(struct ip4_addr));
        dhcps_set_new_lease_cb(esp_netif->dhcps, esp_netif_dhcps_cb, esp_netif);
        dhcps_set_option_info(esp_netif->dhcps, SUBNET_MASK, (void*)&lwip_netmask, sizeof(lwip_netmask));
        if (dhcps_start(esp_netif->dhcps, p_netif, lwip_ip) != ERR_OK) {
            ESP_LOGE(TAG, "DHCP server cannot be started");
            esp_netif->dhcps_status = ESP_NETIF_DHCP_INIT;
            return ESP_ERR_ESP_NETIF_DHCPS_START_FAILED;
        }
        esp_netif->dhcps_status = ESP_NETIF_DHCP_STARTED;
        ESP_LOGD(TAG, "DHCP server started successfully");
        return ESP_OK;
    } else {
        ESP_LOGD(TAG, "dhcp server re init");
        esp_netif->dhcps_status = ESP_NETIF_DHCP_INIT;
        return ESP_OK;
    }
}

esp_err_t esp_netif_dhcps_start(esp_netif_t *esp_netif) _RUN_IN_LWIP_TASK_IF_SUPPORTED(esp_netif_dhcps_start_api, esp_netif, NULL)

static esp_err_t esp_netif_dhcps_stop_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_ARG;
    }

    struct netif *p_netif = esp_netif->lwip_netif;
    if (esp_netif->dhcps_status == ESP_NETIF_DHCP_STARTED) {
        if (dhcps_stop(esp_netif->dhcps, p_netif) != ERR_OK) {
            ESP_LOGD(TAG, "dhcp server if not ready");
            return ESP_ERR_ESP_NETIF_IF_NOT_READY;
        }
    } else if (esp_netif->dhcps_status == ESP_NETIF_DHCP_STOPPED) {
        ESP_LOGD(TAG, "dhcp server already stoped");
        return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED;
    }

    ESP_LOGD(TAG, "dhcp server stop successfully");
    esp_netif->dhcps_status = ESP_NETIF_DHCP_STOPPED;
    return ESP_OK;
}

esp_err_t esp_netif_dhcps_stop(esp_netif_t *esp_netif) _RUN_IN_LWIP_TASK_IF_SUPPORTED(esp_netif_dhcps_stop_api, esp_netif, NULL)
#endif

static esp_err_t esp_netif_set_hostname_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    const char *hostname = msg->data;

    ESP_LOGD(TAG, "%s esp_netif:%p hostname %s", __func__, esp_netif, hostname);

    if (!esp_netif) {
        return ESP_ERR_INVALID_ARG;
    }

#if LWIP_NETIF_HOSTNAME

    struct netif *p_netif = esp_netif->lwip_netif;

    if (strlen(hostname) > ESP_NETIF_HOSTNAME_MAX_SIZE) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (p_netif != NULL) {
        if (esp_netif->hostname) {
            free(esp_netif->hostname);
        }
        esp_netif->hostname = strdup(hostname);
        if (esp_netif->hostname == NULL) {
            p_netif->hostname = CONFIG_LWIP_LOCAL_HOSTNAME;
            return ESP_ERR_NO_MEM;
        }
        p_netif->hostname = esp_netif->hostname;
        return ESP_OK;
    } else {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
#else
    return ESP_ERR_ESP_NETIF_IF_NOT_READY;
#endif
}

esp_err_t esp_netif_set_hostname(esp_netif_t *esp_netif, const char *hostname) _RUN_IN_LWIP_TASK_IF_SUPPORTED(esp_netif_set_hostname_api, esp_netif, hostname)

esp_err_t esp_netif_get_hostname(esp_netif_t *esp_netif, const char **hostname)
{
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif || _IS_NETIF_ANY_POINT2POINT_TYPE(esp_netif)) {
        return ESP_ERR_INVALID_ARG;
    }

#if LWIP_NETIF_HOSTNAME
    struct netif *p_netif = esp_netif->lwip_netif;

    if (p_netif != NULL && p_netif->hostname != NULL) {
        *hostname = p_netif->hostname;
        return ESP_OK;
    } else {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
#else
    return ESP_ERR_ESP_NETIF_IF_NOT_READY;
#endif
}

static esp_err_t esp_netif_up_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_STATE;
    }

    struct netif *lwip_netif = esp_netif->lwip_netif;

#if CONFIG_LWIP_IPV4
    /* use last obtained ip, or static ip */
    netif_set_addr(lwip_netif, (ip4_addr_t*)&esp_netif->ip_info->ip, (ip4_addr_t*)&esp_netif->ip_info->netmask, (ip4_addr_t*)&esp_netif->ip_info->gw);
#endif
    netif_set_up(lwip_netif);
    netif_set_link_up(lwip_netif);

    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STARTED);

    return ESP_OK;
}

esp_err_t esp_netif_up(esp_netif_t *esp_netif) _RUN_IN_LWIP_TASK(esp_netif_up_api, esp_netif, NULL)

static esp_err_t esp_netif_down_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_STATE;
    }

    struct netif *lwip_netif = esp_netif->lwip_netif;

    if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT && esp_netif->dhcpc_status == ESP_NETIF_DHCP_STARTED) {
#if CONFIG_LWIP_IPV4
        dhcp_stop(esp_netif->lwip_netif);

        esp_netif->dhcpc_status = ESP_NETIF_DHCP_INIT;

        esp_netif_reset_ip_info(esp_netif);
#endif
    }
#if CONFIG_LWIP_IPV6
#if ESP_MLDV6_REPORT
        if (esp_netif->flags & ESP_NETIF_FLAG_MLDV6_REPORT) {
            netif_unset_mldv6_flag(esp_netif->lwip_netif);
        }
#endif
    for(int8_t i = 0 ;i < LWIP_IPV6_NUM_ADDRESSES ;i++) {
        netif_ip6_addr_set(lwip_netif, i, IP6_ADDR_ANY6);
        netif_ip6_addr_set_valid_life(lwip_netif, i, 0);
        netif_ip6_addr_set_pref_life(lwip_netif, i, 0);
        netif_ip6_addr_set_state(lwip_netif, i, IP6_ADDR_INVALID);
    }
#endif
#if CONFIG_LWIP_IPV4
    netif_set_addr(lwip_netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
#endif
    netif_set_down(lwip_netif);
    netif_set_link_down(lwip_netif);

    if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT) {
#if CONFIG_LWIP_IPV4
        esp_netif_start_ip_lost_timer(esp_netif);
#endif
    }

    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STOPPED);

    return ESP_OK;
}

esp_err_t esp_netif_down(esp_netif_t *esp_netif) _RUN_IN_LWIP_TASK(esp_netif_down_api, esp_netif, NULL)

bool esp_netif_is_netif_up(esp_netif_t *esp_netif)
{
    ESP_LOGV(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif != NULL && esp_netif->lwip_netif != NULL) {
        if (_IS_NETIF_ANY_POINT2POINT_TYPE(esp_netif)) {
            // ppp implementation uses netif_set_link_up/down to update link state
            return netif_is_link_up(esp_netif->lwip_netif);
        }
        // esp-netif handlers and drivers take care to set_netif_up/down on link state update
        return netif_is_up(esp_netif->lwip_netif);
    } else {
        return false;
    }
}

#if CONFIG_LWIP_IPV4
esp_err_t esp_netif_get_old_ip_info(esp_netif_t *esp_netif, esp_netif_ip_info_t *ip_info)
{
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || ip_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(ip_info, esp_netif->ip_info_old, sizeof(esp_netif_ip_info_t));
    return ESP_OK;
}

esp_err_t esp_netif_get_ip_info(esp_netif_t *esp_netif, esp_netif_ip_info_t *ip_info)
{
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || ip_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct netif *p_netif = esp_netif->lwip_netif;

    if (p_netif != NULL && netif_is_up(p_netif)) {
        ip4_addr_set(&ip_info->ip, ip_2_ip4(&p_netif->ip_addr));
        ip4_addr_set(&ip_info->netmask, ip_2_ip4(&p_netif->netmask));
        ip4_addr_set(&ip_info->gw, ip_2_ip4(&p_netif->gw));

        return ESP_OK;
    }

    memcpy(ip_info, esp_netif->ip_info, sizeof(esp_netif_ip_info_t));

    return ESP_OK;
}


bool esp_netif_is_valid_static_ip(esp_netif_ip_info_t *ip_info)
{
    if (!(ip4_addr_isany_val(ip_info->ip) || ip4_addr_isany_val(ip_info->netmask))) {
        // let's assume valid ip_info is when none of ip and netmask is 'any' address (zeros)
        return true;
    }
    return false;
}

static esp_err_t esp_netif_set_ip_old_info_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    const esp_netif_ip_info_t *ip_info = msg->data;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || ip_info == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    memcpy(msg->esp_netif->ip_info_old, msg->data, sizeof(esp_netif_ip_info_t));
    return ESP_OK;
}

esp_err_t esp_netif_set_old_ip_info(esp_netif_t *esp_netif, const esp_netif_ip_info_t *ip_info) _RUN_IN_LWIP_TASK_IF_SUPPORTED(esp_netif_set_ip_old_info_api, esp_netif, ip_info)

static esp_err_t esp_netif_set_ip_info_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    const esp_netif_ip_info_t *ip_info = msg->data;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || ip_info == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
        if (esp_netif->dhcps_status != ESP_NETIF_DHCP_STOPPED) {
            return ESP_ERR_ESP_NETIF_DHCP_NOT_STOPPED;
            }
    } else if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT) {
        if (esp_netif->dhcpc_status != ESP_NETIF_DHCP_STOPPED) {
            return ESP_ERR_ESP_NETIF_DHCP_NOT_STOPPED;
        }
#if LWIP_DNS /* don't build if not configured for use in lwipopts.h */
        dns_clear_servers(true);
#endif
    }

    ip4_addr_copy(esp_netif->ip_info->ip, ip_info->ip);
    ip4_addr_copy(esp_netif->ip_info->gw, ip_info->gw);
    ip4_addr_copy(esp_netif->ip_info->netmask, ip_info->netmask);

    struct netif *p_netif = esp_netif->lwip_netif;

    if (p_netif != NULL && netif_is_up(p_netif)) {
        netif_set_addr(p_netif, (ip4_addr_t*)&ip_info->ip, (ip4_addr_t*)&ip_info->netmask, (ip4_addr_t*)&ip_info->gw);
        if (ESP_NETIF_FLAG_EVENT_IP_MODIFIED & esp_netif->flags) {
            if (!(ip4_addr_isany_val(ip_info->ip) || ip4_addr_isany_val(ip_info->netmask) || ip4_addr_isany_val(ip_info->gw))) {

                ip_event_t evt_id = esp_netif->get_ip_event;
                ip_event_got_ip_t evt = { .esp_netif = esp_netif, .ip_changed = false};
                int ret;
                if (memcmp(ip_info, esp_netif->ip_info_old, sizeof(esp_netif_ip_info_t))) {
                    evt.ip_changed = true;
                }

                memcpy(&evt.ip_info, ip_info, sizeof(esp_netif_ip_info_t));
                memcpy(esp_netif->ip_info_old, ip_info, sizeof(esp_netif_ip_info_t));
                ret = esp_event_post(IP_EVENT, evt_id, &evt, sizeof(evt), 0);
                if (ESP_OK != ret) {
                    ESP_LOGE(TAG, "set ip info: failed to post got ip event (%x)", ret);
                }

                ESP_LOGD(TAG, "if%p netif set static ip: ip changed=%d", esp_netif, evt.ip_changed);

            }
        }
    }

    return ESP_OK;
}

esp_err_t esp_netif_set_ip_info(esp_netif_t *esp_netif, const esp_netif_ip_info_t *ip_info) _RUN_IN_LWIP_TASK_IF_SUPPORTED(esp_netif_set_ip_info_api, esp_netif, ip_info)
#endif /* CONFIG_LWIP_IPV4 */

struct array_mac_ip_t {
    int num;
    esp_netif_pair_mac_ip_t *mac_ip_pair;
};

#if CONFIG_LWIP_DHCPS
static esp_err_t esp_netif_dhcps_get_clients_by_mac_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *netif = msg->esp_netif;
    struct array_mac_ip_t *mac_ip_list = msg->data;
    for (int i = 0; i < mac_ip_list->num; i++) {
        dhcp_search_ip_on_mac(netif->dhcps, mac_ip_list->mac_ip_pair[i].mac, (ip4_addr_t*)&mac_ip_list->mac_ip_pair[i].ip);
    }
    return ESP_OK;
}
#endif // CONFIG_LWIP_DHCPS

esp_err_t esp_netif_dhcps_get_clients_by_mac(esp_netif_t *esp_netif, int num, esp_netif_pair_mac_ip_t *mac_ip_pair)
{
#if CONFIG_LWIP_DHCPS
    if (esp_netif == NULL || esp_netif->dhcps == NULL || num < 0 || mac_ip_pair == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
    struct array_mac_ip_t array_mac_ip = { num, mac_ip_pair };
    return esp_netif_lwip_ipc_call(esp_netif_dhcps_get_clients_by_mac_api, esp_netif, (void *)&array_mac_ip);
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif // CONFIG_LWIP_DHCPS
}

static esp_err_t esp_netif_set_dns_info_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    esp_netif_dns_param_t *dns_param = msg->data;
    esp_netif_dns_type_t type = dns_param->dns_type;
    esp_netif_dns_info_t *dns = dns_param->dns_info;

    ESP_LOGD(TAG, "esp_netif_set_dns_info: if=%p type=%d dns=%" PRIx32, esp_netif, type, dns->ip.u_addr.ip4.addr);

    ip_addr_t lwip_ip = {};
    ESPIP_TO_IP(&dns->ip, &lwip_ip);
    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
#if ESP_DHCPS
        // if DHCP server configured to set DNS in dhcps API
        if (type != ESP_NETIF_DNS_MAIN) {
            ESP_LOGD(TAG, "set dns invalid type");
            return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
        } else {
            dhcps_dns_setserver(esp_netif->dhcps, &lwip_ip);
        }
#else
        LOG_NETIF_DISABLED_AND_DO("DHCP Server", return ESP_ERR_NOT_SUPPORTED);
#endif
    } else {
        dns_setserver(type, &lwip_ip);
    }

    return ESP_OK;
}

esp_err_t esp_netif_set_dns_info(esp_netif_t *esp_netif, esp_netif_dns_type_t type, esp_netif_dns_info_t *dns)
{
    if (esp_netif == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (dns == NULL) {
        ESP_LOGD(TAG, "set dns null dns");
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (ESP_IP_IS_ANY(dns->ip)) {
        ESP_LOGD(TAG, "set dns invalid dns");
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    esp_netif_dns_param_t dns_param = {
        .dns_type = type,
        .dns_info = dns
    };
    return esp_netif_lwip_ipc_call(esp_netif_set_dns_info_api, esp_netif, (void *)&dns_param);
}

static esp_err_t esp_netif_get_dns_info_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    esp_netif_dns_param_t *dns_param = msg->data;
    esp_netif_dns_type_t type = dns_param->dns_type;
    esp_netif_dns_info_t *dns = dns_param->dns_info;

    ESP_LOGD(TAG, "esp_netif_get_dns_info: esp_netif=%p type=%d", esp_netif, type);

    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
#if ESP_DHCPS
        ip4_addr_t dns_ip;
        dhcps_dns_getserver(esp_netif->dhcps, &dns_ip);
        memcpy(&dns->ip.u_addr.ip4, &dns_ip, sizeof(ip4_addr_t));
        dns->ip.type = ESP_IPADDR_TYPE_V4;
#else
        LOG_NETIF_DISABLED_AND_DO("DHCP Server", return ESP_ERR_NOT_SUPPORTED);
#endif
    } else {
        const ip_addr_t*  dns_ip = NULL;
        dns_ip = dns_getserver(type);
        if(dns_ip != NULL) {
            IP_TO_ESPIP(dns_ip, &dns->ip);
        }
    }

    return ESP_OK;
}

esp_err_t esp_netif_get_dns_info(esp_netif_t *esp_netif, esp_netif_dns_type_t type, esp_netif_dns_info_t *dns)
{
    if (esp_netif == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (dns == NULL) {
        ESP_LOGE(TAG, "%s: dns_info cannot be NULL", __func__);
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    esp_netif_dns_param_t dns_param = {
        .dns_type = type,
        .dns_info = dns
    };
    return esp_netif_lwip_ipc_call(esp_netif_get_dns_info_api, esp_netif, (void *)&dns_param);
}

#if CONFIG_LWIP_IPV6

#ifdef CONFIG_LWIP_MLDV6_TMR_INTERVAL

static void netif_send_mldv6(void *arg)
{
    struct netif *netif = arg;
    if (!netif_is_up(netif)) {
        return;
    }
    mld6_report_groups(netif);
    sys_timeout(CONFIG_LWIP_MLDV6_TMR_INTERVAL*1000, netif_send_mldv6, netif);
}

static void netif_set_mldv6_flag(struct netif *netif)
{
    if (!netif_is_up(netif)) {
        return;
    }
    sys_timeout(CONFIG_LWIP_MLDV6_TMR_INTERVAL*1000, netif_send_mldv6, netif);
}

static void netif_unset_mldv6_flag(struct netif *netif)
{
    sys_untimeout(netif_send_mldv6, netif);
}

#endif

esp_ip6_addr_type_t esp_netif_ip6_get_addr_type(esp_ip6_addr_t* ip6_addr)
{
    ip6_addr_t* lwip_ip6_info = (ip6_addr_t*)ip6_addr;

    if (ip6_addr_isglobal(lwip_ip6_info)) {
        return ESP_IP6_ADDR_IS_GLOBAL;
    } else if (ip6_addr_islinklocal(lwip_ip6_info)) {
        return ESP_IP6_ADDR_IS_LINK_LOCAL;
    } else if (ip6_addr_issitelocal(lwip_ip6_info)) {
        return ESP_IP6_ADDR_IS_SITE_LOCAL;
    } else if (ip6_addr_isuniquelocal(lwip_ip6_info)) {
        return ESP_IP6_ADDR_IS_UNIQUE_LOCAL;
    } else if (ip6_addr_isipv4mappedipv6(lwip_ip6_info)) {
        return ESP_IP6_ADDR_IS_IPV4_MAPPED_IPV6;
    }
    return ESP_IP6_ADDR_IS_UNKNOWN;

}

static void esp_netif_internal_nd6_cb(struct netif *netif, uint8_t ip_index)
{
    esp_netif_t *esp_netif;
    ESP_LOGD(TAG, "%s lwip-netif:%p", __func__, netif);
    if (netif == NULL || (esp_netif = lwip_get_esp_netif(netif)) == NULL) {
        // internal pointer hasn't been configured yet (probably in the interface init_fn())
        return;
    }

    esp_netif_ip6_info_t ip6_info;
    ip6_addr_t lwip_ip6_info;
    ip_event_got_ip6_t evt = { .esp_netif = esp_netif, .ip_index = ip_index };

    ip6_addr_set(&lwip_ip6_info, ip_2_ip6(&netif->ip6_addr[ip_index]));
#if LWIP_IPV6_SCOPES
    memcpy(&ip6_info.ip, &lwip_ip6_info, sizeof(esp_ip6_addr_t));
#else
    memcpy(&ip6_info.ip, &lwip_ip6_info, sizeof(ip6_addr_t));
    ip6_info.ip.zone = 0;   // zero out zone, as not used in lwip
#endif /* LWIP_IPV6_SCOPES */

    if (esp_netif->flags&ESP_NETIF_FLAG_MLDV6_REPORT) {
#if ESP_MLDV6_REPORT
        netif_set_mldv6_flag(netif);
#else
        ESP_LOGW(TAG,"CONFIG_LWIP_ESP_MLDV6_REPORT not enabled, but esp-netif configured with ESP_NETIF_FLAG_MLDV6_REPORT");
#endif
    }

    memcpy(&evt.ip6_info, &ip6_info, sizeof(esp_netif_ip6_info_t));
    int ret = esp_event_post(IP_EVENT, IP_EVENT_GOT_IP6, &evt, sizeof(evt), 0);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "nd6 cb: failed to post IP_EVENT_GOT_IP6 (%x)", ret);
    }
}

static esp_err_t esp_netif_create_ip6_linklocal_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGV(TAG, "%s esp-netif:%p", __func__, esp_netif);

    struct netif *p_netif = esp_netif->lwip_netif;
    if (p_netif != NULL && netif_is_up(p_netif)) {
        netif_create_ip6_linklocal_address(p_netif, 1);
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t esp_netif_create_ip6_linklocal(esp_netif_t *esp_netif) _RUN_IN_LWIP_TASK_IF_SUPPORTED(esp_netif_create_ip6_linklocal_api, esp_netif, NULL)

esp_err_t esp_netif_get_ip6_linklocal(esp_netif_t *esp_netif, esp_ip6_addr_t *if_ip6)
{
    ESP_LOGV(TAG, "%s esp-netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || if_ip6 == NULL || _IS_NETIF_ANY_POINT2POINT_TYPE(esp_netif)) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
    struct netif *p_netif = esp_netif->lwip_netif;

    if (p_netif != NULL && netif_is_up(p_netif) && ip6_addr_ispreferred(netif_ip6_addr_state(p_netif, 0))) {
        memcpy(if_ip6, &p_netif->ip6_addr[0], sizeof(ip6_addr_t));
    } else {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t esp_netif_get_ip6_global(esp_netif_t *esp_netif, esp_ip6_addr_t *if_ip6)
{
    ESP_LOGV(TAG, "%s esp-netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || if_ip6 == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    int i;
    struct netif *p_netif = esp_netif->lwip_netif;

    if (p_netif != NULL && netif_is_up(p_netif)) {
        for (i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (ip6_addr_ispreferred(netif_ip6_addr_state(p_netif, i)) &&
                ip6_addr_isglobal(netif_ip6_addr(p_netif, i))) {
                    memcpy(if_ip6, &p_netif->ip6_addr[i], sizeof(ip6_addr_t));
                    return ESP_OK;
            }
        }
    }

    return ESP_FAIL;
}

int esp_netif_get_all_ip6(esp_netif_t *esp_netif, esp_ip6_addr_t if_ip6[])
{
    ESP_LOGV(TAG, "%s esp-netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || if_ip6 == NULL) {
        return 0;
    }

    int addr_count = 0;
    struct netif *p_netif = esp_netif->lwip_netif;

    if (p_netif != NULL && netif_is_up(p_netif)) {
        for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
            if (!ip_addr_cmp(&p_netif->ip6_addr[i], IP6_ADDR_ANY)) {
                memcpy(&if_ip6[addr_count++], &p_netif->ip6_addr[i], sizeof(ip6_addr_t));
            }
        }
    }
    return addr_count;
}
#endif

esp_netif_flags_t esp_netif_get_flags(esp_netif_t *esp_netif)
{
    return esp_netif->flags;
}

const char *esp_netif_get_ifkey(esp_netif_t *esp_netif)
{
    return esp_netif->if_key;
}

const char *esp_netif_get_desc(esp_netif_t *esp_netif)
{
    return esp_netif->if_desc;
}

int esp_netif_get_route_prio(esp_netif_t *esp_netif)
{
    if (esp_netif == NULL) {
        return -1;
    }
    return esp_netif->route_prio;
}

int32_t esp_netif_get_event_id(esp_netif_t *esp_netif, esp_netif_ip_event_type_t event_type)
{
    switch(event_type) {
        case ESP_NETIF_IP_EVENT_GOT_IP:
            return esp_netif->get_ip_event;
        case ESP_NETIF_IP_EVENT_LOST_IP:
            return esp_netif->lost_ip_event;
        default:
            return -1;
    }
}

struct dhcp_params {
    esp_netif_dhcp_option_mode_t op;
    esp_netif_dhcp_option_id_t id;
    void *val;
    uint32_t len;
};

#if ESP_DHCPS
esp_err_t esp_netif_dhcps_option_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    struct dhcp_params *opt = msg->data;
    void *opt_info = dhcps_option_info(esp_netif->dhcps, opt->id, opt->len);

    esp_netif_dhcp_status_t dhcps_status = esp_netif->dhcps_status;
    if (opt_info == NULL || opt->val == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (opt->op == ESP_NETIF_OP_GET) {
        if (dhcps_status == ESP_NETIF_DHCP_STOPPED) {
            return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED;
        }

        switch (opt->id) {
            case IP_ADDRESS_LEASE_TIME: {
                *(uint32_t *)opt->val = *(uint32_t *)opt_info;
                break;
            }
            case ESP_NETIF_SUBNET_MASK:
            case REQUESTED_IP_ADDRESS: {
                memcpy(opt->val, opt_info, opt->len);
                break;
            }
            case ROUTER_SOLICITATION_ADDRESS: {
                if ((*(uint8_t *)opt_info) & OFFER_ROUTER) {
                    *(uint8_t *)opt->val = 1;
                } else {
                    *(uint8_t *)opt->val = 0;
                }
                break;
            }
            case DOMAIN_NAME_SERVER: {
                if ((*(uint8_t *)opt_info) & OFFER_DNS) {
                    *(uint8_t *)opt->val = 1;
                } else {
                    *(uint8_t *)opt->val = 0;
                }
                break;
            }
            default:
                break;
        }
    } else if (opt->op == ESP_NETIF_OP_SET) {
        if (dhcps_status == ESP_NETIF_DHCP_STARTED) {
            return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED;
        }

        switch (opt->id) {
            case IP_ADDRESS_LEASE_TIME: {
                if (*(uint32_t *)opt->val != 0) {
                    *(uint32_t *)opt_info = *(uint32_t *)opt->val;
                } else {
                    *(uint32_t *)opt_info = DHCPS_LEASE_TIME_DEF;
                }
                break;
            }
            case ESP_NETIF_SUBNET_MASK: {
                esp_netif_ip_info_t *default_ip = esp_netif->ip_info;
                ip4_addr_t *config_netmask = (ip4_addr_t *)opt->val;
                if (!memcmp(&default_ip->netmask, config_netmask, sizeof(struct ip4_addr))) {
                    ESP_LOGE(TAG, "Please use esp_netif_set_ip_info interface to configure subnet mask");
                    /*
                     * This API directly changes the subnet mask of dhcp server
                     * but the subnet mask of the network interface has not changed
                     * If you need to change the subnet mask of dhcp server
                     * you need to change the subnet mask of the network interface first.
                     * If the subnet mask of dhcp server is changed
                     * and the subnet mask of network interface is inconsistent
                     * with the subnet mask of dhcp sever, it may lead to the failure of sending packets.
                     * If want to configure the subnet mask of dhcp server
                     * please use esp_netif_set_ip_info to change the subnet mask of network interface first.
                     */
                    return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                }
                memcpy(opt_info, opt->val, opt->len);
                break;
            }
            case REQUESTED_IP_ADDRESS: {
                esp_netif_ip_info_t info;
                uint32_t server_ip = 0;
                uint32_t start_ip = 0;
                uint32_t end_ip = 0;
                uint32_t range_start_ip = 0;
                uint32_t range_end_ip = 0;
                dhcps_lease_t *poll = opt->val;

                if (poll->enable) {
                    memset(&info, 0x00, sizeof(esp_netif_ip_info_t));
                    esp_netif_get_ip_info(esp_netif, &info);

                    server_ip = htonl(info.ip.addr);
                    range_start_ip = server_ip & htonl(info.netmask.addr);
                    range_end_ip = range_start_ip | ~htonl(info.netmask.addr);
                    if (server_ip == range_start_ip || server_ip == range_end_ip) {
                        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                    }
                    start_ip = htonl(poll->start_ip.addr);
                    end_ip = htonl(poll->end_ip.addr);

                    /*config ip information can't contain local ip*/
                    if ((server_ip >= start_ip) && (server_ip <= end_ip)) {
                        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                    }
                    /*config ip information must be in the same segment as the local ip*/
                    if (start_ip <= range_start_ip || start_ip >= range_end_ip) {
                        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                    }
                    if (end_ip <= range_start_ip || end_ip >= range_end_ip) {
                        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                    }
                    /*The number of configured ip is less than DHCPS_MAX_LEASE*/
                    if ((end_ip - start_ip + 1 > DHCPS_MAX_LEASE) || (start_ip >= end_ip)) {
                        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                    }
                }
                memcpy(opt_info, opt->val, opt->len);
                break;
            }
            case ROUTER_SOLICITATION_ADDRESS: {
                if (*(uint8_t *)opt->val) {
                    *(uint8_t *)opt_info |= OFFER_ROUTER;
                } else {
                    *(uint8_t *)opt_info &= ((~OFFER_ROUTER) & 0xFF);
                }
                break;
            }
            case DOMAIN_NAME_SERVER: {
                if (*(uint8_t *)opt->val) {
                    *(uint8_t *)opt_info |= OFFER_DNS;
                } else {
                    *(uint8_t *)opt_info &= ((~OFFER_DNS) & 0xFF);
                }
                break;
            }

            default:
                break;
        }
        dhcps_set_option_info(esp_netif->dhcps, opt->id, opt_info, opt->len);
    } else {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    return ESP_OK;
}

esp_err_t esp_netif_dhcps_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id, void *opt_val,
                                 uint32_t opt_len)
{
    if (esp_netif == NULL || esp_netif->dhcps == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
    struct dhcp_params opts = { .op = opt_op, .id = opt_id, .len = opt_len, .val = opt_val };
    return esp_netif_lwip_ipc_call(esp_netif_dhcps_option_api, esp_netif, &opts);
}
#endif // ESP_DHCPS

#if CONFIG_LWIP_IPV4

esp_err_t esp_netif_dhcpc_option_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    struct dhcp_params *opt = msg->data;

    struct dhcp *dhcp = netif_dhcp_data(esp_netif->lwip_netif);
    if (dhcp == NULL || opt->val == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
    if (opt->op == ESP_NETIF_OP_GET) {
        if (esp_netif->dhcpc_status == ESP_NETIF_DHCP_STOPPED) {
            return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED;
        }
        switch (opt->id) {
            case ESP_NETIF_IP_REQUEST_RETRY_TIME:
                if (opt->len == sizeof(dhcp->tries)) {
                    *(uint8_t *)opt->val = dhcp->tries;
                }
                break;
#if ESP_DHCP && !ESP_DHCP_DISABLE_VENDOR_CLASS_IDENTIFIER
            case ESP_NETIF_VENDOR_SPECIFIC_INFO:
                return dhcp_get_vendor_specific_information(opt->len, opt->val);
#endif
            default:
                return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
        }
    } else if (opt->op == ESP_NETIF_OP_SET) {
        if (esp_netif->dhcpc_status == ESP_NETIF_DHCP_STARTED) {
            return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED;
        }
        switch (opt->id) {
            case ESP_NETIF_IP_REQUEST_RETRY_TIME:
                if (opt->len == sizeof(dhcp->tries)) {
                    dhcp->tries = *(uint8_t *)opt->val;
                }
                break;
#if ESP_DHCP && !ESP_DHCP_DISABLE_VENDOR_CLASS_IDENTIFIER
                case ESP_NETIF_VENDOR_CLASS_IDENTIFIER:
                return dhcp_set_vendor_class_identifier(opt->len, opt->val);
#endif
            default:
                return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
        }
    } else {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
    return ESP_OK;
}

esp_err_t esp_netif_dhcpc_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id, void *opt_val,
                                 uint32_t opt_len)
{
    if (esp_netif == NULL || esp_netif->lwip_netif == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
    struct dhcp_params opts = { .op = opt_op, .id = opt_id, .len = opt_len, .val = opt_val };
    return esp_netif_lwip_ipc_call(esp_netif_dhcpc_option_api, esp_netif, &opts);
}

#endif /* CONFIG_LWIP_IPV4 */

int esp_netif_get_netif_impl_index(esp_netif_t *esp_netif)
{
    if (esp_netif == NULL || esp_netif->lwip_netif == NULL) {
        return -1;
    }
    return netif_get_index(esp_netif->lwip_netif);
}

esp_err_t esp_netif_get_netif_impl_name_api(esp_netif_api_msg_t *msg)
{
    struct netif* netif = msg->esp_netif->lwip_netif;
    netif_index_to_name(netif_get_index(netif), msg->data);
    return ESP_OK;
}

esp_err_t esp_netif_get_netif_impl_name(esp_netif_t *esp_netif, char* name)
{
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || esp_netif->lwip_netif == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }
    return esp_netif_lwip_ipc_call(esp_netif_get_netif_impl_name_api, esp_netif, name);
}

#if IP_NAPT
static esp_err_t esp_netif_napt_control_api(esp_netif_api_msg_t *msg)
{
    bool enable = (bool)msg->data;
    esp_netif_t *esp_netif = msg->esp_netif;
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    /* Check if the interface is up */
    if (!netif_is_up(esp_netif->lwip_netif)) {
        return ESP_FAIL;
    }

    if (enable) {
        /* Disable napt on all other interface */
        esp_netif_t *netif = esp_netif_next_unsafe(NULL);
        while (netif) {
            if (netif != esp_netif) {
                ip_napt_enable_netif(netif->lwip_netif, 0); // Fails only if netif is down
                ESP_LOGW(TAG, "napt disabled on esp_netif:%p", esp_netif);
            }
            netif = esp_netif_next_unsafe(netif);
        }

        int ret = ip_napt_enable_netif(esp_netif->lwip_netif, 1);

        if (ret == 0) {
            return ESP_FAIL;
        }
        return ESP_OK;
    } else {
        ip_napt_enable_netif(esp_netif->lwip_netif, 0); // Fails only if netif is down
        ESP_LOGD(TAG, "napt disabled on esp_netif:%p", esp_netif);

        return ESP_OK;
    }
}
#endif // !IP_NAPT

esp_err_t esp_netif_napt_enable(esp_netif_t *esp_netif)
{
#if !IP_NAPT
    return ESP_ERR_NOT_SUPPORTED;
#else
    return esp_netif_lwip_ipc_call(esp_netif_napt_control_api, esp_netif, (void*)true /* Enable */);
#endif /* IP_NAPT */
}

typedef struct {
    u8_t authtype;
    const char *user;
    const char *passwd;
} set_auth_msg_t;

static esp_err_t esp_netif_ppp_set_auth_api(esp_netif_api_msg_t *msg)
{
    set_auth_msg_t *params = msg->data;
    return esp_netif_ppp_set_auth_internal(msg->esp_netif, params->authtype, params->user, params->passwd);
}

esp_err_t esp_netif_ppp_set_auth(esp_netif_t *esp_netif, esp_netif_auth_type_t authtype, const char *user, const char *passwd)
{
    set_auth_msg_t msg = { .authtype = authtype, .user = user, .passwd = passwd };
    return esp_netif_lwip_ipc_call(esp_netif_ppp_set_auth_api, esp_netif, &msg);
#if PPP_AUTH_SUPPORT
        lwip_peer2peer_ctx_t *ppp_ctx = (lwip_peer2peer_ctx_t *)netif->related_data;
    assert(ppp_ctx->base.netif_type == PPP_LWIP_NETIF);
    pppapi_set_auth(ppp_ctx->ppp, authtype, user, passwd);
    return ESP_OK;
#else
    ESP_LOGE(TAG, "%s failed: No authorisation enabled in menuconfig", __func__);
    return ESP_ERR_ESP_NETIF_IF_NOT_READY;
#endif
}

esp_err_t esp_netif_napt_disable(esp_netif_t *esp_netif)
{
#if !IP_NAPT
    return ESP_ERR_NOT_SUPPORTED;
#else
    return esp_netif_lwip_ipc_call(esp_netif_napt_control_api, esp_netif, (void*)false /* Disable */);
#endif /* IP_NAPT */
}

#if MIB2_STATS
static esp_err_t esp_netif_set_link_speed_api(esp_netif_api_msg_t *msg)
{
    uint32_t speed = *((uint32_t*)msg->data);
    esp_err_t error = ESP_OK;
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, msg->esp_netif);
    NETIF_INIT_SNMP(msg->esp_netif->lwip_netif, snmp_ifType_ethernet_csmacd, speed);
    LWIP_UNUSED_ARG(speed);     // Maybe unused if SNMP disabled
    return error;
}

esp_err_t esp_netif_set_link_speed(esp_netif_t *esp_netif, uint32_t speed)
_RUN_IN_LWIP_TASK(esp_netif_set_link_speed_api, esp_netif, &speed)
#else
esp_err_t esp_netif_set_link_speed(esp_netif_t *esp_netif, uint32_t speed)
{
    // link speed is used only to collect interface related statistics (if MIB2_STATS enabled)
    return ESP_OK;
}
#endif /* MIB2_STATS */

#if CONFIG_LWIP_IPV6

static esp_err_t esp_netif_join_ip6_multicast_group_api(esp_netif_api_msg_t *msg)
{
    esp_ip6_addr_t *addr = (esp_ip6_addr_t *)msg->data;
    esp_err_t error = ESP_OK;
    ip6_addr_t ip6addr;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, msg->esp_netif);
    memcpy(ip6addr.addr, addr->addr, sizeof(ip6addr.addr));
#if LWIP_IPV6_SCOPES
    ip6addr.zone = 0;
#endif
    if (mld6_joingroup_netif(msg->esp_netif->lwip_netif, &ip6addr) != ERR_OK) {
        error = ESP_ERR_ESP_NETIF_MLD6_FAILED;
        ESP_LOGE(TAG, "failed to join ip6 multicast group");
    }
    return error;
}

esp_err_t esp_netif_join_ip6_multicast_group(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr)
    _RUN_IN_LWIP_TASK(esp_netif_join_ip6_multicast_group_api, esp_netif, addr)

static esp_err_t esp_netif_leave_ip6_multicast_group_api(esp_netif_api_msg_t *msg)
{
    esp_ip6_addr_t *addr = (esp_ip6_addr_t *)msg->data;
    ip6_addr_t ip6addr;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, msg->esp_netif);
    memcpy(ip6addr.addr, addr->addr, sizeof(ip6addr.addr));
#if LWIP_IPV6_SCOPES
    ip6addr.zone = 0;
#endif
    ESP_RETURN_ON_FALSE(mld6_leavegroup_netif(msg->esp_netif->lwip_netif, &ip6addr) == ERR_OK,
                        ESP_ERR_ESP_NETIF_MLD6_FAILED, TAG, "Failed to leave ip6 multicast group");
    return ESP_OK;
}

esp_err_t esp_netif_leave_ip6_multicast_group(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr)
    _RUN_IN_LWIP_TASK(esp_netif_leave_ip6_multicast_group_api, esp_netif, addr)

static esp_err_t esp_netif_add_ip6_address_api(esp_netif_api_msg_t *msg)
{
    ip_event_add_ip6_t *addr = (ip_event_add_ip6_t *)msg->data;
    ip6_addr_t ip6addr;
    esp_err_t error = ESP_OK;
    int8_t index = -1;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, msg->esp_netif);
    memcpy(ip6addr.addr, addr->addr.addr, sizeof(ip6addr.addr));
#if LWIP_IPV6_SCOPES
    ip6addr.zone = 0;
#endif
    err_t err = netif_add_ip6_address(msg->esp_netif->lwip_netif, &ip6addr, &index);
    ESP_RETURN_ON_FALSE(err == ERR_OK && index >= 0, ESP_ERR_ESP_NETIF_IP6_ADDR_FAILED, TAG,
                        "Failed to add ip6 address");

    netif_ip6_addr_set_state(msg->esp_netif->lwip_netif, index,
                             addr->preferred ? IP6_ADDR_PREFERRED : IP6_ADDR_DEPRECATED);
    ip_event_got_ip6_t evt = {.esp_netif = msg->esp_netif, .ip_index = index};
    evt.ip6_info.ip = addr->addr;
    ESP_RETURN_ON_ERROR(esp_event_post(IP_EVENT, IP_EVENT_GOT_IP6, &evt, sizeof(evt), 0), TAG,
                        "Failed to post IP_EVENT_GOT_IP6");
    return error;
}

esp_err_t esp_netif_add_ip6_address(esp_netif_t *esp_netif, const ip_event_add_ip6_t *addr)
    _RUN_IN_LWIP_TASK(esp_netif_add_ip6_address_api, esp_netif, addr)

static esp_err_t esp_netif_remove_ip6_address_api(esp_netif_api_msg_t *msg)
{
    esp_ip6_addr_t *addr = (esp_ip6_addr_t *)msg->data;
    ip6_addr_t ip6addr;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, msg->esp_netif);
    memcpy(ip6addr.addr, addr->addr, sizeof(ip6addr.addr));
#if LWIP_IPV6_SCOPES
    ip6addr.zone = 0;
#endif
    int8_t index = netif_get_ip6_addr_match(msg->esp_netif->lwip_netif, &ip6addr);
    if (index != -1) {
        netif_ip6_addr_set_state(msg->esp_netif->lwip_netif, index, IP6_ADDR_INVALID);
    }
    return ESP_OK;
}

esp_err_t esp_netif_remove_ip6_address(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr)
    _RUN_IN_LWIP_TASK(esp_netif_remove_ip6_address_api, esp_netif, addr)

#endif // CONFIG_LWIP_IPV6
