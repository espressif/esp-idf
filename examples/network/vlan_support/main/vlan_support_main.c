/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Ethernet Basic Example with an additional VLAN interface. */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "ethernet_init.h"
#include "esp_event.h"
#include "esp_log.h"
#include "eth_vlan_utils.h"
#include "sdkconfig.h"
#include "lwip/prot/ethernet.h"
#include "lwip/prot/ieee.h"
#include "esp_netif_net_stack.h"

static const char *TAG = "eth_vlan_example";

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_vlan_event_group;

/* Ehernet Link upevent */
#define VLAN_IF_UP_BIT          BIT0
#if CONFIG_EXAMPLE_EXTRA_VLAN_INTERFACE
#define EXTRA_VLAN_IF_UP_BIT    BIT1
#define ALL_VLAN_IF_UP_BITS     VLAN_IF_UP_BIT | EXTRA_VLAN_IF_UP_BIT
#else
#define ALL_VLAN_IF_UP_BITS     VLAN_IF_UP_BIT
#endif

/**
 * @brief Event handler for Ethernet events
 *
 * @note This function is not required for core functionality.
 *
 * @param arg
 * @param event_base the base ID of the event to register the handler for
 * @param event_id the ID of the event with which to unregister the handler
 * @param event_data the data, specific to the event occurrence, that gets passed to the handler
 */
void eth_event_handler(void *arg, esp_event_base_t event_base,
                       int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
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
        ESP_LOGI(TAG, "Default Event");
        break;
    }
}


/**
 * @brief Event handler for IP_EVENT_ETH_GOT_IP
 *
 * @note This function is not required for core functionality.
 *
 * @param arg
 * @param event_base the base ID of the event to register the handler for
 * @param event_id the ID of the event with which to unregister the handler
 * @param event_data the data, specific to the event occurrence, that gets passed to the handler
 */
void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;
    u8_t hwaddr[NETIF_MAX_HWADDR_LEN];
    u16_t netif_vlan_id;

    esp_netif_get_mac(event->esp_netif, hwaddr);
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "Ethernet interface(%"X8_F":%"X8_F":%"X8_F":%"X8_F":%"X8_F":%"X8_F"): %s, Got IP Address",
             hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5], esp_netif_get_ifkey(event->esp_netif));

    /* Print Intervace VLAN Id */
    struct netif *lwip_netif = esp_netif_get_netif_impl(event->esp_netif);
    netif_vlan_id =  *((uint16_t *)netif_get_client_data(lwip_netif, LWIP_NETIF_CLIENT_DATA_INDEX_MAX + 1));
    if (0xFFF != netif_vlan_id) {
        ESP_LOGI(TAG, "NETIF VLAN: %d", netif_vlan_id);
    }

    ESP_LOGI(TAG, "NETIF IP: " IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "NETIF MASK: " IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "NETIF GW: " IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");

    /* Set event VLAN interfaces are up */
    if (netif_vlan_id == CONFIG_EXAMPLE_ETHERNET_VLAN_ID) {
        xEventGroupSetBits(s_vlan_event_group, VLAN_IF_UP_BIT);
    }
#if CONFIG_EXAMPLE_EXTRA_VLAN_INTERFACE
    else if (netif_vlan_id == CONFIG_EXAMPLE_EXTRA_ETHERNET_VLAN_ID) {
        xEventGroupSetBits(s_vlan_event_group, EXTRA_VLAN_IF_UP_BIT);
    }
#endif

}


