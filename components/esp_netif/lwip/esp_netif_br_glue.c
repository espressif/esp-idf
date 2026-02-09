/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include "esp_netif_br_glue.h"
#include "esp_eth_driver.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"

#if CONFIG_ESP_NETIF_BRIDGE_EN

const static char *TAG = "esp_netif_br_glue";

typedef struct esp_netif_br_glue_t esp_netif_br_glue_t;

typedef enum {
    START_CTX_HANDLER,
    STOP_CTX_HANDLER,
    CONNECT_CTX_HANDLER,
    DISCONNECT_CTX_HANDLER,
    CTX_HANDLERS_END_LIST
} ctx_handl_type_t;

struct esp_netif_br_glue_t {
    esp_netif_driver_base_t base;
    bool br_started;
    esp_netif_t **ports_esp_netifs;
    esp_netif_t *wifi_esp_netif;
    uint8_t port_cnt;
    esp_event_handler_instance_t *eth_ctx_handlers;
    esp_event_handler_instance_t get_ip_ctx_handler;
    esp_event_handler_instance_t *wifi_ctx_handlers;
};

static esp_err_t esp_eth_post_attach_br(esp_netif_t *esp_netif, void *args)
{
    uint8_t eth_mac[6];
    esp_netif_br_glue_t *netif_glue = (esp_netif_br_glue_t *)args;
    netif_glue->base.netif = esp_netif;

    esp_netif_get_mac(esp_netif, eth_mac);
    ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x", eth_mac[0], eth_mac[1],
             eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);

    ESP_LOGI(TAG, "bridge netif glue attached");

    return ESP_OK;
}

static void start_br_if_stopped(esp_netif_br_glue_t *netif_glue)
{
    if (netif_glue->br_started == false) {
        esp_netif_action_start(netif_glue->base.netif, 0, 0, NULL); // basically creates lwip_netif br instance
        netif_glue->br_started = true;
        ESP_LOGD(TAG, "bridge netif %p is started", netif_glue->base.netif);
    }
}

static void stop_br_if_started(esp_netif_br_glue_t *netif_glue)
{
    if (netif_glue->br_started == true) {
        esp_netif_action_stop(netif_glue->base.netif, 0, 0, NULL); // basically removes lwip_netif br
        netif_glue->br_started = false;
        ESP_LOGD(TAG, "bridge netif %p is stopped", netif_glue->base.netif);
    }
}

static bool are_ports_disconnected(esp_netif_br_glue_t *netif_glue)
{
    int disc_cnt;
    // check Ethernet ports at first
    for (disc_cnt = 0; disc_cnt < netif_glue->port_cnt; disc_cnt++) {
        if (esp_netif_is_netif_up(netif_glue->ports_esp_netifs[disc_cnt]) == true) {
            break;
        }
    }

    if (disc_cnt >= netif_glue->port_cnt) {
        // check WiFi port if is also registered
        if (netif_glue->wifi_esp_netif != NULL) {
            if (esp_netif_is_netif_up(netif_glue->wifi_esp_netif) == false) {
                return true;
            }
        } else {
            return true;
        }
    }
    return false;
}

