/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "esp_wifi_netif.h"
#include <string.h>
#include <inttypes.h>
#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
#include "apps_private/wifi_apps_private.h"
#endif

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

    ESP_LOGD(TAG, "%s esp-netif:%p event-id%" PRId32 "", __func__, esp_netif, event_id);

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

    if ((ret = esp_wifi_internal_reg_netstack_buf_cb(esp_netif_netstack_buf_ref, esp_netif_netstack_buf_free)) != ESP_OK) {
        ESP_LOGE(TAG, "netstack cb reg failed with %d", ret);
        return;
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

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
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
#endif

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

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
static void wifi_default_action_nan_started(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_wifi_netifs[WIFI_IF_NAN] != NULL) {
        wifi_start(s_wifi_netifs[WIFI_IF_NAN], base, event_id, data);
        esp_nan_action_start(s_wifi_netifs[WIFI_IF_NAN]);
    }
}

static void wifi_default_action_nan_stopped(void *arg, esp_event_base_t base, int32_t event_id, void *data)
{
    if (s_wifi_netifs[WIFI_IF_NAN] != NULL) {
        esp_netif_action_stop(s_wifi_netifs[WIFI_IF_NAN], base, event_id, data);
        esp_nan_action_stop();
    }
}
#endif

/**
 * @brief Clear default handlers
 */
static esp_err_t clear_default_wifi_handlers(void)
{
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_START, wifi_default_action_sta_start);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_STOP, wifi_default_action_sta_stop);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, wifi_default_action_sta_connected);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, wifi_default_action_sta_disconnected);
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_START, wifi_default_action_ap_start);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_AP_STOP, wifi_default_action_ap_stop);
#endif
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_default_action_sta_got_ip);
#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_NAN_STARTED, wifi_default_action_nan_started);
    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_NAN_STOPPED, wifi_default_action_nan_stopped);
#endif
    esp_unregister_shutdown_handler((shutdown_handler_t)esp_wifi_stop);
    wifi_default_handlers_set = false;
    return ESP_OK;
}

/**
 * @brief Set default handlers
 */
static esp_err_t set_default_wifi_handlers(void)
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

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_START, wifi_default_action_ap_start, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STOP, wifi_default_action_ap_stop, NULL);
    if (err != ESP_OK) {
        goto fail;
    }
#endif

    err = esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_default_action_sta_got_ip, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_NAN_STARTED, wifi_default_action_nan_started, NULL);
    if (err != ESP_OK) {
        goto fail;
    }

    err = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_NAN_STOPPED, wifi_default_action_nan_stopped, NULL);
    if (err != ESP_OK) {
        goto fail;
    }
#endif

    err = esp_register_shutdown_handler((shutdown_handler_t)esp_wifi_stop);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        goto fail;
    }
    wifi_default_handlers_set = true;
    return ESP_OK;

fail:
    clear_default_wifi_handlers();
    return err;
}

/**
 * @brief Set default handlers for station (official API)
 */
esp_err_t esp_wifi_set_default_wifi_sta_handlers(void)
{
    return set_default_wifi_handlers();
}

/**
 * @brief Set default handlers for AP (official API)
 */
esp_err_t esp_wifi_set_default_wifi_ap_handlers(void)
{
    return set_default_wifi_handlers();
}

/**
 * @brief Set default handlers for NAN (official API)
 */
esp_err_t esp_wifi_set_default_wifi_nan_handlers(void)
{
    return set_default_wifi_handlers();
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
    }
    for (i = 0; i< MAX_WIFI_IFS; ++i) {
        // check if all netifs are cleared to delete default handlers
        if (s_wifi_netifs[i] != NULL) {
            break;
        }
    }

    if (i == MAX_WIFI_IFS) { // if all wifi default netifs are null
        ESP_LOGD(TAG, "Clearing wifi default handlers");
        clear_default_wifi_handlers();
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

static inline esp_err_t esp_netif_attach_wifi(esp_netif_t *esp_netif, wifi_interface_t wifi_if)
{
    if (esp_netif == NULL || (wifi_if != WIFI_IF_STA
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    && wifi_if != WIFI_IF_AP
#endif
#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
    && wifi_if != WIFI_IF_NAN
#endif
    )) {
        return ESP_ERR_INVALID_ARG;
    }
    s_wifi_netifs[wifi_if] = esp_netif;
    return create_and_attach(wifi_if, esp_netif);
}

esp_err_t esp_netif_attach_wifi_station(esp_netif_t *esp_netif)
{
    return esp_netif_attach_wifi(esp_netif, WIFI_IF_STA);
}

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
esp_err_t esp_netif_attach_wifi_ap(esp_netif_t *esp_netif)
{
    return esp_netif_attach_wifi(esp_netif, WIFI_IF_AP);
}
#endif

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
esp_err_t esp_netif_attach_wifi_nan(esp_netif_t *esp_netif)
{
    return esp_netif_attach_wifi(esp_netif, WIFI_IF_NAN);
}
#endif


//
// Default WiFi creation from user code
//

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
/**
 * @brief User init default AP (official API)
 */
esp_netif_t* esp_netif_create_default_wifi_ap(void)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_AP();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);
    ESP_ERROR_CHECK(esp_netif_attach_wifi_ap(netif));
    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_ap_handlers());
    return netif;
}
#endif

/**
 * @brief User init default station (official API)
 */
esp_netif_t* esp_netif_create_default_wifi_sta(void)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_STA();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);
    ESP_ERROR_CHECK(esp_netif_attach_wifi_station(netif));
    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_sta_handlers());
    return netif;
}

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
/**
 * @brief User init default NAN (official API)
 */
esp_netif_t* esp_netif_create_default_wifi_nan(void)
{
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_NAN();
    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);
    esp_netif_attach_wifi_nan(netif);
    esp_wifi_set_default_wifi_nan_handlers();
    return netif;
}
#endif

/**
 * @brief User init default wifi esp_netif object (official API)
 */
void esp_netif_destroy_default_wifi(void *esp_netif)
{
    if (esp_netif) {
        esp_wifi_clear_default_wifi_driver_and_handlers(esp_netif);
    }
    esp_netif_destroy(esp_netif);
}

/**
 * @brief User init custom wifi interface
 */
esp_netif_t* esp_netif_create_wifi(wifi_interface_t wifi_if, const esp_netif_inherent_config_t *esp_netif_config)
{
    esp_netif_config_t cfg = {
        .base = esp_netif_config
    };
    if (wifi_if == WIFI_IF_STA) {
        cfg.stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_STA;
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    } else if (wifi_if == WIFI_IF_AP) {
        cfg.stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_AP;
#endif
#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
    } else if (wifi_if == WIFI_IF_NAN) {
        cfg.stack = ESP_NETIF_NETSTACK_DEFAULT_WIFI_NAN;
#endif
    } else {
        return NULL;
    }

    esp_netif_t *netif = esp_netif_new(&cfg);
    assert(netif);
    esp_netif_attach_wifi(netif, wifi_if);
    return netif;
}

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
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

    // ...and stop DHCP server to keep the ESP_NETIF_DHCP_STOPPED state
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
#endif // CONFIG_ESP_WIFI_SOFTAP_SUPPORT
