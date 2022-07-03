/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "netif/wlanif.h"
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
typedef struct wifi_netif_driver {
    esp_netif_driver_base_t base;
    wifi_interface_t wifi_if;
}* wifi_netif_driver_t;

static const char* TAG = "wifi_netif";

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
#if CONFIG_SPIRAM
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
            .transmit_wrap= wifi_transmit_wrap,
            .driver_free_rx_buffer = wifi_free
    };

    return esp_netif_set_driver_config(esp_netif, &driver_ifconfig);
}

void esp_wifi_destroy_if_driver(wifi_netif_driver_t h)
{
    if (h) {
        esp_wifi_internal_reg_rxcb(h->wifi_if, NULL);  // ignore the potential error
                                                       // as the wifi might have been already uninitialized
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
    wifi_interface_t wifi_interface = ifx->wifi_if;

    return esp_wifi_get_mac(wifi_interface, mac);
}

bool esp_wifi_is_if_ready_when_started(wifi_netif_driver_t ifx)
{
#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    // WiFi rxcb to be register wifi rxcb on start for AP only, station gets it registered on connect event
    return (ifx->wifi_if == WIFI_IF_AP);
#else
    return false;
#endif
}

esp_err_t esp_wifi_register_if_rxcb(wifi_netif_driver_t ifx, esp_netif_receive_t fn, void * arg)
{
    if (ifx->base.netif != arg) {
        ESP_LOGE(TAG, "Invalid argument: supplied netif=%p does not equal to interface netif=%p", arg, ifx->base.netif);
        return ESP_ERR_INVALID_ARG;
    }
    wifi_interface_t wifi_interface = ifx->wifi_if;
    wifi_rxcb_t rxcb = NULL;
    esp_err_t ret;

    switch (wifi_interface)
    {

    case WIFI_IF_STA:
        rxcb = wifi_rxcb_sta;
        break;

#ifdef CONFIG_ESP_WIFI_SOFTAP_SUPPORT
    case WIFI_IF_AP:
        rxcb = wifi_rxcb_ap;
        break;
#endif

    default:
        break;
    }

    if (rxcb == NULL) {
        ESP_LOGE(TAG, "Unknown wifi interface id if=%d", wifi_interface);
        return ESP_ERR_NOT_SUPPORTED;
    }

    if ((ret = esp_wifi_internal_reg_rxcb(wifi_interface,  rxcb)) != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_internal_reg_rxcb for if=%d failed with %d", wifi_interface, ret);
        return ESP_ERR_INVALID_STATE;
    }
    set_wifi_netif(wifi_interface, esp_netif_get_netif_impl(arg));
    return ESP_OK;
}
