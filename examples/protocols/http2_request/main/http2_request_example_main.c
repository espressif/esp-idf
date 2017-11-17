/* HTTP2 GET Example using nghttp2
 
   Contacts http2.golang.org and executes the GET/PUT requests. A thin API
   wrapper on top of nghttp2, to properly demonstrate the interactions.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

 */
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "apps/sntp/sntp.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "sh2lib.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

static const char *TAG = "http2-req";

/* The HTTP/2 server to connect to */
#define HTTP2_SERVER_URI  "https://http2.golang.org"
/* A GET request that keeps streaming current time every second */
#define HTTP2_STREAMING_GET_PATH  "/clockstream"
/* A PUT request that echoes whatever we had sent to it */
#define HTTP2_PUT_PATH            "/ECHO"

int handle_get_response(struct sh2lib_handle *handle, const char *data, size_t len, int flags)
{
    if (len) {
        printf("[get-response] %.*s\n", len, data);
    }
    if (flags == DATA_RECV_FRAME_COMPLETE) {
        printf("[get-response] Frame fully received\n");
    }
    if (flags == DATA_RECV_RST_STREAM) {
        printf("[get-response] Stream Closed\n");
    }
    return 0;
}

int handle_echo_response(struct sh2lib_handle *handle, const char *data, size_t len, int flags)
{
    if (len) {
        printf("[echo-response] %.*s\n", len, data);
    }
    if (flags == DATA_RECV_FRAME_COMPLETE) {
        printf("[echo-response] Frame fully received\n");
    }
    if (flags == DATA_RECV_RST_STREAM) {
        printf("[echo-response] Stream Closed\n");
    }
    return 0;
}

int send_put_data(struct sh2lib_handle *handle, char *buf, size_t length, uint32_t *data_flags)
{
#define DATA_TO_SEND "Hello World"
    int copylen = strlen(DATA_TO_SEND);
    if (copylen < length) {
        printf("[data-prvd] Sending %d bytes\n", copylen);
        memcpy(buf, DATA_TO_SEND, copylen);
    } else {
        copylen = 0;
    }

    (*data_flags) |= NGHTTP2_DATA_FLAG_EOF;
    return copylen;
}

static void set_time(void)
{
    struct timeval tv = {
        .tv_sec = 1509449941,
    };
    struct timezone tz = {
        0, 0
    };
    settimeofday(&tv, &tz);

    /* Start SNTP service */
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();
}

static void http2_task(void *args)
{
    /* Waiting for connection */
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    /* Set current time: proper system time is required for TLS based
     * certificate verification.
     */
    set_time();

    /* HTTP2: one connection multiple requests. Do the TLS/TCP connection first */
    printf("Connecting to server\n");
    struct sh2lib_handle hd;
    if (sh2lib_connect(&hd, HTTP2_SERVER_URI) != 0) {
        printf("Failed to connect\n");
        return;
    }
    printf("Connection done\n");

    /* HTTP GET  */
    sh2lib_do_get(&hd, HTTP2_STREAMING_GET_PATH, handle_get_response);

    /* HTTP PUT  */
    sh2lib_do_put(&hd, HTTP2_PUT_PATH, send_put_data, handle_echo_response);

    while (1) {
        /* Process HTTP2 send/receive */
        if (sh2lib_execute(&hd) < 0) {
            printf("Error in send/receive\n");
            break;
        }
        vTaskDelay(2);
    }

    sh2lib_free(&hd);
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
        ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
        ESP_LOGI(TAG, "got ip:%s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
        ESP_ERROR_CHECK(esp_wifi_connect());
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

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    initialise_wifi();

    xTaskCreate(&http2_task, "http2_task", (1024 * 32), NULL, 5, NULL);
}