void app_main(void)
{
    s_vlan_event_group = xEventGroupCreate();

    static esp_vlan_netifs vlan_netif_list;

    // Initialize Ethernet driver
    uint8_t eth_port_cnt = 0;
    esp_eth_handle_t *eth_handle;
    ESP_ERROR_CHECK(example_eth_init(&eth_handle, &eth_port_cnt));

    // Check or multiple ethernet interface
    if (1 < eth_port_cnt) {
        ESP_LOGW(TAG, "Multiple Ethernet Interface detected: Only the first initialized interface is going to be used.");
    }

    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());

    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    static uint16_t vlan_id[MAX_ETH_NETIF_COUNT];
    vlan_netif_list.netif_count = 0;

    // Create new default instance of default esp-netif for Ethernet.
    // vlan_netif_list.esp_netif[0] must be used as default esp-netif for Ethernet,
    // and all untagged and unsupported tagged frames will go to this interface.
    esp_netif_config_t eth_cfg = ESP_NETIF_DEFAULT_ETH();
    vlan_netif_list.esp_netif[vlan_netif_list.netif_count] = esp_netif_new(&eth_cfg);

    // Attach Ethernet drivers to TCP/IP stack
    esp_eth_netif_glue_handle_t eth_glue = esp_eth_new_netif_glue(eth_handle[0]);
    ESP_ERROR_CHECK(esp_netif_attach(vlan_netif_list.esp_netif[vlan_netif_list.netif_count], eth_glue));

    // Set VLAN tag in data of lwip netif.
    // VLAN tag 0xFFF can be used to indicate a wildcard match in management operations or filtering database entries.
    vlan_id[vlan_netif_list.netif_count] = 0xFFF;
    struct netif *lwip_netif = esp_netif_get_netif_impl(vlan_netif_list.esp_netif[vlan_netif_list.netif_count]);
    netif_set_client_data(lwip_netif, LWIP_NETIF_CLIENT_DATA_INDEX_MAX + 1, (void *)&vlan_id[vlan_netif_list.netif_count]);

#if CONFIG_EXAMPLE_ETHERNET_DEF_IF
    /* Set as the default interface */
    esp_netif_set_default_netif(vlan_netif_list.esp_netif[vlan_netif_list.netif_count]);
#endif
    vlan_netif_list.netif_count++;


    // Create new instance of esp-netif for Ethernet vlan interface
    esp_netif_config_t vlan_eth_cfg;
    ESP_ERROR_CHECK(get_vlan_netif_config(CONFIG_EXAMPLE_ETHERNET_VLAN_ID, &vlan_eth_cfg));
    vlan_netif_list.esp_netif[vlan_netif_list.netif_count] = esp_netif_new(&vlan_eth_cfg);
    free_vlan_config(&vlan_eth_cfg);

    // Attach vlan Ethernet drivers to TCP/IP stack
    esp_eth_netif_glue_handle_t vlan_glue = esp_eth_new_netif_glue(eth_handle[0]);
    ESP_ERROR_CHECK(esp_netif_attach(vlan_netif_list.esp_netif[vlan_netif_list.netif_count], vlan_glue));

    // Set VLAN tag in data of lwip netif
    vlan_id[vlan_netif_list.netif_count] = CONFIG_EXAMPLE_ETHERNET_VLAN_ID;
    lwip_netif = esp_netif_get_netif_impl(vlan_netif_list.esp_netif[vlan_netif_list.netif_count]);
    netif_set_client_data(lwip_netif, LWIP_NETIF_CLIENT_DATA_INDEX_MAX + 1, (void *)&vlan_id[vlan_netif_list.netif_count]);

    // Set static IP to the ETH_VLAN interface
    esp_netif_dhcpc_stop(vlan_netif_list.esp_netif[vlan_netif_list.netif_count]);

    esp_netif_ip_info_t info_t;
    memset(&info_t, 0, sizeof(esp_netif_ip_info_t));
    inet_aton(CONFIG_EXAMPLE_VLAN_STATIC_IPV4_ADDR, &info_t.ip.addr);
    inet_aton(CONFIG_EXAMPLE_VLAN_STATIC_ADDR_DEF_GW, &info_t.gw.addr);
    inet_aton(CONFIG_EXAMPLE_VLAN_STATIC_ADDR_MASK, &info_t.netmask.addr);
    esp_netif_set_ip_info(vlan_netif_list.esp_netif[vlan_netif_list.netif_count], &info_t);

