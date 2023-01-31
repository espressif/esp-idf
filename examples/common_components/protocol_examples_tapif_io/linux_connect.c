/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_err.h"
#include "esp_netif.h"      // esp-netif
#include "tapio.h"          // esp-netif's driver side
#include "lwip/tapif.h"     // esp-netif's network stack side

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
    ip_info.ip.addr = ipaddr_addr(CONFIG_EXAMPLE_CONNECT_TAPIF_IP_ADDR);
    ip_info.netmask.addr = ipaddr_addr(CONFIG_EXAMPLE_CONNECT_TAPIF_NETMASK);
    ip_info.gw.addr = ipaddr_addr(CONFIG_EXAMPLE_CONNECT_TAPIF_GW);

    esp_netif_inherent_config_t base_cfg = {
            .if_key = "TAP",
            .flags = ESP_NETIF_FLAG_AUTOUP,
            .ip_info = &ip_info,
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
#endif // EXAMPLE_CONNECT_LWIP_TAPIF
    return ESP_OK;
}
