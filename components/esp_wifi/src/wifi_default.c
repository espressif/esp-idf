// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_private/wifi.h"

//
//  Purpose of this module is to provide basic wifi initialization setup for
//  station and AP and their conversion to esp-netif objects
//  Also this module holds esp-netif handles for AP and STA
//

static const char* TAG = "wifi_init_default";

static esp_netif_t *sta_netif = NULL;
static esp_netif_t *ap_netif = NULL;
static bool wifi_default_handlers_set = false;


static esp_err_t wifi_sta_receive(void *buffer, uint16_t len, void *eb)
{
    return esp_netif_receive(sta_netif, buffer, len, eb);
}

static esp_err_t wifi_ap_receive(void *buffer, uint16_t len, void *eb)
{
    return esp_netif_receive(ap_netif, buffer, len, eb);
}


void wifi_free(void *h, void* buffer)
{
    esp_wifi_internal_free_rx_buffer(buffer);
}


esp_err_t wifi_transmit(void *h, void *buffer, size_t len)
{
    return esp_wifi_internal_tx((wifi_interface_t)h, buffer, len);
}

void wifi_start(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "%s esp-netif:%p event-id%d", __func__, esp_netif, event_id);

    uint8_t mac[6];
    esp_err_t ret;

    wifi_interface_t wifi_interface = (wifi_interface_t) esp_netif_get_io_driver(esp_netif);

    if ((ret = esp_wifi_get_mac(wifi_interface, mac)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_get_mac failed with %d", ret);
        return;
    }
    ESP_LOGD(TAG, "WIFI mac address: %x %x %x %x %x %x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (wifi_interface == ESP_IF_WIFI_AP) {
        // By default register wifi rxcb on start for AP only, station gets it registered on connect event
        if ((ret = esp_wifi_internal_reg_rxcb(wifi_interface,  wifi_ap_receive)) != ESP_OK) {
            ESP_LOGE(TAG, "esp_wifi_internal_reg_rxcb for if=%d failed with %d", wifi_interface, ret);
            return;
        }
    }

    esp_netif_set_mac(esp_netif, mac);
    esp_netif_action_start(esp_netif, base, event_id, data);
}


static void wifi_default_action_sta_start(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (sta_netif != NULL) {
        wifi_start(sta_netif, base, event_id, data);
    }
}

static void wifi_default_action_sta_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (sta_netif != NULL) {
        esp_netif_action_stop(sta_netif, base, event_id, data);
    }
}

static void wifi_default_action_sta_connected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (sta_netif != NULL) {
        esp_err_t ret;
        // By default register wifi rxcb once the STA gets connected
        if ((ret = esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA,  wifi_sta_receive)) != ESP_OK) {
            ESP_LOGE(TAG, "esp_wifi_internal_reg_rxcb for if=%d failed with %d", ESP_IF_WIFI_STA, ret);
            return;
        }

        esp_netif_action_connected(sta_netif, base, event_id, data);
    }
}

static void wifi_default_action_sta_disconnected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (sta_netif != NULL) {
        esp_netif_action_disconnected(sta_netif, base, event_id, data);
    }
}

static void wifi_default_action_ap_start(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (ap_netif != NULL) {
        wifi_start(ap_netif, base, event_id, data);
    }
}

static void wifi_default_action_ap_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (ap_netif != NULL) {
        esp_netif_action_stop(ap_netif, base, event_id, data);
    }
}

static void wifi_default_action_sta_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (sta_netif != NULL) {
        ESP_LOGD(TAG, "Got IP wifi default handler entered");
        int ret = esp_wifi_internal_set_sta_ip();
        if (ret != ESP_OK) {
            ESP_LOGI(TAG, "esp_wifi_internal_set_sta_ip failed with %d", ret);
        }
        esp_netif_action_got_ip(sta_netif, base, event_id, data);
    }
}

esp_err_t esp_wifi_clear_default_wifi_handlers(void)
{
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_START, wifi_default_action_sta_start);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_STOP, wifi_default_action_sta_stop);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, wifi_default_action_sta_connected);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, wifi_default_action_sta_disconnected);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_START, wifi_default_action_ap_start);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STOP, wifi_default_action_ap_stop);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_default_action_sta_got_ip);
    esp_unregister_shutdown_handler((shutdown_handler_t)esp_wifi_stop);
    wifi_default_handlers_set = false;
    return ESP_OK;
}

void _esp_wifi_set_default_ap_netif(esp_netif_t* esp_netif)
{
    assert(esp_netif);
    ap_netif = esp_netif;
}

void _esp_wifi_set_default_sta_netif(esp_netif_t* esp_netif)
{
    assert(esp_netif);
    sta_netif = esp_netif;
}

esp_err_t _esp_wifi_set_default_wifi_handlers(void)
{
    if (wifi_default_handlers_set) {
        return ESP_OK;
    }
    esp_err_t err;
    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, wifi_default_action_sta_start, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_STOP, wifi_default_action_sta_stop, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, wifi_default_action_sta_connected, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, wifi_default_action_sta_disconnected, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, wifi_default_action_ap_start, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STOP, wifi_default_action_ap_stop, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_default_action_sta_got_ip, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_register_shutdown_handler((shutdown_handler_t)esp_wifi_stop);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        goto fail;
    }
    wifi_default_handlers_set = true;
    return ESP_OK;

fail:
    esp_wifi_clear_default_wifi_handlers();
    return err;
}

esp_err_t esp_wifi_set_default_wifi_sta_handlers(void *esp_netif)
{
    _esp_wifi_set_default_sta_netif(esp_netif);
    return _esp_wifi_set_default_wifi_handlers();
}

esp_err_t esp_wifi_set_default_wifi_ap_handlers(void *esp_netif)
{
    _esp_wifi_set_default_ap_netif(esp_netif);
    return _esp_wifi_set_default_wifi_handlers();
}

const esp_netif_driver_ifconfig_t _g_wifi_driver_sta_ifconfig = {
        .handle =  (void*)WIFI_IF_STA,
        .transmit = wifi_transmit,
        .driver_free_rx_buffer = wifi_free,
};

const esp_netif_driver_ifconfig_t _g_wifi_driver_ap_ifconfig = {
        .handle =  (void*)WIFI_IF_AP,
        .transmit = wifi_transmit,
        .driver_free_rx_buffer = wifi_free,
};
