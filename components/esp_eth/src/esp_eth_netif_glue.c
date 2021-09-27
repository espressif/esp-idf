// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdlib.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_eth_netif_glue.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"

const static char *TAG = "esp_eth.netif.netif_glue";

typedef struct esp_eth_netif_glue_t esp_eth_netif_glue_t;

struct esp_eth_netif_glue_t {
    esp_netif_driver_base_t base;
    esp_eth_handle_t eth_driver;
    esp_event_handler_instance_t start_ctx_handler;
    esp_event_handler_instance_t stop_ctx_handler;
    esp_event_handler_instance_t connect_ctx_handler;
    esp_event_handler_instance_t disconnect_ctx_handler;
    esp_event_handler_instance_t get_ip_ctx_handler;
};

bool s_netif_glue_legacy_events_registered = false;

static esp_err_t eth_input_to_netif(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv)
{
    return esp_netif_receive((esp_netif_t *)priv, buffer, length, NULL);
}

static esp_err_t esp_eth_post_attach(esp_netif_t *esp_netif, void *args)
{
    uint8_t eth_mac[6];
    esp_eth_netif_glue_t *netif_glue = (esp_eth_netif_glue_t *)args;
    netif_glue->base.netif = esp_netif;

    esp_eth_update_input_path(netif_glue->eth_driver, eth_input_to_netif, esp_netif);

    // set driver related config to esp-netif
    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle =  netif_glue->eth_driver,
        .transmit = esp_eth_transmit,
        .driver_free_rx_buffer = NULL
    };

    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
    esp_eth_ioctl(netif_glue->eth_driver, ETH_CMD_G_MAC_ADDR, eth_mac);
    ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x", eth_mac[0], eth_mac[1],
             eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);

    esp_netif_set_mac(esp_netif, eth_mac);
    ESP_LOGI(TAG, "ethernet attached to netif");

    return ESP_OK;
}

esp_err_t esp_eth_clear_default_handlers(void *esp_netif)
{
    ESP_RETURN_ON_FALSE(esp_netif, ESP_ERR_INVALID_ARG, TAG, "esp_netif handle can't be null");

    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_START, esp_netif_action_start);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_STOP, esp_netif_action_stop);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, esp_netif_action_connected);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, esp_netif_action_disconnected);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, esp_netif_action_got_ip);

    s_netif_glue_legacy_events_registered = false;

    return ESP_OK;
}

esp_err_t esp_eth_set_default_handlers(void *esp_netif)
{
    esp_err_t ret;

    ESP_RETURN_ON_FALSE(esp_netif, ESP_ERR_INVALID_ARG, TAG, "esp_netif handle can't be null");

    ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_START, esp_netif_action_start, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_STOP, esp_netif_action_stop, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, esp_netif_action_connected, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, esp_netif_action_disconnected, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, esp_netif_action_got_ip, esp_netif);
    if (ret != ESP_OK) {
        goto fail;
    }

    s_netif_glue_legacy_events_registered = true;

    return ESP_OK;

fail:
    esp_eth_clear_default_handlers(esp_netif);
    return ret;
}

static void eth_action_start(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_start: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->eth_driver == eth_handle) {
        esp_netif_action_start(netif_glue->base.netif, base, event_id, event_data);
    }
}

static void eth_action_stop(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_stop: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->eth_driver == eth_handle) {
        esp_netif_action_stop(netif_glue->base.netif, base, event_id, event_data);
    }
}

static void eth_action_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_connected: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->eth_driver == eth_handle) {
        esp_netif_action_connected(netif_glue->base.netif, base, event_id, event_data);
    }
}

static void eth_action_disconnected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_disconnected: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->eth_driver == eth_handle) {
        esp_netif_action_disconnected(netif_glue->base.netif, base, event_id, event_data);
    }
}

static void eth_action_got_ip(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_got_ip: %p, %p, %d, %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->base.netif == ip_event->esp_netif) {
        esp_netif_action_got_ip(ip_event->esp_netif, base, event_id, event_data);
    }
}

static esp_err_t esp_eth_clear_glue_instance_handlers(esp_eth_netif_glue_handle_t eth_netif_glue)
{
    ESP_RETURN_ON_FALSE(eth_netif_glue, ESP_ERR_INVALID_ARG, TAG, "eth_netif_glue handle can't be null");

    if (eth_netif_glue->start_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_START, eth_netif_glue->start_ctx_handler);
        eth_netif_glue->start_ctx_handler = NULL;
    }

    if (eth_netif_glue->stop_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_STOP, eth_netif_glue->stop_ctx_handler);
        eth_netif_glue->stop_ctx_handler = NULL;
    }

    if (eth_netif_glue->connect_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, eth_netif_glue->connect_ctx_handler);
        eth_netif_glue->connect_ctx_handler = NULL;
    }

    if (eth_netif_glue->disconnect_ctx_handler) {
        esp_event_handler_instance_unregister(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, eth_netif_glue->disconnect_ctx_handler);
        eth_netif_glue->disconnect_ctx_handler = NULL;
    }

    if (eth_netif_glue->get_ip_ctx_handler) {
        esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, eth_netif_glue->get_ip_ctx_handler);
        eth_netif_glue->get_ip_ctx_handler = NULL;
    }

    return ESP_OK;
}

static esp_err_t esp_eth_set_glue_instance_handlers(esp_eth_netif_glue_handle_t eth_netif_glue)
{
    ESP_RETURN_ON_FALSE(eth_netif_glue, ESP_ERR_INVALID_ARG, TAG, "eth_netif_glue handle can't be null");

    esp_err_t ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_START, eth_action_start, eth_netif_glue, &eth_netif_glue->start_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_STOP, eth_action_stop, eth_netif_glue, &eth_netif_glue->stop_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, eth_action_connected, eth_netif_glue, &eth_netif_glue->connect_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, eth_action_disconnected, eth_netif_glue, &eth_netif_glue->disconnect_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    ret = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, eth_action_got_ip, eth_netif_glue, &eth_netif_glue->get_ip_ctx_handler);
    if (ret != ESP_OK) {
        goto fail;
    }

    return ESP_OK;

fail:
    esp_eth_clear_glue_instance_handlers(eth_netif_glue);
    return ret;
}

esp_err_t esp_eth_del_netif_glue(esp_eth_netif_glue_handle_t eth_netif_glue)
{
    esp_eth_clear_glue_instance_handlers(eth_netif_glue);

    esp_eth_decrease_reference(eth_netif_glue->eth_driver);
    free(eth_netif_glue);
    return ESP_OK;
}

esp_eth_netif_glue_handle_t esp_eth_new_netif_glue(esp_eth_handle_t eth_hdl)
{
    esp_eth_netif_glue_t *netif_glue = calloc(1, sizeof(esp_eth_netif_glue_t));
    if (!netif_glue) {
        ESP_LOGE(TAG, "create netif glue failed");
        return NULL;
    }
    netif_glue->eth_driver = eth_hdl;
    netif_glue->base.post_attach = esp_eth_post_attach;
    esp_eth_increase_reference(eth_hdl);

    if (s_netif_glue_legacy_events_registered == false) {
        if (esp_eth_set_glue_instance_handlers(netif_glue) != ESP_OK) {
            esp_eth_del_netif_glue(netif_glue);
            return NULL;
        }
    }
    return netif_glue;
}
