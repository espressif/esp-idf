/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include "esp_netif_br_glue.h"
#include "esp_eth_driver.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"

#if CONFIG_ESP_NETIF_BRIDGE_EN

const static char *TAG = "esp_netif_br_glue";

typedef struct esp_netif_br_glue_t esp_netif_br_glue_t;

struct esp_netif_br_glue_t {
    esp_netif_driver_base_t base;
    bool br_started;
    esp_netif_t **ports_esp_netifs;
    uint8_t port_cnt;
    esp_event_handler_instance_t eth_start_ctx_handler;
    esp_event_handler_instance_t eth_stop_ctx_handler;
    esp_event_handler_instance_t eth_connect_ctx_handler;
    esp_event_handler_instance_t eth_disconnect_ctx_handler;
    esp_event_handler_instance_t get_ip_ctx_handler;
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

static void eth_action_start(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_netif_br_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_start: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);

    for (int i = 0; i < netif_glue->port_cnt; i++) {
        if (eth_handle == esp_netif_get_io_driver(netif_glue->ports_esp_netifs[i])) {
            if (netif_glue->br_started == false) {
                esp_netif_action_start(netif_glue->base.netif, base, event_id, event_data); // basically creates lwip_netif br instance
                netif_glue->br_started = true;
                ESP_LOGD(TAG, "bridge netif %p is started", netif_glue->base.netif);
            }
            esp_netif_bridge_add_port(netif_glue->base.netif, netif_glue->ports_esp_netifs[i]);
        }
    }
}

static void eth_action_stop(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_netif_br_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_stop: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);

    for (int i = 0; i < netif_glue->port_cnt; i++) {
        // if one of the bridge's ports is stopped, we need to stop the bridge too, since port's lwip_netif is removed and so it would become
        // an invalid reference in the bridge's internal structure (there is no way how to remove single port from bridge in current LwIP)
        if (eth_handle == esp_netif_get_io_driver(netif_glue->ports_esp_netifs[i])) {
            if (netif_glue->br_started == true) {
                esp_netif_action_stop(netif_glue->base.netif, base, event_id, event_data); // basically removes lwip_netif br
                netif_glue->br_started = false;
                ESP_LOGD(TAG, "bridge netif %p is stopped", netif_glue->base.netif);
            }
        }
    }
}

static void eth_action_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_netif_br_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_connected: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);

    // if bridge interface is already up, do nothing
    if (esp_netif_is_netif_up(netif_glue->base.netif) == true) {
        return;
    }

    for (int i = 0; i < netif_glue->port_cnt; i++) {
        if (eth_handle == esp_netif_get_io_driver(netif_glue->ports_esp_netifs[i])) {
            esp_netif_action_connected(netif_glue->base.netif, base, event_id, event_data);
            ESP_LOGD(TAG, "bridge netif %p is connected", netif_glue->base.netif);
            break;
        }
    }
}

static void eth_action_disconnected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_netif_br_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_disconnected: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);

    for (int i = 0; i < netif_glue->port_cnt; i++) {
        // if this is a Ethernet interface associated with bridge, check if other ports are disconnected
        if (eth_handle == esp_netif_get_io_driver(netif_glue->ports_esp_netifs[i])) {
            int disc_cnt;
            for (disc_cnt = 0; disc_cnt < netif_glue->port_cnt; disc_cnt++) {
                if (esp_netif_is_netif_up(netif_glue->ports_esp_netifs[disc_cnt]) == true) {
                    break;
                }
            }
            // if all ports are disconnected, set bridge as disconnected too
            if (disc_cnt >= netif_glue->port_cnt) {
                esp_netif_action_disconnected(netif_glue->base.netif, base, event_id, event_data);
                ESP_LOGD(TAG, "bridge netif %p is disconnected", netif_glue->base.netif);
            }
        }
    }
}

static void br_action_got_ip(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *)event_data;
    esp_netif_br_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "br_action_got_ip: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->base.netif == ip_event->esp_netif) {
        esp_netif_action_got_ip(ip_event->esp_netif, base, event_id, event_data);
    }
}

static esp_err_t esp_netif_br_glue_clear_instance_handlers(esp_netif_br_glue_handle_t esp_netif_br_glue)
{
    ESP_RETURN_ON_FALSE(esp_netif_br_glue, ESP_ERR_INVALID_ARG, TAG, "esp_netif_br_glue handle can't be null");

    if (esp_netif_br_glue->eth_start_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_START, esp_netif_br_glue->eth_start_ctx_handler);
        esp_netif_br_glue->eth_start_ctx_handler = NULL;
    }

    if (esp_netif_br_glue->eth_stop_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_STOP, esp_netif_br_glue->eth_stop_ctx_handler);
        esp_netif_br_glue->eth_stop_ctx_handler = NULL;
    }

    if (esp_netif_br_glue->eth_connect_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, esp_netif_br_glue->eth_connect_ctx_handler);
        esp_netif_br_glue->eth_connect_ctx_handler = NULL;
    }

    if (esp_netif_br_glue->eth_disconnect_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, esp_netif_br_glue->eth_disconnect_ctx_handler);
        esp_netif_br_glue->eth_disconnect_ctx_handler = NULL;
    }

    if (esp_netif_br_glue->get_ip_ctx_handler) {
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, esp_netif_br_glue->get_ip_ctx_handler);
        esp_netif_br_glue->get_ip_ctx_handler = NULL;
    }

    return ESP_OK;
}

static esp_err_t esp_netif_br_glue_set_instance_handlers(esp_netif_br_glue_handle_t esp_netif_br_glue)
{
    ESP_RETURN_ON_FALSE(esp_netif_br_glue, ESP_ERR_INVALID_ARG, TAG, "esp_netif_br_glue handle can't be null");

    esp_err_t ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_START, eth_action_start, esp_netif_br_glue, &esp_netif_br_glue->eth_start_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_STOP, eth_action_stop, esp_netif_br_glue, &esp_netif_br_glue->eth_stop_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, eth_action_connected, esp_netif_br_glue, &esp_netif_br_glue->eth_connect_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, eth_action_disconnected, esp_netif_br_glue, &esp_netif_br_glue->eth_disconnect_ctx_handler);
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

esp_err_t esp_netif_br_glue_add_port(esp_netif_br_glue_handle_t netif_br_glue, esp_netif_t *esp_netif_port)
{
    if (netif_br_glue->ports_esp_netifs == NULL) {
        netif_br_glue->ports_esp_netifs = malloc(sizeof(esp_netif_t *));
    } else {
        netif_br_glue->ports_esp_netifs = realloc(netif_br_glue->ports_esp_netifs, (netif_br_glue->port_cnt + 1) * sizeof(esp_netif_t *));
    }
    if (!netif_br_glue->ports_esp_netifs) {
        ESP_LOGE(TAG, "no memory to add br port");
        return ESP_ERR_NO_MEM;
    }

    netif_br_glue->ports_esp_netifs[netif_br_glue->port_cnt] = esp_netif_port;
    netif_br_glue->port_cnt++;

    return ESP_OK;
}

esp_err_t esp_netif_br_glue_del(esp_netif_br_glue_handle_t netif_br_glue)
{
    esp_netif_br_glue_clear_instance_handlers(netif_br_glue);
    free(netif_br_glue->ports_esp_netifs);
    free(netif_br_glue);
    netif_br_glue = NULL;
    return ESP_OK;
}

#endif // CONFIG_ESP_NETIF_BRIDGE_EN
