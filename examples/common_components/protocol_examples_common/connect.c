/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include "protocol_examples_common.h"
#include "example_common_private.h"
#include "sdkconfig.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"

static const char *TAG = "example_common";

#if CONFIG_EXAMPLE_CONNECT_IPV6
/* types of ipv6 addresses to be displayed on ipv6 events */
const char *example_ipv6_addr_types_to_str[6] = {
    "ESP_IP6_ADDR_IS_UNKNOWN",
    "ESP_IP6_ADDR_IS_GLOBAL",
    "ESP_IP6_ADDR_IS_LINK_LOCAL",
    "ESP_IP6_ADDR_IS_SITE_LOCAL",
    "ESP_IP6_ADDR_IS_UNIQUE_LOCAL",
    "ESP_IP6_ADDR_IS_IPV4_MAPPED_IPV6"
};
#endif

/**
 * @brief Checks the netif description if it contains specified prefix.
 * All netifs created withing common connect component are prefixed with the module TAG,
 * so it returns true if the specified netif is owned by this module
 */
bool example_is_our_netif(const char *prefix, esp_netif_t *netif)
{
    return strncmp(prefix, esp_netif_get_desc(netif), strlen(prefix) - 1) == 0;
}

esp_netif_t *get_example_netif_from_desc(const char *desc)
{
    esp_netif_t *netif = NULL;
    while ((netif = esp_netif_next(netif)) != NULL) {
        if (strcmp(esp_netif_get_desc(netif), desc) == 0) {
            return netif;
        }
    }
    return netif;
}

void example_print_all_netif_ips(const char *prefix)
{
    // iterate over active interfaces, and print out IPs of "our" netifs
    esp_netif_t *netif = NULL;
    esp_netif_ip_info_t ip;
    for (int i = 0; i < esp_netif_get_nr_of_ifs(); ++i) {
        netif = esp_netif_next(netif);
        if (example_is_our_netif(prefix, netif)) {
            ESP_LOGI(TAG, "Connected to %s", esp_netif_get_desc(netif));
            ESP_ERROR_CHECK(esp_netif_get_ip_info(netif, &ip));

            ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&ip.ip));
#if CONFIG_EXAMPLE_CONNECT_IPV6
            esp_ip6_addr_t ip6[MAX_IP6_ADDRS_PER_NETIF];
            int ip6_addrs = esp_netif_get_all_ip6(netif, ip6);
            for (int j = 0; j < ip6_addrs; ++j) {
                esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&(ip6[j]));
                ESP_LOGI(TAG, "- IPv6 address: " IPV6STR ", type: %s", IPV62STR(ip6[j]), example_ipv6_addr_types_to_str[ipv6_type]);
            }
#endif
        }
    }
}


esp_err_t example_connect(void)
{
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
    if (example_ethernet_connect() != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_ERROR_CHECK(esp_register_shutdown_handler(&example_ethernet_shutdown));
#endif
#if CONFIG_EXAMPLE_CONNECT_WIFI
    if (example_wifi_connect() != ESP_OK) {
        return ESP_FAIL;
    }
    ESP_ERROR_CHECK(esp_register_shutdown_handler(&example_wifi_shutdown));
#endif

#if CONFIG_EXAMPLE_CONNECT_ETHERNET
    example_print_all_netif_ips(EXAMPLE_NETIF_DESC_ETH);
#endif

#if CONFIG_EXAMPLE_CONNECT_WIFI
    example_print_all_netif_ips(EXAMPLE_NETIF_DESC_STA);
#endif

    return ESP_OK;
}


esp_err_t example_disconnect(void)
{
#if CONFIG_EXAMPLE_CONNECT_ETHERNET
    example_ethernet_shutdown();
    ESP_ERROR_CHECK(esp_unregister_shutdown_handler(&example_ethernet_shutdown));
#endif
#if CONFIG_EXAMPLE_CONNECT_WIFI
    example_wifi_shutdown();
    ESP_ERROR_CHECK(esp_unregister_shutdown_handler(&example_wifi_shutdown));
#endif
    return ESP_OK;
}