static void port_action_start(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_netif_br_glue_t *netif_glue = handler_args;

#if CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED
    if (base == WIFI_EVENT) {
        ESP_LOGD(TAG, "wifi_action_start: %p, %p, %" PRId32 ", %p", netif_glue, base, event_id, event_data);
        start_br_if_stopped(netif_glue);
        esp_netif_bridge_add_port(netif_glue->base.netif, netif_glue->wifi_esp_netif);
    } else
#endif /* CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED */
    if (base == ETH_EVENT) {
        esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
        ESP_LOGD(TAG, "eth_action_start: %p, %p, %" PRId32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
        for (int i = 0; i < netif_glue->port_cnt; i++) {
            if (eth_handle == esp_netif_get_io_driver(netif_glue->ports_esp_netifs[i])) {
                start_br_if_stopped(netif_glue);
                esp_netif_bridge_add_port(netif_glue->base.netif, netif_glue->ports_esp_netifs[i]);
            }
        }
    }
}

static void port_action_stop(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_netif_br_glue_t *netif_glue = handler_args;

    // if one of the bridge's ports is stopped, we need to stop the bridge too, since port's lwip_netif is removed and so it would become
    // an invalid reference in the bridge's internal structure (there is no way how to remove single port from bridge in current LwIP)
#if CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED
    if (base == WIFI_EVENT) {
        ESP_LOGD(TAG, "wifi_action_stop: %p, %p, %" PRId32 ", %p", netif_glue, base, event_id, event_data);
        stop_br_if_started(netif_glue);
    } else
#endif /* CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED */
    if (base == ETH_EVENT) {
        esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
        ESP_LOGD(TAG, "eth_action_stop: %p, %p, %" PRId32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
        for (int i = 0; i < netif_glue->port_cnt; i++) {
            if (eth_handle == esp_netif_get_io_driver(netif_glue->ports_esp_netifs[i])) {
                stop_br_if_started(netif_glue);
            }
        }
    }
}

static void port_action_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_netif_br_glue_t *netif_glue = handler_args;

    // if bridge interface is already up, do nothing
    if (esp_netif_is_netif_up(netif_glue->base.netif) == true) {
        ESP_LOGD(TAG, "action_connected, no action bridge is up");
        return;
    }
#if CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED
    if (base == WIFI_EVENT) {
        ESP_LOGD(TAG, "wifi_action_connected: %p, %p, %" PRId32 ", %p", netif_glue, base, event_id, event_data);
        esp_netif_action_connected(netif_glue->base.netif, 0, 0, NULL);
    } else
#endif /* CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED */
    if (base == ETH_EVENT) {
        esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
        ESP_LOGD(TAG, "eth_action_connected: %p, %p, %" PRId32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
        for (int i = 0; i < netif_glue->port_cnt; i++) {
            if (eth_handle == esp_netif_get_io_driver(netif_glue->ports_esp_netifs[i])) {
                esp_netif_action_connected(netif_glue->base.netif, 0, 0, NULL);
                ESP_LOGD(TAG, "bridge netif %p is connected", netif_glue->base.netif);
                break;
            }
        }
    }
    if (esp_netif_is_netif_up(netif_glue->base.netif) == true &&
            (esp_netif_get_flags(netif_glue->base.netif) & ESP_NETIF_DHCP_SERVER) == ESP_NETIF_DHCP_SERVER) {
        esp_netif_dhcps_start(netif_glue->base.netif);
    }
}

static void port_action_disconnected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_netif_br_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "action_disconnected: %p, %p, %" PRId32 ", %p", netif_glue, base, event_id, event_data);
    // if all ports are disconnected, set bridge as disconnected too
    if (are_ports_disconnected(netif_glue)) {
        esp_netif_action_disconnected(netif_glue->base.netif, base, event_id, event_data);
        ESP_LOGD(TAG, "bridge netif %p is disconnected", netif_glue->base.netif);
    }
}

static void br_action_got_ip(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *)event_data;
    esp_netif_br_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "br_action_got_ip: %p, %p, %" PRIu32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->base.netif == ip_event->esp_netif) {
        esp_netif_action_got_ip(ip_event->esp_netif, base, event_id, event_data);
    }
}

static esp_err_t esp_netif_br_glue_clear_instance_handlers(esp_netif_br_glue_handle_t esp_netif_br_glue)
{
    ESP_RETURN_ON_FALSE(esp_netif_br_glue, ESP_ERR_INVALID_ARG, TAG, "esp_netif_br_glue handle can't be null");

    if (esp_netif_br_glue->eth_ctx_handlers[START_CTX_HANDLER]) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_START, esp_netif_br_glue->eth_ctx_handlers[START_CTX_HANDLER]);
    }

    if (esp_netif_br_glue->eth_ctx_handlers[STOP_CTX_HANDLER]) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_STOP, esp_netif_br_glue->eth_ctx_handlers[STOP_CTX_HANDLER]);
    }

    if (esp_netif_br_glue->eth_ctx_handlers[CONNECT_CTX_HANDLER]) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, esp_netif_br_glue->eth_ctx_handlers[CONNECT_CTX_HANDLER]);
    }

    if (esp_netif_br_glue->eth_ctx_handlers[DISCONNECT_CTX_HANDLER]) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, esp_netif_br_glue->eth_ctx_handlers[DISCONNECT_CTX_HANDLER]);
    }
    free(esp_netif_br_glue->eth_ctx_handlers);

    if (esp_netif_br_glue->get_ip_ctx_handler) {
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, esp_netif_br_glue->get_ip_ctx_handler);
        esp_netif_br_glue->get_ip_ctx_handler = NULL;
    }

    return ESP_OK;
}

