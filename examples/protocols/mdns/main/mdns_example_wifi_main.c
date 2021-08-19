/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* MDNS-SD Query and advertise Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif_ip_addr.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "mdns.h"
#include "driver/gpio.h"
#include "netdb.h"
#include "esp_wifi.h"

#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
// #include "mdns_if_defaults.h"


/* FreeRTOS event group to signal when we are connected*/
// static EventGroupHandle_t s_wifi_event_group;

#define EXAMPLE_MDNS_INSTANCE CONFIG_MDNS_INSTANCE
#define EXAMPLE_BUTTON_GPIO     0

static const char * TAG = "mdns-test";
static char * generate_hostname(void);

#if CONFIG_MDNS_RESOLVE_TEST_SERVICES == 1
static void  query_mdns_host_with_gethostbyname(char * host);
static void  query_mdns_host_with_getaddrinfo(char * host);
#endif

static int s_retry_num = 0;

// #define EXAMPLE_ESP_WIFI_SSID     "Coworking"
#define EXAMPLE_ESP_WIFI_SSID     "Coworking"
#define EXAMPLE_ESP_WIFI_PASS     "Coworkers"

#define EXAMPLE_ESP_MAXIMUM_RETRY  2

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            // xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        //xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void initialise_mdns(void)
{
    esp_netif_t* netif_wifi = esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

     wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
         .threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    char * hostname = generate_hostname();

    //initialize mDNS
    // mdns_config_t mdns_config = MDNS_NETIF_DEFAULT_WIFI(netif_wifi);

   //_mdns_action_t _actions[] = {
   //      {
   //        .event_base = WIFI_EVENT,
   //        .event_id = WIFI_EVENT_STA_CONNECTED,
   //        .action = ACTION_ENABLE_WITH_DHCP_CHECK
   //      },
   //      {
   //        .event_base = WIFI_EVENT,
   //        .event_id = WIFI_EVENT_STA_DISCONNECTED,
   //        .action = ACTION_DISABLE
   //      },
   //      {
   //        .event_base = IP_EVENT,
   //        .event_id = IP_EVENT_STA_GOT_IP,
   //        .action = ACTION_ENABLE_ANNOUNCE
   //      }
   // };

   //mdns_config_t mdns_config = {
   //    .netif = netif_wifi,
   //    .num = 3,
   //    .next = {NULL},
   //    .actions = _actions
   //};

    mdns_config_t * mdns_config = calloc(1, sizeof(struct mdns_config_s));
    mdns_config->netif = netif_wifi;
    mdns_config->num = 3;
    mdns_config->actions = malloc(sizeof(_mdns_action_t) * 3);
    //mdns_config->actions = { { .esp_event_base_t = WIFI_EVENT, .event_id = WIFI_EVENT_STA_CONNECTED, .action = ACTION_ENABLE_WITH_DHCP_CHECK}};
    //mdns_config->actionsesp_event_base_t = WIFI_EVENT;
    mdns_config->actions[0].event_base = WIFI_EVENT;
    mdns_config->actions[0].event_id = WIFI_EVENT_STA_CONNECTED;
    mdns_config->actions[0].action = ACTION_ENABLE_WITH_DHCP_CHECK;

    mdns_config->actions[1].event_base = WIFI_EVENT;
    mdns_config->actions[1].event_id = WIFI_EVENT_STA_DISCONNECTED;
    mdns_config->actions[1].action = ACTION_DISABLE;

    mdns_config->actions[2].event_base = IP_EVENT;
    mdns_config->actions[2].event_id = IP_EVENT_STA_GOT_IP;
    mdns_config->actions[2].action = ACTION_ENABLE_ANNOUNCE;

    ESP_ERROR_CHECK( mdns_init_cfg(mdns_config) ); // mdns_add_netif

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
    char * desc;
    asprintf(&desc, "%s: %s", TAG, esp_netif_config.if_desc);
    esp_netif_config.if_desc = desc;
    esp_netif_config.route_prio = 64;
    esp_netif_config_t netif_config = {
        .base = &esp_netif_config,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
    };


    esp_netif_t * netif_eth = esp_netif_new(&netif_config);
    mdns_config_t * mdns_config_eth = calloc(1, sizeof(struct mdns_config_s));
    mdns_config_eth->netif = netif_eth;
    mdns_config_eth->num = 2;
    mdns_config_eth->actions = malloc(sizeof(_mdns_action_t) * 3);
    //mdns_config->actions = { { .esp_event_base_t = WIFI_EVENT, .event_id = WIFI_EVENT_STA_CONNECTED, .action = ACTION_ENABLE_WITH_DHCP_CHECK}};
    mdns_config_eth->actions[0].event_base = ETH_EVENT;
    mdns_config_eth->actions[0].event_id = ETHERNET_EVENT_CONNECTED;
    mdns_config_eth->actions[0].action = ACTION_ENABLE;


    mdns_config_eth->actions[1].event_base = ETH_EVENT;
    mdns_config_eth->actions[1].event_id = ETHERNET_EVENT_DISCONNECTED;
    mdns_config_eth->actions[1].action = ACTION_DISABLE;

    mdns_cfg_add_to_list(mdns_config_eth);

    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK( mdns_hostname_set(hostname) );
    ESP_LOGI(TAG, "mdns hostname set to: [%s]", hostname);
    //set default mDNS instance name
    ESP_ERROR_CHECK( mdns_instance_name_set(EXAMPLE_MDNS_INSTANCE) );

    //structure with TXT records
    mdns_txt_item_t serviceTxtData[3] = {
        {"board", "esp32"},
        {"u", "user"},
        {"p", "password"}
    };

    //initialize service
    ESP_ERROR_CHECK( mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData, 3) );

