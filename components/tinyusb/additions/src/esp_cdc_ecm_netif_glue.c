// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
// Modifications Copyright © 2021 Ci4Rail GmbH
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
#include "esp_cdc_ecm_driver.h"
#include "esp_cdc_ecm_netif_glue.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "descriptors_control.h"
#include "esp_log.h"

const static char *TAG = "CDC-ECM Glue";

typedef struct {
     esp_netif_driver_base_t base;
     esp_cdc_ecm_driver_t* driver_handle;
}esp_cdc_ecm_netif_glue_t;

static esp_err_t esp_eth_post_attach(esp_netif_t *esp_netif, void *args)
{
    uint8_t eth_mac_ext[6];
    uint8_t eth_mac_int[6];
    esp_cdc_ecm_netif_glue_t *glue = (esp_cdc_ecm_netif_glue_t *)args;
    glue->base.netif = esp_netif;
    glue->driver_handle->netif = esp_netif;

    // set driver related config to esp-netif
    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle =  glue->driver_handle,
        .transmit = esp_cdc_ecm_transmit,
        .driver_free_rx_buffer = esp_cdc_ecm_free_rx_buffer
    };

    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
    /* get pre-programmed ethernet MAC address */
    esp_read_mac(eth_mac_ext, ESP_MAC_ETH);
    ESP_LOGI(TAG, "%02x:%02x:%02x:%02x:%02x:%02x", eth_mac_ext[0], eth_mac_ext[1],
             eth_mac_ext[2], eth_mac_ext[3], eth_mac_ext[4], eth_mac_ext[5]);
    memcpy(eth_mac_int, eth_mac_ext, sizeof(eth_mac_ext));
    eth_mac_int[5] ^= 0x01;
    /* set host MAC address */
    tusb_set_mac_address(eth_mac_ext);
    /* set device MAC address */
    esp_netif_set_mac(esp_netif, eth_mac_int);
    ESP_LOGI(TAG, "ethernet attached to netif");

    esp_cdc_ecm_driver_start(glue->driver_handle, esp_netif);

    return ESP_OK;
}

void *esp_cdc_ecm_new_netif_glue(esp_cdc_ecm_driver_t* driver_handle)
{
    esp_cdc_ecm_netif_glue_t *glue = calloc(1, sizeof(esp_cdc_ecm_netif_glue_t));
    if (!glue) {
        ESP_LOGE(TAG, "create netif glue failed");
        return NULL;
    }
    glue->driver_handle = driver_handle;
    glue->base.post_attach = esp_eth_post_attach;
    return &glue->base;
}

esp_err_t esp_cdc_ecm_set_default_handlers(void *esp_netif)
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

