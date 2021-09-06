/*
Copyright © 2021 Ci4Rail GmbH
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <stdio.h>
#include "esp_cdc_ecm_driver.h"
#include "esp_cdc_ecm_netif_glue.h"
#include "tinyusb.h"
#include "esp_log.h"
#include "esp_event.h"
#include "tcp_server.h"

static const char *TAG = "main";

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    switch (event_id) {
        case ETHERNET_EVENT_CONNECTED: {
            ESP_LOGI(TAG, "Ethernet Link Up");
            break;
        }
        case ETHERNET_EVENT_DISCONNECTED: {
            ESP_LOGI(TAG, "Ethernet Link Down");
            break;
        }
        case ETHERNET_EVENT_START: {
            ESP_LOGI(TAG, "Ethernet Started");
            break;
        }
        case ETHERNET_EVENT_STOP: {
            ESP_LOGI(TAG, "Ethernet Stopped");
            break;
        }
        default: {
            break;
        }
    }
}

static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
}

void app_main(void)
{
#if CONFIG_EXAMPLE_STATIC_IP
    const esp_netif_ip_info_t ip_info = {
        .ip.addr = ESP_IP4TOADDR(CONFIG_EXAMPLE_IP_ADDRESS_1, CONFIG_EXAMPLE_IP_ADDRESS_2, CONFIG_EXAMPLE_IP_ADDRESS_3, CONFIG_EXAMPLE_IP_ADDRESS_4),
        .netmask.addr = ESP_IP4TOADDR(255, 255, 255, 0),
        .gw.addr = ESP_IP4TOADDR(0, 0, 0, 0)
    };

    const esp_netif_inherent_config_t base = {   
        .flags = (esp_netif_flags_t)(ESP_NETIF_FLAG_GARP | ESP_NETIF_FLAG_EVENT_IP_MODIFIED),
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_EMPTY(mac)
        .ip_info = &ip_info,
        .get_ip_event = IP_EVENT_ETH_GOT_IP,
        .lost_ip_event = IP_EVENT_ETH_LOST_IP,
        .if_key = "ETH_DEF",
        .if_desc = "eth",
        .route_prio = 50
    };

    esp_netif_config_t cfg = {
        .base = &base,      
        .driver = NULL,                          
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH, 
    };
#else
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
#endif //EXAMPLE_STATIC_IP

    /* Initialize TCP/IP network interface (should be called only once in application) */
    ESP_ERROR_CHECK(esp_netif_init());
    /* Create default event loop that running in background */
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *eth_netif = esp_netif_new(&cfg);
    esp_cdc_ecm_set_default_handlers(eth_netif);
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    /* setup CDC-ECM interface*/
    tinyusb_config_t tusb_cfg = {
        .descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    esp_cdc_ecm_driver_t* handle;
    ESP_ERROR_CHECK(esp_cdc_ecm_driver_install(&handle));
    /* attach Ethernet driver to TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_cdc_ecm_new_netif_glue(handle)));

    /* Create a tasks for tcp_servers */
    xTaskCreate( tcp_server_task, "tcp_server1", 4096, (void*)1234, 4, NULL);
    xTaskCreate( tcp_server_task, "tcp_server2", 4096, (void*)3333, 4, NULL);
}

