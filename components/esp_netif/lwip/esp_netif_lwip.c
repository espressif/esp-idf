// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>

#include "esp_netif_lwip_internal.h"

#include "esp_netif.h"
#include "esp_netif_private.h"

#if CONFIG_ESP_NETIF_TCPIP_LWIP

#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/nd6.h"
#include "lwip/priv/tcpip_priv.h"
#include "lwip/netif.h"
#if LWIP_DNS /* don't build if not configured for use in lwipopts.h */
#include "lwip/dns.h"
#endif

#include "dhcpserver/dhcpserver.h"
#include "dhcpserver/dhcpserver_options.h"

#include "esp_event.h"
#include "esp_log.h"

//
// This is the main module implementing lwip interaction with esp-netif
//

#define ESP_NETIF_HOSTNAME_MAX_SIZE    32

/**
 * @brief thread safe tcpip function utility macro
 */
#define _LWIP_TASK_IPC_CALL(function, netif, param) \
{   \
    return esp_netif_lwip_ipc_call(function, netif, (void *)(param)); \
}

//
//  Internal types
//
typedef enum esp_netif_action {
    ESP_NETIF_UNDEF,
    ESP_NETIF_STARTED,
    ESP_NETIF_STOPPED,
} esp_netif_action_t;

/**
 * @brief Main esp-netif container with interface related information
 */
struct esp_netif_obj {
    // default interface addresses
    uint8_t mac[NETIF_MAX_HWADDR_LEN];
    esp_netif_ip_info_t* ip_info;
    esp_netif_ip_info_t* ip_info_old;

    // lwip netif related
    struct netif* lwip_netif;
    err_t (*lwip_init_fn)(struct netif*);
    void (*lwip_input_fn)(struct netif *netif, void *buffer, size_t len, void *eb);

    // io driver related
    void* driver_handle;
    esp_err_t (*driver_transmit)(void *h, void *buffer, size_t len);
    void (*driver_free_rx_buffer)(void *h, void* buffer);

    // dhcp related
    esp_netif_dhcp_status_t dhcpc_status;
    esp_netif_dhcp_status_t dhcps_status;
    bool timer_running;

    // event translation
    ip_event_t get_ip_event;
    ip_event_t lost_ip_event;

    // misc flags, types, keys, priority
    esp_netif_flags_t flags;
    char * hostname;
    char * if_key;
    char * if_desc;
    int route_prio;
};

//
//  Internal variables for this module
//
extern sys_thread_t g_lwip_task;

static const char *TAG = "esp_netif_lwip";

static sys_sem_t api_sync_sem = NULL;
static sys_sem_t api_lock_sem = NULL;
static bool tcpip_initialized = false;
static esp_netif_t *s_last_default_esp_netif = NULL;

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
    sys_sem_signal(&api_sync_sem);

}

/**
 * @brief Initiates a tcpip remote call if called from another task
 * or calls the function directly if executed from lwip task
 */
