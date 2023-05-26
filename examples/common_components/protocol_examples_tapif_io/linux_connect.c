/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_err.h"
#include "esp_netif.h"      // esp-netif
#include "tapio.h"          // esp-netif's driver side
#include "lwip/tapif.h"     // esp-netif's network stack side
#include "esp_log.h"

#if CONFIG_EXAMPLE_CONNECT_WAIT_FOR_IP
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

static const char *TAG = "linux_connect";

static EventGroupHandle_t s_events;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    xEventGroupSetBits(s_events, 1);
}

#endif // CONFIG_EXAMPLE_CONNECT_WAIT_FOR_IP

#define TAP0_GOT_IP (0x1234)

esp_err_t example_connect(void)
{
#if CONFIG_EXAMPLE_CONNECT_LWIP_TAPIF
    // configure linux tapio
    esp_netif_driver_ifconfig_t driver_cfg = {
            .handle = tapio_create(),
            .transmit = tapio_output,
    };
    // configure lwip netif for the tapif
    struct esp_netif_netstack_config stack_cfg = {
            .lwip = {
                    .init_fn = lwip_tapif_init,
                    .input_fn = lwip_tapif_input,
            }
    };
    // configure inherent esp-netif parameters
    esp_netif_ip_info_t ip_info = {};
    esp_netif_flags_t netif_flags = (ESP_NETIF_FLAG_EVENT_IP_MODIFIED | ESP_NETIF_FLAG_AUTOUP);
#if !CONFIG_EXAMPLE_CONNECT_WAIT_FOR_IP
    ip_info.ip.addr = ipaddr_addr(CONFIG_EXAMPLE_CONNECT_TAPIF_IP_ADDR);
    ip_info.netmask.addr = ipaddr_addr(CONFIG_EXAMPLE_CONNECT_TAPIF_NETMASK);
    ip_info.gw.addr = ipaddr_addr(CONFIG_EXAMPLE_CONNECT_TAPIF_GW);
#else
    netif_flags |= ESP_NETIF_DHCP_CLIENT;
#endif

    esp_netif_inherent_config_t base_cfg = {
            .if_key = "TAP",
            .ip_info = &ip_info,
            .flags = netif_flags,
            .get_ip_event = TAP0_GOT_IP,
            .route_prio = 100
    };

    // put all configs together
    esp_netif_config_t cfg = {
            .base = &base_cfg,
            .driver = &driver_cfg,
            .stack = &stack_cfg
    };

    // create the interface and attach it to the tapio-handle
    esp_netif_t *tap_netif = esp_netif_new(&cfg);
    esp_netif_attach(tap_netif, driver_cfg.handle);
#if CONFIG_EXAMPLE_CONNECT_WAIT_FOR_IP
    ESP_LOGI(TAG, "Waiting for IP addresses...");
    esp_netif_action_connected(tap_netif, 0, 0, 0);
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, TAP0_GOT_IP, event_handler, NULL));
    s_events = xEventGroupCreate();
    xEventGroupWaitBits(s_events, 1, pdFALSE, pdFALSE, portMAX_DELAY);
    esp_netif_get_ip_info(tap_netif, &ip_info);
    ESP_LOGI(TAG, "Assigned IP address:"IPSTR ",", IP2STR(&ip_info.ip));
#endif // CONFIG_EXAMPLE_CONNECT_WAIT_FOR_IP
#endif // EXAMPLE_CONNECT_LWIP_TAPIF
    return ESP_OK;
}