#if CONFIG_MDNS_PUBLISH_DELEGATE_HOST
    char *delegated_hostname;
    if (-1 == asprintf(&delegated_hostname, "%s-delegated", hostname)) {
        abort();
    }

    mdns_ip_addr_t addr4, addr6;
    esp_netif_str_to_ip4("10.0.0.1", &addr4.addr.u_addr.ip4);
    addr4.addr.type = ESP_IPADDR_TYPE_V4;
    esp_netif_str_to_ip6("fd11:22::1", &addr6.addr.u_addr.ip6);
    addr6.addr.type = ESP_IPADDR_TYPE_V6;
    addr4.next = &addr6;
    addr6.next = NULL;
    ESP_ERROR_CHECK( mdns_delegate_hostname_add(delegated_hostname, &addr4) );
    ESP_ERROR_CHECK( mdns_service_add_for_host("test0", "_http", "_tcp", delegated_hostname, 1234, serviceTxtData, 3) );
    free(delegated_hostname);
#endif // CONFIG_MDNS_PUBLISH_DELEGATE_HOST

    //add another TXT item
    ESP_ERROR_CHECK( mdns_service_txt_item_set("_http", "_tcp", "path", "/foobar") );
    //change TXT item value
    ESP_ERROR_CHECK( mdns_service_txt_item_set_with_explicit_value_len("_http", "_tcp", "u", "admin", strlen("admin")) );
    free(hostname);
}

/* these strings match tcpip_adapter_if_t enumeration */
// static const char * if_str[] = {"STA", "AP", "ETH", "MAX"};

/* these strings match mdns_ip_protocol_t enumeration */
// static const char * ip_protocol_str[] = {"V4", "V6", "MAX"};

static void mdns_print_results(mdns_result_t *results)
{
    mdns_result_t *r = results;
    mdns_ip_addr_t *a = NULL;
    //int i = 1;
    int t;
    while(r){
        //printf("%d: Interface: %s, Type: %s\n", i++, if_str[r->tcpip_if], ip_protocol_str[r->ip_protocol]);
        if(r->instance_name){
            printf("  PTR : %s\n", r->instance_name);
        }
        if (r->hostname) {
            printf("  SRV : %s.local:%u\n", r->hostname, r->port);
        }
        if (r->txt_count) {
            printf("  TXT : [%zu] ", r->txt_count);
            for (t = 0; t < r->txt_count; t++) {
                printf("%s=%s(%d); ", r->txt[t].key, r->txt[t].value ? r->txt[t].value : "NULL", r->txt_value_len[t]);
            }
            printf("\n");
        }
        a = r->addr;
        while (a) {
            if (a->addr.type == ESP_IPADDR_TYPE_V6) {
                printf("  AAAA: " IPV6STR "\n", IPV62STR(a->addr.u_addr.ip6));
            } else {
                printf("  A   : " IPSTR "\n", IP2STR(&(a->addr.u_addr.ip4)));
            }
            a = a->next;
        }
        r = r->next;
    }
}

static void query_mdns_service(const char * service_name, const char * proto)
{
    ESP_LOGI(TAG, "Query PTR: %s.%s.local", service_name, proto);

    mdns_result_t * results = NULL;
    esp_err_t err = mdns_query_ptr(service_name, proto, 3000, 20,  &results);
    if(err){
        ESP_LOGE(TAG, "Query Failed: %s", esp_err_to_name(err));
        return;
    }
    if(!results){
        ESP_LOGW(TAG, "No results found!");
        return;
    }

    mdns_print_results(results);
    mdns_query_results_free(results);
}

static bool check_and_print_result(mdns_search_once_t *search)
{
    // Check if any result is available
    mdns_result_t * result = NULL;
    if (!mdns_query_async_get_results(search, 0, &result)) {
        return false;
    }

    if (!result) {   // search timeout, but no result
        return true;
    }

    // If yes, print the result
    mdns_ip_addr_t * a = result->addr;
    while (a) {
        if(a->addr.type == ESP_IPADDR_TYPE_V6){
            printf("  AAAA: " IPV6STR "\n", IPV62STR(a->addr.u_addr.ip6));
        } else {
            printf("  A   : " IPSTR "\n", IP2STR(&(a->addr.u_addr.ip4)));
        }
        a = a->next;
    }
    // and free the result
    mdns_query_results_free(result);
    return true;
}