static inline esp_err_t esp_netif_lwip_ipc_call(esp_netif_api_fn fn, esp_netif_t *netif, void *data)
{
    esp_netif_api_msg_t msg = {
            .esp_netif = netif,
            .data = data,
            .api_fn = fn
    };
    if (g_lwip_task != xTaskGetCurrentTaskHandle()) {
        ESP_LOGD(TAG, "check: remote, if=%p fn=%p\n", netif, fn);
        sys_arch_sem_wait(&api_lock_sem, 0);
        tcpip_send_msg_wait_sem((tcpip_callback_fn)esp_netif_api_cb, &msg, &api_sync_sem);
        sys_sem_signal(&api_lock_sem);
        return msg.ret;
    }
    ESP_LOGD(TAG, "check: local, if=%p fn=%p\n", netif, fn);
    return fn(&msg);
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
 * @brief This function sets default routing netif based on priorities of all interfaces which are up
 * @param esp_netif current interface which just updated state
 * @param action updating action (on-off)
 *
 * @note: This function must be called from lwip thread
 *
 */
static void esp_netif_update_default_netif(esp_netif_t *esp_netif, esp_netif_action_t action) {
    switch (action) {
        case ESP_NETIF_STARTED:
        {
            // check if previously default interface hasn't been destroyed in the meantime
            s_last_default_esp_netif = esp_netif_is_active(s_last_default_esp_netif);
            if (s_last_default_esp_netif && esp_netif_is_netif_up(s_last_default_esp_netif)
                && (s_last_default_esp_netif->route_prio > esp_netif->route_prio)) {
                netif_set_default(s_last_default_esp_netif->lwip_netif);
            } else if (esp_netif_is_netif_up(esp_netif)) {
                s_last_default_esp_netif = esp_netif;
                netif_set_default(s_last_default_esp_netif->lwip_netif);
            }
        }
        break;

        default:
        case ESP_NETIF_STOPPED:
        {
            s_last_default_esp_netif = NULL;
            esp_netif_list_lock();
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
            esp_netif_list_unlock();
            if (s_last_default_esp_netif && esp_netif_is_netif_up(s_last_default_esp_netif)) {
                netif_set_default(s_last_default_esp_netif->lwip_netif);
            }
        }
        break;
    }
}

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


esp_netif_iodriver_handle esp_netif_get_io_driver(esp_netif_t *esp_netif)
{
    return esp_netif->driver_handle;
}

esp_netif_t* esp_netif_get_handle_from_netif_impl(void *dev)
{
    struct netif *lwip_netif = dev;
    return lwip_netif->state;
}

void* esp_netif_get_netif_impl(esp_netif_t *esp_netif)
{
    if (esp_netif) {
        return esp_netif->lwip_netif;
    }
    return NULL;
}

esp_err_t esp_netif_init(void)
{
    if (tcpip_initialized == false) {
        tcpip_initialized = true;
        tcpip_init(NULL, NULL);
        ESP_LOGD(TAG, "LwIP stack has been initialized");
    }

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

    ESP_LOGD(TAG, "esp-netif has been successfully initialized");
    return ESP_OK;
}

esp_err_t esp_netif_deinit(void)
{
    if (tcpip_initialized == true) {
        /* deinit of LwIP not supported:
         * do not deinit semaphores and states,
         * so init could be called multiple times
         *
        tcpip_initialized = false;
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
    if (cfg->base->ip_info == NULL) {
        ip4_addr_set_zero(&esp_netif->ip_info->ip);
        ip4_addr_set_zero(&esp_netif->ip_info->gw);
        ip4_addr_set_zero(&esp_netif->ip_info->netmask);
    } else {
        memcpy(esp_netif->ip_info, cfg->base->ip_info, sizeof(esp_netif_ip_info_t));
    }
    memcpy(esp_netif->ip_info_old, esp_netif->ip_info, sizeof(esp_netif_ip_info_t));

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

    // Install network stack functions -- connects netif and L3 stack
    const esp_netif_netstack_config_t *esp_netif_stack_config = cfg->stack;
    if (esp_netif_stack_config->init_fn) {
        esp_netif->lwip_init_fn = esp_netif_stack_config->init_fn;
    }
    if (esp_netif_stack_config->input_fn) {
        esp_netif->lwip_input_fn = esp_netif_stack_config->input_fn;
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
        if (esp_netif_driver_config->driver_free_rx_buffer) {
            esp_netif->driver_free_rx_buffer = esp_netif_driver_config->driver_free_rx_buffer;
        }
    }
    return ESP_OK;
}

esp_netif_t *esp_netif_new(const esp_netif_config_t *esp_netif_config)
{
    // mandatory configuration must be provided when creating esp_netif object
    if (esp_netif_config == NULL ||
        esp_netif_config->base->if_key == NULL ||
        NULL != esp_netif_get_handle_from_ifkey(esp_netif_config->base->if_key)) {
        ESP_LOGE(TAG, "%s: Failed to configure netif with config=%p (config or if_key is NULL or duplicate key)",
        __func__,  esp_netif_config);
        return NULL;
    }

    // Create parent esp-netif object
    esp_netif_t *esp_netif = calloc(1, sizeof(struct esp_netif_obj));
    if (!esp_netif) {
        ESP_LOGE(TAG, "Failed to allocate %d bytes (fee heap size %d)", sizeof(struct esp_netif_obj),
                 esp_get_free_heap_size());
        return NULL;
    }

    // Create ip info
    esp_netif_ip_info_t *ip_info = calloc(1, sizeof(esp_netif_ip_info_t));
    if (!ip_info) {
        ESP_LOGE(TAG, "Failed to allocate %d bytes (fee heap size %d)", sizeof(esp_netif_ip_info_t),
                 esp_get_free_heap_size());
        free(esp_netif);
        return NULL;
    }
    esp_netif->ip_info = ip_info;

    // creating another ip info (to store old ip)
    ip_info = calloc(1, sizeof(esp_netif_ip_info_t));
    if (!ip_info) {
        ESP_LOGE(TAG, "Failed to allocate %d bytes (fee heap size %d)", sizeof(esp_netif_ip_info_t),
                 esp_get_free_heap_size());
        free(esp_netif->ip_info);
        free(esp_netif);
        return NULL;
    }
    esp_netif->ip_info_old = ip_info;

    // Create underlying lwip netif
    struct netif * lwip_netif = calloc(1, sizeof(struct netif));
    if (!lwip_netif) {
        ESP_LOGE(TAG, "Failed to allocate %d bytes (fee heap size %d)", sizeof(struct netif),
                 esp_get_free_heap_size());
        free(esp_netif->ip_info_old);
        free(esp_netif->ip_info);
        free(esp_netif);
        return NULL;
    }

    lwip_netif->state = esp_netif;
    esp_netif->lwip_netif = lwip_netif;

    esp_netif_add_to_list(esp_netif);

    // Configure the created object with provided configuration
    esp_err_t ret =  esp_netif_init_configuration(esp_netif, esp_netif_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Initial configuration of esp_netif failed with %d", ret);
        esp_netif_destroy(esp_netif);
        return NULL;
    }

    return esp_netif;
}

static void esp_netif_lwip_remove(esp_netif_t *esp_netif)
{
    if (esp_netif->lwip_netif) {
        if (netif_is_up(esp_netif->lwip_netif)) {
            netif_set_down(esp_netif->lwip_netif);
        }
        netif_remove(esp_netif->lwip_netif);
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
    if (NULL == netif_add(esp_netif->lwip_netif, (struct ip4_addr*)&esp_netif->ip_info->ip,
                         (struct ip4_addr*)&esp_netif->ip_info->netmask, (struct ip4_addr*)&esp_netif->ip_info->gw,
                         esp_netif, esp_netif->lwip_init_fn, tcpip_input)) {
        esp_netif_lwip_remove(esp_netif);
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
    return ESP_OK;
}

void esp_netif_destroy(esp_netif_t *esp_netif)
{
    if (esp_netif) {
        esp_netif_remove_from_list(esp_netif);
        free(esp_netif->ip_info);
        free(esp_netif->ip_info_old);
        free(esp_netif->if_key);
        free(esp_netif->if_desc);
        esp_netif_lwip_remove(esp_netif);
        free(esp_netif->lwip_netif);
        free(esp_netif->hostname);
        if (s_last_default_esp_netif == esp_netif) {
            // clear last default netif if it happens to be this just destroyed interface
            s_last_default_esp_netif = NULL;
        }
        free(esp_netif);
    }
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
    esp_netif->driver_free_rx_buffer = driver_config->driver_free_rx_buffer;
    return ESP_OK;
}

static esp_err_t esp_netif_reset_ip_info(esp_netif_t *esp_netif)
{
    ip4_addr_set_zero(&(esp_netif->ip_info->ip));
    ip4_addr_set_zero(&(esp_netif->ip_info->gw));
    ip4_addr_set_zero(&(esp_netif->ip_info->netmask));
    return ESP_OK;
}

esp_err_t esp_netif_set_mac(esp_netif_t *esp_netif, uint8_t mac[])
{
    if (esp_netif == NULL || esp_netif->lwip_netif == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
    memcpy(esp_netif->mac, mac, NETIF_MAX_HWADDR_LEN);
    memcpy(esp_netif->lwip_netif->hwaddr, mac, NETIF_MAX_HWADDR_LEN);
    return ESP_OK;
}


static void esp_netif_dhcps_cb(u8_t client_ip[4])
{
    ESP_LOGI(TAG, "softAP assign IP to station,IP is: %d.%d.%d.%d",
             client_ip[0], client_ip[1], client_ip[2], client_ip[3]);
    ip_event_ap_staipassigned_t evt;

    memset(&evt, 0, sizeof(ip_event_ap_staipassigned_t));
    memcpy((char *)&evt.ip.addr, (char *)client_ip, sizeof(evt.ip.addr));
    int ret = esp_event_send_internal(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &evt, sizeof(evt), 0);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "dhcps cb: failed to post IP_EVENT_AP_STAIPASSIGNED (%x)", ret);
    }
}

static esp_err_t esp_netif_config_sanity_check(const esp_netif_t * esp_netif)
{
    if (esp_netif == NULL) {
        ESP_LOGE(TAG,  "Cannot start esp_netif: esp_netif must not be null");
        return ESP_ERR_INVALID_STATE;
    }

    if (esp_netif->driver_transmit == NULL ||
        esp_netif->driver_handle == NULL ||
        esp_netif->lwip_input_fn == NULL ||
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

    ESP_ERROR_CHECK(esp_netif_config_sanity_check(esp_netif));

    ESP_ERROR_CHECK(esp_netif_lwip_add(esp_netif));

    if (esp_netif->flags&ESP_NETIF_FLAG_GARP) {
#if ESP_GRATUITOUS_ARP
        netif_set_garp_flag(esp_netif->lwip_netif);
#else
        ESP_LOGW(TAG,"CONFIG_LWIP_ESP_GRATUITOUS_ARP not enabled, but esp-netif configured woth ESP_NETIF_FLAG_GARP")
#endif
    }
    struct netif *p_netif = esp_netif->lwip_netif;
    if (esp_netif->flags&ESP_NETIF_FLAG_AUTOUP) {
        ESP_LOGD(TAG, "%s Setting the lwip netif%p UP", __func__, p_netif);
        netif_set_up(p_netif);
    }
    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
        if (esp_netif->dhcps_status != ESP_NETIF_DHCP_STARTED) {
            if (p_netif != NULL && netif_is_up(p_netif)) {
                esp_netif_ip_info_t *default_ip = esp_netif->ip_info;
                ip4_addr_t lwip_ip;
                memcpy(&lwip_ip, &default_ip->ip, sizeof(struct ip4_addr));
                dhcps_set_new_lease_cb(esp_netif_dhcps_cb);
                dhcps_start(p_netif, lwip_ip);
                esp_netif->dhcps_status = ESP_NETIF_DHCP_STARTED;
                ESP_LOGD(TAG, "DHCP server started successfully");
                esp_netif_update_default_netif(esp_netif, ESP_NETIF_STARTED);
                return ESP_OK;
            } else {
                ESP_LOGD(TAG, "DHCP server re init");
                esp_netif->dhcps_status = ESP_NETIF_DHCP_INIT;
                return ESP_OK;
            }
        }
        ESP_LOGD(TAG, "DHCP server already started");
        return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED;
    }

    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STARTED);

    return ESP_OK;
}

esp_err_t esp_netif_start(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_start_api, esp_netif, NULL)

static esp_err_t esp_netif_stop_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    struct netif *lwip_netif = esp_netif->lwip_netif;
    if (lwip_netif == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }

    if (!netif_is_up(lwip_netif)) {
        esp_netif_lwip_remove(esp_netif);
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }

    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
        dhcps_stop(lwip_netif);    // TODO(IDF-1099): dhcps checks status by its self
        if (ESP_NETIF_DHCP_STOPPED != esp_netif->dhcps_status) {
            esp_netif->dhcps_status = ESP_NETIF_DHCP_INIT;
        }
    } else if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT) {
        dhcp_release(lwip_netif);
        dhcp_stop(lwip_netif);
        dhcp_cleanup(lwip_netif);

        esp_netif->dhcpc_status = ESP_NETIF_DHCP_INIT;

        esp_netif_reset_ip_info(esp_netif);
    }

    netif_set_down(lwip_netif);
    esp_netif_lwip_remove(esp_netif);
    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STOPPED);;

    return ESP_OK;
}

esp_err_t esp_netif_stop(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_stop_api, esp_netif, NULL)

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

esp_err_t esp_netif_receive(esp_netif_t *esp_netif, void *buffer, size_t len, void *eb)
{
    esp_netif->lwip_input_fn(esp_netif->lwip_netif, buffer, len, eb);
    return ESP_OK;
}

//
// DHCP:
//
static esp_err_t esp_netif_start_ip_lost_timer(esp_netif_t *esp_netif);

static void esp_netif_dhcpc_cb(struct netif *netif)
{
    if (!netif) {
        ESP_LOGD(TAG, "null netif=%p", netif);
        return;
    }
    ESP_LOGD(TAG, "%s lwip-netif:%p", __func__, netif);

    esp_netif_t *esp_netif = esp_netif_get_handle_from_netif_impl(netif);

    esp_netif_ip_info_t *ip_info = esp_netif->ip_info;
    esp_netif_ip_info_t *ip_info_old = esp_netif->ip_info_old;


    if ( !ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), IP4_ADDR_ANY4) ) {

        //check whether IP is changed
        if ( !ip4_addr_cmp(ip_2_ip4(&netif->ip_addr), (&ip_info->ip)) ||
             !ip4_addr_cmp(ip_2_ip4(&netif->netmask), (&ip_info->netmask)) ||
             !ip4_addr_cmp(ip_2_ip4(&netif->gw), (&ip_info->gw)) ) {
            ip_event_got_ip_t evt = {
                    .esp_netif = esp_netif,
                    .if_index = -1, // invalid index, handle used
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
            ret = esp_event_send_internal(IP_EVENT, evt_id, &evt, sizeof(evt), 0);
            if (ESP_OK != ret) {
                ESP_LOGE(TAG, "dhcpc cb: failed to post got ip event (%x)", ret);
            }
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
                .if_index = -1,
        };
        int ret;

        ESP_LOGD(TAG, "if%p ip lost tmr: raise ip lost event", esp_netif);
        memset(esp_netif->ip_info_old, 0, sizeof(esp_netif_ip_info_t));
        if (esp_netif->lost_ip_event) {
            ret = esp_event_send_internal(IP_EVENT, esp_netif->lost_ip_event,
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

    if ( netif && (CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL > 0) && !ip4_addr_isany_val(ip_info_old->ip)) {
        esp_netif->timer_running = true;
        sys_timeout(CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL * 1000, esp_netif_ip_lost_timer, (void *)esp_netif);
        ESP_LOGD(TAG, "if%p start ip lost tmr: interval=%d", esp_netif, CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL);
        return ESP_OK;
    }

    ESP_LOGD(TAG, "if%p start ip lost tmr: no need start because netif=%p interval=%d ip=%x",
             esp_netif, netif, CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL, ip_info_old->ip.addr);

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

    LWIP_DHCP_IP_ADDR_ERASE(esp_netif);

    return ESP_OK;
}

esp_err_t esp_netif_dhcpc_stop(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_dhcpc_stop_api, esp_netif, NULL)

static esp_err_t esp_netif_dhcpc_start_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_ARG;
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

        dhcp_set_cb(p_netif, esp_netif_dhcpc_cb);

        esp_netif->dhcpc_status = ESP_NETIF_DHCP_STARTED;
        return ESP_OK;
    } else {
        ESP_LOGD(TAG, "dhcp client re init");
        esp_netif->dhcpc_status = ESP_NETIF_DHCP_INIT;
        return ESP_OK;
    }
}

esp_err_t esp_netif_dhcpc_start(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_dhcpc_start_api, esp_netif, NULL)

esp_err_t esp_netif_dhcps_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status)
{
    if (!esp_netif || (esp_netif->flags & ESP_NETIF_DHCP_CLIENT)) {
        return ESP_ERR_INVALID_ARG;
    }

    *status = esp_netif->dhcps_status;
    return ESP_OK;
}

esp_err_t esp_netif_dhcpc_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status)
{
    if (!esp_netif || (esp_netif->flags & ESP_NETIF_DHCP_SERVER)) {
        return ESP_ERR_INVALID_ARG;
    }

    *status = esp_netif->dhcpc_status;
    return ESP_OK;
}

static esp_err_t esp_netif_dhcps_start_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_ARG;
    }

    struct netif *p_netif = esp_netif->lwip_netif;
    if (p_netif != NULL && netif_is_up(p_netif)) {
        esp_netif_ip_info_t *default_ip = esp_netif->ip_info;
        ip4_addr_t lwip_ip;
        memcpy(&lwip_ip, &default_ip->ip, sizeof(struct ip4_addr));
        dhcps_set_new_lease_cb(esp_netif_dhcps_cb);
        dhcps_start(p_netif, lwip_ip);
        esp_netif->dhcps_status = ESP_NETIF_DHCP_STARTED;
        ESP_LOGD(TAG, "DHCP server started successfully");
        return ESP_OK;
    } else {
        ESP_LOGD(TAG, "dhcp server re init");
        esp_netif->dhcps_status = ESP_NETIF_DHCP_INIT;
        return ESP_OK;
    }
}

esp_err_t esp_netif_dhcps_start(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_dhcps_start_api, esp_netif, NULL)

static esp_err_t esp_netif_dhcps_stop_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_ARG;
    }

    struct netif *p_netif = esp_netif->lwip_netif;
    if (esp_netif->dhcps_status == ESP_NETIF_DHCP_STARTED) {
        if (p_netif != NULL) {
            dhcps_stop(p_netif);
        } else {
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

esp_err_t esp_netif_dhcps_stop(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_dhcps_stop_api, esp_netif, NULL)

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
    if (esp_netif->hostname) {
        free(esp_netif->hostname);
    }
    esp_netif->hostname = strdup(hostname);
    if (esp_netif->hostname == NULL) {
        return ESP_ERR_NO_MEM;
    }

    if (strlen(hostname) > ESP_NETIF_HOSTNAME_MAX_SIZE) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (p_netif != NULL) {
        p_netif->hostname = esp_netif->hostname;
        return ESP_OK;
    } else {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }
#else
    return ESP_ERR_ESP_NETIF_IF_NOT_READY;
#endif
}

esp_err_t esp_netif_set_hostname(esp_netif_t *esp_netif, const char *hostname) _LWIP_TASK_IPC_CALL(esp_netif_set_hostname_api, esp_netif, hostname)

esp_err_t esp_netif_get_hostname(esp_netif_t *esp_netif, const char **hostname)
{
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_ARG;
    }

#if LWIP_NETIF_HOSTNAME
    struct netif *p_netif = esp_netif->lwip_netif;

    if (p_netif != NULL) {
        *hostname = p_netif->hostname;
        return ESP_OK;
    } else {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
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

    /* use last obtained ip, or static ip */
    netif_set_addr(lwip_netif, (ip4_addr_t*)&esp_netif->ip_info->ip, (ip4_addr_t*)&esp_netif->ip_info->netmask, (ip4_addr_t*)&esp_netif->ip_info->gw);
    netif_set_up(lwip_netif);

    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STARTED);

    return ESP_OK;
}

esp_err_t esp_netif_up(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_up_api, esp_netif, NULL)

static esp_err_t esp_netif_down_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!esp_netif) {
        return ESP_ERR_INVALID_STATE;
    }

    struct netif *lwip_netif = esp_netif->lwip_netif;

    if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT && esp_netif->dhcpc_status == ESP_NETIF_DHCP_STARTED) {
        dhcp_stop(esp_netif->lwip_netif);

        esp_netif->dhcpc_status = ESP_NETIF_DHCP_INIT;

        esp_netif_reset_ip_info(esp_netif);
    }

    netif_set_addr(lwip_netif, IP4_ADDR_ANY4, IP4_ADDR_ANY4, IP4_ADDR_ANY4);
    netif_set_down(lwip_netif);

    if (esp_netif->flags & ESP_NETIF_DHCP_CLIENT) {
        esp_netif_start_ip_lost_timer(esp_netif);
    }

    esp_netif_update_default_netif(esp_netif, ESP_NETIF_STOPPED);

    return ESP_OK;
}

