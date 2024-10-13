/* Static IP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_eth.h"
#include "ethernet_init.h"
#include "esp_event.h"
#include "esp_log.h"
#include <netdb.h>
#include "nvs_flash.h"

#if CONFIG_EXAMPLE_USE_WIFI && CONFIG_EXAMPLE_USE_ETH
#error "The example is designed to be used only with one Network interface. Either select WiFi or Ethernet."
#endif
#if !(CONFIG_EXAMPLE_USE_WIFI || CONFIG_EXAMPLE_USE_ETH)
#error "Incomplete network interface configuration. Either select WiFi or Ethernet."
#endif

/* The examples use configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#ifdef CONFIG_EXAMPLE_USE_WIFI
#define EXAMPLE_WIFI_SSID             CONFIG_EXAMPLE_WIFI_SSID
#define EXAMPLE_WIFI_PASS             CONFIG_EXAMPLE_WIFI_PASSWORD
#define EXAMPLE_MAXIMUM_RETRY         CONFIG_EXAMPLE_MAXIMUM_RETRY
#endif // EXAMPLE_USE_WIFI
#define EXAMPLE_STATIC_IP_ADDR        CONFIG_EXAMPLE_STATIC_IP_ADDR
#define EXAMPLE_STATIC_NETMASK_ADDR   CONFIG_EXAMPLE_STATIC_NETMASK_ADDR
#define EXAMPLE_STATIC_GW_ADDR        CONFIG_EXAMPLE_STATIC_GW_ADDR
#ifdef CONFIG_EXAMPLE_STATIC_DNS_AUTO
#define EXAMPLE_MAIN_DNS_SERVER       EXAMPLE_STATIC_GW_ADDR
#define EXAMPLE_BACKUP_DNS_SERVER     "0.0.0.0"
#else
#define EXAMPLE_MAIN_DNS_SERVER       CONFIG_EXAMPLE_STATIC_DNS_SERVER_MAIN
#define EXAMPLE_BACKUP_DNS_SERVER     CONFIG_EXAMPLE_STATIC_DNS_SERVER_BACKUP
#endif // CONFIG_EXAMPLE_STATIC_DNS_AUTO
#ifdef CONFIG_EXAMPLE_STATIC_DNS_RESOLVE_TEST
#define EXAMPLE_RESOLVE_DOMAIN        CONFIG_EXAMPLE_STATIC_RESOLVE_DOMAIN
#endif // CONFIG_EXAMPLE_STATIC_DNS_RESOLVE_TEST
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_network_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define CONNECTED_BIT           BIT0
#define WIFI_FAIL_BIT           BIT1

#define ETH_CONNECTION_TMO_MS   (10000)

static const char *TAG = "static_ip";

static esp_err_t example_set_dns_server(esp_netif_t *netif, uint32_t addr, esp_netif_dns_type_t type)
{
    if (addr && (addr != IPADDR_NONE)) {
        esp_netif_dns_info_t dns;
        dns.ip.u_addr.ip4.addr = addr;
        dns.ip.type = IPADDR_TYPE_V4;
        ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, type, &dns));
    }
    return ESP_OK;
}

static void example_set_static_ip(esp_netif_t *netif)
{
    if (esp_netif_dhcpc_stop(netif) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop dhcp client");
        return;
    }
    esp_netif_ip_info_t ip;
    memset(&ip, 0 , sizeof(esp_netif_ip_info_t));
    ip.ip.addr = ipaddr_addr(EXAMPLE_STATIC_IP_ADDR);
    ip.netmask.addr = ipaddr_addr(EXAMPLE_STATIC_NETMASK_ADDR);
    ip.gw.addr = ipaddr_addr(EXAMPLE_STATIC_GW_ADDR);
    if (esp_netif_set_ip_info(netif, &ip) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set ip info");
        return;
    }
    ESP_LOGD(TAG, "Success to set static ip: %s, netmask: %s, gw: %s", EXAMPLE_STATIC_IP_ADDR, EXAMPLE_STATIC_NETMASK_ADDR, EXAMPLE_STATIC_GW_ADDR);
    ESP_ERROR_CHECK(example_set_dns_server(netif, ipaddr_addr(EXAMPLE_MAIN_DNS_SERVER), ESP_NETIF_DNS_MAIN));
    ESP_ERROR_CHECK(example_set_dns_server(netif, ipaddr_addr(EXAMPLE_BACKUP_DNS_SERVER), ESP_NETIF_DNS_BACKUP));
}

#ifdef CONFIG_EXAMPLE_USE_WIFI
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    static int s_retry_num = 0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        example_set_static_ip(arg);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_network_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_network_event_group, CONNECTED_BIT);
    }
}

static void wifi_init_sta(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        sta_netif,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        sta_netif,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. In case your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by wifi_event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_network_event_group,
            CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_network_event_group);
}
#endif // CONFIG_EXAMPLE_USE_WIFI

#ifdef CONFIG_EXAMPLE_USE_ETH
static void eth_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == ETH_EVENT && event_id == ETHERNET_EVENT_CONNECTED) {
        example_set_static_ip(arg);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_network_event_group, CONNECTED_BIT);
    }
}

static void eth_init(void)
{
    uint8_t eth_port_cnt = 0;
    esp_eth_handle_t *eth_handles;
    ESP_ERROR_CHECK(ethernet_init_all(&eth_handles, &eth_port_cnt));
    if (eth_port_cnt > 1) {
        ESP_LOGW(TAG, "multiple Ethernet devices detected, the first initialized is to be used!");
    }

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&cfg);
    // Attach Ethernet driver to TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif,  esp_eth_new_netif_glue(eth_handles[0])));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(ETH_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &eth_event_handler,
                                                        eth_netif,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_ETH_GOT_IP,
                                                        &eth_event_handler,
                                                        eth_netif,
                                                        &instance_got_ip));

    ESP_ERROR_CHECK(esp_eth_start(eth_handles[0]));

    /* Waiting until the connection is established (CONNECTED_BIT). The bits are set by eth_event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_network_event_group,
            CONNECTED_BIT,
            pdFALSE,
            pdFALSE,
            pdMS_TO_TICKS(ETH_CONNECTION_TMO_MS));

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (!(bits & CONNECTED_BIT)) {
        ESP_LOGE(TAG, "Ethernet link not connected in defined timeout of %d msecs", ETH_CONNECTION_TMO_MS);
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_network_event_group);
}
#endif // CONFIG_EXAMPLE_USE_ETH

void app_main(void)
{
    s_network_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

#ifdef CONFIG_EXAMPLE_USE_WIFI
    wifi_init_sta();
#endif
#ifdef CONFIG_EXAMPLE_USE_ETH
    eth_init();
#endif

#ifdef CONFIG_EXAMPLE_STATIC_DNS_RESOLVE_TEST
    struct addrinfo *address_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    ESP_LOGI(TAG, "Trying to resolve IP address for domain: '%s'", EXAMPLE_RESOLVE_DOMAIN);
    int res = getaddrinfo(EXAMPLE_RESOLVE_DOMAIN, NULL, &hints, &address_info);
    if (res != 0 || address_info == NULL) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s: "
                      "getaddrinfo() returns %d, addrinfo=%p", EXAMPLE_RESOLVE_DOMAIN, res, address_info);
    } else {
        if (address_info->ai_family == AF_INET) {
            struct sockaddr_in *p = (struct sockaddr_in *)address_info->ai_addr;
            ESP_LOGI(TAG, "Resolved IPv4 address: %s", ipaddr_ntoa((const ip_addr_t*)&p->sin_addr.s_addr));
        }
#if CONFIG_LWIP_IPV6
        else if (address_info->ai_family == AF_INET6) {
            struct sockaddr_in6 *p = (struct sockaddr_in6 *)address_info->ai_addr;
            ESP_LOGI(TAG, "Resolved IPv6 address: %s", ip6addr_ntoa((const ip6_addr_t*)&p->sin6_addr));
        }
#endif
    }
#endif
}
