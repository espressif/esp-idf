/* CoAP client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"

#include "nvs_flash.h"

#include "coap.h"

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

#define COAP_DEFAULT_TIME_SEC 5
#define COAP_DEFAULT_TIME_USEC 0

/* The examples use uri "coap://californium.eclipse.org" that
   you can set via 'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define COAP_DEFAULT_DEMO_URI "coap://californium.eclipse.org"
*/
#define COAP_DEFAULT_DEMO_URI CONFIG_TARGET_DOMAIN_URI

static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const static int CONNECTED_BIT = BIT0;

const static char *TAG = "CoAP_client";

static void message_handler(struct coap_context_t *ctx, const coap_endpoint_t *local_interface, const coap_address_t *remote,
              coap_pdu_t *sent, coap_pdu_t *received,
                const coap_tid_t id)
{
    unsigned char* data = NULL;
    size_t data_len;
    if (COAP_RESPONSE_CLASS(received->hdr->code) == 2) {
        if (coap_get_data(received, &data_len, &data)) {
            printf("Received: %s\n", data);
        }
    }
}

static void coap_demo_thread(void *p)
{
    struct hostent *hp;
    struct ip4_addr *ip4_addr;

    coap_context_t*   ctx = NULL;
    coap_address_t    dst_addr, src_addr;
    static coap_uri_t uri;
    fd_set            readfds;
    struct timeval    tv;
    int flags, result;
    coap_pdu_t*       request = NULL;
    const char*       server_uri = COAP_DEFAULT_DEMO_URI;
    uint8_t     get_method = 1;

    while (1) {
        /* Wait for the callback to set the CONNECTED_BIT in the
           event group.
        */
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY);
        ESP_LOGI(TAG, "Connected to AP");

        if (coap_split_uri((const uint8_t *)server_uri, strlen(server_uri), &uri) == -1) {
            ESP_LOGE(TAG, "CoAP server uri error");
            break;
        }

        hp = gethostbyname((const char *)uri.host.s);

        if (hp == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        ip4_addr = (struct ip4_addr *)hp->h_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*ip4_addr));

        coap_address_init(&src_addr);
        src_addr.addr.sin.sin_family      = AF_INET;
        src_addr.addr.sin.sin_port        = htons(0);
        src_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;

        ctx = coap_new_context(&src_addr);
        if (ctx) {
            coap_address_init(&dst_addr);
            dst_addr.addr.sin.sin_family      = AF_INET;
            dst_addr.addr.sin.sin_port        = htons(COAP_DEFAULT_PORT);
            dst_addr.addr.sin.sin_addr.s_addr = ip4_addr->addr;

            request            = coap_new_pdu();
            if (request){
                request->hdr->type = COAP_MESSAGE_CON;
                request->hdr->id   = coap_new_message_id(ctx);
                request->hdr->code = get_method;
                coap_add_option(request, COAP_OPTION_URI_PATH, uri.path.length, uri.path.s);

                coap_register_response_handler(ctx, message_handler);
                coap_send_confirmed(ctx, ctx->endpoint, &dst_addr, request);

                flags = fcntl(ctx->sockfd, F_GETFL, 0);
                fcntl(ctx->sockfd, F_SETFL, flags|O_NONBLOCK);

                tv.tv_usec = COAP_DEFAULT_TIME_USEC;
                tv.tv_sec = COAP_DEFAULT_TIME_SEC;

                for(;;) {
                    FD_ZERO(&readfds);
                    FD_CLR( ctx->sockfd, &readfds );
                    FD_SET( ctx->sockfd, &readfds );
                    result = select( FD_SETSIZE, &readfds, 0, 0, &tv );
                    if (result > 0) {
                        if (FD_ISSET( ctx->sockfd, &readfds ))
                            coap_read(ctx);
                    } else if (result < 0) {
                        break;
                    } else {
                        ESP_LOGE(TAG, "select timeout");
                    }
                }
            }
            coap_free_context(ctx);
        }
    }

    vTaskDelete(NULL);
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
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

static void wifi_conn_init(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(wifi_event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    wifi_conn_init();
    xTaskCreate(coap_demo_thread, "coap", 2048, NULL, 5, NULL);
}