esp_err_t esp_netif_down(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_down_api, esp_netif, NULL)

bool esp_netif_is_netif_up(esp_netif_t *esp_netif)
{
    ESP_LOGV(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif != NULL && esp_netif->lwip_netif != NULL && netif_is_up(esp_netif->lwip_netif)) {
        return true;
    } else {
        return false;
    }
}

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

esp_err_t esp_netif_set_old_ip_info(esp_netif_t *esp_netif, const esp_netif_ip_info_t *ip_info) _LWIP_TASK_IPC_CALL(esp_netif_set_ip_old_info_api, esp_netif, ip_info)

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
                ip_event_got_ip_t evt = { .esp_netif = esp_netif, .if_index = -1, .ip_changed = false};
                int ret;
                if (memcmp(ip_info, esp_netif->ip_info_old, sizeof(esp_netif_ip_info_t))) {
                    evt.ip_changed = true;
                }

                memcpy(&evt.ip_info, ip_info, sizeof(esp_netif_ip_info_t));
                memcpy(esp_netif->ip_info_old, ip_info, sizeof(esp_netif_ip_info_t));
                ret = esp_event_send_internal(IP_EVENT, evt_id, &evt, sizeof(evt), 0);
                if (ESP_OK != ret) {
                    ESP_LOGE(TAG, "set ip info: failed to post got ip event (%x)", ret);
                }

                ESP_LOGD(TAG, "if%p netif set static ip: ip changed=%d", esp_netif, evt.ip_changed);

            }
        }
    }

    return ESP_OK;
}