static esp_err_t esp_netif_br_glue_set_instance_handlers(esp_netif_br_glue_handle_t esp_netif_br_glue)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(esp_netif_br_glue, ESP_ERR_INVALID_ARG, TAG, "esp_netif_br_glue handle can't be null");

    esp_netif_br_glue->eth_ctx_handlers = calloc(CTX_HANDLERS_END_LIST, sizeof(esp_event_handler_instance_t));
    ESP_GOTO_ON_FALSE(esp_netif_br_glue->eth_ctx_handlers, ESP_ERR_NO_MEM, fail, TAG, "no memory for Ethernet event context handlers");

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_START, port_action_start, esp_netif_br_glue, &esp_netif_br_glue->eth_ctx_handlers[START_CTX_HANDLER]);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_STOP, port_action_stop, esp_netif_br_glue, &esp_netif_br_glue->eth_ctx_handlers[STOP_CTX_HANDLER]);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, port_action_connected, esp_netif_br_glue, &esp_netif_br_glue->eth_ctx_handlers[CONNECT_CTX_HANDLER]);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, port_action_disconnected, esp_netif_br_glue, &esp_netif_br_glue->eth_ctx_handlers[DISCONNECT_CTX_HANDLER]);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, br_action_got_ip, esp_netif_br_glue, &esp_netif_br_glue->get_ip_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    return ESP_OK;

fail:
    esp_netif_br_glue_clear_instance_handlers(esp_netif_br_glue);
    return ret;
}

#if CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED
static esp_err_t esp_netif_br_glue_clear_instance_handlers_wifi(esp_netif_br_glue_handle_t esp_netif_br_glue)
{
    ESP_RETURN_ON_FALSE(esp_netif_br_glue, ESP_ERR_INVALID_ARG, TAG, "esp_netif_br_glue handle can't be null");
    ESP_RETURN_ON_FALSE(esp_netif_br_glue->wifi_ctx_handlers, ESP_ERR_INVALID_STATE, TAG, "WiFi event handlers are empty");

    if (esp_netif_br_glue->wifi_ctx_handlers[START_CTX_HANDLER]) {
        esp_event_handler_instance_unregister(WIFI_EVENT, WIFI_EVENT_AP_START, esp_netif_br_glue->wifi_ctx_handlers[START_CTX_HANDLER]);
    }

    if (esp_netif_br_glue->wifi_ctx_handlers[STOP_CTX_HANDLER]) {
        esp_event_handler_instance_unregister(WIFI_EVENT, WIFI_EVENT_AP_STOP, esp_netif_br_glue->wifi_ctx_handlers[STOP_CTX_HANDLER]);
    }

    if (esp_netif_br_glue->wifi_ctx_handlers[CONNECT_CTX_HANDLER]) {
        esp_event_handler_instance_unregister(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, esp_netif_br_glue->wifi_ctx_handlers[CONNECT_CTX_HANDLER]);
    }

    if (esp_netif_br_glue->wifi_ctx_handlers[DISCONNECT_CTX_HANDLER]) {
        esp_event_handler_instance_unregister(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, esp_netif_br_glue->wifi_ctx_handlers[DISCONNECT_CTX_HANDLER]);
    }
    free(esp_netif_br_glue->wifi_ctx_handlers);

    return ESP_OK;
}

static esp_err_t esp_netif_br_glue_set_instance_handlers_wifi(esp_netif_br_glue_handle_t esp_netif_br_glue)
{
    esp_err_t ret= ESP_OK;

    ESP_RETURN_ON_FALSE(esp_netif_br_glue, ESP_ERR_INVALID_ARG, TAG, "esp_netif_br_glue handle can't be null");
    ESP_RETURN_ON_FALSE(esp_netif_br_glue->wifi_esp_netif, ESP_ERR_INVALID_ARG, TAG, "WiFi port esp_netif isn't registered");

    esp_netif_br_glue->wifi_ctx_handlers = calloc(CTX_HANDLERS_END_LIST, sizeof(esp_event_handler_instance_t));
    ESP_GOTO_ON_FALSE(esp_netif_br_glue->wifi_ctx_handlers, ESP_ERR_NO_MEM, fail, TAG, "no memory for WiFi event context handlers");

    ret = esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_AP_START, port_action_start, esp_netif_br_glue, &esp_netif_br_glue->wifi_ctx_handlers[START_CTX_HANDLER]);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_AP_STOP, port_action_stop, esp_netif_br_glue, &esp_netif_br_glue->wifi_ctx_handlers[STOP_CTX_HANDLER]);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, port_action_connected, esp_netif_br_glue, &esp_netif_br_glue->wifi_ctx_handlers[CONNECT_CTX_HANDLER]);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, port_action_disconnected, esp_netif_br_glue, &esp_netif_br_glue->wifi_ctx_handlers[DISCONNECT_CTX_HANDLER]);
    if (ret != ESP_OK) {
        goto fail;
    }

    return ESP_OK;
