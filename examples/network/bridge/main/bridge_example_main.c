/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_netif_br_glue.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "ethernet_init.h"
#if CONFIG_EXAMPLE_BR_WIFI
#include "esp_wifi.h"
#include "nvs_flash.h"
#endif // CONFIG_EXAMPLE_BR_WIFI
#include "sdkconfig.h"
#include "esp_console.h"
#include "bridge_console_cmd.h"

static const char *TAG = "eth_bridge_example";

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet (%p) Link Up", eth_handle);
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet (%p) Link Down", eth_handle);
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet (%p) Started", eth_handle);
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet (%p) Stopped", eth_handle);
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

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR "\r", IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR "\r", IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR "\r", IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
}

#if CONFIG_EXAMPLE_BR_WIFI
static void example_wifi_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = CONFIG_EXAMPLE_BR_WIFI_SSID,
            .ssid_len = strlen(CONFIG_EXAMPLE_BR_WIFI_SSID),
            .password = CONFIG_EXAMPLE_BR_WIFI_PASSWORD,
            .max_connection = CONFIG_EXAMPLE_BR_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .channel = CONFIG_EXAMPLE_BR_WIFI_CHANNEL
        },
    };
    if (strlen(CONFIG_EXAMPLE_BR_WIFI_PASSWORD) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
}
#endif //CONFIG_EXAMPLE_BR_WIFI

void app_main(void)
{
    // Initialize Ethernet driver
    uint8_t eth_port_cnt = 0;
    esp_eth_handle_t *eth_handles;
    ESP_ERROR_CHECK(example_eth_init(&eth_handles, &eth_port_cnt));

    // The same MAC address will be used for all Ethernet ports since the bridge acts as one device
    uint8_t common_mac_addr[ETH_ADDR_LEN];
    // If internal Ethernet is not supported by ESP32x SoC, Locally Administered OUI address might be returned.
    // Note that Locally Administered OUI range should be used only when testing on a LAN under your control!
    ESP_ERROR_CHECK(esp_read_mac(common_mac_addr, ESP_MAC_ETH));
    for (int i = 0; i < eth_port_cnt; i++) {
        ESP_ERROR_CHECK(esp_eth_ioctl(eth_handles[i], ETH_CMD_S_MAC_ADDR, common_mac_addr));
    }

    // Initialize TCP/IP network interface
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create instances of esp-netif for Ethernet ports
    esp_netif_t **eth_netifs = calloc(eth_port_cnt, sizeof(esp_netif_t *));
    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    esp_netif_config_t netif_cfg = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };
    char if_key_str[10];
    char if_desc_str[10];
    char num_str[3];
    for (int i = 0; i < eth_port_cnt; i++) {
        itoa(i, num_str, 10);
        strcat(strcpy(if_key_str, "ETH_"), num_str);
        strcat(strcpy(if_desc_str, "eth"), num_str);
        esp_netif_config.if_key = if_key_str;
        esp_netif_config.if_desc = if_desc_str;
        esp_netif_config.route_prio = 50 - i;
        esp_netif_config.flags = 0; // esp-netif flags need to be zero when port's to be bridged

        eth_netifs[i] = esp_netif_new(&netif_cfg);

        // Attach Ethernet driver to TCP/IP stack
        ESP_ERROR_CHECK(esp_netif_attach(eth_netifs[i], esp_eth_new_netif_glue(eth_handles[i])));
    }
    uint8_t br_ports = eth_port_cnt;

#if CONFIG_EXAMPLE_BR_WIFI
    example_wifi_init();
    esp_netif_inherent_config_t esp_netif_config_wifi = ESP_NETIF_INHERENT_DEFAULT_WIFI_AP();
    esp_netif_config_wifi.flags = ESP_NETIF_FLAG_AUTOUP; // esp-netif flags need to be zero when port's to be bridged except for AP's ESP_NETIF_FLAG_AUTOUP
    esp_netif_config_wifi.ip_info = NULL; // no IP address for physical interface
    esp_netif_t *wifi_netif = esp_netif_create_wifi(WIFI_IF_AP, &esp_netif_config_wifi);
    ESP_ERROR_CHECK(esp_wifi_set_default_wifi_ap_handlers());
    br_ports++;
#endif

    // Create instance of esp-netif for bridge interface
#if CONFIG_EXAMPLE_BR_DHCPS
    esp_netif_inherent_config_t esp_netif_br_config = ESP_NETIF_INHERENT_DEFAULT_BR_DHCPS();
#else
    esp_netif_inherent_config_t esp_netif_br_config = ESP_NETIF_INHERENT_DEFAULT_BR();
#endif // CONFIG_EXAMPLE_BR_DHCPS
    esp_netif_config_t netif_br_cfg = {
        .base = &esp_netif_br_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_BR,
    };
    // Bridge configuration
    bridgeif_config_t bridgeif_config = {
        .max_fdb_dyn_entries = 10, // maximum number of address entries in dynamic forwarding database
        .max_fdb_sta_entries = 2,  // maximum number of address entries in static forwarding database
        .max_ports = br_ports      // maximum number of ports the bridge can consist of
    };
    esp_netif_br_config.bridge_info = &bridgeif_config;
    // Set MAC address of bridge interface the same as the Ethernet interface
    memcpy(esp_netif_br_config.mac, common_mac_addr, ETH_ADDR_LEN);
    esp_netif_t *br_netif = esp_netif_new(&netif_br_cfg);

    // Create new esp netif bridge glue instance
    esp_netif_br_glue_handle_t netif_br_glue = esp_netif_br_glue_new();
    // Add Ethernet port interfaces to that esp netif bridge glue instance
    for (int i = 0; i < eth_port_cnt; i++) {
        ESP_ERROR_CHECK(esp_netif_br_glue_add_port(netif_br_glue, eth_netifs[i]));
    }
#if CONFIG_EXAMPLE_BR_WIFI
    ESP_ERROR_CHECK(esp_netif_br_glue_add_wifi_port(netif_br_glue, wifi_netif));
#endif // CONFIG_EXAMPLE_BR_WIFI

    // Attach esp netif bridge glue instance with added ports to bridge netif
    ESP_ERROR_CHECK(esp_netif_attach(br_netif, netif_br_glue));

    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    for (int i = 0; i < eth_port_cnt; i++) {
        // Since the MAC forwarding is performed in lwIP bridge, we need to pass all addresses through the Ethernet MACs
        bool promiscuous = true;
        esp_eth_ioctl(eth_handles[i], ETH_CMD_S_PROMISCUOUS, &promiscuous);
        // Start Ethernet driver state machine
        ESP_ERROR_CHECK(esp_eth_start(eth_handles[i]));
    }
#if CONFIG_EXAMPLE_BR_WIFI
    ESP_ERROR_CHECK(esp_wifi_start());
#endif

    // --- Initialize Console ---
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "bridge>";

    // install console REPL environment
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
    example_register_br_config_commands(br_netif, br_ports);
    // start console REPL
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