esp_err_t esp_netif_set_ip_info(esp_netif_t *esp_netif, const esp_netif_ip_info_t *ip_info) _LWIP_TASK_IPC_CALL(esp_netif_set_ip_info_api, esp_netif, ip_info)

static esp_err_t esp_netif_set_dns_info_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;
    esp_netif_dns_param_t *dns_param = msg->data;
    esp_netif_dns_type_t type = dns_param->dns_type;
    esp_netif_dns_info_t *dns = dns_param->dns_info;

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (!dns) {
        ESP_LOGD(TAG, "set dns null dns");
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (ip4_addr_isany_val(dns->ip.u_addr.ip4)) {
        ESP_LOGD(TAG, "set dns invalid dns");
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    ESP_LOGD(TAG, "set dns if=%p type=%d dns=%x", esp_netif, type, dns->ip.u_addr.ip4.addr);

    ip_addr_t *lwip_ip = (ip_addr_t*)&dns->ip;
    lwip_ip->type = IPADDR_TYPE_V4;

    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
        // if DHCP server configured to set DNS in dhcps API
        if (type != ESP_NETIF_DNS_MAIN) {
            ESP_LOGD(TAG, "set dns invalid type");
            return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
        } else {
            dhcps_dns_setserver(lwip_ip);
        }
    } else {
        dns_setserver(type, lwip_ip);
    }

    return ESP_OK;
}