fail:
    esp_netif_br_glue_clear_instance_handlers_wifi(esp_netif_br_glue);
    return ret;
}
#endif /* CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED */

esp_err_t esp_netif_br_glue_add_port(esp_netif_br_glue_handle_t netif_br_glue, esp_netif_t *esp_netif_port)
{
    if (netif_br_glue->ports_esp_netifs == NULL) {
        netif_br_glue->ports_esp_netifs = malloc(sizeof(esp_netif_t *));
    } else {
        esp_netif_t **new_ports = realloc(netif_br_glue->ports_esp_netifs, (netif_br_glue->port_cnt + 1) * sizeof(esp_netif_t *));
        if (new_ports == NULL) {
            free(netif_br_glue->ports_esp_netifs);
            netif_br_glue->ports_esp_netifs = NULL;
            netif_br_glue->port_cnt = 0;
            ESP_LOGE(TAG, "no memory to add br port");
            return ESP_ERR_NO_MEM;
        }
        netif_br_glue->ports_esp_netifs = new_ports;
    }

    netif_br_glue->ports_esp_netifs[netif_br_glue->port_cnt] = esp_netif_port;
    netif_br_glue->port_cnt++;

    return ESP_OK;
}

esp_err_t esp_netif_br_glue_add_wifi_port(esp_netif_br_glue_handle_t netif_br_glue, esp_netif_t *esp_netif_port)
{
#if CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(netif_br_glue->wifi_esp_netif == NULL, ESP_ERR_INVALID_STATE, fail_ret, TAG, "WiFi interface already registered");
    const char *if_desc = esp_netif_get_desc(esp_netif_port);
    ESP_GOTO_ON_FALSE(strcmp(if_desc, "ap") == 0, ESP_ERR_INVALID_ARG, fail_ret, TAG, "interface is not WiFi AP");

    netif_br_glue->wifi_esp_netif = esp_netif_port;
    ESP_GOTO_ON_ERROR(esp_netif_br_glue_set_instance_handlers_wifi(netif_br_glue), fail, TAG, "failed to create WiFi event handlers");

    return ESP_OK;
fail:
    netif_br_glue->wifi_esp_netif = NULL;
    netif_br_glue->wifi_ctx_handlers = NULL;
fail_ret:
    return ret;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif /* CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED */
}

esp_netif_br_glue_handle_t esp_netif_br_glue_new(void)
{
    esp_netif_br_glue_t *netif_glue = calloc(1, sizeof(esp_netif_br_glue_t));
    if (!netif_glue) {
        ESP_LOGE(TAG, "create netif glue failed");
        return NULL;
    }

    netif_glue->base.post_attach = esp_eth_post_attach_br;

    if (esp_netif_br_glue_set_instance_handlers(netif_glue) != ESP_OK) {
        esp_netif_br_glue_del(netif_glue);
        return NULL;
    }

    return netif_glue;
}

esp_err_t esp_netif_br_glue_del(esp_netif_br_glue_handle_t netif_br_glue)
{
    stop_br_if_started(netif_br_glue);
    esp_netif_br_glue_clear_instance_handlers(netif_br_glue);
#if CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED
    if (netif_br_glue->wifi_esp_netif != NULL) {
        esp_netif_br_glue_clear_instance_handlers_wifi(netif_br_glue);
    }
#endif /* CONFIG_ESP_WIFI_ENABLED || CONFIG_SLAVE_SOC_WIFI_SUPPORTED */
    free(netif_br_glue->ports_esp_netifs);
    free(netif_br_glue);
    netif_br_glue = NULL;
    return ESP_OK;
}

#endif // CONFIG_ESP_NETIF_BRIDGE_EN
