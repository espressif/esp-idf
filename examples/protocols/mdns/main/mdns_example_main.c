/* MDNS-SD Query and advertise Example

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
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mdns.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

#define EXAMPLE_MDNS_HOSTNAME CONFIG_MDNS_HOSTNAME
#define EXAMPLE_MDNS_INSTANCE CONFIG_MDNS_INSTANCE

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

static const char *TAG = "mdns-test";

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        /* enable ipv6 */
        tcpip_adapter_create_ip6_linklocal(TCPIP_ADAPTER_IF_STA);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

static void query_mdns_service(mdns_server_t * mdns, const char * service, const char * proto)
{
    if(!mdns) {
        return;
    }
    uint32_t res;
    if (!proto) {
        ESP_LOGI(TAG, "Host Lookup: %s", service);
        res = mdns_query(mdns, service, 0, 1000);
        if (res) {
            size_t i;
            for(i=0; i<res; i++) {
                const mdns_result_t * r = mdns_result_get(mdns, i);
                if (r) {
                    ESP_LOGI(TAG, "  %u: " IPSTR " " IPV6STR, i+1, 
                        IP2STR(&r->addr), IPV62STR(r->addrv6));
                }
            }
            mdns_result_free(mdns);
        } else {
            ESP_LOGI(TAG, "  Not Found");
        }
    } else {
        ESP_LOGI(TAG, "Service Lookup: %s.%s ", service, proto);
        res = mdns_query(mdns, service, proto, 1000);
        if (res) {
            size_t i;
            for(i=0; i<res; i++) {
                const mdns_result_t * r = mdns_result_get(mdns, i);
                if (r) {
                    ESP_LOGI(TAG, "  %u: %s \"%s\" " IPSTR " " IPV6STR " %u %s", i+1, 
                        (r->host)?r->host:"", (r->instance)?r->instance:"", 
                        IP2STR(&r->addr), IPV62STR(r->addrv6),
                        r->port, (r->txt)?r->txt:"");
                }
            }
            mdns_result_free(mdns);
        }
    }
}

static void mdns_task(void *pvParameters)
{
    mdns_server_t * mdns = NULL;
    while(1) {
        /* Wait for the callback to set the CONNECTED_BIT in the
           event group.
        */
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "Connected to AP");

        if (!mdns) {
            esp_err_t err = mdns_init(TCPIP_ADAPTER_IF_STA, &mdns);
            if (err) {
                ESP_LOGE(TAG, "Failed starting MDNS: %u", err);
                continue;
            }

            ESP_ERROR_CHECK( mdns_set_hostname(mdns, EXAMPLE_MDNS_HOSTNAME) );
            ESP_ERROR_CHECK( mdns_set_instance(mdns, EXAMPLE_MDNS_INSTANCE) );

            const char * arduTxtData[4] = {
                "board=esp32",
                "tcp_check=no",
                "ssh_upload=no",
                "auth_upload=no"
            };

            ESP_ERROR_CHECK( mdns_service_add(mdns, "_arduino", "_tcp", 3232) );
            ESP_ERROR_CHECK( mdns_service_txt_set(mdns, "_arduino", "_tcp", 4, arduTxtData) );
            ESP_ERROR_CHECK( mdns_service_add(mdns, "_http", "_tcp", 80) );
            ESP_ERROR_CHECK( mdns_service_instance_set(mdns, "_http", "_tcp", "ESP32 WebServer") );
            ESP_ERROR_CHECK( mdns_service_add(mdns, "_smb", "_tcp", 445) );
        } else {
            query_mdns_service(mdns, "esp32", NULL);
            query_mdns_service(mdns, "_arduino", "_tcp");
            query_mdns_service(mdns, "_http", "_tcp");
            query_mdns_service(mdns, "_printer", "_tcp");
            query_mdns_service(mdns, "_ipp", "_tcp");
            query_mdns_service(mdns, "_afpovertcp", "_tcp");
            query_mdns_service(mdns, "_smb", "_tcp");
            query_mdns_service(mdns, "_ftp", "_tcp");
            query_mdns_service(mdns, "_nfs", "_tcp");
        }

        ESP_LOGI(TAG, "Restarting in 10 seconds!");
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Starting again!");
    }
}

void app_main()
{
    nvs_flash_init();
    initialise_wifi();
    xTaskCreate(&mdns_task, "mdns_task", 2048, NULL, 5, NULL);
}
