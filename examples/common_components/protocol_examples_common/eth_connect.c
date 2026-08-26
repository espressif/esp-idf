/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include "protocol_examples_common.h"
#include "example_common_private.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "ethernet_init.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"


static const char *TAG = "ethernet_connect";
static SemaphoreHandle_t s_semph_get_ip_addrs = NULL;
#if CONFIG_EXAMPLE_CONNECT_IPV6
static SemaphoreHandle_t s_semph_get_ip6_addrs = NULL;
#endif

static esp_netif_t *eth_start(void);
static void eth_stop(void);


/** Event handler for Ethernet events */

static void eth_on_got_ip(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    if (!example_is_our_netif(EXAMPLE_NETIF_DESC_ETH, event->esp_netif)) {
        return;
    }
    ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
    xSemaphoreGive(s_semph_get_ip_addrs);
}

#if CONFIG_EXAMPLE_CONNECT_IPV6

static void eth_on_got_ipv6(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
    if (!example_is_our_netif(EXAMPLE_NETIF_DESC_ETH, event->esp_netif)) {
        return;
    }
    esp_ip6_addr_type_t ipv6_type = esp_netif_ip6_get_addr_type(&event->ip6_info.ip);
    ESP_LOGI(TAG, "Got IPv6 event: Interface \"%s\" address: " IPV6STR ", type: %s", esp_netif_get_desc(event->esp_netif),
             IPV62STR(event->ip6_info.ip), example_ipv6_addr_types_to_str[ipv6_type]);
    if (ipv6_type == EXAMPLE_CONNECT_PREFERRED_IPV6_TYPE) {
        xSemaphoreGive(s_semph_get_ip6_addrs);
    }
}

static void on_eth_event(void *esp_netif, esp_event_base_t event_base,
                         int32_t event_id, void *event_data)
{
    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_ERROR_CHECK(esp_netif_create_ip6_linklocal(esp_netif));
        break;
    default:
        break;
    }
}

#endif // CONFIG_EXAMPLE_CONNECT_IPV6

static esp_eth_handle_t *s_eth_handles = NULL;
static uint8_t s_eth_count = 0;
static esp_eth_netif_glue_handle_t s_eth_glue = NULL;
static esp_netif_t *s_eth_netif = NULL;

static esp_netif_t *eth_start(void)
{
    ESP_ERROR_CHECK(ethernet_init_all(&s_eth_handles, &s_eth_count));

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    // Warning: the interface desc is used in tests to capture actual connection details (IP, gw, mask)
    esp_netif_config.if_desc = EXAMPLE_NETIF_DESC_ETH;
    esp_netif_config.route_prio = 64;
    esp_netif_config_t netif_config = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };
    s_eth_netif = esp_netif_new(&netif_config);

    s_eth_glue = esp_eth_new_netif_glue(s_eth_handles[0]);
    esp_netif_attach(s_eth_netif, s_eth_glue);

    // Register user defined event handlers
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_on_got_ip, NULL));
#ifdef CONFIG_EXAMPLE_CONNECT_IPV6
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &on_eth_event, s_eth_netif));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &eth_on_got_ipv6, NULL));
#endif

    ESP_ERROR_CHECK(esp_eth_start(s_eth_handles[0]));

    return s_eth_netif;
}

static void eth_stop(void)
{
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_on_got_ip));
#if CONFIG_EXAMPLE_CONNECT_IPV6
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_GOT_IP6, &eth_on_got_ipv6));
    ESP_ERROR_CHECK(esp_event_handler_unregister(ETH_EVENT, ETHERNET_EVENT_CONNECTED, &on_eth_event));
#endif
    ESP_ERROR_CHECK(esp_eth_stop(s_eth_handles[0]));
    ESP_ERROR_CHECK(esp_eth_del_netif_glue(s_eth_glue));
    esp_netif_destroy(s_eth_netif);
    ethernet_deinit_all(s_eth_handles);

    s_eth_glue = NULL;
    s_eth_netif = NULL;
    s_eth_handles = NULL;
    s_eth_count = 0;
}

/* tear down connection, release resources */
void example_ethernet_shutdown(void)
{
    if (s_semph_get_ip_addrs == NULL) {
        return;
    }
    vSemaphoreDelete(s_semph_get_ip_addrs);
    s_semph_get_ip_addrs = NULL;
#if CONFIG_EXAMPLE_CONNECT_IPV6
    vSemaphoreDelete(s_semph_get_ip6_addrs);
    s_semph_get_ip6_addrs = NULL;
#endif
    eth_stop();
}

esp_err_t example_ethernet_connect(void)
{
#if CONFIG_EXAMPLE_CONNECT_IPV4
    s_semph_get_ip_addrs = xSemaphoreCreateBinary();
    if (s_semph_get_ip_addrs == NULL) {
        return ESP_ERR_NO_MEM;
    }
#endif
#if CONFIG_EXAMPLE_CONNECT_IPV6
    s_semph_get_ip6_addrs = xSemaphoreCreateBinary();
    if (s_semph_get_ip6_addrs == NULL) {
        vSemaphoreDelete(s_semph_get_ip_addrs);
        return ESP_ERR_NO_MEM;
    }
#endif
    eth_start();
    ESP_LOGI(TAG, "Waiting for IP(s).");
#if CONFIG_EXAMPLE_CONNECT_IPV4
    xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY);
#endif
#if CONFIG_EXAMPLE_CONNECT_IPV6
    xSemaphoreTake(s_semph_get_ip6_addrs, portMAX_DELAY);
#endif
    return ESP_OK;
}
