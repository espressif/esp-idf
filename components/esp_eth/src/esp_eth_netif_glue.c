/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <inttypes.h>
#include "esp_netif.h"
#include "esp_eth_driver.h"
#include "esp_eth_netif_glue.h"
#include "esp_netif_net_stack.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"
#if CONFIG_ESP_NETIF_L2_TAP
#include "esp_vfs_l2tap.h"
#endif

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

static esp_err_t eth_input_to_netif(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv)
{
#if CONFIG_ESP_NETIF_L2_TAP
    esp_err_t ret = ESP_OK;
    ret = esp_vfs_l2tap_eth_filter(eth_handle, buffer, (size_t *)&length);
    if (length == 0) {
        return ret;
    }
#endif
    return esp_netif_receive((esp_netif_t *)priv, buffer, length, NULL);
}

static void eth_l2_free(void *h, void* buffer)
{
    free(buffer);
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
        .driver_free_rx_buffer = eth_l2_free
    };

    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
    esp_eth_ioctl(netif_glue->eth_driver, ETH_CMD_G_MAC_ADDR, eth_mac);
    ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x", eth_mac[0], eth_mac[1],
             eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);

    esp_netif_set_mac(esp_netif, eth_mac);
    ESP_LOGI(TAG, "ethernet attached to netif");

    return ESP_OK;
}

static void eth_action_start(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_start: %p, %p, %" PRIi32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->eth_driver == eth_handle) {
        esp_netif_action_start(netif_glue->base.netif, base, event_id, event_data);
    }
}

static void eth_action_stop(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_stop: %p, %p, %" PRIi32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->eth_driver == eth_handle) {
        esp_netif_action_stop(netif_glue->base.netif, base, event_id, event_data);
    }
}

static void eth_action_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_connected: %p, %p, %" PRIi32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->eth_driver == eth_handle) {
        eth_speed_t speed;
        esp_eth_ioctl(eth_handle, ETH_CMD_G_SPEED, &speed);
        esp_netif_set_link_speed(netif_glue->base.netif, speed == ETH_SPEED_100M ? 100000000 : 10000000);
        esp_netif_action_connected(netif_glue->base.netif, base, event_id, event_data);
    }
}

static void eth_action_disconnected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_disconnected: %p, %p, %" PRIi32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
    if (netif_glue->eth_driver == eth_handle) {
        esp_netif_action_disconnected(netif_glue->base.netif, base, event_id, event_data);
    }
}

static void eth_action_got_ip(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *ip_event = (ip_event_got_ip_t *)event_data;
    esp_eth_netif_glue_t *netif_glue = handler_args;
    ESP_LOGD(TAG, "eth_action_got_ip: %p, %p, %" PRIi32 ", %p, %p", netif_glue, base, event_id, event_data, *(esp_eth_handle_t *)event_data);
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

    if (esp_eth_set_glue_instance_handlers(netif_glue) != ESP_OK) {
        esp_eth_del_netif_glue(netif_glue);
        return NULL;
    }

    return netif_glue;
}
