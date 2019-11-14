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
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "esp_wifi_netif.h"
#include <string.h>

//
//  Purpose of this module is to provide basic wifi initialization setup for
//  default station and AP and to register default handles for these interfaces
//
static const char* TAG = "wifi_init_default";

static esp_netif_t *s_wifi_netifs[MAX_WIFI_IFS] = { NULL };
static bool wifi_default_handlers_set = false;

static esp_err_t disconnect_and_destroy(esp_netif_t* esp_netif);

//
// Default event handlers
//

/**
 * @brief Wifi start action when station or AP get started
 */
static void wifi_start(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data)
{
    uint8_t mac[6];
    esp_err_t ret;

    ESP_LOGD(TAG, "%s esp-netif:%p event-id%d", __func__, esp_netif, event_id);

    wifi_netif_driver_t driver = esp_netif_get_io_driver(esp_netif);

    if ((ret = esp_wifi_get_if_mac(driver, mac)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_get_mac failed with %d", ret);
        return;
    }
    ESP_LOGD(TAG, "WIFI mac address: %x %x %x %x %x %x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (esp_wifi_is_if_ready_when_started(driver)) {
        if ((ret = esp_wifi_register_if_rxcb(driver,  esp_netif_receive, esp_netif)) != ESP_OK) {
            ESP_LOGE(TAG, "esp_wifi_register_if_rxcb for if=%p failed with %d", driver, ret);
            return;
        }
    }

    esp_netif_set_mac(esp_netif, mac);
    esp_netif_action_start(esp_netif, base, event_id, data);
}

/**
 * @brief Wifi default handlers for specific events for station and APs
 */

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
        esp_netif_t *esp_netif = s_wifi_netifs[WIFI_IF_STA];
        wifi_netif_driver_t driver = esp_netif_get_io_driver(esp_netif);

        if (!esp_wifi_is_if_ready_when_started(driver)) {
            // if interface not ready when started, rxcb to be registered on connection
            if ((ret = esp_wifi_register_if_rxcb(driver,  esp_netif_receive, esp_netif)) != ESP_OK) {
                ESP_LOGE(TAG, "esp_wifi_register_if_rxcb for if=%p failed with %d", driver, ret);
                return;
            }
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

/**
 * @brief Clear default handlers
 */
esp_err_t _esp_wifi_clear_default_wifi_handlers(void)
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

/**
 * @brief Set default handlers
 */
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
    _esp_wifi_clear_default_wifi_handlers();
    return err;
}

/**
 * @brief Set default handlers for station (official API)
 */
esp_err_t esp_wifi_set_default_wifi_sta_handlers(void)
{
    return _esp_wifi_set_default_wifi_handlers();
}

/**
 * @brief Set default handlers for AP (official API)
 */
esp_err_t esp_wifi_set_default_wifi_ap_handlers(void)
{
    return _esp_wifi_set_default_wifi_handlers();
}

/**
 * @brief Clear default handlers and destroy appropriate objects (official API)
 */
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
        _esp_wifi_clear_default_wifi_handlers();
    }
    return disconnect_and_destroy(esp_netif);
}


//
// Object manipulation
//

/**
 * @brief Create and destroy objects
 */
static esp_err_t disconnect_and_destroy(esp_netif_t* esp_netif)
{
    wifi_netif_driver_t driver = esp_netif_get_io_driver(esp_netif);
    esp_netif_driver_ifconfig_t driver_ifconfig = { };
    esp_err_t  ret = esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
    esp_wifi_destroy_if_driver(driver);
    return ret;
}

static esp_err_t create_and_attach(wifi_interface_t wifi_if, esp_netif_t* esp_netif)
{
    wifi_netif_driver_t driver = esp_wifi_create_if_driver(wifi_if);
    if (driver == NULL) {
        ESP_LOGE(TAG, "Failed to create wifi interface handle");
        return ESP_FAIL;
    }
    return esp_netif_attach(esp_netif, driver);
}

esp_err_t esp_netif_attach_wifi_station(esp_netif_t *esp_netif)
{
    if (esp_netif == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    s_wifi_netifs[WIFI_IF_STA] = esp_netif;
    return create_and_attach(WIFI_IF_STA, esp_netif);
}

esp_err_t esp_netif_attach_wifi_ap(esp_netif_t *esp_netif)
{
    if (esp_netif == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    s_wifi_netifs[WIFI_IF_AP] = esp_netif;
    return create_and_attach(WIFI_IF_AP, esp_netif);
}


//
// Default WiFi creation from user code
//

/**
 * @brief User init default AP (official API)
 */
esp_netif_t* esp_netif_create_default_wifi_ap(void)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_AP();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);
    esp_netif_attach_wifi_ap(netif);
    esp_wifi_set_default_wifi_ap_handlers();
    return netif;
}

/**
 * @brief User init default station (official API)
 */
esp_netif_t* esp_netif_create_default_wifi_sta(void)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);
    esp_netif_attach_wifi_station(netif);
    esp_wifi_set_default_wifi_sta_handlers();
    return netif;
}

/**
 * @brief Creates mesh network interfaces based on default STA and AP,
 * but without DHCP, this is to be enabled separately only on root node
 */
esp_err_t esp_netif_create_default_wifi_mesh_netifs(esp_netif_t **p_netif_sta, esp_netif_t **p_netif_ap)
{
    // Create "almost" default AP, with un-flagged DHCP server
    esp_netif_inherent_config_t netif_cfg;
    memcpy(&netif_cfg, ESP_NETIF_BASE_DEFAULT_WIFI_AP, sizeof(netif_cfg));
    netif_cfg.flags &= ~ESP_NETIF_DHCP_SERVER;
    esp_netif_config_t cfg_ap = {
            .base = &netif_cfg,
            .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP,
    };
    esp_netif_t *netif_ap = esp_netif_new(&cfg_ap);
    assert(netif_ap);
    ESP_ERROR_CHECK(esp_netif_attach_wifi_ap(netif_ap));
    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_ap_handlers());

    // ...and stop DHCP server to be compatible with former tcpip_adapter (to keep the ESP_NETIF_DHCP_STOPPED state)
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif_ap));

    // Create "almost" default station, but with un-flagged DHCP client
    memcpy(&netif_cfg, ESP_NETIF_BASE_DEFAULT_WIFI_STA, sizeof(netif_cfg));
    netif_cfg.flags &= ~ESP_NETIF_DHCP_CLIENT;
    esp_netif_config_t cfg_sta = {
            .base = &netif_cfg,
            .stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA,
    };
    esp_netif_t *netif_sta = esp_netif_new(&cfg_sta);
    assert(netif_sta);
    ESP_ERROR_CHECK(esp_netif_attach_wifi_station(netif_sta));
    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_sta_handlers());

    // ...and stop DHCP client (to be started separately if the station were promoted to root)
    ESP_ERROR_CHECK(esp_netif_dhcpc_stop(netif_sta));

    if (p_netif_sta) {
        *p_netif_sta = netif_sta;
    }
    if (p_netif_ap) {
        *p_netif_ap = netif_ap;
    }
    return ESP_OK;
}