#if CONFIG_EXAMPLE_VLAN_DEF_IF
    /* Set as the default interface */
    esp_netif_set_default_netif(vlan_netif_list.esp_netif[vlan_netif_list.netif_count]);
#endif
    vlan_netif_list.netif_count++;


#if defined(CONFIG_EXAMPLE_EXTRA_VLAN_INTERFACE)
    // Create an additional instance of esp-netif for Ethernet vlan interface
    ESP_ERROR_CHECK(get_vlan_netif_config(CONFIG_EXAMPLE_EXTRA_ETHERNET_VLAN_ID, &vlan_eth_cfg));
    vlan_netif_list.esp_netif[vlan_netif_list.netif_count] = esp_netif_new(&vlan_eth_cfg);
    free_vlan_config(&vlan_eth_cfg);

    // Attach vlan Ethernet drivers to TCP/IP stack
    vlan_glue = esp_eth_new_netif_glue(eth_handle[0]);
    ESP_ERROR_CHECK(esp_netif_attach(vlan_netif_list.esp_netif[vlan_netif_list.netif_count], vlan_glue));

    // Set VLAN tag in data of lwip netif
    vlan_id[vlan_netif_list.netif_count] = CONFIG_EXAMPLE_EXTRA_ETHERNET_VLAN_ID;
    lwip_netif = esp_netif_get_netif_impl(vlan_netif_list.esp_netif[vlan_netif_list.netif_count]);
    netif_set_client_data(lwip_netif, LWIP_NETIF_CLIENT_DATA_INDEX_MAX + 1, (void *)&vlan_id[vlan_netif_list.netif_count]);

    // Set static IP to the ETH_VLAN interface
    esp_netif_dhcpc_stop(vlan_netif_list.esp_netif[vlan_netif_list.netif_count]);

    memset(&info_t, 0, sizeof(esp_netif_ip_info_t));
    inet_aton(CONFIG_EXAMPLE_EXTRA_VLAN_STATIC_IPV4_ADDR, &info_t.ip.addr);
    inet_aton(CONFIG_EXAMPLE_EXTRA_VLAN_STATIC_ADDR_DEF_GW, &info_t.gw.addr);
    inet_aton(CONFIG_EXAMPLE_EXTRA_VLAN_STATIC_ADDR_MASK, &info_t.netmask.addr);
    esp_netif_set_ip_info(vlan_netif_list.esp_netif[vlan_netif_list.netif_count], &info_t);

#if CONFIG_EXAMPLE_EXTRA_VLAN_DEF_IF
    /* Set as the default interface */
    esp_netif_set_default_netif(vlan_netif_list.esp_netif[vlan_netif_list.netif_count]);
#endif
    vlan_netif_list.netif_count++;
#endif  //CONFIG_EXAMPLE_EXTRA_VLAN_INTERFACE

    // Update input path of the ethernet driver
    esp_eth_update_input_path(eth_handle[0], eth_input_to_netif, &vlan_netif_list);

    // start Ethernet driver state machine
    ESP_ERROR_CHECK(esp_eth_start(eth_handle[0]));

    /* Wait until all the VLAN interfaces are up */
    xEventGroupWaitBits(s_vlan_event_group,
                        ALL_VLAN_IF_UP_BITS,
                        pdFALSE,
                        pdTRUE,
                        portMAX_DELAY);

#if IP_NAPT
    /* Enable NAPT on the configured interface */
#if CONFIG_EXAMPLE_ETHERNET_NAPT_IF
    if (esp_netif_napt_enable(vlan_netif_list.esp_netif[0]) != ESP_OK) {
#elif CONFIG_EXAMPLE_VLAN_NAPT_IF
    if (esp_netif_napt_enable(vlan_netif_list.esp_netif[1]) != ESP_OK) {
#elif CONFIG_EXAMPLE_EXTRA_VLAN_NAPT_IF
    if (esp_netif_napt_enable(vlan_netif_list.esp_netif[2]) != ESP_OK) {
#endif
        ESP_LOGE(TAG, "Failed to enable NAPT on selected netif");
    }
#endif // #if IP_NAPT
}
