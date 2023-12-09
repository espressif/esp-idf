/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>


#include "esp_netif.h"
#include "esp_netif_private.h"

#if CONFIG_ESP_NETIF_LOOPBACK

#include "esp_log.h"

//
// Purpose of this module is to implement minimal loopback netif to facilitate
//   low level driver testing
//

#define ESP_NETIF_HOSTNAME_MAX_SIZE    32

static const char *TAG = "esp_netif_loopback";

static bool s_netif_initialized = false;
static bool s_netif_started = false;
static bool s_netif_up = false;

/**
 * @brief Main esp-netif container with interface related information
 *
 *
 */

#ifndef NETIF_MAX_HWADDR_LEN
#define NETIF_MAX_HWADDR_LEN 6U
#endif

struct esp_netif_obj {
    // default interface addresses
    uint8_t mac[NETIF_MAX_HWADDR_LEN];
    esp_netif_ip_info_t* ip_info;
    esp_netif_ip_info_t* ip_info_old;

    // io driver related
    void* driver_handle;
    esp_err_t (*driver_transmit)(void *h, void *buffer, size_t len);
    void (*driver_free_rx_buffer)(void *h, void* buffer);

    // misc flags, types, keys, priority
    esp_netif_flags_t flags;
    char * hostname;
    char * if_key;
    char * if_desc;
    int route_prio;
};

void esp_netif_set_ip4_addr(esp_ip4_addr_t *addr, uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    memset(addr, 0, sizeof(esp_ip4_addr_t));
    addr->addr = esp_netif_htonl(esp_netif_ip4_makeu32(a,b,c,d));
}

char * esp_ip4addr_ntoa(const esp_ip4_addr_t *addr, char *buf, int buflen)
{
    return NULL;
}

esp_netif_iodriver_handle esp_netif_get_io_driver(esp_netif_t *esp_netif)
{
    return esp_netif->driver_handle;
}

esp_netif_t* esp_netif_get_handle_from_netif_impl(void *dev)
{
    return NULL;
}

void* esp_netif_get_netif_impl(esp_netif_t *esp_netif)
{
    return NULL;
}

esp_err_t esp_netif_init(void)
{
    ESP_LOGI(TAG, "loopback initialization");
    if (s_netif_initialized) {
        ESP_LOGE(TAG, "esp-netif has already been initialized");
        return ESP_ERR_INVALID_SIZE;
    }

    s_netif_initialized = true;
    ESP_LOGD(TAG, "esp-netif has been successfully initialized");
    return ESP_OK;
}

esp_err_t esp_netif_deinit(void)
{
    ESP_LOGI(TAG, "loopback initialization");
    if (!s_netif_initialized) {
        ESP_LOGE(TAG, "esp-netif has not been initialized yet");
        return ESP_ERR_INVALID_SIZE;
    }
    s_netif_initialized = false;
    ESP_LOGD(TAG, "esp-netif has been successfully deinitialized");
    return ESP_OK;
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
        esp_netif->ip_info->ip.addr = 0;
        esp_netif->ip_info->gw.addr = 0;
        esp_netif->ip_info->netmask.addr = 0;
    } else {
        memcpy(esp_netif->ip_info, cfg->base->ip_info, sizeof(esp_netif_ip_info_t));
    }
    memcpy(esp_netif->ip_info_old, esp_netif->ip_info, sizeof(esp_netif_ip_info_t));

    // Setup main config parameters
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

    // Network stack is bypassed in loopback interface

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
    if (esp_netif_config == NULL) {
        return NULL;
    }

    // Create parent esp-netif object
    esp_netif_t *esp_netif = calloc(1, sizeof(struct esp_netif_obj));
    if (!esp_netif) {
        return NULL;
    }

    // Create ip info
    esp_netif_ip_info_t *ip_info = calloc(1, sizeof(esp_netif_ip_info_t));
    if (!ip_info) {
        free(esp_netif);
        return NULL;
    }
    esp_netif->ip_info = ip_info;

    // creating another ip info (to store old ip)
    ip_info = calloc(1, sizeof(esp_netif_ip_info_t));
    if (!ip_info) {
        free(esp_netif->ip_info);
        free(esp_netif);
        return NULL;
    }
    esp_netif->ip_info_old = ip_info;

    esp_netif_add_to_list_unsafe(esp_netif);

    // Configure the created object with provided configuration
    esp_err_t ret =  esp_netif_init_configuration(esp_netif, esp_netif_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Initial configuration of esp_netif failed with %d", ret);
        esp_netif_destroy(esp_netif);
        return NULL;
    }

    return esp_netif;
}