static void query_mdns_hosts_async(const char * host_name)
{
    ESP_LOGI(TAG, "Query both A and AAA: %s.local", host_name);

    mdns_search_once_t *s_a = mdns_query_async_new(host_name, NULL, NULL, MDNS_TYPE_A, 1000, 1, NULL);
    mdns_query_async_delete(s_a);
    mdns_search_once_t *s_aaaa = mdns_query_async_new(host_name, NULL, NULL, MDNS_TYPE_AAAA, 1000, 1, NULL);
    while (s_a || s_aaaa) {
        if (s_a && check_and_print_result(s_a)) {
            ESP_LOGI(TAG, "Query A %s.local finished", host_name);
            mdns_query_async_delete(s_a);
            s_a = NULL;
        }
        if (s_aaaa && check_and_print_result(s_aaaa)) {
            ESP_LOGI(TAG, "Query AAAA %s.local finished", host_name);
            mdns_query_async_delete(s_aaaa);
            s_aaaa = NULL;
        }
    }
}

static void query_mdns_host(const char * host_name)
{
    ESP_LOGI(TAG, "Query A: %s.local", host_name);

    struct esp_ip4_addr addr;
    addr.addr = 0;

    esp_err_t err = mdns_query_a(host_name, 2000,  &addr);
    if(err){
        if(err == ESP_ERR_NOT_FOUND){
            ESP_LOGW(TAG, "%s: Host was not found!", esp_err_to_name(err));
            return;
        }
        ESP_LOGE(TAG, "Query Failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Query A: %s.local resolved to: " IPSTR, host_name, IP2STR(&addr));
}

static void initialise_button(void)
{
    gpio_config_t io_conf = {0};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = BIT64(EXAMPLE_BUTTON_GPIO);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
}

static void check_button(void)
{
    static bool old_level = true;
    bool new_level = gpio_get_level(EXAMPLE_BUTTON_GPIO);
    if (!new_level && old_level) {
        query_mdns_hosts_async("esp32-mdns");
        query_mdns_host("esp32");
        query_mdns_service("_arduino", "_tcp");
        query_mdns_service("_http", "_tcp");
        query_mdns_service("_printer", "_tcp");
        query_mdns_service("_ipp", "_tcp");
        query_mdns_service("_afpovertcp", "_tcp");
        query_mdns_service("_smb", "_tcp");
        query_mdns_service("_ftp", "_tcp");
        query_mdns_service("_nfs", "_tcp");
    }
    old_level = new_level;
}

static void mdns_example_task(void *pvParameters)
{
#if CONFIG_MDNS_RESOLVE_TEST_SERVICES == 1
    /* Send initial queries that are started by CI tester */
    query_mdns_host("tinytester");
    query_mdns_host_with_gethostbyname("tinytester-lwip.local");
    query_mdns_host_with_getaddrinfo("tinytester-lwip.local");
#endif

    while (1) {
        check_button();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    initialise_mdns();

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    // ESP_ERROR_CHECK(example_connect()); // To-do, change this to explicitelly register first for wifi interface, then add new one

    initialise_button();
    xTaskCreate(&mdns_example_task, "mdns_example_task", 2048, NULL, 5, NULL);
}

/** Generate host name based on sdkconfig, optionally adding a portion of MAC address to it.
 *  @return host name string allocated from the heap
 */
static char* generate_hostname(void)
{
#ifndef CONFIG_MDNS_ADD_MAC_TO_HOSTNAME
    return strdup(CONFIG_MDNS_HOSTNAME);
#else
    uint8_t mac[6];
    char   *hostname;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if (-1 == asprintf(&hostname, "%s-%02X%02X%02X", CONFIG_MDNS_HOSTNAME, mac[3], mac[4], mac[5])) {
        abort();
    }
    return hostname;
#endif
}

#if CONFIG_MDNS_RESOLVE_TEST_SERVICES == 1
/**
 *  @brief Executes gethostbyname and displays list of resolved addresses.
 *  Note: This function is used only to test advertised mdns hostnames resolution
 */
static void  query_mdns_host_with_gethostbyname(char * host)
{
    struct hostent *res = gethostbyname(host);
    if (res) {
        unsigned int i = 0;
        while (res->h_addr_list[i] != NULL) {
            ESP_LOGI(TAG, "gethostbyname: %s resolved to: %s", host, inet_ntoa(*(struct in_addr *) (res->h_addr_list[i])));
            i++;
        }
    }
}

/**
 *  @brief Executes getaddrinfo and displays list of resolved addresses.
 *  Note: This function is used only to test advertised mdns hostnames resolution
 */
static void  query_mdns_host_with_getaddrinfo(char * host)
{
    struct addrinfo hints;
    struct addrinfo * res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (!getaddrinfo(host, NULL, &hints, &res)) {
        while (res) {
            ESP_LOGI(TAG, "getaddrinfo: %s resolved to: %s", host,
                     res->ai_family == AF_INET?
                     inet_ntoa(((struct sockaddr_in *) res->ai_addr)->sin_addr):
                     inet_ntoa(((struct sockaddr_in6 *) res->ai_addr)->sin6_addr));
            res = res->ai_next;
        }
    }
}
#endif
