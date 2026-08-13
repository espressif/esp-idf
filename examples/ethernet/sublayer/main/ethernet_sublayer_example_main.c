/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "ethernet_init.h"
#include "esp_log.h"
#include "sdkconfig.h"

#ifndef CONFIG_ETH_SUBLAYER_SUPPORT
#error "This example requires CONFIG_ETH_SUBLAYER_SUPPORT (enable IDF experimental features)"
#endif

static const char *TAG = "eth_sublayer_example";

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address (%s)", esp_netif_get_ifkey(event->esp_netif));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
}

static esp_err_t create_vlan_netif_config(uint16_t vlan_id, esp_netif_config_t *cfg)
{
    char *if_key;
    if (asprintf(&if_key, "ETH_VLAN%d", vlan_id) < 0) {
        return ESP_ERR_NO_MEM;
    }

    esp_netif_inherent_config_t *base = malloc(sizeof(*base));
    if (base == NULL) {
        free(if_key);
        return ESP_ERR_NO_MEM;
    }
    *base = (esp_netif_inherent_config_t)ESP_NETIF_INHERENT_DEFAULT_ETH();
    base->if_key = if_key;
    base->if_desc = "eth_vlan";
    base->route_prio = 30;

    cfg->base = base;
    cfg->driver = NULL;
    cfg->stack = ESP_NETIF_NETSTACK_DEFAULT_ETH;
    return ESP_OK;
}

static void free_vlan_netif_config(esp_netif_config_t *cfg)
{
    if (cfg && cfg->base) {
        free((void *)cfg->base->if_key);
        free((void *)cfg->base);
    }
}

void app_main(void)
{
    uint8_t eth_port_cnt = 0;
    esp_eth_handle_t *eth_handles = NULL;

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(ethernet_init_all(&eth_handles, &eth_port_cnt));
    if (eth_port_cnt > 1) {
        ESP_LOGW(TAG, "Multiple Ethernet interfaces detected, using the first one");
    }

    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    // Create Ethernet sublayer bound to the physical driver
    esp_eth_sublayer_config_t sub_config = ESP_ETH_SUBLAYER_CONFIG_DEFAULT();
    sub_config.eth_handle = eth_handles[0];
    esp_eth_sublayer_handle_t sublayer = NULL;
    ESP_ERROR_CHECK(esp_eth_sublayer_new(&sub_config, &sublayer));

    // Untagged interface (DHCP)
    esp_eth_sublayer_vlan_handle_t untagged = NULL;
    ESP_ERROR_CHECK(esp_eth_sublayer_vlan_add(sublayer, ESP_ETH_SUBLAYER_UNTAGGED_VID, &untagged));
    esp_netif_config_t eth_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&eth_cfg);
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, untagged));

    // Tagged VLAN interface (static IP)
    esp_eth_sublayer_vlan_handle_t vlan = NULL;
    ESP_ERROR_CHECK(esp_eth_sublayer_vlan_add(sublayer, CONFIG_EXAMPLE_ETHERNET_VLAN_ID, &vlan));
    esp_netif_config_t vlan_cfg;
    ESP_ERROR_CHECK(create_vlan_netif_config(CONFIG_EXAMPLE_ETHERNET_VLAN_ID, &vlan_cfg));
    esp_netif_t *vlan_netif = esp_netif_new(&vlan_cfg);
    free_vlan_netif_config(&vlan_cfg);
    ESP_ERROR_CHECK(esp_netif_attach(vlan_netif, vlan));

    esp_netif_dhcpc_stop(vlan_netif);
    esp_netif_ip_info_t vlan_ip = {0};
    inet_aton(CONFIG_EXAMPLE_VLAN_STATIC_IPV4_ADDR, &vlan_ip.ip.addr);
    inet_aton(CONFIG_EXAMPLE_VLAN_STATIC_ADDR_DEF_GW, &vlan_ip.gw.addr);
    inet_aton(CONFIG_EXAMPLE_VLAN_STATIC_ADDR_MASK, &vlan_ip.netmask.addr);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(vlan_netif, &vlan_ip));

    ESP_ERROR_CHECK(esp_eth_start(eth_handles[0]));
}
