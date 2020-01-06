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

const static char *TAG = "esp_eth.netif.glue";

typedef struct {
    esp_netif_driver_base_t base;
    esp_eth_handle_t eth_driver;
} esp_eth_netif_glue_t;

static esp_err_t eth_input_to_netif(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv)
{
    return esp_netif_receive((esp_netif_t *)priv, buffer, length, NULL);
}

static esp_err_t esp_eth_post_attach(esp_netif_t *esp_netif, void *args)
{
    uint8_t eth_mac[6];
    esp_eth_netif_glue_t *glue = (esp_eth_netif_glue_t *)args;
    glue->base.netif = esp_netif;

    esp_eth_update_input_path(glue->eth_driver, eth_input_to_netif, esp_netif);

    // set driver related config to esp-netif
    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle =  glue->eth_driver,
        .transmit = esp_eth_transmit,
        .driver_free_rx_buffer = NULL
    };

    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
    esp_eth_ioctl(glue->eth_driver, ETH_CMD_G_MAC_ADDR, eth_mac);
    ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x", eth_mac[0], eth_mac[1],
             eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);

    esp_netif_set_mac(esp_netif, eth_mac);
    ESP_LOGI(TAG, "ethernet attached to netif");

    return ESP_OK;
}

void *esp_eth_new_netif_glue(esp_eth_handle_t eth_hdl)
{
    esp_eth_netif_glue_t *glue = calloc(1, sizeof(esp_eth_netif_glue_t));
    if (!glue) {
        ESP_LOGE(TAG, "create netif glue failed");
        return NULL;
    }
    glue->eth_driver = eth_hdl;
    glue->base.post_attach = esp_eth_post_attach;
    esp_eth_increase_reference(eth_hdl);
    return &glue->base;
}

esp_err_t esp_eth_del_netif_glue(void *g)
{
    esp_eth_netif_glue_t *glue = (esp_eth_netif_glue_t *)g;
    esp_eth_decrease_reference(glue->eth_driver);
    free(glue);
    return ESP_OK;
}

esp_err_t esp_eth_clear_default_handlers(void *esp_netif)
{
    if (!esp_netif) {
        ESP_LOGE(TAG, "esp-netif handle can't be null");
        return ESP_ERR_INVALID_ARG;
    }
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_START, esp_netif_action_start);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_STOP, esp_netif_action_stop);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, esp_netif_action_connected);
    esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, esp_netif_action_disconnected);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, esp_netif_action_got_ip);

    return ESP_OK;
}

esp_err_t esp_eth_set_default_handlers(void *esp_netif)
{
    esp_err_t ret;

    if (!esp_netif) {
        ESP_LOGE(TAG, "esp-netif handle can't be null");
        return ESP_ERR_INVALID_ARG;
    }

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

    return ESP_OK;

fail:
    esp_eth_clear_default_handlers(esp_netif);
    return ret;
}