void esp_netif_destroy(esp_netif_t *esp_netif)
{
    if (esp_netif) {
        esp_netif_remove_from_list_unsafe(esp_netif);
        free(esp_netif->ip_info);
        free(esp_netif->ip_info_old);
        free(esp_netif->if_key);
        free(esp_netif->if_desc);
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

esp_err_t esp_netif_set_mac(esp_netif_t *esp_netif, uint8_t mac[])
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_start(esp_netif_t *esp_netif)
{
    ESP_LOGI(TAG, "Netif started");
    s_netif_started = true;
    return ESP_OK;
}


esp_err_t esp_netif_stop(esp_netif_t *esp_netif)
{
    ESP_LOGI(TAG, "Netif stopped");
    s_netif_started = false;
    return ESP_OK;
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
    ESP_LOGV(TAG, "Transmitting data: ptr:%p, size:%lu", data, (long unsigned int) len);
    return (esp_netif->driver_transmit)(esp_netif->driver_handle, data, len);
}

esp_err_t esp_netif_receive(esp_netif_t *esp_netif, void *buffer, size_t len, void *eb)
{
    ESP_LOGV(TAG, "Received data: ptr:%p, size:%lu", buffer, (long unsigned int) len);
    esp_netif_transmit(esp_netif, buffer, len);
    if (eb) {
        esp_netif_free_rx_buffer(esp_netif, eb);
    }
    return ESP_OK;
}

esp_err_t esp_netif_dhcpc_stop(esp_netif_t *esp_netif)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_dhcpc_start(esp_netif_t *esp_netif)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_dhcps_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_dhcpc_get_status(esp_netif_t *esp_netif, esp_netif_dhcp_status_t *status)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_dhcps_start(esp_netif_t *esp_netif)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_dhcps_stop(esp_netif_t *esp_netif)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_set_hostname(esp_netif_t *esp_netif, const char *hostname)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_get_hostname(esp_netif_t *esp_netif, const char **hostname)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_up(esp_netif_t *esp_netif)
{
    ESP_LOGI(TAG, "Netif going up");
    s_netif_up = true;
    return ESP_OK;
}

esp_err_t esp_netif_down(esp_netif_t *esp_netif)
{
    ESP_LOGI(TAG, "Netif going down");
    s_netif_up = false;
    return ESP_OK;
}

bool esp_netif_is_netif_up(esp_netif_t *esp_netif)
{
    return s_netif_up;
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

    memcpy(ip_info, esp_netif->ip_info, sizeof(esp_netif_ip_info_t));
    return ESP_OK;
}


bool esp_netif_is_valid_static_ip(esp_netif_ip_info_t *ip_info)
{
    return true;
}

esp_err_t esp_netif_set_old_ip_info(esp_netif_t *esp_netif, const esp_netif_ip_info_t *ip_info)
{
    ESP_LOGD(TAG, "%s esp_netif:%p", __func__, esp_netif);

    if (esp_netif == NULL || ip_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    memcpy(esp_netif->ip_info_old, ip_info, sizeof(esp_netif_ip_info_t));
    return ESP_OK;
}

esp_err_t esp_netif_set_dns_info(esp_netif_t *esp_netif, esp_netif_dns_type_t type, esp_netif_dns_info_t *dns)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_get_dns_info(esp_netif_t *esp_netif, esp_netif_dns_type_t type, esp_netif_dns_info_t *dns)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_create_ip6_linklocal(esp_netif_t *esp_netif)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_get_ip6_linklocal(esp_netif_t *esp_netif, esp_ip6_addr_t *if_ip6)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_get_ip6_global(esp_netif_t *esp_netif, esp_ip6_addr_t *if_ip6)
{
    return ESP_ERR_NOT_SUPPORTED;
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
    return 0;
}

esp_err_t esp_netif_dhcps_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id, void *opt_val,
                                 uint32_t opt_len)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_dhcpc_option(esp_netif_t *esp_netif, esp_netif_dhcp_option_mode_t opt_op, esp_netif_dhcp_option_id_t opt_id, void *opt_val,
                                 uint32_t opt_len)
{
    return ESP_ERR_NOT_SUPPORTED;
}

int esp_netif_get_netif_impl_index(esp_netif_t *esp_netif)
{
    return 0;
}

esp_err_t esp_netif_join_ip6_multicast_group(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_leave_ip6_multicast_group(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_add_ip6_address(esp_netif_t *esp_netif, const ip_event_add_ip6_t *addr)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_netif_remove_ip6_address(esp_netif_t *esp_netif, const esp_ip6_addr_t *addr)
{
    return ESP_ERR_NOT_SUPPORTED;
}

#endif /* CONFIG_ESP_NETIF_LOOPBACK */
