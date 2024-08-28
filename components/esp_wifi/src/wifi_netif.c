/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_private/wifi.h"
#include "esp_wifi_netif.h"

//
//  Purpose of this module is provide object oriented abstraction to wifi interfaces
//  in order to integrate wifi as esp-netif driver
//

/**
 * @brief WiFi netif driver structure
 */
struct wifi_netif_driver {
    esp_netif_driver_base_t base;
    wifi_interface_t wifi_if;
};

static const char* TAG = "wifi_netif";

/**
 * @brief Local storage for netif handles and callbacks for specific wifi interfaces
 */
static esp_netif_receive_t s_wifi_rxcbs[MAX_WIFI_IFS] = { NULL };
static esp_netif_t *s_wifi_netifs[MAX_WIFI_IFS] = { NULL };

/**
 * @brief WiFi netif driver IO functions, a thin glue layer
 *         to the original wifi interface API
 */
static esp_err_t wifi_sta_receive(void *buffer, uint16_t len, void *eb)
{
    return s_wifi_rxcbs[WIFI_IF_STA](s_wifi_netifs[WIFI_IF_STA], buffer, len, eb);
}

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
static esp_err_t wifi_ap_receive(void *buffer, uint16_t len, void *eb)
{
    return s_wifi_rxcbs[WIFI_IF_AP](s_wifi_netifs[WIFI_IF_AP], buffer, len, eb);
}
#endif

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
static esp_err_t wifi_nan_receive(void *buffer, uint16_t len, void *eb)
{
    return s_wifi_rxcbs[WIFI_IF_NAN](s_wifi_netifs[WIFI_IF_NAN], buffer, len, eb);
}
#endif

static void wifi_free(void *h, void* buffer)
{
    if (buffer) {
        esp_wifi_internal_free_rx_buffer(buffer);
    }
}

static esp_err_t wifi_transmit(void *h, void *buffer, size_t len)
{
    wifi_netif_driver_t driver = h;
    return esp_wifi_internal_tx(driver->wifi_if, buffer, len);
}

static esp_err_t wifi_transmit_wrap(void *h, void *buffer, size_t len, void *netstack_buf)
{
    wifi_netif_driver_t driver = h;
#if CONFIG_SPIRAM_TRY_ALLOCATE_WIFI_LWIP && !CONFIG_SPIRAM_IGNORE_NOTFOUND
    return esp_wifi_internal_tx_by_ref(driver->wifi_if, buffer, len, netstack_buf);
#else
    return esp_wifi_internal_tx(driver->wifi_if, buffer, len);
#endif
}

static esp_err_t wifi_driver_start(esp_netif_t * esp_netif, void * args)
{
    wifi_netif_driver_t driver = args;
    driver->base.netif = esp_netif;
    esp_netif_driver_ifconfig_t driver_ifconfig = {
        .handle =  driver,
        .transmit = wifi_transmit,
        .transmit_wrap = wifi_transmit_wrap,
        .driver_free_rx_buffer = wifi_free
    };

    return esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
}

void esp_wifi_destroy_if_driver(wifi_netif_driver_t h)
{
    if (h) {
        esp_wifi_internal_reg_rxcb(h->wifi_if, NULL);  // ignore the potential error
        // as the wifi might have been already uninitialized
        s_wifi_netifs[h->wifi_if] = NULL;
    }
    free(h);
}

wifi_netif_driver_t esp_wifi_create_if_driver(wifi_interface_t wifi_if)
{
    wifi_netif_driver_t driver = calloc(1, sizeof(struct wifi_netif_driver));
    if (driver == NULL) {
        ESP_LOGE(TAG, "No memory to create a wifi interface handle");
        return NULL;
    }
    driver->wifi_if = wifi_if;
    driver->base.post_attach = wifi_driver_start;
    return driver;
}

esp_err_t esp_wifi_get_if_mac(wifi_netif_driver_t ifx, uint8_t mac[6])
{
    if (ifx == NULL || mac == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    wifi_interface_t wifi_interface = ifx->wifi_if;

    return esp_wifi_get_mac(wifi_interface, mac);
}

bool esp_wifi_is_if_ready_when_started(wifi_netif_driver_t ifx)
{
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    // WiFi rxcb to be register wifi rxcb on start for AP only, station gets it registered on connect event
    return (ifx && ifx->wifi_if == WIFI_IF_AP);
#else
    return false;
#endif
}

esp_err_t esp_wifi_register_if_rxcb(wifi_netif_driver_t ifx, esp_netif_receive_t fn, void * arg)
{
    if (ifx == NULL || fn == NULL || arg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (ifx->base.netif != arg) {
        ESP_LOGE(TAG, "Invalid argument: supplied netif=%p does not equal to interface netif=%p", arg, ifx->base.netif);
        return ESP_ERR_INVALID_ARG;
    }
    wifi_interface_t wifi_interface = ifx->wifi_if;
    s_wifi_rxcbs[wifi_interface] = fn;
    wifi_rxcb_t rxcb = NULL;
    esp_err_t ret;

    switch (wifi_interface) {

    case WIFI_IF_STA:
        rxcb = wifi_sta_receive;
        break;

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    case WIFI_IF_AP:
        rxcb = wifi_ap_receive;
        break;
#endif

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE
    case WIFI_IF_NAN:
        rxcb = wifi_nan_receive;
        break;
#endif

    default:
        break;
    }

    if (rxcb == NULL) {
        ESP_LOGE(TAG, "Unknown wifi interface id if=%d", wifi_interface);
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Interface must be set before registering Wi-Fi RX callback */
    s_wifi_netifs[wifi_interface] = ifx->base.netif;
    if ((ret = esp_wifi_internal_reg_rxcb(wifi_interface,  rxcb)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_internal_reg_rxcb for if=%d failed with %d", wifi_interface, ret);
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}
