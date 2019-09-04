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

typedef struct wifi_netif_driver {
    esp_netif_driver_base_t base;
    wifi_interface_t wifi_if;
}* wifi_netif_driver_t;

static const char* TAG = "wifi_init_default";

#define MAX_WIFI_IFS (2)

static esp_netif_t *s_wifi_netifs[MAX_WIFI_IFS] = { NULL };
static bool wifi_default_handlers_set = false;

static esp_err_t wifi_sta_receive(void *buffer, uint16_t len, void *eb)
{
    return esp_netif_receive(s_wifi_netifs[WIFI_IF_STA], buffer, len, eb);
}

static esp_err_t wifi_ap_receive(void *buffer, uint16_t len, void *eb)
{
    return esp_netif_receive(s_wifi_netifs[WIFI_IF_AP], buffer, len, eb);
}

void wifi_free(void *h, void* buffer)
{
    esp_wifi_internal_free_rx_buffer(buffer);
}

esp_err_t wifi_transmit(void *h, void *buffer, size_t len)
{
    wifi_netif_driver_t driver = h;
    return esp_wifi_internal_tx(driver->wifi_if, buffer, len);
}

void wifi_start(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    ESP_LOGD(TAG, "%s esp-netif:%p event-id%d", __func__, esp_netif, event_id);

    uint8_t mac[6];
    esp_err_t ret;

    wifi_netif_driver_t driver = esp_netif_get_io_driver(esp_netif);
    wifi_interface_t wifi_interface = driver->wifi_if;

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
    if (s_wifi_netifs[WIFI_IF_STA] != NULL) {
        wifi_start(s_wifi_netifs[WIFI_IF_STA], base, event_id, data);
    }
}

static void wifi_default_action_sta_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_wifi_netifs[WIFI_IF_STA] != NULL) {
        esp_netif_action_stop(s_wifi_netifs[WIFI_IF_STA], base, event_id, data);
    }
}

static void wifi_default_action_sta_connected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_wifi_netifs[WIFI_IF_STA] != NULL) {
        esp_err_t ret;
        // By default register wifi rxcb once the STA gets connected
        if ((ret = esp_wifi_internal_reg_rxcb(ESP_IF_WIFI_STA,  wifi_sta_receive)) != ESP_OK) {
            ESP_LOGE(TAG, "esp_wifi_internal_reg_rxcb for if=%d failed with %d", ESP_IF_WIFI_STA, ret);
            return;
        }

        esp_netif_action_connected(s_wifi_netifs[WIFI_IF_STA], base, event_id, data);
    }
}

static void wifi_default_action_sta_disconnected(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_wifi_netifs[WIFI_IF_STA] != NULL) {
        esp_netif_action_disconnected(s_wifi_netifs[WIFI_IF_STA], base, event_id, data);
    }
}

static void wifi_default_action_ap_start(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_wifi_netifs[WIFI_IF_AP] != NULL) {
        wifi_start(s_wifi_netifs[WIFI_IF_AP], base, event_id, data);
    }
}

static void wifi_default_action_ap_stop(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_wifi_netifs[WIFI_IF_AP] != NULL) {
        esp_netif_action_stop(s_wifi_netifs[WIFI_IF_AP], base, event_id, data);
    }
}

static void wifi_default_action_sta_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_wifi_netifs[WIFI_IF_STA] != NULL) {
        ESP_LOGD(TAG, "Got IP wifi default handler entered");
        int ret = esp_wifi_internal_set_sta_ip();
        if (ret != ESP_OK) {
            ESP_LOGI(TAG, "esp_wifi_internal_set_sta_ip failed with %d", ret);
        }
        esp_netif_action_got_ip(s_wifi_netifs[WIFI_IF_STA], base, event_id, data);
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

static esp_err_t wifi_driver_start(esp_netif_t * esp_netif, void * args)
{
    wifi_netif_driver_t driver = args;
    driver->base.netif = esp_netif;
    esp_netif_driver_ifconfig_t driver_ifconfig = {
            .handle =  driver,
            .transmit = wifi_transmit,
            .driver_free_rx_buffer = wifi_free
    };

    return esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
}

static esp_err_t disconnect_and_destroy(esp_netif_t* esp_netif)
{
    wifi_netif_driver_t driver = esp_netif_get_io_driver(esp_netif);
    driver->base.netif = NULL;
    esp_netif_driver_ifconfig_t driver_ifconfig = { };
    esp_err_t  ret = esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
    free(driver);
    return ret;
}

static esp_err_t create_and_attach(wifi_interface_t wifi_if, esp_netif_t* esp_netif)
{
    wifi_netif_driver_t driver = calloc(1, sizeof(struct wifi_netif_driver));
    if (driver == NULL) {
        ESP_LOGE(TAG, "No memory to create a wifi driver");
        return ESP_ERR_NO_MEM;
    }
    driver->wifi_if = wifi_if;
    driver->base.post_attach = wifi_driver_start;
    esp_netif_attach(esp_netif, driver);
    return ESP_OK;
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

esp_err_t esp_wifi_set_default_wifi_driver_and_handlers(wifi_interface_t wifi_if, void *esp_netif)
{
    assert(esp_netif);
    assert(wifi_if < MAX_WIFI_IFS);
    s_wifi_netifs[wifi_if] = esp_netif;
    ESP_ERROR_CHECK(create_and_attach(wifi_if, esp_netif));
    return _esp_wifi_set_default_wifi_handlers();
}

esp_err_t esp_wifi_clear_default_wifi_driver_and_handlers(void *esp_netif)
{
    int i;
    for (i = 0; i< MAX_WIFI_IFS; ++i) {
        // clear internal static pointers to netifs
        if (s_wifi_netifs[i] == esp_netif) {
            s_wifi_netifs[i] = NULL;
        }
        // check if all netifs are cleared to delete default handlers
        if (s_wifi_netifs[i] != NULL) {
            break;
        }
    }

    if (i == MAX_WIFI_IFS) { // if all wifi default netifs are null
        ESP_LOGD(TAG, "Clearing wifi default handlers");
        esp_wifi_clear_default_wifi_handlers();
    }
    return disconnect_and_destroy(esp_netif);
}
