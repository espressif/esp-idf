/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "protocol_examples_common.h"
#include "mdns.h"

static const char *TAG = "MDNS_TEST";
void mdns_test(char *line);

static void get_string(char *line, size_t size)
{
    int count = 0;
    while (count < size) {
        int c = fgetc(stdin);
        if (c == '\n') {
            line[count] = '\0';
            break;
        } else if (c > 0 && c < 127) {
            line[count] = c;
            ++count;
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

/** Generate host name based on sdkconfig, optionally adding a portion of MAC address to it.
 *  @return host name string allocated from the heap
 */
static char* generate_hostname(void)
{
#ifndef CONFIG_TEST_MDNS_ADD_MAC_TO_HOSTNAME
    return strdup(CONFIG_TEST_MDNS_HOSTNAME);
#else
    uint8_t mac[6];
    char   *hostname;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if (-1 == asprintf(&hostname, "%s-%02X%02X%02X", CONFIG_TEST_MDNS_HOSTNAME, mac[3], mac[4], mac[5])) {
        abort();
    }
    return hostname;
#endif
}

static void initialise_mdns(void)
{
    char * hostname = generate_hostname();

    //initialize mDNS
    ESP_ERROR_CHECK( mdns_init() );

    //set mDNS hostname (required if you want to advertise services)
    ESP_ERROR_CHECK( mdns_hostname_set(hostname) );

    ESP_LOGI(TAG, "mdns hostname set to: [%s]", hostname);
    //set default mDNS instance name
    ESP_ERROR_CHECK( mdns_instance_name_set(CONFIG_TEST_MDNS_INSTANCE) );

    //initialize service
    ESP_ERROR_CHECK( mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, NULL, 0) );

#if CONFIG_TEST_MDNS_PUBLISH_DELEGATE_HOST
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
    ESP_ERROR_CHECK( mdns_service_add_for_host("test0", "_http", "_tcp", delegated_hostname, 1234, NULL, 0) );
    free(delegated_hostname);
#endif // CONFIG_TEST_MDNS_PUBLISH_DELEGATE_HOST

    ESP_ERROR_CHECK( mdns_service_subtype_add_for_host("ESP32-WebServer", "_http", "_tcp", NULL, "_server") );

    free(hostname);
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    initialise_mdns();

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    while (1) {
        char line[256];

        get_string(line, sizeof(line));
        mdns_test(line);
        continue;
    }
}