esp_err_t esp_netif_set_dns_info(esp_netif_t *esp_netif, esp_netif_dns_type_t type, esp_netif_dns_info_t *dns)
{
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

    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (!dns) {
        ESP_LOGE(TAG, "%s: dns_info cannot be NULL", __func__);
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (esp_netif->flags & ESP_NETIF_DHCP_SERVER) {
        ip4_addr_t dns_ip = dhcps_dns_getserver();
        memcpy(&dns->ip.u_addr.ip4, &dns_ip, sizeof(ip4_addr_t));
    } else {
        const ip_addr_t*  dns_ip = NULL;
        dns_ip = dns_getserver(type);
        if(dns_ip != NULL) {
            memcpy(&dns->ip.u_addr, &dns_ip, sizeof(ip4_addr_t));
        }
    }

    return ESP_OK;
}

esp_err_t esp_netif_get_dns_info(esp_netif_t *esp_netif, esp_netif_dns_type_t type, esp_netif_dns_info_t *dns)
{
    esp_netif_dns_param_t dns_param = {
        .dns_type = type,
        .dns_info = dns
    };
    return esp_netif_lwip_ipc_call(esp_netif_get_dns_info_api, esp_netif, (void *)&dns_param);
}

static void esp_netif_nd6_cb(struct netif *p_netif, uint8_t ip_idex)
{
    ESP_LOGD(TAG, "%s lwip-netif:%p", __func__, p_netif);
    if (!p_netif) {
        ESP_LOGD(TAG, "esp_netif_nd6_cb called with null p_netif");
        return;
    }

    esp_netif_ip6_info_t ip6_info;
    ip6_addr_t lwip_ip6_info;
    //notify event
    ip_event_got_ip6_t evt = { .esp_netif = p_netif->state, .if_index = -1 };

    ip6_addr_set(&lwip_ip6_info, ip_2_ip6(&p_netif->ip6_addr[ip_idex]));
#if LWIP_IPV6_SCOPES
    memcpy(&ip6_info.ip, &lwip_ip6_info, sizeof(esp_ip6_addr_t));
#else
    memcpy(&ip6_info.ip, &lwip_ip6_info, sizeof(ip6_addr_t));
    ip6_info.ip.zone = 0;   // zero out zone, as not used in lwip
#endif /* LWIP_IPV6_SCOPES */

    memcpy(&evt.ip6_info, &ip6_info, sizeof(esp_netif_ip6_info_t));
    int ret = esp_event_send_internal(IP_EVENT, IP_EVENT_GOT_IP6, &evt, sizeof(evt), 0);
    if (ESP_OK != ret) {
        ESP_LOGE(TAG, "nd6 cb: failed to post IP_EVENT_GOT_IP6 (%x)", ret);
    }
}

static esp_err_t esp_netif_create_ip6_linklocal_api(esp_netif_api_msg_t *msg)
{
    esp_netif_t *esp_netif = msg->esp_netif;

    ESP_LOGD(TAG, "%s esp-netif:%p", __func__, esp_netif);

    struct netif *p_netif = esp_netif->lwip_netif;
    if (p_netif != NULL && netif_is_up(p_netif)) {
        netif_create_ip6_linklocal_address(p_netif, 1);
        nd6_set_cb(p_netif, esp_netif_nd6_cb);
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

esp_err_t esp_netif_create_ip6_linklocal(esp_netif_t *esp_netif) _LWIP_TASK_IPC_CALL(esp_netif_create_ip6_linklocal_api, esp_netif, NULL)

esp_err_t esp_netif_get_ip6_linklocal(esp_netif_t *esp_netif, esp_ip6_addr_t *if_ip6)
{
    ESP_LOGD(TAG, "%s esp-netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || if_ip6 == NULL) {
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

esp_err_t esp_netif_dhcps_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id, void *opt_val,
                                 uint32_t opt_len)
{
    void *opt_info = dhcps_option_info(opt_id, opt_len);
    if (esp_netif == NULL) {
        return ESP_ERR_ESP_NETIF_IF_NOT_READY;
    }

    esp_netif_dhcp_status_t dhcps_status = esp_netif->dhcps_status;
    if (opt_info == NULL || opt_val == NULL) {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    if (opt_op == ESP_NETIF_OP_GET) {
        if (dhcps_status == ESP_NETIF_DHCP_STOPPED) {
            return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED;
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
                if ((*(uint8_t *)opt_info) & OFFER_ROUTER) {
                    *(uint8_t *)opt_val = 1;
                } else {
                    *(uint8_t *)opt_val = 0;
                }
                break;
            }
            case DOMAIN_NAME_SERVER: {
                if ((*(uint8_t *)opt_info) & OFFER_DNS) {
                    *(uint8_t *)opt_val = 1;
                } else {
                    *(uint8_t *)opt_val = 0;
                }
                break;
            }
            default:
                break;
        }
    } else if (opt_op == ESP_NETIF_OP_SET) {
        if (dhcps_status == ESP_NETIF_DHCP_STARTED) {
            return ESP_ERR_ESP_NETIF_DHCP_ALREADY_STARTED;
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
                esp_netif_ip_info_t info;
                uint32_t softap_ip = 0;
                uint32_t start_ip = 0;
                uint32_t end_ip = 0;
                dhcps_lease_t *poll = opt_val;

                if (poll->enable) {
                    memset(&info, 0x00, sizeof(esp_netif_ip_info_t));
                    esp_netif_get_ip_info(esp_netif, &info);

                    softap_ip = htonl(info.ip.addr);
                    start_ip = htonl(poll->start_ip.addr);
                    end_ip = htonl(poll->end_ip.addr);

                    /*config ip information can't contain local ip*/
                    if ((start_ip <= softap_ip) && (softap_ip <= end_ip)) {
                        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                    }

                    /*config ip information must be in the same segment as the local ip*/
                    softap_ip >>= 8;
                    if ((start_ip >> 8 != softap_ip)
                        || (end_ip >> 8 != softap_ip)) {
                        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                    }

                    if (end_ip - start_ip > DHCPS_MAX_LEASE) {
                        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
                    }
                }

                memcpy(opt_info, opt_val, opt_len);
                break;
            }
            case ROUTER_SOLICITATION_ADDRESS: {
                if (*(uint8_t *)opt_val) {
                    *(uint8_t *)opt_info |= OFFER_ROUTER;
                } else {
                    *(uint8_t *)opt_info &= ((~OFFER_ROUTER) & 0xFF);
                }
                break;
            }
            case DOMAIN_NAME_SERVER: {
                if (*(uint8_t *)opt_val) {
                    *(uint8_t *)opt_info |= OFFER_DNS;
                } else {
                    *(uint8_t *)opt_info &= ((~OFFER_DNS) & 0xFF);
                }
                break;
            }

            default:
                break;
        }
        dhcps_set_option_info(opt_id, opt_info, opt_len);
    } else {
        return ESP_ERR_ESP_NETIF_INVALID_PARAMS;
    }

    return ESP_OK;
}

esp_err_t esp_netif_dhcpc_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id, void *opt_val,
                                 uint32_t opt_len)
{
    // TODO(IDF-1100): when dhcp request timeout,change the retry count
    return ESP_ERR_NOT_SUPPORTED;
}

int esp_netif_get_netif_impl_index(esp_netif_t *esp_netif)
{
    if (esp_netif == NULL || esp_netif->lwip_netif == NULL) {
        return -1;
    }
    return netif_get_index(esp_netif->lwip_netif);
}

#endif /* CONFIG_ESP_NETIF_TCPIP_LWIP */
